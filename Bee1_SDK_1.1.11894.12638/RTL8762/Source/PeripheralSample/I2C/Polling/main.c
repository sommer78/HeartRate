/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of I2C.
* @details
* @author	elliot chen
* @date 	2015-06-10
* @version	v0.1
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
#define I2C0_SCL				P1_6		//C9: 0
#define I2C0_SDA				P1_7		//C9: 1

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_i2c.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"

/**
  * @brief read pinmux value of pin.
  * @param  GPIOGroup: which can be 0 to 4.
  * @param  GPIOIndex: where can be 0 to 7 .if GPIOGroup is 4,then GPIOIndex must be 0 to 5.
  * @retval None
  */
uint8_t Pinmux_ReadPinStatus(uint8_t GPIOGroup, uint8_t GPIOIndex)
{	
    UINT32 reg_addr  = 0x0;
    UINT32 reg_value = 0x0;
    UINT8 pin_status = 0x0;

    reg_addr = 0x280 + GPIOGroup * 8 + (GPIOIndex/4)*4;
    reg_value = *((uint32_t*)(0x40000000 + reg_addr));
    pin_status = (reg_value>>(8*(GPIOIndex%4)))&0x7f;
	return pin_status;
}

/*I2C demo code---------------------------------------------------------------*/
void RCC_Configuration(void)
{
	/* turn on I2C clock */
    RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
}

void I2C_PINMUXConfiguration(void)
{
    Pinmux_Config(I2C0_SCL, I2C0_CLK);
	Pinmux_Config(I2C0_SDA, I2C0_DAT);

}

void I2C_PADConfiguration(void)
{
    Pad_Config(I2C0_SCL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(I2C0_SDA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);	

}

void I2C_Configuration(void)    
{
    I2C_InitTypeDef  I2C_InitStructure;
    
    I2C_InitStructure.I2C_ClockSpeed = 100000;
  	I2C_InitStructure.I2C_DeviveMode = I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode = I2C_AddressMode_7BIT;
  	I2C_InitStructure.I2C_SlaveAddress = 0x50;
  	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  	
  	I2C_Init(I2C0, &I2C_InitStructure);
  	I2C_Cmd(I2C0, ENABLE); 	
}

void I2C_TestCode(void)
{
    uint8_t I2C_WriteBuf[16] = {0x00,0x00,0x66,0x68, 0x77, 0x88};
    uint8_t I2C_ReadBuf[16] = {0, 0 , 0, 0};
    //I2C_MasterWrite(I2C0, I2C_WriteBuf, 5);
    I2C_RepeatRead(I2C0, I2C_WriteBuf, 2, I2C_ReadBuf, 4);
}

int main(void)
{
    /* I2C0 sck and sda pin is P2_4 and P2_5 by default.if use other pin, it should deint its pinmux status first */
    Pinmux_Deinit(P2_4);
    Pinmux_Deinit(P2_5);
    
    RCC_Configuration();
    I2C_PINMUXConfiguration();
    I2C_PADConfiguration();
    I2C_Configuration();

    I2C_TestCode();
    while(1);
}
