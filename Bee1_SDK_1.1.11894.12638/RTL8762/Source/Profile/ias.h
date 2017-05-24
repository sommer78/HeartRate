/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     ias.h
  * @brief    Head file for using immediate alert service.
  * @details  IAS data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */
  
/* Define to prevent recursive inclusion */
#ifndef _IAS_H_
#define _IAS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */
    
/* Add Includes here */
#include "profileAPI.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup IAS IAS
  * @brief Immediate Alert Service
  * @{
  */ 

/** @defgroup IAS_Exported_Constants IAS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */  
    
///@cond
/** @brief IAS related services UUIDs */
#define GATT_UUID_IMMEDIATE_ALERT_SERVICE       0x1802
#define GATT_UUID_CHAR_ALERT_LEVEL              0x2A06

/** @brief Index defines for Characteristic's value */ 
#define GATT_SVC_PXP_IMMEDIATE_AlERT_VALUE_INDEX   2
///@endcond

/** @} End of IAS_Exported_Constants */

/** @defgroup IAS_Exported_Types IAS Exported Types
  * @brief  types that other .c files may use all defined here
  * @{
  */
/* Add all public types here */
/** @defgroup IAS_Callback_Data IAS Callback Data
  * @brief IAS data struct for notification data to application.
  * @{
  */
/** Message content */
typedef union _TIAS_UPSTREAM_MSG_DATA
{
    uint8_t write_alert_level;
}TIAS_UPSTREAM_MSG_DATA;

/** IAS service data to inform application */
typedef struct _TIAS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TIAS_UPSTREAM_MSG_DATA    msg_data;
} TIAS_CALLBACK_DATA;
/** @} */

/** @} End of IAS_Exported_Types */

/** @defgroup IAS_Exported_Functions IAS Exported Functions
  * @brief functions that other .c files may use all defined here.
  * @{
  */
uint8_t IAS_AddService(void* pFunc);
/** @} End of IAS_Exported_Functions */

/** @} End of IAS */

/** @} End of RTK_Profile_Module */
  
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif //_IAS_H

