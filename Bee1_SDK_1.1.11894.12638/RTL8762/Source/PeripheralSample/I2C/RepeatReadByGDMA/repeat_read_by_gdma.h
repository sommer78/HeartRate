/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      	repeat_read_by_gdma.h
* @brief		This file provides APIs for I2C module repeat read data(write and read data continuously) by GDMA.	     
* @details   
* @author    	elliot chen
* @date      	2016-3-29
* @version   	v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __REPEAT_READ_BY_GDMA_H
#define __REPEAT_READ_BY_GDMA_H

#ifdef __cpluspuls
 extern "C" {
#endif
     
/* Includes ------------------------------------------------------------------*/
#include"rtl876x.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gdma.h"
#include "rtl876x_i2c.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/* Defines ------------------------------------------------------------------*/

/* Pin setting */
#define I2C_SCL						P2_4		 
#define I2C_SDA						P2_5
#define I2C_TYPE					I2C0
/* GDMA channel for receive data */
#define GDMA_CHANNEL_REV			GDMA_Channel1
#define GDMA_CHANNEL_REV_NUM		1
#define GDMA_CHANNEL_REV_IRQ		GDMA0_CHANNEL1_IRQ
#define GDMAReceiveIntrHandler		Gdma0Ch1IntrHandler
/* GDMA channel for send data */
#define GDMA_CHANNEL_SEND			GDMA_Channel0
#define GDMA_CHANNEL_SEND_NUM		0

/* Configure buffer size of array which want to send and receive by APP! */
/* App user attention: this size must greater than the sum of the length of register and the length of the read data! */
#define I2C_Buf_Maxsize				40

void Board_I2CInit(void);
void Driver_I2C_GDMA_Init(void);
void I2C_RepeatReadByGDMA(uint8_t* pWriteBuf, uint8_t Writelen, uint8_t* pReadBuf, uint8_t Readlen);

#ifdef __cplusplus
}
#endif

#endif /*__REPEAT_READ_BY_GDMA_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

