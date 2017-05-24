/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gls.h
  * @brief    Head file for using glucose service.
  * @details  Glusose data types and external functions declaration.
  * @author   bill
  * @date     2015-6-8
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _GLS_H
#define _GLS_H

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */    
#include "profile.h"
#include "profileAPI.h"
#include "gls_config.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup GLS GLS
  * @brief Battery Service
  * @{
  */ 

/** @defgroup GLS_Exported_Constants GLS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup GLS_Feature GLS Feature
  * @brief  glucose feature bits.
  * @{
  */
#define GLC_FEATURES_LOW_BATTERY                0x0001
#define GLC_FEATURES_SENSOR_MALFUNCTION         0x0002
#define GLC_FEATURES_SENSOR_SAMPLE_SIZE         0x0004
#define GLC_FEATURES_SENSOR_STRIP_INS_ERROR     0x0008
#define GLC_FEATURES_SENSOR_STRIP_TYPE_ERROR    0x0010
#define GLC_FEATURES_SENSOR_RESULT_HIGH_LOW     0x0020
#define GLC_FEATURES_SENSOR_TEMP_HIGH_LOW       0x0040
#define GLC_FEATURES_SENSOR_READ_INTERRUPT      0x0080
#define GLC_FEATURES_GENERAL_DEVICE_FAULT       0x0100
#define GLC_FEATURES_TIME_FAULT                 0x0200
#define GLC_FEATURES_MULTIPLE_BOND              0x0400

/** GLC features supported: */
#define GLC_FEATURES          (GLC_FEATURES_LOW_BATTERY    |  \
                               GLC_FEATURES_SENSOR_STRIP_INS_ERROR)
/** @} */

/** @defgroup GLS_Application_Parameters GLS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
typedef enum _GLS_Param
{
    /** glucose measurement parameters */
    GLS_PARAM_GLC_MS_FLAG = 0x01,
    GLS_PARAM_GLC_MS_BASE_TIME,
    GLS_PARAM_GLC_MS_TIME_OFFSET,
    GLS_PARAM_GLC_MS_CONCENTRATION,
    GLS_PARAM_GLC_MS_CONCENTRATION_UNITS,
    GLS_PARAM_GLC_MS_TYPE_SAMPLE_LOCATION,
    GLS_PARAM_GLC_MS_SENSOR_STATUS_ANNUNCIATION,
    
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
    /** glucose measurement context parameters */
    GLS_PARAM_GLC_MS_CT_FLAG,
    GLS_PARAM_GLC_MS_CT_CARBOHYDRATE_ID,
    GLS_PARAM_GLC_MS_CT_CARBOHYDRATE,
    GLS_PARAM_GLC_MS_CT_MEAL,
    GLS_PARAM_GLC_MS_CT_TESTER_HEALTH,
    GLS_PARAM_GLC_MS_CT_EXERCISE_DURATION,
    GLS_PARAM_GLC_MS_CT_EXERCISE_INTENSITY,
    GLS_PARAM_GLC_MS_CT_MEDICATION_ID,
    GLS_PARAM_GLC_MS_CT_MEDICATION,
    GLS_PARAM_GLC_MS_CT_MEDICATION_UNITS,
    GLS_PARAM_GLC_MS_CT_HbA1c,
#endif
    
    /** Parameters that can be get by application */
    GLS_PARAM_GLC_FEATURES,
    GLS_PARAM_CTL_PNT_PROG_CLR,
    GLS_PARAM_RECORD_NUM,
    GLS_PARAM_RECORD_SEQ_NUM
} TGLSParam;
/** @} */

///@cond
/** @brief service related UUIDs. */
#define GATT_UUID_GLUCOSE                           0x1808
#define GATT_UUID_CHAR_GLC_MEASUREMENT              0x2A18
#define GATT_UUID_CHAR_GLC_MEASUREMENT_CONTEXT      0x2A34
#define GATT_UUID_CHAR_GLC_FEATURE                  0x2A51  
#define GATT_UUID_CHAR_GLC_RACP                     0x2A52
///@endcond

