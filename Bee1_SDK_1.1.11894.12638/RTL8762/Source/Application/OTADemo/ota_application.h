/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ota_application.h
* @brief
* @details
* @author    hunter
* @date      2015-07-14
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __OTA_APPLICATION__
#define __OTA_APPLICATION__
#include "bee_message.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "profileAPI.h"
/******************************************************************
 * @fn          AppHandleIODriverMessage
 * @brief      All the application events are pre-handled in this function.
 *                All the IO MSGs are sent to this function, Then the event handling function
 *                shall be called according to the MSG type.
 *
 * @param    io_driver_msg_recv  - bee io msg data
 * @return     void
 */
extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);

/******************************************************************
 * @fn         AppHandleGATTCallback
 * @brief      All the bt profile callbacks are handled in this function.
 *                Then the event handling function shall be called according to the serviceID
 *                of BEE_IO_MSG.
 *
 * @param    serviceID  -  service id of profile
 * @param    pData  - pointer to callback data
 * @return     void
 */
extern TAppResult AppProfileCallback(uint8_t serviceID, void *pData);


/******************************************************************
 * @fn         OTA_HandleAdcMessage
 * @brief
 *
 * @param      pBeeIoMsg pointer to Bee io message
 * @return     void
 */
extern void OTA_HandleAdcMessage(BEE_IO_MSG  *pBeeIoMsg);

#ifdef __cplusplus
}
#endif

#endif

