/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hrs.h
  * @brief    Variables and interfaces for using Heart Rate Service.
  * @details  Heart Rate Service data structs and functions.
  * @author   ethan_su
  * @date     2015-04-13
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _HRS_SERVICE_DEF_H
#define _HRS_SERVICE_DEF_H

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile.h"
#include "hrs_config.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup HRS HRS
  * @brief Heart Rate Service
  * @{
  */ 

/** @defgroup HRS_Exported_Constants HRS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */      
/** @defgroup HRS_Application_Parameters HRS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
/** @brief  Type of parameters can be set or got by application. */
#define HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG               0x01
#define HRS_HEART_RATE_MEASUREMENT_PARAM_MEASUREMENT_VALUE  0x02
#define HRS_HEART_RATE_MEASUREMENT_PARAM_ENERGY_EXPENDED    0x03
#define HRS_HEART_RATE_MEASUREMENT_PARAM_RR_INTERVAL        0x04

#define HRS_BODY_SENSOR_LOCATION_PARAM_VALUE                0x10
/** @} */

///@cond
#define GATT_UUID_SERVICE_HEART_RATE                        0x180D
#define GATT_UUID_CHAR_HRS_HEART_RATE_MEASUREMENT           0x2A37
#define GATT_UUID_CHAR_HRS_BODY_SENSOR_LOCATION             0x2A38
#define GATT_UUID_CHAR_HRS_HEART_RATE_CP                    0x2A39
///@endcond

/** @defgroup HRS_Upstream_Message HRS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup HRS_Read_Info HRS Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define HRS_READ_BODY_SENSOR_LOCATION_VALUE                 1
/** @} */

/** @defgroup HRS_Notify_Indicate_Info HRS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define HRS_NOTIFY_INDICATE_MEASUREMENT_VALUE_ENABLE        1
#define HRS_NOTIFY_INDICATE_MEASUREMENT_VALUE_DISABLE       2
/** @} */

/** @} End of HRS_Upstream_Message */

/** @defgroup HRS_Sensor_Location HRS Sensor Location
  * @brief  Body Sensor Location Value
  * @{
  */
#define  BODY_SENSOR_LOCATION_VALUE_OTHER                   0
#define  BODY_SENSOR_LOCATION_VALUE_CHEST                   1
#define  BODY_SENSOR_LOCATION_VALUE_WRIST                   2
#define  BODY_SENSOR_LOCATION_VALUE_FINGER                  3
#define  BODY_SENSOR_LOCATION_VALUE_HAND                    4
#define  BODY_SENSOR_LOCATION_VALUE_EAR_LOBE                5
#define  BODY_SENSOR_LOCATION_VALUE_FOOT                    6
/** @} */

/** @defgroup HRS_Control_Point HRS Control Point
  * @brief  Control Point
  * @{
  */

/** @defgroup HRS_Control_Point_OpCodes HRS Control Point OpCodes
  * @brief  Control Point OpCodes
  * @{
  */
typedef enum _THRS_HEART_RATE_CP_OPCODE
{
    HRS_HEART_RATE_CP_OPCODE_RESERVED = 0,
    HRS_HEART_RATE_CP_OPCODE_RESET_ENERGY_EXPENDED = 1
} THRS_HEART_RATE_CP_OPCODE;
/** @} */

/** @defgroup HRS_Control_Point_Response_Codes HRS Control Point Response Codes
  * @brief  Control Point Response Codes
  * @{
  */
#define HRS_CP_RSPCODE_RESERVED                             0x00
#define HRS_CP_RSPCODE_SUCCESS                              0x01
#define HRS_CP_RSPCODE_OPCODE_UNSUPPORT                     0x02
#define HRS_CP_RSPCODE_INVALID_PARAMETER                    0x03
#define HRS_CP_RSPCODE_OPERATION_FAILED                     0x04
/** @} */

///@cond
/** @brief  Judge HRS Control Point operation is available or not. */
#define HRS_CTL_PNT_OPERATE_ACTIVE(x)                     \
    (x == HRS_HEART_RATE_CP_OPCODE_RESET_ENERGY_EXPENDED)
///@endcond

/** @} End of HRS_Control_Point */

///@cond
#define Heart_Rate_Value_Format_UINT8                       0
#define Heart_Rate_Value_Format_UINT16                      1

#define HRS_HEART_RATE_MEASUREMENT_VALUE_MAX_LEN            7
#define HRS_MAX_CTL_PNT_VALUE                               1
///@endcond

/** @} End of HRS_Exported_Constants */

/** @defgroup HRS_Exported_Types HRS Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/* Add all public types here */
/** notification indification falg */
typedef struct _HRS_NOTIFY_INDICATE_FLAG
{
    uint8_t heart_rate_measurement_notify_enable: 1;
    uint8_t rfu: 7;
} HRS_NOTIFY_INDICATE_FLAG;

/** Heart Rate Measurement Value Flag */
typedef struct _HEART_RATE_MEASUREMENT_VALUE_FLAG
{
    uint8_t Heart_Rate_Value_Format_bit: 1;
    uint8_t Sensor_Contact_Status_bits: 2;
    uint8_t Energy_Expended_Status_bit: 1;
    uint8_t RR_Interval_bit: 1;
    uint8_t RFU: 3;
} HEART_RATE_MEASUREMENT_VALUE_FLAG;

/** Heart Rate Measurement Value */
typedef struct _HEART_RATE_MEASUREMENT_VALUE
{
    HEART_RATE_MEASUREMENT_VALUE_FLAG flag;
    uint16_t HeartRateMeasurementValue;
    uint16_t Energy_Expended;
    uint16_t RR_Interval;
} HEART_RATE_MEASUREMENT_VALUE;

/**
 * @brief HRS Control Point data, variable length during connection, max can reach 17 bytes.
 *
 * HRS Control Point data used to store the Control Point Command recieved from the client.
*/
typedef struct _HRSControlPoint
{
    uint8_t    cur_length; /**<  length of current CSC Control Point data . */
    uint8_t    value[HRS_MAX_CTL_PNT_VALUE]; /**<  value of current CSC Control Point data . */
} THRSControlPoint, * PHRSControlPoint;

/** @defgroup HRS_Callback_Data HRS Callback Data
  * @brief HRS data struct for notification data to application.
  * @{
  */
typedef struct _THRS_WRITE_MSG{
    THRS_HEART_RATE_CP_OPCODE opcode; //!< ref: @ref HRS_Control_Point_OpCodes
}THRS_WRITE_MSG;

typedef union _THRS_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; //!< ref: @ref HRS_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref HRS_Read_Info
    THRS_WRITE_MSG write;
}THRS_UPSTREAM_MSG_DATA;

typedef struct _THRS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    THRS_UPSTREAM_MSG_DATA    msg_data;
} THRS_CALLBACK_DATA;
/** @} */

/** @} End of HRS_Exported_Types */

/** @defgroup HRS_Exported_Functions HRS Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
uint8_t HRS_AddService(void* pFunc);
bool HRS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
bool HRS_HeartRateMeasurementValueNotify( uint8_t ServiceId );
/** @} End of HRS_Exported_Functions */

/** @} End of HRS */

/** @} End of RTK_Profile_Module */

#ifdef __cplusplus
}
#endif

#endif /* _HRS_SERVICE_DEF_H */

