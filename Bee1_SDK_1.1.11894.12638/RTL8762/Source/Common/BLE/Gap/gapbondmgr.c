enum { __FILE_NUM__ = 0 };

/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gapbondmgr.c
  * @brief   This file provides all the GAP bond and pairing related functions.
  * @details
  * @author  ranhui
  * @date    11-5-2015
  * @version v1.0
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include "rtl_types.h"
#include "blueapi.h"
#include "gap.h"
#include <string.h>
#include "gapbondmgr.h"
#include "trace.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "rtl876x_flash_storage.h"

// GAPBonding Parameters
static uint8_t gapBond_PairingMode = GAPBOND_PAIRING_MODE_PAIRABLE;
static uint8_t gapBond_MITM = GAPBOND_AUTH_NO_MITM_YES_BOND;
static uint8_t gapBond_SecReqEnable = FALSE;
static uint8_t gapBond_SecReqRequirements = GAPBOND_SEC_REQ_NO_MITM;
static uint8_t gapBond_IOCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
static uint8_t gapBond_OOBDataFlag = FALSE;
static uint8_t gapBond_OOBData[KEYLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t gapBond_FixedPasskeyEnable = FALSE;
static uint32_t gapBond_Passcode = 0;
static uint16_t gapBond_ConnHandle = 0;
static uint8_t  gapBond_ConnectedDevAddr[B_ADDR_LEN] = {0};
static const gapBondCBs_t *pGapBondCB = NULL;
#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
uint8_t  gapBond_KeyDistribution = GAPBOND_KEY_DEFAULT;
#endif

#ifdef BT_GAP_LE_PRIVACY_RESOLVING
#define LE_ENCRYPT_DATA_LENGTH       0x10
typedef struct
{
    uint8_t used;
    uint8_t new_update;
    uint8_t unresolved_addr[B_ADDR_LEN];
    uint8_t unresolved_addr_type;
    uint8_t resolved_addr[B_ADDR_LEN];
    uint8_t resolved_addr_type;
}TGapBondPrivateAddr;

static TGapBondPrivateAddr gapBond_PrivateAddr;
aes_context *gapBond_AesCtxDefault = NULL;
extern void *pvPortMalloc( size_t xSize,RAM_TYPE ramType ) ;
#endif

/**
 * @fn          GAPBondMgr_ParaInit
 * @brief      Initialize parameters of gap bond manager.            
 *
 * @return     void
 */
void GAPBondMgr_ParaInit(void)
{
    gapBond_PairingMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    gapBond_MITM = GAPBOND_AUTH_NO_MITM_YES_BOND;
    gapBond_IOCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    gapBond_OOBDataFlag = FALSE;
    gapBond_FixedPasskeyEnable = FALSE;
    gapBond_Passcode = 0;
    gapBond_ConnHandle = 0;
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
    memset(&gapBond_PrivateAddr, 0, sizeof(gapBond_PrivateAddr));
    if( gapBond_AesCtxDefault == NULL )
    {
        gapBond_AesCtxDefault = pvPortMalloc(sizeof(aes_context), RAM_TYPE_DATA_ON);
    }
#endif
}

/**
 * @brief           Set a GAP Bond Manager parameter.
 *
 *                      NOTE: You can call this function with a GAP Bond Parameter ID and it will set the
 *                      GAP Bond Parameter.  GAP Bond Parameters are defined in (gapbondmanager.h).  If
 *                      the "len" field sets to the size of a "uint16_t" ,the
 *                      "pValue" field must point to a "uint16".
 *
 * @param[in]         param - Profile parameter ID: @ref GAPBOND_MANAGER_PARAMETERS
 * @param[in]         len - length of data to write
 * @param[in]         pValue - pointer to data to write.  This is dependent on
 *                      the parameter ID and WILL be cast to the appropriate
 *                      data type (example: data type of uint16 will be cast to
 *                      uint16 pointer).
 * 
 * @retval   gapAPI_CauseSuccess Success
 * @retval   gapAPI_InvalidRange Invalid length or Invalid param id
 */
TGAP_STATUS GAPBondMgr_SetParameter( uint16_t param, uint8_t len, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;  // return value

    switch ( param )
    {
    case GAPBOND_PAIRING_MODE:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAPBOND_PAIRING_MODE_PAIRABLE) )
        {
            gapBond_PairingMode = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_MITM_PROTECTION:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAPBOND_AUTH_YES_MITM_YES_BOND) )
        {
            gapBond_MITM = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_IO_CAPABILITIES:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAPBOND_IO_CAP_KEYBOARD_DISPLAY) )
        {
            gapBond_IOCap = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_OOB_ENABLED:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= TRUE) )
        {
            gapBond_OOBDataFlag = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_OOB_DATA:
        if ( len == KEYLEN )
        {
            memcpy( gapBond_OOBData, pValue, KEYLEN ) ;
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_PASSKEY:
        if ( (len == sizeof ( uint32_t ))
                && (*((uint32_t*)pValue) <= GAP_PASSCODE_MAX) )
        {
            gapBond_Passcode = *((uint32_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_FIXED_PASSKEY_ENABLE:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= TRUE) )
        {
            gapBond_FixedPasskeyEnable = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;

    case GAPBOND_SEC_REQ_ENABLE:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= TRUE) )
        {
            gapBond_SecReqEnable = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;
            
    case GAPBOND_SEC_REQ_REQUIREMENT:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAPBOND_SEC_REQ_YES_MITM) )
        {
            gapBond_SecReqRequirements = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;
#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
    case GAPBOND_KEY_DISTRIBUTION:
        if ( (len == sizeof ( uint8_t )) && (*((uint8_t*)pValue) <= GAPBOND_KEY_ALL) )
        {
            gapBond_KeyDistribution = *((uint8_t*)pValue);
        }
        else
        {
            ret = gapAPI_InvalidRange;
        }
        break;
#endif
    default:
        ret = gapAPI_InvalidRange;
        break;
    }

    return ( ret );
}

