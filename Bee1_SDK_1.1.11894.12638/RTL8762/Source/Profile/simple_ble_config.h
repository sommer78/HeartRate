/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_config.h
  * @brief   This file includes common constants or types for Simple BLE service/client. 
  *          And some optional feature may be defined in this file. 
  * @details
  * @author  Ethan
  * @date    1-September-2015
  * @version v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _SIMPLE_BLE_CONFIG_H_
#define _SIMPLE_BLE_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup SIMP SIMP
  * @brief Simple BLE Profile
  * @{
  */ 

/** @defgroup SIMP_Config SIMP Config
  * @brief Simple BLE Profile Configurations
  * @{
  */ 
//#define SIMP_USE_UUID128                            /* Indicate Simple BLE Service use 128bit UUID or not(16bit UUID) */
//#define GATT_SERVICE_INCLUDED

/** @} End of SIMP_Config */

/** @defgroup SIMP_Common SIMP Common
  * @brief Simple BLE Profile Common data
  * @{
  */ 

/** @defgroup SIMP_Common_Exported_Constants SIMP Common Exported Constants
  * @brief macros that both Simple BLE service/client may use all defined here
  * @{
  */

/** @defgroup SIMP_UUIDs SIMP UUIDs
  * @brief Simple BLE Profile UUID definitions
  * @{
  */
#define GATT_UUID_SIMPLE_PROFILE                    0xA00A
#define GATT_UUID_CHAR_SIMPLE_V1_READ               0xB001
#define GATT_UUID128_CHAR_SIMPLE_V1_READ            0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xB0, 0x01, 0x00, 0x00
#define GATT_UUID_CHAR_SIMPLE_V2_WRITE              0xB002
#define GATT_UUID_CHAR_SIMPLE_V3_NOTIFY             0xB003
#define GATT_UUID_CHAR_SIMPLE_V4_INDICATE           0xB004
#define GATT_UUID_CHAR_SIMPLE_V5_CTL_PNT            0xB005
#define GATT_UUID_CHAR_SIMPLE_V6_WRITE_LONG         0xB006
/** @} End of SIMP_UUIDs */

/** @defgroup SIMP_Control_Point_OpCodes SIMP Control Point OpCodes
  * @brief  Control Point OpCodes
  * @{
  */
#define SIMPLE_CP_OPCODE_RESERVED                   0x00
#define SIMPLE_CP_OPCODE_SET_CHAR_INDICATE_VAL      0x01
#define SIMPLE_CP_OPCODE_SET_CHAR_NOTIFY_VAL        0x02
#define SIMPLE_CP_OPCODE_RSP_CODE                   0x10
/** @} End of SIMP_Control_Point_OpCodes */

/** @defgroup SIMP_Control_Point_Response_Codes SIMP Control Point Response Codes
  * @brief  Control Point Response Codes
  * @{
  */
#define SIMPLE_CP_RSPCODE_RESERVED                  0x00
#define SIMPLE_CP_RSPCODE_SUCCESS                   0x01
#define SIMPLE_CP_RSPCODE_OPCODE_UNSUPPORT          0x02
#define SIMPLE_CP_RSPCODE_INVALID_PARAMETER         0x03
#define SIMPLE_CP_RSPCODE_OPERATION_FAILED          0x04
/** @} End of SIMP_Control_Point_Response_Codes */

/** @} End of SIMP_Common_Exported_Constants */

/** @} End of SIMP_Common */

/** @} End of SIMP */

/** @} End of RTK_Profile_Module */

#ifdef __cplusplus
}
#endif

#endif
