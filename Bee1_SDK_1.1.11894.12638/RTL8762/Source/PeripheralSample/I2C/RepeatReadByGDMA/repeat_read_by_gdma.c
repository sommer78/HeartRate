/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief		This file provides drivers for I2C module repeat read data(write and read data continuously) by GDMA.
* @details
* @author	elliot chen
* @date 		2016-03-28
* @version	v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "repeat_read_by_gdma.h"

/* Global variables ------------------------------------------------------------*/

/* buffer which want to send by system, APP can not delete it */
uint16_t I2C_SendBuf[I2C_Buf_Maxsize];

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void Board_I2CInit(void)
{
	/* Configure PAD and pinmux */
	Pad_Config(I2C_SCL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(I2C_SDA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);	
	Pinmux_Config(I2C_SCL, I2C0_CLK);
	Pinmux_Config(I2C_SDA, I2C0_DAT);
}

/**
  * @brief  Initialize I2C peripheral.
  * @param   No parameter.
  * @return  void
  */
void I2C_ModuleInit(void)
{
	/* Initialize I2C */
    I2C_InitTypeDef  I2C_InitStructure;
	
    I2C_InitStructure.I2C_ClockSpeed 		= 100000;
  	I2C_InitStructure.I2C_DeviveMode 		= I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode 		= I2C_AddressMode_7BIT;
  	I2C_InitStructure.I2C_SlaveAddress 		= 0x50;
  	I2C_InitStructure.I2C_Ack 				= I2C_Ack_Enable;
  	I2C_Init(I2C_TYPE, &I2C_InitStructure);
}

/**
  * @brief  Initialize GDMA and I2C which want to receive data by GDMA.
  * @param   No parameter.
  * @return  void
  */
void GDMA_ReadInit(void)    
{ 
	/* Initialize GDMA */
	GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_CHANNEL_REV_NUM;
  	GDMA_InitStruct.GDMA_DIR 			     = GDMA_DIR_PeripheralToMemory;
  	GDMA_InitStruct.GDMA_BufferSize 	     = 0;
  	GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
	GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
	GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_SourceMsize	     = GDMA_Msize_1;
	GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
	GDMA_InitStruct.GDMA_SourceAddr 	     = (uint32_t)(&(I2C_TYPE->IC_DATA_CMD));
	GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)NULL;
	GDMA_InitStruct.GDMA_TransferType        = GDMA_TransferType_I2C0_RX;
	GDMA_InitStruct.GDMA_ChannelPriority     = GDMA_CHANNEL_REV_NUM;
    GDMA_Init(GDMA_CHANNEL_REV, &GDMA_InitStruct);
    
    /* Configure NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = GDMA_CHANNEL_REV_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* Enable transfer interrupt */
    GDMA_INTConfig(GDMA_CHANNEL_REV_NUM, GDMA_INT_Transfer,ENABLE);
}

void GDMA_SendInit(void)    
{ 
	/* Initialize GDMA to send data */
	GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_CHANNEL_SEND_NUM;
  	GDMA_InitStruct.GDMA_DIR 			     = GDMA_DIR_MemoryToPeripheral;
  	GDMA_InitStruct.GDMA_BufferSize 	     = 0;
  	GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
	GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
	GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_HalfWord;
	GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_HalfWord;
	GDMA_InitStruct.GDMA_SourceMsize	     = GDMA_Msize_1;
	GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
	GDMA_InitStruct.GDMA_SourceAddr 	     = (uint32_t)NULL;
	GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(I2C_TYPE->IC_DATA_CMD));
	GDMA_InitStruct.GDMA_TransferType        = GDMA_TransferType_I2C0_TX;
	GDMA_InitStruct.GDMA_ChannelPriority     = GDMA_CHANNEL_SEND_NUM;
    GDMA_Init(GDMA_CHANNEL_SEND, &GDMA_InitStruct); 
}

/**
  * @brief  initialization of peripherals.
  * @param   No parameter.
  * @return  void
  */
void Driver_I2C_GDMA_Init(void)
{
	/* Enable I2C and GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

	/* Initialize I2C peripheral */
	I2C_ModuleInit();
	/* Initialize I2C and GDMA which for receive data */
    GDMA_ReadInit();
	/* Initialize GDMA which for send data */
	GDMA_SendInit();
}

/**
  *@brief Sends data and read data in master mode through the I2C peripheral and GDMA peripheral.
  *@param pWriteBuf: address of data buffer whcih want to send.
  *@param Writelen: length of data buffer whcih want to send.
  *@param pReadBuf: address of data buffer whcih want to receive.
  *@param Readlen: length of data buffer whcih want to receive.
  *
  *@note: Writelen + Readlen <= I2C_Buf_Maxsize !!!
  * @retval None
  */
void I2C_RepeatReadByGDMA(uint8_t* pWriteBuf, uint8_t Writelen, uint8_t* pReadBuf, uint8_t Readlen)
{
	uint32_t i = 0;

	/* Configure send data store in send buffer */
	for(i=0; i<Writelen; i++)
	{
		I2C_SendBuf[i] = *pWriteBuf++;
	}

	/* Configure receive command store in send buffer*/
	for(i=Writelen; i<(Writelen+Readlen); i++)
	{
		I2C_SendBuf[i] = BIT(8);	
	}
	I2C_SendBuf[Writelen+Readlen-1] = BIT(8)|BIT(9);
	
	/* Configure I2C receive data size */
	GDMA_SetDestinationAddress(GDMA_CHANNEL_REV, (uint32_t)pReadBuf);
	GDMA_SetBufferSize(GDMA_CHANNEL_REV, Readlen);
	I2C_Cmd(I2C_TYPE, ENABLE);
	GDMA_Cmd(GDMA_CHANNEL_REV_NUM, ENABLE);

	/* Configure I2C total size of data which want to send */
	GDMA_SetSourceAddress(GDMA_CHANNEL_SEND, (uint32_t)I2C_SendBuf);
	GDMA_SetBufferSize(GDMA_CHANNEL_SEND, Writelen+Readlen);

	/* Start write and read */
	GDMA_Cmd(GDMA_CHANNEL_SEND_NUM, ENABLE);

}

/**
  * @brief  GDMA interrupt handle function. 
  * @note: App can control GDMA transmission process here.
  * @param  None.
  * @return None.
  */
void GDMAReceiveIntrHandler(void)
{
	// add application for APP here

	/* Clear GDMA interrupt */
    GDMA_ClearAllTypeINT(GDMA_CHANNEL_REV_NUM);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

