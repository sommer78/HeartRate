/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    Adc demo(one shot mode)
* @details
* @author   tifnan
* @date     2015-06-01
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_adc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "string.h"
#include "trace.h"
#include "board.h"


void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

/* queue size & stak size & task priority */
#define IO_DEMO_EVENT_QUEUE_SIZE        0x10
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoIOEventQueue;

uint8_t StringBuf[48];

extern IRQ_FUN UserIrqFunTable[32+17];

/* functions declaration */
void IO_DemoTask(void* param);


#define GPIO_Output_Pin       GPIO_GetPin(P2_7)

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

int main(void) 
{
    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(char));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(uint16_t));
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux */
    Pinmux_Config(P3_0, DATA_UART_TX);
    Pinmux_Config(P3_1, DATA_UART_RX);
	
	Pinmux_Config(P2_7, GPIO_FUN);
    
    return;
}

void PAD_Configuration(void)
{
    /* uart pad config */
    Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    /* ADC pad config */
    Pad_Config(P2_6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
	
	Pad_Config(P2_7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

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
	GPIO_InitStruct.GPIO_Pin  =  GPIO_Output_Pin;
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
uint16_t t;
void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t demoStrLen = 0;
    uint8_t i = 0;
    uint8_t event = 0;
    uint16_t adcConvertResult = 0;
    
    char* demoTipStr = "### ADC demo(One shot mode/DLPS OFF) ###\r\n";
    char* demoAdcResStr = "## ADC convert result is:0x0000 ##\r\n";
    
    /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /******************* uart init *****************************/
    UART_InitTypeDef uartInitStruct;
    
    UART_StructInit(&uartInitStruct);
    
    /* default 115200 baudrate */
    UART_Init(UART, &uartInitStruct);
    
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
 
    /******************** ADC init ******************************/
    ADC_InitTypeDef adcInitStruct;
    
    ADC_StructInit(&adcInitStruct);
    
    /* use channel 4 */
    adcInitStruct.channelMap = ADC_CH6;
    ADC_Init(ADC, &adcInitStruct);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
    ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);

    /********************send demo tips through uart *****************/
    demoStrLen = strlen(demoTipStr);       
    memcpy(StringBuf, demoTipStr, demoStrLen);
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
    
    /* prepare buffer for print ADC convert result */
    demoStrLen = strlen(demoAdcResStr);  
    memcpy(StringBuf, demoAdcResStr, demoStrLen);
    
    /************************** start task while loop **********************/
    while(1)
    {
        if (xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            /* adc convert end event */
            if (event == IO_DEMO_EVENT_ADC_CONVERT_END) 
            {
                /* adc message */
                while (xQueueReceive(DemoIOMessageQueue, &adcConvertResult, 0) == pdPASS)
                {
					
					
		if(t%2==0){
		GPIO_SetBits(GPIO_Output_Pin);
		}else {
		GPIO_ResetBits(GPIO_Output_Pin);
			}
			t++;
					#if 0
                    /* hex to Ascii */
                    StringBuf[27] = HexToAscii((adcConvertResult & 0xf000) >> 12);
                    StringBuf[28] = HexToAscii((adcConvertResult & 0x0f00) >> 8);
                    StringBuf[29] = HexToAscii((adcConvertResult & 0x00f0) >> 4);
                    StringBuf[30] = HexToAscii((adcConvertResult & 0x000f) >> 0);
                    
                    /* send through uart */
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
                    #endif
                    /* delay 2s */
                //    vTaskDelay(2000 / portTICK_RATE_MS);
                    
                    /* start adc convert again */
                    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
                    ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
                }
            }
        }
    }
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
