/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    Q-decoder demo(for mouse application ref).
* @details
* @author   tifnan
* @date     2015-06-26
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_qdec.h"
#include "rtl876x_rcc.h"
#include "board.h"
#include "qdec.h"
#include "string.h"
#include "trace.h"

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

/* global */
uint8_t StringBuf[100];

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
    
    /* create message queue */
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(QdecMsg));
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux */
    Pinmux_Config(UART_TX_PIN, DATA_UART_TX);
    Pinmux_Config(UART_RX_PIN, DATA_UART_RX);
    
    /* Qdecoder pinmux config */
    Pinmux_Config(QDEC_Y_PHA_PIN, qdec_phase_a_y);
    Pinmux_Config(QDEC_Y_PHB_PIN, qdec_phase_b_y);
    
    return;
} 

void PAD_Configuration(void)
{
    /* uart pad config */
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    /* Qdecoder pad config */
    Pad_Config(QDEC_Y_PHA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(QDEC_Y_PHB_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    /* turn on Qdecoder clock */
    RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);
    
    return; 
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Keyscan IRQ */  
    NVIC_InitStruct.NVIC_IRQChannel = QDECODE_IRQ;
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

void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t demoStrLen = 0;
    uint8_t i = 0;
    QdecMsg msg;
    int16_t delta = 0;
    int16_t y_axis = 0;
    
    char* demoTipStr = "### Bee Q-decoder demo ###\r\n";
    char* demoDeltaStr = "## Delta value is: +00 ##\r\n";
    
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
    
    /******************* Qdecoder init *****************************/
    QDEC_InitTypeDef qdecInitStruct;
    
    QDEC_StructInit(&qdecInitStruct);
    qdecInitStruct.sampleRate = 10000;  //hz
    QDEC_Init(QDEC, &qdecInitStruct);
    QDEC_INTConfig(QDEC, QDEC_INT_NEW_DATA, ENABLE);
    
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
    
    /* prepare for uart print q-decoder delta value */
    demoStrLen = strlen(demoDeltaStr);
    memcpy(StringBuf, demoDeltaStr, demoStrLen);
    
    /************************** start task while loop **********************/
    while(1)
    {
        /* Q-decoder message */
        while(xQueueReceive(DemoIOMessageQueue, &msg, portMAX_DELAY) == pdPASS)
        { 
            if(msg.msgType == IO_DEMO_EVENT_QDEC_VALUE_UPDATE)
            {
                delta = (int16_t)msg.parm - y_axis;
                y_axis = (int16_t)msg.parm;
                
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "--->qdec-delta value is: %d, y_axis = %d", 2, delta, y_axis);
                
                if(delta >= 0)
                {
                    delta*= 10;         /* scale 10 times */
                    StringBuf[19] = '+';
                    StringBuf[20] = HexToAscii(delta / 10);
                    StringBuf[21] = HexToAscii(delta % 10);
                }
                else
                {
                    delta = -delta;
                    delta*= 10;         /* scale 10 times */
                    StringBuf[19] = '-';
                    StringBuf[20] = HexToAscii(delta / 10);
                    StringBuf[21] = HexToAscii(delta % 10);
                }   
            }
            else
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "--->Unknown qdecoder message", 0);
            }
            
            /* send delta value through uart */
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

