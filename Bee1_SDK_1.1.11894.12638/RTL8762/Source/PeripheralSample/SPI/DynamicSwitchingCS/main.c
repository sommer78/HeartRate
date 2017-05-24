/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of SPI comunication when Dynamic switching cs signal.
            Attention: only SPI1 have this function.
* @details
* @author	elliot chen
* @date 	2015-10-08
* @version	v1.0
*********************************************************************************************************
*/
#define SPI1_SCK				P4_0		 
#define SPI1_MOSI				P4_1
#define SPI1_MISO				P4_2		 
#define SPI1_CS0				P4_3
#define SPI1_CS1				P3_0
#define SPI1_CS2				P2_0

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_spi.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

void RCC_Configuration(void)
{
	/* turn on SPI clock */
    RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
}

void SPI_PINMUXConfiguration(void)
{
    Pinmux_Config(SPI1_SCK, SPI1_CLK_Master);
	Pinmux_Config(SPI1_MOSI, SPI1_MO_Master);
	Pinmux_Config(SPI1_MISO, SPI1_MI_Master);
	Pinmux_Config(SPI1_CS0, SPI1_SS_N_0_Master);
	Pinmux_Config(SPI1_CS1, SPI1_SS_N_1_Master);
	Pinmux_Config(SPI1_CS2, SPI1_SS_N_2_Master);
}

void SPI_PADConfiguration(void)
{
    Pad_Config(SPI1_SCK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI1_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI1_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI1_CS0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI1_CS1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI1_CS2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void SPI_Configuration(void)    
{
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure);
    
    SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 400000;
    SPI_InitStructure.SPI_RxThresholdLevel  = 0; 
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola; 
    
    SPI_Init(SPI1, &SPI_InitStructure);
 	SPI_Cmd(SPI1, ENABLE);
}

/*--------------------------SPI demo code----------------------------------*/
void SPI_DemoCode(void)
{
	uint8_t SPI_WriteBuf[3] = {0x01,0x02,0x03};
    
    SPI_SendBuffer(SPI1, SPI_WriteBuf, 3);
    /* wait Tx FIFO empty */
    while(SPI_GetFlagState(SPI1, SPI_FLAG_TFE) == RESET);
    while(SPI_GetFlagState(SPI1, SPI_FLAG_BUSY));
    
#if 1
    /* switch to CS1 signal */
    SPI1->SER = BIT(1);
#else
    /* switch to CS1 signal */
    SPI_SetCSNumber(SPI1, 1);
#endif
    
    SPI_SendBuffer(SPI1, SPI_WriteBuf, 3);
    /* wait Tx FIFO empty */
    while(SPI_GetFlagState(SPI1, SPI_FLAG_TFE) == RESET);
    while(SPI_GetFlagState(SPI1, SPI_FLAG_BUSY));
    
#if 1
    /* switch to CS1 signal */
    SPI1->SER = BIT(2);
#else
    /* switch to CS1 signal */
    SPI_SetCSNumber(SPI1, 2);
#endif
    
    SPI_SendBuffer(SPI1, SPI_WriteBuf, 3);
    /* wait Tx FIFO empty */
    while(SPI_GetFlagState(SPI1, SPI_FLAG_TFE) == RESET);
    while(SPI_GetFlagState(SPI1, SPI_FLAG_BUSY));
}

int main(void)
{
    SPI_PINMUXConfiguration();
    RCC_Configuration();
    SPI_PADConfiguration();
    SPI_Configuration();
    SPI_DemoCode();
    while(1);
}
