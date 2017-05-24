/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_spi.c
* @brief    This file provides all the Spi firmware functions.
* @details
* @author   elliot chen
* @date     2015-05-06
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_spi.h"
#include "rtl876x_rcc.h"

/**
  * @brief  Deinitializes the SPIx peripheral registers to their default reset values.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @retval None
  */
void SPI_DeInit(SPI_TypeDef* SPIx)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx)); 
    
    /*Disable SPI clock */
    if(SPIx == SPI0)
    {
        RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, DISABLE);
    }
    else
    {
        RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, DISABLE);
    }   
}

/**
  * @brief  Initializes the SPIx peripheral according to the specified 
  *   parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *   contains the configuration information for the specified SPI peripheral.
  * @retval None
  */
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx)); 

    /* Check the SPI parameters */
    assert_param(IS_SPI_CLOCK_SPEED(SPI_InitStruct->SPI_ClockSpeed));
    assert_param(IS_SPI_DIRECTION_MODE(SPI_InitStruct->SPI_Direction));
    assert_param(IS_SPI_MODE(SPI_InitStruct->SPI_Mode));
    assert_param(IS_SPI_DATASIZE(SPI_InitStruct->SPI_DataSize));
    assert_param(IS_SPI_CPOL(SPI_InitStruct->SPI_CPOL));
    assert_param(IS_SPI_CPHA(SPI_InitStruct->SPI_CPHA));
    assert_param(IS_SPI_FRAME_FORMAT(SPI_InitStruct->SPI_FrameFormat));

	/* Configure SPI0 mode if select SPI0 */
	if(SPIx == SPI0)
	{
		if(SPI_InitStruct->SPI_Mode == SPI_Mode_Master)
	    {
	        /*Enable SPI0 master mode*/
	        *((uint32_t *)0x40000308) = *((uint32_t *)0x40000308)|BIT(8);
	    }
	    else
	    {
	        /*Enable SPI0 slave mode*/
	        *((uint32_t *)0x40000308) = *((uint32_t *)0x40000308)& (~(BIT(8)));
	    }
	}

    /* Disable SPI device before change configuration */
    SPIx->SSIENR &= ~0x01;
    
    /* ----------------------------Initialize SPI device ------------------------ */
    /* configure SPI parameters */
    SPIx->CTRLR0 = SPI_InitStruct->SPI_DataSize
                    | (SPI_InitStruct->SPI_FrameFormat << 4)
                    | (SPI_InitStruct->SPI_CPHA << 6)
                    | (SPI_InitStruct->SPI_CPOL << 7)
                    | (SPI_InitStruct->SPI_Direction << 8);
    
    /* configure SPI clock speed  in master mode or  enable slave output in slave mode */
    if(SPI_InitStruct->SPI_Mode == SPI_Mode_Master)
    {
		SPIx->BAUDR = (SystemClock/(SPI_InitStruct->SPI_ClockSpeed))%0xFFFF;
        /* Enable slave Select function in master mode */
        SPIx->SER |= BIT(0);
    }
	else
	{
		/* Enable slave output */
		SPIx->CTRLR0 &= ~(BIT(10));		
	}
    
    /*set SPI Tx and Rx threshold level ,below this level or equal this level would trigger Tx and Rx FIFO empty interrupt */
    SPIx->TXFTLR = SPI_InitStruct->SPI_TxThresholdLevel;
    SPIx->RXFTLR = SPI_InitStruct->SPI_RxThresholdLevel;

    /* mask SPI interrupt in REG_DW_SSI_IMR */
    SPIx->IMR = 0;

    /* set read length in SPI EEPROM mode */
    if((SPI_InitStruct->SPI_Direction == SPI_Direction_RxOnly) || (SPI_InitStruct->SPI_Direction == SPI_Direction_EEPROM))
    {
        SPIx->CTRLR1 = SPI_InitStruct->SPI_NDF;
    }
}

