enum { __FILE_NUM__ = 0 };
/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_central.c
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
#include "central.h"
#include "gapbondmgr.h"
#include "simple_ble_central.h"
#include "bee_message.h"
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"

/* task queues used to send message from GAP to Application. */
extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;

/* static functions used in this file. */
static void centralStateNotificationCB( gaprole_States_t newState );
static void centralConnParaUpdateCB(uint16_t connHandle, uint8_t status);
static void centralPasskeyEntryDisplayCB(uint16_t connHandle, uint32_t displayValue);
static void centralPasskeyEntryKeyboardInputCB(uint16_t connHandle);
static void centralOobInputCB(uint16_t connHandle);
static void centralPairStateCB( uint16_t connHandle, uint8_t state, uint8_t status );
static void centralEncryptStateCB(uint16_t connectionHandle, uint8_t  state);

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLECentral_BondMgrCBs =
{
    centralPasskeyEntryDisplayCB,
    centralPasskeyEntryKeyboardInputCB,
    centralOobInputCB,
    centralPairStateCB,
    centralEncryptStateCB
};

/**
  * @brief  Used internal, send message to application from GAP.
  * @param  pBeeMsgBlk: Bee format message block sent to Application.
  * @retval none.
  */
static void central_sendBtMsgToApp(BEE_IO_MSG *pBeeMsgBlk)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;

    SendQueueResult = xQueueSend(hIoQueueHandle, pBeeMsgBlk, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_sendBtMsgToApp fail2", 1, SendQueueResult);
    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "central_sendBtMsgToApp fail", 1, SendQueueResult);
    }
}

/**
  * @brief  Used internal, send device GAP State message to application from GAP.
  * @param  newState: the new state to be notified to app.
  * @retval none.
  */
static void centralStateNotificationCB( gaprole_States_t newState )
{
    BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralStateNotificationCB newState=0x%x", 1, newState);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_CONN_STATE_CHANGE;
    btGapMsg.msgData.gapConnStateChange.newState = newState;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Used internal, send Connection Parameter Update Result message to application from GAP.
  * @param  connHandle: MDL ID of the link.
  * @param  status: result status, 0 means success while 1 means fail.
  * @retval none.
  */
static void centralConnParaUpdateCB(uint16_t connHandle, uint8_t status)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralConnParaUpdateCB connHandle=0x%x, status=%d", 2, connHandle, status);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE;
    btGapMsg.msgData.gapConnParaUpdateChange.connHandle = connHandle;
    btGapMsg.msgData.gapConnParaUpdateChange.status = status;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Used internal, send Passkey Entry Display message to application from GAP.
  * @param  connHandle: MDL ID of the link.
  * @param  displayValue: passkey value to display.
  * @retval none.
  */
static void centralPasskeyEntryDisplayCB(uint16_t connHandle, uint32_t displayValue)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralPasskeyEntryDisplayCB: %d", 1, displayValue);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_PASSKEY_DISPLAY;
    btGapMsg.msgData.gapBondPasskeyDisplay.connHandle = connHandle;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Used internal, send Passkey Entry Keyboard Input message to application from GAP.
  * @param  connHandle: MDL ID of the link.
  * @retval none.
  */
static void centralPasskeyEntryKeyboardInputCB(uint16_t connHandle)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralPasskeyEntryKeyboardInputCB", 0);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_PASSKEY_INPUT;
    btGapMsg.msgData.gapBondPasskeyInput.connHandle = connHandle;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Used internal, send OOB Input message to application from GAP.
  * @param  connHandle: MDL ID of the link.
  * @retval none.
  */
static void centralOobInputCB(uint16_t connHandle)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralOobInputCB", 0);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_OOB_INPUT;
    btGapMsg.msgData.gapBondOobInput.connHandle = connHandle;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Used internal, send Pair State message to application from GAP.
  * @param  connHandle: MDL ID of the link.
  * @param  state: new state to be informed.
  * @param  status: pairing status?
  * @retval none.
  */
static void centralPairStateCB( uint16_t connHandle, uint8_t state, uint8_t status )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralPairStateCB connHandle=0x%x, state = %d, status = %d", 3, connHandle, state, status);
#endif
    BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_BOND_STATE_CHANGE;
    btGapMsg.msgData.gapBondStateChange.connHandle = connHandle;
    btGapMsg.msgData.gapBondStateChange.newState = state;
    btGapMsg.msgData.gapBondStateChange.status = status;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Used internal, send Encryption State message to application from GAP.
  * @param  connHandle: MDL ID of the link.
  * @param  state: new state to be informed.
  * @retval none.
  */
static void centralEncryptStateCB(uint16_t connHandle, uint8_t  state)
{
	BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "centralEncryptStateCB connHandle=0x%x, state = %d", 2, connHandle, state);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_ENCRYPT_STATE_CHANGE;
    btGapMsg.msgData.gapEncryptStateChange.connHandle = connHandle;
    btGapMsg.msgData.gapEncryptStateChange.newState = state;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    central_sendBtMsgToApp(&bee_io_msg);
}

/**
  * @brief  Initiate the GAP Central Role Module.
  * @retval none.
  */
void BtStack_Init_Central(void)
{
    central_GapParaInit();
    GAPBondMgr_ParaInit();
    central_RegisterDeviceStateChangeCB( &centralStateNotificationCB );
    central_RegisterAppCBs(&centralConnParaUpdateCB);
    GAPBondMgr_Register( &simpleBLECentral_BondMgrCBs );
}

