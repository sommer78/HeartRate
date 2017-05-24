enum { __FILE_NUM__ = 0 };

/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    peripheral.c
  * @brief   This file provides the GAP role Peripheral functions.
  * @details
  * @author  Ranhui
  * @date    11-March-2015
  * @version v3.5.0
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
#include "peripheral.h"
#include "blueapi_types.h"
#include "blueapi.h"
#include "profile.h"
#include "trace.h"

/** @cond private**/
/** @addtogroup Peripheral_Role
  * @{
  */

/** @defgroup Peripheral_Role_Private_Constants Peripheral Role Private Constants
  * @{
  */
/** Add all private defines here **/
#define DEFAULT_MIN_CONN_INTERVAL     0x0006  // default minimun connection interval, 100 milliseconds
#define DEFAULT_MAX_CONN_INTERVAL     0x0C80  // default maximun connection interval, 4 seconds
#define DEFAULT_MIN_ADV_INTERVAL      0x0020  // default minimun advertising interval, 20ms, 0.625ms/step
#define DEFAULT_MAX_ADV_INTERVAL      0x4000  // default maximun advertising interval, 10240ms, 0.625ms/step
#define MIN_CONN_INTERVAL             0x0006  // minimum connection interval
#define MAX_CONN_INTERVAL             0x0C80  // maximum connection interval
#define DEFAULT_TIMEOUT_MULTIPLIER    1000    // default timeout multiplier, supervision timeout
#define CONN_INTERVAL_MULTIPLIER      6       // connection interval multiplier
#define MIN_SLAVE_LATENCY             0       // the minimum slave latency
#define MAX_SLAVE_LATENCY             500     // the maximum slave latency
#define MIN_TIMEOUT_MULTIPLIER        0x000a  // minimum timeout multiplier
#define MAX_TIMEOUT_MULTIPLIER        0x0c80  // maximum timeout multiplier
/** End of Peripheral_Role_Private_Constants
  * @}
  */

/** @defgroup Peripheral_Role_Private_Variables Peripheral Role Private Variables
  * @{
  */
/** Add all private variables here **/
extern int8_t   gapPara_btRSSI;     /* Handled in gap_common, but only used in Peripheral and Central. */
extern uint8_t gapBond_FixedPasskeyEnable;
static uint8_t  gapStack_Initialized = 0;
static gaprole_States_t gapRole_state = GAP_PERIPHERAL_STATE_INIT;
static uint8_t  gapPara_AdvertDataLen = 3;
static uint8_t  gapPara_AdvertData[B_MAX_ADV_LEN] =
{
    0x02,   // length of this data
    GAP_ADTYPE_FLAGS,   // AD Type = Flags
    // Limited Discoverable & BR/EDR not supported
    (GAP_ADTYPE_FLAGS_LIMITED|GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED),
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static uint8_t  gapPara_ScanRspDataLen = 0;
static uint8_t  gapPara_ScanRspData[B_MAX_ADV_LEN] = {0};
static uint8_t  gapPara_AdvEventType = GAP_ADTYPE_ADV_IND;
static uint8_t  gapPara_AdvDirectAddrType = PEER_ADDRTYPE_PUBLIC;
static uint8_t  gapRole_AdvDirectAddr[B_ADDR_LEN] = {0};
static uint8_t  gapPara_AdvChanMap = GAP_ADVCHAN_ALL;
static uint8_t  gapParaInternal_AdvFilterScanPolicy = blueAPI_LEFilterAny;
static uint8_t  gapParaInternal_AdvFilterConnectPolicy = blueAPI_LEFilterAny;
static uint8_t  gapPara_AdvFilterPolicy = GAP_FILTER_POLICY_ALL;
static uint16_t gapPara_AdvMinInterval = DEFAULT_MIN_ADV_INTERVAL;
static uint16_t gapPara_AdvMaxInterval = DEFAULT_MAX_ADV_INTERVAL;
static uint8_t  gap_AdvUseWL = FALSE;
//static uint16_t gapPara_ConnHandle = 0;
static uint16_t gapPara_dsPoolID;
static uint16_t gapPara_dsDataOffset;
static uint16_t gapPara_maxTPDUSize;
static uint8_t  gapPara_maxTPDUdsCredits;
static uint16_t gapPara_mtuSize = 23;
static uint8_t  gapPara_ConnectedDevAddr[B_ADDR_LEN] = {0};
static uint8_t  gapPara_ConnectedDevAddrType = 0;
static uint8_t  gapPara_WhiteListDevAddr[B_ADDR_LEN] = {0};
static uint8_t  gapPara_WhiteListDevAddrType = 0;
static uint8_t  gapPara_AdvEnableDefault = FALSE;
static uint16_t gapPara_MinConnInterval = DEFAULT_MIN_CONN_INTERVAL;
static uint16_t gapPara_MaxConnInterval = DEFAULT_MAX_CONN_INTERVAL;
static uint16_t gappara_SlaveLatency = MIN_SLAVE_LATENCY;
static uint16_t gapPara_TimeoutMultiplier = DEFAULT_TIMEOUT_MULTIPLIER;
static uint16_t gapPara_ConnInterval = 0;
static uint16_t gapPara_ConnSlaveLatency = 0;
static uint16_t gapPara_ConnTimeout = 0;
static uint8_t  gapPara_DisconnectedReason = 0;
static uint8_t  gap_ConStateNotified  = FALSE;
/** End of Peripheral_Role_Private_Variables
  * @}
  */
#ifdef ANCS_SUPPORT
extern uint16_t    ancs_write_handle;
#endif
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
extern bool gapbondmgr_CheckRandomAddress(uint8_t * unresolved_addr, uint8_t unresolved_addr_type, remote_BD_struct* pAddr);
#endif
/** @endcond **/

/**
 * @brief       Set a peripheral role parameter.
 *
 *              NOTE: You can call this function with a GAP Peripheral Parameter ID and it will set the
 *                      GAP Parameter.  GAP Peripheral Parameters are defined in (peripheral.h).  
 *                      If the "len" field sets to the size of a "uint16_t", the
 *                      "pValue" field must point to a "uint16".
 *
 * @param[in]         param - Profile parameter ID:  @ref GAP_PARAM_PERIPH_BROAD ; @ref GAP_PARAM_PERIPH_CENTRAL ;
*              @ref GAP_PARAM_PERIPH_ONLY ; @ref GAP_PARAM_ALL_ROLES
 * @param[in]         len - Length of data to write
 * @param[in]         pValue - Pointer to data to write.  This is dependent on
 *                      the parameter ID and will be cast to the appropriate
 *                      data type (example: data type of uint16 will be cast to
 *                      uint16 pointer).
 *
 * @return Set result
 * @retval gapAPI_CauseSuccess Set parameter success.
 * @retval other Set parameter failed.
 */
TGAP_STATUS peripheralSetGapParameter( uint16_t param, uint8_t len, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    /* set common params in GAP. */
    if((param >= GAPPARA_PROFILE_ROLE) && (param <= GAPPARA_BLE_PRIVACY_MODE))
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
            #if defined (RTL8762AX_VB)
            blueAPI_SetLEAdvertiseChannelMap(gapPara_AdvChanMap);
            #endif
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
                gap_AdvUseWL = TRUE;
                
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
                gap_AdvUseWL = FALSE;
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


    case GAPPRRA_ADV_WL_BD_ADDR:
        if ( len == B_ADDR_LEN )
        {
            memcpy( gapPara_WhiteListDevAddr, pValue, B_ADDR_LEN ) ;
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPPRRA_ADV_WL_BD_ADDR_TYPE:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= PEER_ADDRTYPE_PRIVATE_RESOLVE) )
        {
            gapPara_WhiteListDevAddrType = *((uint8_t*)pValue);
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

    default:
        ret = gapAPI_InvalidRange;
        break;
    }

    return ( ret );
}