/**
 * @brief           Get a GAP Bond Manager Parameter.
 *
 *                      NOTE: You can call this function with a GAP Bond Manager Parameter ID and it will get a
 *                      GAP Bond Manager Parameter.  GAP Bond Manager  Parameters are defined in (gapbondmanager.h). 
 *
 * @param[in]         param - Profile parameter ID: @ref GAPBOND_MANAGER_PARAMETERS
 * @param[out]         pValue - pointer to location to get the value.  This is dependent on
 *                      the parameter ID and WILL be cast to the appropriate
 *                      data type (example: data type of uint16 will be cast to
 *                      uint16 pointer).
 *
 * @retval   gapAPI_CauseSuccess Success
 * @retval   gapAPI_InvalidRange Invalid param id
 */

TGAP_STATUS GAPBondMgr_GetParameter( uint16_t param, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;  // return value

    switch ( param )
    {
    case GAPBOND_PAIRING_MODE:
        *((uint8_t*)pValue) = gapBond_PairingMode;
        break;

    case GAPBOND_MITM_PROTECTION:
        *((uint8_t*)pValue) = gapBond_MITM;
        break;

    case GAPBOND_IO_CAPABILITIES:
        *((uint8_t*)pValue) = gapBond_IOCap;
        break;

    case GAPBOND_OOB_ENABLED:
        *((uint8_t*)pValue) = gapBond_OOBDataFlag;
        break;

    case GAPBOND_OOB_DATA:
        memcpy( pValue, gapBond_OOBData, KEYLEN ) ;
        break;

    case GAPBOND_PASSKEY:
        *((uint32_t*)pValue) = gapBond_Passcode;
        break;

    case GAPBOND_FIXED_PASSKEY_ENABLE:
        *((uint32_t*)pValue) = gapBond_FixedPasskeyEnable;
        break;
#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
    case GAPBOND_KEY_DISTRIBUTION:
        *((uint8_t*)pValue) = gapBond_KeyDistribution;
        break;
#endif
    default:
        ret = gapAPI_InvalidRange;        
        break;
    }

    return ( ret );
}

/**
 * @fn          GAPBondMgr_Register
 * @brief      Register gap bond callbacks.       
 *
 * @param    pCB  - pointer to gap bond callbacks 
 * @return     void
 */
void GAPBondMgr_Register( gapBondCBs_t *pCB )
{
    pGapBondCB = pCB;
}

/**
 * @fn          GAPBondMgr_SetPairable
 * @brief      Set Pairable mode of device.       
 *
 * @retval     void
 */
void GAPBondMgr_SetPairable(void)
{
    blueAPI_PairableModeSetReq(gapBond_PairingMode, (TBlueAPI_AuthRequirements)gapBond_MITM, (TBlueAPI_IOCapabilities)gapBond_IOCap, gapBond_OOBDataFlag);
}

/**
 * @fn          GAPBondMgr_Pair
 * @brief      Start pairing.       
 *
 * @retval     void
 */
void GAPBondMgr_Pair(void)
{
    blueAPI_GATTSecurityReq(gapBond_ConnHandle, gapBond_MITM, 7);
}

/**
 * @fn          GAPBondMgr_InputPassKey
 * @brief      Send passkey to gap bond manager when pairing with passkey entry,
 *                and local should input passkey.       
 *
 * @retval     void
 */
void GAPBondMgr_InputPassKey(void)
{
    blueAPI_UserPasskeyReqReplyReq(gapBond_ConnectedDevAddr, gapBond_Passcode, blueAPI_CauseAccept);
}

/**
 * @fn          GAPBondMgr_InputOobData
 * @brief      Send oob data to gap bond manager when pairing with out of bond,
 *                and local should input oob data.       
 *
 * @retval     void
 */
void GAPBondMgr_InputOobData(void)
{
    blueAPI_RemoteOOBDataReqConf(gapBond_ConnectedDevAddr, gapBond_OOBData, blueAPI_CauseAccept);
}

/**
 * @brief      Get bonded device from flash storage
 *
 *                If bonded device exists, 
 *                return true, and device information will be returned by pBondedDevice  
 *
 * Notes: If remote_bd_type = PEER_ADDRTYPE_PRIVATE_IDENTITY_ADDRESS, the peer device uses 
 *        the resolved private address. The addr[6] is identity address and not the real 
 *        address of the peer device.
 *
 * @param[out]    pBondedDevice  - pointer to bonded device information
 *
 * @retval TRUE Get success
 * @retval FALSE Get failed
 */
