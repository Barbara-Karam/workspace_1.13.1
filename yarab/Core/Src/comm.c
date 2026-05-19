#include "comm.h"
#include "usart.h"
#include "control_loop.h"
#include "encoder.h"
#include "motor_control.h"

// ─── Buffers ──────────────────────────────────────────────────────────────────
static uint8_t rx_buf[COMM_RX_BUF_SIZE];
static uint8_t tx_buf[COMM_TX_BUF_SIZE];

// ─── Private: CRC8 ───────────────────────────────────────────────────────────
static uint8_t crc8(uint8_t *data, uint16_t len)
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

// ─── Init ────────────────────────────────────────────────────────────────────
void Comm_Init(void)
{
    // Start UART RX with idle line detection
    // Fires Comm_RxEventCallback when idle or buffer full
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf, COMM_RX_BUF_SIZE);

    // Disable half-transfer interrupt — we only want idle/complete
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

// ─── RX Event Callback ───────────────────────────────────────────────────────
// Called from HAL in stm32f4xx_it.c USART2 IRQ handler
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance != USART2) return;
    Comm_RxEventCallback(size);

    // Restart DMA RX
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

// ─── Parse Incoming Frame ────────────────────────────────────────────────────
void Comm_RxEventCallback(uint16_t size)
{
    // Minimum frame: SYNC1+SYNC2+TYPE+LEN+CRC = 5 bytes
    if (size < 5) return;

    // Validate sync bytes
    if (rx_buf[0] != COMM_SYNC1 || rx_buf[1] != COMM_SYNC2) return;

    uint8_t  msg_type    = rx_buf[2];
    uint8_t  payload_len = rx_buf[3];

    // Validate total frame length
    if ((uint16_t)(4 + payload_len + 1) != size) return;

    // Validate CRC
    uint8_t crc_received = rx_buf[4 + payload_len];
    uint8_t crc_calc     = crc8(&rx_buf[2], 2 + payload_len);
    if (crc_received != crc_calc) return;

    uint8_t *payload = &rx_buf[4];

    switch (msg_type) {

        case MSG_CONTROL_CMD: {
            if (payload_len != sizeof(ControlCmd_t)) return;
            ControlCmd_t cmd;
            // Safe byte copy — payload may not be aligned
            for (uint8_t i = 0; i < sizeof(ControlCmd_t); i++) {
                ((uint8_t*)&cmd)[i] = payload[i];
            }
            // Convert RPM×10 to float RPM
            for (uint8_t i = 0; i < 4; i++) {
                float rpm = (float)cmd.vel_rpm10[i] / 10.0f;
                ControlLoop_SetTarget(i, rpm);
            }
            break;
        }

        case MSG_HEARTBEAT:
            // Echo heartbeat back
            tx_buf[0] = COMM_SYNC1;
            tx_buf[1] = COMM_SYNC2;
            tx_buf[2] = MSG_HEARTBEAT;
            tx_buf[3] = 0;  // No payload
            tx_buf[4] = crc8(&tx_buf[2], 2);
            HAL_UART_Transmit_DMA(&huart2, tx_buf, 5);
            break;

        case MSG_ESTOP:
            Motor_EmergencyStop();
            ControlLoop_Disable();
            break;

        default:
            break;
    }
}

// ─── Process RX — Call From Main Loop ────────────────────────────────────────
// Nothing needed here — all parsing happens in callback
// Keep this function for future non-RT processing
void Comm_ProcessRx(void)
{
    // Intentionally empty — RX handled in UART idle callback
}

// ─── Transmit Feedback — Call From Main Loop ─────────────────────────────────
void Comm_TransmitFeedback(void)
{
    if (!g_ctrl.feedback_ready) return;

    // Check DMA not busy
    if (HAL_UART_GetState(&huart2) & HAL_UART_STATE_BUSY_TX) return;

    g_ctrl.feedback_ready = 0;

    // Build feedback payload
    ActuatorFB_t fb;
    for (uint8_t i = 0; i < 4; i++) {
        fb.vel_rpm10[i] = (int16_t)(g_ctrl.measured_rpm[i] * 10.0f);
        fb.enc_count[i] = Encoder_GetCount(i);
        fb.pwm_out[i]   = (int16_t)g_ctrl.output_pwm[i];
    }
    fb.timestamp_ms = HAL_GetTick();
    fb.sys_status   = 0;  // OK — expand with FDIR later

    // Build frame
    uint8_t payload_len = sizeof(ActuatorFB_t);
    tx_buf[0] = COMM_SYNC1;
    tx_buf[1] = COMM_SYNC2;
    tx_buf[2] = MSG_ACTUATOR_FB;
    tx_buf[3] = payload_len;

    uint8_t *p = &tx_buf[4];
    for (uint8_t i = 0; i < payload_len; i++) {
        p[i] = ((uint8_t*)&fb)[i];
    }

    tx_buf[4 + payload_len] = crc8(&tx_buf[2], 2 + payload_len);

    HAL_UART_Transmit_DMA(&huart2, tx_buf,
                           5 + payload_len);
}
