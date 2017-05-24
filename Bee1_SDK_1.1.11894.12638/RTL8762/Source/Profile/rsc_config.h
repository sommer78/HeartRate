/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    rsc_config.h
  * @brief   This file includes common constants or types for RSC service/client. 
  *          And some optional feature may be defined in this file. 
  * @details
  * @author  Ethan
  * @date    1-September-2015
  * @version v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _RSC_CONFIG_H_
#define _RSC_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup RSC RSC
  * @brief RSC Profile
  * @{
  */ 

/** @defgroup RSC_Common RSC Common
  * @brief RSC Profile Common data
  * @{
  */ 

/** @defgroup RSC_Common_Exported_Constants RSC Common Exported Constants
  * @brief macros that both RSC service/client may use all defined here
  * @{
  */

/** @defgroup RSC_UUIDs RSC UUIDs
  * @brief RSC Profile UUID definitions
  * @{
  */
#define GATT_UUID_RSC                       0x1814
#define GATT_UUID_CHAR_RSC_MEASUREMENT      0x2A53
#define GATT_UUID_CHAR_RSC_FEATURE          0x2A54
#define GATT_UUID_CHAR_SC_CONTROL_POINT     0x2A55
#define GATT_UUID_CHAR_SENSOR_LOCATION      0x2A5D
/** @} End of RSC_UUIDs */

/** @defgroup RSC_Measurement_Flag_Bit_Mask RSC Measurement Flag Bit Mask
  * @brief  RSC measurement 'Flags' present bit mask.
  * @{
  */
#define RSC_INC_STRIDE_LENGTH_MASK          (1)             /**<  Instantaneous Stride Length Present. */
#define RSC_INC_TOTAL_DISTANCE_MASK         (1<<1)          /**< Total Distance Present. */
#define RSC_INC_RUNNING_STATUS_MASK         (1<<2)          /**< Walking or Running Status bits. */
#define RSC_INC_ALL_PRESENTS                (0x07)
/** @} */

/** @defgroup RSC_Feature_Bit_Mask RSC Feature Bit Mask
  * @brief  RSC features bit mask.
  * @{
  */
#define RSC_SUPPORT_STRIDE_LENGTH_MASK      (1)
#define RSC_SUPPORT_TOTAL_DISTANCE_MASK     (1<<1)
#define RSC_SUPPORT_RUNNING_STATUS_MASK     (1<<2)
#define RSC_SUPPORT_CALIBRATE_MASK          (1<<3)
#define RSC_SUPPORT_MULTI_SENSOR_MASK       (1<<4)
#define RSC_ALL_FEATURE_SUPPORTED           (0x1F)
/** @} */

/** @defgroup RSC_Sensor_Location_Bit_Mask RSC Sensor Location Bit Mask
  * @brief  RSC supported sensor location bit mask.
  * @{
  */
#define RSC_SENS_LOC_OTHER_MASK             (1)
#define RSC_SENS_LOC_TOP_OF_SHOE_MASK       (1<<1)
#define RSC_SENS_LOC_IN_SHOE_MASK           (1<<2)
#define RSC_SENS_LOC_HIP_MASK               (1<<3)
#define RSC_SENS_LOC_FRONT_WHEEL_MASK       (1<<4)
#define RSC_SENS_LOC_LEFT_CRANK_MASK        (1<<5)
#define RSC_SENS_LOC_RIGHT_CRANK_MASK       (1<<6)
#define RSC_SENS_LOC_LEFT_PEDAL_MASK        (1<<7)
#define RSC_SENS_LOC_RIGHT_PEDAL_MASK       (1<<8)
#define RSC_SENS_LOC_FRONT_HUB_MASK         (1<<9)
#define RSC_SENS_LOC_REAR_DROPOUT_MASK      (1<<10)
#define RSC_SENS_LOC_CHAINSTAY_MASK         (1<<11)
#define RSC_SENS_LOC_REAR_WHEEL_MASK        (1<<12)
#define RSC_SENS_LOC_REAR_HUB_MASK          (1<<13)
#define RSC_SENS_LOC_CHEST_MASK             (1<<14)
#define RSC_ALL_SENS_LOC_SUPPORTED          (0x7FFF)
/** @} */

/** @defgroup RSC_Control_Point_OpCodes RSC Control Point OpCodes
  * @brief  Control Point OpCodes
  * @{
  */
#define RSC_CP_OPCODE_RESERVED              0x00
#define RSC_CP_OPCODE_SET_CUMULATIVE        0x01
#define RSC_CP_OPCODE_START_CALIBRATION     0x02
#define RSC_CP_OPCODE_UPDATE_SENS_LOC       0x03
#define RSC_CP_OPCODE_REQ_SENS_LOC_LIST     0x04
#define RSC_CP_OPCODE_RSP_CODE              0x10
/** @} */

/** @defgroup RSC_Control_Point_Response_Codes RSC Control Point Response Codes
  * @brief  Control Point Response Codes
  * @{
  */
#define RSC_CP_RSPCODE_RESERVED             0x00
#define RSC_CP_RSPCODE_SUCCESS              0x01
#define RSC_CP_RSPCODE_OPCODE_UNSUPPORT     0x02
#define RSC_CP_RSPCODE_INVALID_PARAMETER    0x03
#define RSC_CP_RSPCODE_OPERATION_FAILED     0x04
/** @} */

/** @defgroup RSC_Error_Codes RSC Error Codes
  * @brief  Error codes defined in RSC spec
  * @{
  */
#define RSC_ERR_PROC_ALREADY_IN_PROG        0x80
#define RSC_ERR_CCCD_IMPROPERLY_CFG         0x81
/** @} */

/** @defgroup RSC_Data_Length RSC Data Length
  * @brief  Max bytes of RSC Measurement data and RSC Control Point data
  * @{
  */
#define RSC_MAX_MEASUREMENT_VALUE           10
#define RSC_MAX_CTL_PNT_VALUE               18
/** @} */

/** @} End of RSC_Common_Exported_Constants */

/** @defgroup RSC_Common_Exported_Types RSC Common Exported Types
  * @brief macros that both RSC service/client may use all defined here
  * @{
  */

/** @defgroup RSC_Sensor_Locations RSC Sensor Locations
  * @brief  All RSC sensor locations defined in spec
  * @{
  */
typedef enum
{
    RSC_SENSOR_LOC_OTHER            = 0,
    RSC_SENSOR_LOC_TOP_OF_SHOE      = 1,
    RSC_SENSOR_LOC_IN_SHOE          = 2,
    RSC_SENSOR_LOC_HIP              = 3,
    RSC_SENSOR_LOC_FRONT_WHEEL      = 4,
    RSC_SENSOR_LOC_LEFT_CRANK       = 5,
    RSC_SENSOR_LOC_RIGHT_CRANK      = 6,
    RSC_SENSOR_LOC_LEFT_PEDAL       = 7,
    RSC_SENSOR_LOC_RIGHT_PEDAL      = 8,
    RSC_SENSOR_LOC_FRONT_HUB        = 9,
    RSC_SENSOR_LOC_REAR_DROPOUT     = 10,
    RSC_SENSOR_LOC_CHAINSTAY        = 11,
    RSC_SENSOR_LOC_REAL_WHEEL       = 12,
    RSC_SENSOR_LOC_REAL_HUB         = 13,
    RSC_SENSOR_LOC_CHEST            = 14,
    RSC_SENSOR_LOC_MAX
} TRSCSensorLocation;
/** @} */

/** @defgroup RSC_Measurement_Data RSC Measurement Data
  * @brief  RSC measurement data, variable length during connection, max can reach 10 bytes
  * @{
  */
typedef struct _RSCMeasurement
{
    uint8_t    cur_length; /**<  length of current RSC measurement data. */
    uint8_t    value[RSC_MAX_MEASUREMENT_VALUE]; /**<  value of current RSC measurement data. */
} TRSCMeasurement, * PRSCMeasurement;
/** @} */

/** @defgroup RSC_Control_Point_Data RSC Control Point Data
  * @brief  RSC Control Point data, variable length during connection, max can reach 18 bytes
  * @{
  */
typedef struct _RSCControlPoint
{
    uint8_t    cur_length; /**<  length of current RSC Control Point data, . */
    uint8_t    value[RSC_MAX_CTL_PNT_VALUE]; /**<  value of current RSC Control Point data, . */
} TRSCControlPoint, * PRSCControlPoint;
/** @} */

/** @} End of RSC_Common_Exported_Types */

/** @} End of RSC_Common */

/** @} End of RSC */

/** @} End of RTK_Profile_Module */


///@cond
/** @brief  optional feature of RSC service, can be modified by user. */
#define RSC_MULTIPLE_SENSOR_LOCATIONS_SUPPORT
#define RSC_SC_CONTROL_POINT_SUPPORT
/** @brief  Index of each characteristic in RSC service database. */
#define GATT_SVC_RSC_MEASUREMENT_INDEX      2
#define GATT_SVC_RSC_FEATURE_INDEX          5
#define GATT_SVC_RSC_SENS_LOC_INDEX         7
#define GATT_SVC_RSC_CTL_PNT_INDEX          9
#define GATT_SVC_RSC_MEAS_CCCD_INDEX        (GATT_SVC_RSC_MEASUREMENT_INDEX + 1)
#define GATT_SVC_RSC_CTL_PNT_CCCD_INDEX     (GATT_SVC_RSC_CTL_PNT_INDEX + 1)
///@endcond

#ifdef __cplusplus
}
#endif

#endif
