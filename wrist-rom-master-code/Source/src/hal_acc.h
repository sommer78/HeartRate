#ifndef HAL_ACC_H
#define HAL_ACC_H

#include "bd_lis3dh_driver.h"
#define LIS3DH_ODR_FREQ LIS3DH_ODR_100Hz
#define ACC_UNIT            0.031
#define TAP_TIME_UNIT
#define TAP_TIME_LIMIT      3
#define TAP_LATENCY         14
#define TAP_WINDOW          30
#define TAP_THRESHOLD       1.5/ACC_UNIT

void hal_acc_init(void);
void hal_acc_PowerDown(void);
void hal_acc_enable(void);
bool hal_acc_ConfigClick(LIS3DH_ODR_t Freq, LIS3DH_Fullscale_t FullScale);
void hal_acc_GetFifoData(uint8_t * val);
void hal_acc_config_wake_int(bool enable);
void hal_acc_config_Dtap(bool enable);
void hal_acc_reset(void);
/*lint --flb "Leave library region" */
#endif

