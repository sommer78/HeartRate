/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     external_flash.c
* @brief    This file provides driver for flash.
* @details
* @author
* @date     2016-05-25
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "external_flash.h"

/**
  * @brief  Read ID of flash chip.
  * @param   No parameter.
  * @return  void
  *
  */
uint8_t ExternalFlash_ReadID(void)
{
	uint8_t sendBuf[4] = {0x9f, 0, 0, 0};
	uint8_t send_len = 4;
	uint8_t recvBuf[4];
	uint8_t index = 0;
	uint8_t recv_len = 0;

	/* Check SPI communication status */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);

	/* Clear receive FIFO */
	recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
	while(recv_len)
	{
		SPI_ReceiveData(FLASH_SPI);
		recv_len--;
	}
	/* Read ID of Flash */
	SPI_SendBuffer(FLASH_SPI, sendBuf, send_len);

	/* Read data */
	while(send_len)
	{
		recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recv_len)
		{
			recvBuf[index++] = SPI_ReceiveData(FLASH_SPI);
			recv_len--;
			send_len--;
		}
	}
	return recvBuf[1];
}

/**
  * @brief Read status register of flash.
  * @param  none.
  * @return value of status register.
  *
  */
static uint8_t ExternalFlash_ReadStatus(void)
{
	uint8_t sendBuf[2] = {0x05, 0};
	uint8_t send_len = 2;
	uint8_t recvBuf[2];
	uint8_t recv_len = 0;
	uint8_t index = 0;

	/* Check SPI communication status */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);

	/* Send command of write status register */
	SPI_SendBuffer(FLASH_SPI, sendBuf, send_len);

	/* Read data */
	while(send_len)
	{
		recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recv_len)
		{
			recvBuf[index++] = SPI_ReceiveData(FLASH_SPI);
			recv_len--;
			send_len--;
		}
	}

	return recvBuf[1];
}

/**
  * @brief Check flash is in progress or not.
  * @param  none.
  * @return SET: Flash is always in progress. RESET: Flash is in standby mode.
  *
  */
FlagStatus ExternalFlash_CheckWriteStatus(void)
{
	uint8_t reg_value = 0;
	uint32_t time_out = 0xfffff;

	do
	{
		/* Time out control */
		time_out--;
		if(time_out == 0)
		{
			return SET; 
		}
		reg_value = ExternalFlash_ReadStatus();
	}
	while(reg_value & STATUS_BUSY);/* Check flash is communicating or not */

	return RESET;
}

/**
  * @brief  Send write enable command before every page program(PP), 
  * sector earse(SE), block earse(BE), chip earse(CE) and write status register(WRSR) command.
  * @param   No parameter.
  * @return  void
  *
  */
void ExternalFlash_WriteCmd(FunctionalState NewState)
{
	uint8_t send_len = 1;
	uint8_t recv_len = 0;

	/* Check earse status */
	ExternalFlash_CheckWriteStatus();
		
	/* Check communication status */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);

	/* Send write command */
	if(NewState == ENABLE)
	{
		/* Send write enable command */
		SPI_SendData(FLASH_SPI, 0x06);
	}
	else
	{
		/* Send write disable command */
		SPI_SendData(FLASH_SPI, 0x04);
	}
	
	/* Read data */
	while(send_len)
	{
		recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recv_len)
		{
			SPI_ReceiveData(FLASH_SPI);
			recv_len--;
			send_len--;
		}
	}

	/* Check earse status */
	ExternalFlash_CheckWriteStatus();
}

/**
  * @brief  Send write status register command.
  * @param  data: data whch want to write to status register.
  * @return  void
  *
  */
void ExternalFlash_WriteStatusRegister(uint16_t data)
{
	uint8_t sendBuf[3] = {0x01, 0, 0};
	uint8_t send_len = 3;
	uint8_t recv_len = 0;
	
	/* Enable write */
	ExternalFlash_WriteCmd(ENABLE);

	/* Write status register */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);
	sendBuf[1] = data & 0xff;
	sendBuf[2] = (data>>8) & 0xff;
	SPI_SendBuffer(FLASH_SPI, sendBuf, send_len);
	
	/* Read data */
	while(send_len)
	{
		recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recv_len)
		{
			SPI_ReceiveData(FLASH_SPI);
			recv_len--;
			send_len--;
		}
	}
}

/**
  * @brief Earse flash.
  * @param  address: address which begin to earse.
  * @param  mode: select earse mode.
  * @return none.  
  *
  */
