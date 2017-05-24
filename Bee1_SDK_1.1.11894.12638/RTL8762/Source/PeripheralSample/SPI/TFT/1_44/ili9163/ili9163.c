/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ili9163.c
* @brief    This file provides drivers for ili9163 which uesd in 1.44 TFT LCD display.
* @details
* @author   elliot chen
* @date     2016-02-24
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "ili9163.h"

/* Global Variables ------------------------------------------------------------*/

/* Configure scan mode of LCD */
static uint8_t lcd_scan_mode = LCD_SCAN_MEMORY_RL_UD;

/**
  * @brief  Write a command to ili9163.
  * @param  command: command which want to write to ili9163.
  * @retval None
  */
void LCD_WriteCmd(uint8_t command)
{
	LCD_AO_RESET;

	while(SPI_GetFlagState(LCD_SPI, SPI_FLAG_TFE) == false);
	SPI_SendData(LCD_SPI, command);

	/* Waiting for SPI data transfer to end */
	while(SPI_GetFlagState(LCD_SPI, SPI_FLAG_BUSY));	
}

/**
  * @brief  Write data to ili9163.
  * @param  data: data which want to write to ili9163.
  * @retval None
  */
void LCD_WriteData(uint8_t data)
{
	LCD_AO_SET;
	
	while(SPI_GetFlagState(LCD_SPI, SPI_FLAG_TFE) == false);
	SPI_SendData(LCD_SPI, data);

	/* Waiting for SPI data transfer to end */
	while(SPI_GetFlagState(LCD_SPI, SPI_FLAG_BUSY));
}

/**
  * @brief  Configure section parameters .
  * @param  x1: column start address.
  * @param  y1: page start address.
  * @param  x2: column end address.
  * @param  y2: page end address.
  * @retval None
  */
void LCD_SectionConfig(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	/* Set column address */
	LCD_WriteCmd(0x2A);
	LCD_WriteData(0x00);
	LCD_WriteData(x1);
	LCD_WriteData(0x00);
	LCD_WriteData(x2);

	/* Set page address */
	LCD_WriteCmd(0x2B);
	LCD_WriteData(0x00);
	LCD_WriteData(y1+32);
	LCD_WriteData(0x00);
	LCD_WriteData(y2+32);

    LCD_WriteCmd(0x2C);
}

/**
  * @brief  Configure LCD scan mode.
  * @param  scanmode: Specified scan mode which can be LCD_SCAN_MEMORY_RL_UD or LCD_SCAN_MEMORY_UD_RL.
  * @retval None
  */
void LCD_ScanModeConfig(uint8_t scanmode)
{
	lcd_scan_mode = scanmode;
}

/**
  * @brief  delay .
  * @param  uMs: the time of delay.
  * @retval None
  */
void delay_ms(uint32_t uMs)
{
	uint32_t i;
	uMs=uMs*4000;
	for(i=0;i<uMs;i++)
	{
		__asm volatile("nop"); 
		__asm volatile("nop");
		__asm volatile("nop");
		__asm volatile("nop");
	}
}

/**
  * @brief  Initializes the ili9163 according to the specified parameters.
  * @param  None.
  * @retval None
  */
