/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of LPCOMP which set P1_0 to counter level.
* @details
* @author   elliot chen
* @date     2015-07-1
* @version  v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
/* counter pin for LPCOMP */
#define LPCOMP_Counter_Pin  P1_0

/*just for test which can be any pin*/
#define TEST_Pin            P1_4
#define GPIO_Test_Pin       GPIO_GetPin(TEST_Pin)
/* Includes ------------------------------------------------------------------*/
#include "legacyperiphapi.h"
#include "trace.h"

#include "rtl876x_rtc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/*------------------------------LPCOMP API driver------------------------------*/

/**
  * @brief  Enable LPCOMP.
  * @param None.
  * @retval None
  */
void RTC_LPCOMP_Enable(void)
{
    UINT8 aon_reg_value = 0x0;
    
    RTC->CR0 |=BIT(6);
    /* Enable signal detector from BTAON */
    aon_reg_value = bee_aon_gp_read(0x9E);
    aon_reg_value |= BIT(0); // Enable signal detector
    bee_aon_gp_write(0x9E, aon_reg_value);
}

/*------------------------------RTC init----------------------------------------*/
void RTC_Initconfig(void)
{
    /* initialize RTC */
    RTC_Reset(); 
    RTC_Enable();    
    RTC_SetPrescaler(0);
    RTC_Start();
}

/*-----------------------------LPCOMP test code---------------------------------*/
void delay_time(void)
{
    uint16_t i = 0;
    uint16_t j = 0;
    for(i=0; i<1000; i++)
    {
        for(j=0; j<1000; j++)
        {;}
    }
}

void LPCOMP_TestCode(void)
{
    uint8_t index = 0;
    for(index=0; index<200; index++)
    {
        if(index%2 == 0)
        {
            GPIO_WriteBit(GPIO_Test_Pin, 0);
        }
        else
        {
            GPIO_WriteBit(GPIO_Test_Pin, 1);
        }
        DBG_DIRECT("LP count = 0x%x!", RTC->LPCOMP_CNT);
    }

}

/*------------------------GPIO test function begin---------------------------*/
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
    Pad_Config(TEST_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void GPIO_InitConfiguration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
   
    GPIO_InitStruct.GPIO_Pin  = GPIO_Test_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
}

/* user code entry here */
int main(void)
{ 
    /* avoid can not be program because of P1_0 use to SWD first */
    delay_time();
    delay_time();
    delay_time();
    delay_time();
    
    /*----------------------LPCOMP init--------------------------------*/
    Pinmux_Config(LPCOMP_Counter_Pin, IDLE_MODE);
    Pad_Config(LPCOMP_Counter_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    /* Initialize RTC before use LPCOMP */
    RTC_Initconfig();
    /* Enable LPCOMP */
    RTC_LPCOMP_Enable();
    
    
    /*-----------------test pin output different level-----------------*/
    RCC_Configuration();
    GPIO_PINMUXConfiguration();
    GPIO_PADConfiguration();
    GPIO_InitConfiguration();
    delay_time();
    delay_time();
    
    /* test code connect P1_0 to GPIO_Test_Pin*/
    LPCOMP_TestCode();
    
    while(1)
    {
        //DBG_DIRECT("LPCOMP_CMP= 0x%x, LPCOMP_CNT = 0x%x!",RTC->LPCOMP_CMP, RTC->LPCOMP_CNT);
        //delay_time();
    }
}
