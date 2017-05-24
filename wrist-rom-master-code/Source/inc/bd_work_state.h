/*************************************************************************
*
* Wearable device team
* work state struct defination
*
**************************************************************************/
#ifndef WORK_STATE_H__
#define WORK_STATE_H__

/**************************************************************************
* Defines the main status
***************************************************************************/
typedef enum {
    LOW_POWER = 0,
    ADVERTISING,
    BINDING,
    NORMAL,
    CHARGING,
    NOTIFYING,
    CAMERA
} WORK_STATE;

#endif
