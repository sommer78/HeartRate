/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_ir.h
* @brief     header file of IR driver.
* @details   
* @author    tifnan_ge
* @date      2015-05-18
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876x_IR_H_
#define _RTL876x_IR_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup IR IR
  * @brief IR driver module
  * @{
  */

/** @defgroup IR_Exported_Types IR Exported Types
  * @{
  */

/**
 * @brief IR initialize parameters
 *
 * ADCx initialize parameters
 */
typedef struct
{
    uint16_t frequency;             //khz
    uint16_t dutyCycle;             //
    uint16_t detectStartTime;       //0x17c defaut, user do not modify if not necessary
    uint16_t detectEndTime;         //0x23a defaut, user do not modify if not necessary
}IR_InitTypeDef, *PIR_InitTypeDef;

/**
  * @}
  */

/** @defgroup IR_Exported_Constants IR Exported Constants
  * @{
  */

#define IS_IR_PERIPH(PERIPH) ((PERIPH) == IR)
#define IR_RX_FIFO_SIZE                   32
#define IR_TX_FIFO_SIZE                   32

/** @defgroup IR_Interrupts_Definition  IR Interrupts Definition 
  * @{
  */

#define IR_INT_TF_EMPTY                             ((uint32_t)(1 << 17))     //Tx fifo empty
#define IR_INT_TX                                   ((uint32_t)(1 << 16))     //Tx interrupt
#define IR_INT_RF_FULL                              ((uint32_t)(1 << 21))     //Rx fifo full
#define IR_INT_RX                                   ((uint32_t)(1 << 20))     //Rx interrupt

#define IS_IR_INT_CONFIG(IT)  (((((IT) & (~(0x03 << 20))) == 0x00) && ((IT) != 0x00))\
                               || ((((IT) & (~(0x03 << 16))) == 0x00) && ((IT) != 0x00)))

#define IS_IR_TX_INT_CONFIG(CONFIG)   (((CONFIG) == IR_INT_TF_EMPTY) || ((CONFIG) == IR_INT_TX))

#define IS_IR_RX_INT_CONFIG(CONFIG)   (((CONFIG) == IR_INT_RF_FULL) || ((CONFIG) == IR_INT_RX))

/**
  * @}
  */


/** @defgroup IR_Flag IR Flag 
  * @{
  */

#define IR_FLAG_RX_ALL                               ((uint16_t)(1 << 9))      //Rx interrupt all flag
#define IR_FLAG_TX_ALL                               ((uint16_t)(1 << 8))      //Tx interrupt all flag
#define IR_FLAG_RX_END                               ((uint16_t)(1 << 7))      //Rx end
#define IR_FLAG_RX_START                             ((uint16_t)(1 << 6))      //Rx start
#define IR_FLAG_RX_FIFO_FULL                         ((uint16_t)(1 << 5))      //Rx FIFO full
#define IR_FLAG_RX_FIFO_EMPTY                        ((uint16_t)(1 << 4))      //Rx FIFO empty
#define IR_FLAG_RX_TH                                ((uint16_t)(1 << 3))      //Rx threshould interrupt flag
#define IR_FLAG_TX_FIFO_FULL                         ((uint16_t)(1 << 2))      //Tx FIFO full
#define IR_FLAG_TX_FIFO_EMPTY                        ((uint16_t)(1 << 1))      //Tx FIFO empty
#define IR_FLAG_TX_TH                                ((uint16_t)(1 << 0))      //Tx threshould interrupt flag

#define IS_IR_GET_FLAG(FLAG)  (((FLAG) == IR_FLAG_RX_ALL)   || ((FLAG) == IR_FLAG_TX_ALL)\
                                    || ((FLAG) == IR_FLAG_RX_END) || ((FLAG) == IR_FLAG_RX_START)\
                                    || ((FLAG) == IR_FLAG_RX_FIFO_FULL) || ((FLAG) == IR_FLAG_RX_FIFO_EMPTY)\
                                    || ((FLAG) == IR_FLAG_RX_TH) || ((FLAG) == IR_FLAG_TX_FIFO_FULL)\
                                    || ((FLAG) == IR_FLAG_TX_FIFO_EMPTY) || ((FLAG) == IR_FLAG_TX_TH))

/**
  * @}
  */

/** @defgroup IR_Interrupts_Clear_Flag IR Interrupts Clear Flag
  * @{
  */

