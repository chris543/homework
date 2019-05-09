#ifndef ICM20602_H__
#define ICM20602_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf_gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern short gyro[3], accel[3];
    extern int16_t Gx_offset, Gy_offset, Gz_offset;
    extern float Acc1G_Values;
    extern float Pitch, Roll;

    extern short g_fGyro_x, g_fAccel_y, g_fAccel_z;
    extern uint8_t disableBalancing;

    void MPU6050_initialize(void); //初始化
    void MPU6050_Pose(void);
    void whoAmI(void);

#ifdef __cplusplus
}
#endif

#endif // ICM20602_H__
