/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     calendar.c
* @brief    This file provides drivers for calculateing calendar which uesd to display since 2000.
* @details
* @author   elliot chen
* @date     2016-02-26
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "calendar.h"

/* Defines and contants--------------------------------------------------------*/

/* Configuration of prescaler value of RTC which can not bigger than 12 */
#define RTC_PRESCALER_VALUE 	12
#define RTC_VALUE_TO_SECONDS	(15 - RTC_PRESCALER_VALUE)
#define OVERFLOW_TO_SECONDS		(9 + RTC_PRESCALER_VALUE)

#define SecsPerComYear  	31536000UL 		/* (365*3600*24) */
#define SecsPerLeapYear 	31622400UL 		/* (366*3600*24) */
#define SecsPerFourYear 	126230400UL 	/* ((365*3600*24)*3+(366*3600*24)) */
#define SecsPerDay      	(3600*24UL)

/* Array for calculating year */
const uint32_t Year_Secs_Accu[5] = {0, 31622400, 63158400, 94694400, 126230400};

/* Array for calculating month when year of current clanedar is common year */ 
const uint32_t Month_Secs_Accu_C[13] = {0, 2678400, 5097600, 7776000, 10368000, 13046400,\
											15638400, 18316800, 20995200, 23587200, 26265600, 28857600, 31536000};

/* Array for calculating month when year of current clanedar is leap year */
const uint32_t Month_Secs_Accu_L[13] = {0, 2678400, 5184000, 7862400, 10454400, 13132800, \
											15724800, 18403200, 21081600, 23673600, 26352000, 28944000, 31622400};

/* Global Variables ------------------------------------------------------------*/

/* Record base system calendar  which can be reset by calling function of Calendar_Update */
CALENDAR calendar_base = {16, 1, 1, 00, 00, 00};

/* Store current value of RTC when calendar is updated */
uint32_t RTC_current_value = 0;

/* Record RTC number of overflow */
volatile uint32_t calendar_overflow = 0;

/**
  * @brief  Initialize RTC for calendar.
  * @param  None.
  * @retval None
  */
static void Calendar_InitRTC(void)
{
	/* Initialize RTC overflow function */
	RTC_Reset();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	RTC_Enable();

	/* RTC counter plus 1 per 8 seconds */
	RTC_SetPrescaler(1 << RTC_PRESCALER_VALUE - 1);
	RTC_EnableOverflow();
	RTC_EnableSystemWakeup();
	
	/*  Enable RTC IRQ  */
	NVIC_ClearPendingIRQ(RTC_IRQ);
	NVIC_SetPriority(RTC_IRQ, 1);
	NVIC_EnableIRQ(RTC_IRQ);

	/* Enable RTC module */
	RTC_Start();
}

/**
  * @brief Get total second of current calendar which record since 2000.
  * @param  None.
  * @retval None
  */
uint32_t static Calendar_CalculateSecond(void)
{
	uint32_t temp;
	uint32_t current_counter = RTC_GetCounter() - RTC_current_value;
	
	/* Calculate total time from current time to year of 2000 */
	temp = (calendar_base.year)*SecsPerComYear + ((calendar_base.year+4)/4)*24*60*60;
	
	if((calendar_base.year%4) != 0)
	{
		/* Add seconds of months in common year */
		temp += Month_Secs_Accu_C[calendar_base.month-1];
	}
	else 
	{
		/* Leap year must sub one day */
		temp -= SecsPerDay;
		/* Add seconds of month in leap year */
		temp += Month_Secs_Accu_L[calendar_base.month-1];
	}
	
	temp += (calendar_base.day - 1)*24*60*60;
	temp += calendar_base.hour * 60*60;
	temp += calendar_base.minute * 60;
	temp += calendar_base.second;
	
	/* Software compensation if RTC counter is overflow but have not trigger RTC overflow interrupt */
	if(current_counter < THRESHOLD_VALUE_OF_JUDGE_OVERFLOW)
	{
		temp += (1 << OVERFLOW_TO_SECONDS);
	}
	
	/* Add total seconds from RTC */
	temp += (calendar_overflow << OVERFLOW_TO_SECONDS) + (RTC_GetCounter() >> RTC_VALUE_TO_SECONDS);

	return temp;
}

