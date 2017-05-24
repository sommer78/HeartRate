/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     lns.h
  * @brief    Variables and interfaces for using Location And Navigation Service.
  * @details  Location And Navigation Service data structs and functions.
  * @author   ranhui
  * @date     2015-04-13
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _LNS_SERVICE_DEF_H
#define _LNS_SERVICE_DEF_H

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profile.h"
#include "profileAPI.h"
#include "lns_config.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup LNS LNS
  * @brief Location And Navigation Service
  * @{
  */ 

/** @defgroup LNS_Exported_Constants LNS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup LNS_LN_Feature LNS LN Feature
  * @{
  */
#define LN_FEATURE_INSTANTANEOUS_SPEED_SUPPORTED            BIT0
#define LN_FEATURE_TOTAL_DISTANCE_SUPPORTED                 BIT1
#define LN_FEATURE_LOCATION_SUPPORTED                       BIT2
#define LN_FEATURE_ELEVATION_SUPPORTED                      BIT3
#define LN_FEATURE_HEADING_SUPPORTED                        BIT4
#define LN_FEATURE_ROLLING_TIME_SUPPORTED                   BIT5
#define LN_FEATURE_UTC_TIME_SUPPORTED                       BIT6
#define LN_FEATURE_REMAINING_DISTANCE_SUPPORTED             BIT7
#define LN_FEATURE_REMAINING_VERTICAL_DISTANCE_SUPPORTED    BIT8
#define LN_FEATURE_ESTIMATED_TIME_OF_ARRIVAL_SUPPORTED      BIT9
#define LN_FEATURE_NUMBER_OF_BEACONS_IN_SOLUTION_SUPPORTED  BIT10
#define LN_FEATURE_NUMBER_OF_BEACONS_IN_VIEW_SUPPORTED      BIT11
#define LN_FEATURE_TIME_TO_FIRST_FIX_SUPPORTED              BIT12
#define LN_FEATURE_ESTIMATED_HORIZONTAL_POSITION_ERROR_SUPPORTED    BIT13
#define LN_FEATURE_ESTIMATED_VERTICAL_POSITION_ERROR_SUPPORTED      BIT14
#define LN_FEATURE_HORIZONTAL_DILUTION_OF_PRECISION_SUPPORTED       BIT15
#define LN_FEATURE_VERTICAL_DILUTION_OF_PRECISION_SUPPORTED         BIT16
#define LN_FEATURE_LOCATION_AND_SPEED_CHARACTERISTIC_CONTENT_MASKING_SUPPORTED  BIT17
#define LN_FEATURE_FIX_RATE_SETTING_SUPPORTED               BIT18
#define LN_FEATURE_ELEVATION_SETTING_SUPPORTED              BIT19
#define LN_FEATURE_POSITION_STATUS_SUPPORTED                BIT20
#define LN_FEATURE_RFU                                      BIT21
/** @} */

/** @defgroup LNS_Application_Parameters LNS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define LNS_PARAM_LN_FEATURE_SUPPORT                        0x30

#define LNS_LAS_PARAM_INC_FLAG                              0x01
#define LNS_LAS_PARAM_INSTANTANEOUS_SPEED                   0x02
#define LNS_LAS_PARAM_TOTAL_DISTANCE                        0x03
#define LNS_LAS_PARAM_LOCATION_LATITUDE                     0x04
#define LNS_LAS_PARAM_LOCATION_LONGITUDE                    0x05
#define LNS_LAS_PARAM_ELEVATION                             0x06
#define LNS_LAS_PARAM_HEADING                               0x07
#define LNS_LAS_PARAM_ROLLING_TIME                          0x08
#define LNS_LAS_PARAM_UTC_TIME                              0x09
#define LNS_LAS_PARAM_POSITION_STATUS                       0x0A
#define LNS_LAS_PARAM_SPEED_AND_DISTANCE_FORMAT             0x0B
#define LNS_LAS_PARAM_ELEVATION_SOURCE                      0x0C
#define LNS_LAS_PARAM_HEADING_SOURCE                        0x0D

#define LNS_NAVIGATION_PARAM_INC_FLAG                       0x11
#define LNS_NAVIGATION_PARAM_BEARING                        0x12
#define LNS_NAVIGATION_PARAM_HEADING                        0x13
#define LNS_NAVIGATION_PARAM_REMAINING_DISTANCE             0x14
#define LNS_NAVIGATION_PARAM_REMAINING_VERTICAL_DISTANCE    0x15
#define LNS_NAVIGATION_PARAM_ESTIMATED_TIME_OF_ARRIVAL      0x16
#define LNS_NAVIGATION_PARAM_POSITION_STATUS                0x17
#define LNS_NAVIGATION_PARAM_HEADING_SOURCE                 0x18
#define LNS_NAVIGATION_PARAM_NAVIGATION_INDICATOR_TYPE      0x19
#define LNS_NAVIGATION_PARAM_WAYPOINT_REACHED               0x1A
#define LNS_NAVIGATION_PARAM_DESTINATION_REACHED            0x1B

#define LNS_PQ_PARAM_INC_FLAG                               0x20
#define LNS_PQ_PARAM_NUMBER_OF_BEACONS_IN_SOLUTION          0x21
#define LNS_PQ_PARAM_NUMBER_OF_BEACONS_IN_VIEW              0x22
#define LNS_PQ_PARAM_TIME_TO_FIRST_FIX                      0x23
#define LNS_PQ_PARAM_EHPE                                   0x24
#define LNS_PQ_PARAM_EVPE                                   0x25
#define LNS_PQ_PARAM_HDOP                                   0x26
#define LNS_PQ_PARAM_VDOP                                   0x27

#define LNS_CP_PARA_NUMBER_OF_ROUTE                         0x41
#define LNS_CP_PARA_NAME_OF_ROUTE                           0x42

#define LNS_PARAM_CTL_PNT_PROG_CLR                          0x51
/** @} */

///@cond
#define GATT_UUID_SERVICE_LOCATION_AND_NAVIGATION           0x1819
#define GATT_UUID_CHAR_LNS_LOCATION_AND_SPEED               0x2A67
#define GATT_UUID_CHAR_LNS_NAVIGATION                       0x2A68
#define GATT_UUID_CHAR_LNS_POSITION_QUALITY                 0x2A69
#define GATT_UUID_CHAR_LNS_LN_FEATURE                       0x2A6A
#define GATT_UUID_CHAR_LNS_LN_CP                            0x2A6B
///@endcond

/** @defgroup LNS_Upstream_Message LNS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup LNS_Read_Info LNS Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define LNS_READ_POSITION_QUALITY_VALUE                     1
/** @} */

/** @defgroup LNS_Notify_Indicate_Info LNS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define LNS_NOTIFY_INDICATE_LOCATION_AND_SPEED_ENABLE       1
#define LNS_NOTIFY_INDICATE_LOCATION_AND_SPEED_DISABLE      2
#define LNS_NOTIFY_INDICATE_CP_ENABLE                       3
#define LNS_NOTIFY_INDICATE_CP_DISABLE                      4
#define LNS_NOTIFY_INDICATE_NAVIGATION_ENABLE               5
#define LNS_NOTIFY_INDICATE_NAVIGATION_DISABLE              6
/** @} */

/** @} End of LNS_Upstream_Message */

/** @defgroup LNS_Control_Point LNS Control Point
  * @brief  Control Point
  * @{
  */

/** @defgroup LNS_Control_Point_OpCodes LNS Control Point OpCodes
  * @brief  Control Point OpCodes
  * @{
  */
typedef enum _TLN_CP_OPCODE
{
    LN_CP_OPCODE_RESERVED = 0,
    LN_CP_OPCODE_SET_CUMULATIVE_VALUE = 1,
    LN_CP_OPCODE_MASK_LOCATION_AND_SPEED_CHAR_CONTENT = 2,
    LN_CP_OPCODE_NAVIGATION_CONTROL = 3,
    LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES = 4,
    LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE = 5,
    LN_CP_OPCODE_SELECT_ROUTE = 6,
    LN_CP_OPCODE_SET_FIX_RATE = 7,
    LN_CP_OPCODE_SET_ELEVATION = 8,
    LN_CP_OPCODE_RESPONSE_CODE = 32
} TLN_CP_OPCODE;
/** @} */

/** @defgroup LNS_Control_Point_Response_Codes LNS Control Point Response Codes
  * @brief  Control Point Response Codes
  * @{
  */
#define LN_CP_RSPCODE_RESERVED                              0x00
#define LN_CP_RSPCODE_SUCCESS                               0x01
#define LN_CP_RSPCODE_OPCODE_UNSUPPORT                      0x02
#define LN_CP_RSPCODE_INVALID_PARAMETER                     0x03
#define LN_CP_RSPCODE_OPERATION_FAILED                      0x04
/** @} */

///@cond
#define LNS_MAX_CTL_PNT_VALUE                               17
/** @brief  Judge CSC Control Point operation is available or not. */
#define LN_CTL_PNT_OPERATE_ACTIVE(x)                     \
    ( (x == LN_CP_OPCODE_SET_CUMULATIVE_VALUE)  || \
      (x == LN_CP_OPCODE_MASK_LOCATION_AND_SPEED_CHAR_CONTENT)  || \
      (x == LN_CP_OPCODE_NAVIGATION_CONTROL) || \
      (x == LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES) || \
      (x == LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE) || \
      (x == LN_CP_OPCODE_SELECT_ROUTE) || \
      (x == LN_CP_OPCODE_SET_FIX_RATE) || \
      (x == LN_CP_OPCODE_SET_ELEVATION) || \
      (x == LN_CP_OPCODE_RESPONSE_CODE) )
///@endcond

/** @} End of LNS_Control_Point */

///@cond
#define LNS_POSITION_AND_QUALITY_VALUE_MAX_LEN 16
#define LNS_LOCATION_AND_SPEED_VALUE_MAX_LEN 28
#define LNS_NAVIGATION_VALUE_MAX_LEN 19
///@endcond

/** @} End of LNS_Exported_Constants */

/** @defgroup LNS_Exported_Types LNS Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/* Add all public types here */
typedef struct _DATE_TIME
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} DATE_TIME;

