/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_gdma.c
* @brief    This file provides all the DMA firmware functions.
* @details
* @author   elliot chen
* @date     2015-05-08
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_gdma.h"
#include "rtl876x_rcc.h"

/**
  * @brief  Deinitializes the GDMA registers to their default reset
  *         values.
  * @param  None
  * @retval None
  */
void GDMA_DeInit(void)
{   
    /* Disable GDMA block in REG_SOC_FUNC_EN */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, DISABLE);
}

/**
  * @brief  Initializes the GDMA Channelx according to the specified
  *         parameters in the GDMA_InitStruct.
  * @param  GDMA_Channelx: where x can be 0 to 5 to select the DMA Channel.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that
  *         contains the configuration information for the specified DMA Channel.
  * @retval None
  */
void GDMA_Init(GDMA_ChannelTypeDef* GDMA_Channelx, GDMA_InitTypeDef* GDMA_InitStruct)
{
	uint32_t temp_bit = 0;
    uint32_t transfer_type = 0;
    uint32_t DMA_TDLR = 0;
    uint32_t DMA_RDLR = 0; 

    /* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
    assert_param(IS_GDMA_ChannelNum(GDMA_InitStruct->GDMA_ChannelNum));
    assert_param(IS_GDMA_DIR(GDMA_InitStruct->GDMA_DIR));
    assert_param(IS_GDMA_SourceInc(GDMA_InitStruct->GDMA_SourceInc));
    assert_param(IS_GDMA_DestinationInc(GDMA_InitStruct->GDMA_DestinationInc));
    assert_param(IS_GDMA_DATA_SIZE(GDMA_InitStruct->GDMA_SourceDataSize));
    assert_param(IS_GDMA_DATA_SIZE(GDMA_InitStruct->GDMA_DestinationDataSize));

    /*------------------configure source and destination address of GDMA---------*/
    /* program SARx register to set source address */
    GDMA_Channelx->SAR = GDMA_InitStruct->GDMA_SourceAddr;
    /* program DARx register to set destination address */
    GDMA_Channelx->DAR = GDMA_InitStruct->GDMA_DestinationAddr;
    
    /* configure MSize, TR_WIDTH, source and destination handshaking interface number */
    transfer_type = GDMA_InitStruct->GDMA_TransferType;

    /* Enable GDMA in DmaCfgReg*/
    GDMA_BASE->DmaCfgReg = 0x01;

    /* read ChEnReg to check channel is busy or not */
    if(GDMA_BASE->ChEnReg & BIT(GDMA_InitStruct->GDMA_ChannelNum))
    {
        //channel is be used
        //error handle code
    }

    /*--------------------------- GDMA Configuration -----------------*/
    /* clear pending interrupts of corresponding GDMA channel */
    GDMA_BASE->CLEAR_TFR = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_BLOCK = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_DST_TRAN = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_SRC_TRAN = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_ERR = BIT(GDMA_InitStruct->GDMA_ChannelNum);

    /* mask pending interrupts of corresponding GDMA channel */
    temp_bit = BIT(GDMA_InitStruct->GDMA_ChannelNum + 8); 
    GDMA_BASE->MASK_TFR = temp_bit;
    GDMA_BASE->MASK_BLOCK = temp_bit;
    GDMA_BASE->MASK_DST_TRAN = temp_bit;
    GDMA_BASE->MASK_SRC_TRAN = temp_bit;
    GDMA_BASE->MASK_ERR = temp_bit;
    
    /*---------------------------- configure CTL register --------------------------------*/
    
    /* configure low 32 bit of CTL register */
    GDMA_Channelx->CTL_LOW = BIT(0)
                        | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                        | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                        | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                        | (GDMA_InitStruct->GDMA_SourceInc << 9)
                        | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                        | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                        | (GDMA_InitStruct->GDMA_DIR << 20);
    /* configure high 32 bit of CTL register */
    GDMA_Channelx->CTL_HIGH = GDMA_InitStruct->GDMA_BufferSize;

    /*---------------------------- configure CFG register --------------------------------*/
    /* configure low 32 bit of CFG register */
    GDMA_Channelx->CFG_LOW = GDMA_InitStruct->GDMA_ChannelPriority << 5;

    /* configure peripheral parameters and configure source or destination hardware handshaking interface */
	/* mask source and destination handshaking interface */
	GDMA_Channelx->CFG_HIGH &=  ~(0x00ff << 7);

	if((transfer_type&0x0f) == GDMA_TransferType_UART0_TX)
	{
		/* enable uart clock and function */
		PERIPH->PERI_FUNC0_EN |= BIT(0);
		PERIPH->PERI_CLK_CTRL0 |= BIT(0);

		/* Mask uart TX threshold value, default value is 0x04 */
		UART->MISCR &= ~(0x0f << 3);
		
		/* set DMA FIFO threshold and enable uart DMA TX */
		if(GDMA_InitStruct->GDMA_DestinationMsize == GDMA_Msize_1)
		{
			UART->MISCR |= ((16 - 1) << 3)| BIT(1);
		}
		else
		{
			DMA_TDLR = 2<<(GDMA_InitStruct->GDMA_DestinationMsize);
			UART->MISCR |= ((16 - DMA_TDLR) << 3)| BIT(1);
		}

		/* Config destination handshaking interface */
		GDMA_Channelx->CFG_HIGH |= (GDMA_InitStruct->GDMA_TransferType&0x0f) << 11;
	}
				
	if((transfer_type&0x0f) == GDMA_TransferType_UART0_RX)
	{
		/* enable uart clock and function */
		PERIPH->PERI_FUNC0_EN |= (1 << 0);
		PERIPH->PERI_CLK_CTRL0 |= (1 << 0);

		/* Mask uart RX threshold value, default value is 0x04 */
		UART->MISCR &= ~(0x0f << 8);
		
		/* set DMA FIFO threshold and enable uart DMA RX */
		if(GDMA_InitStruct->GDMA_SourceMsize == GDMA_Msize_1)
		{
			UART->MISCR |= (1 << 8)|BIT(2);
		}
		else
		{
			DMA_RDLR = 2<<(GDMA_InitStruct->GDMA_SourceMsize);
			UART->MISCR |= (DMA_RDLR << 8)| BIT(2);
		}	

		/* Config source handshaking interface */
		GDMA_Channelx->CFG_HIGH |= (GDMA_InitStruct->GDMA_TransferType&0x0f) << 7;
	}
			
	if((transfer_type&0xf0) == GDMA_TransferType_SPI0_TX)
	{
		/* enable SPI clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_SPI0_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL0 |= SYSBLK_ACTCK_SPI0_EN_Msk;
		
		/*enable SPI DMA transmit and configure FIFO water mark*/
		SPI0->DMACR |= BIT(1);
		if(GDMA_InitStruct->GDMA_DestinationMsize == GDMA_Msize_1)
		{
			SPI0->DMATDLR = 24 - 1;
		}
		else
		{
			DMA_TDLR = 2 << (GDMA_InitStruct->GDMA_DestinationMsize);
			SPI0->DMATDLR = 24 - DMA_TDLR;
		}
		
		/* Config destination handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>4)&0x0f) << 11;
	}
			
	if((transfer_type&0xf0) == GDMA_TransferType_SPI0_RX)
	{
		/* enable SPI clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_SPI0_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL0 |= SYSBLK_ACTCK_SPI0_EN_Msk;
		
		/*enable SPI DMA receive */
		SPI0->DMACR |= BIT(0);
		/* set FIFO water mark */
		if(GDMA_InitStruct->GDMA_SourceMsize == GDMA_Msize_1)
		{
			SPI0->DMARDLR = 0;
		}
		else
		{
			SPI0->DMARDLR = (2<<(GDMA_InitStruct->GDMA_SourceMsize)) - 1;
		}

		/* Config source handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>4)&0x0f) << 7;
	}
			
	if((transfer_type&0xf00) == GDMA_TransferType_SPI1_TX)
	{
		/* enable SPI1 clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_SPI1_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL0 |= SYSBLK_ACTCK_SPI1_EN_Msk;
		
		/*enable SPI DMA transmit and configure FIFO water mark*/
		SPI1->DMACR |= BIT(1);
		if(GDMA_InitStruct->GDMA_DestinationMsize == GDMA_Msize_1)
		{
			SPI1->DMATDLR = 16 - 1;
		}
		else
		{
			DMA_TDLR = 2<<(GDMA_InitStruct->GDMA_DestinationMsize);
			SPI1->DMATDLR = 24 - DMA_TDLR;
		}

		/* Config destination handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>8)&0x0f) << 11;
	}
			
	if((transfer_type&0xf00) == GDMA_TransferType_SPI1_RX)
	{		
		/* enable SPI1 clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_SPI1_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL0 |= SYSBLK_ACTCK_SPI1_EN_Msk;
		
		/*enable SPI DMA receive */
		SPI1->DMACR |= BIT(0);
		/* set FIFO water mark */
		if(GDMA_InitStruct->GDMA_SourceMsize == GDMA_Msize_1)
		{
			SPI1->DMARDLR = 0;
		}
		else
		{
			SPI1->DMARDLR = (2<<(GDMA_InitStruct->GDMA_SourceMsize)) - 1;
		}

		/* Config source handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>8)&0x0f) << 7;
	}
	if((transfer_type&0xf000) == GDMA_TransferType_I2C0_TX)
	{
		/* enable I2C0 clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_I2C0_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL1 |= SYSBLK_ACTCK_I2C0_EN_Msk;
		
		/* Enable I2C transmit GDMA */
		I2C0->IC_DMA_CR |= BIT(1);

		/* Configure I2C GDMA transmit data level */
		if(GDMA_InitStruct->GDMA_DestinationMsize == GDMA_Msize_1)
		{
			I2C0->IC_DMA_TDLR = 16 - 1;
		}
		else
		{
			I2C0->IC_DMA_TDLR = 16 - (2<<(GDMA_InitStruct->GDMA_DestinationMsize));
		}
		/* Config destination handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>12)&0x0f) << 11;
	}
	if((transfer_type&0xf000) == GDMA_TransferType_I2C0_RX)
	{
		/* enable I2C0 clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_I2C0_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL1 |= SYSBLK_ACTCK_I2C0_EN_Msk;
		
		/* Enable I2C receive GDMA */
		I2C0->IC_DMA_CR |= BIT(0);

		/* Configure I2C GDMA receive data level */
		if(GDMA_InitStruct->GDMA_SourceMsize == GDMA_Msize_1)
		{
			/* Must configure to 0 which can read all data out */
			I2C0->IC_DMA_RDLR = 0;
		}
		else
		{
			I2C0->IC_DMA_RDLR = (2<<(GDMA_InitStruct->GDMA_SourceMsize)) - 1;
		}

		/* Config source handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>12)&0x0f) << 7;
	}
			
	if((transfer_type&0xf0000) == GDMA_TransferType_I2C1_TX)
	{
		/* enable I2C1 clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_I2C1_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL1 |= SYSBLK_ACTCK_I2C1_EN_Msk;
		
		/* Enable I2C transmit GDMA */
		I2C1->IC_DMA_CR |= BIT(1);

		/* Configure I2C GDMA transmit data level */
		if(GDMA_InitStruct->GDMA_DestinationMsize == GDMA_Msize_1)
		{
			I2C1->IC_DMA_TDLR = 16 - 1;
		}
		else
		{
			I2C1->IC_DMA_TDLR = 16 - (2<<(GDMA_InitStruct->GDMA_DestinationMsize));
		}
		/* Config destination handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>16)&0x0f) << 11;
	}
			
	if((transfer_type&0xf0000) == GDMA_TransferType_I2C1_RX)
	{
		/* enable I2C1 clock and function */
		SYSBLKCTRL->PERI_FUNC0_EN  |= SYSBLK_I2C1_EN_Msk;
		SYSBLKCTRL->PERI_CLK_CTRL1 |= SYSBLK_ACTCK_I2C1_EN_Msk;
		
		/* Enable I2C receive GDMA */
		I2C1->IC_DMA_CR |= BIT(0);

		/* Configure I2C GDMA receive data level */
		if(GDMA_InitStruct->GDMA_SourceMsize == GDMA_Msize_1)
		{
			I2C1->IC_DMA_RDLR = 0;
		}
		else
		{
			I2C1->IC_DMA_RDLR = (2<<(GDMA_InitStruct->GDMA_SourceMsize)) - 1;
		}

		/* Config source handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>16)&0x0f) << 7;
	}
			
	if((transfer_type&0xf00000) == GDMA_TransferType_ADC)
	{
		/* Config source handshaking interface */
		GDMA_Channelx->CFG_HIGH |= ((GDMA_InitStruct->GDMA_TransferType>>20)&0x0f) << 7;
	}
#if 0
	if((transfer_type&0xf00000) == GDMA_TransferType_Other) 
	{
		/* memory to memory transfer */
	}
#endif

	/* Config FIFO mode and Flow control mode */
	GDMA_Channelx->CFG_HIGH &= ~0x03;
	GDMA_Channelx->CFG_HIGH |= 0x01;
    
    /* ---------------clear pending interrupts of corresponding GDMA channel------------------ */
    GDMA_BASE->CLEAR_TFR = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_BLOCK = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_DST_TRAN = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_SRC_TRAN = BIT(GDMA_InitStruct->GDMA_ChannelNum);
    GDMA_BASE->CLEAR_ERR = BIT(GDMA_InitStruct->GDMA_ChannelNum);
}

