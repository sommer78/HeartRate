/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      centralFullTestApp_application.h
* @brief     centralFullTestApp app implementation
* @details   centralFullTestApp app implementation
* @author    ranhui
* @date      2015-05-06
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _SIMPLE_BLE_CENTRAL_APP_H_
#define _SIMPLE_BLE_CENTRAL_APP_H_
#include "bee_message.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "profileApi.h"
#include "profile_client.h"

/**
 * @brief  Memory Dump in Mouse App
 *
 * Debug Macros--For Memory Dump Tests
 */
/* Memory dump--interrupt maner */
#define MEM_DUMP_INT    0
#define MEM_DUMP_IO_GROUP GPIO_GROUP_B
#define MEM_DUMP_IO_INDEX 2

/**
 * @brief  HOGP report ID definition
 *
 * HOGP report ID definition
 */


extern uint8_t ScanDestAddr[10][6];
extern uint8_t ScanAddrNum;





extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
extern TAppResult App_CentralRoleCallback(TCentralAppCB_MsgType msgType, TCentralAppCB_MsgData centralCbData);
extern TAppResult App_ClientCallback(TClientID clientID, void *pData);
extern void central_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);


#ifdef __cplusplus
}
#endif

#endif