/** Position Quality Value Flag */
typedef struct _POSITION_QUALITY_VALUE_FLAG
{
    uint16_t Number_of_Beacons_in_Solution_Present: 1;
    uint16_t Number_of_Beacons_in_View_Present: 1;
    uint16_t Time_to_First_Fix_Present: 1;
    uint16_t EHPE_Present: 1;
    uint16_t EVPE_Present: 1;
    uint16_t HDOP_Present: 1;
    uint16_t VDOP_Present: 1;
    uint16_t Position_Status: 2;
    uint16_t Rfu: 7;
} POSITION_QUALITY_VALUE_FLAG;

/** Position Quality Value */
typedef struct _POSITION_QUALITY_VALUE
{
    POSITION_QUALITY_VALUE_FLAG flag;
    uint8_t Number_of_Beacons_in_Solution;
    uint8_t Number_of_Beacons_in_View;
    uint16_t Time_to_First_Fix;
    uint32_t EHPE;
    uint32_t EVPE;
    uint8_t HDOP;
    uint8_t VDOP;
} POSITION_QUALITY_VALUE;

typedef struct _LNS_NOTIFY_INDICATE_FLAG
{
    uint8_t location_and_speed_notify_enable: 1;
    uint8_t ln_cp_indicate_enable: 1;
    uint8_t navigation_enable: 1;
    uint8_t rfu: 5;
} LNS_NOTIFY_INDICATE_FLAG;

