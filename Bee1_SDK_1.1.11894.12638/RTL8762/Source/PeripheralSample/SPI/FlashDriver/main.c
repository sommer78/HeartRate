/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides driver code of flash GD25Q32.
* @details
* @author	elliot chen
* @date 	2016-05-06
* @version	v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "external_flash.h"

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void Board_SPI_Init(void)
{
	Pad_Config(FLASH_SCK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(FLASH_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(FLASH_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
	Pad_Config(FLASH_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

	Pinmux_Config(FLASH_SCK, SPI0_CLK_Master);
	Pinmux_Config(FLASH_MOSI, SPI0_MO_Master);
	Pinmux_Config(FLASH_MISO, SPI0_MI_Master);
	Pinmux_Config(FLASH_CS, SPI0_SS_N_0_Master);
}

/**
  * @brief  Initialize SPI peripheral.
  * @param   No parameter.
  * @return  void
  */
void Driver_SPI_Init(void)
{
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	
	SPI_InitTypeDef  SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);   
	SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
	SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_ClockSpeed  = 400000;
	SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
	SPI_InitStructure.SPI_NDF 		  = 0;
	SPI_Init(FLASH_SPI, &SPI_InitStructure); 
	SPI_Cmd(FLASH_SPI, ENABLE);
}

/**
  * @brief  Initialize Flash chip.
  * @param   No parameter.
  * @return  void
  */
void ExternalFlash_Init(void)
{
	Board_SPI_Init();
	Driver_SPI_Init();

	/* Check Flash ID */
	if(GD25Q32_FLASH_ID != ExternalFlash_ReadID())
	{
		//Add error handle code here
	}
}

/**
  * @brief  demo code of operation about Flash.
  * @param  No parameter.
  * @return  void
  */
void External_Flash_TestCode(void)
{
	uint8_t sendBuf[256];
	uint8_t recvBuf[256];
	uint16_t index = 0;

	for(index=0; index<256; index++)
	{
		sendBuf[index] = index + 2;
	}
	
	for(index=0; index<256; index++)
	{
		recvBuf[index] = 0;
	}

	ExternalFlash_Init();
	
	ExternalFlash_Earse(0, SECTOR_ERASE);
	ExternalFlash_Write(0, sendBuf, 20);
	ExternalFlash_Read(0, recvBuf, 20);

	for(index=0; index<256; index++)
	{
		sendBuf[index] = index + 1;
	}
    ExternalFlash_Earse(0, SECTOR_ERASE);
	ExternalFlash_Write(0, sendBuf, 200);
	ExternalFlash_Read(0, recvBuf, 200);
}

/**
* @brief  main() is the entry of user code.
* @param   No parameter.
* @return  void
*/
int main(void)
{
	External_Flash_TestCode();
	while(1);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

