/**
  ******************************************************************************
  * @file    comm.h
  * @brief   Binary framed UART protocol - Jetson Orin Nano <-> STM32
  *          Frame format: [0xAA][0x55][0xFF][MSG_TYPE][LEN][PAYLOAD...][CRC8]
  *
  *  Jetson -> STM32:
  *    MSG_CONTROL_CMD : set motor PWM targets
  *    MSG_HEARTBEAT   : keepalive ping
  *    MSG_ESTOP       : emergency stop
  *
  *  STM32 -> Jetson:
  *    MSG_ACTUATOR_FB : motor states + limit switch states + IMU data
  *    MSG_HEARTBEAT   : keepalive pong
  ******************************************************************************
  */
#ifndef __COMM_H__
#define __COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ── Frame sync bytes ────────────────────────────────────────────────────── */
#define COMM_SYNC1  0xAA
#define COMM_SYNC2  0x55
#define COMM_SYNC3  0xFF

/* ── Message types ───────────────────────────────────────────────────────── */
#define MSG_CONTROL_CMD   0x01
#define MSG_ACTUATOR_FB   0x02
#define MSG_HEARTBEAT     0x10
#define MSG_ESTOP         0xFF

/* ── Buffer sizes ────────────────────────────────────────────────────────── */
#define COMM_RX_BUF_SIZE  64u
#define COMM_TX_BUF_SIZE  64u
#define COMM_MAX_PAYLOAD  24u


/* ────────────────────────────────────────────────────────────────────────── */
/* Jetson -> STM32: Set raw PWM for 2 motors                                */
/* pwm_signed: -4199 to +4199                                               */
/* ────────────────────────────────────────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    int16_t pwm[2];    /* Motor 0, Motor 1 */
} ControlCmd_t;

/* ────────────────────────────────────────────────────────────────────────── */
/* STM32 -> Jetson: Full telemetry feedback                                 */
/* ────────────────────────────────────────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    int16_t  pwm_out[2];        /* Applied PWM per motor          */
    uint8_t  limit_switches;    /* Bitmask: bit0=SW0 ... bit3=SW3 */
    int16_t  accel_x_mg;        /* Accel X in milli-g             */
    int16_t  accel_y_mg;        /* Accel Y in milli-g             */
    int16_t  accel_z_mg;        /* Accel Z in milli-g             */
    int16_t  gyro_x_cdps;       /* Gyro X in centi-deg/s          */
    int16_t  gyro_y_cdps;       /* Gyro Y in centi-deg/s          */
    int16_t  gyro_z_cdps;       /* Gyro Z in centi-deg/s          */
    uint8_t  imu_valid;         /* 1 = IMU data is fresh          */
    uint32_t timestamp_ms;      /* HAL_GetTick()                  */
    uint8_t  sys_status;        /* 0x00 = OK, bit flags for faults*/
} ActuatorFB_t;

/* ── Status flag bits in sys_status ─────────────────────────────────────── */
#define SYS_STATUS_OK           0x00
#define SYS_STATUS_IMU_FAULT    (1 << 0)
#define SYS_STATUS_ESTOP        (1 << 1)
#define SYS_STATUS_LIMIT_HIT    (1 << 2)

/* ── Public API ──────────────────────────────────────────────────────────── */
void Comm_Init(void);
void Comm_RxEventCallback(uint16_t size);
void Comm_ProcessRx(void);
void Comm_TransmitFeedback(void);

#ifdef __cplusplus
}
#endif

#endif /* __COMM_H__ */
