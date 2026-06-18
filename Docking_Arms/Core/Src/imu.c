#include "imu.h"
#include "i2c.h"

#define MPU6050_ADDR 0x68 << 1

/* Add a global variable so we can see what the chip actually is */
volatile uint8_t imu_who_am_i = 0;

HAL_StatusTypeDef IMU_Init(void)
{
    uint8_t data;

    /* Read the WHO_AM_I register (0x75) */
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x75, 1, (uint8_t*)&imu_who_am_i, 1, 1000);

    /* Accept original (0x68) AND common clones (0x70, 0x71, 0x73, 0x98) */
    if (imu_who_am_i == 0x68 || imu_who_am_i == 0x70 || imu_who_am_i == 0x71 ||
        imu_who_am_i == 0x73 || imu_who_am_i == 0x98)
    {
        /* 1. Wake up the chip (Write 0 to PWR_MGMT_1) */
        data = 0;
        if (HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1, &data, 1, 1000) != HAL_OK) return HAL_ERROR;

        /* 2. Set Sample Rate */
        data = 0x07;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x19, 1, &data, 1, 1000);

        /* 3. Set Accel config (+-2g) */
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1C, 1, &data, 1, 1000);

        return HAL_OK;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef IMU_Read(IMUData_t *data)
{
    uint8_t rec_data[14];

    if (HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x3B, 1, rec_data, 14, 1000) == HAL_OK) {
        int16_t accel_x = (int16_t)(rec_data[0] << 8 | rec_data[1]);
        int16_t accel_y = (int16_t)(rec_data[2] << 8 | rec_data[3]);
        int16_t accel_z = (int16_t)(rec_data[4] << 8 | rec_data[5]);
        int16_t gyro_x  = (int16_t)(rec_data[8] << 8 | rec_data[9]);
        int16_t gyro_y  = (int16_t)(rec_data[10] << 8 | rec_data[11]);
        int16_t gyro_z  = (int16_t)(rec_data[12] << 8 | rec_data[13]);

        data->accel_x = accel_x / 16384.0f;
        data->accel_y = accel_y / 16384.0f;
        data->accel_z = accel_z / 16384.0f;
        data->gyro_x  = gyro_x / 131.0f;
        data->gyro_y  = gyro_y / 131.0f;
        data->gyro_z  = gyro_z / 131.0f;
        return HAL_OK;
    }
    return HAL_ERROR;
}