/**
 * @brief           Get a peripheral role parameter.
 *
 *                      NOTE: You can call this function with a GAP Peripheral Parameter ID and it will get a
 *                      GAP Peripheral Parameter.  GAP Peripheral Parameters are defined in (peripheral.h). 
 *
 * @param[in]         param - Profile parameter ID: @ref GAP_PARAM_PERIPH_BROAD ; @ref GAP_PARAM_PERIPH_CENTRAL ;
*              @ref GAP_PARAM_PERIPH_ONLY ; @ref GAP_PARAM_ALL_ROLES
 * @param[out]        pValue - Pointer to location to get the value.  This is dependent on
 *                      the parameter ID and WILL be cast to the appropriate
 *                      data type (example: data type of uint16 will be cast to
 *                      uint16 pointer).
 *
 * @return Get result
 * @retval gapAPI_CauseSuccess Get parameter success.
 * @retval other Get parameter failed.
 */
TGAP_STATUS peripheralGetGapParameter( uint16_t param, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    /* get common params in GAP. */
    if((param >= GAPPARA_PROFILE_ROLE) && (param <= GAPPARA_BLE_PRIVACY_MODE))
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

    case GAPPRRA_ADV_INTERVAL_MIN:
        memcpy( pValue, &gapPara_AdvMinInterval, sizeof(gapPara_AdvMinInterval) ) ;
        break;

    case GAPPRRA_ADV_INTERVAL_MAX:
        memcpy( pValue, &gapPara_AdvMaxInterval, sizeof(gapPara_AdvMaxInterval) ) ;
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

    case GAPPRRA_CONN_BD_ADDR:
        memcpy( pValue, gapPara_ConnectedDevAddr, B_ADDR_LEN ) ;
        break;

    case GAPPRRA_CONN_BD_ADDR_TYPE:
        *((uint8_t*)pValue) = gapPara_ConnectedDevAddrType;
        break;

    case GAPPRRA_ADV_WL_BD_ADDR:
        memcpy( pValue, gapPara_WhiteListDevAddr, B_ADDR_LEN ) ;
        break;

    case GAPPRRA_ADV_WL_BD_ADDR_TYPE:
        *((uint8_t*)pValue) = gapPara_WhiteListDevAddrType;
        break;

    case GAPPRRA_ADV_ENABLE_DEFAULT:
        *((uint8_t*)pValue) = gapPara_AdvEnableDefault;
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

    case GAPPRRA_DISCONNECTED_REASON:
        *((uint8_t*)pValue) = gapPara_DisconnectedReason;
        break;

    case GAPPARA_BLE_RSSI:
        *((int8_t*)pValue) = gapPara_btRSSI;
        break;

    default:
        ret = gapAPI_InvalidRange;
        break;
    }

    return ( ret );
}

/**
 * @fn          GAPRole_RegisterDeviceStateChangeCB
 * @brief       Register application's device connection state callbacks.
 * @param[in]    pAppCallbacks  - pointer to device state change callback
 *
 * @return     void
 */
