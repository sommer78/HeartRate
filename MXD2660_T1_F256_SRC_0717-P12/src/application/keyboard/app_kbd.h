#ifndef __APP_KBD_H__
#define __APP_KBD_H__
#include "mxd2660_gpio.h"
#include "mxd2660_timer.h"
#include "mxd2660_kbd.h"

#if (IR_REMOTER_SUPPORT == LETV_IR_REMOTER)
	#include "letv_ir_key.h"
#elif (IR_REMOTER_SUPPORT == MI_IR_REMOTER)
	#include "mi_ir_key.h"
#endif


#define ROW_0_PIN     (27)
#define ROW_1_PIN  	  (25)
#define ROW_2_PIN  	  (12)
#define ROW_3_PIN     (17)
#define ROW_4_PIN  	  (31)
#define ROW_5_PIN  	  (31)
#define LINE_0_PIN    (26)
#define LINE_1_PIN    (24)
#define LINE_2_PIN    (28)
#define LINE_3_PIN    (14)
#define LINE_4_PIN    (31)
#define LINE_5_PIN    (31)

#define KEYBOARD_WAKEUP_PIN ((1<<LINE_0_PIN)|(1<<LINE_1_PIN)|(1<<LINE_2_PIN)|(1<<LINE_3_PIN)|(1<<LINE_4_PIN)|(1<<LINE_5_PIN))

extern void app_kbd_msg_proc(void *msg);
extern void app_kbd_msg_send(uint8* pkey_arr, uint8 Length);
extern void app_kbd_init(void);


#if KEYBOARD_LOG_EN
	#define kbd_log(address, data) LOG(address, data)
#else
	#define kbd_log(address, data) 
#endif

















#endif
