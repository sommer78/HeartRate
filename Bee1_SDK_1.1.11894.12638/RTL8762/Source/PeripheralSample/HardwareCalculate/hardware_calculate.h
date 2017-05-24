/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hardware_calculate.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-2-29
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_CALCULATE_H
#define __HARDWARE_CALCULATE_H

#ifdef __cpluspuls
 extern "C" {
#endif
     
/* Includes ------------------------------------------------------------------*/
#include"rtl876x.h"
     
/* Defines ------------------------------------------------------------------*/

/*--------Select unit of input parameters-----------------*/

/* If uint of input parameter is PI, please set this define to 1. */
/* If uint of input parameter is radian, please reset to 0 */
#define UNIT_IS_PI					0

/* Optimize performance hardware calculation if this define is 1 */
#define OPTIMIZE_PERFORMANCE		1

/* Configure data type of floating point operation which can be float or double */
#define ALU_IN_TYPE				float   /*!< can be configure to double */
#define ALU_OUT_TYPE			float   /*!< can be configure to double */


typedef struct 
{                                    
	__IO  uint32_t CTL;
	__IO  uint32_t IN0;
	__IO  uint32_t IN1;
	__IO  uint32_t OUT0;
	__IO  uint32_t OUT1;
} HW_ALU_TypeDef;

typedef struct
{
	ALU_OUT_TYPE x;
	ALU_OUT_TYPE y;
}COORDINATE;

void ALU_Cmd(FunctionalState NewState);
ALU_OUT_TYPE sina(ALU_IN_TYPE x);
ALU_OUT_TYPE cosa(ALU_IN_TYPE x);
ALU_OUT_TYPE tana(ALU_IN_TYPE x);
void RotateTransformByAngle(ALU_IN_TYPE x, ALU_IN_TYPE y, ALU_IN_TYPE phase, COORDINATE *coordinate);
ALU_OUT_TYPE arctana(ALU_IN_TYPE x);
ALU_OUT_TYPE sqrta(ALU_IN_TYPE x, ALU_IN_TYPE y);
ALU_OUT_TYPE RotateToAngle(ALU_IN_TYPE x, ALU_IN_TYPE y);

#ifdef __cplusplus
}
#endif

#endif /*__HARDWARE_CALCULATE_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

