/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ili9163.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-2-24
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ILI9163_H
#define __ILI9163_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Defines ------------------------------------------------------------------*/

/* User defines------- -------------------------------------------------------*/
/* LCD parameter configuration */
#define LCD_W 					128
#define LCD_H 					128

/* SPI used by LCD */
#define LCD_SPI                 SPI0
#define LCD_SCK					P4_0
#define LCD_MOSI				P4_1
#define LCD_MISO				P4_2		 
#define LCD_CS              	P3_2

/* GPIO used by LCD */
#define LCD_RST             	P2_0
#define LCD_AO              	P2_1

#define LCD_RST_PIN         	GPIO_GetPin(LCD_RST) 
#define LCD_CS_PIN          	GPIO_GetPin(LCD_CS) 
#define LCD_AO_PIN          	GPIO_GetPin(LCD_AO) 

#define LCD_AO_SET         		{GPIO_SetBits(LCD_AO_PIN);}
#define LCD_AO_RESET       		{GPIO_ResetBits(LCD_AO_PIN);}

#define LCD_CS_SET         		{GPIO_SetBits(LCD_CS_PIN);}
#define LCD_CS_RESET       		{GPIO_ResetBits(LCD_CS_PIN);}

#define LCD_RST_SET         	{GPIO_SetBits(LCD_RST_PIN);}
#define LCD_RST_RESET       	{GPIO_ResetBits(LCD_RST_PIN);}

/* Internal defines -----------------------------------------------------------*/
/* Paint brush */
#define WHITE					0xffff
#define BLACK					0x0000
#define BLUE					0x001f
#define RED						0xf800
#define YELLOW					0xffe0
#define BROWN					0xbc40

typedef enum
{
    LCD_SCAN_MEMORY_RL_UD = 0,   /* left->right  up->down */
    LCD_SCAN_MEMORY_UD_RL        /* up->down left->right */
}TFT_Scan_Mode;

/* Includes ------------------------------------------------------------------*/
#include"rtl876x_spi.h"
#include"rtl876x_gdma.h"
#include"rtl876x_gpio.h"


#ifdef __cplusplus
}
#endif

void LCD_SectionConfig(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void ili9163_Init(void);
void delay_ms(uint32_t uMs);
void LCD_Clear(uint16_t color);
void LCD_DisplayImage(uint8_t* address);

#endif /*__ILI9163_H*/

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

