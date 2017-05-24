/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      external_flash.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-05-25
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __EXTERNAL_FLASH_H
#define __EXTERNAL_FLASH_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "rtl876x.h"
#include "rtl876x_spi.h"

/* Defines -------------------------------------------------------------------*/

typedef enum _FLASH_ERASE_MODULE
{
	BLOCK_ERASE_64  = 0xd8,
	BLOCK_ERASE_32  = 0x52,
	SECTOR_ERASE    = 0x20,
}Flash_erase_module_t;

/* Flash ID */
#define GD25Q32_FLASH_ID 		0xC8

/* Status Register Bits */
#define STATUS_BUSY				0x01
#define	STATUS_WEL				0x02
#define	STATUS_BP0				0x04
#define	STATUS_BP1				0x08
#define	STATUS_TB   			0x20
#define	STATUS_SRP				0x80

/* Flash operation command */
#define CHIP_EARSE				0x60

uint8_t ExternalFlash_ReadID(void);
static uint8_t ExternalFlash_ReadStatus(void);
FlagStatus ExternalFlash_CheckWriteStatus(void);
void ExternalFlash_WriteCmd(FunctionalState NewState);
void ExternalFlash_WriteStatusRegister(uint16_t data);
void ExternalFlash_Earse(uint32_t address, Flash_erase_module_t mode);
void ExternalFlash_EarseChip(void);
void ExternalFlash_Write(uint32_t address, uint8_t *psendBuf, uint16_t len);
void ExternalFlash_Read(uint32_t address, uint8_t *pStoreBuf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /*__EXTERNAL_FLASH_H*/


/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

