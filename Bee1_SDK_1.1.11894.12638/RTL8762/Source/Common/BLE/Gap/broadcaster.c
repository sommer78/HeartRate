enum { __FILE_NUM__ = 0 };

#include "rtl_types.h"
#include "gap.h"
#include <string.h>
#include "broadcaster.h"
#include "blueapi_types.h"
#include "blueapi.h"
#include "trace.h"


#define DEFAULT_MIN_CONN_INTERVAL     0x0006  // 100 milliseconds
#define DEFAULT_MAX_CONN_INTERVAL     0x0C80  // 4 seconds
#define DEFAULT_MIN_ADV_INTERVAL      0x0020  // default minimun advertising interval, 20ms, 0.625ms/step
#define DEFAULT_MAX_ADV_INTERVAL      0x4000  // default maximun advertising interval, 10240ms, 0.625ms/step

#define MIN_CONN_INTERVAL             0x0006
#define MAX_CONN_INTERVAL             0x0C80

#define DEFAULT_TIMEOUT_MULTIPLIER    1000

#define CONN_INTERVAL_MULTIPLIER      6

#define MIN_SLAVE_LATENCY             0
#define MAX_SLAVE_LATENCY             500

#define MIN_TIMEOUT_MULTIPLIER        0x000a
#define MAX_TIMEOUT_MULTIPLIER        0x0c80


static uint8_t  gapStack_Initialized = 0;

static gaprole_States_t gapRole_state = GAP_BROADCASTER_STATE_INIT;

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
static uint16_t gapPara_AdvMinInterval = DEFAULT_MIN_ADV_INTERVAL;
static uint16_t gapPara_AdvMaxInterval = DEFAULT_MAX_ADV_INTERVAL;
static uint8_t  gapPara_AdvEnableDefault = FALSE;


// Application callbacks
//static gapDeviceStateChangeCb pGapDeviceStateChangeCb = NULL;

