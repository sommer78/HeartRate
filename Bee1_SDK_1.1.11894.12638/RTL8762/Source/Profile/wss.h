/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     wss.h
  * @brief    Head file for using weight scale service.
  * @details  Weight scale data types and external functions declaration.
  * @author   Vera
  * @date     2015-10-20
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _WSS_H_
#define _WSS_H_

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */


/* Add Includes here */
#include "profileAPI.h"
#include "wss_config.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup WSS WSS
  * @brief Weight Scale Service
  * @{
  */	
	
/** @defgroup WSS_Application_Parameters WSS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG                   1
#define WSS_PARAM_MEASUREMENT_WEIGHT_VALUE                       2
#define WSS_PARAM_MEASUREMENT_TIME_STAMP                         3
#define	WSS_PARAM_MEASUREMENT_USERID                             4
#define WSS_PARAM_MEASUREMENT_BMI                                5
#define	WSS_PARAM_MEASUREMENT_HEIGHT_VALUE                       6
#define WSS_PARAM_FEATURE_READ_CHAR_VAL                          7
/** @} End of WSS_Application_Parameters*/

///@cond
/** @brief  Index of each characteristic in weight scale service database. */
#define WEIGHT_SCALE_SERVICE_CHAR_FEATURE_READ_INDEX          0x02
#define WEIGHT_SCALE_SERVICE_CHAR_MEASUREMENT_INDICATE_INDEX  0x04
#define WEIGHT_SCALE_SERVICE_CHAR_INDICATE_CCCD_INDEX         (WEIGHT_SCALE_SERVICE_CHAR_MEASUREMENT_INDICATE_INDEX + 1)
///@endcond

/** @defgroupWSS_Measurement_Flag WSS Measurement Flag
  * @{
  */
#define WSS_FLAG_MEASUREMENT_UINT_BIT                       BIT0
#define WSS_FLAG_MEASUREMENT_TIMESTAMP_PRESENT_BIT          BIT1
#define WSS_FLAG_MEASUREMENT_USERID_PRESNET_BIT             BIT2
#define WSS_FLAG_MEASUREMENT_BMI_PRESNET_BIT                BIT3

/** @} */



/** @defgroup WSS_Service_Read_Info WSS Service Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define WEIGHT_SCALE_READ_FEATURE                   1
/** @} End of WSS_Service_Read_Info*/



/** @defgroup WSS_Service_Notify_Indicate_Info WSS Service Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define WSS_INDICATE_WEIGHT_MEASUREMENT_ENABLE     1
#define WSS_INDICATE_WEIGHT_MEASUREMENT_DISABLE    2
        
/** @} */



///@cond
/** @brief The Maximum Length of Weight Measurement Value*/
#define WSS_MEASUREMENT_VALUE_MAX_LEN  15
///@endcond

/** @defgroup WSS_Service_Exported_Types WSS Service Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/** @defgroup TWS_UPSTREAM_MSG_DATA TWS UPSTREAM MSG DATA
  * @brief Weight scale service callback message content.
  * @{
  */
typedef union _TWS_UPSTREAM_MSG_DATA
{
    uint8_t notification_indication_index; //!< ref: @ref WSS_Service_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref WSS_Service_Read_Info
}TWS_UPSTREAM_MSG_DATA;
/** @} End of TWS_UPSTREAM_MSG_DATA */

/** @defgroup TWS_CALLBACK_DATA TWS CALLBACK DATA
  * @brief Weight scale service data to inform application.
  * @{
  */
typedef struct _TWS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TWS_UPSTREAM_MSG_DATA    msg_data;
}TWS_CALLBACK_DATA;
/** @} End of TWS_CALLBACK_DATA */


/** @defgroup WEIGHT_SCALE_FEATURE_VALUE WEIGHT SCALE FEATURE VALUE
  * @brief Weight Scale Feature Value.
  * @{
  */
typedef struct _WEIGHT_SCALE_FEATURE_VALUE
{
    uint32_t Time_Stamp_Supported: 1;
    uint32_t Multiple_Users_Supported: 1;
    uint32_t BMI_Supported: 1;
    uint32_t Weight_Measurement_Resolution: 4;
    uint32_t Height_Measurement_Resolution: 3;
    uint32_t Rfu: 22;
} WEIGHT_SCALE_FEATURE_VALUE;
/** @} End of WEIGHT_SCALE_FEATURE_VALUE */

/** @defgroup WEIGHT_MEASUREMENT_VALUE_FLAG WEIGHT MEASUREMENT VALUE FLAG
  * @brief Weight Measurement Value Flag.
  * @{
  */
typedef struct _WEIGHT_MEASUREMENT_VALUE_FLAG
{
    uint8_t MEASUREMENT_UNITS: 1;
    uint8_t Time_Stamp_PRESENT: 1;
    uint8_t USER_ID_PRESENT: 1;
    uint8_t BMI_AND_HEIGHT_PRESENT: 1;
    uint8_t Rfu: 4;
} WEIGHT_MEASUREMENT_VALUE_FLAG;
/** @} End of WEIGHT_MEASUREMENT_VALUE_FLAG */

/** @defgroup WSS_TIME_STAMP WSS TIME STAMP
  * @brief Weight Scale Feature Time Stamp.
  * @{
  */
typedef struct _WSS_TIME_STAMP
{
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Day;
    uint8_t  Hours;
    uint8_t  Minutes;
    uint8_t  Seconds;
} WSS_TIME_STAMP;
/** @} End of WSS_TIME_STAMP */

/** @defgroup WSS_MEASUREMENT WSS MEASUREMENT
  * @brief Weight Measurement Value.
  * @{
  */
typedef struct _WSS_MEASUREMENT
{
    WEIGHT_MEASUREMENT_VALUE_FLAG flag;
    uint16_t WeightSIValue; 
    uint16_t WeightImperialValue; 
    WSS_TIME_STAMP  TimeStamp;
    uint8_t UserID;
	uint16_t BMI;  
	uint16_t HeightSIValue;
	uint16_t HeightImperialValue;
} WSS_MEASUREMENT;
/** @} End of WSS_MEASUREMENT */

/** @} End of WSS_Service_Exported_Types */



/** @defgroup WSS_Service_Exported_Functions WSS Service Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
extern uint8_t Weight_Scale_Service_AddService(void* pFunc);
extern bool Weight_Scale_Service_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
extern bool Weight_Scale_Service_Measurement_Indicate( uint8_t ServiceId);
/** @} End of WSS_Service_Exported_Functions */

/** @} End of WSS */

/** @} End of RTK_Profile_Module */

#ifdef __cplusplus
}
#endif

#endif /* _WSS_H_ */
