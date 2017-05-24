/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    hal_wdg.h
  * @brief   Watchdog function
  * @details
  * @author  Lory Xu
  * @date    2014-8
  * @version v0.1
  * *************************************************************************************
  */

#ifndef __HAL_WDG_
#define __HAL_WDG_

#include "rtl876x.h"

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Function Hardware Function
  * @{
  */

/** @addtogroup Platform_Function_Watchdog Watchdog
  * @{
  */

/**
  * @brief     Config Watchdog
  * @param[in] divfactor: 16Bit: 32.768k/(1+divfactor)
  * @param[in] cnt_limit: 2^(cnt_limit+1) - 1 ; max 11~15 = 0xFFFF
  * @param[in] Wdt_mode:
  *     @arg 0: Interrupt CPU
  *     @arg 1: Reset system
  * @retval    None
  */
void HalWatchDogConfig(IN  uint16_t divfactor, IN  uint8_t  cnt_limit, IN  uint8_t  Wdt_mode);

/**
  * @brief  Enable Watchdog
  * @retval None
  */
void HalWatchDogEnable(void);

/**
  * @brief  Disable Watchdog
  * @retval None
  */
void HalWatchDogDisable(void);

/**
  * @brief  Watch dog system reset
  * @retval None
  */
void WDG_SystemReset(void);

/** End of Platform_Function_Watchdog
 * @}
 */

/** End of Platform_Function
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* __HAL_WDG_ */
