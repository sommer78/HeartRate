#ifndef _SIMPEL_BEL_OBSERVER_API_H_
#define _SIMPEL_BEL_OBSERVER_API_H_

bool observer_StartBtStack(void);
bool observer_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg );
void observerBlueAPIMessageCB(PBlueAPI_UsMessage pMsg);

#endif
