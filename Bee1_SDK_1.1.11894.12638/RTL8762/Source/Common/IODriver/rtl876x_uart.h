/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_uart.h
* @brief     header file of uart driver.
* @details   
* @author    tifnan_ge
* @date      2015-05-08
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_UART_H_
#define _RTL876X_UART_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver RTK Periphral Driver
  * @{
  */

/** @addtogroup UART UART
  * @brief UART driver module
  * @{
  */

/** @defgroup UART_Exported_Types UART Exported Types
  * @{
  */

/**
 * @brief UART initialize parameters
 *
 * UART initialize parameters
 */
typedef struct
{
    //baudrate calibration
    uint16_t ovsr_adj;
    uint16_t div;
    uint16_t ovsr;

    uint16_t wordLen;
    uint16_t parity;
    uint16_t stopBits;
    uint16_t autoFlowCtrl;

    uint16_t rxTriggerLevel;
    uint16_t dmaEn;
}UART_InitTypeDef, *PUART_InitTypeDef;

/**
  * @}
  */

/** @defgroup UART_Exported_Constants UART Exported Constants
  * @{
  */

#define IS_UART_PERIPH(PERIPH) ((PERIPH) == UART)
#define UART_TX_FIFO_SIZE           16
#define UART_RX_FIFO_SIZE           16

/** @defgroup UART_Interrupts_Definition UART Interrupts Definition 
  * @{
  */

#define UART_INT_RD_AVA                             ((uint16_t)(1 << 0))     //receive data avaliable
#define UART_INT_FIFO_EMPTY                         ((uint16_t)(1 << 1))
#define UART_INT_LINE_STS                           ((uint16_t)(1 << 2))
#define UART_INT_MODEM_STS                          ((uint16_t)(1 << 3))

#define IS_UART_IT(IT) ((((IT) & 0xFFFFFFF0) == 0x00) && ((IT) != 0x00))

#define IS_UART_GET_IT(IT) (((IT) == UART_IT_RD_AVA) || ((IT) == UART_IT_FIFO_EMPTY)\
                            || ((IT) == UART_IT_LINE_STS) || ((IT) == UART_IT_MODEM_STS))
/**
  * @}
  */

/** @defgroup UART_Interrupt_Identifier UART Interrupt Identifier 
  * @{
  */

#define UART_INT_ID_LINE_STATUS                     ((uint16_t)(0x03 << 1))
#define UART_INT_ID_RX_LEVEL_REACH                  ((uint16_t)(0x02 << 1))
#define UART_INT_ID_RX_TMEOUT                       ((uint16_t)(0x06 << 1))
#define UART_INT_ID_TX_EMPTY                        ((uint16_t)(0x01 << 1))
#define UART_INT_ID_MODEM_STATUS                    ((uint16_t)(0x00 << 1))

#define IS_UART_IT_ID(ID) (((ID) == UART_INT_ID_LINE_STATUS) || ((ID) == UART_INT_ID_RX_LEVEL_REACH)\
                            || ((ID) == UART_INT_ID_RX_TMEOUT) || ((ID) == UART_INT_ID_TX_EMPTY)\
                            || ((ID) == UART_INT_ID_MODEM_STATUS))
/**
  * @}
  */

/** @defgroup UART_Flag UART Flag 
  * @{
  */

#define UART_FLAG_INT_PEND                         ((uint16_t)(1 << 0))
#define UART_FLAG_RX_DATA_RDY                      ((uint16_t)(1 << 0))
#define UART_FLAG_RX_OVERRUN                       ((uint16_t)(1 << 1))
#define UART_FLAG_PARTY_ERR                        ((uint16_t)(1 << 2))
#define UART_FLAG_FRAME_ERR                        ((uint16_t)(1 << 3))
#define UART_FLAG_BREAK_ERR                        ((uint16_t)(1 << 4))
#define UART_FLAG_THR_EMPTY                        ((uint16_t)(1 << 5))     //Transmitter Holding Register or Transmitter FIFO empty
#define UART_FLAG_THR_TSR_EMPTY                    ((uint16_t)(1 << 6))     //Transmitter Holding Register(or tx FIFO) and Transmitter shift Register both empty
#define UART_FLAG_RX_FIFO_ERR                      ((uint16_t)(1 << 7))

#define IS_UART_GET_FLAG(FLAG) (((FLAG) == UART_FLAG_RX_DATA_RDY)   || ((FLAG) == UART_FLAG_RX_OVERRUN)\
                                    || ((FLAG) == UART_FLAG_PARTY_ERR) || ((FLAG) == UART_FLAG_FRAME_ERR)\
                                    || ((FLAG) == UART_FLAG_BREAK_ERR) || ((FLAG) == UART_FLAG_THR_EMPTY)\
                                    || ((FLAG) == UART_FLAG_THR_TSR_EMPTY) || ((FLAG) == UART_FLAG_RX_FIFO_ERR))
