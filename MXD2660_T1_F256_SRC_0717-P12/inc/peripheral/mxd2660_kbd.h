/**
****************************************************************************
* @file    mxd2660_kbd.h
* @brief   
* @author  wanli.han
* @version Initial Draft
* @date    2016/11/14
* @history 
* @note    
* detailed description:
* keyboard driver file
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
#ifndef __MXD2660_KBD_H__
#define __MXD2660_KBD_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include "mxdaqua.h"
#include "system_config.h"

#define KB_TIMER_REPEAT      (10)
#define KEYBOARD_TIMER       TIME6		

typedef enum
{
	KBD_INT_FALLING_EDGE = 0x00,     /**< keyboard interrupt trigger mode falling edge */
	KBD_INT_RISING_EDGE = 0x01,      /**< keyboard interrupt trigger mode rising edge */
}KBD_INT_MODE;

typedef enum
{
	GPIO_KEY_RELEASE = 0,      /**< key released */
	GPIO_KEY_1 = 1,
	GPIO_KEY_2 = 2,
	GPIO_KEY_3 = 3,
	GPIO_KEY_4 = 4,
	GPIO_KEY_5 = 5,
	GPIO_KEY_6 = 6,
	GPIO_KEY_7 = 7,
	GPIO_KEY_8 = 8,
	GPIO_KEY_9 = 9,
	GPIO_KEY_10 = 10,
	GPIO_KEY_11 = 11,
	GPIO_KEY_12 = 12,
	GPIO_KEY_13 = 13,
	GPIO_KEY_14 = 14,
	GPIO_KEY_15 = 15,
	GPIO_KEY_16 = 16,
	GPIO_KEY_17 = 17,
	GPIO_KEY_18 = 18,
	GPIO_KEY_19 = 19,
	GPIO_KEY_20 = 20,
	GPIO_KEY_21 = 21,
	GPIO_KEY_22 = 22,
	GPIO_KEY_23 = 23,
	GPIO_KEY_24 = 24,
	GPIO_KEY_25 = 25,
	GPIO_KEY_26 = 26,
	GPIO_KEY_27 = 27,
	GPIO_KEY_28 = 28,
	GPIO_KEY_29 = 29,
	GPIO_KEY_30 = 30,
	GPIO_KEY_31 = 31,
	GPIO_KEY_32 = 32,
	GPIO_KEY_33 = 33,
	GPIO_KEY_34 = 34,
	GPIO_KEY_35 = 35,
	GPIO_KEY_36 = 36,
	GPIO_KEY_REPEAT = 0xFF,    /**< key keep press */
}GPIO_KEY_DEF;


typedef struct
{
	uint8 row;                 /**< keyboard row num */
	uint8 line;                /**< keyboard line num */
	uint8 interval_10ms;            /**< keyboard check repeat and release timer interval*/
	uint8 debounce_tm;         /**< keyboard debounce time */
	uint32 row_pins;           /**< keyboard row pins select,[30:0]every five bits indicate one row pin */
	uint32 line_pins;          /**< keyboard line pins select,[30:0]every five bits indicate one line pin */
	KBD_INT_MODE mode;         /**< keyboard interrupt trigger mode */
}KBD_CONFIG_STRU;

typedef void (*kb_scan_callback)(uint8 scanResault[2]);

/**
* @brief  config keyboard interrupt mask
* @param  mask interrupt mask, one bit is one pin, the order is same as pin_sel in keybaord_init,set mask to 0 will disable keyboard function
* @param  callback when key pressed or released, keyboard module callback function
* @return none
*/
extern void kbd_enable(uint8 mask, kb_scan_callback callback);

/**
* @brief  init keyboard module
* @param  kbdConf point to config information struct
* @return none
*/
extern void kbd_init(KBD_CONFIG_STRU *kbd_conf);

		
#ifdef __cplusplus
}
#endif

#endif /* __MXD2660_KBD_H__ */		
