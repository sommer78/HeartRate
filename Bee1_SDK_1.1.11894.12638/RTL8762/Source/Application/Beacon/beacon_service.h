/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     beacon_service.h
* @brief
* @details
* @author   hunter_shuai
* @date     23-June-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#ifndef _BEACON_SERVICE_H_
#define _BEACON_SERVICE_H_

#include "profileAPI.h"


#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/*ibeacon format id length, unit Byte*/
#define UUID_LENGTH    16
#define MAJORID_LENGTH 2
#define MINORID_LENGTH 2
#define MEASURED_POWER_LENGTH 1

/** @brief  Demo Profile service related UUIDs. */
#define GATT_UUID_CHAR_BEACON_UUID                     0xFFA1
#define GATT_UUID_CHAR_BEACON_MAJORID                  0xFFA2
#define GATT_UUID_CHAR_BEACON_MINORID                  0xFFA3
#define GATT_UUID_CHAR_BEACON_MEASURED_POWER           0xFFA4
#define GATT_UUID_CHAR_BEACON_ADV_INTERVAL             0xFFA6


/** @brief  Index of each characteristic in Demo Profile service database. */
#define BEACON_BLE_SERVICE_CHAR_UUID_INDEX                0x02
#define BEACON_BLE_SERVICE_CHAR_MAJORID_INDEX             0x04
#define BEACON_BLE_SERVICE_CHAR_MINORID_INDEX             0x06
#define BEACON_BLE_SERVICE_CHAR_MEASURED_POWER_INDEX      0x08
#define BEACON_BLE_SERVICE_CHAR_ADV_INTERVAL_INDEX        0x0A


/** @brief Beacon Write callback data type definition.*/
#define BEACON_WRITE_UUID_CHAR_VAL              0x01
#define BEACON_WRITE_MAJORID_CHAR_VAL           0x02
#define BEACON_WRITE_MINORID_CHAR_VAL           0x03
#define BEACON_WRITE_MEASURED_POWER_CHAR_VAL    0x04
#define BEACON_WRITE_OTA_CHAR_VAL               0x05
#define BEACON_WRITE_ADV_INTERVAL_CHAR_VAL      0x06

/** @brief Beacon Read callback data type definition.*/
#define BEACON_READ_UUID_CHAR_VAL           0x01
#define BEACON_READ_MAJORID_CHAR_VAL        0x02
#define BEACON_READ_MINORID_CHAR_VAL        0x03
#define BEACON_READ_MEASURED_POWER_CHAR_VAL 0x04
#define BEACON_READ_ADV_INTERVAL_CHAR_VAL   0x05
#define BEACON_READ_MAC_ADRESS_CHAR_VAL     0x06



typedef struct _TBEACON_WRITE_MSG {
    uint8_t opcode;
    uint8_t *value;
} TBEACON_WRITE_MSG;


typedef union _TBEACON_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    uint8_t read_value_index;
    TBEACON_WRITE_MSG write;
} TBEACON_UPSTREAM_MSG_DATA;

/**
 * @brief beacon servic data struct for notification data to application.
 *
 * beacon service data to inform application.
*/
typedef struct _TBEACON_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;                    /**<  @brief EventId defined upper */
    TBEACON_UPSTREAM_MSG_DATA  msg_data;
} TBEACON_CALLBACK_DATA;

extern uint16_t adv_interval;
extern uint8_t BeaconService_AddService(void *pFunc);
extern bool BeaconBleService_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
extern uint8_t BeaconService_AddOTAService(void *pFunc);

#ifdef __cplusplus
}
#endif

#endif
