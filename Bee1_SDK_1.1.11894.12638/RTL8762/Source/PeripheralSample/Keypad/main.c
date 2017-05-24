/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    Keypad demo.
* @details
* @author   tifnan
* @date     2015-06-26
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_keyscan.h"
#include "rtl876x_rcc.h"
#include "board.h"
#include "keypad.h"
#include "timers.h" //freeRTOS Timer
#include "string.h"
#include "trace.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

/* queue size & stak size & task priority */
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;

/* global */
uint8_t StringBuf[100];
uint8_t RepeatReport = 0;
KEYSCAN_DATA_STRUCT  CurKeyData;
KEYSCAN_DATA_STRUCT  PreKeyData;
xTimerHandle KeyScanTimer = NULL;

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

int main(void) 
{
    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(KeyscanMsg));
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux */
    Pinmux_Config(UART_TX_PIN, DATA_UART_TX);
    Pinmux_Config(UART_RX_PIN, DATA_UART_RX);
    
    /* keypad pinmux config */
    Pinmux_Config(ROW0, KEY_ROW_0);
    Pinmux_Config(ROW1, KEY_ROW_1);
    Pinmux_Config(COLUMN0, KEY_COL_0);
    Pinmux_Config(COLUMN1, KEY_COL_1);
    
    return;
}

void PAD_Configuration(void)
{
    /* uart pad config */
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    /* Keypad pad config */
    Pad_Config(ROW0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(ROW1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(COLUMN0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(COLUMN1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    /* turn on Keyscan clock */
    RCC_PeriphClockCmd(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, ENABLE);
    
    return; 
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Keyscan IRQ */  
    NVIC_InitStruct.NVIC_IRQChannel = KEYSCAN_IRQ;
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

void KeyScanTimerCallback(xTimerHandle xTimer)
{
    memset(&CurKeyData, 0, sizeof(KEYSCAN_DATA_STRUCT));
    memset(&PreKeyData, 0, sizeof(KEYSCAN_DATA_STRUCT));

    KeyscanMsg msg;
    msg.msgType = IO_DEMO_EVENT_KEY_RELEASE;
    msg.pBuf = NULL;
    
    xQueueSend(DemoIOMessageQueue, &msg, 0);
    
    return;
}

void KeyPadInit(void)
{  
    uint32_t keyTimeoutPeriod = 0;
    
    /* data struct init */
    memset(&CurKeyData, 0 ,sizeof(KEYSCAN_DATA_STRUCT));
    memset(&PreKeyData, 0 ,sizeof(KEYSCAN_DATA_STRUCT));
    
    KEYSCAN_InitTypeDef keyScanInitStruct;
    KeyScan_StructInit(KEYSCAN, &keyScanInitStruct);
    
    keyScanInitStruct.debounceTime    = (16 * 32); //16ms
    keyScanInitStruct.rowSize = KEYPAD_ROW_SIZE;
    keyScanInitStruct.colSize = KEYPAD_COLUMN_SIZE;
    
    KeyScan_Init(KEYSCAN, &keyScanInitStruct);
    KeyScan_INTConfig(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
    KeyScan_Cmd(KEYSCAN, ENABLE);

    if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_12_5ms)
    {
        keyTimeoutPeriod += 12;
    }
    else if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_25ms)
    {
        keyTimeoutPeriod += 25;
    }
    else if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_50ms)
    {
        keyTimeoutPeriod += 50;
    }
    else if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_100ms)
    {
        keyTimeoutPeriod += 100;
    }
    else
    {
        //wrong!!
    }
    
    
    
    RepeatReport = FALSE;
    
    keyTimeoutPeriod += (keyScanInitStruct.debounceTime / 32 + 10);

    if (KeyScanTimer == NULL)
    {
        KeyScanTimer = xTimerCreate("KeyScanTimer",                      /* Just a text name, not used by the kernel.    */
                                        (keyTimeoutPeriod / portTICK_RATE_MS), /* The timer period in ticks. KeyTimeoutPeriod = Interval_time +Debounce_time +2ms  */
                                        pdFAIL,                               /* The timers will auto-reload themselves when they expire. */
                                        (void *)1,                            /* Assign each timer a unique id equal to its array index.  */
                                        KeyScanTimerCallback                 /* Each timer calls the same callback when it expires.  */
                                       );
        if (KeyScanTimer == NULL)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, "*** ceate keyscan timer fail", 0);
        }
    }
    
    return;
}

void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t demoStrLen = 0;
    uint8_t i = 0;
    uint8_t event = 0;
    KeyscanMsg msg;
    
    char* demoTipStr = "### Bee keypad demo ###\r\n";
    char* demoOneKeyStr = "## One key press:(0, 0) ##\r\n";
    char* demoTwoKeyStr = "## Two keys press:(0, 0),(0, 0) ##\r\n";
    char* demoKeyReleaseStr = "## Key Release ##\r\n";
    
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
    
    /******************* keypad init *****************************/
    KeyPadInit();
    /********************send demo tips through uart *************/
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
        /* key value update message */
        while(xQueueReceive(DemoIOMessageQueue, &msg, portMAX_DELAY) == pdPASS)
        {
            if(msg.msgType == IO_DEMO_EVENT_KEY_VALUE_UPDATE)
            {
                PKEYSCAN_DATA_STRUCT pKeyData = (PKEYSCAN_DATA_STRUCT)msg.pBuf;
                /* signal key press */
                if(pKeyData->length == 1)
                {
                    demoStrLen = strlen(demoOneKeyStr);
                    memcpy(StringBuf, demoOneKeyStr, demoStrLen);
                    StringBuf[18] = HexToAscii(pKeyData->key[0].row);
                    StringBuf[21] = HexToAscii(pKeyData->key[0].column);
                    
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "--->signal press: position: (%d, %d)", 2,
                            pKeyData->key[0].row, pKeyData->key[0].column);  
                }
                
                /* two keys press */
                if(pKeyData->length == 2)
                {
                    demoStrLen = strlen(demoTwoKeyStr);
                    memcpy(StringBuf, demoTwoKeyStr, demoStrLen);
                    StringBuf[19] = HexToAscii(pKeyData->key[0].row);
                    StringBuf[22] = HexToAscii(pKeyData->key[0].column);
                    StringBuf[26] = HexToAscii(pKeyData->key[1].row);
                    StringBuf[29] = HexToAscii(pKeyData->key[1].column);
                    
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "--->double key press: position: (%d, %d), (%d, %d)", 4,\
                        pKeyData->key[0].row, pKeyData->key[0].column,\
                        pKeyData->key[1].row, pKeyData->key[1].column);
                     
                }
            }                
            else if(msg.msgType == IO_DEMO_EVENT_KEY_RELEASE)
            {
                if(msg.msgType == IO_DEMO_EVENT_KEY_RELEASE)
                {
                    demoStrLen = strlen(demoKeyReleaseStr);
                    memcpy(StringBuf, demoKeyReleaseStr, demoStrLen);
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "--->Key Release!!", 0);
                }
            }
            else
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "--->wrong key event!!", 0);
            }
        
            /* send key value through uart */
            blkCount = demoStrLen / 16;
            remainder =  demoStrLen % 16;

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

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
