/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     csc_service.h
  * @brief    Variables and interfaces for Cycling Speed and Cadence Service.
  * @details  CSC service data structs and functions.
  * @author   ethan_su
  * @date     2014-10-13
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _CSC_SERVICE_DEF_H
#define _CSC_SERVICE_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"
#include "csc_config.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup CSC CSC
  * @brief Cycling Speed and Cadence Service
  * @{
  */ 

/** @defgroup CSC_Exported_Constants CSC Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup CSC_Optional_Features CSC Optional Features
  * @brief CSC Service optional features bit mask
  * @{
  */ 

/** @defgroup CSC_Measurement_Flags CSC Measurement Flags
  * @brief CSC measurement 'Flags' present bit mask
  * @{
  */
#define CSC_INC_WHEEL_REVOL_MASK            (1)
#define CSC_INC_CRANK_REVOL_MASK            (1<<1)
#define CSC_INC_ALL_PRESENTS                (0x03)
/** @} */

/** @defgroup CSC_Features CSC Features
  * @brief CSC features bit mask
  * @{
  */ 
#define CSC_SUPPORT_WHEEL_REVOL_MASK        (1)
#define CSC_SUPPORT_CRANK_REVOL_MASK        (1<<1)
#define CSC_SUPPORT_MULTI_SENSOR_MASK       (1<<2)
#define CSC_ALL_FEATURE_SUPPORTED           (0x07)
/** @} */

/** @defgroup CSC_Sensor_Location CSC Sensor Location
  * @brief CSC supported sensor location  bit mask
  * @{
  */ 
#define CSC_SENS_LOC_OTHER_MASK             (1)
#define CSC_SENS_LOC_TOP_OF_SHOE_MASK       (1<<1)
#define CSC_SENS_LOC_IN_SHOE_MASK           (1<<2)
#define CSC_SENS_LOC_HIP_MASK               (1<<3)
#define CSC_SENS_LOC_FRONT_WHEEL_MASK       (1<<4)
#define CSC_SENS_LOC_LEFT_CRANK_MASK        (1<<5)
#define CSC_SENS_LOC_RIGHT_CRANK_MASK       (1<<6)
#define CSC_SENS_LOC_LEFT_PEDAL_MASK        (1<<7)
#define CSC_SENS_LOC_RIGHT_PEDAL_MASK       (1<<8)
#define CSC_SENS_LOC_FRONT_HUB_MASK         (1<<9)
#define CSC_SENS_LOC_REAR_DROPOUT_MASK      (1<<10)
#define CSC_SENS_LOC_CHAINSTAY_MASK         (1<<11)
#define CSC_SENS_LOC_REAR_WHEEL_MASK        (1<<12)
#define CSC_SENS_LOC_REAR_HUB_MASK          (1<<13)
#define CSC_SENS_LOC_CHEST_MASK             (1<<14)
#define CSC_ALL_SENS_LOC_SUPPORTED          (0x7FFF)
/** @} */

/** @} End of CSC_Optional_Features */

/** @defgroup CSC_Application_Parameters CSC Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define CSC_PARAM_CSC_FEATURE               0x01
#define CSC_PARAM_INC_FLAG                  0x11
#define CSC_PARAM_WHEEL_REVOL               0x12
#define CSC_PARAM_WHEEL_EVT_TIME            0x13
#define CSC_PARAM_CRANK_REVOL               0x14
#define CSC_PARAM_CRANK_EVT_TIME            0x15

#define CSC_PARAM_CTL_PNT_PROG_CLR          0x17

#define CSC_PARAM_SENSOR_LOC                0x21
/** @} */

///@cond
/** @brief  CSC service related UUIDs. */
#define GATT_UUID_CSC                       0x1816
#define GATT_UUID_CHAR_CSC_MEASUREMENT      0x2A5B
#define GATT_UUID_CHAR_CSC_FEATURE          0x2A5C
#define GATT_UUID_CHAR_SENSOR_LOCATION      0x2A5D
#define GATT_UUID_CHAR_SC_CONTROL_POINT     0x2A55
///@endcond

/** @defgroup CSC_Upstream_Message CSC Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup CSC_Read_Info CSC Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define CSC_READ_CSC_FEATURE                                1
#define CSC_READ_SENSOR_LOCATION                            2
/** @} */

/** @defgroup CSC_Notify_Indicate_Info CSC Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define CSC_NOTIFY_INDICATE_MEASUREMENT_ENABLE              1
#define CSC_NOTIFY_INDICATE_MEASUREMENT_DISABLE             2
#define CSC_NOTIFY_INDICATE_SC_CP_ENABLE                    3
#define CSC_NOTIFY_INDICATE_SC_CP_DISABLE                   4
/** @} */

/** @} End of CSC_Upstream_Message */

/** @defgroup CSC_Control_Point CSC Control Point
  * @brief  Control Point
  * @{
  */

/** @defgroup CSC_Control_Point_OpCodes CSC Control Point OpCodes
  * @brief  Control Point OpCodes
  * @{
  */
#define CSC_CP_OPCODE_RESERVED              0x00
#define CSC_CP_OPCODE_SET_CUMULATIVE        0x01
#define CSC_CP_OPCODE_UPDATE_SENS_LOC       0x03
#define CSC_CP_OPCODE_REQ_SENS_LOC_LIST     0x04
#define CSC_CP_OPCODE_RSP_CODE              0x10
/** @} */

/** @defgroup CSC_Control_Point_Response_Codes CSC Control Point Response Codes
  * @brief  Control Point Response Codes
  * @{
  */
#define CSC_CP_RSPCODE_RESERVED             0x00
#define CSC_CP_RSPCODE_SUCCESS              0x01
#define CSC_CP_RSPCODE_OPCODE_UNSUPPORT     0x02
#define CSC_CP_RSPCODE_INVALID_PARAMETER    0x03
#define CSC_CP_RSPCODE_OPERATION_FAILED     0x04
/** @} */

///@cond
/** @brief  Judge CSC Control Point operation is available or not. */
#define CSC_CTL_PNT_OPERATE_ACTIVE(x)                     \
    ( (x == CSC_CP_OPCODE_SET_CUMULATIVE)  || \
      (x == CSC_CP_OPCODE_UPDATE_SENS_LOC)  || \
      (x == CSC_CP_OPCODE_REQ_SENS_LOC_LIST) || \
      (x == CSC_CP_OPCODE_RSP_CODE) )
///@endcond

/** @} End of CSC_Control_Point */

/**
 * @brief CSC sensor locations.
 *
 * All CSC sensor locations defined in spec.
*/
typedef enum
{
    CSC_SENSOR_LOC_OTHER            = 0,
    CSC_SENSOR_LOC_TOP_OF_SHOE      = 1,
    CSC_SENSOR_LOC_IN_SHOE          = 2,
    CSC_SENSOR_LOC_HIP              = 3,
    CSC_SENSOR_LOC_FRONT_WHEEL      = 4,
    CSC_SENSOR_LOC_LEFT_CRANK       = 5,
    CSC_SENSOR_LOC_RIGHT_CRANK      = 6,
    CSC_SENSOR_LOC_LEFT_PEDAL       = 7,
    CSC_SENSOR_LOC_RIGHT_PEDAL      = 8,
    CSC_SENSOR_LOC_FRONT_HUB        = 9,
    CSC_SENSOR_LOC_REAR_DROPOUT     = 10,
    CSC_SENSOR_LOC_CHAINSTAY        = 11,
    CSC_SENSOR_LOC_REAL_WHEEL       = 12,
    CSC_SENSOR_LOC_REAL_HUB         = 13,
    CSC_SENSOR_LOC_CHEST            = 14,
    CSC_SENSOR_LOC_MAX
} TCSCSensorLocation;

///@cond
/** @brief  Error codes defined in CSC service. */
#define CSC_ERR_PROC_ALREADY_IN_PROG        0x80
#define CSC_ERR_CCCD_IMPROPERLY_CFG         0x81

/** @brief  Max bytes of CSC Measurement data. */
#define CSC_MAX_MEASUREMENT_VALUE           11
/** @brief  Max bytes of CSC Control Point data. */
#define CSC_MAX_CTL_PNT_VALUE               18
///@endcond

/** @} End of CSC_Exported_Constants */

/** @defgroup CSC_Exported_Types CSC Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */
  
/* Add all public types here */
/** @defgroup CSC_Callback_Data CSC Callback Data
  * @brief CSC data struct for notification data to application.
  * @{
  */
typedef union _TCSC_CP_PARAMETER{
    uint8_t  sensor_location_value;
    uint32_t cumulative_value;
   
}TCSC_CP_PARAMETER;

typedef struct _TCSC_WRITE_MSG{
    uint8_t opcode; //!< ref: @ref CSC_Control_Point_OpCodes
    TCSC_CP_PARAMETER cp_parameter;
}TCSC_WRITE_MSG;

typedef union _TCSC_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; //!< ref: @ref CSC_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref CSC_Read_Info
    TCSC_WRITE_MSG write;
}TCSC_UPSTREAM_MSG_DATA;

