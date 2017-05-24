enum { __FILE_NUM__ = 0 };

/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    central.c
  * @brief   This file provides the GAP Central Role related functions.
  * @details
  * @author  ranhui
  * @date    2015-5-30
  * @version v0.1
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include "FreeRTOS.h"
#include "task.h"
#include "rtl_types.h"
#include "gap.h"
#include "gapbondmgr.h"
#include <string.h>
#include "central.h"
#include "blueapi_types.h"
#include "blueapi.h"
#include "profile.h"
#include "trace.h"

extern bool client_HandleMessage( PBlueAPI_UsMessage pMsg );

/** @cond private**/
/** @addtogroup Central_Role
  * @{
  */

/** @defgroup Central_Role_Private_Constants Central Role Private Constants
  * @{
  */
/** Add all private defines here **/
#define DEFAULT_MIN_CONN_INTERVAL     0x0006  // default minimun connection interval, 100 milliseconds
#define DEFAULT_MAX_CONN_INTERVAL     0x0C80  // default maximun connection interval, 4 seconds
#define MIN_CONN_INTERVAL             0x0006  // minimum connection interval
#define MAX_CONN_INTERVAL             0x0C80  // maximum connection interval
#define DEFAULT_TIMEOUT_MULTIPLIER    1000    // default timeout multiplier, supervision timeout
#define CONN_INTERVAL_MULTIPLIER      6       // connection interval multiplier
#define MIN_SLAVE_LATENCY             0       // the minimum slave latency
#define MAX_SLAVE_LATENCY             500     // the maximum slave latency
#define MIN_TIMEOUT_MULTIPLIER        0x000a  // minimum timeout multiplier
#define MAX_TIMEOUT_MULTIPLIER        0x0c80  // maximum timeout multiplier
/** End of Central_Role_Private_Constants
  * @}
  */

/** @defgroup Central_Role_Private_Variables Central Role Private Variables
  * @{
  */
/** Add all private variables here **/
extern int8_t   gapPara_btRSSI;     /* Handled in gap_common, but only used in Peripheral and Central. */
extern uint8_t gapBond_FixedPasskeyEnable;
static uint8_t gapStack_Initialized = 0;
static gaprole_States_t gapRole_state = GAP_CENTRAL_STATE_INIT;
static uint8_t gapPara_ScanMode;
static uint16_t gapPara_ScanInterval;
static uint16_t gapPara_ScanWindow;
static uint8_t gapPara_FilterPolicy;
static uint8_t gapPara_FilterDuplicates;
//static uint16_t gapPara_ConnHandle = 0;
static uint16_t gapPara_dsPoolID;
static uint16_t gapPara_dsDataOffset;
static uint16_t gapPara_maxTPDUSize;
static uint8_t gapPara_maxTPDUdsCredits;
static uint16_t gapPara_mtuSize = 23;
static uint8_t gapPara_DestDevAddr[B_ADDR_LEN] = {0};
static uint8_t gapPara_DestDevAddrType = blueAPI_RemoteBDTypeLEPublic;
static uint16_t gapPara_Conn_ScanTimeout = 0;
static uint16_t gapPara_MinConnInterval = DEFAULT_MIN_CONN_INTERVAL;
static uint16_t gapPara_MaxConnInterval = DEFAULT_MAX_CONN_INTERVAL;
static uint16_t gappara_SlaveLatency = MIN_SLAVE_LATENCY;
static uint16_t gapPara_TimeoutMultiplier = DEFAULT_TIMEOUT_MULTIPLIER;
static uint16_t gapPara_CE_Length = 0;
static uint8_t gapPara_ConnectedDevAddr[B_ADDR_LEN] = {0};
static uint8_t gapPara_ConnectedDevAddrType = 0;
static uint16_t gapPara_ConnInterval = 0;
static uint16_t gapPara_ConnSlaveLatency = 0;
static uint16_t gapPara_ConnTimeout = 0;
static pfnCentralAppCB_t pGapCentralAppDirectCb = NULL;
/** End of Central_Role_Private_Variables
  * @}
  */

/** @endcond **/
  
/**
 * @brief   Set a central role parameter.
 *
 * @param[in]   param - profile parameter ID: @ref GAP_PARAM_CENTRAL_OBSERV ; @ref GAP_PARAM_PERIPH_CENTRAL ;
*              @ref GAP_PARAM_CENTRAL_ONLY ; @ref GAP_PARAM_ALL_ROLES
 * @param[in]   len - length of the parameter to set
 * @param[in]   pValue - pointer to buffer to contain the set data
 *
 * @retval  gapAPI_CauseSuccess Operation successful.
 * @retval  gapAPI_InvalidRange Invalid length
 */
