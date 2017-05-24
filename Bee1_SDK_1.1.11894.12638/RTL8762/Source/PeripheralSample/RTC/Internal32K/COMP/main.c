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

/* use comparator 0 */
#define RTC_COMPARATOR_INDEX 		1
#define ABS_PARAMS(a,b) 			((a>b) ? (a-b):(b-a))

/* Includes ------------------------------------------------------------------*/
#include "freeRTOS.h"
#include "timers.h"
#include "os.h"
#include "gap.h"
#include "blueapi.h"
#include "legacyperiphapi.h"
#include "trace.h"
#include "rtl876x_rtc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/* Global variables ------------------------------------------------------------*/

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoIOEventQueue;

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

void RTC_Initconfig(void)
{
	blueAPI_SetRTCInternal32k(ENABLE);
    /* initialize RTC*/
    RTC_ResetForInternal32K();
    RTC_Enable();
    RTC_SetPrescaler(0);
    RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, 32000);//320000+3200*5
    RTC_EnableCOMP(RTC_COMPARATOR_INDEX);
    RTC_EnableSystemWakeup();
    RTC_INTConfig(RTC, RTC_INT_CMP1, ENABLE);

    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    RTC_Start();
}



void RTC_Internal32KTestTask(void* param)
{
	/* Initialize RTC */
    RTC_Initconfig();
	while(1)
	{
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
#if 1
        if(count >= 10)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "RTC params: %d!\n\n", 1, count);
            //count = 0;
        }
#endif        
        RTC_ClearCOMPEvent(RTC_COMPARATOR_INDEX);
    }
}
