enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rsc_client.c
* @brief    RSC client source file.
* @details  Interfaces to discover rsc service related handles.
* @author   ethan_su
* @date     2015-7-20
* @version  v0.1
*********************************************************************************************************
*/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "rsc_client.h"

/**
  * @{ Used for CCCD handle discovering in discovery procedure
  */
typedef enum
{
    RSC_CCCD_DISC_START,
    RSC_CCCD_DISC_MEAS,
    RSC_CCCD_DISC_CTL_PNT,
    RSC_CCCD_DISC_END
}TRscCCCD_DiscState, *PRscCCCD_DiscState;
/** 
  * @}
  */

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  RSC client ID. */
static TClientID RscClient_ClientID = AppProcessGeneralClientMsgID;
/**<  RSC discovery end handle control. */
static TRscClientHandleType RscClient_EndHdlIdx = HDL_RSC_CACHE_LEN;
/**<  Callback used to send data to app from RSC client layer. */
static pfnSpecificClientAppCB_t pfnRscClientAppCB = NULL;
/**<  Handle Cache for simple BLE client to cache discovered handles. */
static uint16_t RscHdlCache[HDL_RSC_CACHE_LEN] = {0};
/**<  Discovery State indicate which CCCD is in discovery. */
static TRscCCCD_DiscState RscClient_CccdDiscState = RSC_CCCD_DISC_START;

/**
  * @brief  Used by application, to set the handles in RSC handle cache.
  * @param  handle_type: handle types of this specific profile.
  * @param  handle_value: handle value to set.
  * @retval TRUE--set success.
  *         FALSE--set failed.
  */