bool GAPBondMgr_GetBondedDevice(remote_BD_struct *pBondedDevice)
{
    bool bRet= false;
    int remote_BD_equal;
    remote_BD_struct remotebd;
    remote_BD_struct invalid_remotebd;
    uint32_t result;
    memset(&invalid_remotebd, 0xFF, sizeof(invalid_remotebd));

    result = fs_load_remote_BD_struct(&remotebd);
    if(result != 0)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAPBondMgr_GetBondedDevice No Bond Device, result(%d)",1, result);
        return false;
    }
    
    remote_BD_equal = memcmp(&remotebd, &invalid_remotebd, sizeof(invalid_remotebd));

    if (remote_BD_equal != 0)
    {
        memcpy(pBondedDevice, &remotebd, sizeof(remotebd));
        bRet=  true;
    }
    else
    {
        bRet= false;
    }
    
    return bRet;
}

/**
 * @fn          GAPBondMgr_EraseAllBondings
 * @brief      Erase bonding device inforamtion from falsh storage
 *
 * @retval     void
 */
void GAPBondMgr_EraseAllBondings(void)
{
    remote_BD_struct remotebd;
    LTK_struct ltk;
    remLTK_struct remltk;
    IRK_struct       irk;
    cccData_struct cccData;

    memset(&remotebd, 0xFF, sizeof(remotebd));
    ltk.linkKeyLength = 0;
    remltk.linkKeyLength = 0;
    irk.linkKeyLength = 0;
    cccData.data_length = 0;

    fs_save_remote_BD_struct(&remotebd);
    fs_save_LTK_struct(&ltk);
    fs_save_remLTK_struct(&remltk);
    fs_save_IRK_struct(&irk);
    fs_save_cccData_struct(&cccData);

}

/**
 * @fn          gapbondmgr_Handle_ConnectMDLInfo
 * @brief      process blueAPI_EventConnectMDLInfo message from bt stack
 * @param    pConnectMDLInfo  - pointer to TBlueAPI_ConnectMDLInfo message
 *
 * @return     void
 */
void gapbondmgr_Handle_ConnectMDLInfo(PBlueAPI_ConnectMDLInfo pConnectMDLInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gapbondmgr_Handle_ConnectMDLInfo: MDL_ID=0x%x,dsPoolID=0x%x,dsDataOffset=0x%x,MTU=0x%x,Credits=0x%x.",
                5, pConnectMDLInfo->local_MDL_ID,
                   pConnectMDLInfo->dsPoolID,
                   pConnectMDLInfo->dsDataOffset,
                   pConnectMDLInfo->maxTPDUSize,
                   pConnectMDLInfo->maxTPDUdsCredits);
#endif
    gapBond_ConnHandle = pConnectMDLInfo->local_MDL_ID;
    /* Send Security Request. Mostly sent by slave. */
    if(TRUE == gapBond_SecReqEnable)
    {
        blueAPI_GATTSecurityReq(gapBond_ConnHandle, gapBond_SecReqRequirements, 7);
    }
}

