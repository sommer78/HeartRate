/**
  ******************************************************************************
  * @file    bsp_gpio.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   gpio应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_gpio.h"   

 /**
  * @brief  初始化GPIO
  * @param  无
  * @retval 无
  */
void GPIO_Config( void )
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

	

  //打开PB口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//端口模式，此为输出推挽模式
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//初始化对应的端口
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//端口模式，此为输出推挽模式
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//初始化对应的端口
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	LED1_OFF;

		
 
}




/*********************************************END OF FILE**********************/
