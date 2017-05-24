/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main
* @brief    IR demo--polling tx
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
#include <string.h>
#include "string.h"
#include "ir_protocol.h"
#include "trace.h"
#include "board.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

uint8_t DemoStrBuffer[100];

int main(void) 
{   
    IRDA_BUF ir_buf;
    uint8_t txCount = 0;
    uint8_t iRcode[2];
    uint8_t rxCount = 0;
    uint8_t strLen = 0;
    uint8_t blkcount = 0;
    uint8_t remainder = 0;
    uint8_t i = 0;
    
    char* demoStr = "### Bee IR Polling Tx Demo ###\r\n";
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
        if(rxCount == 2)
        {
            /* encode NEC */
            irda_nec_encode(iRcode[0], iRcode[1], &ir_buf);
            
            /* send a NEC IR pkt */
            while(txCount < NEC_LENGTH)
            {
                if((NEC_LENGTH - txCount) >= IR_TX_FIFO_SIZE)
                {
                    IR_SendData(IR, ir_buf.pBuf + txCount, 31);
                    /* wait tx end */
                    while(IR_GetFlagState(IR, IR_FLAG_TX_FIFO_EMPTY) != SET);
                    txCount+= 31;
                }
                else
                {  
                    IR_SendData(IR, ir_buf.pBuf + txCount, NEC_LENGTH - txCount);
                    /* wait tx end */
                    while(IR_GetFlagState(IR, IR_FLAG_TX_FIFO_EMPTY) != SET);
                    txCount+= (NEC_LENGTH - txCount);
                }
            }
            
            /* stop tx */
            IR_StopTx(IR);
            txCount = 0;
            rxCount = 0;
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
    return;
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
