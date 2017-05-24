/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_uart.c
* @brief    This file provides all the UART firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-05-08
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"

#define LCR_DLAB_Set                    ((uint32_t)(1 << 7))
#define LCR_DLAB_Reset                  ((uint32_t)~(1 << 7))

#define FCR_CLEAR_RX_FIFO_Set           ((uint32_t)(1 << 1))
#define FCR_CLEAR_RX_FIFO_Reset         ((uint32_t)~(1 << 1))

#define FCR_CLEAR_TX_FIFO_Set           ((uint32_t)(1 << 2))
#define FCR_CLEAR_TX_FIFO_Reset         ((uint32_t)~(1 << 2))

uint32_t UartFcrRegVal = 0;

/**
  * @brief Initializes the UART peripheral according to the specified 
  *   parameters in the UART_InitStruct
  * @param  UARTx: selected UART peripheral.
  * @param  UART_InitStruct: pointer to a UART_InitTypeDef structure that
  *   contains the configuration information for the specified UART peripheral
  * @retval None
  */
void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* UART_InitStruct)
{
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_UART_WORD_LENGTH(UART_InitStruct->wordLen));
    assert_param(IS_UART_PARITY(UART_InitStruct->parity));
    assert_param(IS_UART_STOPBITS(UART_InitStruct->stopBits));
    assert_param(IS_UART_AUTO_FLOW_CTRL(UART_InitStruct->autoFlowCtrl));
    assert_param(IS_UART_DMA_CFG(UART_InitStruct->dmaEn));
    assert_param(IS_UART_RX_FIFO_TRIGGER_LEVEL(UART_InitStruct->rxTriggerLevel));

    uint32_t regVal = 0;

    //clear DLAB bit
    UARTx->LCR &= LCR_DLAB_Reset;

    //read to clear Line Status Reg
    regVal = UARTx->LSR;
    regVal = regVal;    //just fix compile warning

    //clear FIFO
    UARTx->INTID_FCR |= (FCR_CLEAR_RX_FIFO_Set | FCR_CLEAR_TX_FIFO_Set);

    //disable all interrupt
    UARTx->DLH_INTCR = 0x00;

    //set baudrate, firstly set DLAB bit
    UARTx->LCR |= LCR_DLAB_Set;
    //set calibration parameters(OVSR)
    UARTx->STSR &= ~0xF0;
    UARTx->STSR |= (UART_InitStruct->ovsr << 4);
    //set calibration parameters(OVSR_adj) 
    UARTx->SPR &= (~(0x7ff << 16));
    UARTx->SPR |= (UART_InitStruct->ovsr_adj << 16);
    //set DLL and DLH
    UARTx->DLL = (UART_InitStruct->div & 0x00FF);
    UARTx->DLH_INTCR = ((UART_InitStruct->div & 0xFF00) >>8);
    //after set baudrate, clear DLAB bit
    UARTx->LCR &= LCR_DLAB_Reset;

    //set LCR reg
    UARTx->LCR = (UART_InitStruct->parity | UART_InitStruct->stopBits | UART_InitStruct->wordLen);
    //set FCR reg, FIFO must enable
    UARTx->INTID_FCR = ((1 << 0) | UART_InitStruct->rxTriggerLevel | UART_InitStruct->dmaEn);
    //write-only register, must be saved in initialization!
    UartFcrRegVal = ((1 << 0) | UART_InitStruct->rxTriggerLevel | UART_InitStruct->dmaEn);

    /* auto flow control */
    UARTx->MCR &= (~((1 << 5) | (1 << 1)));
    UARTx->MCR |= UART_InitStruct->autoFlowCtrl;
    
    return;
}

/**
  * @brief  Deinitializes the UART peripheral registers to their default reset values(turn off UART clock).
  * @param  UARTx: selected UART peripheral.
  * @retval None
  */
void UART_DeInit(UART_TypeDef* UARTx)
{
    assert_param(IS_UART_PERIPH(UARTx));

    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, DISABLE);

    return;
}

/**
  * @brief  Fills each UART_InitStruct member with its default value.
  * @param  UART_InitStruct: pointer to an UART_InitTypeDef structure which will be initialized.
  * @retval None
  */
void UART_StructInit(UART_InitTypeDef* UART_InitStruct)
{
    //115200 default
    UART_InitStruct->div            = 20;
    UART_InitStruct->ovsr           = 12;
    UART_InitStruct->ovsr_adj       = 0x252;

    UART_InitStruct->parity         = UART_PARITY_NO_PARTY;
    UART_InitStruct->stopBits       = UART_STOP_BITS_1;
    UART_InitStruct->wordLen        = UART_WROD_LENGTH_8BIT;
    UART_InitStruct->dmaEn          = UART_DMA_DISABLE;
    UART_InitStruct->autoFlowCtrl   = UART_AUTO_FLOW_CTRL_DIS;
    UART_InitStruct->rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_8BYTE;
    
    return;
}

/**
  * @brief  Receive data from rx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @param[out]  outBuf: buffer to save data read from UART FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
void UART_ReceiveData(UART_TypeDef* UARTx, uint8_t* outBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    
    while((count--) != 0)
    {
        *outBuf++ = (uint8_t)UARTx->RB_THR;
    }

    return;
}

/**
  * @brief  Send data to tx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @param  inBuf: buffer to be written to Tx FIFO.
  * @param  count: number of data to be written.
  * @retval None
  */
void UART_SendData(UART_TypeDef* UARTx, uint8_t* inBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));

    while((count--) != 0)
    {
        UARTx->RB_THR = *inBuf++;
    }

    return;
}