/**
 * @fn          gapbondmgr_Handle_ConnectMDLInfo
 * @brief      process blueAPI_EventACLStatusInfo message from bt stack
 * @param    pACLStatusInfo  - pointer to TBlueAPI_ACLStatusInfo message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_ACLStatusInfo(PBlueAPI_ACLStatusInfo pACLStatusInfo)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_Handle_ACLStatusInfo: status=%d",
               1, pACLStatusInfo->status);
#endif
    switch (pACLStatusInfo->status)
    {
    case blueAPI_ACLConnectedActive:
        {
            memcpy(gapBond_ConnectedDevAddr, pACLStatusInfo->remote_BD, B_ADDR_LEN);
        }
        break;

    case blueAPI_ACLAuthenticationStarted:
        {
            pGapBondCB->PairStateCB(gapBond_ConnHandle, GAPBOND_PAIRING_STATE_STARTED, 0);
        }
        break;
    case blueAPI_ACLAuthenticationFailure:
        {
            pGapBondCB->PairStateCB(gapBond_ConnHandle, GAPBOND_PAIRING_STATE_COMPLETE, 1);
        }
        break;

    case blueAPI_ACLAuthenticationSuccess:
        {
            pGapBondCB->PairStateCB(gapBond_ConnHandle, GAPBOND_PAIRING_STATE_COMPLETE, 0);
        }

        break;

    case blueAPI_ACLConnectionEncrypted:
        {
            pGapBondCB->EncryptStateCB(gapBond_ConnHandle, GAPBOND_ENCRYPT_STATE_ENABLED);
        }
        break;
    case blueAPI_ACLConnectionNotEncrypted:
        {
            pGapBondCB->EncryptStateCB(gapBond_ConnHandle, GAPBOND_ENCRYPT_STATE_DISABLED);
        }
        break;
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
    case blueAPI_ACLAddressResolved:
        {
            if(gapPara_PrivacyMode != GAP_PRIVACY_MODE_DISABLE)
            {
                uint16_t null_bd[6] ={0,0,0,0,0,0}; 
                if(memcmp(pACLStatusInfo->p.resolve.remote_BD, null_bd, 6)==0)
                {
                    DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR,"gapbondmgr_Handle_ACLStatusInfo: bd all zeros", 0);
                    break;
                }
                if(memcmp(pACLStatusInfo->remote_BD, pACLStatusInfo->p.resolve.remote_BD, 6))
                {
                    memcpy(gapBond_PrivateAddr.unresolved_addr, pACLStatusInfo->remote_BD, B_ADDR_LEN);
                    gapBond_PrivateAddr.unresolved_addr_type = pACLStatusInfo->remote_BD_type;
                    memcpy(gapBond_PrivateAddr.resolved_addr, pACLStatusInfo->p.resolve.remote_BD, B_ADDR_LEN);
                    gapBond_PrivateAddr.resolved_addr_type = pACLStatusInfo->p.resolve.remote_BD_type;
                    gapBond_PrivateAddr.used = true;
                    gapBond_PrivateAddr.new_update = true;
//#ifdef GAP_DEBUG_FLAG
                    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_Handle_ACLStatusInfo: blueAPI_ACLAddressResolved receive resolved addr", 0);
//#endif
                }
            }
        }
        break;
#endif
    default:
        break;
    }
    return ( true );
}

#ifdef BT_GAP_LE_PRIVACY_RESOLVING
bool gapbondmgr_CheckRandomAddress(uint8_t * unresolved_addr, uint8_t unresolved_addr_type, remote_BD_struct* pAddr)
{
    IRK_struct s_tmp;
    uint32_t errorno = 0;
    uint8_t buffer[LE_ENCRYPT_DATA_LENGTH];
    uint8_t irk[LE_ENCRYPT_DATA_LENGTH];
    uint8_t encrypt_buffer[LE_ENCRYPT_DATA_LENGTH];
    uint8_t i;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_CheckRandomAddress", 0);
#endif    
    if(gapBond_PrivateAddr.used)
    {
        if(!memcmp(unresolved_addr, gapBond_PrivateAddr.unresolved_addr,6))
        {
            if((memcmp(gapBond_PrivateAddr.resolved_addr, pAddr->addr, 6)== 0)
                &&(gapBond_PrivateAddr.resolved_addr_type == pAddr->remote_bd_type))
            {            
                return true;
            }
            else
            {
//#ifdef GAP_DEBUG_FLAG
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR,"!!gapBond_PrivateAddr not match", 0);
//#endif
                return false;
            }
        }
    }
    errorno = fs_load_IRK_struct((IRK_struct*)&s_tmp);
    if (errorno != 0 || s_tmp.linkKeyLength == 0)
    {
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"!!gapbondmgr_CheckRandomAddress: no IRK", 0);
#endif
        return false;
    }

    memset(buffer, 0x00, LE_ENCRYPT_DATA_LENGTH);
    buffer[13] = unresolved_addr[5];
    buffer[14] = unresolved_addr[4];
    buffer[15] = unresolved_addr[3];
    for(i = 0; i< LE_ENCRYPT_DATA_LENGTH; i++)
    {
        irk[i] = s_tmp.linkKey[LE_ENCRYPT_DATA_LENGTH -1 - i];
    }

    if(!blueAPI_AES_SetKey(gapBond_AesCtxDefault, irk, 128))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR,"gapbondmgr_CheckRandomAddress: blueAPI_AES_SetKey failed",0);
        return false;
    }

    if(blueAPI_AES_Encrypt(gapBond_AesCtxDefault, buffer, encrypt_buffer))
    {
        if ((encrypt_buffer[13] == unresolved_addr[2])
            &&(encrypt_buffer[14] == unresolved_addr[1])
            &&(encrypt_buffer[15] == unresolved_addr[0]))
        {
#ifdef GAP_DEBUG_FLAG            
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_CheckRandomAddress: match IRK",0);
#endif            
            if(pAddr->remote_bd_type == blueAPI_RemoteBDTypeLERandom)
            {
                return true;
            }
            memcpy(gapBond_PrivateAddr.unresolved_addr, unresolved_addr, B_ADDR_LEN);
            gapBond_PrivateAddr.unresolved_addr_type = unresolved_addr_type;
            memcpy(gapBond_PrivateAddr.resolved_addr, pAddr->addr, B_ADDR_LEN);
            gapBond_PrivateAddr.resolved_addr_type = pAddr->remote_bd_type;
            gapBond_PrivateAddr.used = true;
            gapBond_PrivateAddr.new_update = false;
            return true;
        }
        else
        {
//#ifdef GAP_DEBUG_FLAG            
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR,"gapbondmgr_CheckRandomAddress: not match",0);
//#endif 
        }
    }
    return false;
}

static bool gapbondmgr_CheckAddress(uint8_t * addr, uint8_t addr_type)
{
    uint8_t remote_BD_equal = 1;
    uint32_t errorno = 0;
    remote_BD_struct s_remote_BD;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_CheckAddress", 0);
#endif
    errorno = fs_load_remote_BD_struct(&s_remote_BD);
    if(errorno != 0)
    {
        return false;    
    }
    remote_BD_equal = memcmp(addr, s_remote_BD.addr, 6);
    if((remote_BD_equal == 0) && (s_remote_BD.remote_bd_type == blueAPI_RemoteBDTypeLEResolved))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"!!gapbondmgr_CheckAddress: address equal", 0);
    }
    else
    {
        if(s_remote_BD.remote_bd_type != addr_type)
        {
            remote_BD_equal = 1;
        }
    }
    if(gapPara_PrivacyMode != GAP_PRIVACY_MODE_DISABLE)
    {
        if((remote_BD_equal != 0)
           &&(addr_type == blueAPI_RemoteBDTypeLERandom)
           &&((addr[5] & GAPBOND_PRIVACY_RANDBD_MASK) == GAPBOND_PRIVACY_RANDBD_RESOLVABLE))
        {
            if(gapbondmgr_CheckRandomAddress(addr, addr_type, &s_remote_BD))
            {
                remote_BD_equal = 0;
            }
        }
    }

    if(remote_BD_equal)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR,"gapbondmgr_CheckAddress: not equal", 0);
        return false;
    }
    else
    {
        return true;
    }
}

static void gapbondmgr_UpdateBondingAddress(uint8_t * unresolved_addr, uint8_t unresolved_addr_type)
{
    remote_BD_struct s_remote_BD;
    uint32_t result;
    bool update = false;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_UpdateBondingAddress", 0);
#endif
    if(gapBond_PrivateAddr.new_update)
    {
        result = fs_load_remote_BD_struct(&s_remote_BD); 
        if(result == 0)
        {
            if((!memcmp(gapBond_PrivateAddr.unresolved_addr, unresolved_addr, 6))
                &&(gapBond_PrivateAddr.unresolved_addr_type == unresolved_addr_type))
            {
                update = true;
            }
            else
            {
#ifdef GAP_DEBUG_FLAG
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR,"gapbondmgr_UpdateBondingAddress: can't update address",0);
#endif
            }
        }
        else
        {
            update = true;    
        }
        if(update)
        {
#ifdef GAP_DEBUG_FLAG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_UpdateBondingAddress: change saved address to resolved address", 0);
#endif
            memcpy(s_remote_BD.addr, gapBond_PrivateAddr.resolved_addr, B_ADDR_LEN);
            s_remote_BD.remote_bd_type = gapBond_PrivateAddr.resolved_addr_type;
            fs_save_remote_BD_struct(&s_remote_BD);
        }
        gapBond_PrivateAddr.new_update = false;
    }
    if(!update)
    {
        if(!gapbondmgr_CheckAddress(unresolved_addr, unresolved_addr_type))
        {
            GAPBondMgr_EraseAllBondings();
            memcpy(s_remote_BD.addr, unresolved_addr, B_ADDR_LEN);
            s_remote_BD.remote_bd_type = unresolved_addr_type;
            fs_save_remote_BD_struct(&s_remote_BD);
        }
    }
       
    return;
}
#endif
/**
 * @fn          gapbondmgr_Handle_GATTServerStoreInd
 * @brief      process blueAPI_EventGATTServerStoreInd message from bt stack
 * @param    pStoreInd  - pointer to TBlueAPI_GATTServerStoreInd message
 *
 * @return     void
 */
