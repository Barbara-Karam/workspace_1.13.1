/**
 * @file    comm.c
 * @brief   Binary framed UART protocol (Host <-> STM32 Chaser LLC)
 *          Frame format: [0xAA][0x55][TYPE][LEN][PAYLOAD...][CRC8]
 */
#include "comm.h"
#include "usart.h"
#include "control_loop.h"
#include "encoder.h"
#include "motor_control.h"
#include <string.h>

/* ── Buffer sizes ─────────────────────────────────────────────────────────── */
#define COMM_MAX_PAYLOAD    37u     /* sizeof(ActuatorFB_t) — largest payload */

/* ── Internal buffers ────────────────────────────────────────────────────── */
static uint8_t rx_buf[COMM_RX_BUF_SIZE];
static uint8_t tx_buf[COMM_TX_BUF_SIZE];
static uint8_t hb_buf[8];          /* Dedicated heartbeat buffer —
                                       prevents DMA corruption if feedback
                                       TX is still in flight               */

/* ── TX busy guard ───────────────────────────────────────────────────────── */
static volatile uint8_t s_tx_busy = 0;

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

/* ── Non-blocking DMA TX with HAL return check ───────────────────────────── */
/* Fix #1: check HAL_UART_Transmit_DMA return value so a failed DMA start
   doesn't permanently lock s_tx_busy.                                       */
static void uart_tx(uint8_t *buf, uint8_t len)
{
    if (s_tx_busy) return;

    s_tx_busy = 1;

    if (HAL_UART_Transmit_DMA(&huart2, buf, len) != HAL_OK)
    {
        /* DMA start failed — release lock immediately so next call can retry */
        s_tx_busy = 0;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        s_tx_busy = 0;
    }
}

/* ── Init ────────────────────────────────────────────────────────────────── */
void Comm_Init(void)
{
    /* Arm DMA idle-line RX — fires Comm_RxEventCallback on each idle event */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
}

/* ── Parse received frame (operates on local copy) ───────────────────────── */
static void Comm_ParseFrame(uint8_t *buf, uint16_t size)
{
    /* Minimum frame: SYNC1+SYNC2+TYPE+LEN+CRC = 5 bytes */
    if (size < 5) return;

    /* Validate sync bytes */
    if (buf[0] != COMM_SYNC1 || buf[1] != COMM_SYNC2) return;

    uint8_t msg_type    = buf[2];
    uint8_t payload_len = buf[3];

    /* Fix #10: reject oversized payload before any buffer indexing */
    if (payload_len > COMM_MAX_PAYLOAD) return;

    /* Validate total frame length matches what DMA received */
    if ((uint16_t)(5u + payload_len) != size) return;

    /* Validate CRC */
    uint8_t crc_rx   = buf[4 + payload_len];
    uint8_t crc_calc = crc8(&buf[2], 2 + payload_len);
    if (crc_rx != crc_calc) return;

    uint8_t *payload = &buf[4];

    switch (msg_type)
    {
        case MSG_CONTROL_CMD: {
            if (payload_len != sizeof(ControlCmd_t)) return;
            ControlCmd_t cmd;
            memcpy(&cmd, payload, sizeof(ControlCmd_t));
            for (uint8_t i = 0; i < 4; i++) {
                float rpm = (float)cmd.vel_rpm10[i] / 10.0f;
                ControlLoop_SetTarget(i, rpm);
            }
            break;
        }

        case MSG_HEARTBEAT: {
            /* Reply using dedicated buffer — tx_buf may be DMA-owned */
            hb_buf[0] = COMM_SYNC1;
            hb_buf[1] = COMM_SYNC2;
            hb_buf[2] = MSG_HEARTBEAT;
            hb_buf[3] = 0;
            hb_buf[4] = crc8(&hb_buf[2], 2);
            uart_tx(hb_buf, 5);
            break;
        }

        case MSG_ESTOP:
            Motor_EmergencyStop();
            ControlLoop_Disable();
            break;

        default:
            break;
    }
}

/* ── DMA Idle-line callback ──────────────────────────────────────────────── */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance != USART2) return;

    /* Fix #2: clamp size before memcpy to prevent overrun */
    if (size > COMM_RX_BUF_SIZE) size = COMM_RX_BUF_SIZE;

    /* Copy to local buffer before re-arming DMA — fixes race condition where
       circular DMA overwrites rx_buf while we're still parsing it           */
    static uint8_t local_buf[COMM_RX_BUF_SIZE];
    memcpy(local_buf, rx_buf, size);

    /* Re-arm immediately after copy so we don't miss the next frame */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf, COMM_RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

    Comm_ParseFrame(local_buf, size);
}

/* ── Main-loop RX hook (intentionally empty — all parsing in callback) ───── */
void Comm_ProcessRx(void)
{
    /* Intentionally empty — RX handled in UART idle callback */
}

/* ── Transmit feedback frame ─────────────────────────────────────────────── */
/* Fix #1 applied here too: uart_tx guards against busy DMA and checks
   HAL return, so a dropped frame is silent but the system stays alive.      */
void Comm_TransmitFeedback(void)
{
    if (!g_ctrl.feedback_ready) return;

    g_ctrl.feedback_ready = 0;

    ActuatorFB_t fb;
    for (uint8_t i = 0; i < 4; i++) {
        fb.vel_rpm10[i] = (int16_t)(g_ctrl.measured_rpm[i] * 10.0f);
        fb.enc_count[i] = Encoder_GetCount(i);
        fb.pwm_out[i]   = (int16_t)g_ctrl.output_pwm[i];
    }
    fb.timestamp_ms = HAL_GetTick();
    fb.sys_status   = 0;

    /* Fix #10: validate payload fits before building frame */
    uint8_t payload_len = sizeof(ActuatorFB_t);
    if (payload_len > COMM_MAX_PAYLOAD) return;

    tx_buf[0] = COMM_SYNC1;
    tx_buf[1] = COMM_SYNC2;
    tx_buf[2] = MSG_ACTUATOR_FB;
    tx_buf[3] = payload_len;
    memcpy(&tx_buf[4], &fb, payload_len);
    tx_buf[4 + payload_len] = crc8(&tx_buf[2], 2 + payload_len);

    uart_tx(tx_buf, (uint8_t)(5 + payload_len));
}

/* ── UART error recovery ─────────────────────────────────────────────────── */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        HAL_UART_AbortReceive(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buf, COMM_RX_BUF_SIZE);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}
