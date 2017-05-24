/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl_8762_3wire_spi.c
* @brief    3wire_spi driver.
* @details
* @author   tifnan_ge
* @date     2015-10-28
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl876x_rcc.h"
#include "rtl876x_3wire_spi.h"

#define CFGR_BURST_READ_EN_Set          ((uint32_t)(1 << 20))
#define CFGR_BURST_READ_EN_Reset        ((uint32_t)~(1 << 20))

/**
  * @brief Initializes the SPI_3WIRE peripheral according to the specified 
  *   parameters in the SPI_3WIRE_InitStruct
  * @param  SPI3WIRE: selected SPI_3WIRE peripheral.
  * @param  SPI3WIRE_InitStruct: pointer to a SPI3WIRE_InitTypeDef structure that
  *   contains the configuration information for the specified SPI_3WIRE peripheral
  * @retval None
  */
void SPI3WIRE_Init(SPI2WIRE_TypeDef* SPI3WIRE, SPI3WIRE_InitTypeDef* SPI3WIRE_InitStruct)
{
	/* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    assert_param(IS_SPI3WIRE_MODE(SPI3WIRE_InitStruct->mode));
    assert_param(IS_SPI3WIRE_OE_DELAY_CFG(SPI3WIRE_InitStruct->oeDelayCfg));
    assert_param(IS_SPI3WIRE_END_EXTEND_MODE(SPI3WIRE_InitStruct->endExtMode));
    assert_param(IS_SPI3WIRE_READ_CYCLE_DELAY(SPI3WIRE_InitStruct->readDelay));

    uint32_t divValue = 0;

    divValue = (SystemClock / (SPI3WIRE_InitStruct->speed * 2) - 1) & 0xFF;

	/* Initialize the parameters */
    SPI3WIRE->CFGR = SPI3WIRE_InitStruct->mode | SPI3WIRE_InitStruct->oeDelayCfg|\
    				SPI3WIRE_InitStruct->endExtMode | (divValue << 8) | (SPI3WIRE_InitStruct->readDelay << 3);
	
	/* Enable 2wire SPI mode or not*/
	if(SPI3WIRE_InitStruct->mode == SPI3WIRE_2WIRE_MODE)
	{
		SPI3WIRE->CFGR |= BIT(0);
	}
    
    /* Disable interrupt, clear read data register and read number register */
    SPI3WIRE->INTCR = (0x07 << 1);

    return;
}

/**
  * @brief  Deinitializes the 3WIRE SPI peripheral registers to their default reset values(turn off 3WIRE SPI clock).
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @retval None
  */
void SPI3WIRE_DeInit(SPI2WIRE_TypeDef* SPI3WIRE)
{
	/* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    RCC_PeriphClockCmd(APBPeriph_SPI2W, APBPeriph_SPI2W_CLOCK, DISABLE);

    return;
}

/**
  * @brief  Fills each SPI3WIRE_InitStruct member with its default value.
  * @param  SPI3WIRE_InitStruct: pointer to an SPI3WIRE_InitTypeDef structure which will be initialized.
  * @retval None
  */
void SPI3WIRE_StructInit(SPI2WIRE_TypeDef* SPI3WIRE, SPI3WIRE_InitTypeDef* SPI3WIRE_InitStruct)
{
	/* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    
    SPI3WIRE_InitStruct->mode       = SPI3WIRE_3WIRE_MODE;
    SPI3WIRE_InitStruct->oeDelayCfg = SPI3WIRE_OE_DELAY_1T;
    SPI3WIRE_InitStruct->endExtMode = SPI3WIRE_NORMAL_MODE;
	SPI3WIRE_InitStruct->mode       = SPI3WIRE_3WIRE_MODE;
    SPI3WIRE_InitStruct->speed      = 1000000;              
    
    return;
}

/**
  * @brief  Enables or disables the specified SPI3WIRE interrupts.
  * @param  SPI3WIRE: selected SPI3WIRE peripheral.
  * @param  SPI3WIRE_IT: specifies the SPI3WIRE interrupts sources to be enabled or disabled. 
  *   This parameter can be only be the following value:
  *     @arg SPI3WIRE_IT_INT: enable SPI3WIRE interrupt.
  * @param  NewState: new state of the specified SPI3WIRE interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI3WIRE_ITConfig(SPI2WIRE_TypeDef* SPI3WIRE, uint32_t SPI3WIRE_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_SPI3WIRE_IT(SPI3WIRE_IT));

    if (newState == ENABLE)
    {
        /* Enable the selected 3WIRE_SPI interrupts */
        SPI3WIRE->INTCR |= SPI3WIRE_IT;
    }
    else
    {
        /* Disable the selected 3WIRE_SPI interrupts */
        SPI3WIRE->INTCR &= (uint32_t)~SPI3WIRE_IT;
    }

    return;
}

