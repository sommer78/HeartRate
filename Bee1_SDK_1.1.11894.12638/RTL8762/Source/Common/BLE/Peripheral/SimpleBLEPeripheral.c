enum { __FILE_NUM__ = 0 };
/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simpleBLECentral.c
  * @brief   Simple BLE Central Role interfaces used to transmit messages from GAP to application.
  * @details
  * @author  ranhui
  * @date    11-Aug-2015
  * @version v1.0
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include "peripheral.h"
#include "gapbondmgr.h"
#include "simpleBLEPeripheral.h"
#include "bee_message.h"
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"

/* task queues used to send message from GAP to Application. */
extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;

/* static functions used in this file. */
static void peripheralStateNotificationCB( gaprole_States_t newState );
static void peripheralConnParaUpdateCB(uint16_t connHandle, uint8_t status);
static void peripheralPasskeyEntryDisplayCB(uint16_t connHandle, uint32_t displayValue);
static void peripheralPasskeyEntryKeyboardInputCB(uint16_t connHandle);
static void peripheralOobInputCB(uint16_t connHandle);
static void peripheralPairStateCB( uint16_t connHandle, uint8_t state, uint8_t status );
static void peripheralEncryptStateCB(uint16_t connectionHandle, uint8_t  state);

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLEPeripheral_BondMgrCBs =
{
    peripheralPasskeyEntryDisplayCB,
    peripheralPasskeyEntryKeyboardInputCB,
    peripheralOobInputCB,
    peripheralPairStateCB,
    peripheralEncryptStateCB
};

/******************************************************************
 * @fn          peripheral_sendBtMsgToApp
 * @brief      send BEE_IO_MSG to app task.            
 * @param    pBeeMsgBlk  - pointer to BEE_IO_MSG message
 *
 * @return     void
 */
static void peripheral_sendBtMsgToApp(BEE_IO_MSG *pBeeMsgBlk)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;

    SendQueueResult = xQueueSend(hIoQueueHandle, pBeeMsgBlk, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "peripheral_NotifyStateChange fail2", 1, SendQueueResult);
    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "peripheral_NotifyStateChange fail", 1, SendQueueResult);
    }

}

/******************************************************************
 * @fn          peripheralStateNotificationCB
 * @brief      callback to send state change message to app task.      
 * @param    newState
 *
 * @return     void
 */
static void peripheralStateNotificationCB( gaprole_States_t newState )
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralStateNotificationCB newState=0x%x", 1, newState);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_CONN_STATE_CHANGE;
    btGapMsg.msgData.gapConnStateChange.newState = newState;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          peripheralConnParaUpdateCB
 * @brief      callback to send connection parameter update change message to app task.   
 * @param    connHandle - handle of connection 
 * @param    status - status of update result, 0 -success, otherwise fail.  
 *
 * @return     void
 */
static void peripheralConnParaUpdateCB(uint16_t connHandle, uint8_t status)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralConnParaUpdateCB connHandle=0x%x, status=%d", 2, connHandle, status);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE;
    btGapMsg.msgData.gapConnParaUpdateChange.connHandle = connHandle;
    btGapMsg.msgData.gapConnParaUpdateChange.status = status;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          peripheralPasskeyEntryDisplayCB
 * @brief      callback to send passkey display value  message to app task.   
 * @param    connHandle - handle of connection 
 * @param    displayValue - value to be dispalyed by app.  
 *
 * @return     void
 */
static void peripheralPasskeyEntryDisplayCB(uint16_t connHandle, uint32_t displayValue)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralPasskeyEntryDisplayCB: %d", 1, displayValue);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_PASSKEY_DISPLAY;
    btGapMsg.msgData.gapBondPasskeyDisplay.connHandle = connHandle;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          peripheralPasskeyEntryKeyboardInputCB
 * @brief      callback to send passkey entry input  message to app task.   
 * @param    connHandle - handle of connection 
 *
 * @return     void
 */
static void peripheralPasskeyEntryKeyboardInputCB(uint16_t connHandle)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralPasskeyEntryKeyboardInputCB", 0);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_PASSKEY_INPUT;
    btGapMsg.msgData.gapBondPasskeyInput.connHandle = connHandle;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          peripheralOobInputCB
 * @brief      callback to send oob unput message to app task.   
 * @param    connHandle - handle of connection 
 *
 * @return     void
 */
static void peripheralOobInputCB(uint16_t connHandle)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralOobInputCB", 0);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_OOB_INPUT;
    btGapMsg.msgData.gapBondOobInput.connHandle = connHandle;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          peripheralPairStateCB
 * @brief      callback to send pair state change message to app task.   
 * @param    connHandle - handle of connection 
 * @param    state - pairing state 
 * @param    status - pair result status
 *
 * @return     void
 */
static void peripheralPairStateCB( uint16_t connHandle, uint8_t state, uint8_t status )
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralPairStateCB connHandle=0x%x, state = %d, status = %d", 3, connHandle, state, status);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_STATE_CHANGE;
    btGapMsg.msgData.gapBondStateChange.connHandle = connHandle;
    btGapMsg.msgData.gapBondStateChange.newState = state;
    btGapMsg.msgData.gapBondStateChange.status = status;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          peripheralEncryptStateCB
 * @brief      callback to send encryption state change message to app task.   
 * @param    connHandle - handle of connection 
 * @param    state - encryption state 
 *
 * @return     void
 */
static void peripheralEncryptStateCB(uint16_t connHandle, uint8_t  state)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "peripheralEncryptStateCB connHandle=0x%x, state = %d", 2, connHandle, state);
#endif

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_ENCRYPT_STATE_CHANGE;
    btGapMsg.msgData.gapEncryptStateChange.connHandle = connHandle;
    btGapMsg.msgData.gapEncryptStateChange.newState = state;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    peripheral_sendBtMsgToApp(&bee_io_msg);
}

/******************************************************************
 * @fn          Initial peripheral role
 * @brief      Initialize of gap periopheral role 
 *
 * @return     void
 */
void BtStack_Init_Peripheral(void)
{
    peripheral_RegisterDeviceStateChangeCB( &peripheralStateNotificationCB );
    peripheral_RegisterAppCBs(&peripheralConnParaUpdateCB);
    peripheral_GapParaInit();
    GAPBondMgr_ParaInit();
    GAPBondMgr_Register( &simpleBLEPeripheral_BondMgrCBs );
}



