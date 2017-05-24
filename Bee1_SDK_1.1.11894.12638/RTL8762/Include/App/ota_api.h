/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     ota_api.h
* @brief    Over the air update.
* @details  OTA is used to update device firmware via bluetooth.
* @author   ranhui
* @date     2015-10-29
* @version  v1.2
*************************************************************************************************************
*/

#ifndef _OTA_API_H_
#define  _OTA_API_H_
/** @addtogroup OTA_API OTA Exported API
  * @{
  */

/**
 * @brief  Set OTA mode flag of stack.
 *
 * After call this API, App can call  APIs below
 * HalWatchDogConfig(1,0,1);
 * HalWatchDogEnable();
 * to restart the platform to enter OTA mode.
 * @param bEnable  TRUE to Enable OTA mode, FALSE to disable OTA mode.
 * @return  void
*/
void dfuSetOtaMode(bool bEnable);

/** @} End of OTA_API */
#endif

