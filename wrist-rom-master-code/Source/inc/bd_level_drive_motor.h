/*************************************************************************
*
* Wearable device team
* used to control motor vibrate
*
**************************************************************************/
#ifndef _BD_LEVEL_DRIVE_MOTOR_H_
#define _BD_LEVEL_DRIVE_MOTOR_H_

#include <stdint.h>

typedef enum {
    VIBRATE_NONE = 0,
    FAST_VIBRATE = 1,
    SLOW_VIBRATE = 2
}MOTOR_ACTION;


typedef enum {
    VIBRATE = 0,
    SLIENT  = 1
}MOTOR_OP;

void motor_init(void);

uint32_t motor_control_framework_init(void);

void motor_action_control_start(MOTOR_ACTION action, uint16_t virbrate_times);

void motor_action_control_stop(void );

#endif
