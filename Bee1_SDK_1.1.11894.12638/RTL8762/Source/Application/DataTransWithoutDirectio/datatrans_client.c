enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     simple_ble_client.c
* @brief    Simple BLE client source file.
* @details  Interfaces to implement Simple BLE related functions.
* @author   ethan_su
* @date     2015-08-20
* @version  v0.1
*********************************************************************************************************
*/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "profile.h"
#include "profileApi.h"
#include "profile_client.h"
#include "datatrans_profile.h"
#include "datatrans_client.h"

#include "bee_message.h"
#include "dataTrans_application.h"
#include "module_param_config.h"


/**
  * @{ Used for CCCD handle discovering in discovery procedure
  */
typedef enum
{
    DT_CCCD_DISC_START,
    DT_CCCD_DISC_V1_WRITE_NOTIFY,
    DT_CCCD_DISC_END
} TDtCCCD_DiscState, *PDtCCCD_DiscState;
/**
  * @}
  */

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Simple BLE client ID. */
static TClientID DtClient_ClientID = AppProcessGeneralClientMsgID;
/**<  Simple BLE discovery end handle control. */
static TDtHandleType DtClient_EndHdlIdx = HDL_DT_CACHE_LEN;
/**<  Callback used to send data to app from Simple BLE client layer. */
static pfnSpecificClientAppCB_t pfnDtClientAppCB = NULL;
/**<  Handle Cache for simple BLE client to cache discovered handles. */
static uint16_t DtHdlCache[HDL_DT_CACHE_LEN] = {0};
/**<  Discovery State indicate which CCCD is in discovery. */
static TDtCCCD_DiscState DtClient_CccdDiscState = DT_CCCD_DISC_START;

/* Used by application, to set the handles in Simple BLE handle cache. */
bool DtClient_SetHandle( TDtHandleType handle_type, uint16_t handle_value)
{
    if (handle_type < HDL_DT_CACHE_LEN)
    {
        DtHdlCache[handle_type] = handle_value;
        return TRUE;
    }
    return FALSE;
}






/* Used by application, to start the discovery procedure of Simple BLE server. */
bool DtClient_StartDiscovery( void )
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClient_StartDiscovery", 0);
#endif
    /* First clear handle cache. */
    memset(DtHdlCache, 0, sizeof(DtHdlCache));

    if ( clientAPI_ByUuid128SrvDiscovery( DtClient_ClientID, (uint8_t *)GATT_UUID_DATATRANSFER_SERVICE) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}



