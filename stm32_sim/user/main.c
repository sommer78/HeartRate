/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief  
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include <string.h>
#include "usart.h"
#include "bsp_SysTick.h"






	
int main(void)
{
	

	u8 ret;

	
	
	SysTick_Init();                                                                 //配置 SysTick 为 1ms 中断一次 
	Delay_ms(1000);

	
	uart1_init(115200);
	uart2_init(115200);
	
	
	Delay_ms(100);
	

//	Sim_Usart_GPIO_Configuration();
	MC_INFO("\r\n Battery Reporter  \r\n");
	


	
	
	

  while(1){
  		


		


					
  	}
	
}



/*********************************************END OF FILE**********************/
