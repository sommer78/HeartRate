/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief		This file provides demo code to gnereate the specify number of PWM pulse.
* @details	The code can be used for reference to the preliminary development of the stepper motor.
* @author	elliot chen
* @date 		2016-06-03
* @version	v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_PWM.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/* Defines ------------------------------------------------------------------*/
/* Congfure Timer and PWM parameters */
#define PWM_OUT_0				P3_0
#define PWM_CH_0				PWM0
#define TIM_OUTPUT_PWM          TIM3
#define TIM_OUTPUT_INDEX        3
#define TIM_OUTPUT_PERIOD		(1000-1)
#define PWM_CH_0_PERIOD			2
#define PWM_CH_0_DUTY			1

/* Configure Timer parameters which used for control total number of PWM plus */
#define TIM_CTRL_PWM			TIM2

/*! 
* @brief This parameters control the specify number of PWM plus. 
* @note This parameter value is dependent on PWM frequency. 
*/
#define TIM_CTRL_PWM_PERIOD		(8*(TIM_OUTPUT_PERIOD+1)-1)
#define PWM_CTRLIntrHandler		Timer2IntrHandler

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void Board_PWM_Init(void)
{
	Pad_Config(PWM_OUT_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pinmux_Config(PWM_OUT_0, timer_pwm0);
}

/**
  * @brief  Initialize PWM peripheral.
  * @param   No parameter.
  * @return  void
  */
void Driver_PWM_Init(void)
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);

	PWM_InitTypeDef PWM_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
    TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
	//Timer output frequency = 10MHz/(TIM_Period+1)
	TIM_InitStruct.TIM_Period = TIM_OUTPUT_PERIOD;
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM_OUTPUT_PWM, &TIM_InitStruct);
	//PWM output frequency = Timer output frequency/PWM_Period
	PWM_InitStruct.PWM_Period = PWM_CH_0_PERIOD;
	//PWM_Duty <= PWM_Period! 
    PWM_InitStruct.PWM_Duty = PWM_CH_0_DUTY;
	PWM_InitStruct.PWM_TIMIndex = TIM_OUTPUT_INDEX;
	PWM_Init(PWM_CH_0, &PWM_InitStruct);

	/* Timer which used for control total number of PWM plus */
	TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
	//Timer output frequency = 10MHz/(TIM_Period+1) 
	TIM_InitStruct.TIM_Period = TIM_CTRL_PWM_PERIOD;
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM_CTRL_PWM, &TIM_InitStruct);
	//Config timer interrupt 
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIMER2_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

	/* Enable output PWM and timer module */
	TIM_INTConfig(TIM_CTRL_PWM, ENABLE);
	TIM_Cmd(TIM_OUTPUT_PWM, ENABLE);
	TIM_Cmd(TIM_CTRL_PWM, ENABLE);
	PWM_Cmd(PWM_CH_0, ENABLE);
}

/**
  * @brief  demo code of operation about PWM.
  * @param   No parameter.
  * @return  void
  */
void PWM_SampleCode(void)
{
	/* Configure PAD and pinmux firstly! */
	Board_PWM_Init();

	/* Initialize PWM peripheral */
	Driver_PWM_Init();
    while(1);
}

/**
* @brief  main() is the entry of user code.
* @param   No parameter.
* @return  void
*/
int main(void)
{ 
    PWM_SampleCode();
    while(1);
}

/**
  * @brief  Timer interrupt handle function. 
  * @note: App can control PWM plus total count here.
  * @param  None.
  * @return None.
  */
void PWM_CTRLIntrHandler(void)
{
	/* Disable output PWM */
	PWM_Cmd(PWM_CH_0, DISABLE);

	/* Disable timer interrupt which control the specify number of PWM plus */
	TIM_INTConfig(TIM_CTRL_PWM, DISABLE);

	/* Disable all timer */
	TIM_Cmd(TIM_OUTPUT_PWM, DISABLE);
	TIM_Cmd(TIM_CTRL_PWM, DISABLE);

	/* Clear timer interrupt */
	TIM_ClearINT(TIM_CTRL_PWM);    
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