/**
  * @}
  */

/** @defgroup UART_RX_FIFO_Level UART RX FIFO Level 
  * @{
  */

#define UART_RX_FIFO_TRIGGER_LEVEL_1BYTE            ((uint16_t)(0x00 << 6))
#define UART_RX_FIFO_TRIGGER_LEVEL_4BYTE            ((uint16_t)(0x01 << 6))
#define UART_RX_FIFO_TRIGGER_LEVEL_8BYTE            ((uint16_t)(0x02 << 6))
#define UART_RX_FIFO_TRIGGER_LEVEL_14BYTE           ((uint16_t)(0x03 << 6))

#define IS_UART_RX_FIFO_TRIGGER_LEVEL(LEVEL) (((LEVEL) == UART_RX_FIFO_TRIGGER_LEVEL_1BYTE) || (LEVEL == UART_RX_FIFO_TRIGGER_LEVEL_4BYTE)\
                                || ((LEVEL) == UART_RX_FIFO_TRIGGER_LEVEL_8BYTE) || (LEVEL == UART_RX_FIFO_TRIGGER_LEVEL_14BYTE))
/**
  * @}
  */

/** @defgroup UART_Parity UART Parity 
  * @{
  */

#define UART_PARITY_NO_PARTY                        ((uint16_t)(0x00 << 3))
#define UART_PARITY_ODD                             ((uint16_t)(0x01 << 3))
#define UART_PARITY_EVEN                            ((uint16_t)(0x03 << 3))

#define IS_UART_PARITY(PARITY) (((PARITY) == UART_PARITY_NO_PARTY) || ((PARITY) == UART_PARITY_ODD)\
                                || ((PARITY) == UART_PARITY_EVEN))
/**
  * @}
  */

/** @defgroup UART_DMA UART DMA 
  * @{
  */

#define UART_DMA_ENABLE                             ((uint16_t)(1 << 3))
#define UART_DMA_DISABLE                            ((uint16_t)(0 << 3))

#define IS_UART_DMA_CFG(CFG) (((CFG) == UART_DMA_ENABLE) || ((CFG) == UART_DMA_DISABLE))
/**
  * @}
  */

/** @defgroup UART_Hardware_Flow_Control UART Hardware Flow Control 
  * @{
  */

#define UART_AUTO_FLOW_CTRL_EN                      ((uint16_t)((1 << 5) | (1 << 1)))
#define UART_AUTO_FLOW_CTRL_DIS                     ((uint16_t)0x00)

#define IS_UART_AUTO_FLOW_CTRL(CTRL) (((CTRL) == UART_AUTO_FLOW_CTRL_EN) || ((CTRL) == UART_AUTO_FLOW_CTRL_DIS))

/**
  * @}
  */

/** @defgroup UART_Wrod_Length UART Wrod Length 
  * @{
  */

#define UART_WROD_LENGTH_7BIT                       ((uint16_t)(0 << 0))
#define UART_WROD_LENGTH_8BIT                       ((uint16_t)(1 << 0))

#define IS_UART_WORD_LENGTH(LEN) ((((LEN)) == UART_WROD_LENGTH_7BIT) || (((LEN)) == UART_WROD_LENGTH_8BIT))
/**
  * @}
  */

/** @defgroup UART_Stop_Bits UART Stop Bits
  * @{
  */

#define UART_STOP_BITS_1                           	((uint16_t)(0 << 2))
#define UART_STOP_BITS_2                            ((uint16_t)(1 << 2))

#define IS_UART_STOPBITS(STOP) (((STOP) == UART_STOP_BITS_1) || ((STOP) == UART_STOP_BITS_2))
/**
  * @}
  */

/** @cond private
  * @defgroup Uart_Tx_Rx_FIFO_CLEAR_BIT Uart TRx Fifo Clear Bits
  * @{
  */
  
#define FCR_CLEAR_RX_FIFO_Set           ((uint32_t)(1 << 1))
#define FCR_CLEAR_RX_FIFO_Reset         ((uint32_t)~(1 << 1))

#define FCR_CLEAR_TX_FIFO_Set           ((uint32_t)(1 << 2))
#define FCR_CLEAR_TX_FIFO_Reset         ((uint32_t)~(1 << 2))

/**
  * @}
  * @endcond
  */

/** @defgroup UART_Exported_Global_Variables UART Exported Global Variables
  * @{
  */