/** CSC service data to inform application */
typedef struct _TCSC_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TCSC_UPSTREAM_MSG_DATA    msg_data;
} TCSC_CALLBACK_DATA;
/** @} */

/** @brief CSC measurement data, variable length during connection, max can reach 11 bytes */
typedef struct _CSCMeasurement
{
    uint8_t    cur_length;          /**<  length of current CSC measurement data. */
    uint8_t    value[CSC_MAX_MEASUREMENT_VALUE];/**<  value of current CSC measurement data. */
} TCSCMeasurement, * PCSCMeasurement;

/**
 * @brief CSC Control Point data, variable length during connection, max can reach 17 bytes.
 *
 * CSC Control Point data used to store the Control Point Command recieved from the client.
*/
typedef struct _CSCControlPoint
{
    uint8_t    cur_length;            /**<  length of current CSC Control Point data, . */
    uint8_t    value[CSC_MAX_CTL_PNT_VALUE];            /**<  value of current CSC Control Point data, . */
} TCSCControlPoint, * PCSCControlPoint;
/** @} End of CSC_Exported_Types */

/** @defgroup CSC_Exported_Functions CSC Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
/****************************************************************************************************************
* exported functions other .c files may use all defined here.
****************************************************************************************************************/
/* set CSC Service related data from application */
extern bool CSC_SetParameter( uint8_t param_type,  uint8_t len, void *value_ptr);
/* get CSC Service related data from application */
extern bool CSC_GetParameter( uint8_t param_type, void *value_ptr );
/* notify current CSC measurement data from application */
extern bool CSC_MeasValueNotify( uint8_t ServiceId );
/* indicate control point data response to client from application */
extern bool CSC_CtlPntIndicate( uint8_t ServiceId, uint8_t OpCode, uint8_t RspCode );
/* add CSC Service to application */
extern uint8_t CSC_AddService(void* pFunc);
/** @} End of CSC_Exported_Functions */

/** @} End of CSC */

/** @} End of RTK_Profile_Module */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _CSC_DEF_H */

