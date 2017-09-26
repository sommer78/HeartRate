/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCM_00930_H__
#define __LCM_00930_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "wristband_service.h"

extern void LCM_init_oled(void);
extern void LCM_display_off(void);
extern void LCM_display_on(void);
extern uint32 lcm_cmd_proc(void *msg);

#endif /* __LCM_00930_H */

