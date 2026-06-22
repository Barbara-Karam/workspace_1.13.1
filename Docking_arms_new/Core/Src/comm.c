/**
  ******************************************************************************
  * @file    comm.c
  * @brief   Binary framed UART protocol (Jetson <-> STM32)
  *          Uses HAL_UARTEx_ReceiveToIdle_DMA so we get an event on the
  *          idle line after each complete frame, exactly like the chaser LLC.
  *          RX parsing, TX feedback, heartbeat, and e-stop handled here.
  ******************************************************************************
  */
#include "comm.h"
#include "usart.h"
#include "motor_control.h"
#include "limit_switch.h"
#include "imu.h"
#include <string.h>

/* ── Internal buffers ────────────────────────────────────────────────────── */
static uint8_t s_rx_buf[COMM_RX_BUF_SIZE];
static uint8_t s_tx_buf[COMM_TX_BUF_SIZE];

/* ── Shared state ────────────────────────────────────────────────────────── */
volatile int16_t g_target_pwm[2]  = {0, 0};
volatile uint8_t g_estop_active   = 0;

/* Latest IMU snapshot filled in main loop */
IMU_Data_t g_imu_data = {0};

static volatile uint8_t s_hb_pending = 0;

/* ── CRC-8 (polynomial 0x07) ─────────────────────────────────────────────── */
static uint8_t crc8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0x00;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1);
        }
    }
    return crc;
}

/* ── Frame builder ───────────────────────────────────────────────────────── */
static uint8_t build_frame(uint8_t *buf, uint8_t msg_type,
                           const void *payload, uint8_t payload_len)
{
    buf[0] = COMM_SYNC1;
    buf[1] = COMM_SYNC2;
    buf[2] = COMM_SYNC3;
    buf[3] = msg_type;
    buf[4] = payload_len;
    if (payload && payload_len > 0) {
        memcpy(&buf[5], payload, payload_len);
    }
    buf[5 + payload_len] = crc8(&buf[3], 2 + payload_len);
    return (uint8_t)(6 + payload_len);
}

static void uart_tx(uint8_t *buf, uint8_t len)
{
    HAL_UART_Transmit(&huart2, buf, len, 10);
}

/* ────────────────────────────────────────────────────────────────────────── */
void Comm_Init(void)
{
    huart2.gState = HAL_UART_STATE_READY;
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT); /* Suppress half-transfer IRQ */
}

/* ── DMA Idle-line callback (fires on each complete frame) ───────────────── */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance != USART2) return;
    Comm_RxEventCallback(size);
    /* Re-arm */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

/* ── Parse received frame ───────────────────────────────────────────────── */
void Comm_RxEventCallback(uint16_t size)
{
    if (size < 6) return;

    /* Sync check */
    if (s_rx_buf[0] != COMM_SYNC1 ||
        s_rx_buf[1] != COMM_SYNC2 ||
        s_rx_buf[2] != COMM_SYNC3) return;

    uint8_t msg_type    = s_rx_buf[3];
    uint8_t payload_len = s_rx_buf[4];

    if ((uint16_t)(6 + payload_len) != size) return;

    /* CRC check */
    uint8_t crc_rx   = s_rx_buf[5 + payload_len];
    uint8_t crc_calc = crc8(&s_rx_buf[3], 2 + payload_len);
    if (crc_rx != crc_calc) return;

    uint8_t *payload = &s_rx_buf[5];

    switch (msg_type)
    {
        case MSG_CONTROL_CMD: {
            if (payload_len != sizeof(ControlCmd_t)) return;
            if (g_estop_active) return;   /* Ignore commands during e-stop */
            ControlCmd_t cmd;
            memcpy(&cmd, payload, sizeof(ControlCmd_t));
            g_target_pwm[0] = cmd.pwm[0];
            g_target_pwm[1] = cmd.pwm[1];
            break;
        }

        case MSG_HEARTBEAT:
            s_hb_pending = 1;
            break;

        case MSG_ESTOP:
            g_estop_active = 1;
            g_target_pwm[0] = 0;
            g_target_pwm[1] = 0;
            Motor_EmergencyStop();
            break;

        default:
            break;
    }
}

/* ── Main-loop RX processing (heartbeat reply, etc.) ────────────────────── */
void Comm_ProcessRx(void)
{
    if (s_hb_pending) {
        s_hb_pending = 0;
        uint8_t len = build_frame(s_tx_buf, MSG_HEARTBEAT, NULL, 0);
        uart_tx(s_tx_buf, len);
    }
}

/* ── Transmit full telemetry frame ──────────────────────────────────────── */
/* Unconditional — called every 20ms tick regardless of motor state.        */
/* IMU data and switch states are always valid by the time this is called.  */
void Comm_TransmitFeedback(void)
{
    ActuatorFB_t fb = {0};

    /* Applied PWM values */
    for (uint8_t i = 0; i < 2; i++) {
        MotorState_t ms = Motor_GetState(i);
        fb.pwm_out[i] = ms.enabled
            ? (int16_t)((ms.direction == MOTOR_DIR_FORWARD)
                        ?  (int16_t)ms.pwm_duty
                        : -(int16_t)ms.pwm_duty)
            : 0;
    }

    /* Limit switch bitmask */
    fb.limit_switches = LimitSwitch_ReadAll();

    /* IMU data (scaled to integer for compact frame) */
    fb.imu_valid    = g_imu_data.valid;
    fb.accel_x_mg   = (int16_t)(g_imu_data.accel_x * 1000.0f);
    fb.accel_y_mg   = (int16_t)(g_imu_data.accel_y * 1000.0f);
    fb.accel_z_mg   = (int16_t)(g_imu_data.accel_z * 1000.0f);
    fb.gyro_x_cdps  = (int16_t)(g_imu_data.gyro_x * 100.0f);
    fb.gyro_y_cdps  = (int16_t)(g_imu_data.gyro_y * 100.0f);
    fb.gyro_z_cdps  = (int16_t)(g_imu_data.gyro_z * 100.0f);

    /* System status flags */
    fb.sys_status = SYS_STATUS_OK;
    if (!g_imu_data.valid)              fb.sys_status |= SYS_STATUS_IMU_FAULT;
    if (g_estop_active)                 fb.sys_status |= SYS_STATUS_ESTOP;
    if (fb.limit_switches)              fb.sys_status |= SYS_STATUS_LIMIT_HIT;

    fb.timestamp_ms = HAL_GetTick();

    uint8_t len = build_frame(s_tx_buf, MSG_ACTUATOR_FB, &fb, sizeof(ActuatorFB_t));
    uart_tx(s_tx_buf, len);
}
