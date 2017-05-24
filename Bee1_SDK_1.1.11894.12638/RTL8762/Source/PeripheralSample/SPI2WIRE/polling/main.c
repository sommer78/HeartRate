/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of two wire SPI comunication.
* @details
* @author	elliot chen
* @date 	2015-10-30
* @version	v1.0
*********************************************************************************************************
*/
#define SENSOR_SPI              SPI2WIRE
#define SENSOR_SPI_CLK			P4_0		 
#define SENSOR_SPI_DATA			P4_1

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_3wire_spi.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

void RCC_Configuration(void)
{
	/* turn on  two wire SPI clock */
    RCC_PeriphClockCmd(APBPeriph_SPI2W, APBPeriph_SPI2W_CLOCK, ENABLE);
}

void SPI_PINMUXConfiguration(void)
{
    Pinmux_Config(SENSOR_SPI_CLK, SPI2W_CLK_Master);
    Pinmux_Config(SENSOR_SPI_DATA, SPI2W_DATA_Master);
}

void SPI_PADConfiguration(void)
{
    Pad_Config(SENSOR_SPI_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_DATA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void SPI_InitConfig(void)    
{
    SPI3WIRE_InitTypeDef SPI3WIRE_InitStruct;
	SPI3WIRE_StructInit(SENSOR_SPI, &SPI3WIRE_InitStruct);

	//delay time = (0x06 + 3)/(2 * speed) = 3us
	SPI3WIRE_InitStruct.readDelay  = 0x3;
	SPI3WIRE_InitStruct.mode       = SPI3WIRE_2WIRE_MODE;
    SPI3WIRE_InitStruct.oeDelayCfg = SPI3WIRE_OE_DELAY_1T;
    SPI3WIRE_InitStruct.endExtMode = SPI3WIRE_EXTEND_MODE;
    SPI3WIRE_InitStruct.speed      = 1000000;              
	SPI3WIRE_Init(SENSOR_SPI, &SPI3WIRE_InitStruct);
}

/*-------------------------- two wire SPI demo code----------------------------------*/
/**
 * @brief read one byte through 3wire SPI perpherial .
 * @param  address: address of register which need to read .
 * @return value of register.
*/
uint8_t Mouse_SingleRead(uint8_t address)
{
	uint8_t reg_value = 0;
	uint32_t timeout = 0;

	/* Check SPI busy or not */
    while(SPI3WIRE_GetFlagStatus(SENSOR_SPI,SPI3WIRE_FLAG_BUSY)==SET)
    {
        timeout++;
	    if(timeout > 0xffff)
	    {
            break;
	    }  
    }

	/* Clear Rx FIFO number */
	SPI3WIRE_ClearReadNumber(SENSOR_SPI);
	
	SPI3WIRE_StartSignalRead(SENSOR_SPI, address);
	
	while(1)
	{
	    /* Wait communication to end */
    	if(SPI3WIRE_GetFlagStatus(SENSOR_SPI,SPI3WIRE_FLAG_BUSY)==SET)
    	{
    	    break;
    	}
	    timeout++;
	    if(timeout > 0xffff)
	    {
            break;
	    }  
	}

    /* Get Rx FIFO length */
    while(SPI3WIRE_GetReadDataNum(SENSOR_SPI) == 0);
	/* Read data */
	SPI3WIRE_ReadData(SENSOR_SPI, 1, &reg_value);

	return reg_value;
}

/**
 * @brief write one byte.
 * @param address: address of register which need to write data.
 * @param data: data which need to write.
 * @return TRUE: write success, FALSE: write failure.
*/
BOOL Mouse_SingleWrite(uint8_t address, uint8_t data)
{
    uint32_t delay = 0;
    uint32_t timeout = 0;

    /* Check SPI busy or not */
    while(SPI3WIRE_GetFlagStatus(SENSOR_SPI,SPI3WIRE_FLAG_BUSY)==SET)
    {
        timeout++;
	    if(timeout > 0xffff)
	    {
            return FALSE;
	    }  
    }
    /* Write data */
	SPI3WIRE_StartWrite(SENSOR_SPI, address , data);

	while(1)
	{
	    /* Wait communication to end */
    	if(SPI3WIRE_GetFlagStatus(SENSOR_SPI,SPI3WIRE_FLAG_BUSY)==SET)
    	{
    	    break;
    	}
	    timeout++;
	    if(timeout > 0xffff)
	    {
            return FALSE;
	    }  
	}
	return TRUE;
}

void SPI2WIRE_DemoCode(void)
{
    uint8_t reg_value = 0;
	uint8_t address = 0x00;
	
    /* Read one byte */
	reg_value = Mouse_SingleRead(address);
    /* Write one byte */
	Mouse_SingleWrite(address, reg_value);
}

int main(void)
{
    RCC_Configuration();
    SPI_PINMUXConfiguration();
    SPI_PADConfiguration();
    SPI_InitConfig();
    SPI2WIRE_DemoCode();
    while(1);
}
