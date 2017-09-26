/**
****************************************************************************
* @file    app_kbd.c
* @brief   
* @author  wanli.han
* @version Initial Draft
* @date    2016/11/18
* @history 
* @note    
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/


#include "app_kbd.h"
#include "app_message.h"
#include "cmsis_os.h"
#include "app_ble.h"
#include "mem.h"
#include "mxd2660_timer.h"
#include "system_config.h"


void app_kbd_msg_proc(void *msg)
{
	
}
/**
* @brief 
* @param 
* @return 
*/
void app_kbd_msg_send(uint8* pkey_arr, uint8 Length)
{
	uint16 tmp;
	tmp = pkey_arr[0];
	tmp <<=8;
	tmp |= pkey_arr[1];
	ble_hid_sendMsgToApp(pkey_arr, Length, NULL);

}

/**
* @brief	keyboard scan callback fucntion
* @param	key scan key arr, support max key number is 3
* @return	none
*/
void app_kbd_scan_cb(uint8 key[3])
{
	app_kbd_msg_send(key,2);
}


/**
* @brief   initialize keyboard controller
* @param   none
* @return  none
*/
void app_kbd_init(void)
{
	KBD_CONFIG_STRU g_kbdConf;
	g_kbdConf.debounce_tm = 0x3F;
	g_kbdConf.row = 4;
	g_kbdConf.line = 4;
	g_kbdConf.line_pins = LINE_0_PIN|LINE_1_PIN<<5|LINE_2_PIN<<10|LINE_3_PIN<<15|LINE_4_PIN<<20|LINE_5_PIN<<25;
	g_kbdConf.row_pins = ROW_0_PIN|ROW_1_PIN<<5|ROW_2_PIN<<10|ROW_3_PIN<<15|ROW_4_PIN<<20|ROW_5_PIN<<25;
	g_kbdConf.mode = KBD_INT_FALLING_EDGE;
	g_kbdConf.interval_10ms = 10;
	
	kbd_init(&g_kbdConf);
	kbd_enable(0x1F,app_kbd_scan_cb);
}



