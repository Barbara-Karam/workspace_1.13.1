/**
  ******************************************************************************
  * @file    imu.h
  * @brief   IMU driver header - supports MPU6050 and ICM-42688-P via I2C1
  *          Change IMU_TYPE below to match your hardware.
  ******************************************************************************
  */
#ifndef __IMU_H__
#define __IMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "i2c.h"
#include <stdint.h>

/* ── Select Your IMU ─────────────────────────────────────────────────────── */
/* Uncomment ONE of the following: */
#define IMU_TYPE_MPU6050
// #define IMU_TYPE_ICM42688

/* ── MPU6050 Settings ────────────────────────────────────────────────────── */
#ifdef IMU_TYPE_MPU6050
  #define IMU_I2C_ADDR        (0x68 << 1)   /* AD0 low; use 0x69<<1 if AD0 high */
  #define IMU_REG_PWR_MGMT_1  0x6B
  #define IMU_REG_SMPLRT_DIV  0x19
  #define IMU_REG_CONFIG      0x1A
  #define IMU_REG_GYRO_CONFIG 0x1B
  #define IMU_REG_ACCEL_CONFIG 0x1C
  #define IMU_REG_ACCEL_XOUT  0x3B          /* 6 bytes: AX AY AZ */
  #define IMU_REG_GYRO_XOUT   0x43          /* 6 bytes: GX GY GZ */
  #define IMU_REG_WHO_AM_I    0x75
  #define IMU_WHO_AM_I_VAL    0x68

  /* Scale factors at default ±2g / ±250°/s */
  #define IMU_ACCEL_SCALE     (1.0f / 16384.0f)   /* g per LSB */
  #define IMU_GYRO_SCALE      (1.0f / 131.0f)     /* °/s per LSB */
#endif

/* ── ICM-42688-P Settings ────────────────────────────────────────────────── */
#ifdef IMU_TYPE_ICM42688
  #define IMU_I2C_ADDR        (0x68 << 1)
  #define IMU_REG_WHO_AM_I    0x75
  #define IMU_WHO_AM_I_VAL    0x47
  #define IMU_REG_PWR_MGMT0   0x4E
  #define IMU_REG_ACCEL_XOUT  0x1F          /* 6 bytes */
  #define IMU_REG_GYRO_XOUT   0x25          /* 6 bytes */
  /* Scale factors at default ±2g / ±250°/s */
  #define IMU_ACCEL_SCALE     (1.0f / 16384.0f)
  #define IMU_GYRO_SCALE      (1.0f / 131.0f)
#endif

/* ── IMU Data Structure ──────────────────────────────────────────────────── */
typedef struct {
    float accel_x;   /* g  */
    float accel_y;   /* g  */
    float accel_z;   /* g  */
    float gyro_x;    /* °/s */
    float gyro_y;    /* °/s */
    float gyro_z;    /* °/s */
    uint8_t valid;   /* 1 = last read succeeded */
} IMU_Data_t;

/* ── Public API ──────────────────────────────────────────────────────────── */
HAL_StatusTypeDef IMU_Init(void);
HAL_StatusTypeDef IMU_Read(IMU_Data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H__ */