TGAP_STATUS broadcasterSetGapParameter( uint16_t param, uint8_t len, void *pValue )
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

    case GAPPRRA_ADV_ENABLE_DEFAULT:
        if ( len == sizeof ( uint8_t ) )
        {
            gapPara_AdvEnableDefault = *((uint8_t*)pValue);
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
TGAP_STATUS broadcasterGetGapParameter( uint16_t param, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    /* get common params in GAP. */
    if((param >= GAPPARA_PROFILE_ROLE) && (param <= GAPPARA_BLE_TX_POWER))
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
        *((uint8_t*)pValue) = gapPara_AdvEnableDefault;
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
TGAP_STATUS broadcaster_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks )
{
    if ( gapRole_state == GAP_BROADCASTER_STATE_INIT )
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


void broadcaster_GapParaInit(void)
{
    GAP_ParaInit(GAP_PROFILE_BROADCASTER);
    gapStack_Initialized = 0;
    gapRole_state = GAP_BROADCASTER_STATE_INIT;

    gapPara_AdvEventType = GAP_ADTYPE_ADV_NONCONN_IND;
    gapPara_AdvDirectAddrType = PEER_ADDRTYPE_PUBLIC;

    gapPara_AdvChanMap = GAP_ADVCHAN_ALL;
    gapPara_AdvFilterPolicy = GAP_FILTER_POLICY_ALL;
    gapPara_AdvMinInterval = DEFAULT_MIN_ADV_INTERVAL;
    gapPara_AdvMaxInterval = DEFAULT_MAX_ADV_INTERVAL;

    gapPara_AdvEnableDefault = FALSE;


}

/*----------------------------------------------------------------------------
 * handle blueAPI_Event_DeviceConfigSetRsp
 * --------------------------------------------------------------------------*/

static void broadcaster_Handle_DeviceConfigSetRsp(PBlueAPI_DeviceConfigSetRsp pDevCfgSetRsp )
{
	gapRole_state = GAP_BROADCASTER_STATE_STACK_READY;
    
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
                broadcaster_Init_StartAdvertising();
                gapStack_Initialized = TRUE;
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
static void broadcaster_Handle_LEAdvertiseRsp( PBlueAPI_LEAdvertiseRsp pAdvertiseRsp)
{
    gaprole_States_t old_gapRole_state = gapRole_state;
    if (pAdvertiseRsp->cause ==  blueAPI_CauseSuccess)
    {
        switch (pAdvertiseRsp->advMode)
        {
        case blueAPI_LEAdvModeDisabled:
            {
                if (gapRole_state == GAP_BROADCASTER_STATE_ADVERTISING)
                {
                    gapRole_state = GAP_BROADCASTER_STATE_IDLE;
                }

            }
            break;

        case blueAPI_LEAdvModeEnabled:
            {
                if (gapRole_state == GAP_BROADCASTER_STATE_IDLE)
                {
                    gapRole_state = GAP_BROADCASTER_STATE_ADVERTISING;
                }


                if (gapStack_Initialized == 0)
                {
                    gapStack_Initialized = 1;
                }

            }
            break;

        case blueAPI_LEAdvModeDirectedHighDuty:
            {

            }
            break;
        }
    }
    else
    {

    }

    if (gapRole_state != old_gapRole_state)
    {
        pGapDeviceStateChangeCb(gapRole_state);
    }

}

static void broadcaster_Handle_SetRandomAddressRsp( PBlueAPI_SetRandomAddressRsp pSetRandomAddressRsp)
{


}



/*----------------------------------------------------------------------------
 * handle blueAPI_Event_LEAdvertiseParameterRsp
 * --------------------------------------------------------------------------*/
static void broadcaster_Handle_LEAdvertiseParameterRsp( PBlueAPI_LEAdvertiseParameterSetRsp pAdvertiseParameterSetRsp)
{
    blueAPI_LEAdvertiseDataSetReq(
        blueAPI_LEDataTypeAdvertisingData,
        gapPara_AdvertDataLen,
        gapPara_AdvertData
    );
}

/*----------------------------------------------------------------------------
 * handle blueAPI_Event_LEAdvertiseDataSetRsp
 * --------------------------------------------------------------------------*/
static void broadcaster_Handle_LEAdvertiseDataSetRsp( PBlueAPI_LEAdvertiseDataSetRsp pAdvertiseDataSetRsp)
{
    if (pAdvertiseDataSetRsp->dataType == blueAPI_LEDataTypeAdvertisingData)
    {
        blueAPI_LEAdvertiseDataSetReq(blueAPI_LEDataTypeScanResponseData,
                                      gapPara_ScanRspDataLen,
                                      (uint8_t *)gapPara_ScanRspData
                                     );
    }
    else
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
}


/*----------------------------------------------------------------------------
 * handle blueAPI_Event_InternalEventInfo
 * --------------------------------------------------------------------------*/

static void broadcaster_Handle_InternalEventInfo( PBlueAPI_InternalEventInfo pInternalEventInfo )
{

}

static void broadcaster_Handle_LEModifyWhitelistRsp(PBlueAPI_LEModifyWhitelistRsp pModifyWhitelistRsp)
{

}


/*----------------------------------------------------------------------------
 * BlueAPI message handler.
 * Returns non-zero value if calling routine may release message buffer.
 * --------------------------------------------------------------------------*/
bool broadcaster_ProcessGapEvent( PBlueAPI_UsMessage pMsg )
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
        broadcaster_Handle_DeviceConfigSetRsp( &pMsg->p.DeviceConfigSetRsp);
        break;

    case blueAPI_EventSetRandomAddressRsp:
        broadcaster_Handle_SetRandomAddressRsp( &pMsg->p.SetRandomAddressRsp);
        break;

    case blueAPI_EventLEAdvertiseRsp:
        broadcaster_Handle_LEAdvertiseRsp( &pMsg->p.LEAdvertiseRsp);
        break;

    case blueAPI_EventLEAdvertiseParameterSetRsp:
        broadcaster_Handle_LEAdvertiseParameterRsp( &pMsg->p.LEAdvertiseParameterSetRsp);
        break;

    case blueAPI_EventLEAdvertiseDataSetRsp:
        broadcaster_Handle_LEAdvertiseDataSetRsp( &pMsg->p.LEAdvertiseDataSetRsp);
        break;

    case blueAPI_EventLEModifyWhitelistRsp:
        broadcaster_Handle_LEModifyWhitelistRsp(&pMsg->p.LEModifyWhitelistRsp);
        break;



    //for internal debug use only
    case blueAPI_EventInternalEventInfo:
        broadcaster_Handle_InternalEventInfo(&pMsg->p.InternalEventInfo );
        break;

    }
    return true;
}

bool broadcaster_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg )
{
    if((pMsg->Command == blueAPI_EventRegisterRsp)
        ||(pMsg->Command == blueAPI_EventActInfo)
        ||(pMsg->Command == blueAPI_EventSetBleTxPowerRsp)
#ifdef BT_GAP_VENDOR_CMD_ADD
        ||(pMsg->Command == blueAPI_EventIdle + 1)
#endif
      )
    {
        GAP_ProcessGapEvent(pMsg);
    }
    else
    {
        broadcaster_ProcessGapEvent(pMsg);
    }
    
    blueAPI_BufferRelease( pMsg );
    return true;
}

TGAP_STATUS broadcaster_Init_StartAdvertising(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);

    if (gapPara_AdvEnableDefault == TRUE)
    {
        blueAPI_LEAdvertiseParameterSetReq((TBlueAPI_LEAdvType)gapPara_AdvEventType,
                                        (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterScanPolicy,
                                        (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterConnectPolicy,
                                        gapPara_AdvMinInterval,
                                        gapPara_AdvMaxInterval,
                                        (TBlueAPI_LocalBDType)bdAddrType,
                                        NULL,
                                        (TBlueAPI_RemoteBDType)PEER_ADDRTYPE_PUBLIC
                                        );
    }

    gapRole_state = GAP_BROADCASTER_STATE_IDLE;
    pGapDeviceStateChangeCb(gapRole_state);

    return ret;
}


TGAP_STATUS broadcaster_StartAdvertising(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
    
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

    return ret;
}

TGAP_STATUS broadcaster_StopAdvertising(void)
{
    blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeDisabled);
    return gapAPI_CauseSuccess;
}

TGAP_STATUS broadcaster_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_SetBleTxPowerReq(tx_power_index))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "broadcaster_SetTxPower fail !!!", 0);
    }

    return ret;
}