TGAP_STATUS centralSetGapParameter( uint16_t param, uint8_t len, void *pValue )
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

    case GAPPRRA_CONN_DEST_ADDR:
        {
            memcpy(gapPara_DestDevAddr, pValue, B_ADDR_LEN ) ;
        }
        break;

    case GAPPRRA_CONN_DEST_ADDR_TYPE:
        {
            if ( len == sizeof ( uint8_t ) )
            {
                gapPara_DestDevAddrType = *((uint8_t*)pValue);
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_CONN_SCAN_TIMEOUT:
        {
            uint16_t connScanTimeOut = *((uint16_t*)pValue);
            if ( len == sizeof ( uint16_t ) )
            {
                gapPara_Conn_ScanTimeout = connScanTimeOut;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_MIN_CONN_INTERVAL:
        {
            uint16_t newInterval = *((uint16_t*)pValue);
            if (   len == sizeof ( uint16_t )           &&
                    ( newInterval >= MIN_CONN_INTERVAL ) &&
                    ( newInterval <= MAX_CONN_INTERVAL ) )
            {
                gapPara_MinConnInterval = newInterval;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_MAX_CONN_INTERVAL:
        {
            uint16_t newInterval = *((uint16_t*)pValue);
            if (   len == sizeof ( uint16_t )          &&
                    ( newInterval >= MIN_CONN_INTERVAL) &&
                    ( newInterval <= MAX_CONN_INTERVAL) )
            {
                gapPara_MaxConnInterval = newInterval;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_SLAVE_LATENCY:
        {
            uint16_t latency = *((uint16_t*)pValue);
            if ( len == sizeof ( uint16_t ) && (latency < MAX_SLAVE_LATENCY) )
            {
                gappara_SlaveLatency = latency;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_TIMEOUT_MULTIPLIER:
        {
            uint16_t newTimeout = *((uint16_t*)pValue);
            if ( len == sizeof ( uint16_t )
                    && (newTimeout >= MIN_TIMEOUT_MULTIPLIER) && (newTimeout <= MAX_TIMEOUT_MULTIPLIER) )
            {
                gapPara_TimeoutMultiplier = newTimeout;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_CE_LENGTH:
        {
            uint16_t ce_length = *((uint16_t*)pValue);
            if ( len == sizeof ( uint16_t ) )
            {
                gapPara_CE_Length = ce_length;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    case GAPPRRA_MAXTPDUDSCREDITS:
        {
            uint8_t credits = *((uint8_t*)pValue);
            if ( len == sizeof ( uint8_t ) )
            {
                gapPara_maxTPDUdsCredits = credits;
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
        }
        break;

    default:
        break;
    }

    return ( ret );
}

/**
* @brief   Get a central role parameter.
*
* @param[in]   param - profile parameter ID: @ref GAP_PARAM_CENTRAL_OBSERV ; @ref GAP_PARAM_PERIPH_CENTRAL ;
*              @ref GAP_PARAM_CENTRAL_ONLY ; @ref GAP_PARAM_ALL_ROLES
* @param[out]   pValue - pointer to buffer to contain the read data
*
* @retval  gapAPI_CauseSuccess Operation successful.
* @retval  INVALIDPARAMETER Invalid parameter ID.
*/
TGAP_STATUS centralGetGapParameter( uint16_t param, void *pValue )
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


    case GAPPRRA_CONN_DEST_ADDR:
        {
            memcpy(pValue, gapPara_DestDevAddr, B_ADDR_LEN ) ;
        }
        break;

    case GAPPRRA_CONN_DEST_ADDR_TYPE:
        *((uint8_t*)pValue) = gapPara_DestDevAddrType;
        break;

    case GAPPRRA_CONN_SCAN_TIMEOUT:
        *((uint16_t*)pValue) = gapPara_Conn_ScanTimeout;
        break;

    case GAPPRRA_MIN_CONN_INTERVAL:
        *((uint16_t*)pValue) = gapPara_MinConnInterval;
        break;

    case GAPPRRA_MAX_CONN_INTERVAL:
        *((uint16_t*)pValue) = gapPara_MaxConnInterval;
        break;

    case GAPPRRA_SLAVE_LATENCY:
        *((uint16_t*)pValue) = gappara_SlaveLatency;
        break;

    case GAPPRRA_TIMEOUT_MULTIPLIER:
        *((uint16_t*)pValue) = gapPara_TimeoutMultiplier;
        break;

    case GAPPRRA_CE_LENGTH:
        *((uint16_t*)pValue) = gapPara_CE_Length;
        break;

    case GAPPRRA_CONN_BD_ADDR:
        memcpy( pValue, gapPara_ConnectedDevAddr, B_ADDR_LEN ) ;
        break;

    case GAPPRRA_CONN_BD_ADDR_TYPE:
        *((uint8_t*)pValue) = gapPara_ConnectedDevAddrType;
        break;

    case GAPPRRA_CONN_INTERVAL:
        *((uint16_t*)pValue) = gapPara_ConnInterval;
        break;

    case GAPPRRA_CONN_LATENCY:
        *((uint16_t*)pValue) = gapPara_ConnSlaveLatency;
        break;

    case GAPPRRA_CONN_TIMEOUT:
        *((uint16_t*)pValue) = gapPara_ConnTimeout;
        break;

    case GAPPRRA_CONNHANDLE:
        *((uint16_t*)pValue) = gapPara_ConnHandle;
        break;

    case GAPPRRA_DSPOOLID:
        *((uint16_t*)pValue) = gapPara_dsPoolID;

        break;

    case GAPPRRA_DSDATAOFFSET:
        *((uint16_t*)pValue) = gapPara_dsDataOffset;
        break;

    case GAPPRRA_MAXTPDUSIZE:
        *((uint16_t*)pValue) = gapPara_maxTPDUSize;
        break;

    case GAPPRRA_MAXTPDUDSCREDITS:
        *((uint8_t*)pValue) = gapPara_maxTPDUdsCredits;

        break;

    case GAPPRRA_MTUSIZE:
        *((uint16_t*)pValue) = gapPara_mtuSize;
        break;

    case GAPPARA_BLE_RSSI:
        *((int8_t*)pValue) = gapPara_btRSSI;
        break;

    default:

        break;
    }

    return ( ret );
}

/**
 * @internal
 *
 * @brief   Register device state change callbacks from profile.
 *
 * @param   pAppCallbacks - Callback app providedfor notify device state change
 *
 * @return  events not processed
 */
TGAP_STATUS central_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks )
{
    if ( gapRole_state == GAP_CENTRAL_STATE_INIT )
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

/**
 * @internal
 *
 * @brief   Register parameter update callbacks from profile.
 *
 * @param   pParamUpdateCB - Callback app provided for notify connection parameter change
 *
 */
void central_RegisterAppCBs( gapRolesParamUpdateCB_t pParamUpdateCB )
{
    if ( pParamUpdateCB != NULL )
    {
        pGapRoles_ParamUpdateCB = pParamUpdateCB;
    }
}

/**
 *
 * @brief  Register central direct callback  
 *
 * The registered callback will be used to send message to application directly from central role.
 *
 * @param[in]   pAppCallback - Callback app provided by application: @ref pfnCentralAppCB_t
 *
 */
void central_RegisterAppDirectCB( pfnCentralAppCB_t pAppCallback )
{
    pGapCentralAppDirectCb = pAppCallback;
}

/**
 * @internal
 *
 * @brief   Initialize default parameters for central role.
 */
void central_GapParaInit(void)
{
    /* Initiate GAP Central Role's common parameters. */
    GAP_ParaInit(GAP_PROFILE_CENTRAL);
    
    gapStack_Initialized = 0;
    gapRole_state = GAP_CENTRAL_STATE_INIT;
    gapPara_ScanMode = GAP_SCAN_MODE_ACTIVE;
    gapPara_ScanInterval = 0x10;
    gapPara_ScanWindow = 0x10;
    gapPara_FilterPolicy = GAP_SCAN_FILTER_ANY;
    gapPara_FilterDuplicates = GAP_SCAN_FILTER_DUPLICATE_DISABLE;
    gapPara_ConnHandle = 0;
    gapPara_mtuSize = 23;
    gapPara_ConnectedDevAddrType = 0;
    gapPara_MinConnInterval = DEFAULT_MIN_CONN_INTERVAL;
    gapPara_MaxConnInterval = DEFAULT_MAX_CONN_INTERVAL;
    gappara_SlaveLatency = MIN_SLAVE_LATENCY;
    gapPara_TimeoutMultiplier = DEFAULT_TIMEOUT_MULTIPLIER;
    gapPara_ConnInterval = 0;
    gapPara_ConnSlaveLatency = 0;
    gapPara_ConnTimeout = 0;
}

/**
 * @brief   Update the link connection parameters.
 *
 * @retval  gapAPI_CauseSuccess: Connection update started.
 * @retval  gapAPI_IncorrectMode: No connection to update.
 */
TGAP_STATUS central_SendUpdateParam()
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    // If there is no existing connection no update need be sent
    if ( gapRole_state != GAP_CENTRAL_STATE_CONNECTED )
    {
        return ( gapAPI_NotConnected );
    }
    blueAPI_LEConnectionUpdateReq(gapPara_ConnHandle,
                                  gapPara_MinConnInterval,
                                  gapPara_MaxConnInterval,
                                  gappara_SlaveLatency,
                                  gapPara_TimeoutMultiplier);
    return ret;
}

/**
 * @brief   Start a device discovery scan.
 *
 * @retval  gapAPI_CauseSuccess Discovery scan started.
 * @retval  gapAPI_IncorrectMode Invalid profile role.
 * @retval  gapAPI_AlreadyInRequestedMode Not available.
 */
TGAP_STATUS central_StartScan(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if (gapRole_state == GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN)
    {
        blueAPI_LEScanReq((TBlueAPI_LEScanMode)gapPara_ScanMode,
                          gapPara_ScanInterval,
                          gapPara_ScanWindow,
                          (TBlueAPI_LEFilterPolicy)gapPara_FilterPolicy,
                          (TBlueAPI_LocalBDType)bdAddrType,
                          gapPara_FilterDuplicates);
    }
    else
    {
        ret = gapAPI_IncorrectMode;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_StartScan fail because of state abnormal!!!", 0);
    }

    return ret;
}

/**
 * @brief   Cancel a device discovery scan.
 *
 * @retval  gapAPI_CauseSuccess: Cancel started.
 * @retval  gapAPI_IncorrectMode: Not in discovery mode.
 */
TGAP_STATUS central_StopScan(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if (gapRole_state == GAP_CENTRAL_STATE_SCANNING)
    {
        blueAPI_LEScanReq(blueAPI_LEScanDisabled,
                          gapPara_ScanInterval,
                          gapPara_ScanWindow,
                          (TBlueAPI_LEFilterPolicy)gapPara_FilterPolicy,
                          (TBlueAPI_LocalBDType)bdAddrType,
                          gapPara_FilterDuplicates);
    }
    else
    {
        ret = gapAPI_IncorrectMode;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_StopScan fail because of state abnormal!!!", 0);
    }

    return ret;
}

/**
 * @brief   Establish a link to a peer device.
 *
 * @retval  TRUE Send request success.
 * @retval  FALSE Send request failed.
 */
bool central_Connect()
{
    bool ret = TRUE;
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if(gapRole_state == GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN)
    {
        ret = blueAPI_ConnectGATTMDLReq(gapPara_DestDevAddr,
                                     (TBlueAPI_RemoteBDType)gapPara_DestDevAddrType,
                                     (TBlueAPI_LocalBDType)bdAddrType,
                                     gapPara_ScanInterval,
                                     gapPara_ScanWindow,
                                     gapPara_Conn_ScanTimeout,
                                     gapPara_MinConnInterval,
                                     gapPara_MaxConnInterval,
                                     gappara_SlaveLatency,
                                     gapPara_TimeoutMultiplier,
                                     gapPara_CE_Length
                                    );
        if(ret)
        {
            /* Enter CONNECTING state when send conn req, and notify application of this state. */
            gapRole_state = GAP_CENTRAL_STATE_CONNECTING;
            (*pGapDeviceStateChangeCb)(gapRole_state);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_Connect fail because of state abnormal!!!", 0);
        ret = FALSE;
    }
    return ret;
}

/**
 * @brief   Disconnect a link.
 *
 * @retval  gapAPI_CauseSuccess Terminate started.
 * @retval  gapAPI_IncorrectMode No link to terminate.
 */
TGAP_STATUS central_Disconnect( void )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint16_t conn_interval;
    if ( gapRole_state == GAP_CENTRAL_STATE_CONNECTED )
    {
        /* Delay 2 * conn_interval * 1.25 ms, avoid buffer acl data broke by disconnection. */
        centralGetGapParameter(GAPPRRA_CONN_INTERVAL, &conn_interval);
        vTaskDelay((conn_interval * 2.5) / portTICK_RATE_MS);
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Disconnect after 2 interval delay", 0);
#endif
        blueAPI_DisconnectMDLReq(gapPara_ConnHandle, blueAPI_CauseConnectionDisconnect);
    }
    else
    {
        ret = gapAPI_IncorrectMode;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_Disconnect fail because of state abnormal!!!", 0);
    }
    return ret;
}

/**
* @brief   Set the TX Power of BLE radio in the Central Role.
*
* @param[in]   tx_power_index - valid tx power index: @ref BLE_TX_POWER_INDEX
*
* @retval  gapAPI_CauseSuccess Set request sent success.
* @retval  gapAPI_ErrorUnknown Set request sent fail.
*/
TGAP_STATUS central_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_SetBleTxPowerReq(tx_power_index))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_SetTxPower fail !!!", 0);
    }

    return ret;
}

/**
* @brief   Read rssi value of the connection in the Central Role.
*
* @retval  gapAPI_CauseSuccess Read request sent success.
* @retval  gapAPI_ErrorUnknown Read request sent fail.
*/
TGAP_STATUS central_ReadRSSI( void )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_ReadRSSI(gapPara_ConnHandle))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_ReadRSSI fail !!!", 0);
    }

    return ret;
}

/**
* @brief   Set the Host Channel Classification.
*
* @param[in]   pChannelMap - channel bit map, the most 3 significant bits reserved.
*
* @retval  gapAPI_CauseSuccess Set request sent success.
* @retval  gapAPI_ErrorUnknown Set request sent fail.
*/
TGAP_STATUS central_SetHostChanClass( uint8_t *pChannelMap )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_LESetHostChanClass(pChannelMap))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_SetHostChanClass fail !!!", 0);
    }

    return ret;
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_DeviceConfigSetRsp.
 *
 * @param   pDevCfgSetRsp - message informed from upper stack
 * @retval None
 */
static void central_Handle_DeviceConfigSetRsp(PBlueAPI_DeviceConfigSetRsp pDevCfgSetRsp )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_DeviceConfigSetRsp: opcode=0x%x, cause=0x%x.", 
            2, pDevCfgSetRsp->opCode, pDevCfgSetRsp->cause);
#endif
    gapRole_state = GAP_CENTRAL_STATE_STACK_READY;
    
    switch (pDevCfgSetRsp->opCode)
    {
        case blueAPI_DeviceConfigDeviceName:
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
        case blueAPI_DeviceConfigAppearance:
            {
                if (!gapStack_Initialized)
                {
                    GAPBondMgr_SetPairable();
                    if(gapBond_FixedPasskeyEnable)
                    {
                        uint32_t passKey = 0;
                        GAPBondMgr_GetParameter(GAPBOND_PASSKEY, &passKey);
                        passKey = BLUE_API_USE_LE_FIXED_DISPLAY_VALUE | passKey;
                        blueAPI_DeviceConfigSecuritySetReq(passKey);
                    }
                    else
                    {
                        gapRole_state = GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN;
                        (*pGapDeviceStateChangeCb)(gapRole_state);
                    }
                }
            }
            break;
        case blueAPI_DeviceConfigPerPrefConnParam:
            break;
        case blueAPI_DeviceConfigSecurity:
            {
                gapRole_state = GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN;
                (*pGapDeviceStateChangeCb)(gapRole_state);
            }
            break;
        case blueAPI_DeviceConfigStore:
            break;
        default:
            break;
    }

}

