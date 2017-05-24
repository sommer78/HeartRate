/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_qdec.c
* @brief    This file provides all the QDEC firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-05-15
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_qdec.h"

/**
  * @brief Initializes the Qdecoder peripheral according to the specified 
  *   parameters in the QDEC_InitStruct
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_InitStruct: pointer to a QDEC_InitStruct structure that
  *     contains the configuration information for the specified Qdecoder peripheral
  * @retval None
  */
void QDEC_Init(QDEC_TypeDef* QDECx, QDEC_InitTypeDef* QDEC_InitStruct)
{
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_ACC_COUNT(QDEC_InitStruct->accCount));
    assert_param(IS_QDEC_TIMEOUT_COUNT(QDEC_InitStruct->timeoutCount));
    assert_param(IS_QDEC_AXIS_CONFIG(QDEC_InitStruct->axisConfig));

    //config enbale x/y/z and debounce time
    QDECx->REG_CR0 = ((QDEC_InitStruct->debounceTime * QDEC_InitStruct->sampleRate / 1000000 - 1) << 16)\
                    | (QDEC_InitStruct->axisConfig << 3)\
                    | (QDEC_InitStruct->axisConfig << 0);

    //clear interrupt, config acc count and timeout count
    QDECx->REG_CR1 = (QDEC_InitStruct->timeoutCount << 8)\
                    | (QDEC_InitStruct->accCount << 4)\
                    | (0x07 << 16);

    //config Qdecoder sample clock
    QDECx->REG_DIVNUM = SystemClock / QDEC_InitStruct->sampleRate - 1;

    // Clear all flags & Counter
    QDECx->REG_CR2 = 0x3FFFF;
    
    return;   
}

/**
  * @brief  Deinitializes the Qdecoder peripheral registers to their default reset values(turn off Qdecoder clock).
  * @param  QDECx: selected Qdecoder peripheral.
  * @retval None
  */
void QDEC_DeInit(QDEC_TypeDef* QDECx)
{
    RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, DISABLE);
}


/**
  * @brief  Fills each QDEC_InitStruct member with its default value.
  * @param  QDEC_InitStruct: pointer to an QDEC_InitStruct structure which will be initialized.
  * @retval None
  */
void QDEC_StructInit(QDEC_InitTypeDef* QDEC_InitStruct)
{
    QDEC_InitStruct->sampleRate     = 12800;                    //hz
    QDEC_InitStruct->debounceTime   = 5500;                     //us
    QDEC_InitStruct->axisConfig     = QDEC_Axis_Config_Y_En;    //enable x axis
    QDEC_InitStruct->accCount       = QDEC_ACC_COUNT_64;
    QDEC_InitStruct->timeoutCount   = QDEC_TIMEOUT_COUNT_200ms;
    
    return;
}


/**
  * @brief  Enables or disables the specified Qdecoder interrupts.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_IT: specifies the QDECODER interrupts sources to be enabled or disabled. 
  *   This parameter can be one of the following values:
  *     @arg QDEC_INT_NEW_DATA: When quad decoder get new data and state change(up or down), quad decoder raises an interrupt.
  *     @arg QDEC_INT_ACC_CT: When the accumulation conter reaches (QDEC_InitStruct->accCount), quad decoder raises an interrupt  
  *     @arg QDEC_INT_TIMEOUT_CT: When the time-out conter reaches (QDEC_InitStruct->timeoutCount), quad decoder raises an interrupt to mcu.
  * @param  NewState: new state of the specified QDECODER interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void QDEC_INTConfig(QDEC_TypeDef* QDECx, uint32_t QDEC_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_QDEC_INT_CONFIG(QDEC_IT));

    if (newState == ENABLE)
    {
        /* Enable the selected QDECODER interrupts */
        QDECx->REG_CR1 |= QDEC_IT;
    }
    else
    {
        /* Disable the selected QDECODER interrupts */
        QDECx->REG_CR1  &= (uint32_t)~QDEC_IT;
    }

    return;
}