bool RSCClient_SetHandle( TRscClientHandleType handle_type, uint16_t handle_value)
{
    if(handle_type < HDL_RSC_CACHE_LEN)
    {
        RscHdlCache[handle_type] = handle_value;
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used by application, to get the handles in RSC handle cache.
  * @param  handle_type: handle types of this specific profile.
  * @retval handle value.
  */
uint16_t RSCClient_GetHandle( TRscClientHandleType handle_type )
{
    return RscHdlCache[handle_type];
}

/**
  * @brief  Used by application, when application receive the measurement notification from
  *         the RSC server, can use this function to split the individual data.
  * @param  pRscMeas: pointer to rsc measurement data to be splited.
  * @param[out]  pFlag: flag field of measurement.
  * @param[out]  pSpeed: speed field of measurement.
  * @param[out]  pCadence: cadence field of measurement.
  * @param[out]  pStrideLength: stride length field of measurement.
  * @param[out]  pTotalDistance: total distance field of measurement.
  * @retval None
  */
void RSCClient_MeasSplit(PRSCMeasurement pRscMeas,
                         uint8_t *pFlag,
                         uint16_t *pSpeed,
                         uint8_t *pCadence,
                         uint16_t *pStrideLength,
                         uint32_t *pTotalDistance)
{
    /* Get flag, speed, cadence from measurement struct. */
    *pFlag = pRscMeas->value[0];
    *pSpeed = LE_EXTRN2WORD(pRscMeas->value + 1);
    *pCadence = pRscMeas->value[3];

    /* Get stride length from measurement struct. */
    if ( *pFlag & RSC_INC_STRIDE_LENGTH_MASK )
    {
        *pStrideLength = LE_EXTRN2WORD(pRscMeas->value + 4);
    }
    else
    {
        *pStrideLength = 0;
    }

    /* Get total distance from measurement struct. */
    if ( *pFlag & RSC_INC_TOTAL_DISTANCE_MASK )
    {
        if ( *pFlag & RSC_INC_STRIDE_LENGTH_MASK )
        {
            *pTotalDistance = LE_EXTRN2DWORD(pRscMeas->value + 6);
        }
        else
        {
            *pTotalDistance = LE_EXTRN2DWORD(pRscMeas->value + 4);
        }
    }
    else
    {
        *pTotalDistance = 0;
    }
}

/**
  * @brief  Used by application, when application receive the control point indication from
  *         the RSC server, can use this function to split the individual data.
  * @param  pRscCtlPnt: pointer to rsc control point data to be splited.
  * @param[out]  pReqOpCode: opcode field of control point.
  * @param[out]  pRespValue: response code field of measurement.
  * @param[out]  pSensLocList: sensor location list field of measurement if exist.
  * @retval TRUE--split OK.
  *         FALSE--split failed.
  */
bool RSCClient_CtlPntSplit(PRSCControlPoint pRscCtlPnt,
                           uint8_t *pReqOpCode,
                           uint8_t *pRespValue,
                           PRscClientSuppSensList pSensLocList)
{
    /* Get op code from control point struct. */
    uint8_t op_code = pRscCtlPnt->value[0];

    if(op_code == RSC_CP_OPCODE_RSP_CODE)
    {
        *pReqOpCode = pRscCtlPnt->value[1];
        *pRespValue = pRscCtlPnt->value[2];
        
        if((*pReqOpCode == RSC_CP_OPCODE_REQ_SENS_LOC_LIST)
            && (*pRespValue == RSC_CP_RSPCODE_SUCCESS))
        {
            pSensLocList->list_num = pRscCtlPnt->cur_length - 3;
            memcpy(pSensLocList->list_value,pRscCtlPnt->value + 3,pSensLocList->list_num);
        }
        else
        {
            pSensLocList->list_num = 0;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
  * @brief  Used by application, to start the discovery procedure of RSC server.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_StartDiscovery( void )
{    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_StartDiscovery", 0);
    /* First clear handle cache. */
    memset(RscHdlCache,0,sizeof(RscHdlCache));

    if( clientAPI_ByUuidSrvDiscovery( RscClient_ClientID, GATT_UUID_RSC ) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used internal, start the discovery of RSC characteristics.
  *         NOTE--user can offer this interface for application if required.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
static bool RSCClient_StartCharDiscovery( void )
{
    TCharDiscReq charReq;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_StartCharDiscovery", 0);
    charReq.wStartHandle = RscHdlCache[HDL_RSC_SRV_START];
    charReq.wEndHandle = RscHdlCache[HDL_RSC_SRV_END];
    if( clientAPI_AllCharDiscovery( RscClient_ClientID, charReq ) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used internal, start the discovery of RSC characteristics descriptor.
  *         NOTE--user can offer this interface for application if required.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
static bool RSCClient_StartCharDescriptorDiscovery( TCharDescriptorDiscReq charDescriptorReq )
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_StartCharDescriptorDiscovery", 0);
    if( clientAPI_AllCharDescriptorDiscovery( RscClient_ClientID, charDescriptorReq ) == ProfileResult_Success )
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
bool RSCClient_ReadByHandle( TRscClientReadType readCharType )
{
    TReadHandleReq readHandle;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_ReadByHandle: charType = %d", 1, readCharType);
    
    switch(readCharType)
    {
        case RSC_READ_REQ_MEAS_NOTIF_BIT:
            if(RscHdlCache[HDL_RSC_MEAS_CCCD])
            {
                readHandle.wHandle = RscHdlCache[HDL_RSC_MEAS_CCCD];
                hdl_valid = TRUE;
            }
            break;
        case RSC_READ_REQ_CTL_PNT_IND_BIT:
            if(RscHdlCache[HDL_RSC_CTL_PNT_CCCD])
            {
                readHandle.wHandle = RscHdlCache[HDL_RSC_CTL_PNT_CCCD];
                hdl_valid = TRUE;
            }
            break;
        case RSC_READ_REQ_FEATURE:
            if(RscHdlCache[HDL_RSC_FEATURE])
            {
                readHandle.wHandle = RscHdlCache[HDL_RSC_FEATURE];
                hdl_valid = TRUE;
            }
            break;
        case RSC_READ_REQ_SENSOR_LOC:
            if(RscHdlCache[HDL_RSC_SENSOR_LOCATION])
            {
                readHandle.wHandle = RscHdlCache[HDL_RSC_SENSOR_LOCATION];
                hdl_valid = TRUE;
            }
            break;
        default:
            return FALSE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribRead(RscClient_ClientID, readHandle) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "RSCClient_ReadByHandle: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, read data from server by using UUIDs.
  * @param  readCharType: one of characteristic that has the readable property.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_ReadByUUID( TRscClientReadType readCharType )
{
    TReadUUIDReq readUUIDReq;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_ReadByUUID: charType = %d", 1, readCharType);

    switch(readCharType)
    {
        case RSC_READ_REQ_MEAS_NOTIF_BIT:
            readUUIDReq.wStartHandle = RscHdlCache[HDL_RSC_MEAS] + 1;
            readUUIDReq.wEndHandle = RscHdlCache[HDL_RSC_MEAS_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            break;
        case RSC_READ_REQ_CTL_PNT_IND_BIT:
            readUUIDReq.wStartHandle = RscHdlCache[HDL_RSC_CTL_PNT] + 1;
            readUUIDReq.wEndHandle = RscHdlCache[HDL_RSC_CTL_PNT_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            break;
        case RSC_READ_REQ_FEATURE:
            readUUIDReq.wStartHandle = RscHdlCache[HDL_RSC_SRV_START];
            readUUIDReq.wEndHandle = RscHdlCache[HDL_RSC_SRV_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_RSC_FEATURE;
            break;
        case RSC_READ_REQ_SENSOR_LOC:
            readUUIDReq.wStartHandle = RscHdlCache[HDL_RSC_SRV_START];
            readUUIDReq.wEndHandle = RscHdlCache[HDL_RSC_SRV_END];
            readUUIDReq.UUID16 = GATT_UUID_CHAR_SENSOR_LOCATION;
            break;
        default:
            return FALSE;
    }
    
    if( clientAPI_AttribReadUsingUUID( RscClient_ClientID, readUUIDReq) == ProfileResult_Success )
    {
        return TRUE;
    }
    return FALSE;
}

/**
  * @brief  Used by application, to enable or disable the notification of peer server's RSC measurement Characteristic.
  * @param  command: 0--disable the notification, 1--enable the notification.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_MeasNotifyCmd( bool command )
{
    TWriteReq writeReq;
    uint16_t wCCCDBits;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_MeasNotifyCmd: command = %d", 1, command);

    wCCCDBits = command ? 1 : 0;

    if(RscHdlCache[HDL_RSC_MEAS_CCCD])
    {
        writeReq.wHandle = RscHdlCache[HDL_RSC_MEAS_CCCD];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(uint16_t);
        writeReq.pData = (uint8_t *)&wCCCDBits;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( RscClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "RSCClient_MeasNotifyCmd: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, to enable or disable the indication of peer server's RSC control point Characteristic.
  * @param  command: 0--disable the indication, 1--enable the indication.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_CtlPntIndicateCmd( bool command )
{
    TWriteReq writeReq;
    uint16_t wCCCDBits;
    bool hdl_valid = FALSE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_CtlPntIndicateCmd: command = %d", 1, command);

    wCCCDBits = command ? 2 : 0;

    if(RscHdlCache[HDL_RSC_CTL_PNT_CCCD])
    {
        writeReq.wHandle = RscHdlCache[HDL_RSC_CTL_PNT_CCCD];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = sizeof(uint16_t);
        writeReq.pData = (uint8_t *)&wCCCDBits;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( RscClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "RSCClient_CtlPntIndicateCmd: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used internal, to send write request to peer server's RSC Control Point Characteristic.
  * @param  ctl_pnt_ptr: pointer of control point data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
static bool RSCClient_CtlPntWrite( PRSCControlPoint ctl_pnt_ptr )
{
    TWriteReq writeReq;
    bool hdl_valid = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_CtlPntWrite", 0);

    if(RscHdlCache[HDL_RSC_CTL_PNT])
    {
        writeReq.wHandle = RscHdlCache[HDL_RSC_CTL_PNT];
        writeReq.wType = blueAPI_GATTWriteTypeRequest;
        writeReq.wLength = ctl_pnt_ptr->cur_length;
        writeReq.pData = ctl_pnt_ptr->value;
        hdl_valid = TRUE;
    }

    if(hdl_valid)
    {
        if(clientAPI_AttribWrite( RscClient_ClientID, &writeReq ) == ProfileResult_Success)
        {
            return TRUE;
        }
    }
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "RSCClient_CtlPntWrite: Request fail! Please check!", 0);
    return FALSE;
}

/**
  * @brief  Used by application, send cumulative value write request to RSC server.
  * @param  total_distance: data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_CtlPntSetCumulValue( uint32_t total_distance )
{
    TRSCControlPoint control_point;
    uint8_t op_code = RSC_CP_OPCODE_SET_CUMULATIVE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_CtlPntSetCumulValue", 0);

    /* Set Control Point's op code, cumulative value, length. */
    control_point.value[0] = op_code;
    LE_DWORD2EXTRN(control_point.value + 1, total_distance);
    control_point.cur_length = sizeof(op_code) + sizeof(total_distance);

    /* Send Write request. */
    return RSCClient_CtlPntWrite(&control_point);
}

/**
  * @brief  Used by application, send start sensor calibration write request to RSC server.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_CtlPntStartSensorCalib( void )
{
    TRSCControlPoint control_point;
    uint8_t op_code = RSC_CP_OPCODE_START_CALIBRATION;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_CtlPntStartSensorCalib", 0);

    /* Set Control Point's op code, length(no parameters). */
    control_point.value[0] = op_code;
    control_point.cur_length = sizeof(op_code);

    /* Send Write request. */
    return RSCClient_CtlPntWrite(&control_point);
}

/**
  * @brief  Used by application, send update sensor location write request to RSC server.
  * @param  sensor_location: data to write.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_CtlPntUpdateSensorLocation( uint8_t sensor_location )
{
    TRSCControlPoint control_point;
    uint8_t op_code = RSC_CP_OPCODE_UPDATE_SENS_LOC;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_CtlPntUpdateSensorLocation", 0);

    /* Set Control Point's op code, sensor location, length. */
    control_point.value[0] = op_code;
    control_point.value[1] = sensor_location;
    control_point.cur_length = sizeof(op_code) + sizeof(sensor_location);

    /* Send Write request. */
    return RSCClient_CtlPntWrite(&control_point);
}

/**
  * @brief  Used by application, send request supported sensor location list write request to RSC server.
  * @retval TRUE--send request to upper stack success.
  *         FALSE--send request to upper stack failed.
  */
bool RSCClient_CtlPntReqSensorLocationList( void )
{
    TRSCControlPoint control_point;
    uint8_t op_code = RSC_CP_OPCODE_REQ_SENS_LOC_LIST;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCClient_CtlPntReqSensorLocationList", 0);

    /* Set Control Point's op code, length(no parameters). */
    control_point.value[0] = op_code;
    control_point.cur_length = sizeof(op_code);

    /* Send Write request. */
    return RSCClient_CtlPntWrite(&control_point);
}

/**
  * @brief  Used internal, switch to the next CCCD handle to be discovered.
  *         NOTE--this function only used when peer service has more than one CCCD.
  * @param  pSwitchState: CCCD discovery state.
  * @retval none.
  */
static void RSCClient_SwitchNextDiscDescriptor(TRscCCCD_DiscState *pSwitchState)
{
    TRscCCCD_DiscState new_state;

    switch(*pSwitchState)
    {
        case RSC_CCCD_DISC_START:
            if(RscHdlCache[HDL_RSC_MEAS])
            {
                new_state = RSC_CCCD_DISC_MEAS;
            }
            else if(RscHdlCache[HDL_RSC_CTL_PNT])
            {
                new_state = RSC_CCCD_DISC_CTL_PNT;
            }
            else
            {
                new_state = RSC_CCCD_DISC_END;
            }
            break;
        case RSC_CCCD_DISC_MEAS:
            if(RscHdlCache[HDL_RSC_CTL_PNT])
            {
                new_state = RSC_CCCD_DISC_CTL_PNT;
            }
            else
            {
                new_state = RSC_CCCD_DISC_END;
            }
            break;
        default:
            new_state = RSC_CCCD_DISC_END;
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
static TProfileResult RscClientDiscoverStateCb( TDiscoveryState discoveryState )
{
    bool cb_flag = FALSE;
    bool descriptor_disc_flag = FALSE;
    TAppResult appResult = AppResult_Success;
	TCharDescriptorDiscReq charDescriptorReq;
    TRscClientCB_Data cb_data;
    cb_data.cb_type = RSC_CLIENT_CB_TYPE_DISC_STATE;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RscClientDiscoverStateCb: discoveryState = %d", 1, discoveryState);
    
    switch( discoveryState )
    {
        case DISC_STATE_SRV_DONE:
            /* Indicate that service handle found. Start discover characteristic. */
            if((RscHdlCache[HDL_RSC_SRV_START] != 0)
                || (RscHdlCache[HDL_RSC_SRV_END] != 0))
            {
                if(RSCClient_StartCharDiscovery() == FALSE)
                {
                    cb_data.cb_content.disc_state = RSC_DISC_PENDING;
                    cb_flag = TRUE;
                }
            }
            /* No RSC service handle found. Discover procedure complete. */
            else
            {
                cb_data.cb_content.disc_state = RSC_DISC_DONE;
                cb_flag = TRUE;
            }
            break;
        case DISC_STATE_CHAR_DONE:
            /* We should store the last char end handle if needed. */
            if(RscClient_EndHdlIdx < HDL_RSC_CACHE_LEN)
            {
                RscHdlCache[RscClient_EndHdlIdx] = RscHdlCache[HDL_RSC_SRV_END];
                RscClient_EndHdlIdx = HDL_RSC_CACHE_LEN;
            }
            /* Find the first descriptor to be discovered. */
            RscClient_CccdDiscState = RSC_CCCD_DISC_START;
            RSCClient_SwitchNextDiscDescriptor(&RscClient_CccdDiscState);
            break;
        case DISC_STATE_CHAR_DESCRIPTOR_DONE:
            /* Find the next descriptor to be discovered. */
            RSCClient_SwitchNextDiscDescriptor(&RscClient_CccdDiscState);
            break;
        default:
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "Invalid Discovery State!", 0);
            break;
    }

    /* Switch different char descriptor discovery, if has multi char descriptors. */
    switch(RscClient_CccdDiscState)
    {
        case RSC_CCCD_DISC_MEAS:
            /* Need to discover measurement char descriptor. */
            charDescriptorReq.wStartHandle = RscHdlCache[HDL_RSC_MEAS] + 1;
            charDescriptorReq.wEndHandle = RscHdlCache[HDL_RSC_MEAS_END];
            descriptor_disc_flag = TRUE;
            break;
        case RSC_CCCD_DISC_CTL_PNT:
            /* Need to discover control point char descriptor. */
            charDescriptorReq.wStartHandle = RscHdlCache[HDL_RSC_CTL_PNT] + 1;
            charDescriptorReq.wEndHandle = RscHdlCache[HDL_RSC_CTL_PNT_END];
            descriptor_disc_flag = TRUE;
            break;
        case RSC_CCCD_DISC_END:
            cb_data.cb_content.disc_state = RSC_DISC_DONE;
            cb_flag = TRUE;
            break;
        default:
            /* No need to send char descriptor discovery. */
            break;
    }
    if(descriptor_disc_flag)
    {
        if(RSCClient_StartCharDescriptorDiscovery(charDescriptorReq) == FALSE)
        {
            cb_data.cb_content.disc_state = RSC_DISC_PENDING;
            cb_flag = TRUE;
        }
    }

    /* Send discover state to application if needed. */
    if(cb_flag && pfnRscClientAppCB)
    {
        appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
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
static TProfileResult RscClientDiscoverResultCb( TDiscoverResultType resultType, TDiscoverResultData resultData )
{
    TAppResult appResult = AppResult_Success;
    bool char_cb_flag = FALSE;
    TRscClientCB_Data cb_data;
    cb_data.cb_type = RSC_CLIENT_CB_TYPE_DISC_HDL;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RscClientDiscoverResultCb: resultType = %d", 1, resultType);
    
    switch( resultType )
    {
        case DISC_RESULT_SRV_DATA:
            /* send service handle range to application. */
            if(pfnRscClientAppCB)
            {
                cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_SRV_START;
                cb_data.cb_content.disc_hdl_info.handle_value = resultData.pSrvDiscData->attHandle;
                (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
                cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_SRV_END;
                cb_data.cb_content.disc_hdl_info.handle_value = resultData.pSrvDiscData->endGroupHandle;
                appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
            }
            break;
        case DISC_RESULT_CHAR_UUID16:
            cb_data.cb_content.disc_hdl_info.handle_value = resultData.pCharUUID16DiscData->valueHandle;
            /* When use clientAPI_AllCharDiscovery. */
            if(RscClient_EndHdlIdx < HDL_RSC_CACHE_LEN)
            {
                RscHdlCache[RscClient_EndHdlIdx] = resultData.pCharUUID16DiscData->declHandle - 1;
                RscClient_EndHdlIdx = HDL_RSC_CACHE_LEN;
            }
            /* we should inform intrested handles to upper application. */
            switch(resultData.pCharUUID16DiscData->UUID16)
            {
                case GATT_UUID_CHAR_RSC_MEASUREMENT:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_MEAS;
                    char_cb_flag = TRUE;
                    RscClient_EndHdlIdx = HDL_RSC_MEAS_END;
                    break;
                case GATT_UUID_CHAR_RSC_FEATURE:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_FEATURE;
                    char_cb_flag = TRUE;
                    break;
                case GATT_UUID_CHAR_SENSOR_LOCATION:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_SENSOR_LOCATION;
                    char_cb_flag = TRUE;
                    break;
                case GATT_UUID_CHAR_SC_CONTROL_POINT:
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_CTL_PNT;
                    char_cb_flag = TRUE;
                    RscClient_EndHdlIdx = HDL_RSC_CTL_PNT_END;
                    break;
                default:
                    /* have no intrest on this handle. */
                    break;
            }
            
            /* Inform application if needed. */
            if(char_cb_flag && pfnRscClientAppCB)
            {
                appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
            }
            break;
        case DISC_RESULT_CHAR_DESC_UUID16:
            /* When use clientAPI_AllCharDescriptorDiscovery. */
            if(resultData.pCharDescUUID16DiscData->UUID16 == GATT_UUID_CHAR_CLIENT_CONFIG)
            {
                uint16_t temp_handle = resultData.pCharDescUUID16DiscData->handle;
                cb_data.cb_content.disc_hdl_info.handle_value = temp_handle;
                bool cccd_cb_flag = FALSE;
                if( (temp_handle > RscHdlCache[HDL_RSC_MEAS])
                    && (temp_handle <= RscHdlCache[HDL_RSC_MEAS_END]) )
                {
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_MEAS_CCCD;
                    cccd_cb_flag = TRUE;
                }
                else if( (temp_handle > RscHdlCache[HDL_RSC_CTL_PNT])
                    && (temp_handle <= RscHdlCache[HDL_RSC_CTL_PNT_END]) )
                {
                    cb_data.cb_content.disc_hdl_info.handle_type = HDL_RSC_CTL_PNT_CCCD;
                    cccd_cb_flag = TRUE;
                }
                else
                {
                    /* have no intrest in this handle. */
                }

                /* Inform application if needed. */
                if(cccd_cb_flag && pfnRscClientAppCB)
                {
                    appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
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
static TProfileResult RscClientReadResultCb( TClientRequestResult reqResult, uint16_t wHandle, int iValueSize, uint8_t *pValue )
{
    TAppResult appResult = AppResult_Success;
    TRscClientCB_Data cb_data;
    cb_data.cb_type = RSC_CLIENT_CB_TYPE_READ_RESULT;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RscClientReadResultCb: result= %d, handle = 0x%4.4x, size = %d", 3, reqResult,wHandle,iValueSize);

    /* If read req success, branch to fetch value and send to application. */
    if(reqResult == CLIENT_REQ_SUCCESS)
    {
        if( wHandle == RscHdlCache[HDL_RSC_MEAS_CCCD] )
        {
            if(iValueSize == sizeof(uint16_t))
            {
                uint16_t wCCCBits = *(uint16_t *)pValue;
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_MEAS_NOTIF_BIT;
                if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
                {
                    cb_data.cb_content.read_data.read_content.meas_notif_flag = TRUE;
                }
                else
                {
                    cb_data.cb_content.read_data.read_content.meas_notif_flag = FALSE;
                }
            }
            else
            {
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FAIL;
            }
        }
        else if( wHandle == RscHdlCache[HDL_RSC_FEATURE] )
        {
            if(iValueSize == sizeof(uint16_t))
            {
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FEATURE;
                cb_data.cb_content.read_data.read_content.feature = *(uint16_t *)pValue;
            }
            else
            {
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FAIL;
            }
        }
        else if( wHandle == RscHdlCache[HDL_RSC_CTL_PNT_CCCD] )
        {
            if(iValueSize == sizeof(uint16_t))
            {
                uint16_t wCCCBits = *(uint16_t *)pValue;
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_CTL_PNT_IND_BIT;
                if (wCCCBits & GATT_CCCD_INDICATION_BIT)
                {
                    cb_data.cb_content.read_data.read_content.ctl_pnt_ind_flag = TRUE;
                }
                else
                {
                    cb_data.cb_content.read_data.read_content.ctl_pnt_ind_flag = FALSE;
                }
            }
            else
            {
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FAIL;
            }
        }
        else if( wHandle == RscHdlCache[HDL_RSC_SENSOR_LOCATION] )
        {
            if(iValueSize == sizeof(uint8_t))
            {
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_SENSOR_LOC;
                cb_data.cb_content.read_data.read_content.sens_location = *(uint8_t *)pValue;
            }
            else
            {
                cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FAIL;
            }
        }
        else
        {
            cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FAIL;
        }
    }
    /* Read req fail, inform application. */
    else
    {
        cb_data.cb_content.read_data.read_result = RSC_READ_RESULT_FAIL;
    }
    /* Inform application the read result. */
    if(pfnRscClientAppCB)
    {
        appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when write request complete.
  * @param  reqResult: write request send success or not.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult RscClientWriteResultCb( TClientRequestResult reqResult )
{
    TAppResult appResult = AppResult_Success;
    TRscClientCB_Data cb_data;
    cb_data.cb_type = RSC_CLIENT_CB_TYPE_WRITE_RESULT;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RscClientWriteResultCb: result = %d", 1, reqResult);

    /* If write req success, branch to fetch value and send to application. */
    if(reqResult == CLIENT_REQ_SUCCESS)
    {
        cb_data.cb_content.write_result = RSC_WRITE_RESULT_SUCCESS;
    }
    /* Read req fail, inform application. */
    else
    {
        cb_data.cb_content.write_result = RSC_WRITE_RESULT_FAIL;
    }
    /* Inform application the write result. */
    if(pfnRscClientAppCB)
    {
        appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
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
static TProfileResult RscClientNotifIndResultCb( uint16_t wHandle, int iValueSize, uint8_t *pValue )
{    
    TAppResult appResult = AppResult_Success;
    TRscClientCB_Data cb_data;
    cb_data.cb_type = RSC_CLIENT_CB_TYPE_NOTIF_IND_RESULT;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RscClientNotifIndResultCb: handle = 0x%4.4x", 1, wHandle);

    if ( wHandle == RscHdlCache[HDL_RSC_MEAS] )
    {
        if(iValueSize <= RSC_MAX_MEASUREMENT_VALUE)
        {
            cb_data.cb_content.notif_ind_data.receive_type = RSC_NOTIF_RECEIVE_MEAS;
            cb_data.cb_content.notif_ind_data.receive_content.measurement.cur_length = iValueSize;
            memcpy(cb_data.cb_content.notif_ind_data.receive_content.measurement.value, pValue, iValueSize);
        }
        else
        {
            cb_data.cb_content.notif_ind_data.receive_type = RSC_NOTIF_IND_RECEIVE_FAIL;
        }
    }
    else if( wHandle == RscHdlCache[HDL_RSC_CTL_PNT] )
    {
        if(iValueSize <= RSC_MAX_CTL_PNT_VALUE)
        {
            cb_data.cb_content.notif_ind_data.receive_type = RSC_IND_RECEIVE_CTL_PNT;
            cb_data.cb_content.notif_ind_data.receive_content.control_point.cur_length = iValueSize;
            memcpy(cb_data.cb_content.notif_ind_data.receive_content.control_point.value, pValue, iValueSize);
        }
        else
        {
            cb_data.cb_content.notif_ind_data.receive_type = RSC_NOTIF_IND_RECEIVE_FAIL;
        }
    }
    else
    {
        cb_data.cb_content.notif_ind_data.receive_type = RSC_NOTIF_IND_RECEIVE_FAIL;
    }
    /* Inform application the notif/ind result. */
    if(pfnRscClientAppCB)
    {
        appResult = (*pfnRscClientAppCB)(RscClient_ClientID, &cb_data);
    }

    return (clientAPI_GetProfileResult(appResult));
}

/**
  * @brief  Called by profile client layer, when link disconnected.
  *         NOTE--we should reset some state when disconnected.
  * @retval ProfileResult_Success--procedure OK.
  *         other--procedure exception.
  */
static TProfileResult RscClientDisconnectCb( void )
{    
    TProfileResult result = ProfileResult_Success;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RscClientDisconnectCb.", 0);

    /* Reset some params, when disconnection occurs. */
    RscClient_EndHdlIdx = HDL_RSC_CACHE_LEN;
    RscClient_CccdDiscState = RSC_CCCD_DISC_START;
    
    /* There is no need to inform application, because GAP has done. */

    return (result);
}

/**
 * @brief RSC Client Callbacks.
*/
CONST TGATTClientCBs RscClientCBs =
{
    RscClientDiscoverStateCb,   //!< Discovery State callback function pointer
    RscClientDiscoverResultCb,  //!< Discovery result callback function pointer
    RscClientReadResultCb,      //!< Read response callback function pointer
    RscClientWriteResultCb,     //!< Write result callback function pointer
    RscClientNotifIndResultCb,  //!< Notify Indicate callback function pointer
    RscClientDisconnectCb       //!< Link disconnection callback function pointer
};

/**
  * @brief  add RSC client to application.
  * @param  appCB: pointer of app callback function to handle specific client module data.
  * @retval Client ID of the specific client module.
  */
TClientID RSC_AddClient( pfnSpecificClientAppCB_t appCB )
{
    TClientID rsc_client_id;
    if ( FALSE == clientAPI_RegisterSpecClientCB( &rsc_client_id, RscHdlCache, &RscClientCBs ) )
    {
        rsc_client_id = AppProcessGeneralClientMsgID;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "RSC_AddClient Fail !!!", 0);
        return rsc_client_id;
    }
    RscClient_ClientID = rsc_client_id;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSC_AddClient: client ID = %d", 1, RscClient_ClientID);

    /* register callback for profile to inform application that some events happened. */
    pfnRscClientAppCB = appCB;
    
    return rsc_client_id;
}


