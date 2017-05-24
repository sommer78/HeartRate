/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		lcd_application.c
* @brief	      This file provides demo code of LCD display which use SPI interface to send data by GDMA.
* @details
* @author	elliot chen
* @date 		2016-02-24
* @version	v1.0
*********************************************************************************************************
*/
extern const unsigned char gImage_1[32768];
extern const unsigned char gImage_2[32768];

/* Includes ------------------------------------------------------------------*/
#include "lcd_application.h"

/*Define --------------------------------------------------------------------*/
#define GDMA_SINGLE_MAX_SIZE    4000UL
/* Global variable defines ------------------------------------------------------*/

/* Record GDMA transfer count and the number of last GDMA transfer data */
volatile uint16_t GDMA_transfer_count = 0;
volatile uint16_t remain_number = 0;

/* Record GDMA interrupt number */
volatile uint16_t GDMA_interrupt_count = 0;

/**
  *@brief Initialize PINMUX for LCD display.
  *@param None.
  * @retval None
  */
void LCD_PinmuxConfig(void)
{
    Pinmux_Config(LCD_SCK, SPI0_CLK_Master);
	Pinmux_Config(LCD_MOSI, SPI0_MO_Master);
	Pinmux_Config(LCD_MISO, SPI0_MI_Master);
	//Pinmux_Config(LCD_CS, SPI0_SS_N_0_Master);

	Pinmux_Config(LCD_CS, GPIO_FUN);
	Pinmux_Config(LCD_RST, GPIO_FUN);
	Pinmux_Config(LCD_AO, GPIO_FUN);
}

/**
  *@brief Initialize PAD for LCD display.
  *@param None.
  * @retval None
  */
