enum { __FILE_NUM__ = 0 };

/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_client.c
  * @brief   Simple BLE client source file.
  * @details
  * @author  Ethan
  * @date    11-Aug-2015
  * @version v0.1
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "simple_ble_client.h"

/**
  * @{ Used for CCCD handle discovering in discovery procedure
  */
typedef enum
{
	SIMP_CCCD_DISC_START,
    SIMP_CCCD_DISC_V3_NOTIFY,
    SIMP_CCCD_DISC_V4_INDICATE,
    SIMP_CCCD_DISC_V5_CTL_PNT,
    SIMP_CCCD_DISC_END
}TSimpCCCD_DiscState, *PSimpCCCD_DiscState;
/** 
  * @}
  */

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Simple BLE client ID. */
static TClientID SimpClient_ClientID = AppProcessGeneralClientMsgID;
/**<  Simple BLE discovery end handle control. */
static TSimpClientHandleType SimpClient_EndHdlIdx = HDL_SIMBLE_CACHE_LEN;
/**<  Callback used to send data to app from Simple BLE client layer. */
static pfnSpecificClientAppCB_t pfnSimpClientAppCB = NULL;
/**<  Handle Cache for simple BLE client to cache discovered handles. */
static uint16_t SimpHdlCache[HDL_SIMBLE_CACHE_LEN] = {0};
/**<  Discovery State indicate which CCCD is in discovery. */
static TSimpCCCD_DiscState SimpClient_CccdDiscState = SIMP_CCCD_DISC_START;

/**
  * @brief  Used by application, to set the handles in Simple BLE handle cache.
  * @param  handle_type: handle types of this specific profile.
  * @param  handle_value: handle value to set.
  * @retval TRUE--set success.
  *         FALSE--set failed.
  */
