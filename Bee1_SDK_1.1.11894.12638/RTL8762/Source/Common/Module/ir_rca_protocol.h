/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ir_rca_protocol.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-05-09
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __IR_RCA_PROTOCOL_H
#define __IR_RCA_PROTOCOL_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl_types.h"

/* Defines ------------------------------------------------------------------*/

/* Configure maxium length of user code and data code */
#define MAX_CODE_SIZE           4
/* Configure maxium length of data sent by IR peripheral */
#define MAX_IR_BUF_SIZE  		140
/* Configure maxium length of header of IR protocol */
#define MAX_HEADDER_LEN			16
/* Configure maxium data length of logical 0 or logical 1 */
#define MAX_LOG_WAVFORM_SIZE	2

/** 
  * @brief  IR data structure definition  
  */
typedef struct
{
	/* Unit of carrierFreq is KHz */
	uint8_t 	carrierFreq;
	uint8_t 	code[MAX_CODE_SIZE];
	uint8_t 	codeLen;
	uint16_t	irBuf[MAX_IR_BUF_SIZE];
	uint16_t	bufLen;
}IR_DataTypeDef;

/** 
  * @brief  IR protocol structure definition  
  */
typedef struct 
{
	uint8_t  	headerLen;
	uint16_t 	headerBuf[MAX_HEADDER_LEN];
	uint16_t 	log0Buf[MAX_LOG_WAVFORM_SIZE]; 
	uint16_t 	log1Buf[MAX_LOG_WAVFORM_SIZE]; 
	uint16_t 	stopBuf;
}IR_ProtocolTypeDef;

/** 
  * @brief  IR return type of encoding function  
  */
typedef enum 
{
	 IR_ERROR = -1,
	 IR_SUCCEED = 0,
	 IR_HEADER_ERROR = 1,
	 IR_DATA_ERROR
}IR_RETURN_Type;

/**
 * @brief IR data length. This value depends on IR protocol.
 *
*/
typedef enum
{
	RCA_LENGTH  = 52, /**< RCA data length  */
	NEC_LENGTH  = 68, /**< NEC data length  */
	RC5_LENGTH  = 80, /**< RC5 data length  */
	RC6_LENGTH  = 70, /**< RC6 data length  */
	SONY_LENGTH = 90, /**< SONY data length */
}IR_Mode_Length;

IR_RETURN_Type IR_RCA_Encode(uint8_t address, uint8_t command, IR_DataTypeDef *IR_DataStruct);

#ifdef __cplusplus
}
#endif

#endif /*__IR_RCA_PROTOCOL_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

