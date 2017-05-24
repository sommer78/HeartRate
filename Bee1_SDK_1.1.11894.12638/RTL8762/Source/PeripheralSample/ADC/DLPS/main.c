/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    ADC demo(dlps on)
* @details
* @author   tifnan
* @date     2015-06-03
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "rtl876x_adc.h"
#include "rtl876x_io_dlps.h"
#include "dlps_platform.h"
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

extern IRQ_FUN UserIrqFunTable[32+17];
void IO_DemoTask(void* param);
void ADCIntrHandler(void);

uint8_t DemoStrLen = 0;
uint32_t DlpsCount = 0;
uint8_t CanEnterDlps = 1;
uint8_t TxBuf[100];

/* hex to Ascii*/
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

BOOL DlpsEnterCheck(void)
{
    if(CanEnterDlps)
    {
        return TRUE;
    }
    else
    {  
        return FALSE;
    }
}

void DlpsExitCallback(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DlpsExitCallback", 0);
    
    uint8_t event = IO_DEMO_EVENT_SYSTEM_WAKE;
    
    xQueueSend(DemoIOEventQueue, &event, 0);
    
    CanEnterDlps = 0;

    return;
}

int main(void) 
{
    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
   /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(char));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(uint16_t));
    
    /* enable dlps and register dlps callback function */
    DLPS_IO_Register();
    //  LPS_MODE_Pause();
    DLPS_IO_RegUserDlpsExitCb(DlpsExitCallback);
    /* register enter dlps check function */
    DLPS_ENTER_CHECK_CB_REG(DlpsEnterCheck);
    /* enable dlps */
    LPS_MODE_Set(LPM_DLPS_MODE);
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux, ADC use p2.4, no pinmux */
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
    /* turn on UART and ADC clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
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
    
    return;
}

void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t i = 0;
    uint16_t adcConvertResult = 0;
    uint8_t event = 0;

    char* demoStr = "### ADC Demo (One shot mode/DLPS ON) ###\r\n";
    char* demoStr1 = "## Exit Dlps: 0000st ##, ADC convert value is: 0x0000\r\n";
    
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
    
    /* default 115200 baudrate */
    UART_StructInit(&uartInitStruct);
    
    UART_Init(UART, &uartInitStruct);
    
    /******************** ADC init ******************************/
    UserIrqFunTable[ADC_IRQ]                = (IRQ_FUN)ADCIntrHandler;
    ADC_InitTypeDef adcInitStruct;
    
    ADC_StructInit(&adcInitStruct);
    
    adcInitStruct.channelMap = ADC_CH4; /* use channel 4 */
    ADC_Init(ADC, &adcInitStruct);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
    
    /* send tips through uart */
    DemoStrLen = strlen(demoStr);       
    memcpy(TxBuf, demoStr, DemoStrLen);
    blkCount = DemoStrLen / UART_TX_FIFO_SIZE;
    remainder = DemoStrLen % UART_TX_FIFO_SIZE;
    
    for(i = 0; i < blkCount; i++)
    {
        /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
        UART_SendData(UART, &TxBuf[16 * i], 16);
        /* wait tx fifo empty */
        while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    }
    
    /* send left bytes */
    UART_SendData(UART, &TxBuf[16 * i], remainder);
    /* wait tx fifo empty */
    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    
    DemoStrLen = strlen(demoStr1); 
    memset(TxBuf, 0, sizeof(TxBuf));    
    memcpy(TxBuf, demoStr1, DemoStrLen);
    
    while(1)
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "task while(1)", 0);
        if (xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "event received", 0);
            
            /* adc convert end event */
            if (event == IO_DEMO_EVENT_ADC_CONVERT_END) 
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ADC convert end event received", 0);
                
                /* adc message */
                while (xQueueReceive(DemoIOMessageQueue, &adcConvertResult, 0) == pdPASS)
                {
                    DlpsCount++;
                    
                    /* hex to Ascii */
                    TxBuf[14] = HexToAscii(DlpsCount / 1000);
                    TxBuf[15] = HexToAscii((DlpsCount / 100) % 10);
                    TxBuf[16] = HexToAscii((DlpsCount / 10) % 100 % 10);
                    TxBuf[17] = HexToAscii(DlpsCount % 1000 % 100 % 10);
                    TxBuf[49] = HexToAscii((adcConvertResult & 0xf000) >> 12);
                    TxBuf[50] = HexToAscii((adcConvertResult & 0x0f00) >> 8);
                    TxBuf[51] = HexToAscii((adcConvertResult & 0x00f0) >> 4);
                    TxBuf[52] = HexToAscii((adcConvertResult & 0x000f) >> 0);
                    
                    /* send through uart */
                    blkCount = DemoStrLen / UART_TX_FIFO_SIZE;
                    remainder = DemoStrLen % UART_TX_FIFO_SIZE;

                    for(i = 0; i < blkCount; i++)
                    {
                        /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
                        UART_SendData(UART, &TxBuf[16 * i], 16);
                        /* wait tx fifo empty */
                        while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                    }

                    /* send left bytes */
                    UART_SendData(UART, &TxBuf[16 * i], remainder);
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                    
                    DBG_DIRECT("It is to enter dlps");
                    DBG_DIRECT("It is to enter dlps");
                    /* can enter dlps again */
                    CanEnterDlps = 1;
                }
            }
            
            /* system is wake up */
            if (event == IO_DEMO_EVENT_SYSTEM_WAKE)
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "System wakeup event received", 0);
                 /* start ADC convert again */
                ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
                ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
            }
        } 
    }
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