extern uint32_t UartFcrRegVal;

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup UART_Exported_Functions
  * @{
  */

extern void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* UART_InitStruct);
extern void UART_DeInit(UART_TypeDef* UARTx);
extern void UART_StructInit(UART_InitTypeDef* UART_InitStruct);
extern void UART_ReceiveData(UART_TypeDef* UARTx, uint8_t* outBuf, uint16_t count);
extern void UART_SendData(UART_TypeDef* UARTx, uint8_t* inBuf, uint16_t count);
__STATIC_INLINE void UART_INTConfig(UART_TypeDef* UARTx, uint32_t UART_IT, FunctionalState newState);
__STATIC_INLINE FlagStatus UART_GetFlagState(UART_TypeDef* UARTx, uint32_t UART_FLAG);
__STATIC_INLINE void UART_ClearTxFifo(UART_TypeDef* UARTx);
__STATIC_INLINE void UART_ClearRxFifo(UART_TypeDef* UARTx);
__STATIC_INLINE uint8_t UART_ReceiveByte(UART_TypeDef* UARTx);
__STATIC_INLINE void UART_SendByte(UART_TypeDef* UARTx, uint8_t data);
__STATIC_INLINE uint16_t UART_GetIID(UART_TypeDef* UARTx);

/**
  * @brief  Send one byte to tx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @param  data: byte to send.
  * @retval None
  */
__STATIC_INLINE void UART_SendByte(UART_TypeDef* UARTx, uint8_t data)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    
    UARTx->RB_THR = data;
    
    return;
}

/**
  * @brief  read one byte in rx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @retval the byte read.
  */
__STATIC_INLINE uint8_t UART_ReceiveByte(UART_TypeDef* UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    
    return (uint8_t)(UARTx->RB_THR);
}

/**
  * @brief  Get interrupt identifier.
  * @param  UARTx: selected UART peripheral.
  * @retval The interrupt identifier value.
  *   This return value can be one of the following values:
  *     @arg UART_INT_ID_LINE_STATUS: interrupt identifier--line status interrupt.
  *     @arg UART_INT_ID_RX_LEVEL_REACH: interrupt identifier--rx trigger level reached interrupt.
  *     @arg UART_INT_ID_RX_TMEOUT: interrupt identifier--line status interrupt.
  *     @arg UART_INT_ID_TX_EMPTY: interrupt identifier--line status interrupt.
  *     @arg UART_INT_ID_MODEM_STATUS: interrupt identifier--line status interrupt.
  */
__STATIC_INLINE uint16_t UART_GetIID(UART_TypeDef* UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    
    return (uint16_t)(UARTx->INTID_FCR & (0x0000000E));
}


/**
  * @brief  Clear UART tx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @retval None
  */
__STATIC_INLINE void UART_ClearTxFifo(UART_TypeDef* UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));

    UARTx->INTID_FCR |= FCR_CLEAR_TX_FIFO_Set;

    return;
}

/**
  * @brief  Clear UART rx FIFO.
  * @param  UARTx: selected UART peripheral.
  * @retval None
  */
__STATIC_INLINE void UART_ClearRxFifo(UART_TypeDef* UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));

    UARTx->INTID_FCR |= FCR_CLEAR_RX_FIFO_Set;

    return;
}

/**
  * @brief  Checks whether the specified UART flag is set or not.
  * @param  UARTx: selected UART peripheral.
  * @param  UART_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg UART_FLAG_RX_DATA_RDY: rx data is avaliable.
  *     @arg UART_FLAG_RX_OVERRUN: rx overrun.
  *     @arg UART_FLAG_PARTY_ERR: parity error.
  *     @arg UART_FLAG_FRAME_ERR: UARTx frame error.
  *     @arg UART_FLAG_BREAK_ERR: UARTx break error.
  *     @arg UART_FLAG_THR_EMPTY: tx FIFO is empty.
  *     @arg UART_FLAG_THR_TSR_EMPTY: tx FIFO and tx shift reg are both empty.
  *     @arg UART_FLAG_RX_FIFO_ERR: rx FIFO error.
  * @retval The new state of UART_FLAG (SET or RESET).
  */
__STATIC_INLINE FlagStatus UART_GetFlagState(UART_TypeDef* UARTx, uint32_t UART_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_UART_GET_FLAG(UART_FLAG));
    
    if(UARTx->LSR & UART_FLAG)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Enables or disables the specified UART interrupts.
  * @param  UARTx: selected UARTx peripheral.
  * @param  UART_IT: specifies the UART interrupts sources to be enabled or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg UART_INT_RD_AVA: enable Rx data avaliable interrupt.
  *     @arg UART_INT_FIFO_EMPTY: enable FIFO empty interrupt.  
  *     @arg UART_INT_LINE_STS: enable line status interrupt.
  *     @arg UART_INT_MODEM_STS: enable modem status interrupt.
  * @param  NewState: new state of the specified UART interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void UART_INTConfig(UART_TypeDef* UARTx, uint32_t UART_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_UART_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_UART_IT(UART_IT));

    if (newState == ENABLE)
    {
        /* Enable the selected UARTx interrupts */
        UARTx->DLH_INTCR |= UART_IT;
    }
    else
    {
        /* Disable the selected UARTx interrupts */
        UARTx->DLH_INTCR &= (uint32_t)~UART_IT;
    }

    return;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_UART_H_ */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor *****END OF FILE****/



