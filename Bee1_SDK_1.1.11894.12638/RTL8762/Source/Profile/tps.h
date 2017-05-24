/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     tps.h
  * @brief    Head file for using tx power service.
  * @details  TPS data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _TPS_H_
#define _TPS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"

/** @addtogroup RTK_Profile_Module
  * @{
  */

/** @defgroup TPS TPS
  * @brief Tx Power Service
  * @{
  */ 

/** @defgroup TPS_Exported_Constants TPS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */      
/** @defgroup TPS_Application_Parameters TPS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */     
#define TPS_PARAM_TX_POWER                      1
/** @} */

///@cond
/** @brief  TPS profile related services UUIDs */
#define GATT_UUID_TX_POWER_SERVICE              0x1804
#define GATT_UUID_CHAR_TX_LEVEL                 0x2A07

/** @brief TPS Attribute Index */ 
#define TPS_TX_POWER_VALUE_INDEX          2
///@endcond

/** @defgroup TPS_Upstream_Message TPS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
/** @defgroup TPS_Read_Info TPS Read Info
  * @brief  Read characteristic value.
  * @{
  */
#define TPS_READ_TX_POWER_VALUE         2
/** @} */
/** @} End of TPS_Upstream_Message */

/** @} End of TPS_Exported_Constants */

/** @defgroup TPS_Exported_Types TPS Exported Types
  * @brief  types that other .c files may use all defined here
  * @{
  */
/* Add all public types here */
/** @defgroup TPS_Upstream_Message TPS Upstream Message
  * @brief TPS data struct for notification data to application.
  * @{
  */
/** Message content: @ref TPS_Upstream_Message */
typedef union _TTPS_UPSTREAM_MSG_DATA
{
    INT8 read_value_index;
}TTPS_UPSTREAM_MSG_DATA;

/** TPS service data to inform application. */
typedef struct _TTPS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TTPS_UPSTREAM_MSG_DATA    msg_data;
} TTPS_CALLBACK_DATA;
/** @} */

/** @} End of TPS_Exported_Types */

/** @defgroup TPS_Exported_Functions TPS Exported Functions
  * @brief functions that other .c files may use all defined here.
  * @{
  */
bool TPS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr );
uint8_t TPS_AddService(void* pFunc);
/** @} End of TPS_Exported_Functions */

/** @} End of TPS */

/** @} End of RTK_Profile_Module */
  
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* _TPS_H_ */
