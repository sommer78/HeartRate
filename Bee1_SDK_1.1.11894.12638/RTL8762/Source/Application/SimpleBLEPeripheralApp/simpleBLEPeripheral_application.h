/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      simpleBLEPeripheral_application.h
* @brief     SimpleBLEPeripheral
* @details   SimpleBLEPeripheral
* @author    ranhui
* @date      2015-03-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _MOUSE_APPLICATION__
#define _MOUSE_APPLICATION__
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
 * @fn          AppProfileCallback
 * @brief      All the bt profile callbacks are handled in this function.
 *                Then the event handling function shall be called according to the serviceID
 *                of BEE_IO_MSG.
 *
 * @param    serviceID  -  service id of profile
 * @param    pData  - pointer to callback data
 * @return     void
 */
TAppResult AppProfileCallback(uint8_t serviceID, void *pData);
#ifdef ANCS_SUPPORT
void AppAncsNotificationCallback(uint8_t ancs_notification_type, uint8_t *pData, uint8_t len);
#endif
#ifdef __cplusplus
}
#endif

#endif

