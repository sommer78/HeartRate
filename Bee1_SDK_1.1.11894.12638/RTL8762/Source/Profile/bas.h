/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     bas.h
  * @brief    Head file for using battery service.
  * @details  BAS data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _BAS_H_
#define _BAS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup BAS BAS
  * @brief Battery Service
  * @{
  */ 

/** @defgroup BAS_Exported_Constants BAS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */      
/** @defgroup BAS_Application_Parameters BAS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define BAS_PARAM_BATTERY_LEVEL                 1
/** @} */

///@cond
/** @brief  BAS related UUIDs. */
#define GATT_UUID_BATTERY                       0x180F
#define GATT_UUID_CHAR_BAS_LEVEL                0x2A19

/** @brief BAS Attribute Index */ 
#define GATT_SVC_BAS_BATTERY_LEVEL_INDEX        2 /**< @brief Index for battery level chars's value */
#define GATT_SVC_BAS_CHAR_CCCD_INDEX            3 /**< @brief CCCD Index for battery level chars's value */
///@endcond

/** @defgroup BAS_Upstream_Message BAS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
  
/** @defgroup BAS_Read_Info BAS Read Info
  * @brief  Parameter for read characteristic value.
  * @{
  */
#define BAS_READ_BATTERY_LEVEL  1
/** @} */

/** @defgroup BAS_Notify_Indicate_Info BAS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define BAS_NOTIFY_BATTERY_LEVEL_ENABLE  1
#define BAS_NOTIFY_BATTERY_LEVEL_DISABLE  2
/** @} */

/** @} End of BAS_Upstream_Message */

/** @} End of BAS_Exported_Constants */

/** @defgroup BAS_Exported_Types BAS Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */
/* Add all public types here */
/** @defgroup BAS_Callback_Data BAS Callback Data
  * @brief BAS data struct for notification data to application.
  * @{
  */
/** Message content: @ref BAS_Upstream_Message */
typedef union _TBAS_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    uint8_t read_value_index;
}TBAS_UPSTREAM_MSG_DATA;

/** BAS service data to inform application */
typedef struct _TBAS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TBAS_UPSTREAM_MSG_DATA    msg_data;
} TBAS_CALLBACK_DATA;
/** @} */

/** @} End of BAS_Exported_Types */

/** @defgroup BAS_Exported_Functions BAS Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
bool BAS_SetParameter( uint8_t param_type, uint8_t length, uint8_t *value );
uint8_t BAS_AddService(void* pFunc);
bool BAS_BatteryLevelValueNotify( uint8_t ServiceId, uint8_t BatteryLevel );
bool BAS_BatteryLevelValueReadConfirm( uint8_t ServiceId, uint8_t BatteryLevel );

/** @} End of BAS_Exported_Functions */

/** @} End of BAS */

/** @} End of RTK_Profile_Module */
  
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _BAS_H_ */
