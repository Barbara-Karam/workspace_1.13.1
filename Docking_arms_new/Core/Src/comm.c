/**
 * @file    comm.c
 * @brief   Binary framed UART protocol (Jetson <-> STM32)
 */
#include "comm.h"
#include "usart.h"
#include "motor_control.h"
#include "limit_switch.h"
#include "imu.h"
#include <string.h>
#include <stdio.h>

/* ── Internal buffers ────────────────────────────────────────────────────── */
static uint8_t s_rx_buf[COMM_RX_BUF_SIZE];
static uint8_t s_tx_buf[COMM_TX_BUF_SIZE];
static uint8_t s_hb_buf[8]; /* Dedicated heartbeat buffer — prevents DMA corruption */

/* ── Shared state ────────────────────────────────────────────────────────── */
volatile int16_t g_target_pwm[2]  = {0, 0};
volatile uint8_t g_estop_active   = 0;

IMU_Data_t g_imu_data = {0};
static volatile uint8_t s_hb_pending = 0;
static volatile uint8_t s_tx_busy    = 0;

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
static uint8_t build_frame(uint8_t *buf, uint8_t msg_type, const void *payload, uint8_t payload_len)
{
    /* Fix #10: guard against payload exceeding protocol maximum */
    if (payload_len > COMM_MAX_PAYLOAD) return 0;

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

/* ── Fix #1: check HAL_UART_Transmit_DMA return value ───────────────────── */
static void uart_tx(uint8_t *buf, uint8_t len)
{
    if (s_tx_busy) return;

    s_tx_busy = 1;

    if (HAL_UART_Transmit_DMA(&huart2, buf, len) != HAL_OK)
    {
        /* DMA start failed — release the lock so the next call can retry */
        s_tx_busy = 0;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        s_tx_busy = 0;
    }
}

/* ────────────────────────────────────────────────────────────────────────── */
void Comm_Init(void)
{
    huart2.gState = HAL_UART_STATE_READY;
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

/* ── Parse received frame ───────────────────────────────────────────────── */
static void Comm_ParseFrame(uint8_t *buffer, uint16_t size)
{
    if (size < 6) return;

    int16_t start_idx = -1;
    for (uint16_t i = 0; i <= size - 6; i++) {
        if (buffer[i] == COMM_SYNC1 && buffer[i+1] == COMM_SYNC2 && buffer[i+2] == COMM_SYNC3) {
            start_idx = i;
            break;
        }
    }

    if (start_idx == -1) return;

    uint8_t msg_type    = buffer[start_idx + 3];
    uint8_t payload_len = buffer[start_idx + 4];

    /* Fix #10: reject frames claiming an oversized payload before any indexing */
    if (payload_len > COMM_MAX_PAYLOAD) return;

    if (start_idx + 6 + payload_len > size) return;

    uint8_t crc_rx   = buffer[start_idx + 5 + payload_len];
    uint8_t crc_calc = crc8(&buffer[start_idx + 3], 2 + payload_len);

    if (crc_rx != crc_calc) return;

    uint8_t *payload = &buffer[start_idx + 5];

    switch (msg_type)
    {
        case MSG_CONTROL_CMD: {
            if (payload_len != sizeof(ControlCmd_t)) return;
            if (g_estop_active) return;
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

/* ── DMA Idle-line callback (fires on each complete frame) ───────────────── */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance != USART2) return;

    /* Fix #2: clamp size before memcpy to prevent overrun */
    if (size > COMM_RX_BUF_SIZE) size = COMM_RX_BUF_SIZE;

    static uint8_t local_buf[COMM_RX_BUF_SIZE];
    memcpy(local_buf, s_rx_buf, size);

    /* Re-arm immediately after copy so we don't miss the next frame */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

    Comm_ParseFrame(local_buf, size);
}

/* ── Main-loop RX processing (heartbeat reply, etc.) ────────────────────── */
void Comm_ProcessRx(void)
{
    if (s_hb_pending) {
        s_hb_pending = 0;
        uint8_t len = build_frame(s_hb_buf, MSG_HEARTBEAT, NULL, 0);
        if (len > 0) uart_tx(s_hb_buf, len);
    }
}

/* ── Transmit full telemetry frame ──────────────────────────────────────── */
void Comm_TransmitFeedback(void)
{
    /* FIX #1: Prevent buffer overwrite if DMA is actively transmitting */
    if (s_tx_busy) return;

    ActuatorFB_t fb = {0};

    for (uint8_t i = 0; i < 2; i++) {
        MotorState_t ms = Motor_GetState(i);
        fb.pwm_out[i] = ms.enabled
            ? (int16_t)((ms.direction == MOTOR_DIR_FORWARD) ? (int16_t)ms.pwm_duty : -(int16_t)ms.pwm_duty)
            : 0;
    }

    fb.limit_switches = LimitSwitch_ReadAll();

    fb.imu_valid    = g_imu_data.valid;
    fb.accel_x_mg   = (int16_t)(g_imu_data.accel_x * 1000.0f);
    fb.accel_y_mg   = (int16_t)(g_imu_data.accel_y * 1000.0f);
    fb.accel_z_mg   = (int16_t)(g_imu_data.accel_z * 1000.0f);
    fb.gyro_x_cdps  = (int16_t)(g_imu_data.gyro_x * 100.0f);
    fb.gyro_y_cdps  = (int16_t)(g_imu_data.gyro_y * 100.0f);
    fb.gyro_z_cdps  = (int16_t)(g_imu_data.gyro_z * 100.0f);

    fb.sys_status = SYS_STATUS_OK;
    if (!g_imu_data.valid)   fb.sys_status |= SYS_STATUS_IMU_FAULT;
    if (g_estop_active)      fb.sys_status |= SYS_STATUS_ESTOP;
    if (fb.limit_switches)   fb.sys_status |= SYS_STATUS_LIMIT_HIT;
    fb.timestamp_ms = HAL_GetTick();

    uint8_t len = build_frame(s_tx_buf, MSG_ACTUATOR_FB, &fb, sizeof(ActuatorFB_t));
    if (len > 0) uart_tx(s_tx_buf, len);
}

/* ── UART Error Recovery ─────────────────────────────────────────────────── */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        HAL_UART_AbortReceive(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_rx_buf, COMM_RX_BUF_SIZE);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}
