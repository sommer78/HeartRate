/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of RTC overflow use.
* @details If value of conuter add from 0x0000 to 0xffffff, it will cause RTC over flow interrupt.
* @author   elliot chen
* @date     2015-07-10
* @version  v0.1
*********************************************************************************************************
*/

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
    /* initialize RTC */
    RTC_Reset(); 
    RTC_Enable();    
    RTC_SetPrescaler(0);
    RTC_EnableOverflow();
    //RTC_DisableOverflow();
    
    /*  Enable RTC IRQ  */
    NVIC_ClearPendingIRQ(RTC_IRQ);
    NVIC_SetPriority(RTC_IRQ,0);
    NVIC_EnableIRQ(RTC_IRQ);
    
    RTC_Start();
}

/*-----------------------------RTC over flow test code---------------------------------*/
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
	//NVIC_DisableIRQ(RTC_IRQ);
    DBG_DIRECT("RTCIntrHandler!");
    
    /* over flow interrupt */
    if((RTC->SR_OFF & BIT(1)) == BIT(1))
    {
        RTC_ClearOverflowEvent();
        DBG_DIRECT("overflow interrupt!");
    }
    
    //NVIC_ClearPendingIRQ(RTC_IRQ);
	//NVIC_EnableIRQ(RTC_IRQ);
}
