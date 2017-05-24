/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file      ir_rca_protocol.c
* @brief    This file provides driver of RCA protocol encoding.
* @details
* @author  elliot_chen
* @date     2016-05-09
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "ir_rca_protocol.h"

/* Constants------------------------------------------------------------------- */
#define PULSE_HIGH  	0x8000
#define PULSE_LOW   	0x0000
#define USER_CODE_WDTH	4
#define DATA_CODE_WDTH	8

/*! 
* @ brief:RCA protocol structure.
* @ note: Store parameters of TCL RCA protocol. 
* @ Carrier frequency = 38kHz
* @ duty factor = 1/2
* @ first pulse : 4ms 4ms
* @ Address (4 bits) is sent first, then ~Address
* @ Command (8 bits) follows, then ~Command
* @ LSB is sent first !
*/
const IR_ProtocolTypeDef RCA_PROTOCOL = 
{
	 2,                                                 /* headerLen */
	{PULSE_HIGH|4000 , PULSE_LOW|4000},                 /* headerBuf */
	{PULSE_HIGH|500  , PULSE_LOW|2000},                 /* log0Buf */
	{PULSE_HIGH|500  , PULSE_LOW|1000},                 /* log1Buf */
	 PULSE_HIGH|500										/* stopBuf */
};

/**
  * @brief    unit convert.
  * @param  time: time of waveform.
  * @param  carrier_cycle: cycle of carrier.
  * @retval   vaule of data whose unit is cycle of carrier.  
  */
static uint16_t ConvertToCarrierCycle(uint32_t time, uint32_t freq) 
{ 
    return ((time & 0x8000) | ((time &0x7FFF)*freq/1000)); 
}

/**
  * @brief   check specify bit value of data.
  * @param a: data which want to check. 
  * @param  bit_pos: position of bit which want to check  
  * @retval 
  */
static uint8_t CheckBitStatus (uint32_t a, uint32_t bit_pos)
{
    return ((a >> bit_pos) & 0x01); 
} 

/**
  * @brief    Encode data to packet.
  * @param  IR_DataStruct: pointer to struct which store RCA code.
  * @param  IR_Protocol: pointer to specify IR protocol structure.
  * @retval None
  */
static IR_RETURN_Type RCA_EncodePacket(IR_DataTypeDef *IR_DataStruct, IR_ProtocolTypeDef *IR_Protocol)
{
	uint8_t codeWidth = 0;
	uint16_t index = 0;
	uint16_t bitPos = 0;
	uint16_t bufLen = IR_Protocol->headerLen;
	uint16_t Log1[MAX_LOG_WAVFORM_SIZE];
    uint16_t Log0[MAX_LOG_WAVFORM_SIZE];

	/* Error handle */
	if(IR_DataStruct->carrierFreq == 0)
    {
        return IR_ERROR;
    }

	/* Encoding logical 1 and logical 0 */
	for (index=0; index < MAX_LOG_WAVFORM_SIZE; index++)
    {
        Log1[index] = ConvertToCarrierCycle(IR_Protocol->log1Buf[index],IR_DataStruct->carrierFreq);
        Log0[index] = ConvertToCarrierCycle(IR_Protocol->log0Buf[index],IR_DataStruct->carrierFreq);
    }

	/* Encode header */
    for (index=0; index<IR_Protocol->headerLen; index++)
    {
        IR_DataStruct->irBuf[index]=  ConvertToCarrierCycle(IR_Protocol->headerBuf[index],IR_DataStruct->carrierFreq);
    }

	/* Encode user and data code */
	for (index=0; index<IR_DataStruct->codeLen; index++)
	{
		if((index%2) == 0)
		{
			codeWidth = USER_CODE_WDTH;
		}
		else
		{
			codeWidth = DATA_CODE_WDTH;
		}
		
		for (bitPos=0; bitPos<codeWidth; bitPos++)
        {   
            if (CheckBitStatus(IR_DataStruct->code[index], bitPos)== 0x01)
            {
            	/* Logical 1 data */
            	IR_DataStruct->irBuf[bufLen] = Log1[0];
               	IR_DataStruct->irBuf[bufLen+1] = Log1[1];
            }
            else
            {
            	/* Logical 0 data */
             	IR_DataStruct->irBuf[bufLen] = Log0[0];
               	IR_DataStruct->irBuf[bufLen+1] = Log0[1];       
            }
            bufLen += MAX_LOG_WAVFORM_SIZE;
        }
	}
	/* Encode stop code */
    if(IR_Protocol->stopBuf != 0)
    {
        IR_DataStruct->irBuf[bufLen] = ConvertToCarrierCycle(IR_Protocol->stopBuf,IR_DataStruct->carrierFreq);
        bufLen++;
    }
	IR_DataStruct->bufLen = bufLen; 
	
	return IR_SUCCEED;
}

/**
  * @brief    Encode data of RCA protocol.
  * @param  address: user code.
  * @param   command: data code.
  * @param   IR_DataStruct: pointer to struct which store RCA code.
  * @retval None
  */
IR_RETURN_Type IR_RCA_Encode(uint8_t address, uint8_t command, IR_DataTypeDef *IR_DataStruct)
{
	IR_DataStruct->carrierFreq = 38;
	IR_DataStruct->codeLen= 4;
	IR_DataStruct->code[0] = address;
	IR_DataStruct->code[1] = command;
	IR_DataStruct->code[2] = ~address;
	IR_DataStruct->code[3] = ~command;

	return RCA_EncodePacket(IR_DataStruct, (IR_ProtocolTypeDef*)(&RCA_PROTOCOL));
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

