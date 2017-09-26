/**
*********************************************************************************************************
*               Copyright(c) 2017, ETEK Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      simpress.h
* @brief     header file of simpress.c
* @details   
* @author    jiangs
* @date      2017-05-04
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _SIMPRESS_H_
#define _SIMPRESS_H_

#ifdef __cplusplus
 extern "C" {
#endif




typedef struct BLOOD_PRESS
{
	
    uint16_t  	high;    
	uint16_t  	low;   
	
} BLOOD_PRESS_T, *BLOOD_PRESS_INDEX;

uint16_t getBloodPress(uint16_t heartRate, int hour,int age,BLOOD_PRESS_T* bloodPress);



#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_ADC_H_ */

