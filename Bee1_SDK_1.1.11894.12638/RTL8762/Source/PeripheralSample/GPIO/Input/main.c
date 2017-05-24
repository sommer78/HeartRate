/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of GPIO input mode.
* @details
* @author   elliot chen
* @date     2015-06-10
* @version  v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
#define TEST_Pin            P0_5
#define GPIO_Test_Pin       GPIO_GetPin(TEST_Pin)
/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/*PWM test code---------------------------------------------------------------*/
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
    Pad_Config(TEST_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
}

void GPIO_InitConfiguration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
   
    GPIO_InitStruct.GPIO_Pin  = GPIO_Test_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd = DISABLE;
    
    GPIO_Init(&GPIO_InitStruct);
}

void GPIO_TestCode(void)
{
   GPIO_ReadInputData();
}

int main(void)
{
    RCC_Configuration();
    GPIO_PINMUXConfiguration();
    GPIO_PADConfiguration();
    GPIO_InitConfiguration();
    GPIO_TestCode();
    while(1);
}