/**
 * @internal
 *
 * @brief   handle Set Random Address response.
 *
 * @param   pSetRandomAddressRsp - message informed from upper stack
 *
 */
static void central_Handle_SetRandomAddressRsp( PBlueAPI_SetRandomAddressRsp pSetRandomAddressRsp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_SetRandomAddressRsp: cause=0x%x, subCause=0x%x.", 
            2, pSetRandomAddressRsp->cause, pSetRandomAddressRsp->subCause);
#endif
}

/**
 * @internal
 *
 * @brief   handle Create MDL indication.
 *
 * @param   pCreateMDLInd - message informed from upper stack
 *
 */
static void central_Handle_CreateMDLInd( PBlueAPI_CreateMDLInd pCreateMDLInd )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_CreateMDLInd: bd = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x, bdtype=%d, MDL_ID=0x%x",
                8, pCreateMDLInd->remote_BD[5],
                   pCreateMDLInd->remote_BD[4],
                   pCreateMDLInd->remote_BD[3],
                   pCreateMDLInd->remote_BD[2],
                   pCreateMDLInd->remote_BD[1],
                   pCreateMDLInd->remote_BD[0],
                   pCreateMDLInd->remote_BD_type,
                   pCreateMDLInd->local_MDL_ID);
#endif

    memcpy(gapPara_ConnectedDevAddr, pCreateMDLInd->remote_BD, B_ADDR_LEN);
    gapPara_ConnectedDevAddrType = pCreateMDLInd->remote_BD_type;
    /* Accept connection here, means a GATT connection real established. */
    blueAPI_CreateMDLConf( pCreateMDLInd->local_MDL_ID,
                           1,
                           blueAPI_CauseAccept);
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_ConnectMDLRsp.
 *
 * @param   pConnectMDLRsp - message informed from upper stack
 *
 */
