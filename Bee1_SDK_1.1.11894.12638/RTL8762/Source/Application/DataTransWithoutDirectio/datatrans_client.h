/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     simple_ble_client.h
  * @brief    Head file for using Simple BLE Client.
  * @details  Simple BLE data structs and external functions declaration.
  * @author   ethan_su
  * @date     2015-08-20
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _DT_BLE_CLIENT_H_
#define _DT_BLE_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"
#include "profile_client.h"
#include "datatrans_profile.h"

/** @addtogroup RTK_Client_Module RTK Client Module
  * @{
  */

/** @defgroup SIMP SIMP
  * @brief Simple BLE Client
  * @{
  */

/** @defgroup SIMP_Application_Parameters SIMP Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */

/** @defgroup TSimpHandleType TSimpHandleType
  * @{ Handle cache for intrested UUIDs
  */
typedef enum
{
    HDL_DT_SRV_START,           // start handle of simple ble service
    HDL_DT_SRV_END,             // end handle of simple ble service
    HDL_DT_V1_WRITE_NOTIFY,           // V3 notify value handle
    HDL_DT_V1_WRITE_NOTIFY_END,       // V3 notify value end handle
    HDL_DT_V1_WRITE_NOTIFY_CCCD,      // V3 notify CCCD handle
    HDL_DT_CACHE_LEN
} TDtHandleType;
/** End of TSimpHandleType * @} */

/** @defgroup TSimpDiscState TSimpDiscState
  * @{ used to inform app the discovery procedure is done or pending
  */
typedef enum
{
    DT_DISC_DONE,
    DT_DISC_PENDING
} TDtDiscState, *PDtDiscState;
/** End of TSimpDiscState
  * @}
  */

/** @defgroup TSimpDiscHdlInfo TSimpDiscHdlInfo
  * @{ handle information found in discovery procedure
  */
typedef struct
{
    TDtHandleType  handle_type;
    uint16_t        handle_value;
} TDtDiscHdlInfo;
/** End of TSimpDiscHdlInfo
  * @}
  */

/** @defgroup TSimpClientReadType TSimpClientReadType
  * @{ Simple BLE client read request type, used by app to send read request
  */
typedef enum
{
    DT_READ_REQ_V1_WRITE_NOTIFY_NOTIF_BIT
} TDtClientReadType, *PDtClientReadType;
/** End of TSimpClientReadType
  * @}
  */

/** @defgroup TSimpClientReadResult TSimpClientReadResult
  * @{ Simple BLE client read result type, used to inform app which read value responsed
  */
typedef enum
{
    DT_READ_RESULT_V1_WRITE_NOTIFY_NOTIF_BIT,
    DT_READ_RESULT_FAIL
} TDtClientReadResult;
/** End of TSimpClientReadResult
  * @}
  */

/** @defgroup TSimpClientReadContent TSimpClientReadContent
  * @{ Simple BLE client read content, used to inform app read response data content
  */
typedef union
{
    bool v1_write_notify_notif_flag;
} TDtClientReadContent, *PDtClientReadContent;
/** End of TSimpClientReadContent
  * @}
  */

/** @defgroup TSimpClientReadData TSimpClientReadData
  * @{ Simple BLE client read data, used to inform app read response data
  */
typedef struct
{
    TDtClientReadResult read_result;
    TDtClientReadContent read_content;
} TDtClientReadData;
/** End of TSimpClientReadData
  * @}
  */

/** @defgroup TSimpClientWriteResult TSimpClientWriteResult
  * @{ Simple BLE client write request result
  */
typedef enum
{
    DT_WRITE_RESULT_SUCCESS,
    DT_WRITE_RESULT_FAIL
} TDtClientWriteResult;
/** End of TSimpClientWriteResult
  * @}
  */

/** @defgroup TSimpClientNotifIndReceiveType TSimpClientNotifIndReceiveType
  * @{ Simple BLE client notif/ind receive type
  */
typedef enum
{
    DT_NOTIF_RECEIVE_V1_WRITE_NOTIFY,
    DT_NOTIF_IND_RECEIVE_FAIL
} TDtClientNotifIndReceiveType;
/** End of TSimpClientNotifIndReceiveType
  * @}
  */

/** @defgroup TSimpClientNotifIndReceiveContent TSimpClientNotifIndReceiveContent
  * @{ Simple BLE client notif/ind receive content
  */
typedef union
{
    uint8_t v1_write_notify;
} TDtClientNotifIndReceiveContent, *PDtClientNotifIndReceiveContent;
/** End of TSimpClientNotifIndReceiveContent
  * @}
  */

/** @defgroup TSimpClientNotifIndData TSimpClientNotifIndData
  * @{ The notif/ind data received by Simple BLE client from Simple BLE server
  */
typedef struct
{
    TDtClientNotifIndReceiveType receive_type;
    TDtClientNotifIndReceiveContent receive_content;
} TDtClientNotifIndData, *PDtClientNotifIndData;
/** End of TSimpClientNotifIndData
  * @}
  */

/* Simple BLE client callback related data. */

/** @defgroup TSimpClientCB_Type TSimpClientCB_Type
  * @{ Event type to inform app
  */
typedef enum
{
    DT_CLIENT_CB_TYPE_DISC_STATE,          //!< Discovery procedure state, done or pending.
    DT_CLIENT_CB_TYPE_DISC_HDL,            //!< Discovered handle information.
    DT_CLIENT_CB_TYPE_READ_RESULT,         //!< Read request's result data, responsed from server.
    DT_CLIENT_CB_TYPE_WRITE_RESULT,        //!< Write request result, success or fail.
    DT_CLIENT_CB_TYPE_NOTIF_IND_RESULT,    //!< Notification or indication data received from server.
    DT_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} TDtClientCB_Type;
/** End of TSimpClientCB_Type
  * @}
  */

/** @defgroup TSimpClientCB_Content TSimpClientCB_Content
  * @{ Callback content sent to application from Simple BLE client
  */
typedef union
{
    TDtDiscState disc_state;
    TDtDiscHdlInfo disc_hdl_info;
    TDtClientReadData read_data;
    TDtClientWriteResult write_result;
    TDtClientNotifIndData notif_ind_data;
} TDtClientCB_Content;
/** End of TSimpClientCB_Content
  * @}
  */

/** @defgroup TSimpClientCB_Data TSimpClientCB_Data
  * @{ Callback data sent to application from Simple BLE client, include type and content
  */
typedef struct
{
    TDtClientCB_Type     cb_type;
    TDtClientCB_Content    cb_content;
} TDtClientCB_Data;
/** End of TSimpClientCB_Data
  * @}
  */

extern TClientID Dt_AddClient( pfnSpecificClientAppCB_t appCB );
extern bool DtClient_SetHandle( TDtHandleType handle_type, uint16_t handle_value);
extern bool DtClient_StartDiscovery( void );
extern bool DtClient_V1NotifyCmd( bool command );
bool DtClient_V1WriteChar(uint8_t *v1_write_val, uint16_t length);


/** @} End of Simple BLE */

/** @} End of RTK_Client_Module */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _RSC_DEF_H */
