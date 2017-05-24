/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    IR demo--interrupt tx
* @details
* @author   tifnan
* @date     2015-05-25
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_ir.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "ir_protocol.h"
#include <string.h>
#include "string.h"
#include "board.h"
#include "trace.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

uint8_t DemoStrBuffer[100];

/* global */
uint8_t TxCount = 0;
uint8_t TxEndFlag = 0;
uint8_t TxStartFlag = 0;
IRDA_BUF IrBuf;

int main(void) 
{   
    uint8_t iRcode[2];
    uint8_t rxCount = 0;
    uint8_t strLen = 0;
    uint8_t blkcount = 0;
    uint8_t remainder = 0;
    uint8_t i = 0;
    
    char* demoStr = "### Bee IR Interrupt Tx Demo ###\r\n";
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
    
    UART_Init(UART, &uartInitStruct);
    
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
    
    /* IR init */
    IR_InitTypeDef irInitStruct;
    
    IR_StructInit(&irInitStruct);
    
    IR_Init(IR, &irInitStruct);
    
    IR_Cmd(IR, IR_MODE_TX, ENABLE);

    /* set tx threshold */
    IR_SetTxThreshold(IR, 31);
    
    while(1)
    {
        /* wait uart data */
        if(UART_GetFlagState(UART, UART_FLAG_RX_DATA_RDY) == SET)
        {
            iRcode[rxCount] = UART_ReceiveByte(UART);
            rxCount++;
        }
        
        /* NEC IR address and command are received */
        if(rxCount == 2 && TxStartFlag == 0)
        {
            /* encode NEC */
            irda_nec_encode(iRcode[0], iRcode[1], &IrBuf);
            /* start to send a NEC IR pkt */
            IR_SendData(IR, IrBuf.pBuf + TxCount, 31);
            /* enable IR tx interrupt */
            IR_INTConfig(IR, IR_INT_TF_EMPTY, ENABLE);
            TxCount+= 31;
            TxStartFlag = 1;
        }
        
        /* wait tx end */
        if(TxEndFlag == 1)
        {
            rxCount = 0;
            TxEndFlag = 0;
            TxStartFlag = 0;
            TxCount = 0;
        }
    }
    
    return 0;
}

void PINMUX_Configuration(void)
{
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    Pinmux_Config(P4_1, IRDA_TX);
    
    return;
}

void PAD_Configuration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART and IR clock */
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    return;
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQ;
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
