/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ota_service.h
* @brief
* @details
* @author   hunter_shuai
* @date     14-July-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#ifndef _OTA_SERVICE_H_
#define _OTA_SERVICE_H_

#include "profileAPI.h"


#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */


/** @brief  Demo Profile service related UUIDs. */

#define GATT_UUID_CHAR_OTA                          0xFFD1
#define GATT_UUID_CHAR_MAC                          0xFFD2
#define GATT_UUID_CHAR_PATCH                        0xFFD3
#define GATT_UUID_CHAR_APP_VERSION                  0xFFD4



/** @brief  Index of each characteristic in Demo Profile service database. */
#define BLE_SERVICE_CHAR_OTA_INDEX                0x02
#define BLE_SERVICE_CHAR_MAC_ADDRESS_INDEX        0x04
#define BLE_SERVICE_CHAR_PATCH_INDEX              0x06
#define BLE_SERVICE_CHAR_APP_VERSION_INDEX        0x08



/** @brief OTA Write callback data type definition.*/
#define OTA_WRITE_CHAR_VAL      0x01

#define OTA_VALUE_ENTER      0x01

typedef struct _TOTA_WRITE_MSG {
    uint8_t opcode;
    uint8_t value;
} TOTA_WRITE_MSG;


typedef union _TOTA_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    uint8_t read_value_index;
    TOTA_WRITE_MSG write;
} TOTA_UPSTREAM_MSG_DATA;

/**
 * @brief OTA servic data struct for notification data to application.
 *
 * OTA service data to inform application.
*/
typedef struct _TOTA_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;                    /**<  @brief EventId defined upper */
    TOTA_UPSTREAM_MSG_DATA  msg_data;
} TOTA_CALLBACK_DATA;


extern uint8_t OTAService_AddService(void *pFunc);

#ifdef __cplusplus
}
#endif

#endif
