/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     calendar_internal32K.c
* @brief    This file provides drivers for calculateing calendar which uesd to display since 2000.
* @details
* @author   elliot chen
* @date     2016-02-26
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "calendar_internal32K.h"

/* Defines and contants--------------------------------------------------------*/
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
/* Record RTC number of overflow */
//volatile uint32_t calendar_overflow = 0;
bool check_en = true;
volatile uint32_t count = 0;
uint32_t rtc_check_params[2] = {0};

/* External function ------------------------------------------------------------*/
uint32_t FMC_ioctl(uint32_t cmd, uint32_t p1, uint32_t p2);


bool blueAPI_GattSendVendorHciCmd(uint16_t opCode, uint8_t len, uint8_t *para)
{
	TApiBufUserDefined ApiBufUserDefined;
	ApiBufUserDefined.Type = API_TYPE_GATT_SEND_VENDOR_HCI_CMD;
	ApiBufUserDefined.p.ApiBufVendorCmd.opCode = opCode;
	ApiBufUserDefined.p.ApiBufVendorCmd.len = len;
	ApiBufUserDefined.p.ApiBufVendorCmd.para = para;
	return blueAPI_UserDefined(&ApiBufUserDefined); 
}

bool blueAPI_SetRTCInternal32k (bool enable_flag)
{
	uint8_t len;
	uint8_t hci_para[2] = {0};

	hci_para[0] = 0xa;
	hci_para[1] = (uint8_t) enable_flag;

	len = sizeof(hci_para);
			
	DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, " blueAPI_SetRTCInternal32k: enable_flag (%d)",
					1, enable_flag);
	
	return blueAPI_GattSendVendorHciCmd(0xFC6E, len, hci_para);
}

/**
  * @brief  Initialize RTC for calendar.
  * @param  None.
  * @retval None
  */	
void Calendar_InitRTC(void)
{
	blueAPI_SetRTCInternal32k(ENABLE);

	/* Initialize RTC*/
	RTC_ResetForInternal32K();
	RTC_Enable();
	RTC_SetPrescaler(0);
	RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, 32000);
	RTC_EnableCOMP(RTC_COMPARATOR_INDEX);
	RTC_EnableSystemWakeup();

	/* Enable comparator interrupt */
	if(RTC_COMPARATOR_INDEX == 0)
	{
		RTC_INTConfig(RTC, RTC_INT_CMP0, ENABLE);
	}
	else if(RTC_COMPARATOR_INDEX == 1)
	{
		RTC_INTConfig(RTC, RTC_INT_CMP1, ENABLE);
	}
	else if(RTC_COMPARATOR_INDEX == 2)
	{
		RTC_INTConfig(RTC, RTC_INT_CMP2, ENABLE);
	}
	else
	{
		if(RTC_COMPARATOR_INDEX == 3)
		{
			RTC_INTConfig(RTC, RTC_INT_CMP3, ENABLE);
		}
	}

	/* Config RTC interrupt */
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
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
	
	/* Add total seconds from RTC */
	//temp += (calendar_overflow << 21) + RTC_GetCounter()/8;
	temp += count; 

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

	/* Clear RTC number of overflow */
	//calendar_overflow = 0;
	
	/* Initialize RTC to calculate again */
	Calendar_InitRTC();
}

void RTCIntrHandler(void)
{  
    uint32_t CompareValue = 0;

	/* record time */
	count++;
	
    /* selected comparator interrupt */
    if((RTC->SR_OFF & BIT(16 + RTC_COMPARATOR_INDEX)) == BIT(16 + RTC_COMPARATOR_INDEX))
    {
		/* Clear RTC counter and enable RTC again */
		RTC->CR0 = (BIT(31)|BIT(17)|0x04);
		RTC->CR0 = (BIT(31)|BIT(17)|0x05);
		if(FMC_ioctl(0x0e, rtc_check_params, 2) == 0)
		{
			check_en = false;
		}

		if(rtc_check_params[1] > rtc_check_params[0])
        {
            // fast
            RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, 16 + 32000*(1 + (rtc_check_params[1] - rtc_check_params[0])/rtc_check_params[0]));
            //DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Fast: 0x%d!\n\n", 1, rtc_check_params[1] - rtc_check_params[0]);
        }
        else
        {
            // slow
            RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, 16 + 32000*(1 - (rtc_check_params[0] - rtc_check_params[1])/rtc_check_params[0]));
        }       
        //RTC->SR_OFF = BIT(31)|BIT(16 + RTC_COMPARATOR_INDEX);
        RTC_ClearCOMPEvent(RTC_COMPARATOR_INDEX);
    }
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