void gapbondmgr_Handle_GATTServerStoreInd(PBlueAPI_GATTServerStoreInd pStoreInd)
{
    uint8_t         dataLen   = 0;
    uint8_t *       pData     = NULL;
    uint8_t remote_BD_equal;    
    TBlueAPI_Cause  cause     = blueAPI_CauseReject;
    static cccData_struct s_cccData;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_Handle_GATTServerStoreInd: opCode=0x%x",
               1, pStoreInd->opCode);
#endif

#ifdef BT_GAP_LE_PRIVACY_RESOLVING
    if(gapbondmgr_CheckAddress(pStoreInd->remote_BD, pStoreInd->remote_BD_Type))
    {
        remote_BD_equal = 0;
    }
    else
    {
        remote_BD_equal = 1;
    }
#else
	remote_BD_struct s_remote_BD;
	uint32_t result;
    result = fs_load_remote_BD_struct(&s_remote_BD);    
    if(result == 0)
    {
        remote_BD_equal = memcmp(pStoreInd->remote_BD, s_remote_BD.addr, 6);
        if(s_remote_BD.remote_bd_type != pStoreInd->remote_BD_Type)
        {
            remote_BD_equal = 1;
        }
    }
    else
    {
        remote_BD_equal = 1;/*not equal*/
    }
#endif  
    switch (pStoreInd->opCode)
    {
    case blueAPI_GATTStoreOpGetCCCBits:
        {
            if (remote_BD_equal == 0)
            {
                fs_load_cccData_struct(&s_cccData);

                if (s_cccData.data_length == 0)
                {
                    dataLen = 0;
                    pData   = NULL;
                    cause   = blueAPI_CauseReject;
                }
                else
                {
                    dataLen = s_cccData.data_length;
                    pData = s_cccData.val;
                    cause   = blueAPI_CauseSuccess;
                }
            }
            else
            {

            }
        }
        break;

    case blueAPI_GATTStoreOpGetAllCCCBits:
        {
            fs_load_cccData_struct(&s_cccData);
            if (s_cccData.data_length == 0)
            {
                dataLen = 0;
                pData   = NULL;
                cause   = blueAPI_CauseReject;
            }
            else
            {
                dataLen = s_cccData.data_length;
                pData = s_cccData.val;
                cause   = blueAPI_CauseSuccess;
            }
        }
        break;

    case blueAPI_GATTStoreOpSetCCCBits:
        {
            if (remote_BD_equal == 0)
            {
                s_cccData.data_length = pStoreInd->dataLength;
                memcpy( s_cccData.val, pStoreInd->data, pStoreInd->dataLength);
                fs_save_cccData_struct(&s_cccData);

                dataLen = pStoreInd->dataLength;
                pData   = pStoreInd->data;
                cause   = blueAPI_CauseSuccess;
            }
            else
            {
                dataLen = 0;
                pData   = NULL;
                cause   = blueAPI_CauseReject;
            }
        }
        break;

    case blueAPI_GATTStoreOpDeleteAllCCCBits:
        {
            cccData_struct s_cccData;
            s_cccData.data_length = 0;
            fs_save_cccData_struct(&s_cccData);

            cause   = blueAPI_CauseSuccess;
        }
        break;

    default:
        break;
    }

    blueAPI_GATTServerStoreConf(pStoreInd->opCode,
                                pStoreInd->remote_BD,
                                pStoreInd->remote_BD_Type,
                                pStoreInd->restartHandle,
                                dataLen,
                                pData,
                                cause
                               );
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- gapbondmgr_Handle_GATTServerStoreInd\n", 0 );
#endif
}

