#include "imu.h"
#include <string.h>

/* Internal I2C handle pointer — set by IMU_Init() */
I2C_HandleTypeDef *_imu_i2c = NULL;

/* ---------------------------------------------------------------
 * Helper: write one byte to a register
 * --------------------------------------------------------------- */
static HAL_StatusTypeDef imu_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return HAL_I2C_Master_Transmit(_imu_i2c, MPU6050_ADDR, buf, 2, 10);
}

/* ---------------------------------------------------------------
 * Helper: read N bytes starting at reg into dst
 * --------------------------------------------------------------- */
static HAL_StatusTypeDef imu_read_reg(uint8_t reg, uint8_t *dst, uint16_t len)
{
    HAL_StatusTypeDef s;
    s = HAL_I2C_Master_Transmit(_imu_i2c, MPU6050_ADDR, &reg, 1, 10);
    if (s != HAL_OK) return s;
    return HAL_I2C_Master_Receive(_imu_i2c, MPU6050_ADDR, dst, len, 10);
}

/* ---------------------------------------------------------------
 * IMU_Init
 * Call once at startup with a pointer to the configured hi2c handle.
 * Returns HAL_OK if MPU6050 found and configured, HAL_ERROR otherwise.
 * --------------------------------------------------------------- */
HAL_StatusTypeDef IMU_Init(I2C_HandleTypeDef *hi2c)
{
    _imu_i2c = hi2c;

    uint8_t who_am_i = 0;

    /* Check WHO_AM_I register — MPU6050 always returns 0x68 */
    if (imu_read_reg(MPU6050_REG_WHO_AM_I, &who_am_i, 1) != HAL_OK)
        return HAL_ERROR;

    if (who_am_i != 0x68)
        return HAL_ERROR;   /* Wrong chip or wiring problem */

    /* Wake up — clear SLEEP bit in PWR_MGMT_1 */
    if (imu_write_reg(MPU6050_REG_PWR_MGMT_1, 0x00) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(10);  /* Let PLL stabilise */

    /* Use PLL with X-axis gyro reference (recommended over internal oscillator) */
    if (imu_write_reg(MPU6050_REG_PWR_MGMT_1, 0x01) != HAL_OK)
        return HAL_ERROR;

    /* Digital Low Pass Filter — DLPF_CFG = 3 → 42 Hz BW, 4.8 ms delay */
    if (imu_write_reg(MPU6050_REG_CONFIG, 0x03) != HAL_OK)
        return HAL_ERROR;

    /* Sample rate divider = 7 → sample rate = 1kHz / (7+1) = 125 Hz */
    if (imu_write_reg(MPU6050_REG_SMPLRT_DIV, 0x07) != HAL_OK)
        return HAL_ERROR;

    /* Gyro  full scale: ±250 °/s  (bits [4:3] = 00) */
    if (imu_write_reg(MPU6050_REG_GYRO_CONFIG, 0x00) != HAL_OK)
        return HAL_ERROR;

    /* Accel full scale: ±2 g      (bits [4:3] = 00) */
    if (imu_write_reg(MPU6050_REG_ACCEL_CONFIG, 0x00) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

/* ---------------------------------------------------------------
 * IMU_Read
 * Burst-reads all 14 bytes (accel + temp + gyro) in one I2C transaction.
 * Converts raw counts to SI units and fills IMUData_t.
 * Returns HAL_OK on success.
 * --------------------------------------------------------------- */
HAL_StatusTypeDef IMU_Read(IMUData_t *data)
{
    if (_imu_i2c == NULL || data == NULL)
        return HAL_ERROR;

    uint8_t raw[14];  /* ACCEL_XOUT_H ... GYRO_ZOUT_L (14 consecutive registers) */

    if (imu_read_reg(MPU6050_REG_ACCEL_XOUT_H, raw, 14) != HAL_OK)
        return HAL_ERROR;

    /* Reconstruct signed 16-bit values (big-endian from MPU) */
    int16_t ax_raw = (int16_t)(raw[0]  << 8 | raw[1]);
    int16_t ay_raw = (int16_t)(raw[2]  << 8 | raw[3]);
    int16_t az_raw = (int16_t)(raw[4]  << 8 | raw[5]);
    int16_t t_raw  = (int16_t)(raw[6]  << 8 | raw[7]);
    int16_t gx_raw = (int16_t)(raw[8]  << 8 | raw[9]);
    int16_t gy_raw = (int16_t)(raw[10] << 8 | raw[11]);
    int16_t gz_raw = (int16_t)(raw[12] << 8 | raw[13]);

    /* Convert to physical units */
    data->accel_x = (float)ax_raw / ACCEL_SCALE_2G * GRAVITY_MS2;  /* m/s² */
    data->accel_y = (float)ay_raw / ACCEL_SCALE_2G * GRAVITY_MS2;
    data->accel_z = (float)az_raw / ACCEL_SCALE_2G * GRAVITY_MS2;

    data->gyro_x  = (float)gx_raw / GYRO_SCALE_250;  /* deg/s */
    data->gyro_y  = (float)gy_raw / GYRO_SCALE_250;
    data->gyro_z  = (float)gz_raw / GYRO_SCALE_250;

    /* Temperature: formula from MPU6050 datasheet */
    data->temp_c  = (float)t_raw / 340.0f + 36.53f;  /* °C */

    data->timestamp_ms = HAL_GetTick();

    return HAL_OK;
}
