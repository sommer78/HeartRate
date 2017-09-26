/**
****************************************************************************
* @file rtc_calendar.h
* @brief
* @author wanli.han
* @version Initial Draft
* @date 2016/11/30
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
#ifndef __RTC_CALENDAR_H__
#define __RTC_CALENDAR_H__


#include "system_config.h"
#define BASE_YEAR     (1970)   /** < reference year is 1970-01-01 */

typedef struct
{
	uint16 year;
	uint8 mon;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
	uint8 ms;
}CALENDAR_DATE;

typedef struct HARDWARE_TIMER {
	uint16	ct;	        /* 32768 counter :delete*/
	uint8 ms;
	uint32	sec;		/* seconds */
}HARDWARE_TIMER_STRU;


/**
* @brief  compare two date
* @param  dateA compare date
* @param  dateB compared date
* @return 0:dateA=dateB
* @return 1:dateA>dateB
* @return 0xFE:dateA<dateB
*/
extern uint8 calendar_date_compare(CALENDAR_DATE dateA, CALENDAR_DATE dateB);
/**
* @brief  check whether the year is leap year
* @param  year checked year
* @return 0:not leap year
* @return 1:leap year
*/
extern uint8 year_is_leap_year(uint16 year);

/**
* @brief get poweron time
* @param tm HARDWARE_TIMER_STRU struct point to receive time
* @return none
*/
extern void get_system_on_timer(HARDWARE_TIMER_STRU *tm);
/**
* @brief calculator new date after sec form date
* @param new_date point to receive new date struct
* @param base_date form which day to calculator
* @param sec after how many second
* @return none
*/
extern void calc_new_date(CALENDAR_DATE *new_date, CALENDAR_DATE base_date, uint32 sec);
/**
* @brief get current system date. should this function update system_time & base_tiem & system_date?
* @param date point to receive current date struct
* @return none
*/
extern void get_system_date(CALENDAR_DATE *date);
/**
* @brief get how many second fly form 1970-01-01, this function will not check if the parameter is legal or not
* @param date which day will be calculator
* @param tm HARDWARE_TIMER_STRU struct point to receive time 
* @return none
*/
extern void get_date_second(CALENDAR_DATE date, HARDWARE_TIMER_STRU *tm);
/**
* @brief update system date
* @param date new date will be update
* @return none
*/
extern void update_system_date(CALENDAR_DATE date);
/**
* @brief initialize calendar module
* @param none
* @return none
*/
extern void calendar_init(void);


#endif

