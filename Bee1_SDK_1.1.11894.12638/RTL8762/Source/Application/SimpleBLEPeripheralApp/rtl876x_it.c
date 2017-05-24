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

#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"





#define GPIO_SHOW_Pin      GPIO_GetPin(P2_4)
#define hrTimerHandle									Timer4IntrHandler




char StrBuffer[16];



void ADCIntrHandler(void)
{
#if 0
    uint8_t event = IO_DEMO_EVENT_ADC_CONVERT_END;
    portBASE_TYPE TaskWoken = pdFALSE;
    uint16_t adcConvertRes = 0;
	
	ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
    
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
    
    /* read ADC convert result*/
    adcConvertRes = ADC_Read(ADC, ADC_CH4);
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ADC convert result = 0x%x", 1, adcConvertRes);
    /* send convert result to task*/
    xQueueSendFromISR(DemoIOMessageQueue, &adcConvertRes, &TaskWoken);
    /* send event to DemoIO task */
    xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
    
    portEND_SWITCHING_ISR(TaskWoken);


	uint8_t event = HR_EVENT_BATTERY_LEVEL_LOW;
    portBASE_TYPE TaskWoken = pdFALSE;
    
    /* adc battery level compared interrupt */
    if(ADC_GetIntFlagStatus(ADC, ADC_INT_CMP_BAT) == SET)
    {
        ADC_ClearINTPendingBit(ADC, ADC_INT_CMP_BAT);
        
        /* disable battery channel compare interrupt */
        ADC_INTConfig(ADC, ADC_INT_CMP_BAT, DISABLE);
        
        /* send event to DemoIO task */
        xQueueSendFromISR(HREventQueue, &event, &TaskWoken);
       
    }
    
    portEND_SWITCHING_ISR(TaskWoken);
#endif

     uint8_t event = HR_EVENT_ADC_CONVERT_END;
    portBASE_TYPE TaskWoken = pdFALSE;
    uint16_t adcConvertRes = 0;
	
	ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
    
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
    
    /* read ADC convert result*/
    adcConvertRes = ADC_Read(ADC, ADC_CH6);
   // DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ADC convert result = 0x%x", 1, adcConvertRes);
    /* send convert result to task*/
    xQueueSendFromISR(HRMessageQueue, &adcConvertRes, &TaskWoken);
    /* send event to DemoIO task */
    xQueueSendFromISR(HREventQueue, &event, &TaskWoken);
    
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}

int K;
extern int count;
void hrTimerHandle()
{
//	HRState bpmState;
//	uint16_t heartRate ;
//	uint32_t ret ;
uint16_t adcTestValue ;
int len;
	TIM_ClearINT(TIM4);
    if(K%2==0){
		GPIO_SetBits(GPIO_SHOW_Pin);
		}else {
		GPIO_ResetBits(GPIO_SHOW_Pin);
			}
		K++;
	adcTestValue =  	 ADC_Read(ADC, P2_6);
	#if 1
	//test_hr_scan();
	
			if(count<12000&&count>8000){
			
			len = sprintf( StrBuffer, "%d,",adcTestValue);
			UART_SendData(UART, StrBuffer, len);
				if(count%16 == 0){
				len = sprintf( StrBuffer, "\r\n");
				UART_SendData(UART, StrBuffer, len);
				}
				
			}
			count++;
	#endif
	
		
		//bpmState =	 heartRateWaveDetect(adcTestValue);  
		//if(bpmState==HRFinish){
	//		heartRate = getHeartRateFilter();
		//	sprintf( StrBuffer, "Count[%d]=%d\r\n",count,heartRate);
		//	UART_SendData(UART, StrBuffer, 16);
		//	count++;
		//	heartRateInit();
					
		//	}
//	testadcbuf[i++] = adcTestValue;
//	adcTestValue = 0;
			
}
