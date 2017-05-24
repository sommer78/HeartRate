/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      lcd_application.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-2-24
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_APPLICATION_H
#define __LCD_APPLICATION_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Defines ------------------------------------------------------------------*/

/* GDMA Configuration */
#define LCD_GDMA_CHANNEL_NUM   	0
#define LCD_GDMA_CHANNEL    	GDMA_Channel0
#define LCD_GDMA_IRQ            GDMA0_CHANNEL0_IRQ
#define LCD_GDMA_IntrHandler	Gdma0Ch0IntrHandler

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_nvic.h"
#include "ili9163.h"

void LCD_Demo(void);

#ifdef __cplusplus
}
#endif

#endif /*__LCD_APPLICATION_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

