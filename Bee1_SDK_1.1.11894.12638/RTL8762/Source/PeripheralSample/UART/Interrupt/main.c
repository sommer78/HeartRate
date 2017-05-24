/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    uart demo--interrupt
* @details
* @author   tifnan
* @date     2015-05-25
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include <string.h>
#include "string.h"
#include "board.h"
#include "trace.h"

/* globals */
uint8_t RxBuffer[100];
uint8_t DemoStrBuffer[100];
uint8_t RxCount = 0;
uint8_t RxEndFlag = 0;

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

int main(void)
{
    uint8_t blkcount= 0;
    uint8_t remainder = 0;
    uint8_t strLen = 0;
    uint8_t i = 0;
    
    char* demoStr = "### Welcome to use RealTek Bee ###\r\n";
    strLen = strlen(demoStr);
    memcpy(DemoStrBuffer, demoStr, strLen);
    
    /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /* uart init */
    UART_InitTypeDef uartInitStruct;
    
    UART_StructInit(&uartInitStruct);
    
    /* change default rx trigger level */
    uartInitStruct.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_14BYTE;
    UART_Init(UART, &uartInitStruct);
    
    //enable rx interrupt and line status interrupt
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
    
    /* send demo tips */
    blkcount = strLen / UART_TX_FIFO_SIZE;
    remainder = strLen % UART_TX_FIFO_SIZE;
                     
    /* send block bytes(16 bytes) */
    for(i = 0; i < blkcount; i++)
    {
        UART_SendData(UART, &DemoStrBuffer[16 * i], 16);
        /* wait tx fifo empty */
        while(UART_GetFlagState(UART, UART_FLAG_THR_TSR_EMPTY) != SET);
    }
    
    /* send left bytes */
    UART_SendData(UART, &DemoStrBuffer[16 * i], remainder);
    /* wait tx fifo empty */
    while(UART_GetFlagState(UART, UART_FLAG_THR_TSR_EMPTY) != SET);
    
    /* while loop */
    while(1)
    {
        /* rx end */
        if(RxEndFlag == 1)
        {
            /* 16 is uart rx FIFO size */
            blkcount = RxCount / 16;
            remainder = RxCount % 16;
            
            /* send block bytes(16 bytes) */
            for(i = 0; i < blkcount; i++)
            {
                UART_SendData(UART, &RxBuffer[16 * i], 16);
                /* wait tx fifo empty */
                while(UART_GetFlagState(UART, UART_FLAG_THR_TSR_EMPTY) != SET);
            }
            
            /* send left bytes */
            UART_SendData(UART, &RxBuffer[16 * i], remainder);
            /* wait tx fifo empty */
            while(UART_GetFlagState(UART, UART_FLAG_THR_TSR_EMPTY) != SET);
            
            /* return */
            UART_SendByte(UART, '\r');
            UART_SendByte(UART, '\n');
            while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
            
            RxEndFlag = 0;
            RxCount = 0;
        }
    }
    
    return 0;
}

void PINMUX_Configuration(void)
{
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    
    return;
}

void PAD_Configuration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    return;
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    return;
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
