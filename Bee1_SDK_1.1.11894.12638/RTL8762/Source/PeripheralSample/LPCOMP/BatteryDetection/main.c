/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of LPCOMP which to detection battery level.
* @details  Always enter AnalogComparatorIntrHandler interrupt if connect P4_3 with voltage which higher than threshold voltage.
            you can mask interrupt by calling code (RTC->CR1 &= ~BIT(21);)
            if voltage of P1_0 is lower than threshold voltage, it will stop enter interrupt immediately 
            no matter mask interrupt or not.
* @author   elliot chen
* @date     2015-07-1
* @version  v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
/* Monitoring voltage pin for LPCOMP */
#define Monitoring_voltage_Pin  P1_0


/* Includes ------------------------------------------------------------------*/
#include "legacyperiphapi.h"
#include "trace.h"
#include "rtl876x_rtc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/*------------------------------LPCOMP API driver for low power comparator------------------------------*/

/**
  * @brief  Enable LPCOMP.
  * @param None.
  * @retval None
  */
void LPCOMP_BatteryDetection_Enable(void)
{
    UINT8 aon_reg_value = 0x0;
    
    /* Set RTC_BTAON_POWERCUT_BIT */
    *((volatile uint32_t *)0x40000014) |= BIT(9);
    
    /* Release ISO_BT_CORE_RTC */
    bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)&(~BIT(4)));

    /* Enable lpcomp out sync signal to CPU interrupt */
    RTC->CR1 |= BIT(21);
    
    /*---------------set threshold voltage of low power comparator(user can set threshold voltage here)-----------*/
    aon_reg_value = bee_aon_gp_read(0x8E);
    /* clear value of setting voltage */
    aon_reg_value &= ~(0x1f);
    
    /* threshold voltage = (0x02 * 0.05 + 0.05)*4V = 0.6V */
    aon_reg_value |= 0x02;
    bee_aon_gp_write(0x8E, aon_reg_value);
    
    /* Enable signal detector from BTAON */
    aon_reg_value = bee_aon_gp_read(0x9E);
    aon_reg_value |= BIT(0); 
    bee_aon_gp_write(0x9E, aon_reg_value);  
}

/**
  * @brief Disable LPCOMP enter AnalogComparatorIntrHandler.
  * @param None.
  * @retval None
  */
void LPCOMP_DisableINT(void)
{
    RTC->CR1 &= ~(BIT(21));
}

/*------------------------------- user code entry here---------------------------------------------------*/
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

int main(void)
{ 
    /* avoid that P1_0 can not be program because of being used to SWD function first */
    delay_time();
    delay_time();
    delay_time();
    delay_time();
    delay_time();
    
    /*----------------------LPCOMP init--------------------------------*/
    Pinmux_Config(Monitoring_voltage_Pin, IDLE_MODE);
    Pad_Config(Monitoring_voltage_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    
    /* Enable LPCOMP */
    LPCOMP_BatteryDetection_Enable();
    
    /* Config LPCOMP interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = ANALOG_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
       
    while(1)
    {
       ;
    }
}

void AnalogComparatorIntrHandler(void)
{
    DBG_DIRECT("Enter AnalogComparatorIntrHandler!");
    
    /* mask ANALOG_IRQ interrupt */
    //LPCOMP_DisableINT();
}