/**
 * @fn          gapbondmgr_Handle_AuthResultInd
 * @brief      process blueAPI_EventAuthResultInd message from bt stack
 * @param    pAuthResultInd  - pointer to TBlueAPI_AuthResultInd message
 *
 * @return     void
 */
bool gapbondmgr_Handle_AuthResultInd(PBlueAPI_AuthResultInd pAuthResultInd)
{
    TBlueAPI_Cause  cause = blueAPI_CauseSuccess;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"gapbondmgr_Handle_AuthResultInd: cause=0x%x",
               1, pAuthResultInd->cause);
#endif
// begin - save to flash
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
    gapbondmgr_UpdateBondingAddress(pAuthResultInd->remote_BD, pAuthResultInd->remote_BD_Type);
#else
    remote_BD_struct s_remote_BD;
    s_remote_BD.addr[0] = pAuthResultInd->remote_BD[0];
    s_remote_BD.addr[1] = pAuthResultInd->remote_BD[1];
    s_remote_BD.addr[2] = pAuthResultInd->remote_BD[2];
    s_remote_BD.addr[3] = pAuthResultInd->remote_BD[3];
    s_remote_BD.addr[4] = pAuthResultInd->remote_BD[4];
    s_remote_BD.addr[5] = pAuthResultInd->remote_BD[5];
    s_remote_BD.remote_bd_type = pAuthResultInd->remote_BD_Type;
    fs_save_remote_BD_struct(&s_remote_BD);
#endif

    switch (pAuthResultInd->keyType)
    {
    case blueAPI_LinkKeyTypeLELocalLTK:
        {
            LTK_struct s_tmp;
            s_tmp.linkKeyLength = pAuthResultInd->linkKeyLength;
            memcpy( &s_tmp.linkKey, &pAuthResultInd->linkKey, pAuthResultInd->linkKeyLength);
            fs_save_LTK_struct(&s_tmp);
            //for slave
            pGapBondCB->PairStateCB(gapBond_ConnHandle, GAPBOND_PAIRING_STATE_BONDED, 0);
        }
        break;

    case blueAPI_LinkKeyTypeLERemoteLTK:
        {
            remLTK_struct s_tmp;
            s_tmp.linkKeyLength = pAuthResultInd->linkKeyLength;
            memcpy( &s_tmp.linkKey, &pAuthResultInd->linkKey, pAuthResultInd->linkKeyLength);
            fs_save_remLTK_struct(&s_tmp);
            //for master
            pGapBondCB->PairStateCB(gapBond_ConnHandle, GAPBOND_PAIRING_STATE_BONDED, 0);

        }
        break;

    case blueAPI_LinkKeyTypeLERemoteIRK:
        {
            IRK_struct s_tmp;
            s_tmp.linkKeyLength = pAuthResultInd->linkKeyLength;
            memcpy( &s_tmp.linkKey, &pAuthResultInd->linkKey, pAuthResultInd->linkKeyLength);
            fs_save_IRK_struct(&s_tmp);
        }
        break;

    case blueAPI_LinkKeyTypeDeleted:
        {
            {
                LTK_struct s_tmp;
                s_tmp.linkKeyLength = 0;
                fs_save_LTK_struct(&s_tmp);
            }

            {
                remLTK_struct s_tmp;
                s_tmp.linkKeyLength = 0;
                fs_save_remLTK_struct(&s_tmp);
            }

            {
                IRK_struct s_tmp;
                s_tmp.linkKeyLength = 0;
                fs_save_IRK_struct(&s_tmp);
            }
        }
    }
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "store data into flash", 0);
#endif
    // end - save to flash

    blueAPI_AuthResultConf(pAuthResultInd->remote_BD,
                           (TBlueAPI_RemoteBDType)pAuthResultInd->remote_BD_Type,
                           cause);
    return true;
}