TGAP_STATUS peripheral_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks )
{
    if ( gapRole_state == GAP_PERIPHERAL_STATE_INIT )
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
 * @fn          GAPRole_RegisterAppCBs
 * @brief       Register application's connection parameter update callback.
 * @param    pParamUpdateCB  - pointer to connection parameter update callback
 *
 * @return     void
 */
void peripheral_RegisterAppCBs( gapRolesParamUpdateCB_t pParamUpdateCB )
{
    if ( pParamUpdateCB != NULL )
    {
        pGapRoles_ParamUpdateCB = pParamUpdateCB;
    }
}

/**
 * @fn          peripheral_GapParaInit
 * @brief      Initialize parameters of gap peripheral role.            
 *
 * @return     void
 */
void peripheral_GapParaInit(void)
{
    /* Initiate GAP Peripheral Role's common parameters. */
    GAP_ParaInit(GAP_PROFILE_PERIPHERAL);
    
    gapStack_Initialized = 0;
    gapRole_state = GAP_PERIPHERAL_STATE_INIT;
    gapPara_AdvEventType = GAP_ADTYPE_ADV_IND;
    gapPara_AdvDirectAddrType = PEER_ADDRTYPE_PUBLIC;
    gapPara_AdvChanMap = GAP_ADVCHAN_ALL;
    gapPara_AdvFilterPolicy = GAP_FILTER_POLICY_ALL;
    gapPara_AdvMinInterval = DEFAULT_MIN_ADV_INTERVAL;
    gapPara_AdvMaxInterval = DEFAULT_MAX_ADV_INTERVAL;
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
    gap_AdvUseWL = FALSE;
    gapPara_AdvEnableDefault = FALSE;
    gap_ConStateNotified = FALSE;
}

/**
 * @fn          peripheral_Handle_DeviceConfigSetRsp
 * @brief      process blueAPI_EventDeviceConfigSetRsp message from bt stack
 * @param    pDevCfgSetRsp  - pointer to tBlueAPI_DeviceConfigSetRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_DeviceConfigSetRsp(PBlueAPI_DeviceConfigSetRsp pDevCfgSetRsp )
{
    gapRole_state = GAP_PERIPHERAL_STATE_STACK_READY;
    
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
                GAPBondMgr_SetPairable();
                if(gapBond_FixedPasskeyEnable)
                {
                    uint32_t passKey = 0;
                    GAPBondMgr_GetParameter(GAPBOND_PASSKEY, &passKey);
                    passKey = BLUE_API_USE_LE_FIXED_DISPLAY_VALUE | passKey;
                    blueAPI_DeviceConfigSecuritySetReq(passKey);
                }
#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
                {
                    uint8_t key_mode;
                    GAPBondMgr_GetParameter(GAPBOND_KEY_DISTRIBUTION, &key_mode);
                    blueAPI_SetSMPKeyDistribution(key_mode);
                }
#endif
                //blueAPI_SetAdvFastDisable(1);
                profileServiceRegister();
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

/**
 * @fn          peripheral_Handle_LEModifyWhitelistRsp
 * @brief      process blueAPI_EventLEModifyWhitelistRsp message from bt stack
 * @param    pModifyWhiteListRsp  - pointer to TBlueAPI_LEModifyWhitelistRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_LEModifyWhitelistRsp(PBlueAPI_LEModifyWhitelistRsp pModifyWhiteListRsp )
{
    uint8_t bdAddrType;
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);

    blueAPI_LEAdvertiseParameterSetReq((TBlueAPI_LEAdvType)gapPara_AdvEventType,
                                       (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterScanPolicy,
                                       (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterConnectPolicy,
                                       gapPara_AdvMinInterval,
                                       gapPara_AdvMaxInterval,
                                       (TBlueAPI_LocalBDType)bdAddrType,
                                       gapRole_AdvDirectAddr,
                                       (TBlueAPI_RemoteBDType)gapPara_AdvDirectAddrType
                                      );
}

/**
 * @fn          peripheral_Handle_LEAdvertiseRsp
 * @brief      process blueAPI_EventLEAdvertiseRsp message from bt stack
 * @param    pAdvertiseRsp  - pointer to TBlueAPI_LEAdvertiseRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_LEAdvertiseRsp( PBlueAPI_LEAdvertiseRsp pAdvertiseRsp)
{
    gaprole_States_t old_gapRole_state = gapRole_state;
    if (pAdvertiseRsp->cause ==  blueAPI_CauseSuccess)
    {
        switch (pAdvertiseRsp->advMode)
        {
        case blueAPI_LEAdvModeDisabled:
            {
                if (gapRole_state == GAP_PERIPHERAL_STATE_ADVERTISING)
                {
                    gapRole_state = GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN;
                }
                else if (gapRole_state == GAP_PERIPHERAL_STATE_CONNECTED_ADV)
                {
                    gapRole_state = GAP_PERIPHERAL_STATE_CONNECTED;
                }
            }
            break;

        case blueAPI_LEAdvModeEnabled:
            {
                if (gapRole_state == GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN)
                {
                    gapRole_state = GAP_PERIPHERAL_STATE_ADVERTISING;
                }
                else if (gapRole_state == GAP_PERIPHERAL_STATE_CONNECTED)
                {
                    gapRole_state = GAP_PERIPHERAL_STATE_CONNECTED_ADV;
                }

                if (gapStack_Initialized == 0)
                {
                    gapStack_Initialized = 1;
                }

            }
            break;

        }
    }
    else
    {
        if (pAdvertiseRsp->advMode ==  blueAPI_LEAdvModeDirectedHighDuty)
        {
            if (gapRole_state == GAP_PERIPHERAL_STATE_ADVERTISING)
            {
                gapRole_state = GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN;
            }
            else if (gapRole_state == GAP_PERIPHERAL_STATE_CONNECTED_ADV)
            {
                gapRole_state = GAP_PERIPHERAL_STATE_CONNECTED;
            }
        }

    }

    if (gapRole_state != old_gapRole_state)
    {
        pGapDeviceStateChangeCb(gapRole_state);
    }

}

/**
 * @fn          peripheral_Handle_SetRandomAddressRsp
 * @brief      process blueAPI_EventSetRandomAddressRsp message from bt stack
 * @param    pSetRandomAddressRsp  - pointer to TBlueAPI_SetRandomAddressRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_SetRandomAddressRsp( PBlueAPI_SetRandomAddressRsp pSetRandomAddressRsp)
{

}

/**
 * @fn          peripheral_Handle_LEAdvertiseParameterRsp
 * @brief      process blueAPI_EventLEAdvertiseParameterSetRsp message from bt stack
 * @param    pAdvertiseParameterSetRsp  - pointer to TBlueAPI_LEAdvertiseParameterSetRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_LEAdvertiseParameterRsp( PBlueAPI_LEAdvertiseParameterSetRsp pAdvertiseParameterSetRsp)
{
    blueAPI_LEAdvertiseDataSetReq(
        blueAPI_LEDataTypeAdvertisingData,
        gapPara_AdvertDataLen,
        gapPara_AdvertData
    );
}

/**
 * @fn          peripheral_Handle_LEAdvertiseDataSetRsp
 * @brief      process blueAPI_EventLEAdvertiseDataSetRsp message from bt stack
 * @param    pAdvertiseDataSetRsp  - pointer to TBlueAPI_LEAdvertiseDataSetRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_LEAdvertiseDataSetRsp( PBlueAPI_LEAdvertiseDataSetRsp pAdvertiseDataSetRsp)
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
        if (pAdvertiseDataSetRsp->cause == blueAPI_CauseSuccess)
        {
            if (gapPara_AdvEventType == GAP_ADTYPE_ADV_HDC_DIRECT_IND)
            {

                if (gapRole_state == GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN)
                {
                    gapRole_state = GAP_PERIPHERAL_STATE_ADVERTISING;
                }
                else if (gapRole_state == GAP_PERIPHERAL_STATE_CONNECTED)
                {
                    gapRole_state = GAP_PERIPHERAL_STATE_CONNECTED_ADV;
                }

                pGapDeviceStateChangeCb(gapRole_state);

                blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeDirectedHighDuty);
            }
            else
            {
                blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeEnabled);
            }
        }
        else
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, "peripheral_Handle_LEAdvertiseDataSetRsp failed\n\n", 0);
        }

    }
}

/**
 * @fn          peripheral_Handle_CreateMDLInd
 * @brief      process blueAPI_EventCreateMDLInd message from bt stack
 * @param    pCreateMDLInd  - pointer to tBlueAPI_CreateMDLInd message
 *
 * @return     bool
 */
static void peripheral_Handle_CreateMDLInd( PBlueAPI_CreateMDLInd pCreateMDLInd )
{
    memcpy(gapPara_ConnectedDevAddr, pCreateMDLInd->remote_BD, B_ADDR_LEN);
    gapPara_ConnectedDevAddrType = pCreateMDLInd->remote_BD_type;

    blueAPI_CreateMDLConf( pCreateMDLInd->local_MDL_ID,
                           1,
                           blueAPI_CauseAccept);

}

/**
 * @fn          peripheral_Handle_ConnectMDLRsp
 * @brief      process blueAPI_EventConnectMDLRsp message from bt stack
 * @param    pConnectMDLRsp  - pointer to TBlueAPI_ConnectMDLRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_ConnectMDLRsp( PBlueAPI_ConnectMDLRsp pConnectMDLRsp )
{
    if (pConnectMDLRsp->cause == blueAPI_CauseSuccess)
    {
        memcpy(gapPara_ConnectedDevAddr, pConnectMDLRsp->remote_BD, B_ADDR_LEN);
        gapPara_ConnectedDevAddrType = pConnectMDLRsp->remote_BD_type;
    }
}

/**
 * @fn          peripheral_Handle_MCLStatusInfo
 * @brief      process blueAPI_EventMCLStatusInfo message from bt stack
 * @param    pMCLStatusInfo  - pointer to TBlueAPI_MCLStatusInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_MCLStatusInfo(PBlueAPI_MCLStatusInfo pMCLStatusInfo )
{

}

/**
 * @fn          peripheral_Handle_ConnectMDLInfo
 * @brief      process blueAPI_EventConnectMDLInfo message from bt stack
 * @param    pConnectMDLInfo  - pointer to TBlueAPI_ConnectMDLInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_ConnectMDLInfo(PBlueAPI_ConnectMDLInfo pConnectMDLInfo )
{
    gapPara_ConnHandle = pConnectMDLInfo->local_MDL_ID;

    gapPara_dsPoolID = pConnectMDLInfo->dsPoolID;
    gapPara_dsDataOffset = pConnectMDLInfo->dsDataOffset;
    gapPara_maxTPDUSize = pConnectMDLInfo->maxTPDUSize;
    gapPara_maxTPDUdsCredits = pConnectMDLInfo->maxTPDUdsCredits;

    profileUpdateConnInfo(pConnectMDLInfo);// add by lilly.
    gapRole_state = GAP_PERIPHERAL_STATE_CONNECTED;
    
    gap_ConStateNotified = FALSE;  
    
}

/**
 * @fn          peripheral_Handle_DisconnectMDLRsp
 * @brief      process blueAPI_EventDisconnectMDLRsp message from bt stack
 * @param    pDisconnectMDLRsp  - pointer to TBlueAPI_DisconnectMDLRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_DisconnectMDLRsp( PBlueAPI_DisconnectMDLRsp pDisconnectMDLRsp )
{

}

/**
 * @fn          peripheral_Handle_DisconnectMDLInd
 * @brief      process blueAPI_EventDisconnectMDLInd message from bt stack
 * @param    pDisconnectMDLInd  - pointer to TBlueAPI_DisconnectMDLInd message
 *
 * @return     bool
 */
static void peripheral_Handle_DisconnectMDLInd(PBlueAPI_DisconnectMDLInd pDisconnectMDLInd )
{
    gapPara_DisconnectedReason = pDisconnectMDLInd->cause;
    blueAPI_DisconnectMDLConf( pDisconnectMDLInd->local_MDL_ID );
}

/**
 * @fn          peripheral_Handle_DeleteMDLInfo
 * @brief      process blueAPI_EventDeleteMDLInfo message from bt stack
 * @param    pDeleteMDLInfo  - pointer to TBlueAPI_DeleteMDLInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_DeleteMDLInfo( PBlueAPI_DeleteMDLInfo pDeleteMDLInfo )
{
#if 0
    if (gapPara_ConnHandle == pDeleteMDLInfo->local_MDL_ID)
    {
        gapRole_state = GAP_PERIPHERAL_STATE_STACK_READY;
        pGapDeviceStateChangeCb(gapRole_state);
    }
#endif
#ifdef ANCS_SUPPORT
    ancs_write_handle = 0;
#endif
}

/**
 * @fn          peripheral_Handle_InternalEventInfo
 * @brief      process blueAPI_EventInternalEventInfo message from bt stack
 * @param    pInternalEventInfo  - pointer to TBlueAPI_InternalEventInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_InternalEventInfo( PBlueAPI_InternalEventInfo pInternalEventInfo )
{

}

/**
 * @fn          peripheral_Handle_ACLStatusInfo
 * @brief      process blueAPI_EventACLStatusInfo message from bt stack
 * @param    pACLStatusInfo  - pointer to TBlueAPI_ACLStatusInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_ACLStatusInfo(PBlueAPI_ACLStatusInfo pACLStatusInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
               "<-- peripheral_Handle_ACLStatusInfo: status=%d",
               1,
               pACLStatusInfo->status);
#endif
    switch (pACLStatusInfo->status)
    {
    case blueAPI_ACLConnectedActive:
        break;
    case blueAPI_ACLConnectionDisconnected:
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
                   "<-- peripheral_Handle_ACLStatusInfo: remote addr: 0x%02x:%02x:%02x:%02x:%02x:%02x, addrtype:%d",
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
        gapRole_state = GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN;
        pGapDeviceStateChangeCb(gapRole_state);


        break;

    default:
        break;
    }


}

/**
 * @fn          peripheral_Handle_GATTMtuSizeInfo
 * @brief      process blueAPI_EventGATTMtuSizeInfo message from bt stack
 * @param    pMtuSizeInfo  - pointer to TBlueAPI_GATTMtuSizeInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_GATTMtuSizeInfo(PBlueAPI_GATTMtuSizeInfo  pMtuSizeInfo)
{
    if (pMtuSizeInfo->local_MDL_ID == gapPara_ConnHandle)
    {
        gapPara_mtuSize = pMtuSizeInfo->mtuSize;
        profileUpdateMtu(pMtuSizeInfo->mtuSize);
    }
}

/**
 * @fn          peripheral_Handle_LEConnectionUpdateRsp
 * @brief      process blueAPI_EventLEConnectionUpdateRsp message from bt stack
 * @param    pConnectionUpdateRsp  - pointer to TBlueAPI_LEConnectionUpdateRsp message
 *
 * @return     bool
 */
static void peripheral_Handle_LEConnectionUpdateRsp(PBlueAPI_LEConnectionUpdateRsp pConnectionUpdateRsp)
{
    if (pConnectionUpdateRsp->cause != blueAPI_CauseSuccess)
    {
        pGapRoles_ParamUpdateCB(gapPara_ConnHandle, GAP_CONN_PARAM_UPDATE_STATUS_FAIL);
    }
    else
    {
        pGapRoles_ParamUpdateCB(gapPara_ConnHandle, GAP_CONN_PARAM_UPDATE_STATUS_PENDING);
    }
}
/**
 * @fn          peripheral_Handle_LEConnectionUpdateInd
 * @brief      process blueAPI_EventLEConnectionUpdateInd message from bt stack
 * @param    pConnectionUpdateInd  - pointer to TBlueAPI_LEConnectionUpdateInd message
 *
 * @return     bool
 */
static void peripheral_Handle_LEConnectionUpdateInd(PBlueAPI_LEConnectionUpdateInd pConnectionUpdateInd)
{
    blueAPI_LEConnectionUpdateConf(gapPara_ConnHandle, blueAPI_CauseAccept);
}

/**
 * @fn          peripheral_Handle_LEConnectionParameterInfo
 * @brief      process blueAPI_EventLEConnectionParameterInfo message from bt stack
 * @param    pConnectionParameterInfo  - pointer to TBlueAPI_LEConnectionParameterInfo message
 *
 * @return     bool
 */
static void peripheral_Handle_LEConnectionParameterInfo(PBlueAPI_LEConnectionParameterInfo pConnectionParameterInfo)
{
    if (pConnectionParameterInfo->local_MDL_ID == gapPara_ConnHandle)
    {
        gapPara_ConnInterval = pConnectionParameterInfo->connInterval;
        gapPara_ConnSlaveLatency = pConnectionParameterInfo->connLatency;
        gapPara_ConnTimeout = pConnectionParameterInfo->supervisionTimeout;
        
        if(gap_ConStateNotified)
        {
            pGapRoles_ParamUpdateCB(gapPara_ConnHandle, GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS);
        }
        else
        {
            pGapDeviceStateChangeCb(gapRole_state);      
            gap_ConStateNotified = TRUE;
        }
    }
}
#ifdef ANCS_SUPPORT

extern uint16_t    ancs_cp_value_handle;
void peripheral_AncsDiscover(void)
{
    uint8_t ancs_uuid128[16] = {0xD0, 0x00, 0x2D,0x12,0x1E, 0x4B, 0x0F, 0xA4, 0x99, 0x4E, 0xCE, 0xB5,0x31, 0xF4 ,0x05 , 0x79};
#ifdef ANCS_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheral_AncsDiscover: gapPara_ConnHandle(0x%02x)", 1, gapPara_ConnHandle);    
#endif
    blueAPI_GATTDiscoveryReq(gapPara_ConnHandle, blueAPI_GATTDiscoveryServiceByUUID, 1, 0xFFFF, 0, ancs_uuid128);
}

bool peripheral_AncsGetNotificationAttributes(uint32_t NotificationUID, uint8_t *pAttributeIDs, uint8_t AttributeIDsLen)
{
    uint8_t * pBuffer = NULL;        
    uint8_t CommandID = CP_CommandIDGetNotificationAttributes;
    uint16_t wLength = sizeof(CommandID) + sizeof(NotificationUID) + AttributeIDsLen;
    uint16_t     wOffset = gProfileData.wDsDataOffset + 3;
    if(ancs_write_handle != 0)
        return false;
    
    if ( blueAPI_BufferGet(gProfileData.wDsPoolId, wLength,wOffset,(void **)&pBuffer) == blueAPI_CauseSuccess )
    {
        TBlueAPI_GATTWriteType   writeType = blueAPI_GATTWriteTypeRequest;
        
        memcpy( pBuffer + wOffset, &CommandID, sizeof(CommandID)  );
        memcpy( pBuffer + wOffset + sizeof(CommandID) , &NotificationUID, sizeof(NotificationUID));
        memcpy( pBuffer + wOffset + sizeof(CommandID) + sizeof(NotificationUID), pAttributeIDs, AttributeIDsLen);

        if (blueAPI_GATTAttributeWriteReq(
                                                                    pBuffer,
                                                                    gProfileData.local_MDL_ID,
                                                                    writeType,
                                                                    ancs_cp_value_handle,
                                                                    wLength,
                                                                    wOffset)
                                                                    )
        {
            ancs_write_handle = ancs_cp_value_handle;
            return true;
        }
        else
        {
            blueAPI_BufferRelease(pBuffer);
        }
    }

    return false;
}

bool peripheral_AncsGetAppAttributes(uint8_t *AppIdentifier, char *pAttributeIDs, uint8_t AttributeIDsLen)
{
    uint8_t * pBuffer = NULL;        
    uint8_t CommandID = CP_CommandIDGetAppAttributes;
    uint16_t wLength = sizeof(CommandID) + strlen((const char*)AppIdentifier) + AttributeIDsLen;
    uint16_t     wOffset = gProfileData.wDsDataOffset + 3;
    if(ancs_write_handle != 0)
        return false;    
    if ( blueAPI_BufferGet(gProfileData.wDsPoolId, wLength,wOffset,(void **)&pBuffer) == blueAPI_CauseSuccess )
    {
        TBlueAPI_GATTWriteType   writeType = blueAPI_GATTWriteTypeRequest;
        
        memcpy( pBuffer + wOffset, &CommandID, sizeof(CommandID)  );
        memcpy( pBuffer + wOffset + sizeof(CommandID) , AppIdentifier, strlen((const char*)AppIdentifier)+1);
        memcpy( pBuffer + wOffset + sizeof(CommandID) + strlen((const char*)AppIdentifier)+1, pAttributeIDs, AttributeIDsLen);

        if (blueAPI_GATTAttributeWriteReq(
                                                                    pBuffer,
                                                                    gProfileData.local_MDL_ID,
                                                                    writeType,
                                                                    ancs_cp_value_handle,
                                                                    wLength,
                                                                    wOffset)
                                                                    )
        {
            ancs_write_handle = ancs_cp_value_handle;
            return true;
        }
        else
        {
            blueAPI_BufferRelease(pBuffer);
        }
    }

    return false;
}

bool peripheral_AncsPerformNotificationAction(uint32_t NotificationUID, uint8_t ActionID)
{
    uint8_t * pBuffer = NULL;        
    uint8_t CommandID = CP_CommandIDPerformNotificationAction;
    uint16_t wLength = sizeof(CommandID) + sizeof(NotificationUID) + sizeof(ActionID);
    uint16_t     wOffset = gProfileData.wDsDataOffset + 3;
    if(ancs_write_handle != 0)
        return false;      
    if ( blueAPI_BufferGet(gProfileData.wDsPoolId, wLength,wOffset,(void **)&pBuffer) == blueAPI_CauseSuccess )
    {
        TBlueAPI_GATTWriteType   writeType = blueAPI_GATTWriteTypeRequest;
        
        memcpy( pBuffer + wOffset, &CommandID, sizeof(CommandID)  );
        memcpy( pBuffer + wOffset + sizeof(CommandID) , &NotificationUID, sizeof(NotificationUID));
        memcpy( pBuffer + wOffset + sizeof(CommandID) + sizeof(NotificationUID), &ActionID, sizeof(ActionID));

        if (blueAPI_GATTAttributeWriteReq(
                                                                    pBuffer,
                                                                    gProfileData.local_MDL_ID,
                                                                    writeType,
                                                                    ancs_cp_value_handle,
                                                                    wLength,
                                                                    wOffset)
                                                                    )
        {
            ancs_write_handle = ancs_cp_value_handle;
            return true;
        }
        else
        {
            blueAPI_BufferRelease(pBuffer);
        }
    }
        return false;
}

extern uint16_t    ancs_ns_value_handle;
extern uint16_t    ancs_ds_value_handle;
extern bool ancs_enableNotification(uint16_t cccHandle, uint16_t cccValue);
bool peripheral_AncsSetNotificationSourceNotify(bool notify)
{
    uint16_t cccValue;
    if(notify)
    {
        cccValue = 0x0001;
    }
    else
    {
        cccValue = 0;
    }
    return ancs_enableNotification(ancs_ns_value_handle + 1,cccValue);
    
}
bool peripheral_AncsSetDataSourceNotify(bool notify)
{
    uint16_t cccValue;
    if(notify)
    {
        cccValue = 0x0001;
    }
    else
    {
        cccValue = 0;
    }
    return ancs_enableNotification(ancs_ds_value_handle + 1,cccValue);
}
#endif
/**
 * @fn          peripheral_ProcessGapEvent
 * @brief      process TBlueAPI_UsMessage message from bt stack, only gap 
 *                related messages are handled here
 * @param    pMsg  - pointer to TBlueAPI_UsMessage message
 *
 * @return     bool
 */
bool peripheral_ProcessGapEvent( PBlueAPI_UsMessage pMsg )
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
        peripheral_Handle_DeviceConfigSetRsp( &pMsg->p.DeviceConfigSetRsp);
        break;

    case blueAPI_EventSetRandomAddressRsp:
        peripheral_Handle_SetRandomAddressRsp( &pMsg->p.SetRandomAddressRsp);
        break;

    case blueAPI_EventLEAdvertiseRsp:
        peripheral_Handle_LEAdvertiseRsp( &pMsg->p.LEAdvertiseRsp);
        break;

    case blueAPI_EventLEAdvertiseParameterSetRsp:
        peripheral_Handle_LEAdvertiseParameterRsp( &pMsg->p.LEAdvertiseParameterSetRsp);
        break;

    case blueAPI_EventLEAdvertiseDataSetRsp:
        peripheral_Handle_LEAdvertiseDataSetRsp( &pMsg->p.LEAdvertiseDataSetRsp);
        break;

    case blueAPI_EventLEModifyWhitelistRsp:
        peripheral_Handle_LEModifyWhitelistRsp(&pMsg->p.LEModifyWhitelistRsp);
        break;

    //for establish connection
    case blueAPI_EventCreateMDLInd:
        peripheral_Handle_CreateMDLInd(&pMsg->p.CreateMDLInd );
        break;

    case blueAPI_EventConnectMDLRsp:
        peripheral_Handle_ConnectMDLRsp(&pMsg->p.ConnectMDLRsp );
        break;

    case blueAPI_EventMCLStatusInfo:
        peripheral_Handle_MCLStatusInfo(&pMsg->p.MCLStatusInfo );
        break;

    case blueAPI_EventConnectMDLInfo:
        peripheral_Handle_ConnectMDLInfo(&pMsg->p.ConnectMDLInfo );
        break;

    case blueAPI_EventDisconnectMDLRsp:
        peripheral_Handle_DisconnectMDLRsp(&pMsg->p.DisconnectMDLRsp );
        break;

    case blueAPI_EventDisconnectMDLInd:
        peripheral_Handle_DisconnectMDLInd(&pMsg->p.DisconnectMDLInd );
        break;

    case blueAPI_EventDeleteMDLInfo:
        peripheral_Handle_DeleteMDLInfo(&pMsg->p.DeleteMDLInfo );
        break;

    case blueAPI_EventACLStatusInfo:
        peripheral_Handle_ACLStatusInfo(&pMsg->p.ACLStatusInfo );
        break;

    case blueAPI_EventGATTMtuSizeInfo:
        peripheral_Handle_GATTMtuSizeInfo(&pMsg->p.GATTMtuSizeInfo);
        break;

    //for connection parameter update
    case blueAPI_EventLEConnectionUpdateRsp:
        peripheral_Handle_LEConnectionUpdateRsp(&pMsg->p.LEConnectionUpdateRsp);
        break;
    case blueAPI_EventLEConnectionUpdateInd:
        peripheral_Handle_LEConnectionUpdateInd(&pMsg->p.LEConnectionUpdateInd);
        break;
    case blueAPI_EventLEConnectionParameterInfo:
        peripheral_Handle_LEConnectionParameterInfo(&pMsg->p.LEConnectionParameterInfo);
        break;

    //for internal debug use only
    case blueAPI_EventInternalEventInfo:
        peripheral_Handle_InternalEventInfo(&pMsg->p.InternalEventInfo );
        break;

    }
    return true;
}

