/*************************************************************************
*
* Wearable device team
* This file implement led flash control
*
**************************************************************************/
#ifndef _INTERACTION_
#define _INTERACTION_

#include "stdint.h"
#include "config.h"

#define NOTIFICATION_DEBUG          0
#define NOTIFICATION_BONDING        1
#define NOTIFICATION_BONDED         2
#define NOTIFICATION_TEST           3
#define NOTIFICATION_CHARGING       4
#define NOTIFICATION_CALLING        5
#define NOTIFICATION_ALARM          6
#define NOTIFICATION_LOSE           7
#define NOTIFICATION_SWITCH         8
#define NOTIFICATION_TARGET         9
#define NOTIFICATION_STATE          10

#define NOTIFICATION_ANIMATE        11



// typedef enum {
//     Bonding = 0,
//     Charging,
//     Calling,
//     Alarm,
//     Counter,
//     Sleep,
//     Unknown
// }Notification_Status;

void notification_start( uint8_t type, uint16_t value );
void notification_stop( void );

#endif //_INTERACTION_
