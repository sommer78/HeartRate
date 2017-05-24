/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_ir.c
* @brief    This file provides all the IR firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-05-18
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_ir.h"

/**
  * @brief Initializes the IR peripheral according to the specified 
  *   parameters in the IR_InitStruct
  * @param  IRx: selected IR peripheral.
  * @param  IR_InitStruct: pointer to a IR_InitTypeDef structure that
  *   contains the configuration information for the specified IR peripheral
  * @retval None
  */
void IR_Init(IR_TypeDef* IRx, IR_InitTypeDef* IR_InitStruct)
{
    assert_param(IS_IR_PERIPH(IRx));
    assert_param(IS_IR_FREQUENCY(IR_InitStruct->frequency));
    assert_param(IS_IR_DETECT_START_END_TIME(IR_InitStruct->detectStartTime));
    assert_param(IS_IR_DETECT_START_END_TIME(IR_InitStruct->detectEndTime));

    // disable all interrupt
    IRx->INTCR &= (~(IR_INT_TF_EMPTY | IR_INT_TX | IR_INT_RF_FULL | IR_INT_RX));

    //set duty cycle and divider
    uint32_t div =  ((SystemClock / 1000)/IR_InitStruct->frequency) - 1;
    uint32_t duty = ((div + 1) / IR_InitStruct->dutyCycle) - 1;
    IRx->MFCR = (div | (duty << 16));

    //set divider number
    IRx->INTCR |= ((IR_InitStruct->frequency - 1) << 2);
    
    //set detect start and end time
    IRx->CR &= ~(0x3FF << 16);
    IRx->CR |= (IR_InitStruct->detectEndTime << 16);
    IRx->CR &= ~(0x3FF << 0);
    IRx->CR |= (IR_InitStruct->detectStartTime << 0);

    //clear all interrupt flags amd FIFO
    IRx->INTCR |= (IR_INT_CLEAR_RX_END | IR_INT_CLEAR_RX_START | IR_INT_CLEAR_RX_FIFO_FULL\
                    | IR_INT_CLEAR_RX_TH | IR_INT_CLEAR_TX_FIFO_EMPTY | IR_INT_CLEAR_TX_TH | (1 << 30));
    

    return;
}

/**
  * @brief  Deinitializes the IR peripheral registers to their default reset values(turn off IRx clock).
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
void IR_DeInit(IR_TypeDef* IRx)
{
    assert_param(IS_IR_PERIPH(IRx));
    
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, DISABLE);
    
    return;
}

/**
  * @brief  Fills each IR_InitStruct member with its default value.
  * @param  IR_InitStruct: pointer to an IR_InitTypeDef structure which will be initialized.
  * @retval None
  */
void IR_StructInit(IR_InitTypeDef* IR_InitStruct)
{
    IR_InitStruct->frequency            = 38;                  //38khz
    IR_InitStruct->dutyCycle            = 2;                   
    IR_InitStruct->detectEndTime        = 0x23a;
    IR_InitStruct->detectStartTime      = 0x180;
    
    return;
}

/**
  * @brief  start to send data.
  * @param  IRx: selected IR peripheral.
  * @param  buf: data buffer to send.
  * @param  length: buffer length.
  * @retval None
  */
void IR_SendData(IR_TypeDef* IRx, uint16_t* pBuf, uint16_t length)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    
    while(length--)
    {
        IRx->TFR = *pBuf++;
    }
    
    IRx->CSR1 |= CSR1_START_TX_Set;
    
    return;
}

/**
  * @brief  start to read rx data.
  * @param  IRx: selected IR peripheral.
  * @param  buf: data buffer to send.
  * @retval None
  */
void IR_ReceiveData(IR_TypeDef* IRx, uint16_t* pBuf, uint16_t length)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    while(length--)
    {
        *pBuf++ = IRx->RFR;
    }
    
    return;
}

