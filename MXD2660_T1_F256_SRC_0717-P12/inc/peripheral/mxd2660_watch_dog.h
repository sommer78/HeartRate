/**
****************************************************************************
* @file    mxd2660_watch_dog.h
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
#ifndef __MXD2660_WATCH_DOG_H__
#define __MXD2660_WATCH_DOG_H__

#ifdef __cplusplus
 extern "C" {
#endif 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "system_config.h"

#define WDT_SET_REG(reg, value)      (reg = value)
#define WDT_SET_BIT(reg,n)           (reg |= (1<<n))
#define WDT_CLR_BIT(reg,n)           (reg &= ~(1<<n))

//wdt_int_status 0x000
//[31:2]	Reserved
//[1] field_wdt_ir_int_overflow_status
#define WDT_INT_OVERFLOW_BIT          (1)
	 #define WDT_SET_OVERFLOW_INT_EN    (WDT_SET_BIT(WDT->int_mask, WDT_INT_OVERFLOW_BIT))
	 #define WDT_SET_OVERFLOW_INT_DIS   (WDT_CLR_BIT(WDT->int_mask, WDT_INT_OVERFLOW_BIT))
//[0] field_wdt_ir_int_mat_status
#define WDT_INT_MATCH_BIT             (0)
	 #define WDT_SET_MATCH_INT_EN       (WDT_SET_BIT(WDT->int_mask, WDT_INT_MATCH_BIT))
	 #define WDT_SET_MATCH_INT_DIS      (WDT_CLR_BIT(WDT->int_mask, WDT_INT_MATCH_BIT))

//wdtir_wdt_int_mask   0x001
//wdtir_wdt_int_clear  0x002

//wdt_trig_cfg 0x003
//[31:5]	Reserved
//[4]   wen_trig:when after set rbus_wdt_wen, set wen_trig bit ,this will load 7bit rbus_wdt_wen value to WDT
#define WDT_LOAD_WEN_BIT              (4)
	#define WDT_LOAD_WEN_TRIG         (WDT_SET_REG(WDT->trig_cfg, (1<<WDT_LOAD_WEN_BIT)))
//[3]   trigovrflw:set watchdog timer to 0xffffffe0, near overflow,can generate overflow interrupt quickly
#define WDT_LOAD_TEST_OVFL_BIT        (3)
   #define WDT_LOAD_OVFL_VALUE        (WDT_SET_REG(WDT->trig_cfg, (1<<WDT_LOAD_TEST_OVFL_BIT)))
//[2]	comp_trig: when after set compare, set comp_trig bit ,this will load 16bit compare value to WDT
#define WDT_LOAD_COMP_BIT             (2)
   #define WDT_LOAD_COMP_VALUE        (WDT_SET_REG(WDT->trig_cfg, (1<<WDT_LOAD_COMP_BIT)))
//[1]	service_wdt_trig:software service watchdog timer ,and will not generate wdt_resetn
#define WDT_SERVICE_DOG_BIT           (1)
   #define WDT_SERVICE_DOG            (WDT_SET_REG(WDT->trig_cfg, (1<<WDT_SERVICE_DOG_BIT)))
//[0]	cnt_clr:counter clear pulse
#define WDT_CLEAR_CNT_BIT             (0)
   #define WDT_CLEAR_CNT              (WDT_SET_REG(WDT->trig_cfg, (1<<WDT_CLEAR_CNT_BIT)))


//wdt_ctrl 0x004
//[31:7] Reserved
//[6:0]	wen:when rbus_wdt_wen[6:0] = 7'h0 ,enable watchdog timer
#define WDT_SET_ENABLE_READY          (WDT_SET_REG(WDT->wen_ctrl, 0x00))
#define WDT_SET_DISABLE_READY         (WDT_SET_REG(WDT->wen_ctrl, 0x5A))

//wdt_compare 0x005
//[31:16]	Reserved
//[15:0] when need overflow int/capture int ,this match_reg need be zero
#define WDT_SET_COMP(value)     (WDT_SET_REG(WDT->compare, value))
#define WDT_SET_TIMER(tm)       (WDT_SET_COMP((tm*32768)/1000+0.5))


/**
* @brief  conffigure watchdog monitor time
* 1.the function will not check the range of tm_ms
* 2.if config en to 0, after this function you can use watch_dog_enable function to enable the controller
* @param  tm_ms monitor time, unit is ms, maxinum is 131072000
* @param  en whether enable or not:1-enable, 0-disable
* @return none
*/
void watch_dog_init(uint32 ms);
/**
* @brief  control the watchdog controller work or not
* @param  none
* @return none
*/
void watch_dog_enable(void);
/**
* @brief  service watchdog
* @param  none
* @return none
*/
void watch_dog_feed(void);

#ifdef __cplusplus
}
#endif

#endif