#define IR_INT_CLEAR_RX_END                          ((uint32_t)(1 << 29))      //write this bit to clear rx end interrupt flag
#define IR_INT_CLEAR_RX_START                        ((uint32_t)(1 << 28))      //write this bit to clear rx start interrupt flag
#define IR_INT_CLEAR_RX_FIFO_FULL                    ((uint32_t)(1 << 27))      //write this bit to clear rx fifo full interrupt flag
#define IR_INT_CLEAR_RX_TH                           ((uint32_t)(1 << 26))      //write this bit to clear rx fifo th interrupt flag
#define IR_INT_CLEAR_TX_FIFO_EMPTY                   ((uint32_t)(1 << 25))      //write this bit to clear tx fifo empty interrupt flag
#define IR_INT_CLEAR_TX_TH                           ((uint32_t)(1 << 23))      //write this bit to clear tx fifo th interrupt flag

#define IS_IR_INT_CLEAR_FLAG(FLAG) (((FLAG) == IR_INT_CLEAR_RX_END) || ((FLAG) == IR_INT_CLEAR_RX_START)\
                                    || ((FLAG) == IR_INT_CLEAR_RX_FIFO_FULL) || ((FLAG) == IR_INT_CLEAR_RX_TH)\
                                    || ((FLAG) == IR_INT_CLEAR_TX_FIFO_EMPTY) || ((FLAG) == IR_INT_CLEAR_TX_TH))

/**
  * @}
  */

/** @defgroup IR_Status IR Status
  * @{
  */

#define IR_STATUS_IDLE                              ((uint16_t)(0x00))
#define IR_STATUS_BUSY                              ((uint16_t)(0x01))

#define IS_IR_STATUS(STATUS) (((STATUS) == IR_STATUS_IDLE) || ((STATUS) == IR_STATUS_BUSY))

/**
  * @}
  */

/** @defgroup IR_Mode IR Mode
  * @{
  */

#define IR_MODE_TX                                 ((uint16_t)(1 << 0))
#define IR_MODE_RX                                 ((uint16_t)(1 << 1))

#define IS_IR_MODE(MODE) (((MODE) == IR_MODE_TX) || ((MODE) == IR_MODE_RX))

/**
  * @}
  */

/** @defgroup IR_Data_Type IR Data Type 
  * @{
  */

#define IR_DATA_TYPE_NORMAL                         ((uint16_t)(0 << 1))
#define IR_DATA_TYPE_INVERSE                        ((uint16_t)(1 << 1))

#define IS_IR_DATA_TYPE(TYPE) (((TYPE) == IR_DATA_TYPE_NORMAL) || ((TYPE) == IR_DATA_TYPE_INVERSE))

/**
  * @}
  */

/** @defgroup IR_Frequency IR Frequency 
  * @{
  */

#define IS_IR_FREQUENCY(F) (((F) >= 1) && ((F) <= 64))

/**
  * @}
  */

/** @defgroup IR_Tx_Rx_Threshold IR TRX Threshold
  * @{
  */

#define IS_IR_TXRX_THRESHOLD(THD) (((THD) >= 0) && ((THD) <= 31))

/**
  * @}
  */

/** @defgroup IR_Detect_Start_End_Time IR Detect Time
  * @{
  */

#define IS_IR_DETECT_START_END_TIME(TIME) (((TIME) > 0) && ((TIME) <= 0x3FF))

/**
  * @}
  */

/** @cond private
  * @defgroup IR_START_TX_RX_BIT IR Start TRx Bit
  * @{
  */
#define CSR1_START_TX_Set                    ((uint32_t)(1 << 0))
#define CSR1_START_TX_Reset                  ((uint32_t)~(1 << 0))

#define CSR2_START_RX_Set                    ((uint32_t)(1 << 0))
#define CSR2_START_RX_Reset                  ((uint32_t)~(1 << 0))
/**
  * @}
  * @endcond
  */

/** @defgroup IR_Exported_Global_Variables
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */ 

/** @defgroup IR_Exported_Functions IR Exported Functions
  * @{
  */

