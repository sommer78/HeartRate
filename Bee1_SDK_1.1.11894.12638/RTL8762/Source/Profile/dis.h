/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     dis.h
  * @brief    Head file for using device information service.
  * @details  DIS data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _DEVICE_INFO_SERVICE_H_
#define _DEVICE_INFO_SERVICE_H_

#ifdef  __cplusplus
 extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"

/** @addtogroup RTK_Profile_Module
  * @{
  */

/** @defgroup DIS DIS
  * @brief Device Information Service
  * @{
  */ 

/** @defgroup DIS_Exported_Constants DIS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */      
/** @defgroup DIS_Application_Parameters DIS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */ 
#define DIS_PARAM_MANUFACTURER_NAME         1
#define DIS_PARAM_MODEL_NUMBER              2
#define DIS_PARAM_SERIAL_NUMBER             3
#define DIS_PARAM_HARDWARE_REVISION         4
#define DIS_PARAM_FIRMWARE_REVISION         5
#define DIS_PARAM_SOFTWARE_REVISION         6
#define DIS_PARAM_SYSTEM_ID                 7
#define DIS_PARAM_IEEE_DATA_LIST            8
#define DIS_PARAM_PNP_ID                    9
/** @} */

///@cond
/** @brief  IEEE 11073 authoritative body values. */ 
#define DIS_IEEE_11073_BODY_EMPTY           0
#define DIS_IEEE_11073_BODY_IEEE            1
#define DIS_IEEE_11073_BODY_CONTINUA        2
#define DIS_IEEE_11073_BODY_EXP             254

#define DIS_SYSTEM_ID_LENGTH                8
#define DIS_PNP_ID_LENGTH                   7

/** @brief DIS related UUIDs */ 
#define GATT_UUID_DEVICE_INFORMATION_SERVICE   0x180A

#define GATT_UUID_CHAR_SYSTEM_ID               0x2A23
#define GATT_UUID_CHAR_MODEL_NUMBER            0x2A24
#define GATT_UUID_CHAR_SERIAL_NUMBER           0x2A25
#define GATT_UUID_CHAR_FIRMWARE_REVISION       0x2A26
#define GATT_UUID_CHAR_HARDWARE_REVISION       0x2A27
#define GATT_UUID_CHAR_SOFTWARE_REVISION       0x2A28
#define GATT_UUID_CHAR_MANUFACTURER_NAME       0x2A29
#define GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST   0x2A2A
#define GATT_UUID_CHAR_PNP_ID                  0x2A50
///@endcond

/** @defgroup DIS_Upstream_Message DIS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
/** @defgroup DIS_Read_Info DIS Read Info
  * @brief  Read characteristic value.
  * @{
  */
#define DIS_READ_MANU_NAME_INDEX        1
#define DIS_READ_MODEL_NUM_INDEX        2
#define DIS_READ_SERIAL_NUM_INDEX       3
#define DIS_READ_HARDWARE_REV_INDEX     4
#define DIS_READ_FIRMWARE_REV_INDEX     5
#define DIS_READ_SOFTWARE_REV_INDEX     6
#define DIS_READ_SYSTEM_ID_INDEX        7
#define DIS_READ_IEEE_CERT_STR_INDEX    8
#define DIS_READ_PNP_ID_INDEX           9
/** @} */
/** @} End of DIS_Upstream_Message */

/** @} End of DIS_Exported_Constants */

/** @defgroup DIS_Exported_Types DIS Exported Types
  * @brief  types that other .c files may use all defined here
  * @{
  */
/* Add all public types here */
/** @defgroup DIS_Callback_Data DIS Callback Data
  * @brief DIS data struct for notification data to application.
  * @{
  */
/** Message content: @ref DIS_Upstream_Message */
typedef union _TDIS_UPSTREAM_MSG_DATA
{
    uint8_t read_value_index;
}TDIS_UPSTREAM_MSG_DATA;

/** DIS service data to inform application */
typedef struct _TDIS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TDIS_UPSTREAM_MSG_DATA    msg_data;
} TDIS_CALLBACK_DATA;
/** @} */

/** @} End of DIS_Exported_Types */

/** @defgroup DIS_Exported_Functions DIS Exported Functions
  * @brief functions that other .c files may use all defined here.
  * @{
  */
bool DIS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr );
uint8_t DIS_AddService(void* pFunc);
/** @} End of DIS_Exported_Functions */

/** @} End of DIS */

/** @} End of RTK_Profile_Module */
  
#ifdef  __cplusplus
}
#endif      /* __cplusplus */

#endif /* _DEVICE_INFO_SERVICE_H_ */