/** @defgroup GLS_Upstream_Message GLS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
  
/** @defgroup GLS_Read_Info GLS Read Info
  * @brief  Parameter for read characteristic value.
  * @{
  */
#define GLS_EVT_READ_FEATURE                            0x01
/** @} */

/** @defgroup GLS_Notify_Indicate_Info GLS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define GLS_EVT_GLC_MEASUREMENT_NOTIFY_ENABLE           0x01
#define GLS_EVT_GLC_MEASUREMENT_NOTIFY_DISABLE          0x02
#define GLS_EVT_GLC_MEASUREMENT_CONTEXT_NOTIFY_ENABLE   0x03
#define GLS_EVT_GLC_MEASUREMENT_CONTEXT_NOTIFY_DISABLE  0x04
#define GLS_EVT_GLC_RACP_INDICATE_ENABLE                0x05
#define GLS_EVT_GLC_RACP_INDICATE_DISABLE               0x06
/** @} */

/** @} End of GLS_Upstream_Message */

/** @defgroup GLS_Control_Point GLS Control Point
  * @brief  glucose record access control point
  * @{
  */

/** @brief  Glucose Record Access Control Point OpCodes. */
typedef enum _GlcCtrlPointOpcode
{
    GLC_RACP_OPCODE_RESERVED = 0x00,
    GLC_RACP_OPCODE_REPORT_RECS = 0x01,
    GLC_RACP_OPCODE_DELETE_RECS = 0x02,
    GLC_RACP_OPCODE_ABORT_OPERATION = 0x03,
    GLC_RACP_OPCODE_REPORT_NBR_OF_RECS = 0x04,
    GLC_RACP_OPCODE_NBR_OF_RECS_RESP = 0x05,
    GLC_RACP_OPCODE_RESP_CODE = 0x06
} TGlcCtrlPointOpcode;

/** @brief  Check glucose Record Access Control Point operation is available or not. */
#define GLC_RACP_OPERATION_ACTIVE(x)        \
    ((x >= GLC_RACP_OPCODE_REPORT_RECS) &&  \
     (x <= GLC_RACP_OPCODE_RESP_CODE))

/** @brief  Glucose Record Access Control Point Operator. */
typedef enum _GlcCtrlPointOperator
{
    GLC_RACP_OPERATOR_NULL = 0x00,
    GLC_RACP_OPERATOR_ALL_RECS = 0x01,
    GLC_RACP_OPERATOR_LT_EQ = 0x02,
    GLC_RACP_OPERATOR_GT_EQ = 0x03,
    GLC_RACP_OPERATOR_RANGE = 0x04,
    GLC_RACP_OPERATOR_FIRST = 0x05,
    GLC_RACP_OPERATOR_LAST = 0x06
} TGlcCtrlPointOperator;

/** @brief Filter Type values. */
typedef enum _GlcCtrlPointFilterType
{
    GLC_RACP_FILTER_TYPE_RESERVED = 0x00,
    GLC_RACP_FILTER_TYPE_SEQ_NBR = 0x01,
    GLC_RACP_FILTER_TYPE_TIME = 0x02
} TGlcCtrlPointFilterType;

/** @brief  Glucose Record Access Control Point Response Codes. */
typedef enum _GlcCtrlPointRespCodes
{
    GLC_RACP_RESP_RESERVED = 0x00,
    GLC_RACP_RESP_SUCCESS = 0x01,
    GLC_RACP_RESP_OPCODE_NOT_SUPPORTED = 0x02,
    GLC_RACP_RESP_INVALID_OPERATOR = 0x03,
    GLC_RACP_RESP_OPERATOR_NOT_SUPPORTED = 0x04,
    GLC_RACP_RESP_INVALID_OPERAND = 0x05,
    GLC_RACP_RESP_NO_RECS_FOUND = 0x06,
    GLC_RACP_RESP_ABORT_UNSUCCESSFUL = 0x07,
    GLC_RACP_RESP_PROC_NOT_COMPLETED = 0x08,
    GLC_RACP_RESP_OPERAND_NOT_SUPPORTED = 0x09
} TGlcCtrlPointRespCodes;