/**
 * @fn          gapbondmgr_Handle_AuthResultRequestInd
 * @brief      process blueAPI_EventAuthResultInd message from bt stack
 * @param    pAuthResultRequestInd  - pointer to TBlueAPI_AuthResultRequestInd message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_AuthResultRequestInd(PBlueAPI_AuthResultRequestInd pAuthResultRequestInd)
{
    uint32_t errorno = 0;
    bool bIndError = false;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
               "gapbondmgr_Handle_AuthResultRequestInd: bd=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x bdtype = 0x%x, keytype=0x%x \n", 8,
               pAuthResultRequestInd->remote_BD[0],
               pAuthResultRequestInd->remote_BD[1],
               pAuthResultRequestInd->remote_BD[2],
               pAuthResultRequestInd->remote_BD[3],
               pAuthResultRequestInd->remote_BD[4],
               pAuthResultRequestInd->remote_BD[5],
               pAuthResultRequestInd->remote_BD_Type,
               pAuthResultRequestInd->keyType);
#endif
    // begin - load from flash
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
    if(gapPara_PrivacyMode != GAP_PRIVACY_MODE_DISABLE)
    {
        if(!gapbondmgr_CheckAddress(pAuthResultRequestInd->remote_BD, pAuthResultRequestInd->remote_BD_Type))
        {
            blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                          pAuthResultRequestInd->remote_BD_Type,
                                          0,
                                          NULL,
                                          pAuthResultRequestInd->keyType,
                                          pAuthResultRequestInd->restartHandle,
                                          blueAPI_CauseReject);
            bIndError = true;
            return true;
        }
    }
    else
    {
#endif
        remote_BD_struct s_remote_BD;
        errorno = fs_load_remote_BD_struct(&s_remote_BD);

        uint8_t remote_BD_equal = memcmp(pAuthResultRequestInd->remote_BD, s_remote_BD.addr, 6);
    //    if (errorno != 0 || remote_BD_equal != 0 || pAuthResultRequestInd->remote_BD_Type != s_remote_BD.remote_bd_type )
        //Fix IOT issue with IOS
        if (errorno != 0 || pAuthResultRequestInd->remote_BD_Type != s_remote_BD.remote_bd_type )
        {
            blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                          pAuthResultRequestInd->remote_BD_Type,
                                          0,
                                          NULL,
                                          pAuthResultRequestInd->keyType,
                                          pAuthResultRequestInd->restartHandle,
                                          blueAPI_CauseReject);
            bIndError = true;
            return true;
        }
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
    }
#endif
    static LTK_struct s_tmp; // for return s_tmp.linkKey
    switch (pAuthResultRequestInd->keyType)
    {
    case blueAPI_LinkKeyTypeLELocalLTK:
        {
            errorno = fs_load_LTK_struct(&s_tmp);
            if (errorno != 0 || s_tmp.linkKeyLength == 0)
            {
                blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                              pAuthResultRequestInd->remote_BD_Type,
                                              0,
                                              NULL,
                                              pAuthResultRequestInd->keyType,
                                              pAuthResultRequestInd->restartHandle,
                                              blueAPI_CauseReject);
                bIndError = true;
            }
        }
        break;

    case blueAPI_LinkKeyTypeLERemoteLTK:
        {
            errorno = fs_load_remLTK_struct((remLTK_struct*)&s_tmp);
            if (errorno != 0 || s_tmp.linkKeyLength == 0)
            {
                blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                              pAuthResultRequestInd->remote_BD_Type,
                                              0,
                                              NULL,
                                              pAuthResultRequestInd->keyType,
                                              pAuthResultRequestInd->restartHandle,
                                              blueAPI_CauseReject);
                bIndError = true;
            }
        }
        break;

    case blueAPI_LinkKeyTypeLERemoteIRK:
        {
            errorno = fs_load_IRK_struct((IRK_struct*)&s_tmp);
            if (errorno != 0 || s_tmp.linkKeyLength == 0)
            {
                blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                              pAuthResultRequestInd->remote_BD_Type,
                                              0,
                                              NULL,
                                              pAuthResultRequestInd->keyType,
                                              pAuthResultRequestInd->restartHandle,
                                              blueAPI_CauseReject);
                bIndError = true;
            }
        }
        break;

    case blueAPI_LinkKeyTypeDeleted:
        {
            // oxff to erase
            blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                          pAuthResultRequestInd->remote_BD_Type,
                                          0,
                                          NULL,
                                          pAuthResultRequestInd->keyType,
                                          pAuthResultRequestInd->restartHandle,
                                          blueAPI_CauseReject);
            bIndError = true;
        }
        break;
    }

    if (bIndError == true)
    {
        return true;
    }

#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "keytype=0x%x \n", 1, s_remote_BD.remote_bd_type);
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "linkKeyLength=0x%x \n", 1, s_tmp.linkKeyLength);
#endif

    UINT8 i = 0;
    for (i = 0; i < 28; i++)
    {
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "key=0x%x \n", 1, s_tmp.linkKey[i]);
#endif
    }
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "restartHandle=0x%x \n", 1, pAuthResultRequestInd->restartHandle);
#endif
    blueAPI_AuthResultRequestConf(pAuthResultRequestInd->remote_BD,
                                  pAuthResultRequestInd->remote_BD_Type,
                                  s_tmp.linkKeyLength,
                                  s_tmp.linkKey,
                                  pAuthResultRequestInd->keyType,
                                  pAuthResultRequestInd->restartHandle,
                                  blueAPI_CauseAccept
                                 );

    return true;
}

/**
 * @fn          gapbondmgr_Handle_PairableModeSetRsp
 * @brief      process blueAPI_EventPairableModeSetRsp message from bt stack
 * @param    pPairableModeSetRsp  - pointer to TBlueAPI_PairableModeSetRsp message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_PairableModeSetRsp(PBlueAPI_PairableModeSetRsp pPairableModeSetRsp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gapbondmgr_Handle_PairableModeSetRsp: cause=0x%x.",
                1, pPairableModeSetRsp->cause);
#endif
    return true;
}

/**
 * @fn          gapbondmgr_Handle_UserPasskeyReqInd
 * @brief      process blueAPI_EventUserPasskeyReqInd message from bt stack
 * @param    pUserPasskeyReqInd  - pointer to TBlueAPI_UserPasskeyReqInd message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_UserPasskeyReqInd(PBlueAPI_UserPasskeyReqInd pUserPasskeyReqInd)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gapbondmgr_Handle_UserPasskeyReqInd.",0);
#endif
    blueAPI_UserPasskeyReqConf(pUserPasskeyReqInd->remote_BD,
                               blueAPI_CauseAccept
                              );

    //notify app to input passkey
    pGapBondCB->PasskeyEntryKeyboardInputCB(gapBond_ConnHandle);
    return true;
}

/**
 * @fn          gapbondmgr_Handle_UserPasskeyNotificationInfo
 * @brief      process blueAPI_EventUserPasskeyNotificationInfo message from bt stack
 * @param    pUserPasskeyNotificationInfo  - pointer to TBlueAPI_UserPasskeyNotificationInfo
 *                 message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_UserPasskeyNotificationInfo(PBlueAPI_UserPasskeyNotificationInfo pUserPasskeyNotificationInfo)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gapbondmgr_Handle_UserPasskeyNotificationInfo diapVal=%d.",
                    1, pUserPasskeyNotificationInfo->displayValue);
#endif
    //notify app to display passkey
    gapBond_Passcode = pUserPasskeyNotificationInfo->displayValue;
    pGapBondCB->PasskeyEntryDisplayCB(gapBond_ConnHandle, pUserPasskeyNotificationInfo->displayValue);

    return true;
}

/**
 * @fn          gapbondmgr_Handle_UserPasskeyReqReplyRsp
 * @brief      process blueAPI_EventUserPasskeyReqReplyRsp message from bt stack
 * @param    pUserPasskeyReqReplyRsp  - pointer to TBlueAPI_UserPasskeyReqReplyRsp
 *                 message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_UserPasskeyReqReplyRsp(PBlueAPI_UserPasskeyReqReplyRsp pUserPasskeyReqReplyRsp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gapbondmgr_Handle_UserPasskeyReqReplyRsp cause=0x%x.",
                    1, pUserPasskeyReqReplyRsp->cause);
#endif
    return true;
}

/**
 * @fn          gapbondmgr_Handle_RemoteOOBDataReqInd
 * @brief      process blueAPI_EventRemoteOOBDataReqInd message from bt stack
 * @param    pRemoteOOBDataReqInd  - pointer to TBlueAPI_RemoteOOBDataReqInd message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_RemoteOOBDataReqInd(PBlueAPI_RemoteOOBDataReqInd pRemoteOOBDataReqInd)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gapbondmgr_Handle_RemoteOOBDataReqInd .",0);
#endif
    //notify app to input oob data
    pGapBondCB->OobInputCB(gapBond_ConnHandle);

    return true;
}

/**
 * @fn          gapbondmgr_Handle_GATTSecurityRsp
 * @brief      process blueAPI_EventGATTSecurityRsp message from bt stack
 * @param    pGATTSecurityRsp  - pointer to TBlueAPI_GATTSecurityRsp message
 *
 * @return     bool
 */
