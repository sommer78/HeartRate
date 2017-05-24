/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	       This file provides demo code of I2C repeat read by GDMA.
* @details
* @author	elliot chen
* @date 		2016-03-29
* @version	v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "repeat_read_by_gdma.h"

/* Global variables ------------------------------------------------------------*/
/* Specifiy buffer size must reference maximum size of I2C_SendBuf */
uint8_t I2C_RevBuf[I2C_Buf_Maxsize];

/**
  * @brief  this sample code is for user to see procedures of I2C repeat read by GDMA.
  * @note: user can Refer to the following procedures.
  * @param  none.
  * @retval None
  */
void sample_code(void)
{
	uint8_t writeBuf[10] = {0x00,0x00,0x11,0x22};
    uint32_t i = 0;
    
    for(i=2; i<10; i++)
    {
        writeBuf[i] = i;
    }
	
    Board_I2CInit();
    Driver_I2C_GDMA_Init();

	I2C_RepeatReadByGDMA(writeBuf, 2, I2C_RevBuf, 5);

	/* Delay some time to ensure GDMA transmission finish */
	for(i=0; i<0xfffff; i++);

	I2C_RepeatReadByGDMA(writeBuf, 2, I2C_RevBuf, 6);
}

/**
* @brief  main() is the entry of user code.
* @param   No parameter.
* @return  void
*/
int main(void)
{
    sample_code();
    while(1);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

