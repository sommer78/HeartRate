#ifndef SIMPLEBLEBROADCASTER_API_H
#define SIMPLEBLEBROADCASTER_API_H



bool broadcaster_StartBtStack(void);
bool broadcaster_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg );

void broadcasterBlueAPIMessageCB(PBlueAPI_UsMessage pMsg);

#endif