/**
 * @fn          peripheral_HandleBlueAPIMessage
 * @brief      process TBlueAPI_UsMessage message from bt stack
 * @param    pMsg  - pointer to TBlueAPI_UsMessage message
 *
 * @return     bool
 */
bool peripheral_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg )
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
        peripheral_ProcessGapEvent(pMsg);
    }
    
    GAPBondMgr_ProcessEvent(pMsg);
    bufferReleased = profile_HandleMessage(pMsg);

    if ( !bufferReleased && pMsg)
    {
        blueAPI_BufferRelease(pMsg);
        pMsg = NULL;
    }

    return true;
}

/**
 * @fn         peripheral_SendUpdateParam
 * @brief      Send connection parameter request msg to bt stack.
 *
 *             App can call this api to modify connection interval, slave latency and supervision timeout value.
 *
 * @return     Send result
 * @retval     gapAPI_CauseSuccess Send request success
 * @retval     gapAPI_NotConnected Failed because of invalid state
 */
TGAP_STATUS peripheral_SendUpdateParam(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    // If there is no existing connection no update need be sent
    if ( gapRole_state != GAP_PERIPHERAL_STATE_CONNECTED &&  gapRole_state != GAP_PERIPHERAL_STATE_CONNECTED_ADV)
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
 * @fn          peripheral_Init_StartAdvertising
 * @brief      Start advertising if default advertising is enable
 *
 *                This API will cause 4 APIs to be called
 *                blueAPI_LEAdvertiseParameterSetReq
 *                blueAPI_LEAdvertiseDataSetReq - adv data
 *                blueAPI_LEAdvertiseDataSetReq - scan response data
 *                blueAPI_LEAdvertiseReq
 *
 * @return     Send result
 * @retval     gapAPI_CauseSuccess Send request success
 */
TGAP_STATUS peripheral_Init_StartAdvertising(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if (gapPara_AdvEnableDefault == TRUE)
    {
        if (gap_AdvUseWL == TRUE)
        {
            blueAPI_LEModifyWhitelistReq(blueAPI_LEWhitelistOpAdd,
                                         gapPara_WhiteListDevAddr,
                                         (TBlueAPI_RemoteBDType)gapPara_WhiteListDevAddrType);
        }
        else
        {
            blueAPI_LEAdvertiseParameterSetReq((TBlueAPI_LEAdvType)gapPara_AdvEventType,
                                               (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterScanPolicy,
                                               (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterConnectPolicy,
                                               gapPara_AdvMinInterval,
                                               gapPara_AdvMaxInterval,
                                               (TBlueAPI_LocalBDType)bdAddrType,
                                               gapRole_AdvDirectAddr,
                                               (TBlueAPI_RemoteBDType)gapPara_AdvDirectAddrType
                                              );


        }
    }

    return ret;
}

/**
 * @fn          peripheral_Handle_ServiceRegisterCompleteEvt
 * @brief      process service register complete message from bt profile
 *
 * @return     void
 */
void peripheral_Handle_ServiceRegisterCompleteEvt(void)
{
    gapRole_state = GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN;
    pGapDeviceStateChangeCb(gapRole_state);
}

/**
 * @fn          peripheral_StartAdvertising
 * @brief      Start advertising
 *
 *                This API will cause 4 APIs to be called
 *                blueAPI_LEAdvertiseParameterSetReq
 *                blueAPI_LEAdvertiseDataSetReq - adv data
 *                blueAPI_LEAdvertiseDataSetReq - scan response data
 *                blueAPI_LEAdvertiseReq
 * @return     Send result
 * @retval     gapAPI_CauseSuccess Send request success
 * @retval     gapAPI_IncorrectMode Failed because of invalid state
 */
TGAP_STATUS peripheral_StartAdvertising(void)
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;
    uint8_t bdAddrType;
#if 1   //add for gap status switch err.
    if(gapRole_state == GAP_PERIPHERAL_STATE_CONNECTED)
    {
            //DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "cannot_StartAdvertising\n\n", 0);
        return gapAPI_IncorrectMode;
    }
#endif    
    GAP_GetParameter(GAPPARA_BD_ADDR_TYPE, &bdAddrType);
    if (gap_AdvUseWL == TRUE)
    {
        blueAPI_LEModifyWhitelistReq(blueAPI_LEWhitelistOpAdd,
                                     gapPara_WhiteListDevAddr,
                                     (TBlueAPI_RemoteBDType)gapPara_WhiteListDevAddrType);
    }
    else
    {
        blueAPI_LEAdvertiseParameterSetReq((TBlueAPI_LEAdvType)gapPara_AdvEventType,
                                            (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterScanPolicy,
                                            (TBlueAPI_LEFilterPolicy)gapParaInternal_AdvFilterConnectPolicy,
                                            gapPara_AdvMinInterval,
                                            gapPara_AdvMaxInterval,
                                            (TBlueAPI_LocalBDType)bdAddrType,
                                            gapRole_AdvDirectAddr,
                                            (TBlueAPI_RemoteBDType)gapPara_AdvDirectAddrType
                                            );
    }

    return ret;
}

/**
 * @fn          peripheral_StopAdvertising
 * @brief      Stop advertising
 *
 * @return     Send result
 * @retval     gapAPI_CauseSuccess Send request success
 */
TGAP_STATUS peripheral_StopAdvertising(void)
{
    blueAPI_LEAdvertiseReq(blueAPI_LEAdvModeDisabled);
    return gapAPI_CauseSuccess;
}

/**
 * @fn          peripheral_Disconnect
 * @brief      Terminates the existing connection.
 *
 * @return     Send result
 * @retval     gapAPI_CauseSuccess Send request success
 * @retval     gapAPI_IncorrectMode Failed, invalid state
 */
TGAP_STATUS peripheral_Disconnect( void )
{
    uint16_t conn_interval;
    if ( gapRole_state == GAP_PERIPHERAL_STATE_CONNECTED )
    {
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheral_Disconnect from App", 0);
#endif
        /* Delay 2 * conn_interval * 1.25 ms, avoid buffer acl data broke by disconnection. */
        peripheralGetGapParameter(GAPPRRA_CONN_INTERVAL, &conn_interval);
        vTaskDelay((conn_interval * 2.5) / portTICK_RATE_MS);
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheral_Disconnect after 2 interval delay", 0);
#endif
        blueAPI_DisconnectMDLReq(gapPara_ConnHandle, blueAPI_CauseConnectionDisconnect);
    }
    else
    {
        return ( gapAPI_IncorrectMode );
    }
    return gapAPI_CauseSuccess;
}

/**
* @brief   Set the TX Power of BLE radio
*
* @param[in]   tx_power_index - valid tx power index: @ref BLE_TX_POWER_INDEX
*
* @return  Send result
* @retval  gapAPI_CauseSuccess: Set request sent success.
* @retval  gapAPI_ErrorUnknown: Set request sent fail.
*/
TGAP_STATUS peripheral_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_SetBleTxPowerReq(tx_power_index))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "peripheral_SetTxPower fail !!!", 0);
    }

    return ret;
}