extern void IR_Init(IR_TypeDef* IRx, IR_InitTypeDef* IR_InitStruct);
extern void IR_DeInit(IR_TypeDef* IRx);
extern void IR_StructInit(IR_InitTypeDef* IR_InitStruct);
extern void IR_SendData(IR_TypeDef* IRx, uint16_t* pBuf, uint16_t length);
extern void IR_ReceiveData(IR_TypeDef* IRx, uint16_t* pBuf, uint16_t length);
__STATIC_INLINE void IR_SetTxThreshold(IR_TypeDef* IRx, uint16_t thd);
__STATIC_INLINE void IR_SetRxThreshold(IR_TypeDef* IRx, uint16_t thd);
__STATIC_INLINE void IR_SetTxInverse(IR_TypeDef* IRx, FunctionalState NewState);
__STATIC_INLINE void IR_SetRxInverse(IR_TypeDef* IRx, FunctionalState NewState);
__STATIC_INLINE void IR_INTConfig(IR_TypeDef* IRx, uint32_t IR_IT, FunctionalState newState);
__STATIC_INLINE FlagStatus IR_GetFlagState(IR_TypeDef* IRx, uint32_t IR_FLAG);
__STATIC_INLINE void IR_ClearINTPendingBit(IR_TypeDef* IRx, uint32_t IR_CLEAR_IT);
__STATIC_INLINE void IR_ClearFifo(IR_TypeDef* IRx);
__STATIC_INLINE void IR_Cmd(IR_TypeDef* IRx, uint16_t IR_MODE, FunctionalState NewState);
__STATIC_INLINE void IR_StartTx(IR_TypeDef* IRx);
__STATIC_INLINE void IR_StopTx(IR_TypeDef* IRx);
__STATIC_INLINE void IR_StartRx(IR_TypeDef* IRx);
__STATIC_INLINE void IR_StopRx(IR_TypeDef* IRx);
__STATIC_INLINE uint16_t IR_GetRxFifoSize(IR_TypeDef* IRx);
__STATIC_INLINE uint16_t IR_GetTxFifoFreeSize(IR_TypeDef* IRx);

/**
  * @brief  Enables or disables the specified IR interrupts.
  * @param  IRx: selected IR peripheral.
  * @param  IR_IT: specifies the IR interrupts sources to be enabled or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg IR_INT_TF_EMPTY: enable tx fifo empty interrupt.
  *     @arg IR_INT_TX: enable tx interrupt.  
  *     @arg IR_INT_RF_FULL: enable tx fifo full interrupt.
  *     @arg IR_INT_RX: enable rx interrupt.
  * @param  NewState: new state of the specified IR interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void IR_INTConfig(IR_TypeDef* IRx, uint32_t IR_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_IR_INT_CONFIG(IR_IT));

    if (newState == ENABLE)
    {
        /* Enable the selected IR interrupts */
        IRx->INTCR |= IR_IT;
    }
    else
    {
        /* Disable the selected IR interrupts */
        IRx->INTCR &= (~IR_IT);
    }

    return;
}

/**
  * @brief  Clears the IR interrupt pending bits.
  * @param  IRx: selected IR peripheral.
  * @param  IR_IT: specifies the interrupt pending bit to clear. 
  *   This parameter can be any combination of the following values:
  *     @arg IR_INT_CLEAR_RX_END: rx end
  *     @arg IR_INT_CLEAR_RX_START: rx start
  *     @arg IR_INT_CLEAR_RX_FIFO_FULL: rx fifo full
  *     @arg IR_INT_CLEAR_RX_TH: rx threshold reached
  *     @arg IR_INT_CLEAR_TX_FIFO_EMPTY: tx fifo empty
  *     @arg IR_INT_CLEAR_TX_TH: tx threshold reached
  * @retval None
  */
__STATIC_INLINE void IR_ClearINTPendingBit(IR_TypeDef* IRx, uint32_t IR_CLEAR_IT)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    assert_param(IS_IR_INT_CLEAR_FLAG(IR_CLEAR_IT));

    IRx->INTCR |= IR_CLEAR_IT;
    
    return;
}


/**
  * @brief  Checks whether the specified IR flag is set or not.
  * @param  IRx: selected IR peripheral.
  * @param  IR_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg IR_FLAG_RX_END: rx end.
  *     @arg IR_FLAG_RX_START: rx start.
  *     @arg IR_FLAG_RX_FIFO_FULL: rx fifo full.
  *     @arg IR_FLAG_RX_FIFO_EMPTY: rx fifo empty.
  *     @arg IR_FLAG_RX_TH: tx threshold reached.
  *     @arg IR_FLAG_TX_FIFO_FULL: tx fifo full.
  *     @arg IR_FLAG_TX_FIFO_EMPTY: tx fifo empty.
  *     @arg IR_FLAG_TX_TH: rx threshold reached.
  * @retval The new state of IR_FLAG (SET or RESET).
  */
