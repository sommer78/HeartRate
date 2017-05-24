enum { __FILE_NUM__ = 0 };

#include "rtl_types.h"
#include "gap.h"
#include "gapbondmgr.h"
#include <string.h>
#include "observer.h"
#include "blueapi_types.h"
#include "blueapi.h"
#include "trace.h"


#define DEFAULT_MIN_CONN_INTERVAL     0x0006  // 100 milliseconds
#define DEFAULT_MAX_CONN_INTERVAL     0x0C80  // 4 seconds
#define MIN_CONN_INTERVAL             0x0006
#define MAX_CONN_INTERVAL             0x0C80
#define DEFAULT_TIMEOUT_MULTIPLIER    1000
#define CONN_INTERVAL_MULTIPLIER      6
#define MIN_SLAVE_LATENCY             0
#define MAX_SLAVE_LATENCY             500
#define MIN_TIMEOUT_MULTIPLIER        0x000a
#define MAX_TIMEOUT_MULTIPLIER        0x0c80

static uint8_t  gapStack_Initialized = 0;
static gaprole_States_t gapRole_state = GAP_OBSERVER_STATE_INIT;
static uint8_t gapPara_ScanMode;
static uint16_t gapPara_ScanInterval;
static uint16_t gapPara_ScanWindow;
static uint8_t gapPara_FilterPolicy;
static uint8_t gapPara_FilterDuplicates;

pfnObserverAppCB_t pGapObserverAppDirectCb = NULL;

TGAP_STATUS observerSetGapParameter( uint16_t param, uint8_t len, void *pValue )
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
 * Public function defined in observer.h.
 */
TGAP_STATUS observerGetGapParameter( uint16_t param, void *pValue )
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
 * Public function defined in observer.h.
 */
TGAP_STATUS observer_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks )
{
    if ( gapRole_state == GAP_OBSERVER_STATE_INIT )
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

/* Used by application, register app callback. */
void observer_RegisterAppDirectCB( pfnObserverAppCB_t pAppCallback )
{
    pGapObserverAppDirectCb = pAppCallback;
}

void observer_GapParaInit(void)
{
    GAP_ParaInit(GAP_PROFILE_OBSERVER);
    gapStack_Initialized = 0;
    gapRole_state = GAP_OBSERVER_STATE_INIT;

    gapPara_ScanMode = GAP_SCAN_MODE_ACTIVE;
    gapPara_ScanInterval = 0x10;
    gapPara_ScanWindow = 0x10;
    gapPara_FilterPolicy = GAP_SCAN_FILTER_ANY;
    gapPara_FilterDuplicates = GAP_SCAN_FILTER_DUPLICATE_DISABLE;
}

/*----------------------------------------------------------------------------
 * handle blueAPI_Event_DeviceConfigSetRsp
 * --------------------------------------------------------------------------*/

static void observer_Handle_DeviceConfigSetRsp(PBlueAPI_DeviceConfigSetRsp pDevCfgSetRsp )
{
    gapRole_state = GAP_OBSERVER_STATE_STACK_READY;

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
                gapRole_state = GAP_OBSERVER_STATE_IDLE_NO_SCAN_NO_CONN;
                pGapDeviceStateChangeCb(gapRole_state);
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

static void observer_Handle_SetRandomAddressRsp( PBlueAPI_SetRandomAddressRsp pSetRandomAddressRsp)
{

}


/*----------------------------------------------------------------------------
 * handle blueAPI_Event_InternalEventInfo
 * --------------------------------------------------------------------------*/

static void observer_Handle_InternalEventInfo( PBlueAPI_InternalEventInfo pInternalEventInfo )
{

}

static void observer_Handle_LEModifyWhitelistRsp(PBlueAPI_LEModifyWhitelistRsp pModifyWhitelistRsp)
{

}

static void observer_Handle_LEScanRsp(PBlueAPI_LEScanRsp pLeScanResp)
{
    if (pLeScanResp->cause == blueAPI_CauseSuccess)
    {
        pGapDeviceStateChangeCb(gapRole_state);
    }
}

static void observer_Handle_LEScanInfo(PBlueAPI_LEScanInfo pLeScanInfo)
{
    TObserverAppCB_MsgType msgType;
    TObserverAppCB_MsgData msgData;

    /* Send Adv data to application to process, use callback. */
    if(pGapObserverAppDirectCb)
    {
        msgType = ADV_SCAN_RSP_DATA_MSGTYPE;
        msgData.pAdvScanRspData = pLeScanInfo;
        (*pGapObserverAppDirectCb)(msgType, msgData);
    }
}

/*----------------------------------------------------------------------------
 * BlueAPI message handler.
 * Returns non-zero value if calling routine may release message buffer.
 * --------------------------------------------------------------------------*/
bool observer_ProcessGapEvent( PBlueAPI_UsMessage pMsg )
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
        observer_Handle_DeviceConfigSetRsp( &pMsg->p.DeviceConfigSetRsp);
        break;

    case blueAPI_EventSetRandomAddressRsp:
        observer_Handle_SetRandomAddressRsp( &pMsg->p.SetRandomAddressRsp);
        break;

    case blueAPI_EventLEModifyWhitelistRsp:
        observer_Handle_LEModifyWhitelistRsp(&pMsg->p.LEModifyWhitelistRsp);
        break;

    //for le scan
    case blueAPI_EventLEScanRsp:
        observer_Handle_LEScanRsp(&pMsg->p.LEScanRsp);
        break;

    case blueAPI_EventLEScanInfo:
        observer_Handle_LEScanInfo(&pMsg->p.LEScanInfo);
        break;


    //for internal debug use only
    case blueAPI_EventInternalEventInfo:
        observer_Handle_InternalEventInfo(&pMsg->p.InternalEventInfo );
        break;

    }
    return true;
}

bool observer_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg )
{
    bool bufferReleased = false;
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
        observer_ProcessGapEvent(pMsg);
    }

    if ( !bufferReleased && pMsg)
    {
        blueAPI_BufferRelease(pMsg);
        pMsg = NULL;
    }

    return true;
}

TGAP_STATUS observer_StartScan(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if (gapRole_state == GAP_OBSERVER_STATE_IDLE_NO_SCAN_NO_CONN)
    {
        blueAPI_LEScanReq((TBlueAPI_LEScanMode)gapPara_ScanMode,
                          gapPara_ScanInterval,
                          gapPara_ScanWindow,
                          (TBlueAPI_LEFilterPolicy)gapPara_FilterPolicy,
                          (TBlueAPI_LocalBDType)bdAddrType,
                          gapPara_FilterDuplicates);

        gapRole_state = GAP_OBSERVER_STATE_SCANNING;
    }
    return ret;
}

TGAP_STATUS observer_StopScan(void)
{
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if (gapRole_state == GAP_OBSERVER_STATE_SCANNING)
    {

        blueAPI_LEScanReq(blueAPI_LEScanDisabled,
                          gapPara_ScanInterval,
                          gapPara_ScanWindow,
                          (TBlueAPI_LEFilterPolicy)gapPara_FilterPolicy,
                          (TBlueAPI_LocalBDType)bdAddrType,
                          gapPara_FilterDuplicates);
        gapRole_state = GAP_OBSERVER_STATE_IDLE_NO_SCAN_NO_CONN;
    }

    return gapAPI_CauseSuccess;
}

TGAP_STATUS observer_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_SetBleTxPowerReq(tx_power_index))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "observer_SetTxPower fail !!!", 0);
    }

    return ret;
}

/*********************************************************************
*********************************************************************/

