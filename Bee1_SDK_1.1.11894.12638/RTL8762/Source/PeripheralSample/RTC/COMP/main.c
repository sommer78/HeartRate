/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of RTC COMP use.
* @details  If value of counter is equal to value of selected comparator, it will cause interrupt. 
* @author   elliot chen
* @date     2015-07-10
* @version  v0.1
*********************************************************************************************************
*/
/* Defines -------------------------------------------------------------------*/
/* use comparator 0 */
#define RTC_COMPARATOR_INDEX 0

/* Includes ------------------------------------------------------------------*/
#include "legacyperiphapi.h"
#include "trace.h"

#include "rtl876x_rtc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/*------------------------------RTC init----------------------------------------*/
void RTC_Initconfig(void)
{
    /* initialize RTC*/
    RTC_Reset(); 
    RTC_Enable();    
    RTC_SetPrescaler(0);
    RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, 32768);
    RTC_EnableCOMP(RTC_COMPARATOR_INDEX);
    RTC_EnableSystemWakeup();
    
    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);    
    
    RTC_Start();
}

/*-----------------------------RTC test code---------------------------------*/
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

void RTC_TestCode(void)
{
    //add user code
}

/*------------------------------main entry here---------------------------------*/
int main(void)
{ 
    /* Initialize RTC */
    RTC_Initconfig();
    
    //RTC_TestCode();
    while(1)
    {
        DBG_DIRECT("RTC_Counter = 0x%x!", RTC_GetCounter());
        delay_time();
    }
}

void RTCIntrHandler(void)
{  
    uint32_t CompareValue = 0;
    
    /* selected comparator interrupt */
    if((RTC->SR_OFF & BIT(16 + RTC_COMPARATOR_INDEX)) == BIT(16 + RTC_COMPARATOR_INDEX))
    {
        DBG_DIRECT("RTC_Counter = 0x%x, RTC_COMP = 0x%x!", RTC_GetCounter(), RTC_GetCOMPValue(RTC_COMPARATOR_INDEX));
        
        CompareValue = RTC_GetCOMPValue(RTC_COMPARATOR_INDEX) + 32768;
        RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, CompareValue & 0xFFFFFF);
        
        RTC_ClearCOMPEvent(RTC_COMPARATOR_INDEX);
    }
}