/**
  * @brief  Fills each GDMA_InitStruct member with its default value.
  * @param  GDMA_InitStruct : pointer to a GDMA_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void GDMA_StructInit(GDMA_InitTypeDef* GDMA_InitStruct)
{
    /*-------------- Reset DMA init structure parameters values ------------------*/
    GDMA_InitStruct->GDMA_ChannelNum      = 0;
    GDMA_InitStruct->GDMA_DIR             = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct->GDMA_BufferSize      = 200;
    GDMA_InitStruct->GDMA_SourceInc       = DMA_SourceInc_Fix;
    GDMA_InitStruct->GDMA_DestinationInc  = DMA_DestinationInc_Inc;

    GDMA_InitStruct->GDMA_SourceDataSize  = GDMA_DataSize_Byte;
    GDMA_InitStruct->GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct->GDMA_SourceMsize     = GDMA_Msize_1;
    GDMA_InitStruct->GDMA_DestinationMsize  = GDMA_Msize_1;
    
    GDMA_InitStruct->GDMA_SourceAddr      = 0;
    GDMA_InitStruct->GDMA_DestinationAddr = 0;
    GDMA_InitStruct->GDMA_TransferType    = GDMA_TransferType_Other;
    GDMA_InitStruct->GDMA_ChannelPriority = 0;
}