static void central_Handle_ConnectMDLRsp( PBlueAPI_ConnectMDLRsp pConnectMDLRsp )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_ConnectMDLRsp: bd = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x, bdtype=%d, MDL_ID=0x%x, cause=0x%x",
                9, pConnectMDLRsp->remote_BD[5],
                   pConnectMDLRsp->remote_BD[4],
                   pConnectMDLRsp->remote_BD[3],
                   pConnectMDLRsp->remote_BD[2],
                   pConnectMDLRsp->remote_BD[1],
                   pConnectMDLRsp->remote_BD[0],
                   pConnectMDLRsp->remote_BD_type,
                   pConnectMDLRsp->local_MDL_ID,
                   pConnectMDLRsp->cause);
#endif
    if (pConnectMDLRsp->cause == blueAPI_CauseSuccess)
    {
        /* Means connection established succeed, state will be later switched in ConnectMDLInfo. */
        memcpy(gapPara_ConnectedDevAddr, pConnectMDLRsp->remote_BD, B_ADDR_LEN);
        gapPara_ConnectedDevAddrType = pConnectMDLRsp->remote_BD_type;
    }
    else
    {
        /* Means connection establish failed, switch to IDLE_NO_SCAN_NO_CONN state. */
        gapRole_state = GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN;
        (*pGapDeviceStateChangeCb)(gapRole_state);
    }
}

