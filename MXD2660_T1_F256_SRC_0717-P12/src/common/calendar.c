/**
****************************************************************************
* @file rtc_calendar.c
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
#include "calendar.h"
#include "timer.h"
#include "mxd2660_rtc.h"

//current system date
CALENDAR_DATE system_date;
//system power on time
HARDWARE_TIMER jiffies;
//when reset system date, save the secends form 1970-01-01 to target time
HARDWARE_TIMER system_time;
//the jiffies value when reset system date.
HARDWARE_TIMER base_time;


/**
* @brief  compare two date
* @param  dateA compare date
* @param  dateB compared date
* @return 0:dateA=dateB
* @return 1:dateA>dateB
* @return 0xFF:dateA<dateB
*/
uint8 calendar_date_compare(CALENDAR_DATE dateA, CALENDAR_DATE dateB)
{
	HARDWARE_TIMER timeA,timeB;
	get_date_second(dateA,&timeA);
	get_date_second(dateB,&timeB);
	if(timeA.sec>timeB.sec)
	{
		return 1;
	}
	else if(timeA.sec<timeB.sec)
	{
		return 0xFF;
	}
	if(timeA.ms>timeB.ms)
	{
		return 1;
	}
	else if(timeA.ms<timeB.ms)
	{
		return 0xFF;
	}
	return 0;
}

/**
* @brief  check whether the year is leap year
* @param  year checked year
* @return 0:not leap year
* @return 1:leap year
*/
uint8 year_is_leap_year(uint16 year)
{
	if(0 == (year & 0x03))
	{
		if(0==(year%100))
		{
			if(0==(year%400))
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}
	return 0;
}
/**
* @brief get poweron time
* @param tm HARDWARE_TIMER struct point to receive time
* @return none
*/
void get_system_on_timer(HARDWARE_TIMER *tm)
{
	uint8 high;
	uint32 low;

	rtc_get_current_count(&high,&low);
	tm->sec = high;
	tm->sec <<= 17;
	tm->sec |= low>>15;
	tm->ms = (low & 0x7FFF)/1000;
}

/**
* @brief calculator new date after sec form date
* @param new_date point to receive new date struct
* @param base_date form which day to calculator
* @param sec after how many second
* @return none
*/
void calc_new_date(CALENDAR_DATE *new_date, CALENDAR_DATE base_date, uint32 sec)
{
	uint8 month_day[]={31,28,31,30,31,30,31,31,30,31,30,31};
	uint16 day = sec/(24*60*60);
	uint32 tmp = sec%(24*60*60);

	new_date->sec = base_date.sec + tmp % 60;
	tmp = tmp/60;//min
	new_date->min = base_date.min + (new_date->sec/60) + (tmp % 60);
	new_date->sec = new_date->sec % 60;
	tmp = tmp/60;//hour
	new_date->hour = base_date.hour + (new_date->min/60) + (tmp % 60);
	new_date->min = new_date->min % 60;

	new_date->day = base_date.day + (new_date->hour/24);
	new_date->hour = new_date->hour % 24;
	new_date->mon = base_date.mon;
	new_date->year = base_date.year;

	//adjust the date to first day at this year
	tmp = 0;
	if(year_is_leap_year(new_date->year))
	{
		month_day[1] = 29;
	}
	while(tmp < new_date->mon-1)
	{
		day += month_day[tmp];
		tmp++;
	}
	day += new_date->day-1;
	new_date->day = 1;
	new_date->mon = 1;
	while(day > 365)
	{
		if(year_is_leap_year(new_date->year))
		{
			day -= 366;
		}
		else
		{
			day -= 365;
		}
		new_date->year++;
	}
	//remaining day little than a leap yare
	if(year_is_leap_year(new_date->year))
	{
		month_day[1] = 29;
	}
	else
	{
        if(365 == day)
		{
			new_date->year++;
			return;
		}
		month_day[1] = 28;
	}
	while(day >= month_day[new_date->mon-1])
	{
		day -= month_day[new_date->mon-1];
		new_date->mon++;
	}
	new_date->day = (day+1)&0xFF;
    
}

/**
* @brief get current system date. should this function update system_time & base_tiem & system_date?
* @param date point to receive current date struct
* @return none
*/
void get_system_date(CALENDAR_DATE *date)
{
	HARDWARE_TIMER tm;
	uint32 sec;
	get_system_on_timer(&tm);

	sec = tm.sec - base_time.sec;
	calc_new_date(date,system_date,sec);
}

/**
* @brief get how many second fly form 1970-01-01, this function will not check if the parameter is legal or not
* @param date which day will be calculator
* @param tm HARDWARE_TIMER struct point to receive time 
* @return none
*/
void get_date_second(CALENDAR_DATE date, HARDWARE_TIMER *tm)
{
	uint32 day = 0;
	uint32 sec = 0;
	uint16 tmp = BASE_YEAR;
	uint8 month_day[]={31,28,31,30,31,30,31,31,30,31,30,31};
	while(tmp < date.year)
	{
		if(year_is_leap_year(tmp))
		{
			day += 366;
		}
		else
		{
			day += 355;
		}
		tmp++;
	}
	if(year_is_leap_year(tmp))
	{
		month_day[1] = 29;
	}
	tmp = 0;
	while(tmp < date.mon-1)
	{
		day += month_day[tmp];
		tmp++;
	}
	day += date.day-1;
	sec = date.hour * 3600 +date.sec;
	tm->ms = date.ms;
	tm->sec = sec + day*3600*24;
}
/**
* @brief update system date
* @param date new date will be update
* @return none
*/
void update_system_date(CALENDAR_DATE date)
{
	system_date.year = date.year;
	system_date.mon = date.mon;
	system_date.day = date.day;
	system_date.hour = date.hour;
	system_date.min = date.min;
	system_date.sec = date.sec;
	system_date.ms = date.ms;
	get_system_on_timer(&base_time);
	get_date_second(date, &system_time);
}
/**
* @brief update system date use seconds from 1970-01-01
* @param seconds passed from 1970-01-01
* @return none
*/
void update_system_date_sec(uint32 sec)
{
	CALENDAR_DATE base;
	CALENDAR_DATE new;

	calc_new_date(&new,base,sec);
	update_system_date(new);
}
/**
* @brief get system date use seconds from 1970-01-01
* @param seconds passed from 1970-01-01
* @return none
*/
uint32 get_system_date_sec(void)
{
	CALENDAR_DATE sys;
	HARDWARE_TIMER tm;
	get_system_date(&sys);
	get_date_second(sys,&tm);
	return tm.sec;
}
/**
* @brief initialize calendar module
* @param none
* @return none
*/
void calendar_init(void)
{
	CALENDAR_DATE date;
	date.year = BASE_YEAR;
	date.mon = 1;
	date.day = 1;
	date.hour = 0;
	date.min = 0;
	date.sec = 0;
	date.ms = 0;
	update_system_date(date);
}

