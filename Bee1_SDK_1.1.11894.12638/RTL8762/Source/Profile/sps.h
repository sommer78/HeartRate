/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     sps.h
  * @brief    Head file for using scan parameters service.
  * @details  SPS data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _SPS_H_
#define _SPS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup SPS SPS
  * @brief Scan Parameters Service
  * @{
  */ 

/** @defgroup SPS_Exported_Constants SPS Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */      
/** @defgroup SPS_Application_Parameters SPS Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define SPS_PARAM_SCAN_REFRESH                  1
/** @} */

///@cond
/** @brief  ScanParameters service related UUIDs. */
#define GATT_UUID_SPS                           0x1813
#define GATT_UUID_CHAR_SCAN_INTERVAL_WINDOW     0x2A4F
#define GATT_UUID_CHAR_SCAN_REFRESH             0x2A31

/** @brief  defines for Characteristic Index in Scan parameters Service */
#define GATT_SVC_SPS_SCAN_INTERVAL_INDEX        2 /**<  @brief  Index for Scan Interval chars's value      */
#define GATT_SVC_SPS_SCAN_REFRESH_INDEX         4 /**<  @brief  Index for Scan refresh chars's value      */
#define GATT_SVC_SPS_SCAN_REFRESH_CCCD_INDEX    5/**<  @brief  CCCD Index for Scan Interval chars's value      */
///@endcond

/** @defgroup SPS_Upstream_Message SPS Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */

/** @defgroup SPS_Write_Info SPS Write Info
  * @brief  Parameter for writing characteristic value.
  * @{ 
  */
#define SPS_WRITE_SCAN_INTERVAL_WINDOW              1
/** @} */

/** @defgroup SPS_Notify_Indicate_Info SPS Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define SPS_NOTIFY_INDICATE_SCAN_REFRESH_ENABLE     1
#define SPS_NOTIFY_INDICATE_SCAN_REFRESH_DISABLE    2
/** @} */

/** @} End of SPS_Upstream_Message */

/** @} End of SPS_Exported_Constants */

/** @defgroup SPS_Exported_Types SPS Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */
/* Add all public types here */
typedef struct _TSPS_SCAN_INTERVAL_WINDOW
{
    uint16_t    scan_interval;
    uint16_t    scan_window;
} TSPS_SCAN_INTERVAL_WINDOW;

/** @defgroup SPS_Callback_Data SPS Callback Data
  * @brief SPS data struct for notification data to application.
  * @{
  */
typedef union _TSPS_WRITE_PARAMETER
{
    TSPS_SCAN_INTERVAL_WINDOW scan;
    
}TSPS_WRITE_PARAMETER;

typedef struct _TSPS_WRITE_MSG{
    uint8_t write_type;
    TSPS_WRITE_PARAMETER write_parameter;
}TSPS_WRITE_MSG;

typedef union _TSPS_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    TSPS_WRITE_MSG write;
}TSPS_UPSTREAM_MSG_DATA;

typedef struct _TSPS_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;                    /**<  @brief EventId defined upper */
    TSPS_UPSTREAM_MSG_DATA    msg_data;
} TSPS_CALLBACK_DATA;
/** @} */

/** @} End of SPS_Exported_Types */

/** @defgroup SPS_Exported_Functions SPS Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
/* set SPS Service related data from application */
bool SPS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr);
/* add SPS Service to application */
uint8_t SPS_AddService(void* pFunc);
bool SPS_ScanIntervalWindowValueNotify( uint8_t ServiceId, TSPS_SCAN_INTERVAL_WINDOW scanIntervalWindow);
/** @} End of SPS_Exported_Functions */

/** @} End of SPS */

/** @} End of RTK_Profile_Module */
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* _SPS_H_ */

