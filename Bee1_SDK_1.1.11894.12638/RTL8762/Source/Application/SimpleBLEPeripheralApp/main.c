/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    ADC auto sensor mode demo.
* @details  Auto sensor mode will do ADC convert continuously.This demo use auto sensor mode to detect 
*           battery level. Also, compare function is be used to detect low battery level(2v).
* @author   tifnan
* @date     2015-07-01
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_adc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "freeRTOS.h"
#include "timers.h"
#include "string.h"
#include "trace.h"
#include "board.h"
#include "lts1303m.h"
#include  "heart_rate_sample.h"



void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

#define TEST_Pin            P2_4
#define GPIO_TEST_Pin       GPIO_GetPin(TEST_Pin)
#define HRLED_Pin            P2_7
#define GPIO_HRLED_Pin      GPIO_GetPin(HRLED_Pin)
#define HR_ADC							P2_6




/* queue size & stak size & task priority */
#define HR_EVENT_QUEUE_SIZE        0x10
#define HR_MESSAGE_QUEUE_SIZE      0x10
#define HR_TASK_STACK_SIZE         1024
#define HR_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* task handle & queue handle */
xTaskHandle HRTaskHandle;
xQueueHandle HRMessageQueue;
xQueueHandle HREventQueue;
xTimerHandle ADC_CollectTimer = NULL;

uint8_t StringBuf[48];
char s;

extern IRQ_FUN UserIrqFunTable[32+17];



const HEART_RATE_PARAM_T HeartRateParam = 
{
		
	25,		  /**< 锯齿波判断标准初始为200 > */
	160,			 /**<   心率最大值 初始为160    > */
	40,				 /**<   心率最小值 初始为40    > */
	2,				 /**<   采样率 默认为2ms    > */	
	30000,			 /**<   所有的采样点 60*1000/2 > */	
	10,			 /**<  波形窗口采样 10> */	
	3500,		 /**<  心率采样最大值默认4000点> */	
	200,			 /**<  平整度最大值 默认为200> */	
	4000,			 /**<  取多少点进行算法> */	
	3,		 /**<  取多少个波形进行计算    默认3个> */	
	10,		 /**<  平整度超过多少判断为方波默认10个> */	
	188,	 /**<最大周期数 (60000/HeartRateMAX)/SampleRate> */		
	750,	 /**<最小周期数 (60000/HeartRateMIN)/SampleRate> */		
  100,	 /**< 有效波峰直接的间值    > */		
};


/* functions declaration */
void HR_DemoTask(void* param);

/* hex to Ascii */
uint8_t HexToAscii(uint8_t hex)
{
    if(hex <= 9)
    {
        return (hex - 0x00 + 0x30);
    }
    else if(hex >= 0x0A && hex <= 0x0F)
    {
        return (hex - 0x0A + 0x41);
    }
    else
    {
        /* wrong */
        return 0xFF;
    }  
}

/* return voltage (mV)  */
uint16_t HexToVoltage(uint16_t hex)
{
    if(hex < 0x2DF2)
    {
        return 0;
    }
    else
    {
        return (20 * ((hex - 0x2DF2) / 0x10A + 1));
    } 
}

/* mv to hex to set compare value */
uint16_t VlotageToHex(uint32_t vol)
{
    return ((vol / 20) * 0x10A + 0x2DF2);
}

int main(void) 
{
    /* create io test task */
    xTaskCreate(HR_DemoTask, "HR_Demo", HR_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, HR_TASK_PRIORITY, &HRTaskHandle);
    
    /* create event queue and message queue */
    HREventQueue = xQueueCreate(HR_EVENT_QUEUE_SIZE, sizeof(char));
    HRMessageQueue = xQueueCreate(HR_MESSAGE_QUEUE_SIZE, sizeof(uint16_t));
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux */
    Pinmux_Config(P3_0, DATA_UART_TX);
    Pinmux_Config(P3_1, DATA_UART_RX);
    Pinmux_Config(TEST_Pin, GPIO_FUN);
	Pinmux_Config(HRLED_Pin, GPIO_FUN);
    return;
}