/**
 * @internal
 *
 * @brief   handle MCL Status information.
 *
 * @param   pMCLStatusInfo - message informed from upper stack
 *
 */
static void central_Handle_MCLStatusInfo(PBlueAPI_MCLStatusInfo pMCLStatusInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_MCLStatusInfo: bd = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x, MCL_ID=0x%x, status=0x%x",
                8, pMCLStatusInfo->remote_BD[5],
                   pMCLStatusInfo->remote_BD[4],
                   pMCLStatusInfo->remote_BD[3],
                   pMCLStatusInfo->remote_BD[2],
                   pMCLStatusInfo->remote_BD[1],
                   pMCLStatusInfo->remote_BD[0],
                   pMCLStatusInfo->local_MCL_ID,
                   pMCLStatusInfo->status);
#endif
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_ConnectMDLInfo.
 *
 * @param   pConnectMDLInfo - message informed from upper stack
 *
 */
static void central_Handle_ConnectMDLInfo(PBlueAPI_ConnectMDLInfo pConnectMDLInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_ConnectMDLInfo: MDL_ID=0x%x,dsPoolID=0x%x,dsDataOffset=0x%x,MTU=0x%x,Credits=0x%x.",
                5, pConnectMDLInfo->local_MDL_ID,
                   pConnectMDLInfo->dsPoolID,
                   pConnectMDLInfo->dsDataOffset,
                   pConnectMDLInfo->maxTPDUSize,
                   pConnectMDLInfo->maxTPDUdsCredits);
