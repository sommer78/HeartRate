/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      central.h
* @brief    Head file for Gap Central role
* @details
* @author    ranhui
* @date      2015-4-27
* @version   v0.1
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion **/
#ifndef CENTRAL_H
#define CENTRAL_H

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

/** @defgroup GAP_Central_Role GAP Central Role
  * @{
  */ 

/** @defgroup GAP_Central_Exported_Types GAP Central Exported Types
  * @{
  */

/** @defgroup TCentralAppCB_MsgType TCentralAppCB_MsgType
  * @{ Message data type, when data sent to app directly
  */
typedef enum
{
    ADV_SCAN_RSP_DATA_MSGTYPE,              //!< MSG used to send received advertising or scan response data. 
    CONN_PARAM_CONFIRM_MSGTYPE               //!< MSG used to send proposed connection parameter confirm result. 
}TCentralAppCB_MsgType, *PCentralAppCB_MsgType;
/** End of TCentralAppCB_MsgType * @} */

/** @defgroup TCentralAppCB_MsgData TCentralAppCB_MsgData
  * @{ Message data sent to app directly from central role
  */
typedef union
{
    PBlueAPI_LEScanInfo   pAdvScanRspData;             //!< The message data of ADV_SCAN_RSP_DATA_MSGTYPE
    PBlueAPI_LEConnectionUpdateInd  pConnUpdateIndData;//!< The message data of CONN_PARAM_CONFIRM_MSGTYPE
}TCentralAppCB_MsgData, *PCentralAppCB_MsgData;
/** End of TCentralAppCB_MsgData * @} */

/** @defgroup pfnCentralAppCB_t Central App Callback
  * @{ Callback function used to send message from central role to application directly.
  */
typedef TAppResult (*pfnCentralAppCB_t)(TCentralAppCB_MsgType msgType, TCentralAppCB_MsgData msgData);
/** End of pfnCentralAppCB_t * @} */

/** End of GAP_Central_Exported_Types
* @}
*/

/** @defgroup GAP_Central_Exported_APIs GAP Central Exported APIs
  * @{
  */
extern TGAP_STATUS centralSetGapParameter( uint16_t param, uint8_t len, void *pValue );
extern TGAP_STATUS centralGetGapParameter( uint16_t param, void *pValue );
extern bool central_Connect(void);
extern TGAP_STATUS central_Disconnect(void);
extern TGAP_STATUS central_SendUpdateParam(void);
extern TGAP_STATUS central_StartScan(void);
extern TGAP_STATUS central_StopScan(void);
extern TGAP_STATUS central_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index );
extern TGAP_STATUS central_ReadRSSI( void );
extern TGAP_STATUS central_SetHostChanClass( uint8_t *pChannelMap );
extern void central_RegisterAppDirectCB( pfnCentralAppCB_t pAppCallback );
/** End of GAP_Central_Exported_APIs
* @}
*/

/** End of GAP_Central_Role
* @}
*/

/** End of RTK_GAP_MODULE
* @}
*/

///@cond
extern void central_RegisterAppCBs( gapRolesParamUpdateCB_t pParamUpdateCB );
extern TGAP_STATUS central_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks );
extern void central_GapParaInit(void);
bool central_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg );
///@endcond

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CENTRAL_H */
