/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     rsc_client.h
  * @brief    Head file for using Running Speed and Cadence Client.
  * @details  RSC data structs and external functions declaration.
  * @author   ethan_su
  * @date     2015-07-23
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _RSC_CLIENT_H_
#define _RSC_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"
#include "profile_client.h"
#include "rsc_config.h"

/** @addtogroup RSC RSC
  * @brief RSC Profile
  * @{
  */

/** @defgroup RSC_Client RSC Client
  * @brief RSC Client
  * @{
  */ 

/** @defgroup RSC_Client_Exported_Types RSC Client Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/** @defgroup TRscClientHandleType TRscClientHandleType
  * @{ Handle cache for intrested UUIDs
  */
typedef enum
{
    HDL_RSC_SRV_START,          //!< Group start handle of RSC service.
    HDL_RSC_SRV_END,            //!< Group end handle of RSC service.
    HDL_RSC_MEAS,               //!< RSC measurement value handle.
    HDL_RSC_MEAS_END,           //!< RSC measurement character end handle.
    HDL_RSC_MEAS_CCCD,          //!< RSC measurement CCCD handle.
    HDL_RSC_FEATURE,            //!< RSC feature handle.
    HDL_RSC_SENSOR_LOCATION,    //!< RSC sensor location handle.
    HDL_RSC_CTL_PNT,            //!< RSC control point handle.
    HDL_RSC_CTL_PNT_END,        //!< RSC control point end handle.
    HDL_RSC_CTL_PNT_CCCD,       //!< RSC control point CCCD handle.
    HDL_RSC_CACHE_LEN
}TRscClientHandleType;
/** End of TRscClientHandleType
  * @}
  */

/** @defgroup TRscClientDiscState TRscClientDiscState
  * @{ used to inform app the discovery procedure is done or pending
  */
typedef enum
{
    RSC_DISC_DONE,
    RSC_DISC_PENDING
}TRscClientDiscState, *PRscClientDiscState;
/** End of TRscClientDiscState
  * @}
  */
  
/** @defgroup TRscClientDiscHdlInfo TRscClientDiscHdlInfo
  * @{ handle information found in discovery procedure
  */
typedef struct
{
    TRscClientHandleType  handle_type;
    uint16_t        handle_value;
}TRscClientDiscHdlInfo;
/** End of TRscClientDiscHdlInfo
  * @}
  */

/** @defgroup TRscClientReadType TRscClientReadType
  * @{ RSC client read request type, used by app to send read request
  */
typedef enum
{
    RSC_READ_REQ_MEAS_NOTIF_BIT,
    RSC_READ_REQ_CTL_PNT_IND_BIT,
    RSC_READ_REQ_FEATURE,
    RSC_READ_REQ_SENSOR_LOC
}TRscClientReadType, *PRscClientReadType;
/** End of TRscClientReadType
  * @}
  */

/** @defgroup TRscClientReadResult TRscClientReadResult
  * @{ RSC client read result type, used to inform app which read value responsed
  */
typedef enum
{
    RSC_READ_RESULT_MEAS_NOTIF_BIT,
    RSC_READ_RESULT_CTL_PNT_IND_BIT,
    RSC_READ_RESULT_FEATURE,
    RSC_READ_RESULT_SENSOR_LOC,
    RSC_READ_RESULT_FAIL
}TRscClientReadResult;
/** End of TRscClientReadResult
  * @}
  */

/** @defgroup TRscClientReadContent TRscClientReadContent
  * @{ RSC client read content, used to inform app read response data content
  */
typedef union
{
    bool meas_notif_flag;
    bool ctl_pnt_ind_flag;
    uint16_t feature;
    uint8_t sens_location;
}TRscClientReadContent, *PRscClientReadContent;
/** End of TRscClientReadContent
  * @}
  */

/** @defgroup TRscClientReadData TRscClientReadData
  * @{ RSC client read data, used to inform app read response data
  */
typedef struct
{
    TRscClientReadResult read_result;
    TRscClientReadContent read_content;
}TRscClientReadData;
/** End of TRscClientReadData
  * @}
  */

/** @defgroup TRscClientWriteResult TRscClientWriteResult
  * @{ RSC client write request result
  */
typedef enum
{
    RSC_WRITE_RESULT_SUCCESS,
    RSC_WRITE_RESULT_FAIL
}TRscClientWriteResult;
/** End of TRscClientWriteResult
  * @}
  */

/** @defgroup TRscClientNotifIndReceiveType TRscClientNotifIndReceiveType
  * @{ RSC client notif/ind receive type
  */
typedef enum
{
    RSC_NOTIF_RECEIVE_MEAS,
    RSC_IND_RECEIVE_CTL_PNT,
    RSC_NOTIF_IND_RECEIVE_FAIL
}TRscClientNotifIndReceiveType;
/** End of TRscClientNotifIndReceiveType
  * @}
  */

