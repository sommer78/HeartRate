#ifndef __GPIO_H
#define	__GPIO_H


#include "stm32f10x.h"



void GPIO_Config( void );
#define LED4_OFF GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define LED4_ON GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED3_OFF GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define LED3_ON GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define LED2_OFF GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define LED2_ON GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define LED1_OFF GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define LED1_ON GPIO_ResetBits(GPIOB,GPIO_Pin_0)
 #define LED2_TRIG GPIO_WriteBit(GPIOA,GPIO_Pin_0, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_0)))
 #define LED1_TRIG GPIO_WriteBit(GPIOB,GPIO_Pin_0, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_0)))

#endif /* __GPIO_H */