void ExternalFlash_Earse(uint32_t address, Flash_erase_module_t mode)
{
	uint8_t sendBuf[4] = {0, 0, 0, 0};
	uint8_t send_len = 4;
	uint16_t recv_len = 0;
	
	/* Enable write */
	ExternalFlash_WriteCmd(ENABLE);

	/* Write data */
	sendBuf[0] = mode;
	sendBuf[1] = (address>>16) & 0xff;
	sendBuf[2] = (address>>8) & 0xff;
	sendBuf[3] = address & 0xff;

	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);
	SPI_SendBuffer(FLASH_SPI, sendBuf, send_len);
	
	/* Read data no matter it is useful or not */
	while(send_len)
	{
		recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recv_len)
		{
			SPI_ReceiveData(FLASH_SPI);
			recv_len--;
			send_len--;
		}
	}
}

/**
  * @brief Earse chip.
  * @param  none.
  * @return none.  
  *
  */
void ExternalFlash_EarseChip(void)
{
	uint8_t sendBuf[4] = {0, 0, 0, 0};
	uint8_t send_len = 1;
	uint16_t recv_len = 0;
	
	/* Enable write */
	ExternalFlash_WriteCmd(ENABLE);

	/* Write data */
	sendBuf[0] = CHIP_EARSE;
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);
	SPI_SendBuffer(FLASH_SPI, sendBuf, send_len);
	
	/* Read data no matter it is useful or not */
	while(send_len)
	{
		recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recv_len)
		{
			SPI_ReceiveData(FLASH_SPI);
			recv_len--;
			send_len--;
		}
	}
}

/**
  * @brief Write flash.
  * @param  address: address which begin to write.
  * @param  psendBuf: address of data buffer which want to write.
  * @param  len: length of data buffer which want to write.
  * @return none.  
  *
  */
void ExternalFlash_Write(uint32_t address, uint8_t *psendBuf, uint16_t len)
{
	uint8_t sendBuf[4];
	uint16_t recv_len = 0;
	
	/* Enable write */
	ExternalFlash_WriteCmd(ENABLE);

	/* Check communication status */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);

	/* Switch to Tx only mode */
	SPI_ChangeDirection(FLASH_SPI, SPI_Direction_TxOnly);

	/* Send write command */
	sendBuf[0] = 0x02;
	sendBuf[1] = (address>>16) & 0xff;
	sendBuf[2] = (address>>8) & 0xff;
	sendBuf[3] = address & 0xff;
	SPI_SendBuffer(FLASH_SPI, sendBuf, 4);

	/* Write data */
	SPI_SendBuffer(FLASH_SPI, psendBuf, len);

	/* Check communication status */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);
	
	/* Read data no matter it is useful or not */
	recv_len = SPI_GetRxFIFOLen(FLASH_SPI);
	while(recv_len)
	{
		SPI_ReceiveData(FLASH_SPI);
		recv_len--;
	}

	/* Switch to full duplex mode */
	SPI_ChangeDirection(FLASH_SPI, SPI_Direction_FullDuplex);
}

/**
  * @brief Read flash.
  * @param  address: address which begin to read.
  * @param  pStoreBuf: address of data buffer which want to read.
  * @param  len: length of data buffer which want to read.
  * @return none.  
  *
  */
 void ExternalFlash_Read(uint32_t address, uint8_t *pStoreBuf, uint16_t len)
{
	uint32_t recvlen = 0;
	uint8_t sendBuf[4] ={0x03,0,0,0};

	/* Read flash status register */
	ExternalFlash_CheckWriteStatus();

	/* Check communication status */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_BUSY) == SET);
	
	/* Switch to EEPROM mode */
	SPI_ChangeDirection(FLASH_SPI, SPI_Direction_EEPROM);

	/* Clear RX FIFO */
	recvlen = SPI_GetRxFIFOLen(FLASH_SPI);
	/* Read data if RX FIFO has data */
	while(recvlen)
	{
		SPI_ReceiveData(FLASH_SPI);
		recvlen--;
	}

	/* Configure length of data which you want to read */
	SPI_SetReadLen(FLASH_SPI, len);
	
	/* Send read command and address */
	sendBuf[0] = 0x03;
	sendBuf[1] = (address>>16) & 0xff;
	sendBuf[2] = (address>>8) & 0xff;
	sendBuf[3] = address & 0xff;
	SPI_SendBuffer(FLASH_SPI, sendBuf, 4);
	
	/* Wait RX FIFO not empty flag */
	while(SPI_GetFlagState(FLASH_SPI, SPI_FLAG_RFNE) == RESET);

	/* Read data */
	while(len)
	{
		recvlen = SPI_GetRxFIFOLen(FLASH_SPI);
		while(recvlen)
		{
			*pStoreBuf++ = (uint8_t)SPI_ReceiveData(FLASH_SPI);
			recvlen--;
			len--;
		}
	}
	/* Switch to full duplex mode */
	SPI_ChangeDirection(FLASH_SPI, SPI_Direction_FullDuplex);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