typedef struct _LOCATION_AND_SPEED_VALUE_FLAG
{
    uint16_t Instantaneous_Speed_Present: 1;
    uint16_t Total_Distance_Present: 1;
    uint16_t Location_Present: 1;
    uint16_t Elevation_Present: 1;
    uint16_t Heading_Present: 1;
    uint16_t Rolling_Time_Present: 1;
    uint16_t UTC_Time_Present: 1;
    uint16_t Position_Status: 2;
    uint16_t Speed_and_Distance_format: 1;
    uint16_t Elevation_Source: 2;
    uint16_t Heading_Source: 1;
    uint16_t RFU: 3;
} LOCATION_AND_SPEED_VALUE_FLAG;

/** Location and Speed Value */
typedef struct _LOCATION_AND_SPEED_VALUE
{
    LOCATION_AND_SPEED_VALUE_FLAG flag;
    uint16_t Instantaneous_Speed;
    uint32_t Total_Distance; //uint24
    int32_t Location_Latitude; //sint32
    int32_t Location_Longitute;
    int32_t Elevation;//sint24
    uint16_t Heading;
    uint8_t Rolling_Time;
    DATE_TIME UTC_Time;
} LOCATION_AND_SPEED_VALUE;

/** Navigation Value */
typedef struct _NAVIGATION_VALUE_FLAG
{
    uint16_t Remaining_Distance_Present: 1;
    uint16_t Remaining_Vertical_Distance_Present: 1;
    uint16_t Estimated_Time_of_Arrival_Present: 1;
    uint16_t Position_Status: 2;
    uint16_t Heading_Source: 1;
    uint16_t Navigation_Indicator_Type: 1;
    uint16_t Waypoint_Reached: 1;
    uint16_t Destination_Reached: 1;
    uint16_t RFUs: 7;
} NAVIGATION_VALUE_FLAG;

