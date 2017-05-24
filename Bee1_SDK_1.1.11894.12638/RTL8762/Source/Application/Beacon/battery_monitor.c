/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     battery_monitor.c
* @brief
* @details
* @author   hunter_shuai
* @date     2015-06-16
* @version  v0.1
*********************************************************************************************************
*/
#include "battery_monitor.h"
#include "rtl876x_rcc.h"
#include "rtl876x_adc.h"
#include "bee_message.h"
#include "os.h"
#include "trace.h"

void BatteryMonitor_Init(void)
{
    /* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

    /* ADC init */
    ADC_InitTypeDef adcInitStruct;

    ADC_StructInit(&adcInitStruct);

    /* use channel battery */
    adcInitStruct.channelMap = ADC_CH_BAT;//;
    ADC_Init(ADC, &adcInitStruct);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);

    return;
}