__STATIC_INLINE FlagStatus IR_GetFlagState(IR_TypeDef* IRx, uint32_t IR_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    assert_param(IS_IR_GET_FLAG(IR_FLAG));
    
    if(IRx->SR & IR_FLAG)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Clear IR FIFO.
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
__STATIC_INLINE void IR_ClearFifo(IR_TypeDef* IRx)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    IRx->INTCR |= (1 << 30);

    return;
}

/**
  * @brief  Enables or disables the IR peripheral.
  * @param  IRx: selected IR peripheral.
  * @param  IR_MODE: IR mode.
  *   This parameter can be one of the following values, or logic OR of them:
  *     @arg IR_MODE_TX: IR tx.
  *     @arg IR_MODE_RX: IR rx.
  * @param  NewState: new state of the IR peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void IR_Cmd(IR_TypeDef* IRx, uint16_t IR_MODE, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    assert_param(IS_IR_MODE(IR_MODE));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE)
    {
         IRx->INTCR |= IR_MODE;
    }
    else
    {
        IRx->INTCR &= ~IR_MODE;
    }

    return;
}


/**
  * @brief  Start IR tx.
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
__STATIC_INLINE void IR_StartTx(IR_TypeDef* IRx)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    IRx->CSR1 |= CSR1_START_TX_Set;

    return;
}

/**
  * @brief  Stop IR tx.
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
__STATIC_INLINE void IR_StopTx(IR_TypeDef* IRx)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    IRx->CSR1 &= CSR1_START_TX_Reset;

    return;
}

/**
  * @brief  Start IR rx.
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
__STATIC_INLINE void IR_StartRx(IR_TypeDef* IRx)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    IRx->CSR2 |= CSR2_START_RX_Set;

    return;
}

/**
  * @brief  Stop IR rx.
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
__STATIC_INLINE void IR_StopRx(IR_TypeDef* IRx)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    IRx->CSR2 &= CSR2_START_RX_Reset;

    return;
}

/**
  * @brief  Get Rx Fifo current size.
  * @param  IRx: selected IR peripheral.
  * @retval None
  */
__STATIC_INLINE uint16_t IR_GetRxFifoSize(IR_TypeDef* IRx)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    
    return ((uint16_t)((IRx->SR & (0x3F << 24)) >> 24));
}

/**
  * @brief  Get free size of tx fifo .
  * @param  IRx: selected IR peripheral.
  * @retval the free size of Tx Fifo(0--32)
  */
__STATIC_INLINE uint16_t IR_GetTxFifoFreeSize(IR_TypeDef* IRx)
{
     assert_param(IS_IR_PERIPH(IRx));
     
     return (uint16_t)(IR_TX_FIFO_SIZE - ((IRx->SR & (0x3F << 16)) >> 16));
}

/**
  * @brief  set tx threshold.
  * @param  IRx: selected IR peripheral.
  * @param  thd: tx threshold.
  * @retval None
  */
__STATIC_INLINE void IR_SetTxThreshold(IR_TypeDef* IRx, uint16_t thd)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));
    
    IRx->CSR1 &= (~(0x1F << 2));
    IRx->CSR1 |= (thd << 2);
    
    return;
}

/**
  * @brief  set tx threshold.
  * @param  IRx: selected IR peripheral.
  * @param  thd: rx threshold.
  * @retval None
  */
__STATIC_INLINE void IR_SetRxThreshold(IR_TypeDef* IRx, uint16_t thd)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    IRx->CR &= (~((uint32_t)0x1F << 27));
    IRx->CR |= (thd << 27);
    
    return;
}

/**
  * @brief  set or reset tx data inverse.
  * @param  IRx: selected IR peripheral.
  * @param  NewState: new state of the IR peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void IR_SetTxInverse(IR_TypeDef* IRx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    if(NewState == ENABLE)
    {
        IRx->CSR1 |= (1 << 1);
    }
    else
    {
        IRx->CSR1 &= (~(1 << 1));
    }
    
    return;
}

/**
  * @brief  set or reset rx data inverse.
  * @param  IRx: selected IR peripheral.
  * @param  NewState: new state of the IR peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void IR_SetRxInverse(IR_TypeDef* IRx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_IR_PERIPH(IRx));

    if(NewState == ENABLE)
    {
        IRx->CSR2 |= (1 << 1);
    }
    else
    {
        IRx->CSR2 &= (~(1 << 1));
    }
    
    return;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876x_IR_H_ */

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



