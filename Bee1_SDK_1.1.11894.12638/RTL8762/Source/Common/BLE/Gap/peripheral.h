/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      peripheral.h
* @brief    Head file for Gap peripheral role
* @details
* @author    ranhui
* @date      2015-4-27
* @version   v0.1
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion **/
#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Add Includes here **/
#include "rtl_types.h"
#include "blueapi_types.h"
#include "gap.h"

/** @addtogroup RTK_GAP_MODULE RTK GAP Module
  * @{
  */


/** @defgroup GAP_Peripheral_Role GAP Peripheral Role
  * @{
  */
  
/** @defgroup GAP_Peripheral_Exported_Types GAP Peripheral Exported Types
  * @{
  */
/** @defgroup TRANDOM_ADDR_TYPE Random address type
  * @{ Random address type
  */
typedef enum
{
    RANDOM_ADDR_TYPE_STATIC,              //!< Static random address. 
    RANDOM_ADDR_TYPE_RESOLVABLE,          //!< Resolvable private address. 
    RANDOM_ADDR_TYPE_NON_RESOLVABLE,      //!< Non-resolvable private address. 
    RANDOM_ADDR_TYPE_UNKNOWN              //!< Unknown.   
}TRANDOM_ADDR_TYPE, *PRANDOM_ADDR_TYPE;
/** End of TRANDOM_ADDR_TYPE * @} */

/** End of GAP_Peripheral_Exported_Types
* @}
*/
/** @defgroup GAP_Peripheral_Exported_API_Functions GAP Peripheral Exported API Functions
  * @{
  */
extern TGAP_STATUS peripheralSetGapParameter( uint16_t param, uint8_t len, void *pValue );
extern TGAP_STATUS peripheralGetGapParameter( uint16_t param, void *pValue );
extern TGAP_STATUS peripheral_Disconnect(void);
extern TGAP_STATUS peripheral_SendUpdateParam(void);
extern TGAP_STATUS peripheral_StartAdvertising(void);
extern TGAP_STATUS peripheral_StopAdvertising(void);
extern TGAP_STATUS peripheral_Init_StartAdvertising(void);
extern TGAP_STATUS peripheral_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index );
extern TGAP_STATUS peripheral_ReadRSSI( void );
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
extern TRANDOM_ADDR_TYPE peripheral_CheckRandomAddressType(uint8_t* random_addr);
extern bool peripheral_ResolveRandomAddress(uint8_t* random_addr, uint8_t* resolved_addr);
#endif
/** End of GAP_Peripheral_Exported_API_Functions
* @}
*/

///@cond
extern void peripheral_RegisterAppCBs( gapRolesParamUpdateCB_t pParamUpdateCB );
extern TGAP_STATUS peripheral_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks );
extern void peripheral_GapParaInit(void);
extern void peripheral_Handle_ServiceRegisterCompleteEvt(void);
bool peripheral_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg );
///@endcond

#ifdef ANCS_SUPPORT
/** @defgroup GAP_ANCS_Exported_API_Functions GAP ANCS Exported API Functions
  * @{
  */
void peripheral_AncsDiscover(void);
bool peripheral_AncsGetNotificationAttributes(uint32_t NotificationUID, uint8_t *pAttributeIDs, uint8_t AttributeIDsLen);
bool peripheral_AncsGetAppAttributes(uint8_t *AppIdentifier, char *pAttributeIDs, uint8_t AttributeIDsLen);
bool peripheral_AncsPerformNotificationAction(uint32_t NotificationUID, uint8_t ActionID);
bool peripheral_AncsSetNotificationSourceNotify(bool notify);
bool peripheral_AncsSetDataSourceNotify(bool notify);
/** End of GAP_ANCS_Exported_API_Functions
* @}
*/
#endif
/** End of GAP_Peripheral_Role
* @}
*/

/** End of RTK_GAP_MODULE
* @}
*/

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERAL_H */
