/**
 *****************************************************************************
 * @file     app_ir.c
 * @author   wanli.han
 * @version  V1.0.0
 * @date     14-Oct-2016
 * @brief    ir function control and application message handle
 *****************************************************************************
 * @attention before use, you need config some macro so that the program can be
 * work. the macro must be provide is: IR_0,IR_1,IR_PWM2_PRES,IR_HEADER_COMP
 * IR_TAIL_MODIFY,IR_REPEAT,IR_REPEAT_TIMER
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */
#include "app_ir.h"
#include "mxd2660_ir.h"
#include "mxd2660_pwm.h"
#include "mxd2660_gpio.h"


uint8 gIrSendCode[4] = {0};
uint8 gIrSendCnt  = 0;
uint8 gIrSendTotel  = 0;
uint8 gIrSendTailModify = 0;


void ir_match_recall(void)
{
	if(gIrSendCnt<gIrSendTotel)
	{
		uint8 i = gIrSendCnt>>3;
		uint8 j = gIrSendCnt&0x07;
		if(0x80 == ((gIrSendCode[i]<<j)&0x80))
		{
			ir_output1(560, 1690);
		}
		else
		{
			ir_output0(560, 560);
		}
		gIrSendCnt++;
	}
	else if(gIrSendTailModify)
	{
		gIrSendTailModify --;
		ir_output_repeat(560, 0);
	}
	else
	{
		gIrSendCnt++;
	}
	if(gIrSendCnt>gIrSendTotel+1)
	{
		ir_stop();
		ir_log(LOG_IR_SEND_END,gIrSendCnt);
	}
}

void app_ir_init(void)
{  
    gpio_init_function(GPIO_5, IR);
}

void app_ir_msg_proc(uint8 ir_key)
{
	ir_log(LOG_IR_MSG_KEY,ir_key);
	if(LETV_REPEAT == ir_key)
	{
		gIrSendTotel = 2;
		gIrSendCnt = 2;
		gIrSendTailModify = 1;
		ir_output_repeat(9000, 2250);
		ir_start();
		ir_output_repeat(560, 0);
	}
	else if(LETV_RELEASE != ir_key)
	{
		gIrSendCode[0] = CUSTOMER_HEAD1;
		gIrSendCode[1] = CUSTOMER_HEAD2;
		gIrSendCode[2] = ir_key;
		gIrSendCode[3] = ~ir_key;
		gIrSendTotel = 32;
		gIrSendCnt = 0;
		gIrSendTailModify = 1;
        ir_init(9500, 4500, ir_match_recall);
		ir_start();
		ir_output0(560, 560);
		gIrSendCnt++;
	}
}
void app_ir_send(uint8 ir_key)
{
	
}


