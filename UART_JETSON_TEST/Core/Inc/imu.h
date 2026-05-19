#ifndef __IMU_H
#define __IMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* ---------------------------------------------------------------
 * MPU6050 I2C address
 * AD0 pin → GND = 0x68   (default, leave AD0 unconnected or to GND)
 * AD0 pin → VCC = 0x69
 * --------------------------------------------------------------- */
#define MPU6050_ADDR          (0x68 << 1)   /* HAL uses 8-bit address */

/* MPU6050 Register map (only what we need) */
#define MPU6050_REG_SMPLRT_DIV    0x19
#define MPU6050_REG_CONFIG        0x1A
#define MPU6050_REG_GYRO_CONFIG   0x1B
#define MPU6050_REG_ACCEL_CONFIG  0x1C
#define MPU6050_REG_ACCEL_XOUT_H  0x3B   /* 6 bytes: AX_H AX_L AY_H AY_L AZ_H AZ_L */
#define MPU6050_REG_TEMP_OUT_H    0x41   /* 2 bytes */
#define MPU6050_REG_GYRO_XOUT_H   0x43   /* 6 bytes: GX_H GX_L GY_H GY_L GZ_H GZ_L */
#define MPU6050_REG_PWR_MGMT_1    0x6B
#define MPU6050_REG_WHO_AM_I      0x75

/* Sensitivity scale factors */
#define ACCEL_SCALE_2G    16384.0f   /* LSB/g  — ±2g range */
#define GYRO_SCALE_250    131.0f     /* LSB/(°/s) — ±250°/s range */
#define GRAVITY_MS2       9.80665f

/* IMU data structure — all globals in main.c mirror these */
typedef struct {
    float accel_x;   /* m/s² */
    float accel_y;
    float accel_z;
    float gyro_x;    /* deg/s */
    float gyro_y;
    float gyro_z;
    float temp_c;    /* °C */
    uint32_t timestamp_ms;
} IMUData_t;

/* Public API */
HAL_StatusTypeDef IMU_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef IMU_Read(IMUData_t *data);

/* Internal handle — stored by IMU_Init, used by IMU_Read */
extern I2C_HandleTypeDef *_imu_i2c;

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H */