void LCD_PADConfig(void)
{
    Pad_Config(LCD_SCK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(LCD_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(LCD_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	//Pad_Config(LCD_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

	Pad_Config(LCD_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(LCD_RST, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(LCD_AO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

/**
  *@brief Initialize SPI for LCD display which use SPI interface to send data.
  *@param None.
  * @retval None
  */
void LCD_Write_Init(void)
{
	/* Initialize SPI module */
	SPI_InitTypeDef  SPI_InitStructure;
	SPI_DeInit(LCD_SPI);
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
    SPI_StructInit(&SPI_InitStructure);   
    SPI_InitStructure.SPI_Direction   = SPI_Direction_TxOnly;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 10000000;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
	SPI_InitStructure.SPI_NDF 		  = 0;
	SPI_Init(LCD_SPI, &SPI_InitStructure); 
 	SPI_Cmd(LCD_SPI, ENABLE);

	/* Initialize GPIO module */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin  = LCD_CS_PIN |LCD_RST_PIN | LCD_AO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_ITCmd = DISABLE;
	GPIO_Init(&GPIO_InitStruct);
	GPIO_SetBits(LCD_CS_PIN);
    GPIO_SetBits(LCD_RST_PIN);
    GPIO_SetBits(LCD_AO_PIN);
}

/**
  *@brief Initialize SPI and GDMA for LCD display which use SPI interface to send data by GDMA.
  *@param None.
  * @retval None
  */
void LCD_WriteByGDMA_Init(void)    
{
	/* SPI init */
    SPI_InitTypeDef  SPI_InitStructure;
	SPI_Cmd(LCD_SPI, DISABLE);
    SPI_StructInit(&SPI_InitStructure);   
    SPI_InitStructure.SPI_Direction   = SPI_Direction_TxOnly;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 10000000;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
	SPI_Init(LCD_SPI, &SPI_InitStructure); 

	/* GDMA init */
	GDMA_InitTypeDef GDMA_InitStruct;
	RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_StructInit(&GDMA_InitStruct);
	GDMA_InitStruct.GDMA_ChannelNum      = LCD_GDMA_CHANNEL_NUM;
  	GDMA_InitStruct.GDMA_DIR 			 = GDMA_DIR_MemoryToPeripheral;
  	GDMA_InitStruct.GDMA_BufferSize 	 = 0;
  	GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Inc;
	GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_SourceMsize	  = GDMA_Msize_8;
	GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_8;   
	GDMA_InitStruct.GDMA_SourceAddr 	 = (uint32_t)NULL;
	GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)LCD_SPI->DR;
	GDMA_InitStruct.GDMA_TransferType    = GDMA_TransferType_SPI0_TX;
	GDMA_InitStruct.GDMA_ChannelPriority = LCD_GDMA_CHANNEL_NUM;
    
    GDMA_Init(LCD_GDMA_CHANNEL, &GDMA_InitStruct);  
 	SPI_Cmd(LCD_SPI, ENABLE);
    
    /* GDMA IRQ configuration */
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = LCD_GDMA_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
    GDMA_INTConfig(LCD_GDMA_CHANNEL_NUM, GDMA_INT_Transfer,ENABLE);
}

/**
  *@brief Config address and length of display buffer.
  *@param pBuf: address of buffer.
  *@param len: length of buffer.
  * @retval None
  */
void LCD_SendBufferByGDMA(UINT32 pBuf, UINT32 len)
{
    GDMA_transfer_count = len / GDMA_SINGLE_MAX_SIZE;
    remain_number = len % GDMA_SINGLE_MAX_SIZE;

	/* Waiting for transmission to complete */
    while (GDMA_GetChannelStatus(LCD_GDMA_CHANNEL_NUM) == SET);
	
    GDMA_SetSourceAddress(LCD_GDMA_CHANNEL, pBuf);

    if (len > GDMA_SINGLE_MAX_SIZE)
    {
        GDMA_SetBufferSize(LCD_GDMA_CHANNEL, GDMA_SINGLE_MAX_SIZE);
    }
    else
    {
        GDMA_SetBufferSize(LCD_GDMA_CHANNEL, remain_number);
    }

    GDMA_Cmd(LCD_GDMA_CHANNEL_NUM, ENABLE);
}

/**
  *@brief demo code for LCD display.
  *@param None.
  * @retval None
  */
void LCD_Demo(void)
{
	LCD_PADConfig();
	LCD_PinmuxConfig();
    LCD_Write_Init();
	ili9163_Init();
	LCD_Clear(BLUE);
	//LCD_DisplayImage(gImage_1);
    //LCD_SectionConfig(0, 0, LCD_W-1, LCD_H-1);
    
	LCD_WriteByGDMA_Init();
    while(1)
   	{
		LCD_SendBufferByGDMA((uint32_t)gImage_1, 32768);
		delay_ms(500);
		LCD_SendBufferByGDMA((uint32_t)gImage_2, 32768);
		delay_ms(500);
	}
}

/**
  * @brief  GDMA interrupt handle function which control GDMA transmission process.
  * @param  None.
  * @return None.
  */
void LCD_GDMA_IntrHandler(void)
{
    GDMA_interrupt_count++;

    if(GDMA_interrupt_count < GDMA_transfer_count)
    {
        /* Clear GDMA transfer interrupt */
        GDMA_BASE->CLEAR_TFR = BIT(LCD_GDMA_CHANNEL_NUM);

        /* Enable the selected GDMA Channel */
        GDMA_BASE->ChEnReg = BIT(LCD_GDMA_CHANNEL_NUM) | BIT(LCD_GDMA_CHANNEL_NUM + 8);
    }
    else
    {	
    	/* Transmission size > single block maxium transmission size */
        if(GDMA_transfer_count > 0)
        {
        	/* Transmission size % single block maxium transmission size !=0 */
            if(remain_number > 0)
            {
                LCD_GDMA_CHANNEL->CTL_HIGH = remain_number;
				/* Clear Record of GDMA transfer count and can be used for next GDMA transmission */
                remain_number = 0;
                GDMA_transfer_count = 0;

                /* Clear GDMA transfer interrupt */
                GDMA_BASE->CLEAR_TFR = BIT(LCD_GDMA_CHANNEL_NUM);

                /* Enable the selected GDMA Channel */
                GDMA_BASE->ChEnReg = BIT(LCD_GDMA_CHANNEL_NUM) | BIT(LCD_GDMA_CHANNEL_NUM + 8);
            }
			else
			{
				/* Clear Record of GDMA transfer count and can be used for next GDMA transmission */
				GDMA_transfer_count = 0;
				/* Clear GDMA transfer interrupt */
                GDMA_BASE->CLEAR_TFR = BIT(LCD_GDMA_CHANNEL_NUM);
				/* Clear GDMA interrupt number */
            	GDMA_interrupt_count = 0;
			}
        }
        else
        {
        	remain_number = 0;
            /* Clear GDMA transfer interrupt */
            GDMA_BASE->CLEAR_TFR = BIT(LCD_GDMA_CHANNEL_NUM);
			/* Clear GDMA interrupt number */
            GDMA_interrupt_count = 0;
        }
    }
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

