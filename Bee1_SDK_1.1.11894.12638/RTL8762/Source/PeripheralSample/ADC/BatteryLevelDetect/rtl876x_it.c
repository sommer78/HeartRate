/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		rtl876x_it.c
* @brief	Battery detect demo.
* @details
* @author	tifnan
* @date 	2015-07-02
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
    uint8_t event = IO_DEMO_EVENT_ADC_CONVERT_DONE;
    uint16_t adcConvertRes = 0;
    portBASE_TYPE TaskWoken = pdFALSE;
    
    /* one shot mode: adc convert done */
    if(ADC_GetIntFlagStatus(ADC, ADC_INT_ONE_SHOT_DONE) == SET)
    {
        ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
        
        /* read ADC convert result*/
        adcConvertRes = ADC_Read(ADC, ADC_CH_BAT);
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Battery level(hex) is: 0x%x", 1, adcConvertRes);
        /* send convert result to task*/
        xQueueSendFromISR(DemoIOMessageQueue, &adcConvertRes, &TaskWoken);
        /* send event to DemoIO task */
        xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);  
    }
    
    /* adc battery level compared interrupt */
    if(ADC_GetIntFlagStatus(ADC, ADC_INT_CMP_BAT) == SET)
    {
        ADC_ClearINTPendingBit(ADC, ADC_INT_CMP_BAT);
        
        /* disable battery channel compare interrupt */
        ADC_INTConfig(ADC, ADC_INT_CMP_BAT, DISABLE);
        
        /* send event to DemoIO task */
        event = IO_DEMO_EVENT_BATTERY_LEVEL_LOW;
        xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
       
    }
    
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}
