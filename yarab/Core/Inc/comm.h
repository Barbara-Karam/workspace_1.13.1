#ifndef COMM_H
#define COMM_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

// ─── Frame Format ─────────────────────────────────────────────────────────────
// [0xAA][0x55][TYPE:1][LEN:1][PAYLOAD:N][CRC8:1]
#define COMM_SYNC1          0xAA
#define COMM_SYNC2          0x55
#define COMM_RX_BUF_SIZE    64
#define COMM_TX_BUF_SIZE    64

// ─── Message Types ────────────────────────────────────────────────────────────
typedef enum {
    MSG_CONTROL_CMD  = 0x01,   // Host → STM32: velocity targets
    MSG_ACTUATOR_FB  = 0x02,   // STM32 → Host: sensor feedback
    MSG_HEARTBEAT    = 0x03,   // Bidirectional: keep-alive
    MSG_ESTOP        = 0x04,   // Host → STM32: emergency stop
} MsgType_t;

// ─── Payload Structs ──────────────────────────────────────────────────────────

// Host → STM32 (sent at ~50Hz from ROS)
typedef struct __attribute__((packed)) {
    int16_t  vel_rpm10[4];   // Target RPM × 10 per motor
                              // e.g. 1200 = 120.0 RPM
    uint8_t  mode;            // 0=velocity PID, 1=open loop PWM
} ControlCmd_t;

// STM32 → Host (sent at 100Hz)
typedef struct __attribute__((packed)) {
    int16_t  vel_rpm10[4];     // Measured RPM × 10
    int32_t  enc_count[4];     // Absolute encoder counts
    int16_t  pwm_out[4];       // Applied PWM value
    uint32_t timestamp_ms;     // HAL_GetTick()
    uint8_t  sys_status;       // 0=OK, 1=WARN, 2=FAULT
} ActuatorFB_t;

// ─── API ─────────────────────────────────────────────────────────────────────
void Comm_Init(void);
void Comm_ProcessRx(void);       // Call from main loop
void Comm_TransmitFeedback(void); // Call from main loop when flag set

// Internal callback — do not call directly
void Comm_RxEventCallback(uint16_t size);

#endif