/* Used internal, start the discovery of Simple BLE characteristics. */
static bool DtClient_StartCharDiscovery( void )
{
    TCharDiscReq charReq;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClient_StartCharDiscovery", 0);
#endif
    charReq.wStartHandle = DtHdlCache[HDL_DT_SRV_START];
    charReq.wEndHandle = DtHdlCache[HDL_DT_SRV_END];
    if ( clientAPI_AllCharDiscovery( DtClient_ClientID, charReq ) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}





/* Used internal, start the discovery of Simple BLE characteristics descriptor. */
static bool DtClient_StartCharDescriptorDiscovery( TCharDescriptorDiscReq charDescriptorReq )
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClient_StartCharDescriptorDiscovery", 0);
#endif
    if ( clientAPI_AllCharDescriptorDiscovery( DtClient_ClientID, charDescriptorReq ) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}





/* Used by application, to enable or disable the notification of Simple BLE measurement value. */
bool DtClient_V1NotifyCmd( bool command )
{
    TWriteReq writeReq;
    uint16_t wCCCDBits;
    bool hdl_valid = FALSE;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClient_V1NotifyCmd: command = %d", 1, command);
#endif

    wCCCDBits = command ? 1 : 0;

    if (DtHdlCache[HDL_DT_V1_WRITE_NOTIFY_CCCD])
    {
        writeReq.wHandle = DtHdlCache[HDL_DT_V1_WRITE_NOTIFY_CCCD];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(uint16_t);
        writeReq.pData = (uint8_t *)&wCCCDBits;
        hdl_valid = TRUE;
    }

    if (hdl_valid)
    {
        if (clientAPI_AttribWrite( DtClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "DtClient_V1NotifyCmd: Request fail! Please check!", 0);
#endif
    return FALSE;
}




bool DtClient_V1WriteChar(uint8_t *v1_write_val, uint16_t length)
{
    TWriteReq writeReq;
    bool hdl_valid = FALSE;


    if (DtHdlCache[HDL_DT_V1_WRITE_NOTIFY])
    {
        writeReq.wHandle = DtHdlCache[HDL_DT_V1_WRITE_NOTIFY];
        writeReq.wType = blueAPI_GATTWriteTypeCommand;  //write request or command?
        writeReq.wLength = length;
        writeReq.pData = v1_write_val;
        hdl_valid = TRUE;
    }

    if (hdl_valid)
    {
        if (clientAPI_AttribWrite( DtClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }

    return FALSE;
}





static void DtClient_SwitchNextDiscDescriptor(TDtCCCD_DiscState *pSwitchState)
{
    TDtCCCD_DiscState new_state;

    switch (*pSwitchState)
    {
        case DT_CCCD_DISC_START:
            if (DtHdlCache[HDL_DT_V1_WRITE_NOTIFY])
            {
                new_state = DT_CCCD_DISC_V1_WRITE_NOTIFY;
            }
            else
            {
                new_state = DT_CCCD_DISC_END;
            }
            break;
        default:
            new_state = DT_CCCD_DISC_END;
            break;
    }

    *pSwitchState = new_state;

}






/* Called by profile client layer, when discover state of discovery procedure changed. */
static TProfileResult DtClientDiscoverStateCb( TDiscoveryState discoveryState )
{
    bool cb_flag = FALSE;
    bool descriptor_disc_flag = FALSE;
    TProfileResult result = ProfileResult_Success;
    TCharDescriptorDiscReq charDescriptorReq;
    TDtClientCB_Data cb_data;
    cb_data.cb_type = DT_CLIENT_CB_TYPE_DISC_STATE;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClientDiscoverStateCb: discoveryState = %d", 1, discoveryState);
#endif

    switch ( discoveryState )
    {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if ((DtHdlCache[HDL_DT_SRV_START] != 0)
                    || (DtHdlCache[HDL_DT_SRV_END] != 0))
            {
                if (DtClient_StartCharDiscovery() == FALSE)
                {
                    cb_data.cb_content.disc_state = DT_DISC_PENDING;
                    cb_flag = TRUE;
                }
            }
            /* No Simple BLE service handle found. Discover procedure complete. */
            else
            {
                cb_data.cb_content.disc_state = DT_DISC_DONE;
                cb_flag = TRUE;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            /* We should store the last char end handle if needed. */
            if (DtClient_EndHdlIdx < HDL_DT_CACHE_LEN)
            {
                DtHdlCache[DtClient_EndHdlIdx] = DtHdlCache[HDL_DT_SRV_END];
                DtClient_EndHdlIdx = HDL_DT_CACHE_LEN;
            }
            /* Find the first descriptor to be discovered. */
            DtClient_CccdDiscState = DT_CCCD_DISC_START;
            DtClient_SwitchNextDiscDescriptor(&DtClient_CccdDiscState);
            break;
        case DISC_STATE_CHAR_DESCRIPTOR_DONE:
            /* Find the next descriptor to be discovered. */
            DtClient_SwitchNextDiscDescriptor(&DtClient_CccdDiscState);
            break;
        default:
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "Invalid Discovery State!", 0);
#endif
            break;
    }

    /* Switch different char descriptor discovery, if has multi char descriptors. */
    switch (DtClient_CccdDiscState)
    {
        case DT_CCCD_DISC_V1_WRITE_NOTIFY:
            /* Need to discover v3 notify char descriptor. */
            charDescriptorReq.wStartHandle = DtHdlCache[HDL_DT_V1_WRITE_NOTIFY] + 1;
            charDescriptorReq.wEndHandle = DtHdlCache[HDL_DT_V1_WRITE_NOTIFY_END];
            descriptor_disc_flag = TRUE;
            break;
        case DT_CCCD_DISC_END:
            cb_data.cb_content.disc_state = DT_DISC_DONE;
            cb_flag = TRUE;
            break;
        default:
            /* No need to send char descriptor discovery. */
            break;
    }
    if (descriptor_disc_flag)
    {
        if (DtClient_StartCharDescriptorDiscovery(charDescriptorReq) == FALSE)
        {
            cb_data.cb_content.disc_state = DT_DISC_PENDING;
            cb_flag = TRUE;
        }
    }

    /* Send discover state to application if needed. */
    if (cb_flag && pfnDtClientAppCB)
    {
        (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
    }

    return (result);
}





/* Called by profile client layer, when discover result fetched. */
static TProfileResult DtClientDiscoverResultCb( TDiscoverResultType resultType, TDiscoverResultData resultData )
{
    TProfileResult result = ProfileResult_Success;
    bool char_cb_flag = FALSE;
    TDtClientCB_Data cb_data;
    cb_data.cb_type = DT_CLIENT_CB_TYPE_DISC_HDL;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClientDiscoverResultCb: resultType = %d", 1, resultType);
#endif

    switch ( resultType )
    {
        case DISC_RESULT_SRV_DATA:
            /* send service handle range to application. */
            if (pfnDtClientAppCB)
            {
                cb_data.cb_content.disc_hdl_info.handle_type = HDL_DT_SRV_START;
                cb_data.cb_content.disc_hdl_info.handle_value = resultData.pSrvDiscData->attHandle;
                (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
                cb_data.cb_content.disc_hdl_info.handle_type = HDL_DT_SRV_END;
                cb_data.cb_content.disc_hdl_info.handle_value = resultData.pSrvDiscData->endGroupHandle;
                (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
            }
            break;
        case DISC_RESULT_CHAR_UUID16:
            cb_data.cb_content.disc_hdl_info.handle_value = resultData.pCharUUID16DiscData->valueHandle;
            /* When use clientAPI_AllCharDiscovery. */
            if (DtClient_EndHdlIdx < HDL_DT_CACHE_LEN)
            {
                DtHdlCache[DtClient_EndHdlIdx] = resultData.pCharUUID16DiscData->declHandle - 1;
                DtClient_EndHdlIdx = HDL_DT_CACHE_LEN;
            }
            /* we should inform intrested handles to upper application. */
            switch (resultData.pCharUUID16DiscData->UUID16)
            {
                case GATT_UUID_CHAR_DATA:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_DT_V1_WRITE_NOTIFY;
                    char_cb_flag = TRUE;
                    DtClient_EndHdlIdx = HDL_DT_V1_WRITE_NOTIFY_END;
                    break;
                default:
                    /* have no intrest on this handle. */
                    break;
            }

            /* Inform application if needed. */
            if (char_cb_flag && pfnDtClientAppCB)
            {
                (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
            }
            break;
        case DISC_RESULT_CHAR_DESC_UUID16:
            /* When use clientAPI_AllCharDescriptorDiscovery. */
            if (resultData.pCharDescUUID16DiscData->UUID16 == GATT_UUID_CHAR_CLIENT_CONFIG)
            {
                uint16_t temp_handle = resultData.pCharDescUUID16DiscData->handle;
                cb_data.cb_content.disc_hdl_info.handle_value = temp_handle;
                bool cccd_cb_flag = FALSE;
                if ( (temp_handle > DtHdlCache[HDL_DT_V1_WRITE_NOTIFY])
                        && (temp_handle <= DtHdlCache[HDL_DT_V1_WRITE_NOTIFY_END]) )
                {
                    cb_data.cb_content.disc_hdl_info.handle_type =  HDL_DT_V1_WRITE_NOTIFY_CCCD;
                    cccd_cb_flag = TRUE;
                }
                else
                {
                    /* have no intrest in this handle. */
                }

                /* Inform application if needed. */
                if (cccd_cb_flag && pfnDtClientAppCB)
                {
                    (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
                }
            }
            break;
        default:
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "Invalid Discovery Result Type!", 0);
#endif
            break;
    }

    return (result);
}




/* Called by profile client layer, when write request complete. */
static TProfileResult DtClientWriteResultCb( TClientRequestResult reqResult )
{
    TProfileResult result = ProfileResult_Success;
    TDtClientCB_Data cb_data;
    cb_data.cb_type = DT_CLIENT_CB_TYPE_WRITE_RESULT;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClientWriteResultCb: result = %d", 1, reqResult);
#endif

    /* If write req success, branch to fetch value and send to application. */
    if (reqResult == CLIENT_REQ_SUCCESS)
    {
        cb_data.cb_content.write_result = DT_WRITE_RESULT_SUCCESS;

        Profile_DataTransSendData();
        if (( g_AppCB->txAppDataQueueFree.ElementCount >= 1) && (gConfigParam->fc_enable.fc_enable))
        {
            BEE_IO_MSG app_msg;
            app_msg.IoType = IO_UART_MSG_TYPE;
            app_msg.subType = MSG_UART_RX_EMPTY;
            AppSendMsgFromGATTCallback(&app_msg);
        }
        xTimerReset(TimersUartDataExtra, 0);


    }
    /* Read req fail, inform application. */
    else
    {
        cb_data.cb_content.write_result = DT_WRITE_RESULT_FAIL;
    }

#if 1
    /* Inform application the write result. */
    if (pfnDtClientAppCB)
    {
        (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
    }
#endif

    return (result);
}










/* Called by profile client layer, when notification or indication arrived. */
static TProfileResult DtClientNotifIndResultCb( uint16_t wHandle, int iValueSize, uint8_t *pValue )
{
    TProfileResult result = ProfileResult_Success;
    TDtClientCB_Data cb_data;
    cb_data.cb_type = DT_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClientNotifIndResultCb: handle = 0x%4.4x", 1, wHandle);
#endif

    if ( wHandle == DtHdlCache[HDL_DT_V1_WRITE_NOTIFY] )
    {
        if (iValueSize <= gProfileData.wMTUSize - 3)
        {
            ProfileHandleDataRx((uint16_t)(iValueSize), pValue);
            cb_data.cb_content.notif_ind_data.receive_type = DT_NOTIF_RECEIVE_V1_WRITE_NOTIFY;
            cb_data.cb_content.notif_ind_data.receive_content.v1_write_notify = *(uint8_t *)pValue;
        }
        else
        {
            cb_data.cb_content.notif_ind_data.receive_type = DT_NOTIF_IND_RECEIVE_FAIL;
        }
    }
    else
    {
        cb_data.cb_content.notif_ind_data.receive_type = DT_NOTIF_IND_RECEIVE_FAIL;
    }

#if 1
    /* Inform application the notif/ind result. */
    if (pfnDtClientAppCB)
    {
        (*pfnDtClientAppCB)(DtClient_ClientID, &cb_data);
    }
#endif

    return (result);
}






/* Called by profile client layer, when link disconnected. */
static TProfileResult DtClientDisconnectCb( void )
{
    TProfileResult result = ProfileResult_Success;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DtClientDisconnectCb.", 0);
#endif

    /* Reset some params, when disconnection occurs. */
    DtClient_EndHdlIdx = HDL_DT_CACHE_LEN;
    DtClient_CccdDiscState = DT_CCCD_DISC_START;

    /* There is no need to inform application, because GAP has done. */

    return (result);
}

/**
 * @brief Simple BLE Client Callbacks.
*/
CONST TGATTClientCBs DtClientCBs =
{
    DtClientDiscoverStateCb,   //!< Discovery State callback function pointer
    DtClientDiscoverResultCb,  //!< Discovery result callback function pointer
    NULL,      //!< Read response callback function pointer
    DtClientWriteResultCb,     //!< Write result callback function pointer
    DtClientNotifIndResultCb,  //!< Notify Indicate callback function pointer
    DtClientDisconnectCb       //!< Link disconnection callback function pointer
};

/**
  * @brief add RSC client to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return client ID auto generated by profile layer.
  * @retval  client ID
  */
TClientID Dt_AddClient( pfnSpecificClientAppCB_t appCB )
{
    TClientID dt_client_id;
    if ( FALSE == clientAPI_RegisterSpecClientCB( &dt_client_id, DtHdlCache, &DtClientCBs ) )
    {
        dt_client_id = AppProcessGeneralClientMsgID;
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "Dt_AddClient Fail !!!", 0);
#endif
        return dt_client_id;
    }
    DtClient_ClientID = dt_client_id;
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Dt_AddClient: client ID = %d", 1, DtClient_ClientID);
#endif

    /* register callback for profile to inform application that some events happened. */
    pfnDtClientAppCB = appCB;

    return dt_client_id;
}