#endif
    /* Connection establish complete, get the link information. */
    gapPara_ConnHandle = pConnectMDLInfo->local_MDL_ID;
    gapPara_dsPoolID = pConnectMDLInfo->dsPoolID;
    gapPara_dsDataOffset = pConnectMDLInfo->dsDataOffset;
    gapPara_maxTPDUSize = pConnectMDLInfo->maxTPDUSize;
    gapPara_maxTPDUdsCredits = pConnectMDLInfo->maxTPDUdsCredits;
    /* Switch to CONNECTED state, and inform application. */
    gapRole_state = GAP_CENTRAL_STATE_CONNECTED;
    (*pGapDeviceStateChangeCb)(gapRole_state);
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_DisconnectMDLRsp.
 *
 * @param   pDisconnectMDLRsp - message informed from upper stack
 *
 */
static void central_Handle_DisconnectMDLRsp( PBlueAPI_DisconnectMDLRsp pDisconnectMDLRsp )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_DisconnectMDLRsp: MDL_ID=0x%x, cause=0x%x.",
                2, pDisconnectMDLRsp->local_MDL_ID,
                   pDisconnectMDLRsp->cause);
#endif
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_DisconnectMDLInd.
 *
 * @param   pDisconnectMDLInd - message informed from upper stack
 *
 */
static void central_Handle_DisconnectMDLInd(PBlueAPI_DisconnectMDLInd pDisconnectMDLInd )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_DisconnectMDLInd: MDL_ID=0x%x, cause=0x%x.",
                2, pDisconnectMDLInd->local_MDL_ID,
                   pDisconnectMDLInd->cause);
#endif
    blueAPI_DisconnectMDLConf( pDisconnectMDLInd->local_MDL_ID );
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_DeleteMDLInfo.
 *
 * @param   pDeleteMDLInfo - message informed from upper stack
 *
 */
static void central_Handle_DeleteMDLInfo( PBlueAPI_DeleteMDLInfo pDeleteMDLInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_DeleteMDLInfo: MDL_ID=0x%x.",
                1, pDeleteMDLInfo->local_MDL_ID);
#endif
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_InternalEventInfo.
 *
 * @param   pInternalEventInfo - message informed from upper stack
 *
 */
static void central_Handle_InternalEventInfo( PBlueAPI_InternalEventInfo pInternalEventInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "central_Handle_InternalEventInfo: eventType=0x%x.",
                1, pInternalEventInfo->eventType);
#endif
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_ACLStatusInfo.
 *
 * @param   pACLStatusInfo - message informed from upper stack
 *
 */
static void central_Handle_ACLStatusInfo(PBlueAPI_ACLStatusInfo pACLStatusInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_ACLStatusInfo: status=%d",
               1, pACLStatusInfo->status);
#endif
    switch (pACLStatusInfo->status)
    {
        case blueAPI_ACLConnectedActive:
            break;
        case blueAPI_ACLConnectionDisconnected:
#ifdef GAP_DEBUG_FLAG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
                       "<-- central_Handle_ACLStatusInfo: remote addr: 0x%02x:%02x:%02x:%02x:%02x:%02x, addrtype:%d",
                       7,
                       pACLStatusInfo->remote_BD[0],
                       pACLStatusInfo->remote_BD[1],
                       pACLStatusInfo->remote_BD[2],
                       pACLStatusInfo->remote_BD[3],
                       pACLStatusInfo->remote_BD[4],
                       pACLStatusInfo->remote_BD[5],
                       pACLStatusInfo->remote_BD_type
                      );
#endif
            /* Means link disconnected, switch to IDLE_NO_SCAN_NO_CONN state, and inform application. */
            gapRole_state = GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN;
            (*pGapDeviceStateChangeCb)(gapRole_state);
            break;
        default:
            break;
    }
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_GATTMtuSizeInfo.
 *
 * @param   pMtuSizeInfo - message informed from upper stack
 *
 */