/**
* @brief   Read rssi value of the connection
*
* @return  Send result
* @retval  gapAPI_CauseSuccess: Set request sent success.
* @retval  gapAPI_ErrorUnknown: Set request sent fail.
*/
TGAP_STATUS peripheral_ReadRSSI( void )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    if(!blueAPI_ReadRSSI(gapPara_ConnHandle))
    {
        ret = gapAPI_ErrorUnknown;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "peripheral_ReadRSSI fail !!!", 0);
    }

    return ret;
}
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
/**
 * @brief      Get random address type
 *
 * @param[in]  random_addr  - random address
 *
 * @return  random address type
 */
TRANDOM_ADDR_TYPE peripheral_CheckRandomAddressType(uint8_t* random_addr)
{
    uint8_t significant_bits = random_addr[5] & GAPBOND_PRIVACY_RANDBD_MASK;
    if(significant_bits == GAPBOND_PRIVACY_RANDBD_STATIC)
    {
        return RANDOM_ADDR_TYPE_STATIC;
    }
    else if(significant_bits == GAPBOND_PRIVACY_RANDBD_RESOLVABLE)
    {
        return RANDOM_ADDR_TYPE_RESOLVABLE;
    }
    else if(significant_bits == GAPBOND_PRIVACY_RANDBD_NON_RESOLVABLE)
    {
        return RANDOM_ADDR_TYPE_NON_RESOLVABLE;
    }
    else
    {
        return RANDOM_ADDR_TYPE_UNKNOWN;
    }
}