void PAD_Configuration(void)
{
    /* uart pad config */
    Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TEST_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(HRLED_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(HR_ADC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
 
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    /* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
	
    /* Enable GPIO clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	/* Initialize GPIO */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin  = GPIO_TEST_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin  = GPIO_HRLED_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
    return; 
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* ADC IRQ */  
    NVIC_InitStruct.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);
    
    /* UART IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);
    
    return;
}

/* adc value read timer timeout */
void ADC_CollectTimeout(xTimerHandle handle)
{
    uint16_t adcValue = 0;
    uint8_t event = HR_EVENT_TIMER_TIMEOUT;
    
    /* read battery value(hex) */
   adcValue = ADC_Read(ADC, HR_ADC);
    
  //  DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Battery level hex is: 0x%x", 1, adcValue);
    
    /* send convert result to task*/
    xQueueSend(HRMessageQueue, &adcValue, 0);
    /* send event to HR task */
    xQueueSend(HREventQueue, &event, 0);
    
    /* reset timer */
    xTimerReset(ADC_CollectTimer, 0);
    
    return;
}


void hrTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_InitStruct;

    TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_32KHZ;
	TIM_InitStruct.TIM_Period =  16*5; //2.5ms
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM4, &TIM_InitStruct);
    
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIMER4_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_ClearINT(TIM4);
    TIM_INTConfig(TIM4,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}



void heartRateSubroutine(){
		int i = 0;	
	    HRState bpmState;
		uint16_t heartRate;

		heartRateInit();
	
		
		for(i=0;i<4000;i++){
		
		bpmState = 	 heartRateWaveDetect(testWave_TYPE3[i]);  
			
		if(bpmState==HRFinish){
		heartRate =	getHeartRateFilter();
		sprintf( StringBuf, "HeartRate=%d\r\n",heartRate);
		UART_SendData(UART, StringBuf, 16);	
		heartRateInit();
		
		//
		}
		if(bpmState==HRPointMax){
		heartRateInit();
			}
		if(bpmState==HRLineOut){
			sprintf( StringBuf, "LineOut=%d\r\n",evennessValue);
		UART_SendData(UART, StringBuf, 16);	
		heartRateClrRam();
			}
		
	}

}


int count ;
void HR_DemoTask(void* param)
{

    uint8_t event = 0;
    uint16_t adcConvertResult = 0;
	int len;
	count = 0;
   // uint16_t voltage = 0;   //mv
    
  //  char* demoTipStr = "### ADC auto sensor mode demo ###\r\n";
 //   char* demoAdcResStr = "00000,";
   // char* demoBatLowStr = "## Battery level is low!! ##\r\n";
    
    /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();

	/* Output high */
	GPIO_SetBits(GPIO_HRLED_Pin);
    
    /******************* uart init *****************************/
    UART_InitTypeDef uartInitStruct;
    
    UART_StructInit(&uartInitStruct);
    
    /* default 115200 baudrate */
    UART_Init(UART, &uartInitStruct);
    
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
 
    /******************** ADC init ******************************/
    ADC_InitTypeDef adcInitStruct;
    
    ADC_StructInit(&adcInitStruct);
    
    /* use battery channel & auto sensor mode */
    adcInitStruct.channelMap = ADC_CH6;
    adcInitStruct.adcMode = ADC_One_Shot_Mode;
    ADC_Init(ADC, &adcInitStruct);
	//ADC_SetFifoDataEndian(ADC, ADC_FIFO_LITTLE_ENDIAN);
    /* set compare threshold value(2000mv) */
   // ADC_SetCmpThd(ADC, ADC_CH_BAT, VlotageToHex(2000));
   // ADC_SetCmpConfig(ADC, ADC_CH_BAT, ADC_CMP_CFG_SMALLER, ADC_Cmp_Fifo_Disable);
    /* enable battery channel compare interrupt */
    // ADC_INTConfig(ADC, ADC_INT_FIFO_FULL, ENABLE);
 //   ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
    /* enable auto sensor mode */
 //   ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
    
    /* create sw timer, 2s */
  //  ADC_CollectTimer = xTimerCreate((const char*)"InactiveTimer",\
   //                     10 / portTICK_RATE_MS, pdFALSE, NULL, ADC_CollectTimeout);
  //  xTimerStart(ADC_CollectTimer, 0);
   // hrTimerInit();
		heartRateParamSetup(HeartRateParam);
		clrHeartRateStack();
		heartRateInit();
  while(1){
  		
		heartRateSubroutine();
	}



    /************************** start task while loop **********************/
    while(1)
    {
        while(xQueueReceive(HREventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            /* adc read timer timeout event */
            if (event == HR_EVENT_TIMER_TIMEOUT) 
            {
                /* voltage value(hex) */
                if(xQueueReceive(HRMessageQueue, &adcConvertResult, 0) == pdPASS)
                {
                #if 1
                if(s%2==0){
					GPIO_SetBits(GPIO_TEST_Pin);
					}else {
					GPIO_ResetBits(GPIO_TEST_Pin);
					
					}
					s++;
				#endif
                

				
				}
				
				
				
             
   
                   
               }
           
		 /* adc convert end event */
            if (event == HR_EVENT_ADC_CONVERT_END) 
            {
                /* adc message */
                while (xQueueReceive(HRMessageQueue, &adcConvertResult, 0) == pdPASS)
                {
					
					
			
			
			if(count>8000&&count<8001){
				len = sprintf( StringBuf, "%d,",adcConvertResult);
				UART_SendData(UART, StringBuf, len);
				if(count%16 == 0){
				len = sprintf( StringBuf, "\r\n");
				UART_SendData(UART, StringBuf, len);
				}
			
				
			}
			count++;
                 /* start adc convert again */
                 ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
                 ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
                }
            
            }
			
            #if 0
            /* battery level low even */
            if(event == HR_EVENT_BATTERY_LEVEL_LOW)
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ADC battery level low", 0);
                /* stop battery level detect timer */
                xTimerStop(ADC_CollectTimer, 0);
                /* disable auto sensor mode */
                ADC_Cmd(ADC, ADC_Auto_Sensor_Mode, DISABLE);
                
                 /* send message through uart */
                demoStrLen = strlen(demoBatLowStr);  
                memcpy(StringBuf, demoBatLowStr, demoStrLen);

                blkCount = demoStrLen / UART_TX_FIFO_SIZE;
                remainder = demoStrLen % UART_TX_FIFO_SIZE;

                for(i = 0; i < blkCount; i++)
                {
                    /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
                    UART_SendData(UART, &StringBuf[16 * i], 16);
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                }

                /* send left bytes */
                UART_SendData(UART, &StringBuf[16 * i], remainder);
                /* wait tx fifo empty */
                while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
            }
			#endif
        }
    }
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}

