/**
****************************************************************************
* @file    mxd2660_rtc.h
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
#ifndef __MXD2660_RTC_H__
#define __MXD2660_RTC_H__

#ifdef __cplusplus
 extern "C" {
#endif 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "system_config.h"

#define RTC_CHANNEL_TOTEL               (4)
//[31:6]Reserved
//[5] field_int_overflow_status,event on counter overflow
#define RTC_OVERFLOW_BIT                (5)
//[4] field_int_tick_status ,event on counter increment
#define RTC_TICK_BIT                    (4)
//[3] compare event on comp3 register match
#define RTC_COMP_3_MATCH_BIT            (3)
//[2] compare event on comp2 register match
#define RTC_COMP_2_MATCH_BIT            (2)
//[1] compare event on comp1 register match
#define RTC_COMP_1_MATCH_BIT            (1)
//[0] compare event on comp0 register match
#define RTC_COMP_0_MATCH_BIT            (0)

typedef enum
{
	RTC_CH_0 = 0,
	RTC_CH_1 = 1,
	RTC_CH_2 = 2,
	RTC_CH_3 = 3,
}RTC_CHANNLE;

//rtc interrupt handler prototype declaration
typedef void (*RTC_INT_HANDLER)(void);
extern uint8 g_rtc_overflow_flag;//rtc counter overflow flag
extern uint8 g_rtc_comp0int_flag;//rtc compare0 interrupt flag, used for softerware timer only

/**
* @brief  enable RTC channel interrupt
* @param  ch rtc channel, see RTC_CHANNLE enum
* @param  handler channel interrupt callback function
* @return none
*/
extern void rtc_enable_ch(RTC_CHANNLE ch, RTC_INT_HANDLER handler);
/**
* @brief  disable RTC channel interrupt
* @param  ch rtc channel, see RTC_CHANNLE enum
* @return none
*/
extern void rtc_disable_ch(RTC_CHANNLE ch);
/**
* @brief  enable RTC tick interupt
* @param  handler tick interrupt callback function
* @return none
*/
extern void rtc_enable_tick(RTC_INT_HANDLER handler);
/**
* @brief  disable RTC tick interupt
* @param  none
* @return none
*/
extern void rtc_disable_tick(void);
/**
* @brief  enable RTC overflow interupt
* @param  handler overflow interrupt callback function
* @return none
*/
extern void rtc_enable_overflow(RTC_INT_HANDLER handler);
/**
* @brief  disable RTC overflow interupt
* @param  none
* @return none
*/
extern void rtc_disable_overflow(void);
/**
* @brief  get current counter value
* @param  val_hi point to save current counter high bit[39:32]
* @param  val_low point to save current counter high bit[31:0]
* @return 
*/
extern void rtc_get_current_count(uint8 *val_hi, uint32 *val_low);
/**
* @brief  get current compare value
* @param  val_hi point to save compare high bit[39:32]
* @param  val_low point to save compare low bit[31:0]
* @return 
*/
extern void rtc_get_compare(uint8 ch,uint8 *val_hi, uint32 *val_low);
/**
* @brief  get how long from power on
* @param  none
* @return power on time, unit is second
*/
extern uint32 rtc_get_power_on_sec(void);

/**
* @brief  set rtc compare register value, this value is the target interrupt value
* @param  ch rtc channel, see RTC_CHANNLE enum
* @param  val_hi compare[39:32] value
* @param  val_low compare[31:0] value
* @return none
*/
extern void rtc_set_compare(RTC_CHANNLE ch, uint8 val_hi, uint32 val_low);
/**
* @brief  set rtc compare base on current counter
* @param  ch rtc channel, see RTC_CHANNLE enum
* @param  val_hi compare[39:32] value
* @param  val_low compare[31:0] value
* @return none
*/
extern void rtc_set_compare_offset(RTC_CHANNLE ch, uint8 val_hi, uint32 val_low);
/**
* @brief  start RTC timer
* @param  none
* @return none
*/
extern void rtc_start(void);
/**
* @brief  stop RTC timer
* @param  none
* @return none
*/
extern void rtc_stop(void);
/**
* @brief  config rtc compare value and open interrupt, this function use fRTC = 32768
* @param  ch rtc channel, see RTC_CHANNLE enum
* @param  ms interrupt time
* @return none
*/
extern void rtc_set_timerout(RTC_CHANNLE ch, uint32 ms);

extern void rtc_timer_handler(void);


#ifdef __cplusplus
}
#endif

#endif