/** @brief profile specific attribute protocol application error codes */
#define GLC_ERR_PROC_ALREADY_IN_PROGRESS                0x80
#define GLC_ERR_CCCD_IMPROPERLY_CONFIGURED              0x81
/** @} */

/** @brief concentration unit */
#define GLC_FLAGS_UNITS_MOL_L_ON                        1
#define GLC_FLAGS_UNITS_KG_L_ON                         0

/** @brief medication unit */
#define GLC_FLAGS_MEDICATION_UNITS_LITERS_ON            1
#define GLC_FLAGS_MEDICATION_UNITS_KG_ON                0

/** GLC sensor status annunciation bits */
#define GLC_SS_ANNUNC_LOW_BATTERY                       0x0001

/** special short float values */
#define SFLOAT_VALUE_NaN                                0x07ff    /**< not a number             */
#define SFLOAT_VALUE_NRes                               0x0800    /**< not at this resolution   */
#define SFLOAT_VALUE_PlusINFINITY                       0x07fe    /**< + INFINITY               */
#define SFLOAT_VALUE_MinusINFINITY                      0x0802    /**< - INFINITY               */
#define SFLOAT_VALUE_RFU                                0x0801    /**< reserved for future use  */

/** @brief RACP database parameters */
#define GLC_RACP_DATABASE_SIZE                          (GLC_RACP_MAX_NBR_OF_STORED_RECS + 1)
#define GLC_RACP_INIT_SEQ_NBR_DEFAULT                   0

/** @} End of GLS_Exported_Constants */

/** @defgroup GLS_Exported_Types GLS Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/* Add all public types here */

/** GLC measurement flag bits */
typedef struct _GLCMeasurementFlag
{
    uint8_t time_offset: 1;  /**< time offset         */
    uint8_t con_ts_loc: 1;  /**< concentration, time/sample location */
    uint8_t con_units: 1;  /**< 0: kg/L, 1: mol/L        */
    uint8_t ss_annuciation: 1;  /**< sensor status annunciation */
    uint8_t ctxt_info_follows: 1;  /**< context information  */
    uint8_t rfu: 3;
} TGLCMeasurementFlag;

/** GLC measurement context flag bits */
typedef struct _GLCMsrCtxtFlag
{
    uint8_t carbohydrate: 1;  /**< Carbohydrates ID and field  */
    uint8_t meal: 1;  /**< Meal ID and field           */
    uint8_t tester_health: 1;  /**< Tester-Health field         */
    uint8_t excercise: 1;  /**< Exercise Duration and Intensity field */
    uint8_t medication: 1; /**< Medication ID and field     */
    uint8_t medication_units: 1;  /**< Medication units 0:kg, 1:liter */
    uint8_t HbA1c: 1;  /**< HbA1c field                 */
    uint8_t ext_flags: 1;  /**< extended flag               */
} TGLCMsrCtxtFlag;

typedef uint8_t     TIMESTAMP[7];
typedef uint8_t     SFLOAT[2];         /* 4 bit (MSB) exponent, 12 bit mantissa */

typedef struct _GLCMeasurementValue
{
    TGLCMeasurementFlag   flags;
    uint16_t     seq_num;
    TIMESTAMP  base_time;
#if (GLC_INCLUDE_TIME_OFFSET)
    int16_t      time_offset; //minutes
#endif
#if (GLC_INCLUDE_CONC_TS_LOC)
    SFLOAT     concentration;
    uint8_t    type_sample_location;
#endif
#if (GLC_INCLUDE_SS_ANNUNC)
    uint16_t    ss_annunciation;
#endif
} TGLCMeasurementValue, * PGLCMeasurementValue;

typedef struct _GLCMeasurementContext
{
    TGLCMsrCtxtFlag    flags;
    uint16_t   seq_num;
#if (GLC_INCLUDE_EXT_FLAGS)
    uint8_t    ext_flags;
#endif
#if (GLC_INCLUDE_CARBOHYDRATE)
    uint8_t    carbohydrate_ID;
    SFLOAT     carbohydrate;
#endif
#if (GLC_INCLUDE_MEAL)
    uint8_t    meal;
#endif
#if (GLC_INCLUDE_TESTER_HEALTH)
    uint8_t    tester_health;
#endif
#if (GLC_INCLUDE_EXCERCISE)
    uint16_t   exercise_duration;
    uint8_t    exercise_intensity;
#endif
#if (GLC_INCLUDE_MEDICATION)
    uint8_t    medication_ID;
    SFLOAT     medication;
#endif
#if (GLC_INCLUDE_HbA1c)
    SFLOAT     HbA1c;
#endif
} TGLCMeasurementContext, * PGLCMeasurementContext;

