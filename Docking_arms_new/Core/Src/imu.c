/**
  ******************************************************************************
  * @file    imu.c
  * @brief   IMU driver: MPU6050 (default) or ICM-42688-P
  *          Change IMU_TYPE in imu.h to switch between them.
  *          Both use I2C1 at 400kHz (PB6=SCL, PB7=SDA).
  ******************************************************************************
  */
#include "imu.h"

/* ── Shared I2C timeout ──────────────────────────────────────────────────── */
#define IMU_I2C_TIMEOUT_MS  10

/* ────────────────────────────────────────────────────────────────────────── */
/*  MPU6050 Implementation                                                    */
/* ────────────────────────────────────────────────────────────────────────── */
#ifdef IMU_TYPE_MPU6050

HAL_StatusTypeDef IMU_Init(void)
{
    uint8_t data;

    /* Verify WHO_AM_I */
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR,
                                              IMU_REG_WHO_AM_I, 1,
                                              &data, 1, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK || data != IMU_WHO_AM_I_VAL) return HAL_ERROR;

    /* Wake up device (clear sleep bit) */
    data = 0x00;
    ret = HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR,
                            IMU_REG_PWR_MGMT_1, 1,
                            &data, 1, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) return ret;

    /* Sample rate divider: 0 -> 1kHz with DLPF, or Gyro_rate / (1+SMPLRT_DIV) */
    data = 0x00;
    ret = HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR,
                            IMU_REG_SMPLRT_DIV, 1,
                            &data, 1, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) return ret;

    /* DLPF config: bandwidth ~94Hz accel / 98Hz gyro */
    data = 0x02;
    ret = HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR,
                            IMU_REG_CONFIG, 1,
                            &data, 1, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) return ret;

    /* Gyro full-scale: ±250°/s (bits[4:3] = 00) */
    data = 0x00;
    ret = HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR,
                            IMU_REG_GYRO_CONFIG, 1,
                            &data, 1, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) return ret;

    /* Accel full-scale: ±2g (bits[4:3] = 00) */
    data = 0x00;
    ret = HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR,
                            IMU_REG_ACCEL_CONFIG, 1,
                            &data, 1, IMU_I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef IMU_Read(IMU_Data_t *data)
{
    uint8_t raw[12];  /* 6 bytes accel + 6 bytes gyro */

    /* Burst read accel registers */
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR,
                                              IMU_REG_ACCEL_XOUT, 1,
                                              raw, 6, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) { data->valid = 0; return ret; }

    /* Burst read gyro registers */
    ret = HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR,
                           IMU_REG_GYRO_XOUT, 1,
                           &raw[6], 6, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) { data->valid = 0; return ret; }

    /* Convert raw big-endian int16 to floats */
    int16_t raw_ax = (int16_t)((raw[0]  << 8) | raw[1]);
    int16_t raw_ay = (int16_t)((raw[2]  << 8) | raw[3]);
    int16_t raw_az = (int16_t)((raw[4]  << 8) | raw[5]);
    int16_t raw_gx = (int16_t)((raw[6]  << 8) | raw[7]);
    int16_t raw_gy = (int16_t)((raw[8]  << 8) | raw[9]);
    int16_t raw_gz = (int16_t)((raw[10] << 8) | raw[11]);

    data->accel_x = (float)raw_ax * IMU_ACCEL_SCALE;
    data->accel_y = (float)raw_ay * IMU_ACCEL_SCALE;
    data->accel_z = (float)raw_az * IMU_ACCEL_SCALE;
    data->gyro_x  = (float)raw_gx * IMU_GYRO_SCALE;
    data->gyro_y  = (float)raw_gy * IMU_GYRO_SCALE;
    data->gyro_z  = (float)raw_gz * IMU_GYRO_SCALE;
    data->valid   = 1;

    return HAL_OK;
}

#endif /* IMU_TYPE_MPU6050 */


/* ────────────────────────────────────────────────────────────────────────── */
/*  ICM-42688-P Implementation                                                */
/* ────────────────────────────────────────────────────────────────────────── */
#ifdef IMU_TYPE_ICM42688

HAL_StatusTypeDef IMU_Init(void)
{
    uint8_t data;

    /* Verify WHO_AM_I */
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR,
                                              IMU_REG_WHO_AM_I, 1,
                                              &data, 1, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK || data != IMU_WHO_AM_I_VAL) return HAL_ERROR;

    /* Enable accel and gyro in low-noise mode */
    data = 0x0F;  /* ACCEL_MODE = LN, GYRO_MODE = LN */
    ret = HAL_I2C_Mem_Write(&hi2c1, IMU_I2C_ADDR,
                            IMU_REG_PWR_MGMT0, 1,
                            &data, 1, IMU_I2C_TIMEOUT_MS);
    /* Allow 1ms for startup */
    HAL_Delay(1);
    return ret;
}

HAL_StatusTypeDef IMU_Read(IMU_Data_t *data)
{
    uint8_t raw[12];

    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR,
                                              IMU_REG_ACCEL_XOUT, 1,
                                              raw, 6, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) { data->valid = 0; return ret; }

    ret = HAL_I2C_Mem_Read(&hi2c1, IMU_I2C_ADDR,
                           IMU_REG_GYRO_XOUT, 1,
                           &raw[6], 6, IMU_I2C_TIMEOUT_MS);
    if (ret != HAL_OK) { data->valid = 0; return ret; }

    int16_t raw_ax = (int16_t)((raw[0]  << 8) | raw[1]);
    int16_t raw_ay = (int16_t)((raw[2]  << 8) | raw[3]);
    int16_t raw_az = (int16_t)((raw[4]  << 8) | raw[5]);
    int16_t raw_gx = (int16_t)((raw[6]  << 8) | raw[7]);
    int16_t raw_gy = (int16_t)((raw[8]  << 8) | raw[9]);
    int16_t raw_gz = (int16_t)((raw[10] << 8) | raw[11]);

    data->accel_x = (float)raw_ax * IMU_ACCEL_SCALE;
    data->accel_y = (float)raw_ay * IMU_ACCEL_SCALE;
    data->accel_z = (float)raw_az * IMU_ACCEL_SCALE;
    data->gyro_x  = (float)raw_gx * IMU_GYRO_SCALE;
    data->gyro_y  = (float)raw_gy * IMU_GYRO_SCALE;
    data->gyro_z  = (float)raw_gz * IMU_GYRO_SCALE;
    data->valid   = 1;

    return HAL_OK;
}

#endif /* IMU_TYPE_ICM42688 */
