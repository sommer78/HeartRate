/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		rtl876x_it.c
* @brief	adc demo--one shot mode
* @details
* @author	tifnan
* @date 	2015-06-01
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
    uint8_t event = IO_DEMO_EVENT_ADC_CONVERT_END;
    portBASE_TYPE TaskWoken = pdFALSE;
    uint16_t adcConvertRes = 0;
	
	ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
    
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
    
    /* read ADC convert result*/
    adcConvertRes = ADC_Read(ADC, ADC_CH4);
  //  DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ADC convert result = 0x%x", 1, adcConvertRes);
    /* send convert result to task*/
    xQueueSendFromISR(DemoIOMessageQueue, &adcConvertRes, &TaskWoken);
    /* send event to DemoIO task */
    xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
    
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}
