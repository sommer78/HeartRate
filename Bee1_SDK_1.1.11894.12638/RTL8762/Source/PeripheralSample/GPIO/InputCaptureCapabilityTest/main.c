/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code to test GPIO interrupt capture capability.
* @details
* @author   elliot chen
* @date     2016-01-18
* @version  v1.0
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/

/* Test pin define */
#define TEST_Pin                    P0_0
#define GPIO_Test_Pin               GPIO_GetPin(TEST_Pin)
#define GPIO_Test_IRQ				GPIO0_IRQ
#define GPIOTestIntrHandler         Gpio0IntrHandler

/* Timer of record GPIO interrupt time */
#define TIM_RECORD					TIM3

/* Test PWM waveform define */
#define PWM_OUTPUT					P4_0
#define TIM_ID                  	TIM2
#define TIM_ID_INDEX				2

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_tim.h"
#include "rtl876x_pwm.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/* Global --------------------------------------------------------------------*/
#define LOOP_QUEUE_MAX_SIZE		1024
#define QUEUE_CAPABILITY			(LOOP_QUEUE_MAX_SIZE - 1)
uint32_t LoopQueue[LOOP_QUEUE_MAX_SIZE] = {0};
volatile uint32_t writeIndex = 0;

/*GPIO test code---------------------------------------------------------------*/
void RCC_Configuration(void)
{
    /* turn on GPIO and timer clock */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
}

void PINMUXConfiguration(void)
{
    Pinmux_Config(TEST_Pin, GPIO_FUN);
	Pinmux_Config(PWM_OUTPUT, timer_pwm0);
}

void PADConfiguration(void)
{
    Pad_Config(TEST_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
	/* Configure PAD of pin for output PWM */
	Pad_Config(PWM_OUTPUT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void PWM_InitConfiguration(void)
{
	PWM_InitTypeDef PWM_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
    TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
	TIM_InitStruct.TIM_Period = 125 - 1;
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM_ID, &TIM_InitStruct);
	TIM_Cmd(TIM_ID, ENABLE);

	PWM_InitStruct.PWM_Period = 2;
    PWM_InitStruct.PWM_Duty = 1;
	PWM_InitStruct.PWM_TIMIndex = TIM_ID_INDEX;
	PWM_Init(PWM0, &PWM_InitStruct);
	/* Output 40KHz PWM */
	PWM_Cmd(PWM0, ENABLE);
}

void ReadTimer_InitConfiguration(void)
{
	/* TIM which for Read current timer value of GPIO interrupt */
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_InitStruct.TIM_Period = 0xffffffff;
	TIM_TimeBaseInit(TIM_RECORD, &TIM_InitStruct);
	TIM_Cmd(TIM_RECORD, ENABLE);
}


void InitConfiguration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
   
    GPIO_InitStruct.GPIO_Pin  = GPIO_Test_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd = ENABLE;
    GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;  
    GPIO_Init(&GPIO_InitStruct);
    GPIO_INTConfig(GPIO_Test_Pin, ENABLE);
    GPIO_MaskINTConfig(GPIO_Test_Pin, DISABLE);

	ReadTimer_InitConfiguration();
	PWM_InitConfiguration();
}

void IRQConfiguration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = GPIO_Test_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);   
}

void TestCode(void)
{
    //add user code here
    /* delay */
    while(1)
   	{
   		uint32_t i = 0;
   		for(i=0; i<0xfffff; i++)
   		{
			__NOP;
		}
	}
}

int main(void)
{
    RCC_Configuration();
    PINMUXConfiguration();
    PADConfiguration();
    IRQConfiguration();
    InitConfiguration();  
    TestCode();
}

void GPIOTestIntrHandler(void)
{
	uint32_t value = TIM_RECORD->CurrentValue;
	
    GPIO_MaskINTConfig(GPIO_Test_Pin, ENABLE);
    
    // add user code here
    LoopQueue[writeIndex++] = value;
	writeIndex &= QUEUE_CAPABILITY;
    
    GPIO_ClearINTPendingBit(GPIO_Test_Pin);
    GPIO_MaskINTConfig(GPIO_Test_Pin, DISABLE);
}