/**
  * @brief  Fills each SPI_InitStruct member with its default value.
  * @param  SPI_InitStruct : pointer to a SPI_InitTypeDef structure which will be initialized.
  * @retval None
  */
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct)
{
    SPI_InitStruct->SPI_Mode              = SPI_Mode_Master;
    SPI_InitStruct->SPI_DataSize          = SPI_DataSize_8b;    /* 8-bit serial data transfer */
    SPI_InitStruct->SPI_FrameFormat       = SPI_Frame_Motorola;
    SPI_InitStruct->SPI_Direction         = SPI_Direction_FullDuplex; 
    SPI_InitStruct->SPI_CPOL              = SPI_CPOL_High;      /* SPI_CPOL_Low:inactive state of clock is low; SPI_CPOL_High:inactive state of clock is high */
    SPI_InitStruct->SPI_CPHA              = SPI_CPHA_2Edge;                                                          
    SPI_InitStruct->SPI_ClockSpeed        = 500000;             /* Uint: Hz */
    SPI_InitStruct->SPI_TxThresholdLevel  = 1;                  /* Transmit FIFO Threshold */
    SPI_InitStruct->SPI_RxThresholdLevel  = 0;                  /* Receive FIFO Threshold */    
    SPI_InitStruct->SPI_NDF               = 1;
}

/**
  * @brief  Enables or disables the specified SPI peripheral.
  * @param  SPIx: where x can be 0 or 1 to select the SPI peripheral.
  * @param  NewState: new state of the SPIx peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
  
    if (NewState != DISABLE)
    {
        /* Enable the selected SPI peripheral */
        SPIx->SSIENR |= 0x01;
    }
    else
    {
        /* Disable the selected SPI peripheral */
        SPIx->SSIENR &= ~0x01;
    }
}

/**
  * @brief  Transmits a number of bytes through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @param  Data : bytes to be transmitted.
  * @retval None
  */
void SPI_SendBuffer(SPI_TypeDef* SPIx, UINT8 *pBuf, UINT16 len)
{
    UINT16 i = 0;
    
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));

    for(i = 0; i < len; i++)
    {
        SPIx->DR[0] = (UINT16)(*pBuf++);  
        /*read TFNF bit status in SR register: SET is Tx FIFO is not full*/
        while(!(SPIx->SR & BIT(1)));
    }   
}

/**
  * @brief  Enables or disables the specified SPI/I2S interrupts.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_IT: specifies the SPI/I2S interrupt source to be enabled or disabled. 
  *   This parameter can be one of the following values:
  *     @arg SPI_INT_TXE: Tx buffer empty interrupt mask
  *     @arg SPI_INT_TXO: Tx buffer overflow interrupt mask
  *     @arg SPI_INT_RXU: receive FIFO Underflow Interrupt mask
  *     @arg SPI_INT_RXO: receive FIFO Overflow Interrupt mask
  *     @arg SPI_INT_RXF: receive FIFO Full Interrupt mask which equal RXNE Interrupt!!!
  *     @arg SPI_INT_MST: multi-Master Contention Interrupt mask
  * @param  NewState: new state of the specified SPI interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_INTConfig(SPI_TypeDef* SPIx, uint8_t SPI_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_SPI_CONFIG_IT(SPI_IT));

    if (NewState != DISABLE)
    {
        /* Enable the selected SPI interrupt */
        SPIx->IMR |= SPI_IT;
    }
    else
    {
        /* Disable the selected SPI interrupt */
        SPIx->IMR &= (uint16_t)~(SPI_IT);
    }
}

/**
  * @brief  Clear the specified SPI interrupt.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_IT: specifies the SPI interrupt to clear. 
  *   This parameter can be one of the following values:
  *     @arg SPI_INT_MST: Multi-Master Contention Interrupt.
  *     @arg SPI_INT_RXO: Receive FIFO Overflow Interrupt.
  *     @arg SPI_INT_RXU: Receive FIFO Underflow Interrupt.
  *     @arg SPI_INT_TXO: Transmit FIFO Overflow Interrupt .
  * @retval None.
  */
void SPI_ClearINTPendingBit(SPI_TypeDef* SPIx, uint16_t SPI_IT)
{
	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(IS_SPI_CONFIG_IT(SPI_IT));

	switch(SPI_IT)
	{
		case SPI_INT_RXO:
			SPIx->RXOICR;
			break;
		case SPI_INT_RXU:
			SPIx->RXUICR;
			break;
		case SPI_INT_TXO:
			SPIx->TXOICR;
			break;
		case SPI_INT_MST:
			SPIx->RSVD_44;
			break;
		default:
			break;
	}

}
/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

