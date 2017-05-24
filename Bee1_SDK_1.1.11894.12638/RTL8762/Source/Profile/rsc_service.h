/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     rsc_service.h
  * @brief    Head file for using Running Speed and Cadence Service.
  * @details  RSC data structs and external functions declaration.
  * @author   ethan_su
  * @date     2014-10-10
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _RSC_SERVICE_DEF_H
#define _RSC_SERVICE_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"
#include "rsc_config.h"

/** @addtogroup RSC RSC
  * @brief RSC Profile
  * @{
  */

/** @defgroup RSC_Service RSC Service
  * @brief RSC Service
  * @{
  */ 

/** @defgroup RSC_Exported_Constants RSC Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup RSC_Application_Parameters RSC Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define RSC_PARAM_CSC_FEATURE               0x01
#define RSC_PARAM_INC_FLAG                  0x11
#define RSC_PARAM_SPEED                     0x12
#define RSC_PARAM_CADENCE                   0x13
#define RSC_PARAM_STRIDE_LENGTH             0x14
#define RSC_PARAM_TOTALDISTANCE             0x15
#define RSC_PARAM_CTL_PNT_PROG_CLR          0x17
#define RSC_PARAM_SENSOR_LOC                0x21
/** @} */

/** @defgroup RSC_Upstream_Message RSC Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup RSC_Read_Info RSC Read Info
  * @brief  Parameter for reading characteristic value.
  * @{
  */
#define RSC_READ_RSC_FEATURE                            1
#define RSC_READ_SENSOR_LOCATION                        2
/** @} */

/** @defgroup RSC_Notify_Indicate_Info RSC Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define RSC_NOTIFY_INDICATE_MEASUREMENT_ENABLE          1
#define RSC_NOTIFY_INDICATE_MEASUREMENT_DISABLE         2
#define RSC_NOTIFY_INDICATE_SC_CP_ENABLE                3
#define RSC_NOTIFY_INDICATE_SC_CP_DISABLE               4
/** @} */

/** @} End of RSC_Upstream_Message */

///@cond
/** @brief  Judge RSC Control Point operation is available or not. */
#define RSC_CTL_PNT_OPERATE_ACTIVE(x)                     \
    (((x >= RSC_CP_OPCODE_SET_CUMULATIVE) &&  \
      (x <= RSC_CP_OPCODE_REQ_SENS_LOC_LIST)) || \
     x == RSC_CP_OPCODE_RSP_CODE )
///@endcond

/** @} End of RSC_Exported_Constants */

/** @defgroup RSC_Exported_Types RSC Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */
  
/* Add all public types here */
/** @defgroup RSC_Callback_Data RSC Callback Data
  * @brief RSC data struct for notification data to application.
  * @{
  */
typedef union _TRSC_CP_PARAMETER{
    uint8_t  sensor_location_value;
    uint32_t cumulative_value;
}TRSC_CP_PARAMETER;

typedef struct _TRSC_WRITE_MSG{
    uint8_t opcode; //!< ref: @ref RSC_Control_Point_OpCodes
    TRSC_CP_PARAMETER cp_parameter;
}TRSC_WRITE_MSG;

typedef union _TRSC_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; //!< ref: @ref RSC_Notify_Indicate_Info
    uint8_t read_value_index; //!< ref: @ref RSC_Read_Info
    TRSC_WRITE_MSG write;
}TRSC_UPSTREAM_MSG_DATA;

/** RSC service data to inform application */
typedef struct _TRSC_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    TRSC_UPSTREAM_MSG_DATA    msg_data;
} TRSC_CALLBACK_DATA;
/** @} */

/** @} End of RSC_Exported_Types */

/** @defgroup RSC_Exported_Functions RSC Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
/* set RSC Service related data from application */
extern bool RSC_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
/* get RSC Service related data from application */
extern bool RSC_GetParameter( uint8_t param_type, void *value_ptr );
/* notify current RSC measurement data from application */
extern bool RSC_MeasValueNotify( uint8_t ServiceId );
/* indicate control point data response to client from application */
extern void RSC_CalibResutlIndicate( uint8_t serviceID, bool result);
/* add RSC Service to application */
extern uint8_t RSC_AddService(void* pFunc);
/** @} End of RSC_Exported_Functions */

/** @} End of RSC_Service */

/** @} End of RSC */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _RSC_DEF_H */
