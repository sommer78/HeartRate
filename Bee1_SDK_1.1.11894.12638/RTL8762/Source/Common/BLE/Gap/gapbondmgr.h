/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gapbondmgr.h
  * @brief   This file contains all the functions prototypes for the GAP bond and pairing 
  *          related functions. 
  * @details
  * @author  ranhui
  * @date    11-5-2015
  * @version v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef GAPBONDMGR_H
#define GAPBONDMGR_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Add Includes here **/
#include "gap.h"
#include "rtl_types.h"
#include "blueapi_types.h"
#include "rtl876x_flash_storage.h"

/** @addtogroup RTK_GAP_MODULE RTK GAP Module
  * @{
  */

/** @defgroup GAP_Bond_Manager GAP Bond Manager
  * @{
  */ 


/** @defgroup GAP_BOND_MANAGER_Exported_Constants GAP Bond Manager Exported Constants
  * @{
  */

/** @defgroup GAPBOND_MANAGER_PARAMETERS GAP Bond Manager Parameters
 * @{
 */
#define GAPBOND_PAIRING_MODE         0x100  //!< Pairing Mode. Read/Write. Size is uint8_t. Default is GAPBOND_PAIRING_MODE_PAIRABLE.@ref  GAPBOND_PAIRING_MODE_DEFINES
#define GAPBOND_MITM_PROTECTION      0x102  //!< Man-In-The-Middle (MITM) basically turns on Passkey protection in the pairing algorithm. Read/Write. Size is uint8_t. Default is GAPBOND_AUTH_NO_MITM_YES_BOND.@ref GAPBOND_MITM_DEFINES
#define GAPBOND_IO_CAPABILITIES      0x103  //!< I/O capabilities.  Read/Write. Size is uint8_t. Default is GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT @ref GAPBOND_IO_CAP_DEFINES.
#define GAPBOND_OOB_ENABLED          0x104  //!< OOB data available for pairing algorithm. Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_OOB_DATA             0x105  //!< OOB Data. Read/Write. size uint8_t[16]. Default is all 0's.
#define GAPBOND_PASSKEY              0x106  //!< The default passcode for MITM protection. size is uint32_t. Range is 0 - 999,999. Default is 0.
#define GAPBOND_FIXED_PASSKEY_ENABLE 0x107  //!< The fix passcode available for pairing. Read/Write. size is uint8_t. Default is 0(disabled).
#define GAPBOND_SEC_REQ_ENABLE       0x108  //!< Send smp security request when connected. Write Only. size is uint8_t. Default is 0(disabled).
#define GAPBOND_SEC_REQ_REQUIREMENT  0x109  //!< Security request requirements. Write Only. size is uint8_t. Default is GAPBOND_SEC_REQ_NO_MITM. @ref GAPBOND_SEC_REQ_DEFINES
#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
#define GAPBOND_KEY_DISTRIBUTION     0x110  //!< Key distribution. Read/Write. Key distribution type please refer to GAP Key distribution Values.
#endif
/** @} End GAPBOND_MANAGER_PARAMETERS */

/** @defgroup GAPBOND_PAIRING_MODE_DEFINES GAP Bond Manager Pairing Modes
 * @{
 */
#define GAPBOND_PAIRING_MODE_NO_PAIRING          0x00  //!< Pairing is not allowed
#define GAPBOND_PAIRING_MODE_PAIRABLE              0x01  //!< Wait for a pairing request or slave security request
/** @} End GAPBOND_PAIRING_MODE_DEFINES */

/** @defgroup GAPBOND_SEC_REQ_DEFINES GAP Security Requirements
 * @{
 */
#define GAPBOND_SEC_REQ_NO_MITM          0x00  //!< Requires nothing
#define GAPBOND_SEC_REQ_YES_MITM         0x01  //!< Requires authenticated (mitm secure) linkkey
/** @} End GAPBOND_SEC_REQ_DEFINES */

/** @defgroup GAPBOND_IO_CAP_DEFINES GAP Bond Manager I/O Capabilities
 * @{
 */
#define GAPBOND_IO_CAP_DISPLAY_ONLY                 blueAPI_IOCapDisplayOnly   //!<  Only a Display present, no Keyboard or Yes/No Keys.
#define GAPBOND_IO_CAP_DISPLAY_YES_NO             blueAPI_IOCapDisplayYesNo    //!<  Display and Yes/No Keys present.
#define GAPBOND_IO_CAP_KEYBOARD_ONLY               blueAPI_IOCapKeyboardOnly   //!<  Only a Keyboard present, no Display.
#define GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT     blueAPI_IOCapNoIO                //!<  No input/output capabilities.
#define GAPBOND_IO_CAP_KEYBOARD_DISPLAY          blueAPI_IOCapKeyboardDisplay  //!<  Keyboard and Display present.
/** @} End GAPBOND_IO_CAP_DEFINES */

/** @defgroup GAPBOND_PAIRING_STATE_DEFINES GAP Bond Manager Pairing States
 * @{
 */
