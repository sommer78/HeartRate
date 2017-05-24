/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of UART receive data by GDMA.
* @details
* @author	elliot chen
* @date 	2015-06-11
* @version	v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_gdma.h"
#include "rtl876x_uart.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"

void RCC_Configuration(void)
{
	/* Turn on GDMA and UART clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
}

void UART_PINMUXConfiguration(void)
{
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
}

void UART_PADConfiguration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

void UartConfiguration(void)
{
    UART_PINMUXConfiguration();
    UART_PADConfiguration();
    /* Data uart init */ 
    UART_InitTypeDef uartInitStruct;
    UART_StructInit(&uartInitStruct);
    uartInitStruct.dmaEn = UART_DMA_ENABLE;
    UART_Init(UART, &uartInitStruct);
}

void UARTReceiveDataByGDMA(void)
{
    uint8_t GDMA_ReceiveBuffer[256];
    uint8_t i = 0;
    GDMA_InitTypeDef GDMA_InitStruct;    
    
    /* Initialize test buffer which for storing data from UART */
    for(i=0; i<255; i++)
    {   
        GDMA_ReceiveBuffer[i] = 0;
    }
    
    /* GDMA init */
    GDMA_StructInit(&GDMA_InitStruct);      
	GDMA_InitStruct.GDMA_ChannelNum      = 1;
  	GDMA_InitStruct.GDMA_DIR 			 = GDMA_DIR_PeripheralToMemory;
	/* Configure total transmission size */
  	GDMA_InitStruct.GDMA_BufferSize 	 = 20;
  	GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Fix;
	GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_SourceMsize	  = GDMA_Msize_1;
	GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_1;   
	GDMA_InitStruct.GDMA_SourceAddr 	 = (uint32_t)(&(UART->RB_THR));
	GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)GDMA_ReceiveBuffer;
	GDMA_InitStruct.GDMA_TransferType    = GDMA_TransferType_UART0_RX;
	GDMA_InitStruct.GDMA_ChannelPriority = 6;//channel prority between 0 to 6
    
    GDMA_Init(GDMA_Channel1, &GDMA_InitStruct);
    
    /* Configure NVIC of GDMA */
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = GDMA0_CHANNEL1_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
    GDMA_INTConfig(1, GDMA_INT_Transfer,ENABLE);
   	
    /* Start to receive data from data uart */
    GDMA_Cmd(1, ENABLE);
}


int main(void)
{
    RCC_Configuration();
    UartConfiguration();
    UARTReceiveDataByGDMA();
    while(1);

}

void Gdma0Ch1IntrHandler(void)
{
    GDMA_ClearAllTypeINT(1);
	
	/* Enable GDMA again to receive data again */
    GDMA_Cmd(1, ENABLE);
}
