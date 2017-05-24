/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of SPI comunication.
* @details
* @author	elliot chen
* @date 	2015-06-04
* @version	v0.1
*********************************************************************************************************
*/
#define SPI0_SCK				P4_0		 
#define SPI0_MOSI				P4_1
#define SPI0_MISO				P4_2		 
#define SPI0_CS				    P4_3

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_spi.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

void RCC_Configuration(void)
{
	/* turn on SPI clock */
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

void SPI_Configuration(void)    
{
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure);
    
    SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 400000;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola; 
    
    SPI_Init(SPI0, &SPI_InitStructure);
	SPI_Cmd(SPI0, ENABLE);
}

/*--------------------------SPI demo code----------------------------------*/
void SPI_DemoCode(void)
{
	uint8_t SPI_WriteBuf[16] = {0x01,0x02,0x00};
    uint8_t SPI_ReadBuf[16] = {0, 0 , 0, 0};
    uint8_t idx = 0;
    uint8_t len = 0;
	
    /*---------------read flash ID--------------*/
    SPI_WriteBuf[0] = 0x9f;
    SPI_SendBuffer(SPI0, SPI_WriteBuf, 4);
    
    /*Waiting for SPI data transfer to end*/
    while(SPI_GetFlagState(SPI0, SPI_FLAG_BUSY));
    
    /*read ID number of flash GD25Q20*/
    len = SPI_GetRxFIFOLen(SPI0);
    for (idx=0; idx<len; idx++)
	{
        SPI_ReadBuf[idx] = SPI_ReceiveData(SPI0);
        //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SPI_ReadBuf[%d] = 0x%x", 2, idx, SPI_ReadBuf[idx]);
    }
}

int main(void)
{
    RCC_Configuration();
    SPI_PINMUXConfiguration();
    SPI_PADConfiguration();
    SPI_Configuration();
    SPI_DemoCode();
    while(1);
}
