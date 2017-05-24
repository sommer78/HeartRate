/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      calendar.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-2-26
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CALENDAR_H
#define __CALENDAR_H

#ifdef __cpluspuls
 extern "C" {
#endif

/*! 
* @brief threshold value if RTC counter is overflow but have not trigger RTC overflow interrupt. 
* @note User can configure it. 220~250 
*/
#define THRESHOLD_VALUE_OF_JUDGE_OVERFLOW		250

/*! 
* @brief Whether print log or not. 
* @note User can configure it. 
*/
#define CALENDAR_DBG_EN			1			

/* Includes ------------------------------------------------------------------*/
#include"legacyperiphapi.h"
#include"rtl876x.h"
#include"rtl876x_nvic.h"
#include"rtl876x_rtc.h"
#if CALENDAR_DBG_EN
#include"trace.h"
#endif

/* Defines ------------------------------------------------------------------*/
 typedef struct
{
	uint16_t 	year;
	uint8_t 	month;
	uint8_t 	day;
	uint8_t 	hour;
	uint8_t 	minute;
	uint8_t 	second;
}CALENDAR, *pCALENDAR;

void Calendar_Init(CALENDAR *calendar);
void Calendar_GetCurrentValue(CALENDAR *calendar);
void Calendar_Update(CALENDAR *calendar);

#ifdef __cplusplus
}
#endif


#endif /*__CALENDAR_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