/**
  * @brief  Checks whether the specified Qdecoder flag is set or not.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg QDEC_FLAG_OVERFLOW_X: The overflow flag for accunulation counter for X axis.
  *     @arg QDEC_FLAG_UNDERFLOW_X: The underflow flag for accunulation counter for X axis.
  *     @arg QDEC_FLAG_OVERFLOW_Y: The overflow flag for accunulation counter for Y axis.
  *     @arg QDEC_FLAG_UNDERFLOW_Y: The underflow flag for accunulation counter for Y axis.
  *     @arg QDEC_FLAG_OVERFLOW_Z: The overflow flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_UNDERFLOW_Z: The underflow flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_ILLEGAL_X: The illegal flag for accunulation counter for Y axis.
  *     @arg QDEC_FLAG_ILLEGAL_X: The illegal flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_ILLEGAL_X: The illegal flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_INT_X: X axis interrupt.
  *     @arg QDEC_FLAG_INT_Y: Y axis interrupt.
  *     @arg QDEC_FLAG_INT_Z: Z axis interrupt.
  * @retval The new state of QDEC_FLAG (SET or RESET).
  */
FlagStatus QDEC_GetFlagState(QDEC_TypeDef* QDECx, uint32_t QDEC_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_GET_FLAG(QDEC_FLAG));

    switch(QDEC_FLAG)
    {
        case QDEC_FLAG_OVERFLOW_X:
            if(QDECx->REG_SR_X & (QDEC_FLAG << 27))
            {
                bitstatus = SET;
            }
           break;

         case QDEC_FLAG_UNDERFLOW_X:
            if(QDECx->REG_SR_X & (QDEC_FLAG << 12))
            {
                bitstatus = SET;
            }
            break;

         case QDEC_FLAG_OVERFLOW_Y:
            if(QDECx->REG_SR_Y & (QDEC_FLAG << 26))
            {
                bitstatus = SET;
            }
           break;

         case QDEC_FLAG_UNDERFLOW_Y:
            if(QDECx->REG_SR_Y & (QDEC_FLAG << 11))
            {
                bitstatus = SET;
            }
            break;

          case QDEC_FLAG_OVERFLOW_Z:
            if(QDECx->REG_SR_Z & (QDEC_FLAG << 25))
            {
                bitstatus = SET;
            }
           break;

         case QDEC_FLAG_UNDERFLOW_Z:
            if(QDECx->REG_SR_Z & (QDEC_FLAG << 10))
            {
                bitstatus = SET;
            }
            break;
            
         case QDEC_FLAG_ILLEGAL_X:
            if(QDECx->REG_SR_Z & (QDEC_FLAG << 23))
            {
                bitstatus = SET;
            }
            break;
            
        case QDEC_FLAG_ILLEGAL_Y:
            if(QDECx->REG_SR_Z & (QDEC_FLAG << 22))
            {
                bitstatus = SET;
            }
            break;
        
        case QDEC_FLAG_ILLEGAL_Z:
            if(QDECx->REG_SR_Z & (QDEC_FLAG << 21))
            {
                bitstatus = SET;
            }
            break;

          case QDEC_FLAG_INT_X:
          case QDEC_FLAG_INT_Y:
          case QDEC_FLAG_INT_Z:
            if(QDECx->REG_CR1& QDEC_FLAG)
            {
                bitstatus = SET;
            }
            break;

          default:
            break;
          
    }
    
    return bitstatus;
}

/**
  * @brief  Enables or disables mask the specified Qdecoder axis interrupts.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_AXIS: specifies the Qdecoder axis. 
  *   This parameter can be one or logical OR of the following values:
  *     @arg QDEC_AXIS_X: The qdecoder X axis.
  *     @arg QDEC_AXIS_Y: The qdecoder Y axis.
  *     @arg QDEC_AXIS_Z: The qdecoder Z axis.
  * @param  NewState: new state of the specified Qdecoder interrupts mask.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void QDEC_INTMask(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_ALL_AXIS_DIR(QDEC_AXIS));
    assert_param(IS_FUNCTIONAL_STATE(newState));

    if (newState == ENABLE)
    {
        /* mask the selected QDEC interrupts */
        QDECx->REG_CR1 |= (QDEC_AXIS << 12);
    }
    else
    {
         /* unmask the selected QDEC interrupts */
         QDEC->REG_CR1 &= (~(QDEC_AXIS << 12));
    }
    
    return;
}
