/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of GPIO interrupt mode.
* @details
* @author   elliot chen
* @date     2015-06-10
* @version  v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
#define TEST_Pin                    P0_5
#define GPIO_Test_Pin               GPIO_GetPin(TEST_Pin)
#define GPIOTestIntrHandler         Gpio5IntrHandler
/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/*GPIO test code---------------------------------------------------------------*/
void RCC_Configuration(void)
{
    /* turn on SPI clock */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
}

void GPIO_PINMUXConfiguration(void)
{
    Pinmux_Config(TEST_Pin, GPIO_FUN);
}

void GPIO_PADConfiguration(void)
{
    Pad_Config(TEST_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
}

void GPIO_InitConfiguration(void)
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
}

void GPIO_IRQConfiguration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = GPIO5_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);   
}

void GPIO_TestCode(void)
{
    //add user code here
}

int main(void)
{
    RCC_Configuration();
    GPIO_PINMUXConfiguration();
    GPIO_PADConfiguration();
    GPIO_IRQConfiguration();
    GPIO_InitConfiguration();  
    GPIO_TestCode();
    while(1);
}

void GPIOTestIntrHandler(void)
{
    GPIO_MaskINTConfig(GPIO_Test_Pin, ENABLE);
    
    // add user code here
    
    GPIO_ClearINTPendingBit(GPIO_Test_Pin);
    GPIO_MaskINTConfig(GPIO_Test_Pin, DISABLE);
}
