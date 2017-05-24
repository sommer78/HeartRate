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
#include "freeRTOS.h"
#include "timers.h"
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
xTimerHandle ADC_CollectTimer = NULL;

uint8_t StringBuf[48];

extern IRQ_FUN UserIrqFunTable[32+17];

/* functions declaration */
void IO_DemoTask(void* param);

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
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    
    return;
}

void PAD_Configuration(void)
{
    /* uart pad config */
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    /* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
    
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
    uint8_t event = IO_DEMO_EVENT_TIMER_TIMEOUT;
    
    /* read battery value(hex) */
    adcValue = ADC_Read(ADC, ADC_CH_BAT);
    
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Battery level hex is: 0x%x", 1, adcValue);
    
    /* send convert result to task*/
    xQueueSend(DemoIOMessageQueue, &adcValue, 0);
    /* send event to DemoIO task */
    xQueueSend(DemoIOEventQueue, &event, 0);
    
    /* reset timer */
    xTimerReset(ADC_CollectTimer, 0);
    
    return;
}

void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t demoStrLen = 0;
    uint8_t i = 0;
    uint8_t event = 0;
    uint16_t adcConvertResult = 0;
    uint16_t voltage = 0;   //mv
    
    char* demoTipStr = "### ADC auto sensor mode demo ###\r\n";
    char* demoAdcResStr = "## Battery Level is: 0.00V ##\r\n";
    char* demoBatLowStr = "## Battery level is low!! ##\r\n";
    
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
    
    /* use battery channel & auto sensor mode */
    adcInitStruct.channelMap = ADC_CH_BAT;
    adcInitStruct.adcMode = ADC_Auto_Sensor_Mode;
    ADC_Init(ADC, &adcInitStruct);
    /* set compare threshold value(2000mv) */
    ADC_SetCmpThd(ADC, ADC_CH_BAT, VlotageToHex(2000));
    ADC_SetCmpConfig(ADC, ADC_CH_BAT, ADC_CMP_CFG_SMALLER, ADC_Cmp_Fifo_Disable);
    /* enable battery channel compare interrupt */
    ADC_INTConfig(ADC, ADC_INT_CMP_BAT, ENABLE);
    /* enable auto sensor mode */
    ADC_Cmd(ADC, ADC_Auto_Sensor_Mode, ENABLE);
    
    /* create sw timer, 2s */
    ADC_CollectTimer = xTimerCreate((const char*)"InactiveTimer",\
                        20000 / portTICK_RATE_MS, pdFALSE, NULL, ADC_CollectTimeout);
    xTimerStart(ADC_CollectTimer, 0);
    
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

    /************************** start task while loop **********************/
    while(1)
    {
        while(xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            /* adc read timer timeout event */
            if (event == IO_DEMO_EVENT_TIMER_TIMEOUT) 
            {
                /* voltage value(hex) */
                if(xQueueReceive(DemoIOMessageQueue, &adcConvertResult, 0) == pdPASS)
                {
                    /* hex to real voltage */
                    voltage =  HexToVoltage(adcConvertResult);
                    
                    /* prepare buffer for print battery level */
                    demoStrLen = strlen(demoAdcResStr);  
                    memcpy(StringBuf, demoAdcResStr, demoStrLen);
                    
                    /* hex to Ascii */
                    StringBuf[21] = HexToAscii(voltage / 1000);
                    StringBuf[23] = HexToAscii((voltage / 100) % 10);
                    StringBuf[24] = HexToAscii((voltage / 10) % 100 % 10);
                    
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
                }
            }
            
            /* battery level low even */
            if(event == IO_DEMO_EVENT_BATTERY_LEVEL_LOW)
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
        }
    }
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}

