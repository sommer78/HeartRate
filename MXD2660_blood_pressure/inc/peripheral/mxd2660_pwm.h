/**
****************************************************************************
* @file mxd2660_pwm.h
* @brief
* @author wanli.han
* @version Initial Draft
* @date 2016/11/18
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
#ifndef __MXD2660_PWM_H__
#define __MXD2660_PWM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "mxd2660_timer.h"
#include "system_config.h"

/**
* @brief  enable pwm
* @param  timer_n timer choice
* @return none
*/
extern void pwm_enable(TIMER_DEFINE timer_n);
/**
* @brief  disable pwm
* @param  timer_n timer choice
* @return none
*/
extern void pwm_disable(TIMER_DEFINE timer_n);
/**
* @brief  pwm config function
* @param  timer_n timer choice  
* @param  pwm_freq pwm output frequency, unit is hz
* @param  duty duty cycle 
* @return none
*/
extern void pwm_config(TIMER_DEFINE timer_n, uint32 pwm_freq, uint8 duty);
/**
* @brief  timer config function
* @param  timer_n timer choice  
* @param  pwm_freq pwm output frequency, unit is hz
* @param  clk timer work clock
* @return none
*/
extern void pwm_init(TIMER_DEFINE timer_n, TIMER_CLK clk);
/**
* @brief  set pwm counter
* @param  timer_n timer choice
* @param  total_cnt one cycle longth
* @param  low_cnt low level duration
* @return none
*/
extern void pwm_set_compare(TIMER_DEFINE timer_n, int16 total_cnt, uint16 low_cnt);
/**
* @brief  config pwm duty
* @param  timer_n timer choice
* @param  duty pwm duty,0~100
* @return none
*/
extern void pwm_config_duty(TIMER_DEFINE timer_n, uint8 duty);
/**
* @brief  get pwm current compare
* @param  timer_n timer choice
* @return compare value,[31:16]:duty compare value,[15:0]:compare value
*/
extern uint32 pwm_get_current_comp(TIMER_DEFINE timer_n);
/**
* @brief  comfig pwm output frequency
* @param  timer_n timer choice
* @param  freq targget output frequency
* @return none
*/
extern void pwm_config_output_freq(TIMER_DEFINE timer_n, uint16 freq);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
