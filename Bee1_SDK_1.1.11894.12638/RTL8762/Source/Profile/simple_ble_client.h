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
#ifndef _SIMPLE_BLE_CLIENT_H_
#define _SIMPLE_BLE_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"
#include "profile_client.h"
#include "simple_ble_config.h"

/** @addtogroup SIMP SIMP
  * @{
  */

/** @defgroup SIMP_Client SIMP Client
  * @brief Simple BLE Client
  * @{
  */ 

/** @defgroup SIMP_Client_Exported_Types SIMP Client Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/** @defgroup TSimpClientHandleType TSimpClientHandleType
  * @{ Handle cache for intrested UUIDs
  */
typedef enum
{
    HDL_SIMBLE_SRV_START,           // start handle of simple ble service
    HDL_SIMBLE_SRV_END,             // end handle of simple ble service
    HDL_SIMBLE_V1_READ,             // V1 read value handle
    HDL_SIMBLE_V2_WRITE,            // V2 write value handle
    HDL_SIMBLE_V3_NOTIFY,           // V3 notify value handle
    HDL_SIMBLE_V3_NOTIFY_END,       // V3 notify value end handle
    HDL_SIMBLE_V3_NOTIFY_CCCD,      // V3 notify CCCD handle
    HDL_SIMBLE_V4_INDICATE,         // V4 indicate value handle
    HDL_SIMBLE_V4_INDICATE_END,     // V4 indicate value end handle
    HDL_SIMBLE_V4_INDICATE_CCCD,    // V4 indicate CCCD handle
    HDL_SIMBLE_V5_CTL_PNT,          // V5 control point handle
    HDL_SIMBLE_V5_CTL_PNT_END,      // V5 control point end handle
    HDL_SIMBLE_V5_CTL_PNT_CCCD,     // V5 control point CCCD handle
    HDL_SIMBLE_CACHE_LEN
}TSimpClientHandleType;
/** End of TSimpClientHandleType * @} */

/** @defgroup TSimpClientDiscState TSimpClientDiscState
  * @{ used to inform app the discovery procedure is done or pending
  */
typedef enum
{
    SIMP_DISC_DONE,
    SIMP_DISC_PENDING
}TSimpClientDiscState, *PSimpClientDiscState;
/** End of TSimpClientDiscState
  * @}
  */
  
/** @defgroup TSimpClientDiscHdlInfo TSimpClientDiscHdlInfo
  * @{ handle information found in discovery procedure
  */
typedef struct
{
    TSimpClientHandleType  handle_type;
    uint16_t        handle_value;
}TSimpClientDiscHdlInfo;
/** End of TSimpClientDiscHdlInfo
  * @}
  */

/** @defgroup TSimpClientReadType TSimpClientReadType
  * @{ Simple BLE client read request type, used by app to send read request
  */
typedef enum
{
    SIMP_READ_REQ_V1_READ,
    SIMP_READ_REQ_V3_NOTIFY_NOTIF_BIT,
    SIMP_READ_REQ_V4_INDICATE_IND_BIT,
    SIMP_READ_REQ_V5_CTL_PNT_IND_BIT
}TSimpClientReadType, *PSimpClientReadType;
/** End of TSimpClientReadType
  * @}
  */

/** @defgroup TSimpClientReadResult TSimpClientReadResult
  * @{ Simple BLE client read result type, used to inform app which read value responsed
  */
typedef enum
{
    SIMP_READ_RESULT_V1_READ,
    SIMP_READ_RESULT_V3_NOTIFY_NOTIF_BIT,
    SIMP_READ_RESULT_V4_INDICATE_IND_BIT,
    SIMP_READ_RESULT_V5_CTL_PNT_IND_BIT,
    SIMP_READ_RESULT_FAIL
}TSimpClientReadResult;
/** End of TSimpClientReadResult
  * @}
  */

/** @defgroup TSimpClientReadContent TSimpClientReadContent
  * @{ Simple BLE client read content, used to inform app read response data content
  */
typedef union
{
    uint8_t v1_read;
    bool v3_notify_notif_flag;
    bool v4_indicate_ind_flag;
    bool v5_ctl_pnt_ind_flag;
}TSimpClientReadContent, *PSimpClientReadContent;
/** End of TSimpClientReadContent
  * @}
  */

/** @defgroup TSimpClientReadData TSimpClientReadData
  * @{ Simple BLE client read data, used to inform app read response data
  */
typedef struct
{
    TSimpClientReadResult read_result;
    TSimpClientReadContent read_content;
}TSimpClientReadData;
/** End of TSimpClientReadData
  * @}
  */

/** @defgroup TSimpClientWriteResult TSimpClientWriteResult
  * @{ Simple BLE client write request result
  */
