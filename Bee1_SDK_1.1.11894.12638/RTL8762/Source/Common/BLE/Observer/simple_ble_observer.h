
#ifndef _SIMPLE_BLE_OBSERVER_H_
#define _SIMPLE_BLE_OBSERVER_H_


#include "gap.h"
#include "bee_message.h"
#ifdef __cplusplus
extern "C"
{
#endif

extern void BtStack_Init_Observer(void);
extern void SimpleBLEObserver_Init(void);
extern void Observer_HandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBLEPERIPHERAL_H */
