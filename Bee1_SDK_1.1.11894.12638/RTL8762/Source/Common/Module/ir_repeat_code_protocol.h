/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ir_repeat_code_protocol.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-06-12
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __IR_REPEAT_CODE_PROTOCOL_H
#define __IR_REPEAT_CODE_PROTOCOL_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl_types.h"

/* Defines ------------------------------------------------------------------*/

/* brief: Configure maxium length of repeat code of IR protocol. 
  * note: MAX_REPEAT_CODE_LEN >= RepeatCodeLen!!! 
  * User can modify this define to adjust this driver to various application.
 */
#define MAX_REPEAT_CODE_LEN		3

/** 
  * @brief IR data structure definition which user can send by IR module of RTL8762A. 
  */
typedef struct
{
	/* Unit of carrierFreq is KHz */
	uint8_t 	carrierFreq;
	uint16_t	irBuf[MAX_REPEAT_CODE_LEN];
	uint16_t	bufLen;
}IR_RepeatDataTypeDef;

/** 
  * @brief  IR protocol structure definition  which store IR repeat code. 
  * User can modify REPEAT_CODE_PROTOCOL whose type is  IR_RepeatCodeTypeDef to adjust to various application.
  */
typedef struct 
{
	uint8_t  	RepeatCodeLen;
	uint16_t 	RepeatCodeBuf[MAX_REPEAT_CODE_LEN];
}IR_RepeatCodeTypeDef;

BOOL IR_Repeat_Code_Encode(uint8_t frequency_KHz, IR_RepeatDataTypeDef *IR_DataStruct);

#ifdef __cplusplus
}
#endif

#endif /*__IR_REPEAT_CODE_PROTOCOL_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

