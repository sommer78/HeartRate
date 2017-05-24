/**
*********************************************************************************************************
*               Copyright(c) 2017, ETEK Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      cal_funcation.h
* @brief     header file of cal_funcation.c
* @details   
* @author    jiangs
* @date      2017-05-12
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _CAL_FUNCATION_H_
#define _CAL_FUNCATION_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "rtl876x.h"
//#include "stm32f10x.h"
/*
 * Defines the prototype to which the application bpm  function must conform.
 *  
 */
 /**<      > */
int getArrayMaxIndex(uint16_t *array,int len);
int getArrayMinIndex(uint16_t *array,int len);
void pushArrayData(uint16_t array[],int length,uint16_t data);

uint16_t getArrayAverageWithoutPeak(uint16_t *array,int length);
uint16_t getArrayAverage(uint16_t *array,int length);

uint8_t getSlopeType(uint16_t before,uint16_t after);

uint16_t getSlopeValue(uint16_t before,uint16_t after);

uint8_t HexToAscii(uint8_t hex);


#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_ADC_H_ */

