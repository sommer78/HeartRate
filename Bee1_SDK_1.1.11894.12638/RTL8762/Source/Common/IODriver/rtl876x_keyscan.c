/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_keyscan.c
* @brief    This file provides all the KEYSCAN firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-04-30
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_keyscan.h"

/* KeyScan clock enable mask, must be on after key scan configuration */
#define CR0_CLOCK_EN_Set                    ((uint32_t)((uint32_t)1 << 31))
#define CR0_CLOCK_EN_Reset                  ((uint32_t)~((uint32_t)1 << 31))

/* KeyScan program debounce time enable mask */
#define CR2_PROG_DEBOUNCE_EN_Set            ((uint32_t)(1 << 27))
#define CR2_PROG_DEBOUNCE_EN_Reset          ((uint32_t)~(1 << 27))

/* KeyScan interrupt mask bit mask */
#define INTCR_MASK_EN_Set                   ((uint32_t)(0x01 << 15))
#define INTCR_MASK_EN_Reset                 ((uint32_t)~(0x01 << 15))

/**
  * @brief Initializes the KeyScan peripheral according to the specified 
  *   parameters in the KeyScan_InitStruct
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_InitStruct: pointer to a KEYSCAN_InitTypeDef structure that
  *   contains the configuration information for the specified KeyScan peripheral
  * @retval None
  */
void KeyScan_Init(KEYSCAN_TypeDef* KeyScan, KEYSCAN_InitTypeDef* KeyScan_InitStruct)
{
    assert_param(IS_KeyScan_PERIPH(KeyScan));
    assert_param(IS_KEYSCAN_ROW_NUM(KeyScan_InitStruct->rowSize));
    assert_param(IS_KEYSCAN_COL_NUM(KeyScan_InitStruct->colSize));
    assert_param(IS_KEYSCAN_DETECT_PERIOD(KeyScan_InitStruct->detectPeriod));
    assert_param(IS_KEYSCAN_TIMEOUT(KeyScan_InitStruct->timeout));
    assert_param(IS_KEYSCAN_INTERVAL(KeyScan_InitStruct->scanInterval));
    assert_param(IS_KEYSCAN_DEBOUNCE_EN(KeyScan_InitStruct->debounceEn));
    assert_param(IS_KEYSCAN_DEBOUNCE_TIME(KeyScan_InitStruct->debounceTime));
    assert_param(IS_KEYSCAN_DETECT_MODE(KeyScan_InitStruct->detectMode));
    assert_param(IS_KEYSCAN_FIFO_OVR_CTRL(KeyScan_InitStruct->fifoOvrCtrl));
    assert_param(IS_KEYSCAN_MAX_SCAN_DATA(KeyScan_InitStruct->maxScanData));

    //turn off keyscan clock firstly
    KeyScan->CR0 &= CR0_CLOCK_EN_Reset;

    //mask all interrupt
    KeyScan->INTCR |= (1 << 15);
    
    KeyScan->CR1 = ( (0x03 << 27)\
                    | (0x03 << 24)\
                    | (((1 << KeyScan_InitStruct->rowSize) - 1) << 16)\
                    | (KeyScan_InitStruct->detectPeriod)\
                    | (KeyScan_InitStruct->debounceEn)\
                    | (KeyScan_InitStruct->timeout)\
                    | (KeyScan_InitStruct->scanInterval));

    //prog debounce time
    KeyScan->CR2 |= CR2_PROG_DEBOUNCE_EN_Set;
    KeyScan->CR2 &= ~(0x7FF << 16);
    KeyScan->CR2 |= (KeyScan_InitStruct->debounceTime << 16);
    KeyScan->CR2 &= CR2_PROG_DEBOUNCE_EN_Reset;
                    
    KeyScan->CR0 =  (((1 << KeyScan_InitStruct->colSize) - 1) << 8)\
                    | ((KeyScan_InitStruct->colSize - 1) << 0)\
                    | ((KeyScan_InitStruct->rowSize - 1) << 5);

    //KeyScan->CR1 &= ((0x07 << 24) | (0x07 << 27));                

    KeyScan->CR2 = (KeyScan_InitStruct->detectMode)\
                    | (KeyScan_InitStruct->fifoOvrCtrl)\
                    | (KeyScan_InitStruct->maxScanData)\
                    | (0x0F << 12);

    //unmask all interrupt
    KeyScan->INTCR &= (~(1 << 15));

    return;   
}