typedef enum
{
    SIMP_WRITE_RESULT_SUCCESS,
    SIMP_WRITE_RESULT_FAIL
}TSimpClientWriteResult;
/** End of TSimpClientWriteResult
  * @}
  */

/** @defgroup TSimpClientNotifIndReceiveType TSimpClientNotifIndReceiveType
  * @{ Simple BLE client notif/ind receive type
  */
typedef enum
{
    SIMP_NOTIF_RECEIVE_V3_NOTIFY,
    SIMP_IND_RECEIVE_V4_INDICATE,
    SIMP_IND_RECEIVE_V5_CTL_PNT,
    SIMP_NOTIF_IND_RECEIVE_FAIL
}TSimpClientNotifIndReceiveType;
/** End of TSimpClientNotifIndReceiveType
  * @}
  */

/** @defgroup TSimpClientNotifIndReceiveContent TSimpClientNotifIndReceiveContent
  * @{ Simple BLE client notif/ind receive content
  */
typedef union
{
    uint8_t v3_notify;
    uint8_t v4_indicate;
    uint8_t v5_control_point[3];
}TSimpClientNotifIndReceiveContent, *PSimpClientNotifIndReceiveContent;
/** End of TSimpClientNotifIndReceiveContent
  * @}
  */

/** @defgroup TSimpClientNotifIndData TSimpClientNotifIndData
  * @{ The notif/ind data received by Simple BLE client from Simple BLE server
  */
typedef struct
{
    TSimpClientNotifIndReceiveType receive_type;
    TSimpClientNotifIndReceiveContent receive_content;
}TSimpClientNotifIndData, *PSimpClientNotifIndData;
/** End of TSimpClientNotifIndData
  * @}
  */

/* Simple BLE client callback related data. */

/** @defgroup TSimpClientCB_Type TSimpClientCB_Type
  * @{ Event type to inform app
  */
typedef enum
{
    SIMP_CLIENT_CB_TYPE_DISC_STATE,          //!< Discovery procedure state, done or pending.
    SIMP_CLIENT_CB_TYPE_DISC_HDL,            //!< Discovered handle information.
    SIMP_CLIENT_CB_TYPE_READ_RESULT,         //!< Read request's result data, responsed from server.
    SIMP_CLIENT_CB_TYPE_WRITE_RESULT,        //!< Write request result, success or fail.
    SIMP_CLIENT_CB_TYPE_NOTIF_IND_RESULT,    //!< Notification or indication data received from server.
    SIMP_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} TSimpClientCB_Type;
/** End of TSimpClientCB_Type
  * @}
  */

/** @defgroup TSimpClientCB_Content TSimpClientCB_Content
  * @{ Callback content sent to application from Simple BLE client
  */
typedef union
{
    TSimpClientDiscState disc_state;
    TSimpClientDiscHdlInfo disc_hdl_info;
    TSimpClientReadData read_data;
    TSimpClientWriteResult write_result;
    TSimpClientNotifIndData notif_ind_data;
}TSimpClientCB_Content;
/** End of TSimpClientCB_Content
  * @}
  */

/** @defgroup TSimpClientCB_Data TSimpClientCB_Data
  * @{ Callback data sent to application from Simple BLE client, include type and content
  */
typedef struct
{
    TSimpClientCB_Type     cb_type;
    TSimpClientCB_Content    cb_content;
} TSimpClientCB_Data;
/** End of TSimpClientCB_Data
  * @}
  */

/** End of SIMP_Client_Exported_Types * @} */

/** @defgroup SIMP_Client_Exported_Functions SIMP Client Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
extern TClientID SimpBle_AddClient( pfnSpecificClientAppCB_t appCB );
extern bool SimpBleClient_SetHandle( TSimpClientHandleType handle_type, uint16_t handle_value);
extern uint16_t SimpBleClient_GetHandle( TSimpClientHandleType handle_type );
extern bool SimpBleClient_StartDiscovery( void );
extern bool SimpBleClient_ReadByHandle( TSimpClientReadType readCharType );
extern bool SimpBleClient_ReadByUUID( TSimpClientReadType readCharType );
extern bool SimpBleClient_V3NotifyCmd( bool command );
extern bool SimpBleClient_V4IndicateCmd( bool command );
extern bool SimpBleClient_V5CtlPntIndicateCmd( bool command );
extern bool SimpBleClient_CtlPntSetV4Indicate(uint8_t v4_ind_val);
extern bool SimpBleClient_CtlPntSetV3Notify(uint8_t v3_notif_val);
extern bool SimpBleClient_WriteV2WriteChar(uint8_t v2_write_val);
/** @} End of SIMP_Client_Exported_Functions */

/** @} End of SIMP_Client */

/** @} End of SIMP */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _SIMPLE_BLE_CLIENT_H_ */