static void central_Handle_GATTMtuSizeInfo(PBlueAPI_GATTMtuSizeInfo  pMtuSizeInfo)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_GATTMtuSizeInfo: MDL_ID=0x%x,MTU=0x%x.",
               2, pMtuSizeInfo->local_MDL_ID, pMtuSizeInfo->mtuSize);
#endif
    if (pMtuSizeInfo->local_MDL_ID == gapPara_ConnHandle)
    {
        gapPara_mtuSize = pMtuSizeInfo->mtuSize;
    }
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_LEModifyWhitelistRsp.
 *
 * @param   pModifyWhitelistRsp - message informed from upper stack
 *
 */
static void central_Handle_LEModifyWhitelistRsp(PBlueAPI_LEModifyWhitelistRsp pModifyWhitelistRsp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"pModifyWhitelistRsp: operation=0x%x, cause=0x%x.",
               2, pModifyWhitelistRsp->operation, pModifyWhitelistRsp->cause);
#endif
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_LEScanRsp.
 *
 * @param   pLeScanResp - message informed from upper stack
 *
 */
static void central_Handle_LEScanRsp(PBlueAPI_LEScanRsp pLeScanResp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_LEScanRsp: cause=0x%x.",
               1, pLeScanResp->cause);
#endif
    if (pLeScanResp->cause == blueAPI_CauseSuccess)
    {
        if(gapRole_state == GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN)
        {
            gapRole_state = GAP_CENTRAL_STATE_SCANNING;
        }
        else if(gapRole_state == GAP_CENTRAL_STATE_SCANNING)
        {
            gapRole_state = GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN;
        }
        /* Inform current state to app, may be SCANNING or IDLE_NO_SCAN_NO_CONN. */
        (*pGapDeviceStateChangeCb)(gapRole_state);
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central Start or Stop Scan fail because of invalid params!!!", 0);
    }
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_LEScanInfo.
 *
 * @param   pLeScanInfo - message informed from upper stack
 *
 */
static void central_Handle_LEScanInfo(PBlueAPI_LEScanInfo pLeScanInfo)
{
    TCentralAppCB_MsgType msgType;
    TCentralAppCB_MsgData msgData;

#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_LEScanInfo: data sent to app.",0);
#endif

    /* Send Adv data to application to process, use callback. */
    if(pGapCentralAppDirectCb)
    {
        msgType = ADV_SCAN_RSP_DATA_MSGTYPE;
        msgData.pAdvScanRspData = pLeScanInfo;
        (*pGapCentralAppDirectCb)(msgType, msgData);
    }
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_LEConnectionUpdateRsp.
 *
 * @param   pConnectionUpdateRsp - message informed from upper stack
 *
 */
static void central_Handle_LEConnectionUpdateRsp(PBlueAPI_LEConnectionUpdateRsp pConnectionUpdateRsp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_LEConnectionUpdateRsp: MDL_ID=0x%x,cause=0x%x.",
               2, pConnectionUpdateRsp->local_MDL_ID,pConnectionUpdateRsp->cause);
#endif

    if (pConnectionUpdateRsp->cause != blueAPI_CauseSuccess)
    {
        pGapRoles_ParamUpdateCB(gapPara_ConnHandle, GAP_CONN_PARAM_UPDATE_STATUS_FAIL);
    }
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_LEConnectionUpdateInd.
 *
 * @param   pConnectionUpdateInd - message informed from upper stack
 *
 */
static void central_Handle_LEConnectionUpdateInd(PBlueAPI_LEConnectionUpdateInd pConnectionUpdateInd)
{
    TAppResult appResult = AppResult_Success;
    TBlueAPI_Cause blueAPI_cause = blueAPI_CauseAccept;
    TCentralAppCB_MsgType msgType;
    TCentralAppCB_MsgData msgData;

#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_LEConnectionUpdateInd: MDL_ID=0x%x.",
               1, pConnectionUpdateInd->local_MDL_ID);
#endif

    /* Send proposed connection update parameter to application to process, use callback. */
    if(pGapCentralAppDirectCb)
    {
        msgType = CONN_PARAM_CONFIRM_MSGTYPE;
        msgData.pConnUpdateIndData = pConnectionUpdateInd;
        appResult = (*pGapCentralAppDirectCb)(msgType, msgData);
    }

    if(appResult == AppResult_Reject)
    {
        blueAPI_cause = blueAPI_CauseReject;
    }

    blueAPI_LEConnectionUpdateConf(gapPara_ConnHandle, blueAPI_cause);
}

/**
 * @internal
 *
 * @brief   handle blueAPI_Event_LEConnectionParameterInfo.
 *
 * @param   pConnectionParameterInfo - message informed from upper stack
 *
 */
