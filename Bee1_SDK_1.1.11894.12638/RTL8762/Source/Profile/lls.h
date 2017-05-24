/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     lls.h
  * @brief    Head file for using link loss service.
  * @details  LLS data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _LLS_H_
#define _LLS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup LLS LLS
  * @brief Link Loss Service
  * @{
  */ 

/** @defgroup LLS_Exported_Constants LLS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */      
/** @defgroup LLS_Application_Parameters LLS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */ 
#define LLS_PARAM_LINK_LOSS_ALERT_LEVEL               1
/** @} */

///@cond
/** @brief LLS profile related services UUIDs. */
#define GATT_UUID_LINK_LOSS_SERVICE             0x1803
#define GATT_UUID_CHAR_ALERT_LEVEL              0x2A06

/** @brief LLS Attribute Index */ 
#define GATT_SVC_PXP_LINK_LOSS_ALERT_LEVEL_INDEX   2
///@endcond

/** @defgroup LLS_Upstream_Message LLS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
/** @defgroup LLS_Read_Info LLS Read Info
  * @brief  Read characteristic value.
  * @{
  */
#define LLS_READ_ALERT_LEVEL 1
/** @} */
/** @} End of LLS_Upstream_Message */

/**
  * @} End of LLS_Exported_Constants
  */

/** @defgroup LLS_Exported_Types LLS Exported Types
  * @brief  types that other .c files may use all defined here
  * @{
  */
/* Add all public types here */
/** @defgroup LLS_Callback_Data LLS Callback Data
  * @brief LLS data struct for notification data to application.
  * @{
  */
/** Message content: @ref LLS_Upstream_Message */
typedef union _TLLS_UPSTREAM_MSG_DATA
{
    uint8_t read_value_index;
    uint8_t write_alert_level;
}TLLS_UPSTREAM_MSG_DATA;

/** LLSdata service data to inform application */
typedef struct _TLLS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TLLS_UPSTREAM_MSG_DATA    msg_data;
} TLLS_CALLBACK_DATA;
/** @} */

/** @} End of LLS_Exported_Types */

/** @defgroup LLS_Exported_Functions LLS Exported Functions
  * @brief functions that other .c files may use all defined here.
  * @{
  */
bool LLS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr );
uint8_t LLS_AddService(void* pFunc);
/** @} End of LLS_Exported_Functions */

/** @} End of LLS */

/** @} End of RTK_Profile_Module */
  
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif 

