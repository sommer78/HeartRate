/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of SPI receive data by GDMA.
* @details
* @author	elliot chen
* @date 	2015-06-04
* @version	v0.1
*********************************************************************************************************
*/
/*Defines--------------------------------------------------------------------*/
#define SPI0_SCK				P4_0		 
#define SPI0_MOSI				P4_1
#define SPI0_MISO				P4_2		 
#define SPI0_CS				    P4_3

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_gdma.h"
#include "rtl876x_spi.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/*--------------------------GDMA memory to memory transfer demo code----------------------------------*/
void RCC_Configuration(void)
{
	/* turn on GDMA and SPI clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
}

void SPI_PINMUXConfiguration(void)
{
    Pinmux_Config(SPI0_SCK, SPI0_CLK_Master);
	Pinmux_Config(SPI0_MOSI, SPI0_MO_Master);
	Pinmux_Config(SPI0_MISO, SPI0_MI_Master);
	Pinmux_Config(SPI0_CS, SPI0_SS_N_0_Master);
}

void SPI_PADConfiguration(void)
{
    Pad_Config(SPI0_SCK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI0_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI0_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI0_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

/*------------------------------test GDMA RX-------------------------------*/
void GDMA_RXConfiguration(void)    
{
	SPI_InitTypeDef  SPI_InitStructure;
	GDMA_InitTypeDef GDMA_InitStruct;
    
    uint8_t i = 0;
	uint8_t GDMA_WriteCmdBuffer[100] = {0x02,0xff,0x00,0x00};
    uint8_t GDMA_RevflashBuffer[100];

	/*----------------test buffer init------------------*/
    for(i=4; i<100; i++)
    {   
        GDMA_WriteCmdBuffer[i] = i + 1;
    }
    
    for(i=0; i<100; i++)
    {   
        GDMA_RevflashBuffer[i] = 0;
    }
    
	/*----------------SPI init---------------------------*/
    SPI_StructInit(&SPI_InitStructure);   
    SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 8000000;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;

	/*---------------------GDMA initial------------------------------*/
    GDMA_InitStruct.GDMA_ChannelNum          = 1;
  	GDMA_InitStruct.GDMA_DIR 			     = GDMA_DIR_PeripheralToMemory;
  	GDMA_InitStruct.GDMA_BufferSize 	     = 50;
  	GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
	GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;

	GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_SourceMsize	     = GDMA_Msize_1;
	GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
	
	GDMA_InitStruct.GDMA_SourceAddr 	     = (uint32_t)SPI0->DR;
	GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)GDMA_RevflashBuffer;
	GDMA_InitStruct.GDMA_TransferType        = GDMA_TransferType_SPI0_RX;
	GDMA_InitStruct.GDMA_ChannelPriority     = 0;
    
    GDMA_Init(GDMA_Channel1, &GDMA_InitStruct);
    SPI_Init(SPI0, &SPI_InitStructure);
    
 	SPI_Cmd(SPI0, ENABLE);
    
    /*-----------------GDMA IRQ-----------------------------*/
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = GDMA0_CHANNEL1_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	/* Enable transfer interrupt */
    GDMA_INTConfig(1, GDMA_INT_Transfer,ENABLE);
    GDMA_Cmd(1, ENABLE);
}

int main(void)
{
    RCC_Configuration();
    SPI_PINMUXConfiguration();
    SPI_PADConfiguration();
    GDMA_RXConfiguration();
    while(1);
}

void Gdma0Ch1IntrHandler(void)
{
    GDMA_ClearAllTypeINT(1);
    GDMA_Cmd(1, ENABLE);
}
