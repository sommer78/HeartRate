/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of TIM event mode.
* @details  P0_5 output pluse if event coming.
* @author	elliot chen
* @date 	2015-06-10
* @version	v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
#define TIM_EVENT_TEST			P0_5
#define TIM_ID                  TIM2

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "trace.h"

/*TIM event mode test code---------------------------------------------------------------*/
void RCC_Configuration(void)
{
	/* turn on I2C clock */
	RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
}

void TIM_PINMUXConfiguration(void)
{
    Pinmux_Config(TIM_EVENT_TEST, timer_ete0);
}

void TIM_PADConfiguration(void)
{
    Pad_Config(TIM_EVENT_TEST, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void TIM_InitConfiguration(void)
{
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
    TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    /*cycle time = 1ms*/
	TIM_InitStruct.TIM_Period = 10000;
	TIM_InitStruct.TIM_Mode = 1;
    
    /* set timer event parameter */
    TIM_InitStruct.TIM_EventMode = true;
    TIM_InitStruct.TIM_EventIndex = 0;
    /* set plus duration TIM_EventDuration_32us = 100ns,TIM_EventDuration_64us */
    TIM_InitStruct.TIM_EventDuration = TIM_EventDuration_64us;
    
	TIM_TimeBaseInit(TIM_ID, &TIM_InitStruct);
	TIM_Cmd(TIM_ID, ENABLE);
}
void TIM_TestCode(void)
{
    uint32_t i = 0;
    RCC_Configuration();
    TIM_PINMUXConfiguration();
    TIM_PADConfiguration();
	TIM_InitConfiguration();
    i = *((volatile uint32_t *)0x40000384UL);
    DBG_DIRECT("i = 0x%x", i);
}

int main(void)
{
    TIM_TestCode();
    while(1);
}

