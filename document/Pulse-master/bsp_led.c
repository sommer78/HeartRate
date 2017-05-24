#include "bsp_led.h"

__IO int fadeRate = 4000;             // breath light fadeRate

void LED_Configuration()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LED_On()
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4, Bit_SET);
}

void LED_Off()
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4, Bit_RESET);
}

// 软件延时
void Delay(__IO uint32_t nCount)
{
  while(nCount--);
} 

int constrain(int num, int min, int max)
{
	if (num < min)
		return min;
	else if (num > max)
		return max;
	else
		return num;
}

/* 通过高低电平的变化实现占空比改变，达到改变平均电流的作用，每次计算改变一次 */
void fadeLED(int fadeRate)
{
	LED_On();
	Delay(fadeRate);
	LED_Off();
	Delay(4000 - fadeRate);
}

void ledFadeToBeat()
{
    fadeRate -= 25;                         //  set onetime LED fade value
    fadeRate = constrain(fadeRate,0,4000);  //  keep LED fade value from going into negative numbers!
    fadeLED(fadeRate);                      //  fade LED
}
