/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of use calendar function.
* @details  
* @author   elliot chen
* @date     2016-02-29
* @version  v1.0
*********************************************************************************************************
*/
#include"calendar.h"
#include"trace.h"

/* definite variable for record current calendar */
CALENDAR calendar = {16, 2, 29, 11, 47, 50}; 

/**
  * @brief  delay .
  * @param  uMs: the time of delay.
  * @retval None
  */
void delay_ms(uint32_t uMs)
{
	uint32_t i;
	uMs=uMs*4000;
	for(i=0;i<uMs;i++)
	{
		__asm volatile("nop"); 
		__asm volatile("nop");
		__asm volatile("nop");
		__asm volatile("nop");
	}
}
int main(void)
{
    /* Init calendar which initialize RTC and based calendar data struct */
    /* call function of Calendar_Init directly which can update base calendar data if user want to configure again */
	Calendar_Init(&calendar);
	
	while(1)
	{
		Calendar_GetCurrentValue(&calendar);
		DBG_DIRECT(" Year = %d, month= %d, day = %d, hour = %d, minute = %d, second = %d!", calendar.year, calendar.month, calendar.day, calendar.hour, calendar.minute, calendar.second);
		/* DBG_BUFFER(MODULE_APP,LEVEL_INFO," Year = %d, month= %d, day = %d, hour = %d, minute = %d, second = %d!", 6, \
										calendar.year, calendar.month, calendar.day, calendar.hour, calendar.minute, calendar.second);*/
		delay_ms(1000);
	}
}