/** @defgroup TRscClientNotifIndReceiveContent TRscClientNotifIndReceiveContent
  * @{ RSC client notif/ind receive content
  */
typedef union
{
    TRSCMeasurement measurement;
    TRSCControlPoint control_point;
}TRscClientNotifIndReceiveContent, *PRscClientNotifIndReceiveContent;
/** End of TRscClientNotifIndReceiveContent
  * @}
  */

/** @defgroup TRscClientNotifIndData TRscClientNotifIndData
  * @{ The notif/ind data received by RSC client from RSC server
  */
typedef struct
{
    TRscClientNotifIndReceiveType receive_type;
    TRscClientNotifIndReceiveContent receive_content;
}TRscClientNotifIndData, *PRscClientNotifIndData;
/** End of TRscClientNotifIndData
  * @}
  */

/* RSC client callback related data. */

/** @defgroup TRscClientCB_Type TRscClientCB_Type
  * @{ Event type to inform app
  */
typedef enum
{
    RSC_CLIENT_CB_TYPE_DISC_STATE,          //!< Discovery procedure state, done or pending.
    RSC_CLIENT_CB_TYPE_DISC_HDL,            //!< Discovered handle information.
    RSC_CLIENT_CB_TYPE_READ_RESULT,         //!< Read request's result data, responsed from server.
    RSC_CLIENT_CB_TYPE_WRITE_RESULT,        //!< Write request result, success or fail.
    RSC_CLIENT_CB_TYPE_NOTIF_IND_RESULT,    //!< Notification or indication data received from server.
    RSC_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} TRscClientCB_Type;
/** End of TRscClientCB_Type
  * @}
  */

/** @defgroup TRscClientCB_Content TRscClientCB_Content
  * @{ Callback content sent to application from RSC client
  */
typedef union
{
    TRscClientDiscState disc_state;
    TRscClientDiscHdlInfo disc_hdl_info;
    TRscClientReadData read_data;
    TRscClientWriteResult write_result;
    TRscClientNotifIndData notif_ind_data;
}TRscClientCB_Content;
/** End of TRscClientCB_Content
  * @}
  */

/** @defgroup TRscClientCB_Data TRscClientCB_Data
  * @{ Callback data sent to application from RSC client, include type and content
  */
typedef struct
{
    TRscClientCB_Type     cb_type;
    TRscClientCB_Content    cb_content;
} TRscClientCB_Data;
/** End of TRscClientCB_Data
  * @}
  */
  
/** @defgroup TRscClientSuppSensList TRscClientSuppSensList
  * @{ Used to store the sensor location list supported by RSC server
  */
typedef struct
{
    uint8_t list_num;
    uint8_t list_value[RSC_MAX_CTL_PNT_VALUE-3];
}TRscClientSuppSensList, *PRscClientSuppSensList;
/** End of TRscClientSuppSensList
  * @}
  */

/** @} End of RSC_Client_Exported_Types */


/** @defgroup RSC_Client_Exported_Functions RSC Client Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
extern TClientID RSC_AddClient( pfnSpecificClientAppCB_t appCB );
extern bool RSCClient_SetHandle( TRscClientHandleType handle_type, uint16_t handle_value);
extern uint16_t RSCClient_GetHandle( TRscClientHandleType handle_type );
extern void RSCClient_MeasSplit(PRSCMeasurement pRscMeas,
                                uint8_t *pFlag,
                                uint16_t *pSpeed,
                                uint8_t *pCadence,
                                uint16_t *pStrideLength,
                                uint32_t *pTotalDistance);
extern bool RSCClient_CtlPntSplit( PRSCControlPoint pRscCtlPnt,
                                   uint8_t *pReqOpCode,
                                   uint8_t *pRespValue,
                                   PRscClientSuppSensList pSensLocList);
extern bool RSCClient_StartDiscovery( void );
extern bool RSCClient_ReadByHandle( TRscClientReadType readCharType );
extern bool RSCClient_ReadByUUID( TRscClientReadType readCharType );
extern bool RSCClient_MeasNotifyCmd( bool command );
extern bool RSCClient_CtlPntIndicateCmd( bool command );
extern bool RSCClient_CtlPntSetCumulValue( uint32_t total_distance );
extern bool RSCClient_CtlPntStartSensorCalib( void );
extern bool RSCClient_CtlPntUpdateSensorLocation( uint8_t sensor_location );
extern bool RSCClient_CtlPntReqSensorLocationList( void );
/** @} End of RSC_Client_Exported_Functions */

/** @} End of RSC_Client */

/** @} End of RSC */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _RSC_DEF_H */