/**
 * @brief      Get bonded device from flash storage
 *
 *             If bonded device exists, 
 *             return true, and resolved address information will be returned by resolved_addr.
 *
 * @param[in]   random_addr  - random address
 * @param[out]  resolved_addr  - the resolved address
 *
 * @return Send result.
 * @retval true successfully
 * @retval false failed
 */
bool peripheral_ResolveRandomAddress(uint8_t* random_addr, uint8_t* resolved_addr)
{
    bool cause = false;
    if(peripheral_CheckRandomAddressType(random_addr) == RANDOM_ADDR_TYPE_RESOLVABLE)
    {
        remote_BD_struct s_remote_BD;
        uint32_t errorno = 0;
        errorno = fs_load_remote_BD_struct(&s_remote_BD);
        if(errorno == 0)
        {
            if(s_remote_BD.remote_bd_type == PEER_ADDRTYPE_PRIVATE_IDENTITY_ADDRESS)
            {
                if(gapbondmgr_CheckRandomAddress(random_addr, blueAPI_RemoteBDTypeLERandom, &s_remote_BD))
                {
                    memcpy(resolved_addr, s_remote_BD.addr, B_ADDR_LEN);
                    cause = true;
                }
            }
        }
    }
    return cause;
}
#endif
/*********************************************************************
*********************************************************************/
