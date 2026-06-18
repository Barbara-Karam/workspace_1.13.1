#ifndef __IMU_H
#define __IMU_H

#include "main.h"

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} IMUData_t;

HAL_StatusTypeDef IMU_Init(void);
HAL_StatusTypeDef IMU_Read(IMUData_t *data);

#endif