#define GAPBOND_PAIRING_STATE_STARTED             0x00  //!< Pairing started
#define GAPBOND_PAIRING_STATE_COMPLETE            0x01  //!< Pairing complete
#define GAPBOND_PAIRING_STATE_BONDED              0x02  //!< Devices bonded
/** @} End GAPBOND_PAIRING_STATE_DEFINES */

/** @defgroup GAPBOND_ENCRYPT_STATE_DEFINES GAP Bond Manager Encrypt States
 * @{
 */
#define GAPBOND_ENCRYPT_STATE_ENABLED              0x00 //!< Link Encrypted
#define GAPBOND_ENCRYPT_STATE_DISABLED             0x01 //!< Link Not Encrypted
/** @} End GAPBOND_ENCRYPT_STATE_DEFINES */

/** @defgroup GAPBOND_MITM_DEFINES GAP Bond Mitm Flags
 * @{
 */
#define GAPBOND_AUTH_NO_MITM_NO_BOND  blueAPI_AuthNoMITMRequiredNoStore //!<  MITM protection not required, no bonding.
#define GAPBOND_AUTH_NO_MITM_YES_BOND blueAPI_AuthNoMITMRequiredBonding //!<  MITM protection not required, perform dedicated Bonding.
#define GAPBOND_AUTH_YES_MITM_NO_BOND blueAPI_AuthMITMRequiredNoStore   //!<  MITM protection required, no bonding.
#define GAPBOND_AUTH_YES_MITM_YES_BOND blueAPI_AuthMITMRequiredBonding  //!<  MITM protection required, perform dedicated Bonding.
/** @} End GAPBOND_MITM_DEFINES */

#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
/** @defgroup GAPBOND_KEY_DISTRIBUTION GAP Key distribution Values
 * @{
 */
#define GAPBOND_KEY_DEFAULT      0x00  //!< Use default key distribution. Only support LTK.
#define GAPBOND_KEY_TX_IRK       0x01  //!< Can receive peer's IRK. Not send local IRK.
#define GAPBOND_KEY_TX_RX_IRK    0x02  //!< Can receive peer's IRK. Can send local IRK.
#define GAPBOND_KEY_ALL          0x03  //!< Can receive all keys. Can send all keys.
/** @} End GAPBOND_KEY_DISTRIBUTION */
#endif
///@cond
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
#define GAPBOND_PRIVACY_RANDBD_STATIC            0xC0
#define GAPBOND_PRIVACY_RANDBD_RESOLVABLE        0x40
#define GAPBOND_PRIVACY_RANDBD_NON_RESOLVABLE    0x00
#define GAPBOND_PRIVACY_RANDBD_MASK              0xC0
#endif
///@endcond
/** @}End GAP_BOND_MANAGER_Exported_Constants */
  
///@cond
typedef void (*pfnPasskeyEntryDisplayCB_t)(uint16_t connectionHandle, uint32_t displayValue);
typedef void (*pfnPasskeyEntryKeyboardInputCB_t)(uint16_t connectionHandle);
typedef void (*pfnOobInputCB_t)(uint16_t connectionHandle);
typedef void (*pfnPairStateCB_t)(uint16_t connectionHandle, uint8_t  state, uint8_t  status);
typedef void (*pfnEncryptStateCB_t)(uint16_t connectionHandle, uint8_t  state);

/**
 * Callback Registration Structure
 */
typedef struct
{
    pfnPasskeyEntryDisplayCB_t     PasskeyEntryDisplayCB;
    pfnPasskeyEntryKeyboardInputCB_t    PasskeyEntryKeyboardInputCB;
    pfnOobInputCB_t OobInputCB;
    pfnPairStateCB_t    PairStateCB;
    pfnEncryptStateCB_t EncryptStateCB;
} gapBondCBs_t;
///@endcond

/**
 * @defgroup GAPROLES_BONDMGR_EXPORT_APIS GAP Bond Manager Export API Functions
 *
 * @{
 */
extern TGAP_STATUS GAPBondMgr_SetParameter( uint16_t param, uint8_t len, void *pValue );
extern TGAP_STATUS GAPBondMgr_GetParameter( uint16_t param, void *pValue );
extern void GAPBondMgr_SetPairable(void);
extern void GAPBondMgr_InputPassKey(void);
extern void GAPBondMgr_InputOobData(void);
extern bool GAPBondMgr_GetBondedDevice(remote_BD_struct *pBondedDevice);
extern void GAPBondMgr_EraseAllBondings(void);
extern void GAPBondMgr_Pair(void);
/**
* @}
*/
///@cond
extern void GAPBondMgr_ParaInit(void);
extern void GAPBondMgr_Register( gapBondCBs_t *pCB );
extern bool GAPBondMgr_ProcessEvent( PBlueAPI_UsMessage pMsg );
///@endcond

/**
  * @}END GAP_Bond_Manager */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* GAPBONDMGR_H */
