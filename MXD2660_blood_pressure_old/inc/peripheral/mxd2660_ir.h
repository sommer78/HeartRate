/**
****************************************************************************
* @file mxd2660_ir.h
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
#ifndef __MXD2660_IR_H__
#define __MXD2660_IR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "mxd2660_timer.h"
#include "mxd2660_ir.h"
#include "system_config.h"
    
#define MODULATION_CARRIER(LOW,HIGH)       ((((LOW)<<2)<<16)+(((LOW+HIGH)<<2)-1))
    

typedef enum
{
	IR_POLARITY_RISING = 0,     /**< low to high: ____--- */
	IR_POLARITY_FALLING = 1,    /**< high to low: ---____ */
}IR_POLARITY;

/**
* @brief  initialize ir config
* @param  boot_code_time boot code duration
* @param  start_code_time start code duration
* @return none
*/
extern void ir_init(uint16 boot_code_time, uint16 start_code_time, TIMER_OUT_HADNLER handler);
/**
* @brief  start ir send
* @param  none
* @return none
*/
extern void ir_start(void);
/**
* @brief  stop ir send
* @param  none
* @return none
*/
extern void ir_stop(void);
/**
* @brief  set ir out polar: low to high or high to low
* @param  polarity 1-low to high; 0-high to low
* @return none
*/
extern void ir_change_polarity(IR_POLARITY polarity);
/**
* @brief  ir send code 0
* @param  carrier_time effective carrier
* @param  no_carrier_time invalid carrier
* @return none
*/
extern void ir_output0(uint16 carrier_time, uint16 no_carrier_time);
/**
* @brief  ir send code 1
* @param  carrier_time effective carrier
* @param  no_carrier_time invalid carrier
* @return none
*/
extern void ir_output1(uint16 carrier_time, uint16 no_carrier_time);
/**
* @brief  ir send repeat code
* @param  carrier_time effective carrier
* @param  no_carrier_time invalid carrier
* @return none
*/
extern void ir_output_repeat(uint16 carrier_time, uint16 no_carrier_time);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
