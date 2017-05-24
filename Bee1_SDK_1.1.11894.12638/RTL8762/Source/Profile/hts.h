/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hts.h
  * @brief    Variables and interfaces for using Health Thermometer Service.
  * @details  Health Therm Service data structs and functions.
  * @author   ethan_su
  * @date     2015-04-13
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _HTS_SERVICE_DEF_H
#define _HTS_SERVICE_DEF_H

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile.h"
#include "hts_config.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup HTS HTS
  * @brief Health Thermometer Service
  * @{
  */ 

/** @defgroup HTS_Exported_Constants HTS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup HTS_Application_Parameters HTS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define HTS_PARAM_MEASUREMENT_TEMPPARAM_FLAG                1
#define HTS_PARAM_MEASUREMENT_TEMPERATUER_VALUE             2
#define HTS_PARAM_MEASUREMENT_TIME_STAMP                    3
#define HTS_PARAM_MEASUREMENT_TEMPERATURE_TYPE              4
#define HTS_PARAM_MEASUREMENT_INTERVAL                      5
#define HTS_PARAM_VALID_RANGE                               6
/** @} */

///@cond
#define GATT_UUID_HELTH_TEMPERATURE_SERVICE                 0x1809
#define GATT_UUID_CHAR_HTS_MEASUREMENT_INTERVAL             0x2A21
#define GATT_UUID_CHAR_HTS_INTERMEDIATE_TMPERATURE          0x2A1E
#define GATT_UUID_CHAR_HTS_TEMPERATURE_MEASUREMENT          0x2a1c
#define GATT_UUID_CHAR_HTS_TEMPERATURE_TYPE                 0x2a1d
///@endcond

/** @defgroup HTS_Upstream_Message HTS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup HTS_Read_Info HTS Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define HTS_READ_TEMPERATURE_TYPE_VALUE                     1
#define HTS_READ_MEASUREMENT_INTERVAL_VALUE                 2
#define HTS_READ_VALID_RANGE_VALUE                          3
/** @} */

/** @defgroup HTS_Write_Info HTS Write Info
  * @brief  Parameter for writing characteristic value.
  * @{ 
  */
#define HTS_WRITE_MEASUREMENT_INTERVAL_VALUE                1
/** @} */

/** @defgroup HTS_Notify_Indicate_Info HTS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define HTS_NOTIFY_INDICATE_TEMPERATURE_MEASUREMENT_VALUE_ENABLE     1
#define HTS_NOTIFY_INDICATE_TEMPERATURE_MEASUREMENT_VALUE_DISABLE    2
#define HTS_NOTIFY_INDICATE_INTERMEDIATE_TEMPERATURE_VALUE_ENABLE    3
#define HTS_NOTIFY_INDICATE_INTERMEDIATE_TEMPERATURE_VALUE_DISABLE   4
#define HTS_NOTIFY_INDICATE_MEASUREMENT_INTERVAL_VALUE_ENABLE        5
#define HTS_NOTIFY_INDICATE_MEASUREMENT_INTERVAL_VALUE_DISABLE       6
/** @} */

/** @} End of HTS_Upstream_Message */

/** @defgroup HTS_Measurement_Flag HTS Measurement Flag
  * @{
  */
#define HTS_FLAG_MEASUREMENT_UINT_BIT                       BIT0
#define HTS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT          BIT1
#define HTS_FLAG_MEASUREMENT_TYPE_PRESNET_BIT               BIT2
/** @} */

///@cond
#define HTS_MEASUREMENT_VALUE_MAX_LEN  13
///@endcond

/** @} End of HTS_Exported_Constants */

/** @defgroup HTS_Exported_Types HTS Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/* Add all public types here */
typedef enum
{
    HTS_TEMPERATURE_TYPE_ARMPIT = 1,
    HTS_TEMPERATURE_TYPE_BODY,
    HTS_TEMPERATURE_TYPE_EAR,
    HTS_TEMPERATURE_TYPE_FINGER,
    HTS_TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT,
    HTS_TEMPERATURE_TYPE_MOUTH,
    HTS_TEMPERATURE_TYPE_RECTUM,
    HTS_TEMPERATURE_TYPE_TOE,
    HTS_TEMPERATURE_TYPE_TYMPANUM,
    HTS_TEMPERATURE_TYPE_RESERVED,
} HTS_TEMPERATURE_TYPE;

/** Health Thermometer Measurement Value Flag */
typedef struct _HEALTH_THERMOMETER_MEASUREMENT_VALUE_FLAG
{
    uint8_t Temp_value_units_bit: 1;
    uint8_t Temp_time_stamp_present_bit: 1;
    uint8_t Temp_type_present_bit: 1;
    uint8_t RFU: 5;
} HEALTH_THERMOMETER_MEASUREMENT_VALUE_FLAG;

typedef struct 
{
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Day;
    uint8_t  Hours;
    uint8_t  Minutes;
    uint8_t  Seconds;
} TIME_STAMP;

typedef struct _THTS_MEASUREMENT_INTERVAL_VALID_RANGE
{
    uint16_t LowerValue;
    uint16_t UpperValue;
} THTS_MEASUREMENT_INTERVAL_VALID_RANGE;


/** Health Thermometer Measurement Value */
typedef struct
{
    HEALTH_THERMOMETER_MEASUREMENT_VALUE_FLAG flag;
    uint32_t TempCelsiusValue; //FLOAT Celsius
    uint32_t TempFahrenheitValue; //FLOAT Celsius
    TIME_STAMP  TimeStamp;
    uint8_t TemperNextDesc;
} HTS_MESAURMENT;

/** @defgroup HTS_Callback_Data HTS Callback Data
  * @brief HTS data struct for notification data to application.
  * @{
  */
typedef struct _THTS_WRITE_MSG{
    uint8_t write_index; //!< ref: @ref HTS_Write_Info
    uint16_t measurement_interval; //seconds
}THTS_WRITE_MSG;

typedef union _THTS_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; //!< ref: @ref HTS_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref HTS_Read_Info
    THTS_WRITE_MSG write;
}THTS_UPSTREAM_MSG_DATA;

typedef struct _THTS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    THTS_UPSTREAM_MSG_DATA    msg_data;
} THTS_CALLBACK_DATA;
/** @} */

/** @} End of HTS_Exported_Types */

/** @defgroup HTS_Exported_Functions HTS Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
uint8_t HTS_AddService(void* pFunc);
bool HTS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
bool HTS_MeasurementValueIndicate( uint8_t ServiceId );
bool HTS_IntermediateTemperatureValueNotify( uint8_t ServiceId );
bool HTS_MeasurementIntervalNotify( uint8_t ServiceId, uint16_t seconds);
/** @} End of HTS_Exported_Functions */

/** @} End of HTS */

/** @} End of RTK_Profile_Module */
#ifdef __cplusplus
}
#endif

#endif // _HTS_SERVICE_DEF_H

