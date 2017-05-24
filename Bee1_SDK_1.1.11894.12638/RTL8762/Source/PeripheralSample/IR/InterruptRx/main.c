/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    IR demo--interrupt rx
* @details
* @author   tifnan
* @date     2015-05-26
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_ir.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "ir_protocol.h"
#include "trace.h"
#include "board.h"
#include <string.h>
#include "string.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

/* hex to Ascii*/
uint8_t  HexToAscii(uint8_t hex)
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

/* global */
uint8_t RxCount = 0;
uint8_t RxEndFlag = 0;
IRDA_BUF IrBuf;
uint8_t TxBuf[48] = {0};

int main(void) 
{   
    uint8_t address = 0;
    uint8_t cmd = 0;
    
    char* demoStr = "## Address:0x00, Command:0x00 ##\r\n";
    
    memset(&IrBuf, 0, sizeof(IRDA_BUF));
    
    memcpy(TxBuf, demoStr, strlen(demoStr));
    
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
    
    /* IR init */
    IR_InitTypeDef irInitStruct;
    
    IR_StructInit(&irInitStruct);
    
    IR_Init(IR, &irInitStruct);
    
    IR_Cmd(IR, IR_MODE_RX, ENABLE);
    
    /* enable IR tx interrupt */
    IR_INTConfig(IR, IR_INT_RX, ENABLE);

    /* set tx threshold */
    IR_SetRxThreshold(IR, 24);
    
    /* start rx */
    IR_StartRx(IR);
    
    while(true)
    {
        if(RxEndFlag != 0)
        {
            /* stop rx */
            IR_StopRx(IR);
            /* disable rx interrupt */
            IR_INTConfig(IR, IR_INT_RX, DISABLE);
            if(IrBuf.pBufLen == 68)
            {
                /* decode */
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IR start decode", 0);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IrBuf.pBufLen = 0x%x", 1, IrBuf.pBufLen);
                IrBuf.CarrierFrequency_kHz = 38;

                if(IRDA_SUCCEED != irda_nec_decode(&address, &cmd, &IrBuf))
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "irda_nec_decode error!!", 0);
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IR Rx address: 0x%x, command: 0x%x", 2, address, cmd);
                    /* hex to Ascii */
                    TxBuf[13] = HexToAscii(address >> 4);
                    TxBuf[14] = HexToAscii(address & 0x0F);
                    TxBuf[27] = HexToAscii(cmd >> 4);
                    TxBuf[28] = HexToAscii(cmd & 0x0F);
                    /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
                    UART_SendData(UART, TxBuf, 16);
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                    /* continue send 16 bytes */
                    UART_SendData(UART, &TxBuf[16], 16);
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                    
                    /* return */
                    UART_SendByte(UART, '\r');
                    UART_SendByte(UART, '\n');
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                }
            }
            else if(IrBuf.pBufLen == 4)
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Empty IR pkt", 0);
            }
            else
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Wrong IR pkt!!!!", 0);
            }

            //clear memory
            RxCount = 0;
            memset(&IrBuf, 0, sizeof(IRDA_BUF));
            RxEndFlag = 0;
            /* enable interrupt and start rx again */
            IR_INTConfig(IR, IR_INT_RX, ENABLE);
            IR_StartRx(IR);
        }
    }
    
    return 0;
}

void PINMUX_Configuration(void)
{
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    Pinmux_Config(P4_0, IRDA_RX);
    
    return;
}

void PAD_Configuration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
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