void ili9163_Init(void)
{
	/*------------------Reset LCD driver-------------------*/
 	/* low to hign reset LCD */
	LCD_RST_RESET;
	delay_ms(150);
	LCD_RST_SET;
	delay_ms(50);
	
	LCD_CS_RESET;
	LCD_WriteCmd(0x11); 	/* Exit Sleep */
	delay_ms(50);
	LCD_WriteCmd(0x26); 	/*Set Default Gamma */
	LCD_WriteData(0x04);
	LCD_WriteCmd(0xB1);		/* Set Frame Rate */
	LCD_WriteData(0x0e);
	LCD_WriteData(0x10);
	LCD_WriteCmd(0xC0); 	/*Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD */
	LCD_WriteData(0x08);
	LCD_WriteData(0x00);
	
	LCD_WriteCmd(0xC1); 	/*Set BT[2:0] for AVDD & VCL & VGH & VGL */
	LCD_WriteData(0x05);
	
	LCD_WriteCmd(0xC5); 	/* Set VMH[6:0] & VML[6:0] for VOMH & VCOML */
	LCD_WriteData(0x38);
	LCD_WriteData(0x40);
	
	LCD_WriteCmd(0x3a); 	/* Set Color Format */
	LCD_WriteData(0x05);

	LCD_WriteCmd(0x36); 	/* RGB */
	
	if(lcd_scan_mode == LCD_SCAN_MEMORY_RL_UD)
    {
    	LCD_WriteData(0x18);
	}
    else
	{
		if(lcd_scan_mode == LCD_SCAN_MEMORY_UD_RL)
		{
			LCD_WriteData(0x28); 
		}
    }

    /*------------Configure display parameter------------------*/
    /* Set Column Address*/
	LCD_WriteCmd(0x2A);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x7F);
	/* Set Page Address */
	LCD_WriteCmd(0x2B); 
	LCD_WriteData(0x00);
	LCD_WriteData(32);
	LCD_WriteData(0x00);
	LCD_WriteData(127+32);

	LCD_WriteCmd(0xB4); 
	LCD_WriteData(0x00);

	/* Enable Gamma bit */
	LCD_WriteCmd(0xf2);
	LCD_WriteData(0x01);
	LCD_WriteCmd(0xE0); 
	LCD_WriteData(0x3f);//p1
	LCD_WriteData(0x22);//p2
	LCD_WriteData(0x20);//p3
	LCD_WriteData(0x30);//p4
	LCD_WriteData(0x29);//p5
	LCD_WriteData(0x0c);//p6
	LCD_WriteData(0x4e);//p7
	LCD_WriteData(0xb7);//p8
	LCD_WriteData(0x3c);//p9
	LCD_WriteData(0x19);//p10
	LCD_WriteData(0x22);//p11
	LCD_WriteData(0x1e);//p12
	LCD_WriteData(0x02);//p13
	LCD_WriteData(0x01);//p14
	LCD_WriteData(0x00);//p15
	LCD_WriteCmd(0xE1); 
	LCD_WriteData(0x00);//p1
	LCD_WriteData(0x1b);//p2
	LCD_WriteData(0x1f);//p3
	LCD_WriteData(0x0f);//p4
	LCD_WriteData(0x16);//p5
	LCD_WriteData(0x13);//p6
	LCD_WriteData(0x31);//p7
	LCD_WriteData(0x84);//p8
	LCD_WriteData(0x43);//p9
	LCD_WriteData(0x06);//p10
	LCD_WriteData(0x1d);//p11
	LCD_WriteData(0x21);//p12
	LCD_WriteData(0x3d);//p13
	LCD_WriteData(0x3e);//p14
	LCD_WriteData(0x3f);//p15

	/* Display on */
	LCD_WriteCmd(0x29);
	LCD_WriteCmd(0x2C);
}

/**
  * @brief  Clear the LCD according to the specified parameters.
  * @param  color: color of LCD.
  * @retval None
  */
void LCD_Clear(uint16_t color)
{
	uint8_t VH,VL;
	uint16_t i,j;
	VH = color >> 8;
	VL = color;	
    
	LCD_SectionConfig(0, 0, LCD_W-1, LCD_H-1);
    
    for(i=0; i<LCD_W; i++)
	{
	    for (j=0; j<LCD_H; j++)
	   	{
         	LCD_WriteData(VH);
			LCD_WriteData(VL);	
	    }
	}
}

/**
  * @brief  Display picture by SPI.
  * @param  address: address of picture data.
  * @retval None
  */
void LCD_DisplayImage(uint8_t* address)
{
    uint32_t i = 0;
    LCD_SectionConfig(0, 0, LCD_W-1, LCD_H-1);
    
    for(i=0; i<32768; i++)
    {
        LCD_WriteData(*address++);
    }
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

