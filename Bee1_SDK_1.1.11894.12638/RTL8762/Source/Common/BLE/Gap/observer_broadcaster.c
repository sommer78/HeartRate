enum { __FILE_NUM__ = 0 };

#include "rtl_types.h"
#include "gap.h"
#include <string.h>
#include "observer_broadcaster.h"
#include "blueapi_types.h"
#include "blueapi.h"
#include "trace.h"
#include "mesh_api.h"

#define DEBUG_MESH_PACKT_ONLY

static uint8_t  gapStack_Initialized = 0;

static gaprole_States_t gapRole_state = GAP_OB_STATE_INIT;

static uint8_t  gapPara_AdvertDataLen = 3;
static uint8_t  gapPara_AdvertData[B_MAX_ADV_LEN] =
{
    0x02,   // length of this data
    GAP_ADTYPE_FLAGS,   // AD Type = Flags
    // Limited Discoverable & BR/EDR not supported
    (GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED),
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static uint8_t  gapPara_ScanRspDataLen = 0;
static uint8_t  gapPara_ScanRspData[B_MAX_ADV_LEN] = {0};

static uint8_t  gapPara_AdvEventType = GAP_ADTYPE_ADV_IND;
static uint8_t  gapPara_AdvDirectAddrType = PEER_ADDRTYPE_PUBLIC;
static uint8_t  gapRole_AdvDirectAddr[B_ADDR_LEN] = {0};
static uint8_t  gapPara_AdvChanMap = GAP_ADVCHAN_ALL;
static uint8_t  gapPara_AdvFilterPolicy = GAP_FILTER_POLICY_ALL;
static uint8_t  gapParaInternal_AdvFilterScanPolicy = blueAPI_LEFilterAny;
static uint8_t  gapParaInternal_AdvFilterConnectPolicy = blueAPI_LEFilterAny;
static uint16_t gapPara_AdvMinInterval = 0x80;
static uint16_t gapPara_AdvMaxInterval = 0x80;
static uint8_t  gapPara_ScanEnableDefault = FALSE;

static uint8_t  gapPara_ScanMode;
static uint16_t gapPara_ScanInterval;
static uint16_t gapPara_ScanWindow;
static uint8_t  gapPara_FilterPolicy;
static uint8_t  gapPara_FilterDuplicates;

static uint8_t  scanFlag = 0;
static uint8_t  advFlag = 0;

/**
 * @brief           Set a GAP Role parameter.
 *
 *                      NOTE: You can call this function with a GAP Peripheral Parameter ID and it will set the
 *                      GAP Parameter.  GAP Peripheral Parameters are defined in (peripheral.h).  Also,
 *                      the "len" field must be set to the size of a "uint16_t" and the
 *                      "pValue" field must point to a "uint16".
 *
 * @param[in]         param - Profile parameter ID: @ref GAP_PERIPHERAL_PARAMETERS
 * @param[in]         len - length of data to write
 * @param[in]         pValue - pointer to data to write.  This is dependent on
 *                      the parameter ID and WILL be cast to the appropriate
 *                      data type (example: data type of uint16 will be cast to
 *                      uint16 pointer).
 *
 * @return          gapAPI_CauseSuccess or INVALIDPARAMETER (invalid paramID)
 */

TGAP_STATUS obSetGapParameter( uint16_t param, uint8_t len, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
	/* set common params in GAP. */
    if((param >= GAPPARA_PROFILE_ROLE) && (param <= GAPPRRA_APPEARANCE))
    {
        ret = GAP_SetParameter(param,len,pValue);
        return ( ret );
    }
	
    switch ( param )
    {
	case GAPPRRA_ROLE_STATE:
        if ( len == sizeof(uint8_t) )
        {
            gapRole_state = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;


    case GAPPRRA_ADVERT_DATA:
        if ( len <= B_MAX_ADV_LEN )
        {
            memset( gapPara_AdvertData, 0, B_MAX_ADV_LEN );
            memcpy( gapPara_AdvertData, pValue, len );
            gapPara_AdvertDataLen = len;
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_SCAN_RSP_DATA:
        if ( len <= B_MAX_ADV_LEN )
        {
            memset( gapPara_ScanRspData, 0, B_MAX_ADV_LEN );
            memcpy( gapPara_ScanRspData, pValue, len );
            gapPara_ScanRspDataLen = len;
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_EVENT_TYPE:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAP_ADTYPE_ADV_LDC_DIRECT_IND) )
        {
            gapPara_AdvEventType = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_DIRECT_ADDR_TYPE:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= PEER_ADDRTYPE_PRIVATE_RESOLVE) )
        {
            gapPara_AdvDirectAddrType = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_DIRECT_ADDR:
        if ( len == B_ADDR_LEN )
        {
            memcpy( gapRole_AdvDirectAddr, pValue, B_ADDR_LEN ) ;
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_CHANNEL_MAP:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= 0x07) )
        {
            gapPara_AdvChanMap = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_FILTER_POLICY:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAP_FILTER_POLICY_WHITE) )
        {
            gapPara_AdvFilterPolicy = *((uint8_t*)pValue);

            if (gapPara_AdvFilterPolicy != GAP_FILTER_POLICY_ALL)
            {
                if(gapPara_AdvFilterPolicy == GAP_FILTER_POLICY_WHITE_SCAN)
                {
                    gapParaInternal_AdvFilterScanPolicy = blueAPI_LEFilterWhitelist;
                }
                else
                if(gapPara_AdvFilterPolicy == GAP_FILTER_POLICY_WHITE_CON)
                {
                    gapParaInternal_AdvFilterConnectPolicy = blueAPI_LEFilterWhitelist;      
                }
                else
                {
                    gapParaInternal_AdvFilterScanPolicy = blueAPI_LEFilterWhitelist;
                    gapParaInternal_AdvFilterConnectPolicy = blueAPI_LEFilterWhitelist;               
                }
            }
            else
            {
                gapParaInternal_AdvFilterScanPolicy = blueAPI_LEFilterAny;
                gapParaInternal_AdvFilterConnectPolicy = blueAPI_LEFilterAny;               
            }
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_INTERVAL_MIN:
        if ( len == sizeof ( uint16_t ) )
        {
            gapPara_AdvMinInterval = *((uint16_t*)pValue);

        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_INTERVAL_MAX:
        if ( len == sizeof ( uint16_t ) )
        {
            gapPara_AdvMaxInterval = *((uint16_t*)pValue);

        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_SCAN_ENABLE_DEFAULT:
        if ( len == sizeof ( uint8_t ) )
        {
            gapPara_ScanEnableDefault = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPARA_SCANMODE:
        if ( len == sizeof ( uint8_t ) )
        {
            gapPara_ScanMode = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPARA_SCANINTERVAL:
        if ( len == sizeof ( uint16_t ) )
        {
            gapPara_ScanInterval = *((uint16_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPARA_SCANWINDOW:
        if ( len == sizeof ( uint16_t ) )
        {
            gapPara_ScanWindow = *((uint16_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPARA_FILTERPOLICY:
        if ( len == sizeof ( uint8_t ) )
        {
            gapPara_FilterPolicy = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;


    case GAPPARA_FILTERDUPLICATES:
        if ( len == sizeof ( uint8_t ) )
        {
            gapPara_FilterDuplicates = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;        

    default:
        break;
    }

    return ( ret );
}

/*********************************************************************
 * @brief   Get a GAP Role parameter.
 *
 * Public function defined in peripheral.h.
 */
TGAP_STATUS obGetGapParameter( uint16_t param, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    /* get common params in GAP. */
    if((param >= GAPPARA_PROFILE_ROLE) && (param <= GAPPRRA_APPEARANCE))
    {
        ret = GAP_GetParameter(param, pValue);
        return ( ret );
    }
    
    switch ( param )
    {
    case GAPPRRA_ROLE_STATE:
        *((uint8_t*)pValue) = gapRole_state;
        break;
        
    case GAPPRRA_ADVERT_DATA:
        memcpy( pValue , gapPara_AdvertData, gapPara_AdvertDataLen );
        break;

    case GAPPRRA_SCAN_RSP_DATA:
        memcpy( pValue, gapPara_ScanRspData, gapPara_ScanRspDataLen ) ;
        break;

    case GAPPRRA_ADV_EVENT_TYPE:
        *((uint8_t*)pValue) = gapPara_AdvEventType;
        break;

    case GAPPRRA_ADV_DIRECT_ADDR_TYPE:
        *((uint8_t*)pValue) = gapPara_AdvDirectAddrType;
        break;

    case GAPPRRA_ADV_DIRECT_ADDR:
        memcpy( pValue, gapRole_AdvDirectAddr, B_ADDR_LEN ) ;
        break;

    case GAPPRRA_ADV_CHANNEL_MAP:
        *((uint8_t*)pValue) = gapPara_AdvChanMap;
        break;

    case GAPPRRA_ADV_FILTER_POLICY:
        *((uint8_t*)pValue) = gapPara_AdvFilterPolicy;
        break;

    case GAPPRRA_ADV_ENABLE_DEFAULT:
        *((uint8_t*)pValue) = gapPara_ScanEnableDefault;
        break;

    case GAPPARA_SCANMODE:
        *((uint8_t*)pValue) = gapPara_ScanMode;
        break;

    case GAPPARA_SCANINTERVAL:
        *((uint16_t*)pValue) = gapPara_ScanInterval;
        break;

    case GAPPARA_SCANWINDOW:
        *((uint16_t*)pValue) = gapPara_ScanWindow;
        break;

    case GAPPARA_FILTERPOLICY:
        *((uint8_t*)pValue) = gapPara_FilterPolicy;
        break;

    case GAPPARA_FILTERDUPLICATES:
        *((uint8_t*)pValue) = gapPara_FilterDuplicates;
        break;

    default:

        break;
    }

    return ( ret );
}

/*********************************************************************
 * @brief   Does the device initialization.
 *
 * Public function defined in peripheral.h.
 */
TGAP_STATUS ob_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks )
{
    if ( gapRole_state == GAP_OB_STATE_INIT )
    {
        // Clear all of the Application callbacks
        if ( pAppCallbacks )
        {
            pGapDeviceStateChangeCb = pAppCallbacks;
        }

        return ( gapAPI_CauseSuccess );
    }
    else
    {
        return ( gapAPI_AlreadyInRequestedMode );
    }
}


void ob_GapParaInit(void)
{
    GAP_ParaInit(GAP_PROFILE_BROADCASTER|GAP_PROFILE_OBSERVER);
    gapStack_Initialized = 0;
    gapRole_state = GAP_OB_STATE_INIT;

    gapPara_AdvEventType = GAP_ADTYPE_ADV_NONCONN_IND;//GAP_ADTYPE_ADV_IND,GAP_ADTYPE_ADV_NONCONN_IND
    gapPara_AdvDirectAddrType = PEER_ADDRTYPE_PUBLIC;

    gapPara_AdvChanMap = GAP_ADVCHAN_ALL;
    gapPara_AdvFilterPolicy = GAP_FILTER_POLICY_ALL;
    gapPara_AdvMinInterval = 0x80;
    gapPara_AdvMaxInterval = 0x80;

    gapPara_ScanEnableDefault = TRUE;

    gapPara_ScanMode = GAP_SCAN_MODE_PASSIVE;
    gapPara_ScanInterval = 0x10;
    gapPara_ScanWindow = 0x10;
    gapPara_FilterPolicy = GAP_SCAN_FILTER_ANY;
    gapPara_FilterDuplicates = GAP_SCAN_FILTER_DUPLICATE_DISABLE; //GAP_SCAN_FILTER_DUPLICATE_ENABLE

}

/*----------------------------------------------------------------------------
 * handle blueAPI_Event_DeviceConfigSetRsp
 * --------------------------------------------------------------------------*/

static void ob_Handle_DeviceConfigSetRsp(PBlueAPI_DeviceConfigSetRsp pDevCfgSetRsp )
{
	gapRole_state = GAPSTATE_STACK_READY;
    
    switch (pDevCfgSetRsp->opCode)
    {
    case    blueAPI_DeviceConfigDeviceName:
        {
            uint16_t appearance;
            GAP_GetParameter(GAPPRRA_APPEARANCE, &appearance);
            //config Appearance
            if (!gapStack_Initialized)
            {
                blueAPI_DeviceConfigAppearanceSetReq(appearance);
            }
        }
        break;
    case    blueAPI_DeviceConfigAppearance:
        {
            if (!gapStack_Initialized)
            {
                if (gapPara_ScanEnableDefault == TRUE)
                {
                    ob_StartScan();
                }
                gapStack_Initialized = 1;
            }
        }
        break;
    case    blueAPI_DeviceConfigPerPrefConnParam:
        break;
    case    blueAPI_DeviceConfigSecurity:
        break;
    case    blueAPI_DeviceConfigStore:
        break;

    default:
        break;
    }

}


/*----------------------------------------------------------------------------
 * handle blueAPI_EventLEAdvertiseRsp
 * --------------------------------------------------------------------------*/
static void ob_Handle_LEAdvertiseRsp( PBlueAPI_LEAdvertiseRsp pAdvertiseRsp)
{
    if (pAdvertiseRsp->cause == blueAPI_CauseSuccess)
    {
        switch (pAdvertiseRsp->advMode)
        {
            case blueAPI_LEAdvModeDisabled:
                ob_AdvStateSet(gapRole_state, OB_STATE_IDLE);
                break;

            case blueAPI_LEAdvModeEnabled:
                ob_AdvStateSet(gapRole_state, OB_STATE_ACTIVE);
                break;

            case blueAPI_LEAdvModeDirectedHighDuty:
                break;
        }
    }
    else
    {
        switch (pAdvertiseRsp->advMode)
        {
            case blueAPI_LEAdvModeDisabled:
                ob_AdvStateSet(gapRole_state, OB_STATE_ACTIVE);
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "!!! ob_Handle_LEAdvertiseRsp: Disable Advertise failed, cause = %d !!!", 1, pAdvertiseRsp->cause);
                break;

            case blueAPI_LEAdvModeEnabled:
                ob_AdvStateSet(gapRole_state, OB_STATE_IDLE);
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "!!! ob_Handle_LEAdvertiseRsp: Enable Advertise failed, cause = %d !!!", 1, pAdvertiseRsp->cause);
                break;

            case blueAPI_LEAdvModeDirectedHighDuty:
                break;
        }
    }
    pGapDeviceStateChangeCb(gapRole_state);
}

static void ob_Handle_SetRandomAddressRsp( PBlueAPI_SetRandomAddressRsp pSetRandomAddressRsp)
{


}



/*----------------------------------------------------------------------------
 * handle blueAPI_Event_LEAdvertiseParameterRsp
 * --------------------------------------------------------------------------*/
static void ob_Handle_LEAdvertiseParameterRsp( PBlueAPI_LEAdvertiseParameterSetRsp pAdvertiseParameterSetRsp)
{
    /*
    blueAPI_LEAdvertiseDataSetReq(blueAPI_LEDataTypeScanResponseData,
                                  gapPara_ScanRspDataLen,
                                  gapPara_ScanRspData); 
    */
    blueAPI_LEAdvertiseDataSetReq(blueAPI_LEDataTypeAdvertisingData,
                                  gapPara_AdvertDataLen,
                                  gapPara_AdvertData);
}

/*----------------------------------------------------------------------------
 * handle blueAPI_Event_LEAdvertiseDataSetRsp
 * --------------------------------------------------------------------------*/
static void ob_Handle_LEAdvertiseDataSetRsp( PBlueAPI_LEAdvertiseDataSetRsp pAdvertiseDataSetRsp)
{
    if (pAdvertiseDataSetRsp->dataType == blueAPI_LEDataTypeAdvertisingData)
    {
        if (gapPara_AdvEventType == GAP_ADTYPE_ADV_HDC_DIRECT_IND)
        {
            blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeDirectedHighDuty);
        }
        else
        {
            blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeEnabled);
        }
    }
    else
    {
        blueAPI_LEAdvertiseDataSetReq(blueAPI_LEDataTypeAdvertisingData,
                                      gapPara_AdvertDataLen,
                                      gapPara_AdvertData);
    }
}


/*----------------------------------------------------------------------------
 * handle blueAPI_Event_InternalEventInfo
 * --------------------------------------------------------------------------*/

static void ob_Handle_InternalEventInfo( PBlueAPI_InternalEventInfo pInternalEventInfo )
{

}

static void ob_Handle_LEModifyWhitelistRsp(PBlueAPI_LEModifyWhitelistRsp pModifyWhitelistRsp)
{

}


static void ob_Handle_LEScanRsp(PBlueAPI_LEScanRsp pLeScanResp)
{
    if (pLeScanResp->cause == blueAPI_CauseSuccess)
    {
        if (scanFlag == 1)
        {
            ob_ScanStateSet(gapRole_state, OB_STATE_ACTIVE);
        }
        else
        {
            ob_ScanStateSet(gapRole_state, OB_STATE_IDLE);
        }
    }
    else
    {
        if (scanFlag == 1)
        {
            ob_ScanStateSet(gapRole_state, OB_STATE_IDLE);
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "!!! ob_Handle_LEScanRsp: Enable Scan failed, cause = %d !!!", 1, pLeScanResp->cause);
        }
        else
        {
            ob_ScanStateSet(gapRole_state, OB_STATE_ACTIVE);
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "!!! ob_Handle_LEScanRsp: Disable Scan failed, cause = %d !!!", 1, pLeScanResp->cause);
        }
    }
    pGapDeviceStateChangeCb(gapRole_state);
}

static void ob_Handle_LEScanInfo(PBlueAPI_LEScanInfo pLeScanInfo)
{
#ifndef DEBUG_MESH_PACKT_ONLY
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
                                            "ob_Handle_LEScanInfo: remote_BD = 0x%02x%02x%02x%02x%02x%02x,remote_BD_type = %d, advType = %d, rssi = %d, length = %d", 
                                            10, 
                                            pLeScanInfo->remote_BD[5],
                                            pLeScanInfo->remote_BD[4],
                                            pLeScanInfo->remote_BD[3],
                                            pLeScanInfo->remote_BD[2],
                                            pLeScanInfo->remote_BD[1],
                                            pLeScanInfo->remote_BD[0],
                                            pLeScanInfo->remote_BD_type,
                                            pLeScanInfo->advType,
                                            pLeScanInfo->rssi,
                                            pLeScanInfo->dataLength);
#endif

    MeshBearerHandleLEScanInfo(pLeScanInfo);

/*
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "ob_Handle_LEScanInfo: length = %d ", 1, pLeScanInfo->dataLength);
    for(uint8_t i = 0; i < pLeScanInfo->dataLength; i++)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "scan_data[%d] = %d ", 2, i, pLeScanInfo->data[i]);
    } 
*/
}



/*----------------------------------------------------------------------------
 * BlueAPI message handler.
 * Returns non-zero value if calling routine may release message buffer.
 * --------------------------------------------------------------------------*/
bool ob_ProcessGapEvent( PBlueAPI_UsMessage pMsg )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAPRole_ProcessEvent: Command(0x%02x)", 1, pMsg->Command);
#endif
    switch ( pMsg->Command )
    {
    default:
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAPRole_Not_ProcessEvent: Command(0x%02x)", 1, pMsg->Command);
#endif
        break;

    case blueAPI_EventDeviceConfigSetRsp:
        ob_Handle_DeviceConfigSetRsp( &pMsg->p.DeviceConfigSetRsp);
        break;

    case blueAPI_EventSetRandomAddressRsp:
        ob_Handle_SetRandomAddressRsp( &pMsg->p.SetRandomAddressRsp);
        break;

    case blueAPI_EventLEAdvertiseRsp:
        ob_Handle_LEAdvertiseRsp( &pMsg->p.LEAdvertiseRsp);
        break;

    case blueAPI_EventLEAdvertiseParameterSetRsp:
        ob_Handle_LEAdvertiseParameterRsp( &pMsg->p.LEAdvertiseParameterSetRsp);
        break;

    case blueAPI_EventLEAdvertiseDataSetRsp:
        ob_Handle_LEAdvertiseDataSetRsp( &pMsg->p.LEAdvertiseDataSetRsp);
        break;

    case blueAPI_EventLEModifyWhitelistRsp:
        ob_Handle_LEModifyWhitelistRsp(&pMsg->p.LEModifyWhitelistRsp);
        break;

    //for le scan
    case blueAPI_EventLEScanRsp:
        ob_Handle_LEScanRsp(&pMsg->p.LEScanRsp);
        break;

    case blueAPI_EventLEScanInfo:
        ob_Handle_LEScanInfo(&pMsg->p.LEScanInfo);
        break;


    //for internal debug use only
    case blueAPI_EventInternalEventInfo:
        ob_Handle_InternalEventInfo(&pMsg->p.InternalEventInfo );
        break;

    }
    return true;
}

bool ob_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg )
{
    if((pMsg->Command == blueAPI_EventRegisterRsp)
        ||(pMsg->Command == blueAPI_EventActInfo))
    {
        GAP_ProcessGapEvent(pMsg);
    }
    else
    {
        ob_ProcessGapEvent(pMsg);
    }
    
    blueAPI_BufferRelease( pMsg );
    return true;
}

TGAP_STATUS ob_StartAdvertising(void)
{
    uint8_t bdAddrType;
    uint8_t adv_state = ob_AdvStateGet(gapRole_state);

    if (adv_state == OB_STATE_IDLE)
    {
        if (advFlag == 0)
        {
            GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);

            blueAPI_LEAdvertiseParameterSetReq((TBlueAPI_LEAdvType)gapPara_AdvEventType,
                                               (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterScanPolicy,
                                               (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterConnectPolicy,
                                               gapPara_AdvMinInterval,
                                               gapPara_AdvMaxInterval,
                                               (TBlueAPI_LocalBDType)bdAddrType,
                                               NULL,
                                               (TBlueAPI_RemoteBDType)PEER_ADDRTYPE_PUBLIC
                                              );
            advFlag = 1;
        }
        else
        {
            blueAPI_LEAdvertiseDataSetReq(blueAPI_LEDataTypeAdvertisingData,
                                          gapPara_AdvertDataLen,
                                          gapPara_AdvertData);
        }
        
        ob_AdvStateSet(gapRole_state, OB_STATE_START);
        return gapAPI_CauseSuccess;
    }
    else if (adv_state == OB_STATE_START)
    {
        return gapAPI_AlreadyInRequestedMode;
    }
    else
    {
        return gapAPI_IncorrectMode;
    }
}

TGAP_STATUS ob_StopAdvertising(void)
{
    uint8_t adv_state = ob_AdvStateGet(gapRole_state);
    
    if (adv_state == OB_STATE_ACTIVE)
    {
        blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeDisabled);
        ob_AdvStateSet(gapRole_state, OB_STATE_STOP);
        return gapAPI_CauseSuccess;
    }
    else if (adv_state == OB_STATE_STOP)
    {
        return gapAPI_AlreadyInRequestedMode;
    }
    else
    {
        return gapAPI_IncorrectMode;
    }
}


TGAP_STATUS ob_StartScan(void)
{
    uint8_t bdAddrType;
    uint8_t scan_state = ob_ScanStateGet(gapRole_state);
    
    if (scan_state == OB_STATE_IDLE)
    {
        GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
        blueAPI_LEScanReq((TBlueAPI_LEScanMode)gapPara_ScanMode,
                          gapPara_ScanInterval,
                          gapPara_ScanWindow,
                          (TBlueAPI_LEFilterPolicy)gapPara_FilterPolicy,
                          (TBlueAPI_LocalBDType)bdAddrType,
                          gapPara_FilterDuplicates);
        ob_ScanStateSet(gapRole_state, OB_STATE_START);
        scanFlag = 1;
        return gapAPI_CauseSuccess;
    }
    else if (scan_state == OB_STATE_START)
    {
        return gapAPI_AlreadyInRequestedMode;
    }
    else
    {
        return gapAPI_IncorrectMode;
    }
}

TGAP_STATUS ob_StopScan(void)
{
    uint8_t bdAddrType;    
    uint8_t scan_state = ob_ScanStateGet(gapRole_state);
        
    if (scan_state == OB_STATE_ACTIVE)
    {
        GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
        blueAPI_LEScanReq(blueAPI_LEScanDisabled,
                          gapPara_ScanInterval,
                          gapPara_ScanWindow,
                          (TBlueAPI_LEFilterPolicy)gapPara_FilterPolicy,
                          (TBlueAPI_LocalBDType)bdAddrType,
                          gapPara_FilterDuplicates);
        ob_ScanStateSet(gapRole_state, OB_STATE_STOP);
        scanFlag = 0;
        return gapAPI_CauseSuccess;
    }
    else if (scan_state == OB_STATE_STOP)
    {
        return gapAPI_AlreadyInRequestedMode;
    }
    else
    {
        return gapAPI_IncorrectMode;
    }
}


