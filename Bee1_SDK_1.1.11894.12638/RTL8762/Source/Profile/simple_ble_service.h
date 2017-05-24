/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     simple_ble_service.h
  * @brief    Demonstration of how to implement a self-definition service.
  * @details  Demonstration of different kinds of service interfaces.
  * @author   ethan
  * @date     2015-5-10
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _SIMPLE_BLE_SERVICE_H_
#define _SIMPLE_BLE_SERVICE_H_

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"
#include "simple_ble_config.h"

/** @addtogroup SIMP SIMP
  * @brief Simple BLE Profile
  * @{
  */

/** @defgroup SIMP_Service SIMP Service
  * @brief Simple BLE Service
  * @{
  */ 

/** @defgroup SIMP_Service_Exported_Constants SIMP Service Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */
#define GATT_UUID_GATT                               0x1801

/** @defgroup SIMP_Service_Application_Parameters SIMP Service Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define SIMPLE_BLE_SERVICE_PARAM_V1_READ_CHAR_VAL           0x01
#define SIMPLE_BLE_SERVICE_PARAM_CTL_PNT_PROG_CLR           0x02
/** @} */

///@cond
/** @brief  Index of each characteristic in Demo Profile service database. */
#define SIMPLE_BLE_SERVICE_CHAR_V1_READ_INDEX               0x02
#define SIMPLE_BLE_SERVICE_CHAR_V2_WRITE_INDEX              0x04
#define SIMPLE_BLE_SERVICE_CHAR_V3_NOTIFY_INDEX             0x06
#define SIMPLE_BLE_SERVICE_CHAR_V4_INDICATE_INDEX           0x09
#define SIMPLE_BLE_SERVICE_CHAR_V5_CTL_PNT_INDEX            0x0C
#define SIMPLE_BLE_SERVICE_CHAR_NOTIFY_CCCD_INDEX (SIMPLE_BLE_SERVICE_CHAR_V3_NOTIFY_INDEX + 1)
#define SIMPLE_BLE_SERVICE_CHAR_INDICATE_CCCD_INDEX (SIMPLE_BLE_SERVICE_CHAR_V4_INDICATE_INDEX + 1)
#define SIMPLE_BLE_SERVICE_CHAR_CTL_PNT_CCCD_INDEX (SIMPLE_BLE_SERVICE_CHAR_V5_CTL_PNT_INDEX + 1)
#define SIMPLE_BLE_SERVICE_CHAR_V6_WRITE_INDEX              0xe 
///@endcond

/** @defgroup SIMP_Service_Upstream_Message SIMP Service Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup SIMP_Service_Read_Info SIMP Service Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define SIMP_READ_V1                                        1
/** @} */

/** @defgroup SIMP_Service_Write_Info SIMP Service Write Info
  * @brief  Parameter for writing characteristic value.
  * @{ 
  */
#define SIMP_WRITE_V2                                       1
#define SIMP_WRITE_V5_OPCODE_SET_CHAR_INDICATE_VAL          2
#define SIMP_WRITE_V5_OPCODE_SET_CHAR_NOTIFY_VAL            3
#define SIMP_PRE_WRITE_V6                                   4
/** @} */

/** @defgroup SIMP_Service_Notify_Indicate_Info SIMP Service Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define SIMP_NOTIFY_INDICATE_V3_ENABLE     1
#define SIMP_NOTIFY_INDICATE_V3_DISABLE    2
#define SIMP_NOTIFY_INDICATE_V4_ENABLE     3
#define SIMP_NOTIFY_INDICATE_V4_DISABLE    4
#define SIMP_NOTIFY_INDICATE_V5_ENABLE     5
#define SIMP_NOTIFY_INDICATE_V5_DISABLE    6
/** @} */

/** @} End of SIMP_Service_Upstream_Message */

///@cond
/** @brief  Check simple BLE profile Control Point operation is available or not. */
#define SIMPLE_BLE_SERVICE_CTL_PNT_OPERATE_ACTIVE(x)                     \
    ((x == SIMPLE_CP_OPCODE_SET_CHAR_INDICATE_VAL) ||  \
     (x == SIMPLE_CP_OPCODE_SET_CHAR_NOTIFY_VAL) || \
     (x == SIMPLE_CP_OPCODE_RSP_CODE))
///@endcond

/** @} End of SIMP_Service_Exported_Constants */

/** @defgroup SIMP_Service_Exported_Types SIMP Service Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/** @defgroup TSIMP_WRITE_MSG TSIMP_WRITE_MSG
  * @brief Simple BLE service written msg to application.
  * @{
  */
typedef struct _TSIMP_WRITE_MSG{
    uint8_t opcode; //!< ref: @ref SIMP_Control_Point_OpCodes, @ref SIMP_Service_Write_Info
    uint8_t value;
}TSIMP_WRITE_MSG;
/** @} End of TSIMP_WRITE_MSG */

/** @defgroup TSIMP_PRE_WRITE_MSG TSIMP_PRE_WRITE_MSG
  * @brief Simple BLE service prepare written msg to application.
  * @{
  */
typedef struct _TSIMP_PRE_WRITE_MSG{
    uint8_t opcode; //!< ref: @ref SIMP_Control_Point_OpCodes, @ref SIMP_Service_Write_Info
	uint16_t Len;
	uint8_t* pValue;
}TSIMP_PRE_WRITE_MSG;
/** @} End of TSIMP_PRE_WRITE_MSG */

/** @defgroup TSIMP_UPSTREAM_MSG_DATA TSIMP_UPSTREAM_MSG_DATA
  * @brief Simple BLE service callback message content.
  * @{
  */
typedef union _TSIMP_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; //!< ref: @ref SIMP_Service_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref SIMP_Service_Read_Info
    TSIMP_PRE_WRITE_MSG pre_write;
	TSIMP_WRITE_MSG write;
}TSIMP_UPSTREAM_MSG_DATA;
/** @} End of TSIMP_UPSTREAM_MSG_DATA */

/** @defgroup TSIMP_CALLBACK_DATA TSIMP_CALLBACK_DATA
  * @brief Simple BLE service data to inform application.
  * @{
  */
typedef struct _TSIMP_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TSIMP_UPSTREAM_MSG_DATA    msg_data;
} TSIMP_CALLBACK_DATA;
/** @} End of TSIMP_CALLBACK_DATA */

/** @} End of SIMP_Service_Exported_Types */

/** @defgroup SIMP_Service_Exported_Functions SIMP Service Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
extern uint8_t SimpBleService_AddService(void* pFunc);
extern bool SimpBleService_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
extern bool SimpBleService_SimpleV3Notify( uint8_t ServiceId, uint8_t value);
extern bool SimpBleService_SimpleV4Indicate( uint8_t ServiceId, uint8_t value);
/** @} End of SIMP_Service_Exported_Functions */

/** @} End of SIMP_Service */

/** @} End of SIMP */

#ifdef __cplusplus
}
#endif

#endif /* _SIMPLE_BLE_SERVICE_H_ */