/**
  * @brief  Clears the 3WIRE_SPI interrupt pending bits.
  * @param  SPI3WIRE: selected 3WIRE_SPI peripheral.
  * @param  SPI3WIRE_IT: specifies the SPI3WIRE interrupts sources to be enabled or disabled. 
  *   This parameter can be only be the following value:
  *     @arg SPI3WIRE_IT_INT: enable SPI3WIRE interrupt.
  * @retval None
  */
void SPI3WIRE_ClearITPendingBit(SPI2WIRE_TypeDef* SPI3WIRE, uint32_t SPI3WIRE_IT)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    assert_param(IS_SPI3WIRE_IT(SPI3WIRE));

    SPI3WIRE->INTCR |= (SPI3WIRE_IT << 1);
    
    return;
}

/**
  * @brief  Checks whether the specified 3WIRE_SPI flag is set or not.
  * @param  SPI3WIRE: selected 3WIRE_SPI peripheral.
  * @param  SPI3WIRE_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI3WIRE_FLAG_BUSY: 3wire spi is busy.
  *     @arg SPI3WIRE_FLAG_INT_IND: there is 3wire spi interrupt.
  * @retval The new state of UART_FLAG (SET or RESET).
  */
FlagStatus SPI3WIRE_GetFlagStatus(SPI2WIRE_TypeDef* SPI3WIRE, uint32_t SPI3WIRE_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    assert_param(IS_SPI3WIRE_FLAG(SPI3WIRE_FLAG));
    
    if(SPI3WIRE->SR & SPI3WIRE_FLAG)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Get total number of data byte in each SPI read.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @retval The total number of data byte in each SPI read.
  */
uint16_t SPI3WIRE_GetReadDataNum(SPI2WIRE_TypeDef* SPI3WIRE)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));
    
    return (uint16_t)(SPI3WIRE->SR & 0x0F);
}

/**
  * @brief  Clear all read data registers.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @retval None
  */
void SPI3WIRE_ClearAllReadData(SPI2WIRE_TypeDef* SPI3WIRE)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));

    SPI3WIRE->INTCR |= (1 << 3);

    return;
}

/**
  * @brief  Clear read data number.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @retval None
  */
void SPI3WIRE_ClearReadNumber(SPI2WIRE_TypeDef* SPI3WIRE)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));

    SPI3WIRE->INTCR |= (1 << 2);

    return;
}

/**
  * @brief  start to write data.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @param  address: write address.
  * @param  data: write data.
  * @retval None
  */
void SPI3WIRE_StartWrite(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t address, uint8_t data)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));

    SPI3WIRE->CR = (uint32_t)(1 << 15) | ((uint32_t)address << 8) | (uint32_t)data;
    
    return;
}

/**
  * @brief  write data.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @param  readNum: read number.
  * @param  outBuf: buffer to store read datas.
  * @retval None
  */
void SPI3WIRE_ReadData(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t readNum, uint8_t* outBuf)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));

    uint8_t i = 0;
    uint8_t j = 0;
    uint32_t regVal = 0;
    uint8_t regIndex =  readNum / 4;

    for(i = 0; i < regIndex; i++)
    {
        regVal = *((volatile uint32_t*)(0x40004000UL + 0x40 + i));
        
        for(j = 0; j < 4; j++)
        {
            *outBuf++ = (uint8_t)(regVal >> (8 * j));
        }
    }
    
    if(readNum > regIndex * 4)
    {
        regVal = *((volatile uint32_t*)(0x40004000UL + 0x40 + regIndex));
    
        for(j = 0; j < (readNum - regIndex * 4); j++)
        {
            *outBuf++ = (uint8_t)(regVal >> (8 * j));
        }
    }

    return;
}


/**
  * @brief  start signal read.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @param  address: read address.
  * @retval None.
  */
void SPI3WIRE_StartSignalRead(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t address)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));

    /* Disable burst read */
    SPI3WIRE->CFGR &= CFGR_BURST_READ_EN_Reset;

    /* Read address */
    SPI3WIRE->CR = (uint32_t)(0 << 15) | ((uint32_t)address << 8);
    
    return;
}

/**
  * @brief  start burst read.
  * @param  SPI3WIRE: selected 3WIRE SPI peripheral.
  * @param  address: read address.
  * @param  readNum: number of data to read.
  * @retval None.
  */
void SPI3WIRE_StartBurstRead(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t address, uint8_t readNum)
{
    /* Check the parameters */
    assert_param(IS_SPI3WIRE_PERIPH(SPI3WIRE));

    /* Disable burst read */
    SPI3WIRE->CFGR |= CFGR_BURST_READ_EN_Set;

    /* Read num */
    SPI3WIRE->CFGR &= (uint32_t)(~(0x0F << 16));
    SPI3WIRE->CFGR |= (readNum << 16);

    /* Read address */
    SPI3WIRE->CR = (uint32_t)(0 << 15) | ((uint32_t)address << 8);
    
    return;
}