/**
  * @brief  Enables or disables the specified GDMA Channelx.
  * @param  GDMA_Channelx: x can be 0 to 5 to select the DMA Channel.
  * @param  NewState: new state of the DMA Channelx. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void GDMA_Cmd(uint8_t GDMA_ChannelNum, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_GDMA_ChannelNum(GDMA_ChannelNum));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected DMAy Channelx which must enable write protect bit */
        GDMA_BASE->ChEnReg = BIT(GDMA_ChannelNum) | BIT(GDMA_ChannelNum + 8);
    }
    else
    {
        /* Disable the selected DMAy Channelx */
        GDMA_BASE->ChEnReg = BIT(GDMA_ChannelNum + 8);
    }
}

/**
  * @brief  Enables or disables the specified DMAy Channelx interrupts.
  * @param  GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  * @param  GDMA_IT: specifies the GDMA interrupts sources to be enabled
  *   or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg GDMA_INT_Transfer:  Transfer complete interrupt unmask
  *     @arg GDMA_INT_Block:  Block transfer interrupt unmask
  *     @arg GDMA_INT_SrcTransfer:  SourceTransfer interrupt unmask
  *     @arg GDMA_INT_DstTransfer:  Destination Transfer interrupt unmask
  *     @arg GDMA_INT_Error:  Transfer error interrupt unmask
  * @param  NewState: new state of the specified DMA interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void GDMA_INTConfig(uint8_t GDMA_ChannelNum, uint32_t GDMA_IT, FunctionalState NewState)
{
    uint32_t temp_bit = 0;
    
    /* Check the parameters */
    assert_param(IS_GDMA_ChannelNum(GDMA_ChannelNum));
    assert_param(IS_GDMA_CONFIG_IT(GDMA_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    
    if (NewState != DISABLE)
    {
        /* Enable the selected DMA interrupts which will open write enable bit */
        temp_bit = BIT(GDMA_ChannelNum) | BIT(GDMA_ChannelNum + 8); 

        if(GDMA_IT & GDMA_INT_Transfer)
        {
            GDMA_BASE->MASK_TFR = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_Block)
        {
            GDMA_BASE->MASK_BLOCK = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_SrcTransfer)
        {
            GDMA_BASE->MASK_SRC_TRAN = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_DstTransfer)
        {
            GDMA_BASE->MASK_DST_TRAN = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_Error)
        {
            GDMA_BASE->MASK_ERR = temp_bit;
        }
    }
    else
    {
        /* Disable the selected DMA interrupts */
        temp_bit = BIT(GDMA_ChannelNum + 8);
        if(GDMA_IT & GDMA_INT_Transfer)
        {
            GDMA_BASE->MASK_TFR = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_Block)
        {
            GDMA_BASE->MASK_BLOCK = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_SrcTransfer)
        {
            GDMA_BASE->MASK_SRC_TRAN = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_DstTransfer)
        {
            GDMA_BASE->MASK_DST_TRAN = temp_bit;
        }
        if(GDMA_IT & GDMA_INT_Error)
        {
            GDMA_BASE->MASK_ERR = temp_bit;
        }
    }
}

/**
  * @brief  Enables or disables the specified DMAy Channelx interrupts.
  * @param  GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  * @param  GDMA_IT: specifies the GDMA interrupts sources to be enabled
  *   or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg GDMA_INT_Transfer:  clear transfer complete interrupt 
  *     @arg GDMA_INT_Block:  clear Block transfer interrupt 
  *     @arg GDMA_INT_SrcTransfer:  clear SourceTransfer interrupt 
  *     @arg GDMA_INT_DstTransfer:  clear Destination Transfer interrupt 
  *     @arg GDMA_INT_Error:  clear Transfer error interrupt 
  * @param  NewState: new state of the specified DMA interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void GDMA_ClearINTPendingBit(uint8_t GDMA_ChannelNum, uint32_t GDMA_IT)
{
    uint32_t temp_bit = 0;
    
    /* Check the parameters */
    assert_param(IS_GDMA_ChannelNum(GDMA_ChannelNum));
    assert_param(IS_GDMA_CONFIG_IT(GDMA_IT));
    
    /* clear the selected DMA interrupts */
    temp_bit = BIT(GDMA_ChannelNum) | BIT(GDMA_ChannelNum + 8); 

    if(GDMA_IT & GDMA_INT_Transfer)
    {
        GDMA_BASE->CLEAR_TFR = temp_bit;
    }
    if(GDMA_IT & GDMA_INT_Block)
    {
        GDMA_BASE->CLEAR_BLOCK = temp_bit;
    }
    if(GDMA_IT & GDMA_INT_SrcTransfer)
    {
        GDMA_BASE->CLEAR_SRC_TRAN = temp_bit;
    }
    if(GDMA_IT & GDMA_INT_DstTransfer)
    {
        GDMA_BASE->CLEAR_DST_TRAN = temp_bit;
    }
    if(GDMA_IT & GDMA_INT_Error)
    {
        GDMA_BASE->CLEAR_ERR = temp_bit;
    }
}

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

