/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    pxp_io_new.h
  * @brief
  * @details
  * @author  Chuanguo Xue
  * @date    7-April-2015
  * @version v0.1
  * *************************************************************************************
  */

#ifndef _PXP_IO_NEW_H_
#define _PXP_IO_NEW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "rtl876x_gpio.h"
enum
{
    KEY_FIND_ME,
    KEY_ALERT_CANCEL
};

extern void KEY_Init(GPIO_InitTypeDef *GPIO_INT_PARAM);
extern void ADC_Battery_Init(void);
extern void RTC_Initconfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __PXP_IO_NEW_H */