typedef struct _NAVIGATION_VALUE
{
    NAVIGATION_VALUE_FLAG flag;
    uint16_t Bearing;
    uint16_t Heading;
    uint32_t Reamining_Distance;
    int32_t Reamining_Vertical_Distance;
    DATE_TIME Estimated_Time_of_Arrival;
} NAVIGATION_VALUE;

/**
 * @brief CSC Control Point data, variable length during connection, max can reach 17 bytes.
 *
 * CSC Control Point data used to store the Control Point Command recieved from the client.
*/
typedef struct _TLNSControlPoint
{
    uint8_t    cur_length;            /**<  length of current LNS Control Point data, . */
    uint8_t    value[LNS_MAX_CTL_PNT_VALUE];            /**<  value of current LNS Control Point data, . */
} TLNSControlPoint, * PLNSControlPoint;

/** @defgroup LNS_Callback_Data LNS Callback Data
  * @brief LNS data struct for notification data to application.
  * @{
  */
typedef union _TLNS_CP_PARAMETER{
    uint32_t cumulative_total_distance; //uint24
    uint16_t mask_location_and_speed;
    uint8_t  navigation_control;
    uint16_t  number_of_desired_route; //request_name_of_route
    uint16_t  select_route_desired_route_number;
    uint8_t  set_fix_rate;
    int32_t  set_elevation; //sint24    
}TLNS_CP_PARAMETER;

typedef struct _TLNS_WRITE_MSG{
    TLN_CP_OPCODE opcode; //!< ref: @ref LNS_Control_Point_OpCodes
    TLNS_CP_PARAMETER cp_parameter;
}TLNS_WRITE_MSG;

typedef union _TLNS_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; //!< ref: @ref LNS_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref LNS_Read_Info
    TLNS_WRITE_MSG write;
}TLNS_UPSTREAM_MSG_DATA;

/** LNS service data to inform application */
typedef struct _TLNS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TLNS_UPSTREAM_MSG_DATA    msg_data;
} TLNS_CALLBACK_DATA;
/** @} */

/** @} End of LNS_Exported_Types */

/** @defgroup LNS_Exported_Functions LNS Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
uint8_t LNS_AddService(void* pFunc);
bool LNS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
bool LNS_LocationAndSpeedValueNotify( uint8_t ServiceId );
bool LNS_NavigationValueNotify( uint8_t ServiceId );
/** @} End of LNS_Exported_Functions */

/** @} End of LNS */

/** @} End of RTK_Profile_Module */

#ifdef __cplusplus
}
#endif

#endif // _HTS_SERVICE_DEF_H

