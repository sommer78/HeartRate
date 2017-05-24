/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      calendar_internal32K.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-2-26
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CALENDAR_INTERNAL32K_H
#define __CALENDAR_INTERNAL32K_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Defines ------------------------------------------------------------*/
/* Just for test. Enable or disable whether print log if RTC occur over flow. */
#define CALENDAR_DBG_EN			1		
/* use comparator 0 */
#define RTC_COMPARATOR_INDEX 	1

/* Includes ------------------------------------------------------------------*/
#include"legacyperiphapi.h"
#include"rtl876x.h"
#include "gap.h"
#include "blueapi.h"
#include"rtl876x_nvic.h"
#include"rtl876x_rtc.h"
#if CALENDAR_DBG_EN
#include"trace.h"
#endif

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

#ifdef __cplusplus
}
#endif


#endif /*__CALENDAR_INTERNAL32K_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

