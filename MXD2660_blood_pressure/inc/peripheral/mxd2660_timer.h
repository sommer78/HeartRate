/**
****************************************************************************
* @file mxd2660_timer.h
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
#ifndef __MXD2660_TIMER_H__
#define __MXD2660_TIMER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "system_config.h"


typedef enum
{
	TIME0 = 0,
	TIME1 = 1,		 
	TIME2 = 2,	  
	TIME3 = 3,		  
	TIME4 = 4,	  
	TIME5 = 5,	  
	TIME6 = 6,
	TIME_MAX = 7
}TIMER_DEFINE;


typedef enum 
{
    TIME0_MATCH = (1<<0),
    TIME0_OVFL = (1<<1),

    TIME1_MATCH = (1<<2),
    TIME1_OVFL = (1<<3),
        
    TIME2_MATCH = (1<<4),
    TIME2_OVFL = (1<<5),
    
    TIME3_MATCH = (1<<6),
    TIME3_OVFL = (1<<7),
        
    TIME4_MATCH = (1<<8),
    TIME4_OVFL = (1<<9),
    
    TIME5_MATCH = (1<<10),
    TIME5_OVFL = (1<<11),
    
    TIME6_MATCH = (1<<12),
    TIME6_OVFL = (1<<13),
	
	TIMER_INT_MAX
}TIMER_INT_MASK;

typedef enum
{
	TIMER_CLK_16M = 0,
	TIMER_CLK_8M = 1,
	TIMER_CLK_4M = 3,
	TIMER_CLK_2M = 7,
	TIMER_CLK_1M = 15,
}TIMER_CLK;

typedef enum
{
    TIMER_CHANNEL_0 = 0,
    TIMER_CHANNEL_1 = 1,
    TIMER_CHANNEL_2 = 2,
}TIMER_CHANNEL;

#define TIMER_16M_500US_COMP                (0x1F40)
#define TIMER_16M_1MS_COMP                  (0x3E80)
#define TIMER_16M_5MS_COMP                  (0x13880)
#define TIMER_16M_10MS_COMP                 (0x27100)
#define TIMER_16M_1S_COMP                   (0xF42400)
#define TIMER_8M_500US_COMP                 (0x0FA0)
#define TIMER_8M_1MS_COMP                   (0x1F40)
#define TIMER_8M_5MS_COMP                   (0x9C40)
#define TIMER_8M_10MS_COMP                  (0x13880)
#define TIMER_8M_1S_COMP                    (0x7A1200)
#define TIMER_4M_500US_COMP                 (0x07D0)
#define TIMER_4M_1MS_COMP                   (0x0FA0)
#define TIMER_4M_5MS_COMP                   (0x4E20)
#define TIMER_4M_10MS_COMP                  (0x9C40)
#define TIMER_4M_1S_COMP                    (0x3D0900)
#define TIMER_2M_500US_COMP                 (0x03E8)
#define TIMER_2M_1MS_COMP                   (0x07D0)
#define TIMER_2M_5MS_COMP                   (0x2710)
#define TIMER_2M_10MS_COMP                  (0x4E20)
#define TIMER_2M_1S_COMP                    (0x1E8480)

#define IR_16M_33K_COMP                     (0x00F201E4)
#define IR_16M_36K_COMP                     (0x00DE01BC)
#define IR_16M_38K_COMP                     (0x00D201A4)
#define IR_16M_40K_COMP                     (0x00C80190)
#define IR_16M_55K_COMP                     (0x008F011E)

typedef void (*TIMER_OUT_HADNLER)(void);


/**
* @brief  config time interrupt handler
* @param  timer_n see enum TIMER_DEFINE
* @param  channel interrupt channel, see enum TIMER_CHANNEL
* @param  handler timerout handler
* @return none
*/
extern void hw_timer_handler(TIMER_DEFINE timer_n, TIMER_CHANNEL ch, TIMER_OUT_HADNLER handler);
/**
* @brief  config timer compare value
* @param  timer_n see enum TIMER_DEFINE
* @param  comp how long will the timer counter
* @return none
*/
extern void hw_timer_config_compare(TIMER_DEFINE timer_n, uint32 comp);
/**
* @brief  clear timer counter
* @param  timer_n timer choice 
* @return none
*/
extern void hw_timer_clr_cnt(TIMER_DEFINE timer_n);
/**
* @brief  get timer_n current counter
* @param  timer_n timer choice
* @return current counter
*/
extern uint32 hw_timer_get_counter(TIMER_DEFINE timer_n);
/**
* @brief  config timer timerout
* @param  timer_n see enum TIMER_DEFINE
* @param  clk timer work clock
* @param  ms timer out value, unit is ms
* @return none
*/
extern void hw_timer_config_timerout(TIMER_DEFINE timer_n, TIMER_CLK clk, uint16 ms);
/**
* @brief  config timer timerout
* @param  timer_n see enum TIMER_DEFINE
* @param  clk timer work clock
* @param  us timer out value, unit is us
* @return none
*/
extern void hw_timer_config_timerout_us(TIMER_DEFINE timer_n, TIMER_CLK clk, uint16 us);
/**
* @brief  start a timer
* @param  timer_n timer choice
* @return none
*/
extern void hw_timer_start(TIMER_DEFINE timer_n);
/**
* @brief  stop a timer
* @param  timer_n timer choice
* @return none
*/
extern void hw_timer_stop(TIMER_DEFINE timer_n);
/**
* @brief  remove timer interrupt form a handle
* @param  channel interrupt channel, see enum TIMER_CHANNEL
* @param  timer_n timer choice
* @return none
*/
extern void hw_timer_interrupt_deinit(TIMER_CHANNEL channel, TIMER_DEFINE timer_n);
/**
* @brief  get timer_n current compare value
* @param  timer_n timer choice
* @return compare value
*/
extern uint32 hw_timer_get_compare(TIMER_DEFINE timer_n);
/**
* @brief  set timer mask
* @param  timer_n timer choice
* @return none
*/
extern void hw_timer_set_mask(TIMER_DEFINE timer_n);
/**
* @brief  initialize timer
* @param  timer_n see enum TIMER_DEFINE
* @param  clk targget timer clock, see TIMER_CLK enum
* @param  ms timerout value
* @param  channel interrupt channel, see enum TIMER_CHANNEL
* @param  handler timerout handler
* @return none
*/
extern void hw_timer_init(TIMER_DEFINE timer_n, TIMER_CLK clk, uint16 ms, TIMER_CHANNEL channel, TIMER_OUT_HADNLER handler);
/**
* @brief  save config to ram, if user do some config use API which API without save oper
* @param  NONE
* @return none
*/
extern void hw_timer_save_config(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __GPT_H__ */

