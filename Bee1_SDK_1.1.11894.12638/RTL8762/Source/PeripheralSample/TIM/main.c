/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of TIM.
* @details  P0_5 volatige level jump when enter TIM2 interrupt.
* @author	elliot chen
* @date 	2015-06-10
* @version	v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
#define TIM_GPIOTEST			P0_5
#define TIM_ID                  TIM2

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "trace.h"

/*TIM test code---------------------------------------------------------------*/
void RCC_Configuration(void)
{
	/* turn on I2C clock */
	RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
}

void TIM_PINMUXConfiguration(void)
{
    Pinmux_Config(TIM_GPIOTEST, GPIO_FUN);
}

void TIM_PADConfiguration(void)
{
    Pad_Config(TIM_GPIOTEST, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void TIM_InitConfiguration(void)
{
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Pin  = GPIO_GetPin(TIM_GPIOTEST);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_ITCmd = DISABLE;
	GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
	GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
	GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
    
	GPIO_Init(&GPIO_InitStruct);
	GPIO_SetBits(GPIO_GetPin(TIM_GPIOTEST));
	
    TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
	TIM_InitStruct.TIM_Period = 1000*10000;
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM_ID, &TIM_InitStruct);
    
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIMER2_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_ClearINT(TIM_ID);
    TIM_INTConfig(TIM_ID,ENABLE);
	TIM_Cmd(TIM_ID, ENABLE);
}
void TIM_TestCode(void)
{
    RCC_Configuration();
    TIM_PINMUXConfiguration();
    TIM_PADConfiguration();
	TIM_InitConfiguration();
}

int main(void)
{
    TIM_TestCode();
    while(1);
}

void Timer2IntrHandler(void)
{
	uint32_t GPIO_value = 0;
    #if false
	GPIO_value = GPIO_ReadOutputData();
	GPIO_Write(~GPIO_value);
    #else
    GPIO_value = GPIO_ReadOutputDataBit(GPIO_GetPin(TIM_GPIOTEST));
    GPIO_WriteBit(GPIO_GetPin(TIM_GPIOTEST),!GPIO_value);
    #endif
	TIM_ClearINT(TIM_ID);    
}
