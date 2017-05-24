/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      beacon_application.h
* @brief     beacon
* @details
* @author    hunter
* @date      2015-06-28
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __BEACON_APPLICATION__
#define __BEACON_APPLICATION__
#include "bee_message.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"

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
extern TAppResult AppHandleProfileCallback(uint8_t serviceID, void *pData);


/******************************************************************
 * @fn         Beacon_HandleAdcMessage
 * @brief
 *
 * @param      pBeeIoMsg pointer to Bee io message
 * @return     void
 */
extern void Beacon_HandleAdcMessage(BEE_IO_MSG  *pBeeIoMsg);

/******************************************************************
 * @fn         BeaconStartAdvertising
 * @brief
 *
 * @param      void
 * @return     void
 */
extern void BeaconStartAdvertising(void);

/******************************************************************
 * @fn         BeaconStopAdvertising
 * @brief
 *
 * @param      void
 * @return     void
 */
extern void BeaconStopAdvertising(void);


#ifdef __cplusplus
}
#endif

#endif

