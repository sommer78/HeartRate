/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		rtl876x_it.c
* @brief	Auto sensor mode demo.
* @details
* @author	tifnan
* @date 	2015-07-01
* @version	v0.1
*********************************************************************************************************
*/

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "rtl876x_adc.h"
#include "board.h"
#include "trace.h"

void ADCIntrHandler(void)
{
    uint8_t event = IO_DEMO_EVENT_BATTERY_LEVEL_LOW;
    portBASE_TYPE TaskWoken = pdFALSE;
    
    /* adc battery level compared interrupt */
    if(ADC_GetIntFlagStatus(ADC, ADC_INT_CMP_BAT) == SET)
    {
        ADC_ClearINTPendingBit(ADC, ADC_INT_CMP_BAT);
        
        /* disable battery channel compare interrupt */
        ADC_INTConfig(ADC, ADC_INT_CMP_BAT, DISABLE);
        
        /* send event to DemoIO task */
        xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
       
    }
    
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}