bool gapbondmgr_Handle_GATTSecurityRsp(PBlueAPI_GATTSecurityRsp pGATTSecurityRsp)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- gapbondmgr_Handle_GATTSecurityRsp: keyType=%d keySize=%d cause=%d\r\n", 3,
               pGATTSecurityRsp->keyType,
               pGATTSecurityRsp->keySize,
               pGATTSecurityRsp->cause
              );
#endif
    return true;
}

/**
 * @fn          GAPBondMgr_ProcessEvent
 * @brief      process blueapi upstream messages here
 * @param    pMsg  - pointer to type TBlueAPI_UsMessage message
 *
 * @return     bool
 */
bool GAPBondMgr_ProcessEvent( PBlueAPI_UsMessage pMsg )
{
    switch ( pMsg->Command )
    {
    default:
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAPBondMgr_Not_ProcessEvent: Command(0x%02x)", 1, pMsg->Command);
#endif
        break;
    case blueAPI_EventConnectMDLInfo:
        gapbondmgr_Handle_ConnectMDLInfo(&pMsg->p.ConnectMDLInfo);
        break;
    case blueAPI_EventACLStatusInfo:
        gapbondmgr_Handle_ACLStatusInfo(&pMsg->p.ACLStatusInfo);
        break;
    case blueAPI_EventGATTServerStoreInd:
        gapbondmgr_Handle_GATTServerStoreInd(&pMsg->p.GATTServerStoreInd);
        break;
    case blueAPI_EventAuthResultInd:
        gapbondmgr_Handle_AuthResultInd(&pMsg->p.AuthResultInd);
        break;
    case blueAPI_EventAuthResultRequestInd:
        gapbondmgr_Handle_AuthResultRequestInd(&pMsg->p.AuthResultRequestInd);
        break;
    case blueAPI_EventPairableModeSetRsp:
        gapbondmgr_Handle_PairableModeSetRsp(&pMsg->p.PairableModeSetRsp);
        break;
    case blueAPI_EventUserPasskeyReqInd:
        gapbondmgr_Handle_UserPasskeyReqInd(&pMsg->p.UserPasskeyReqInd);
        break;
    case blueAPI_EventUserPasskeyNotificationInfo:
        gapbondmgr_Handle_UserPasskeyNotificationInfo(&pMsg->p.UserPasskeyNotificationInfo);
        break;
    case blueAPI_EventUserPasskeyReqReplyRsp:
        gapbondmgr_Handle_UserPasskeyReqReplyRsp(&pMsg->p.UserPasskeyReqReplyRsp);
        break;
    case blueAPI_EventRemoteOOBDataReqInd:
        gapbondmgr_Handle_RemoteOOBDataReqInd(&pMsg->p.RemoteOOBDataReqInd);
        break;
    //for master only
    case blueAPI_EventGATTSecurityRsp:
        gapbondmgr_Handle_GATTSecurityRsp(&pMsg->p.GATTSecurityRsp);
        break;
    }
    return true;
}

