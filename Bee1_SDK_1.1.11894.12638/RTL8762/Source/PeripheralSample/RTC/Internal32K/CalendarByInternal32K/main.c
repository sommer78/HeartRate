/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of to test internal 32K clock precision.
* @details  Please use special driver of rtl876x_rtc.c and rtl876x_rtc.h which  stored in file folder of RTCDriverForInternal32K. 
* @author   elliot chen
* @date     2016-03-08
* @version  v1.0
*********************************************************************************************************
*/

/* Defines -------------------------------------------------------------------*/
/* queue size & stak size & task priority */
#define IO_DEMO_EVENT_QUEUE_SIZE        0x10
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* Includes ------------------------------------------------------------------*/
#include "freeRTOS.h"
#include "timers.h"
#include "os.h"
#include "trace.h"
#include "calendar_internal32K.h"

/* Global variables ------------------------------------------------------------*/
/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoIOEventQueue;
/* definite variable for record current calendar */
CALENDAR calendar = {16, 3, 23, 14, 00, 50}; 

void RTC_Internal32KTestTask(void* param)
{
	/* Initialize calendar */
    Calendar_Init(&calendar);
	
	while(1)
	{
		/* Get current calendar */
		Calendar_GetCurrentValue(&calendar);
		DBG_BUFFER(MODULE_APP,LEVEL_INFO," Year = %d, month= %d, day = %d, hour = %d, minute = %d, second = %d!", 6, \
										calendar.year, calendar.month, calendar.day, calendar.hour, calendar.minute, calendar.second);
		vTaskDelay(1000 / portTICK_RATE_MS);;
	}
}

int main(void) 
{
    /* create io test task */
    xTaskCreate(RTC_Internal32KTestTask, "RTC_Internal32KTest", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(char));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(uint16_t));
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

