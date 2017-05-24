enum { __FILE_NUM__ = 0 };


#include "broadcaster.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "simple_ble_broadcaster.h"
#include "bee_message.h"
#include <string.h>

extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;


static void broadcasterStateNotificationCB( gaprole_States_t newState );




static void broadcaster_sendBtMsgToApp(BEE_IO_MSG *pBeeMsgBlk)
{

    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;

    SendQueueResult = xQueueSend(hIoQueueHandle, pBeeMsgBlk, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "broadcaster_sendBtMsgToApp fail2", 1, SendQueueResult);
    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "broadcaster_sendBtMsgToApp fail", 1, SendQueueResult);
    }

}


static void broadcasterStateNotificationCB( gaprole_States_t newState )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "broadcasterStateNotificationCB newState=0x%x", 1, newState);
#endif
    {
        BEE_IO_MSG bee_io_msg;
        BT_STACK_MSG btGapMsg;

        bee_io_msg.IoType = BT_STATUS_UPDATE;
        bee_io_msg.subType = BT_MSG_TYPE_CONN_STATE_CHANGE;
        btGapMsg.msgData.gapConnStateChange.newState = newState;

        memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

        broadcaster_sendBtMsgToApp(&bee_io_msg);

    }



}




void BtStack_Init_Broadcaster(void)
{
    broadcaster_RegisterDeviceStateChangeCB( &broadcasterStateNotificationCB );
    broadcaster_GapParaInit();
}