/**
  * @brief Get current calendar.
  * @param  None.
  * @retval None
  */
void Calendar_GetCurrentValue(CALENDAR *calendar)
{
	uint32_t temp = Calendar_CalculateSecond();
	uint32_t num_of_four_year = temp / SecsPerFourYear;
	uint32_t remain_Sec = temp % SecsPerFourYear;
	int index = 1;
	int Off4Y = 0;

	/* Calculate year of current calendar */
	while(remain_Sec >= Year_Secs_Accu[index++])
	{
		Off4Y++;
	}
	calendar->year = num_of_four_year*4 + Off4Y; 
	
	/* calculate the rest of the seconds to month */
	remain_Sec = remain_Sec - Year_Secs_Accu[index-2];
	index = 0;
	if(calendar->year%4)  
	{
		/* Common year */  
		while(remain_Sec >= Month_Secs_Accu_C[index++]); 
		/* Calculate specific month */
		calendar->month = index - 1;  
		remain_Sec = remain_Sec - Month_Secs_Accu_C[index-2];  
	}  
	else  
	{
		/* Leap year */
		while(remain_Sec >= Month_Secs_Accu_L[index++]);
		/* Calculate specific month */
		calendar->month = index - 1;
		remain_Sec = remain_Sec - Month_Secs_Accu_L[index-2]; 		
	}

	/* Calculate specific year for display which since 2000*/
	calendar->year += 2000;
	
	/* calculate the rest of the seconds to day */
	calendar->day = remain_Sec / SecsPerDay + 1;  
	remain_Sec = remain_Sec % SecsPerDay;

	/* calculate the rest of the seconds to hour, minute and second */
	calendar->second =(remain_Sec%3600)%60;
	calendar->minute =(remain_Sec%3600)/60;
	calendar->hour =(remain_Sec/3600);  
}

/**
  * @brief update current calendar which can be updated by user.
  * @param  None.
  * @retval None
  */
void Calendar_Init(CALENDAR *calendar)
{
	calendar_base.year   = calendar->year;
	calendar_base.month  = calendar->month;
	calendar_base.day    = calendar->day;
	calendar_base.hour   = calendar->hour;
	calendar_base.minute = calendar->minute;
	calendar_base.second = calendar->second;

	/* Initialize RTC to calculate again */
	Calendar_InitRTC();

	/* Clear current value of RTC when calendar is updated */
	RTC_current_value = 0;
		
	/* Clear number of RTC overflow */
	calendar_overflow = 0;
}

/**
  * @brief update current calendar which can be updated by user.
  * @param  calendar: pointer to CALENDAR data struct.
  * @retval None
  */
void Calendar_Update(CALENDAR *calendar)
{
	calendar_base.year   = calendar->year;
	calendar_base.month  = calendar->month;
	calendar_base.day    = calendar->day;
	calendar_base.hour   = calendar->hour;
	calendar_base.minute = calendar->minute;
	calendar_base.second = calendar->second;

	/* Record current value of RTC when calendar is updated */
	RTC_current_value = RTC_GetCounter();
}

#if 0
/**
  * @brief Check current calendar is leap year or not.
  * @param  year: year data of current calendar.
  * @retval The new state of leap year or not
  *     @SET: leap year.
  *     @RESET: not leap year
  */
 FlagStatus static Calendar_CheckLeapYear(uint16_t year)
{
	if(year%4 == 0)
	{
		if(year%100 == 0)
		{
			if(year%400 == 0)
			{
				return SET;
			}
			else
			{
				return RESET;
			}
		}
		else
		{
			return SET;
		}
	}
	else
	{
		return RESET;	
	}
}
#endif

/**
  * @brief RTC interrupt handle function.
  * @param  None.
  * @retval None
  */
void RTCIntrHandler(void)
{	
	/* Over flow interrupt which record number of overflow for calculating calendar */
	if((RTC->SR_OFF & BIT(1)) == BIT(1))
	{
		/* Record number of overflow */
		calendar_overflow++;
		RTC_ClearOverflowEvent();
#if CALENDAR_DBG_EN
		DBG_BUFFER(MODULE_APP,LEVEL_INFO,"RTC OverFlow!",0);
#endif
	}
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

