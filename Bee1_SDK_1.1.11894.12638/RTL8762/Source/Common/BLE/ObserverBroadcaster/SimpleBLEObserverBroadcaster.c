enum { __FILE_NUM__ = 0 };

#include "FreeRTOS.h"
#include "queue.h"
#include "mesh_api.h"
#include "bee_message.h"
#include "os.h"
#include <string.h>
#include "simpleBLEObserverBroadcaster.h"
#include "observer_broadcaster.h"

static void obStateNotificationCB( gaprole_States_t newState )
{
    TMeshQueueMsg mesh_queue_msg;
	BEE_IO_MSG bee_io_msg;
    
	BT_STACK_MSG btGapMsg;
    btGapMsg.msgData.gapConnStateChange.newState = newState;

    mesh_queue_msg.type = MESH_BT_STATUS_UPDATE;
    mesh_queue_msg.sub_type = BT_MSG_TYPE_CONN_STATE_CHANGE;
    memcpy(&mesh_queue_msg.parm, &btGapMsg, sizeof(mesh_queue_msg.parm));
    MeshSendQueueMessage(&mesh_queue_msg);
    
	bee_io_msg.IoType = BT_STATUS_UPDATE;
	bee_io_msg.subType = BT_MSG_TYPE_CONN_STATE_CHANGE;
	memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
	SendMessage(&bee_io_msg);
}

void BtStack_Init_ob(void)
{
    ob_RegisterDeviceStateChangeCB( obStateNotificationCB );
    ob_GapParaInit();
}
