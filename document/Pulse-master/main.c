#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "bsp_usart1.h"
#include "bsp_adc.h"
#include "bsp_led.h"
#include "bsp_SysTick.h"
#include "interrupt_Config.h"


int main(void)
{
	/* LED config */
	LED_Configuration();
	/* USART1 config */
	USART1_Config();
	
	/* enable ADC1 and config ADC1 to DMA mode */
	ADC1_Init();
	
	/* NVIC config */
	NVIC_Configuration();

	/* Timer2 config */
	TIM2_Configurtion();
	
	/* enable TIM2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	
	
	while (1)
	{
		//printf("while\n");
	}
}