typedef struct _GLCControlPoint
{
    TGlcCtrlPointOpcode op_code;
    TGlcCtrlPointOperator op; /**< operator */
    uint8_t operand[18];
} TGLCControlPoint, * PTGLCControlPoint;

typedef struct _PatientRecord
{
    TGLCMeasurementValue glc_measurement_value;
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
    TGLCMeasurementContext glc_measurement_context;
#endif  
} TPatientRecord, *PTPatientRecord;

typedef struct _RecordDataBase
{
    TPatientRecord records[GLC_RACP_DATABASE_SIZE];
    int record_num;
/** 
    The tail points to an empty record who plays a role as a guard.
    1. empty tail == head
    2. one   (tail - head + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE == 1
    3. full (head - tail + GLC_RACP_DATABASE_SIZE) % GLC_RACP_DATABASE_SIZE == 1
*/
    int head;
    int tail;
    uint16_t seq_num; /**< sequence number of latest record */
} TRecordDataBase, *PTRecordDataBase;

typedef struct _GLCRACP
{
    TGLCControlPoint ctrl_point;
    int cp_length; /**< length of current operand of control point */
    TRecordDataBase record_db;
} TGLCRACP, * PTGLCRACP;

/** Flags indicate CCCD */
typedef struct _GLCNotifyIndicateFlag
{
    uint8_t GLC_measurement_notify_enable: 1;
    uint8_t GLC_measurement_context_notify_enable: 1;
    uint8_t GLC_RACP_indicate_enable: 1;
    uint8_t rfu: 5;
} TGLCNotifyIndicateFlag;

/** @defgroup GLS_Callback_Data GLS Callback Data
  * @brief GLS data struct for notification data to application.
  * @{
  */
typedef union _TGLSUpstreamMsgData
{
    uint8_t notify_indicate_index;
    uint8_t read_value_index;
    TGLCControlPoint write;
} TGLSUpstreamMsgData;

typedef struct _TGLS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TGLSUpstreamMsgData    msg_data;
} TGLS_CALLBACK_DATA;
/** @} */

/** @} End of GLS_Exported_Types */

/** @defgroup GLS_Exported_Functions GLS Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
/** add GLS from application */
extern uint8_t GLS_AddService(void* pFunc);
/** set GLS related data from application */
extern bool GLS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr );
/** get GLS related data from application */
extern bool GLS_GetParameter( uint8_t param_type, uint8_t *len, void *value_ptr );
/** notify current RSC measurement data from application */
extern bool GLS_GlcMeasurementNotify( uint8_t ServiceId, int index );
#if GLC_MEASUREMENT_CONTEXT_SUPPORT
/** notify current RSC measurement data from application */
extern bool GLS_GlcMeasurementContextNotify( uint8_t ServiceId, int index );
#endif
/** indicate control point data response to client from application */
extern bool GLS_RACPResponse( uint8_t service_ID, uint8_t rsp_code);
extern bool GLS_RACPNumResponse( uint8_t service_ID, uint16_t num);
/** prepare a new record in database */
extern void GLS_PrepareNewRecord(void);
/** push a new record into database */
extern void GLS_PushNewRecord(void);
/** report records sub procedure */
extern bool GLS_ReportRecordsTask(uint8_t service_ID);
/** calculate user facing time by adding basetime and timeoffset */
extern void UserFaceTime(TIMESTAMP time_in, int16_t time_offset, TIMESTAMP time_out);
/** abort RACP procedure by app */
void GLS_AbortRACPProcedure(void);
/** @} End of GLS_Exported_Functions */

/** @} End of GLS */

/** @} End of RTK_Profile_Module */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GLUCOSE_SERVICE_H */