static void central_Handle_LEConnectionParameterInfo(PBlueAPI_LEConnectionParameterInfo pConnectionParameterInfo)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"central_Handle_LEConnectionParameterInfo: MDL_ID=0x%x,Interval=0x%x,Latency=0x%x,Timeout=0x%x.",
               4, pConnectionParameterInfo->local_MDL_ID,
                  pConnectionParameterInfo->connInterval,
                  pConnectionParameterInfo->connLatency,
                  pConnectionParameterInfo->supervisionTimeout);
#endif

    if (pConnectionParameterInfo->local_MDL_ID == gapPara_ConnHandle)
    {
        gapPara_ConnInterval = pConnectionParameterInfo->connInterval;
        gapPara_ConnSlaveLatency = pConnectionParameterInfo->connLatency;
        gapPara_ConnTimeout = pConnectionParameterInfo->supervisionTimeout;
        pGapRoles_ParamUpdateCB(gapPara_ConnHandle, GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS);
    }
}

/**
 * @internal
 *
 * @brief   BlueAPI message handler, returns non-zero value if calling routine may release message buffer.
 *
 * @param   pMsg - message informed from upper stack
 *
 */
bool central_ProcessGapEvent( PBlueAPI_UsMessage pMsg )
{
    switch ( pMsg->Command )
    {
        default:
#ifdef GAP_DEBUG_FLAG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAP_Central_Role_Not_ProcessEvent: Command(0x%02x)", 1, pMsg->Command);
#endif
            break;
        case blueAPI_EventDeviceConfigSetRsp:
            central_Handle_DeviceConfigSetRsp( &pMsg->p.DeviceConfigSetRsp);
            break;
        case blueAPI_EventSetRandomAddressRsp:
            central_Handle_SetRandomAddressRsp( &pMsg->p.SetRandomAddressRsp);
            break;
        case blueAPI_EventLEModifyWhitelistRsp:
            central_Handle_LEModifyWhitelistRsp(&pMsg->p.LEModifyWhitelistRsp);
            break;
        //for le scan
        case blueAPI_EventLEScanRsp:
            central_Handle_LEScanRsp(&pMsg->p.LEScanRsp);
            break;
        case blueAPI_EventLEScanInfo:
            central_Handle_LEScanInfo(&pMsg->p.LEScanInfo);
            break;
        //for establish connection
        case blueAPI_EventCreateMDLInd:
            central_Handle_CreateMDLInd(&pMsg->p.CreateMDLInd );
            break;
        case blueAPI_EventConnectMDLRsp:
            central_Handle_ConnectMDLRsp(&pMsg->p.ConnectMDLRsp );
            break;
        case blueAPI_EventMCLStatusInfo:
            central_Handle_MCLStatusInfo(&pMsg->p.MCLStatusInfo );
            break;
        case blueAPI_EventConnectMDLInfo:
            central_Handle_ConnectMDLInfo(&pMsg->p.ConnectMDLInfo );
            break;
        case blueAPI_EventDisconnectMDLRsp:
            central_Handle_DisconnectMDLRsp(&pMsg->p.DisconnectMDLRsp );
            break;
        case blueAPI_EventDisconnectMDLInd:
            central_Handle_DisconnectMDLInd(&pMsg->p.DisconnectMDLInd );
            break;
        case blueAPI_EventDeleteMDLInfo:
            central_Handle_DeleteMDLInfo(&pMsg->p.DeleteMDLInfo );
            break;
        case blueAPI_EventACLStatusInfo:
            central_Handle_ACLStatusInfo(&pMsg->p.ACLStatusInfo );
            break;
        case blueAPI_EventGATTMtuSizeInfo:
            central_Handle_GATTMtuSizeInfo(&pMsg->p.GATTMtuSizeInfo);
            break;
        //for connection parameter update
        case blueAPI_EventLEConnectionUpdateRsp:
            central_Handle_LEConnectionUpdateRsp(&pMsg->p.LEConnectionUpdateRsp);
            break;
        case blueAPI_EventLEConnectionUpdateInd:
            central_Handle_LEConnectionUpdateInd(&pMsg->p.LEConnectionUpdateInd);
            break;
        case blueAPI_EventLEConnectionParameterInfo:
            central_Handle_LEConnectionParameterInfo(&pMsg->p.LEConnectionParameterInfo);
            break;
        //for internal debug use only
        case blueAPI_EventInternalEventInfo:
            central_Handle_InternalEventInfo(&pMsg->p.InternalEventInfo );
            break;
    }
    return true;
}

/**
 * @internal
 *
 * @brief   Central Profile Task event processing function.
 *
 * @param   pMsg - Message from BT stack
 *
 * @return  True- success
 */
bool central_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg )
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
        central_ProcessGapEvent(pMsg);
    }
    
    GAPBondMgr_ProcessEvent(pMsg);
    bufferReleased = client_HandleMessage(pMsg);

    if ( !bufferReleased && pMsg)
    {
        blueAPI_BufferRelease(pMsg);
        pMsg = NULL;
    }

    return true;
}

/*********************************************************************
*********************************************************************/

