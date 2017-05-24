enum { __FILE_NUM__ = 0 };

#include "observer.h"
#include "gapbondmgr.h"
#include "simple_ble_observer.h"
#include "bee_message.h"
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Task msg queue used to send msg to Application. */
extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;

static void observerStateNotificationCB( gaprole_States_t newState );

/* We send GAP msg to app through IO Queue. */
static void observer_sendBtMsgToApp(BEE_IO_MSG *pBeeMsgBlk)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;

    SendQueueResult = xQueueSend(hIoQueueHandle, pBeeMsgBlk, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "observer_sendBtMsgToApp fail2", 1, SendQueueResult);
    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "observer_sendBtMsgToApp fail", 1, SendQueueResult);
    }
}

/* When state changed in Observer GAP role, new state will be sent to application through IO Queue. */
static void observerStateNotificationCB( gaprole_States_t newState )
{
    BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "observerStateNotificationCB newState=0x%x", 1, newState);
#endif
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_CONN_STATE_CHANGE;
    btGapMsg.msgData.gapConnStateChange.newState = newState;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    observer_sendBtMsgToApp(&bee_io_msg);
}

/* Initiate the Observer GAP role. */
void BtStack_Init_Observer(void)
{
    observer_RegisterDeviceStateChangeCB( &observerStateNotificationCB );
    observer_GapParaInit();
}