/**
  * @brief  Deinitializes the Keyscan peripheral registers to their default reset values(turn off keyscan clock).
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval None
  */
void KeyScan_DeInit(KEYSCAN_TypeDef* KeyScan)
{
    /* Check the parameters */
    assert_param(IS_KeyScan_PERIPH(KeyScan));
    
    RCC_PeriphClockCmd(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, DISABLE);

    return;
}


/**
  * @brief  Fills each I2C_InitStruct member with its default value.
  * @param  KeyScan_InitStruct: pointer to an KEYSCAN_InitTypeDef structure which will be initialized.
  * @retval None
  */
void KeyScan_StructInit(KEYSCAN_TypeDef* KeyScan, KEYSCAN_InitTypeDef* KeyScan_InitStruct)
{
    KeyScan_InitStruct->colSize         = 4;
    KeyScan_InitStruct->rowSize         = 4;
    
    KeyScan_InitStruct->detectPeriod    = KeyScan_Detect_Period_10ms;
    KeyScan_InitStruct->debounceEn      = KeyScan_Debounce_Enable;
    KeyScan_InitStruct->timeout         = KeyScan_Timeout_2s;
    KeyScan_InitStruct->scanInterval    = KeyScan_Scan_Interval_12_5ms;
    
    KeyScan_InitStruct->debounceTime    = (8 * 32); //8ms
    KeyScan_InitStruct->detectMode      = KeyScan_Detect_Mode_Edge;
    KeyScan_InitStruct->fifoOvrCtrl     = KeyScan_FIFO_OVR_CTRL_DIS_LAST;
    KeyScan_InitStruct->maxScanData     = 0x1A;

    return;
}

/**
  * @brief  Enables or disables the specified KeyScan interrupts.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_IT: specifies the KeyScan interrupts sources to be enabled or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg KEYSCAN_INT_TIMEOUT: KeyScan timeout interrupt mask
  *     @arg KEYSCAN_INT_OVER_THRESHOLD: Kescan FIFO data over threshold interrupt mask
  *     @arg KEYSCAN_INT_SCAN_END: KeyScan scan end interrupt mask
  * @param  NewState: new state of the specified KeyScan interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_INTConfig(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_KeyScan_PERIPH(KeyScan));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_KEYSCAN_CONFIG_IT(KeyScan_IT));

    if (newState == ENABLE)
    {
        //mask interrupt, clear interrupt status all status and fifo
        KeyScan->INTCR |= (0xF << 12);
        KeyScan->CR2 |= (0xF << 12);
        
        /* Enable the selected KeyScan interrupts */
        KeyScan->INTCR |= KeyScan_IT;

        //unmask interrupt
        KeyScan->INTCR &= (~(1 << 15));
    }
    else
    {
        /* Disable the selected KeyScan interrupts */
         KeyScan->INTCR &= (uint32_t)~KeyScan_IT;
    }
}

/**
  * @brief  Enables or disables the specified KeyScan interrupts mask.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  NewState: new state of the specified KeyScan interrupts mask.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_INTMask(KEYSCAN_TypeDef* KeyScan, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_KeyScan_PERIPH(KeyScan));
    assert_param(IS_FUNCTIONAL_STATE(newState));

    if (newState == ENABLE)
    {
        /* mask KeyScan interrupts */
        KeyScan->INTCR |= INTCR_MASK_EN_Set;
    }
    else
    {
        /* enable KeyScan interrupts */
         KeyScan->INTCR &= INTCR_MASK_EN_Reset;
    }
}

/**
  * @brief  Read data from keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param[out]  outBuf: buffer to save data read from KeyScan FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
void KeyScan_Read(KEYSCAN_TypeDef* KeyScan, uint8_t* outBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_KeyScan_PERIPH(KeyScan));

    uint16_t i = 0;

    for(i = 0; i < count; i++)
    {
        *outBuf++ = (uint8_t)KeyScan->FIFODATA;
    }

    return;
}

/**
  * @brief  Enables or disables the KeyScan peripheral.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  NewState: new state of the KeyScan peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_Cmd(KEYSCAN_TypeDef* KeyScan, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_KeyScan_PERIPH(KeyScan));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the selected I2C peripheral */
    KeyScan->CR0 |= CR0_CLOCK_EN_Set;
  }
  else
  {
    /* Disable the selected I2C peripheral */
    KeyScan->CR0 &= CR0_CLOCK_EN_Reset;
  }
}