bool SimpBleClient_SetHandle( TSimpClientHandleType handle_type, uint16_t handle_value)
{
    if(handle_type < HDL_SIMBLE_CACHE_LEN)
    {
        SimpHdlCache[handle_type] = handle_value;
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used by application, to get the handles in Simple BLE handle cache.
  * @param  handle_type: handle types of this specific profile.
  * @retval handle value.
  */
uint16_t SimpBleClient_GetHandle( TSimpClientHandleType handle_type )
{
    return SimpHdlCache[handle_type];
}

/**
  * @brief  Used by application, to start the discovery procedure of Simple BLE server.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_StartDiscovery( void )
{    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_StartDiscovery", 0);
    /* First clear handle cache. */
    memset(SimpHdlCache,0,sizeof(SimpHdlCache));

    if( clientAPI_ByUuidSrvDiscovery( SimpClient_ClientID, GATT_UUID_SIMPLE_PROFILE) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used internal, start the discovery of Simple BLE characteristics.
  *         NOTE--user can offer this interface for application if required.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
static bool SimpBleClient_StartCharDiscovery( void )
{
    TCharDiscReq charReq;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_StartCharDiscovery", 0);
    charReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_SRV_START];
    charReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_SRV_END];
    if( clientAPI_AllCharDiscovery( SimpClient_ClientID, charReq ) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used internal, start the discovery of Simple BLE characteristics descriptor.
  *         NOTE--user can offer this interface for application if required.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
static bool SimpBleClient_StartCharDescriptorDiscovery( TCharDescriptorDiscReq charDescriptorReq )
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_StartCharDescriptorDiscovery", 0);
    if( clientAPI_AllCharDescriptorDiscovery( SimpClient_ClientID, charDescriptorReq ) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used by application, read data from server by using handles.
  * @param  readCharType: one of characteristic that has the readable property.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_ReadByHandle( TSimpClientReadType readCharType )
{
    TReadHandleReq readHandle;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_ReadByHandle: charType = %d", 1, readCharType);
    
    switch(readCharType)
    {
        case SIMP_READ_REQ_V1_READ:
            if(SimpHdlCache[HDL_SIMBLE_V1_READ])
            {
                readHandle.wHandle = SimpHdlCache[HDL_SIMBLE_V1_READ];
                hdl_valid = TRUE;
            }
            break;
        case SIMP_READ_REQ_V3_NOTIFY_NOTIF_BIT:
            if(SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_CCCD])
            {
                readHandle.wHandle = SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_CCCD];
                hdl_valid = TRUE;
            }
            break;
        case SIMP_READ_REQ_V4_INDICATE_IND_BIT:
            if(SimpHdlCache[HDL_SIMBLE_V4_INDICATE_CCCD])
            {
                readHandle.wHandle = SimpHdlCache[HDL_SIMBLE_V4_INDICATE_CCCD];
                hdl_valid = TRUE;
            }
            break;
        case SIMP_READ_REQ_V5_CTL_PNT_IND_BIT:
            if(SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_CCCD])
            {
                readHandle.wHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_CCCD];
                hdl_valid = TRUE;
            }
            break;
        default:
            return FALSE;
    }
            
    if(hdl_valid)
    {
        if(clientAPI_AttribRead(SimpClient_ClientID, readHandle) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SimpBleClient_ReadByHandle: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, read data from server by using UUIDs.
  * @param  readCharType: one of characteristic that has the readable property.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_ReadByUUID( TSimpClientReadType readCharType )
{
    TReadUUIDReq readUUIDReq;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpClient_ReadByUUID: charType = %d", 1, readCharType);

    switch(readCharType)
    {
        case SIMP_READ_REQ_V1_READ:
            readUUIDReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_SRV_START];
            readUUIDReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_SRV_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_SIMPLE_V1_READ;
            break;
        case SIMP_READ_REQ_V3_NOTIFY_NOTIF_BIT:
            readUUIDReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_V3_NOTIFY] + 1;
            readUUIDReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            break;
        case SIMP_READ_REQ_V4_INDICATE_IND_BIT:
            readUUIDReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_V4_INDICATE] + 1;
            readUUIDReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_V4_INDICATE_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            break;
        case SIMP_READ_REQ_V5_CTL_PNT_IND_BIT:
            readUUIDReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT] + 1;
            readUUIDReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            break;
        default:
            return FALSE;
    }
    
    if( clientAPI_AttribReadUsingUUID( SimpClient_ClientID, readUUIDReq) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used by application, to enable or disable the notification of peer server's V3 Notify Characteristic.
  * @param  command: 0--disable the notification, 1--enable the notification.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_V3NotifyCmd( bool command )
{
    TWriteReq writeReq;
    uint16_t wCCCDBits;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_V3NotifyCmd: command = %d", 1, command);

    wCCCDBits = command ? 1 : 0;

    if(SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_CCCD])
    {
        writeReq.wHandle = SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_CCCD];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(uint16_t);
        writeReq.pData = (uint8_t *)&wCCCDBits;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( SimpClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SimpBleClient_V3NotifyCmd: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, to enable or disable the indication of peer server's V4 Indicate Characteristic.
  * @param  command: 0--disable the indication, 1--enable the indication.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_V4IndicateCmd( bool command )
{
    TWriteReq writeReq;
    uint16_t wCCCDBits;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_V4IndicateCmd: command = %d", 1, command);

    wCCCDBits = command ? 2 : 0;

    if(SimpHdlCache[HDL_SIMBLE_V4_INDICATE_CCCD])
    {
        writeReq.wHandle = SimpHdlCache[HDL_SIMBLE_V4_INDICATE_CCCD];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(uint16_t);
        writeReq.pData = (uint8_t *)&wCCCDBits;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( SimpClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SimpBleClient_V4IndicateCmd: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, to enable or disable the indication of peer server's V5 Control Point Characteristic.
  * @param  command: 0--disable the indication, 1--enable the indication.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_V5CtlPntIndicateCmd( bool command )
{
    TWriteReq writeReq;
    uint16_t wCCCDBits;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_V5CtlPntIndicateCmd: command = %d", 1, command);

    wCCCDBits = command ? 2 : 0;

    if(SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_CCCD])
    {
        writeReq.wHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_CCCD];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(uint16_t);
        writeReq.pData = (uint8_t *)&wCCCDBits;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( SimpClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SimpBleClient_V5CtlPntIndicateCmd: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used internal, to send write request to peer server's V5 Control Point Characteristic.
  * @param  ctl_pnt_ptr: pointer of control point data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
static bool SimpBleClient_CtlPntWrite( uint8_t *ctl_pnt_ptr )
{
    TWriteReq writeReq;
    bool hdl_valid = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_CtlPntWrite", 0);

    if(SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT])
    {
        writeReq.wHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = 3;
        writeReq.pData = ctl_pnt_ptr;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( SimpClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SimpBleClient_CtlPntWrite: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, write peer server's V4 Indicate char value, through control point.
  * @param  v4_ind_val: data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_CtlPntSetV4Indicate(uint8_t v4_ind_val)
{
    uint8_t control_point[3] = {0};
    uint8_t op_code = SIMPLE_CP_OPCODE_SET_CHAR_INDICATE_VAL;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_CtlPntSetV4Indicate", 0);

    /* Set Control Point's op code, v4 indicate value, length. */
    control_point[0] = op_code;
    control_point[1] = v4_ind_val;

    /* Send Write request. */
    return SimpBleClient_CtlPntWrite(control_point);
}

/**
  * @brief  Used by application, write peer server's V3 Notify char value, through control point.
  * @param  v3_notif_val: data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_CtlPntSetV3Notify(uint8_t v3_notif_val)
{
    uint8_t control_point[3] = {0};
    uint8_t op_code = SIMPLE_CP_OPCODE_SET_CHAR_NOTIFY_VAL;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_CtlPntSetV3Notify", 0);

    /* Set Control Point's op code, v3 notify value. */
    control_point[0] = op_code;
    control_point[1] = v3_notif_val;

    /* Send Write request. */
    return SimpBleClient_CtlPntWrite(control_point);
}

/**
  * @brief  Used by application, to send write request to peer server's V2 Write Characteristic.
  * @param  v2_write_val: data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool SimpBleClient_WriteV2WriteChar(uint8_t v2_write_val)
{
    TWriteReq writeReq;
    bool hdl_valid = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClient_WriteV2WriteChar", 0);

    if(SimpHdlCache[HDL_SIMBLE_V2_WRITE])
    {
        writeReq.wHandle = SimpHdlCache[HDL_SIMBLE_V2_WRITE];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(v2_write_val);
        writeReq.pData = &v2_write_val;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( SimpClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SimpBleClient_WriteV2WriteChar: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used internal, switch to the next CCCD handle to be discovered.
  *         NOTE--this function only used when peer service has more than one CCCD.
  * @param  pSwitchState: CCCD discovery state.
  * @retval none.
  */
static void SimpBleClient_SwitchNextDiscDescriptor(TSimpCCCD_DiscState *pSwitchState)
{
    TSimpCCCD_DiscState new_state;

    switch(*pSwitchState)
    {
        case SIMP_CCCD_DISC_START:
            if(SimpHdlCache[HDL_SIMBLE_V3_NOTIFY])
            {
                new_state = SIMP_CCCD_DISC_V3_NOTIFY;
            }
            else if(SimpHdlCache[HDL_SIMBLE_V4_INDICATE])
            {
                new_state = SIMP_CCCD_DISC_V4_INDICATE;
            }
            else if(SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT])
            {
                new_state = SIMP_CCCD_DISC_V5_CTL_PNT;
            }
            else
            {
                new_state = SIMP_CCCD_DISC_END;
            }
            break;
        case SIMP_CCCD_DISC_V3_NOTIFY:
            if(SimpHdlCache[HDL_SIMBLE_V4_INDICATE])
            {
                new_state = SIMP_CCCD_DISC_V4_INDICATE;
            }
            else if(SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT])
            {
                new_state = SIMP_CCCD_DISC_V5_CTL_PNT;
            }
            else
            {
                new_state = SIMP_CCCD_DISC_END;
            }
            break;
        case SIMP_CCCD_DISC_V4_INDICATE:
            if(SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT])
            {
                new_state = SIMP_CCCD_DISC_V5_CTL_PNT;
            }
            else
            {
                new_state = SIMP_CCCD_DISC_END;
            }
            break;
        default:
            new_state = SIMP_CCCD_DISC_END;
            break;
    }

    *pSwitchState = new_state;

}

/**
  * @brief  Called by profile client layer, when discover state of discovery procedure changed.
  * @param  discoveryState: current service discovery state.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult SimpBleClientDiscoverStateCb( TDiscoveryState discoveryState )
{
    bool cb_flag = FALSE;
    bool descriptor_disc_flag = FALSE;
    TAppResult appResult = AppResult_Success;
	TCharDescriptorDiscReq charDescriptorReq;
    TSimpClientCB_Data cb_data;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_DISC_STATE;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClientDiscoverStateCb: discoveryState = %d", 1, discoveryState);
    
    switch( discoveryState )
    {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if((SimpHdlCache[HDL_SIMBLE_SRV_START] != 0)
                || (SimpHdlCache[HDL_SIMBLE_SRV_END] != 0))
            {
                if(SimpBleClient_StartCharDiscovery() == FALSE)
                {
                    cb_data.cb_content.disc_state = SIMP_DISC_PENDING;
                    cb_flag = TRUE;
                }
            }
            /* No Simple BLE service handle found. Discover procedure complete. */
            else
            {
                cb_data.cb_content.disc_state = SIMP_DISC_DONE;
                cb_flag = TRUE;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            /* We should store the last char end handle if needed. */
            if(SimpClient_EndHdlIdx < HDL_SIMBLE_CACHE_LEN)
            {
                SimpHdlCache[SimpClient_EndHdlIdx] = SimpHdlCache[HDL_SIMBLE_SRV_END];
                SimpClient_EndHdlIdx = HDL_SIMBLE_CACHE_LEN;
            }
            /* Find the first descriptor to be discovered. */
            SimpClient_CccdDiscState = SIMP_CCCD_DISC_START;
            SimpBleClient_SwitchNextDiscDescriptor(&SimpClient_CccdDiscState);
            break;
        case DISC_STATE_CHAR_DESCRIPTOR_DONE:
            /* Find the next descriptor to be discovered. */
            SimpBleClient_SwitchNextDiscDescriptor(&SimpClient_CccdDiscState);
            break;
        default:
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "Invalid Discovery State!", 0);
            break;
    }

    /* Switch different char descriptor discovery, if has multi char descriptors. */
    switch(SimpClient_CccdDiscState)
    {
        case SIMP_CCCD_DISC_V3_NOTIFY:
            /* Need to discover v3 notify char descriptor. */
            charDescriptorReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_V3_NOTIFY] + 1;
            charDescriptorReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_END];
            descriptor_disc_flag = TRUE;
            break;
        case SIMP_CCCD_DISC_V4_INDICATE:
            /* Need to discover v4 indicate char descriptor. */
            charDescriptorReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_V4_INDICATE] + 1;
            charDescriptorReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_V4_INDICATE_END];
            descriptor_disc_flag = TRUE;
            break;
        case SIMP_CCCD_DISC_V5_CTL_PNT:
            /* Need to discover v5 control point char descriptor. */
            charDescriptorReq.wStartHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT] + 1;
            charDescriptorReq.wEndHandle = SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_END];
            descriptor_disc_flag = TRUE;
            break;
        case SIMP_CCCD_DISC_END:
            cb_data.cb_content.disc_state = SIMP_DISC_DONE;
            cb_flag = TRUE;
            break;
        default:
            /* No need to send char descriptor discovery. */
            break;
    }
    if(descriptor_disc_flag)
    {
        if(SimpBleClient_StartCharDescriptorDiscovery(charDescriptorReq) == FALSE)
        {
            cb_data.cb_content.disc_state = SIMP_DISC_PENDING;
            cb_flag = TRUE;
        }
    }

    /* Send discover state to application if needed. */
    if(cb_flag && pfnSimpClientAppCB)
    {
        appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when discover result fetched.
  * @param  resultType: indicate which type of value discovered in service discovery procedure.
  * @param  resultData: value discovered.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult SimpBleClientDiscoverResultCb( TDiscoverResultType resultType, TDiscoverResultData resultData )
{
    TAppResult appResult = AppResult_Success;
    bool char_cb_flag = FALSE;
    TSimpClientCB_Data cb_data;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_DISC_HDL;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClientDiscoverResultCb: resultType = %d", 1, resultType);
    
    switch( resultType )
    {
        case DISC_RESULT_SRV_DATA:
            /* send service handle range to application. */
            if(pfnSimpClientAppCB)
            {
                cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_SRV_START;
                cb_data.cb_content.disc_hdl_info.handle_value = resultData.pSrvDiscData->attHandle;
                (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
                cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_SRV_END;
                cb_data.cb_content.disc_hdl_info.handle_value = resultData.pSrvDiscData->endGroupHandle;
                appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
            }
            break;
        case DISC_RESULT_CHAR_UUID16:
            cb_data.cb_content.disc_hdl_info.handle_value = resultData.pCharUUID16DiscData->valueHandle;
            /* When use clientAPI_AllCharDiscovery. */
            if(SimpClient_EndHdlIdx < HDL_SIMBLE_CACHE_LEN)
            {
                SimpHdlCache[SimpClient_EndHdlIdx] = resultData.pCharUUID16DiscData->declHandle - 1;
                SimpClient_EndHdlIdx = HDL_SIMBLE_CACHE_LEN;
            }
            /* we should inform intrested handles to upper application. */
            switch(resultData.pCharUUID16DiscData->UUID16)
            {
                case GATT_UUID_CHAR_SIMPLE_V1_READ:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V1_READ;
                    char_cb_flag = TRUE;
                    break;
                case GATT_UUID_CHAR_SIMPLE_V2_WRITE:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V2_WRITE;
                    char_cb_flag = TRUE;
                    break;
                case GATT_UUID_CHAR_SIMPLE_V3_NOTIFY:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V3_NOTIFY;
                    char_cb_flag = TRUE;
                    SimpClient_EndHdlIdx = HDL_SIMBLE_V3_NOTIFY_END;
                    break;
                case GATT_UUID_CHAR_SIMPLE_V4_INDICATE:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V4_INDICATE;
                    char_cb_flag = TRUE;
                    SimpClient_EndHdlIdx = HDL_SIMBLE_V4_INDICATE_END;
                    break;
                case GATT_UUID_CHAR_SIMPLE_V5_CTL_PNT:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V5_CTL_PNT;
                    char_cb_flag = TRUE;
                    SimpClient_EndHdlIdx = HDL_SIMBLE_V5_CTL_PNT_END;
                    break;
                default:
                    /* have no intrest on this handle. */
                    break;
            }
            
            /* Inform application if needed. */
            if(char_cb_flag && pfnSimpClientAppCB)
            {
                appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
            }
            break;
        case DISC_RESULT_CHAR_DESC_UUID16:
            /* When use clientAPI_AllCharDescriptorDiscovery. */
            if(resultData.pCharDescUUID16DiscData->UUID16 == GATT_UUID_CHAR_CLIENT_CONFIG)
            {
                uint16_t temp_handle = resultData.pCharDescUUID16DiscData->handle;
                cb_data.cb_content.disc_hdl_info.handle_value = temp_handle;
                bool cccd_cb_flag = FALSE;
                if( (temp_handle > SimpHdlCache[HDL_SIMBLE_V3_NOTIFY])
                    && (temp_handle <= SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_END]) )
                {
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V3_NOTIFY_CCCD;
                    cccd_cb_flag = TRUE;
                }
                else if( (temp_handle > SimpHdlCache[HDL_SIMBLE_V4_INDICATE])
                    && (temp_handle <= SimpHdlCache[HDL_SIMBLE_V4_INDICATE_END]) )
                {
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V4_INDICATE_CCCD;
                    cccd_cb_flag = TRUE;
                }
                else if( (temp_handle > SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT])
                    && (temp_handle <= SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_END]) )
                {
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_SIMBLE_V5_CTL_PNT_CCCD;
                    cccd_cb_flag = TRUE;
                }
                else
                {
                    /* have no intrest in this handle. */
                }

                /* Inform application if needed. */
                if(cccd_cb_flag && pfnSimpClientAppCB)
                {
                    appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
                }
            }
            break;
        default:
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "Invalid Discovery Result Type!", 0);
            break;
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when read request responsed.
  * @param  reqResult: read request from peer device success or not.
  * @param  wHandle: handle of the value in read response.
  * @param  iValueSize: size of the value in read response.
  * @param  pValue: pointer to the value in read response.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult SimpBleClientReadResultCb( TClientRequestResult reqResult, uint16_t wHandle, int iValueSize, uint8_t *pValue )
{
    TAppResult appResult = AppResult_Success;
    TSimpClientCB_Data cb_data;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_READ_RESULT;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClientReadResultCb: result= %d, handle = 0x%4.4x, size = %d", 3, reqResult,wHandle,iValueSize);

    /* If read req success, branch to fetch value and send to application. */
    if(reqResult == CLIENT_REQ_SUCCESS)
    {
        if( wHandle == SimpHdlCache[HDL_SIMBLE_V1_READ] )
        {
            if(iValueSize == sizeof(uint8_t))
            {
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_V1_READ;
                cb_data.cb_content.read_data.read_content.v1_read = *(uint8_t *)pValue;
            }
            else
            {
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_FAIL;
            }
        }
        else if( wHandle == SimpHdlCache[HDL_SIMBLE_V3_NOTIFY_CCCD] )
        {
            if(iValueSize == sizeof(uint16_t))
            {
                uint16_t wCCCBits = *(uint16_t *)pValue;
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_V3_NOTIFY_NOTIF_BIT;
                if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
                {
                    cb_data.cb_content.read_data.read_content.v3_notify_notif_flag = TRUE;
                }
                else
                {
                    cb_data.cb_content.read_data.read_content.v3_notify_notif_flag = FALSE;
                }
            }
            else
            {
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_FAIL;
            }
        }
        else if( wHandle == SimpHdlCache[HDL_SIMBLE_V4_INDICATE_CCCD] )
        {
            if(iValueSize == sizeof(uint16_t))
            {
                uint16_t wCCCBits = *(uint16_t *)pValue;
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_V4_INDICATE_IND_BIT;
                if (wCCCBits & GATT_CCCD_INDICATION_BIT)
                {
                    cb_data.cb_content.read_data.read_content.v4_indicate_ind_flag = TRUE;
                }
                else
                {
                    cb_data.cb_content.read_data.read_content.v4_indicate_ind_flag = FALSE;
                }
            }
            else
            {
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_FAIL;
            }
        }
        else if( wHandle == SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT_CCCD] )
        {
            if(iValueSize == sizeof(uint16_t))
            {
                uint16_t wCCCBits = *(uint16_t *)pValue;
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_V5_CTL_PNT_IND_BIT;
                if (wCCCBits & GATT_CCCD_INDICATION_BIT)
                {
                    cb_data.cb_content.read_data.read_content.v5_ctl_pnt_ind_flag = TRUE;
                }
                else
                {
                    cb_data.cb_content.read_data.read_content.v5_ctl_pnt_ind_flag = FALSE;
                }
            }
            else
            {
                cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_FAIL;
            }
        }
        else
        {
            cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_FAIL;
        }
    }
    /* Read req fail, inform application. */
    else
    {
        cb_data.cb_content.read_data.read_result = SIMP_READ_RESULT_FAIL;
    }
    /* Inform application the read result. */
    if(pfnSimpClientAppCB)
    {
        appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when write request complete.
  * @param  reqResult: write request send success or not.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult SimpBleClientWriteResultCb( TClientRequestResult reqResult )
{
    TAppResult appResult = AppResult_Success;
    TSimpClientCB_Data cb_data;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_WRITE_RESULT;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClientWriteResultCb: result = %d", 1, reqResult);

    /* If write req success, branch to fetch value and send to application. */
    if(reqResult == CLIENT_REQ_SUCCESS)
    {
        cb_data.cb_content.write_result = SIMP_WRITE_RESULT_SUCCESS;
    }
    /* Read req fail, inform application. */
    else
    {
        cb_data.cb_content.write_result = SIMP_WRITE_RESULT_FAIL;
    }
    /* Inform application the write result. */
    if(pfnSimpClientAppCB)
    {
        appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when notification or indication arrived.
  * @param  wHandle: handle of the value in received data.
  * @param  iValueSize: size of the value in received data.
  * @param  pValue: pointer to the value in received data.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult SimpBleClientNotifIndResultCb( uint16_t wHandle, int iValueSize, uint8_t *pValue )
{    
    TAppResult appResult = AppResult_Success;
    TSimpClientCB_Data cb_data;
    cb_data.cb_type = SIMP_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClientNotifIndResultCb: handle = 0x%4.4x", 1, wHandle);

    if ( wHandle == SimpHdlCache[HDL_SIMBLE_V3_NOTIFY] )
    {
        if(iValueSize <= sizeof(uint8_t))
        {
            cb_data.cb_content.notif_ind_data.receive_type = SIMP_NOTIF_RECEIVE_V3_NOTIFY;
            cb_data.cb_content.notif_ind_data.receive_content.v3_notify = *(uint8_t *)pValue;
        }
        else
        {
            cb_data.cb_content.notif_ind_data.receive_type = SIMP_NOTIF_IND_RECEIVE_FAIL;
        }
    }
    else if( wHandle == SimpHdlCache[HDL_SIMBLE_V4_INDICATE] )
    {
        if(iValueSize <= sizeof(uint8_t))
        {
            cb_data.cb_content.notif_ind_data.receive_type = SIMP_IND_RECEIVE_V4_INDICATE;
            cb_data.cb_content.notif_ind_data.receive_content.v4_indicate = *(uint8_t *)pValue;
        }
        else
        {
            cb_data.cb_content.notif_ind_data.receive_type = SIMP_NOTIF_IND_RECEIVE_FAIL;
        }
    }
    else if( wHandle == SimpHdlCache[HDL_SIMBLE_V5_CTL_PNT] )
    {
        if(iValueSize <= 3)
        {
            cb_data.cb_content.notif_ind_data.receive_type = SIMP_IND_RECEIVE_V5_CTL_PNT;
            memcpy(cb_data.cb_content.notif_ind_data.receive_content.v5_control_point, pValue, iValueSize);
        }
        else
        {
            cb_data.cb_content.notif_ind_data.receive_type = SIMP_NOTIF_IND_RECEIVE_FAIL;
        }
    }
    else
    {
        cb_data.cb_content.notif_ind_data.receive_type = SIMP_NOTIF_IND_RECEIVE_FAIL;
    }
    /* Inform application the notif/ind result. */
    if(pfnSimpClientAppCB)
    {
        appResult = (*pfnSimpClientAppCB)(SimpClient_ClientID, &cb_data);
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when link disconnected.
  *         NOTE--we should reset some state when disconnected.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult SimpBleClientDisconnectCb( void )
{    
    TProfileResult result = ProfileResult_Success;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleClientDisconnectCb.", 0);

    /* Reset some params, when disconnection occurs. */
    SimpClient_EndHdlIdx = HDL_SIMBLE_CACHE_LEN;
    SimpClient_CccdDiscState = SIMP_CCCD_DISC_START;
    
    /* There is no need to inform application, because GAP has done. */

    return (result);
}

/**
 * @brief Simple BLE Client Callbacks.
*/
CONST TGATTClientCBs SimpBleClientCBs =
{
    SimpBleClientDiscoverStateCb,   //!< Discovery State callback function pointer
    SimpBleClientDiscoverResultCb,  //!< Discovery result callback function pointer
    SimpBleClientReadResultCb,      //!< Read response callback function pointer
    SimpBleClientWriteResultCb,     //!< Write result callback function pointer
    SimpBleClientNotifIndResultCb,  //!< Notify Indicate callback function pointer
    SimpBleClientDisconnectCb       //!< Link disconnection callback function pointer
};

/**
  * @brief  add Simple BLE client to application.
  * @param  appCB: pointer of app callback function to handle specific client module data.
  * @retval Client ID of the specific client module.
  */
TClientID SimpBle_AddClient( pfnSpecificClientAppCB_t appCB )
{
    TClientID simp_client_id;
    if ( FALSE == clientAPI_RegisterSpecClientCB( &simp_client_id, SimpHdlCache, &SimpBleClientCBs ) )
    {
        simp_client_id = AppProcessGeneralClientMsgID;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "SimpBle_AddClient Fail !!!", 0);
        return simp_client_id;
    }
    SimpClient_ClientID = simp_client_id;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBle_AddClient: client ID = %d", 1, SimpClient_ClientID);

    /* register callback for profile to inform application that some events happened. */
    pfnSimpClientAppCB = appCB;
    
    return simp_client_id;
}

