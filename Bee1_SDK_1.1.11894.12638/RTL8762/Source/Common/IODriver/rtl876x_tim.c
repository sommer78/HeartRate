/**
**********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_tim.c
* @brief    This file provides all the Timer firmware functions.
* @details
* @author   elliot chen
* @date            2015-05-15
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_tim.h"
#include "rtl876x_rcc.h"

/* TIM Private Defines */
#define TIMER_CLK_SOURCE_REG *((volatile uint32_t *)0x40000360UL)
#define REG_LOW_PRI_INT_EN   *((volatile uint32_t *)0x4000800CUL)
#define REG_PERI_TIM_EVT_CTRL *((volatile uint32_t *)0x40000384UL)

/**
  * @brief  Deinitializes the TIMx peripheral registers to their default reset values.
  * @param  TIMx: where x can be 0 to 7 to select the TIM peripheral.
  * @retval None
  */
void TIM_DeInit()
{   
    /*enable timer IP clock and function */
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, DISABLE);
}

/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to 
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TIMx: where x can be 0 to 7 to select the TIM peripheral.
  * @param  TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the 
  *         specified TIM peripheral.
  * @retval None
  */
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
    uint32_t timerid = 0;
    uint32_t tempreg = 0;
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_CLOCK_SRC(TIM_TimeBaseInitStruct->TIM_ClockSrc));
    assert_param(IS_TIM_MODE(TIM_TimeBaseInitStruct->TIM_Mode));

    /* Select clock source which can be 32 KHz or 10 MHz */
    tempreg = (uint32_t)TIMx;
    timerid = (tempreg - TIM0_REG_BASE)/20; 
    if(TIM_TimeBaseInitStruct->TIM_ClockSrc == TIM_CLOCK_10MHZ)
    {
        TIMER_CLK_SOURCE_REG |= BIT(timerid);
    }
    else
    {
        TIMER_CLK_SOURCE_REG &= ~(BIT(timerid));
    }
    
    /* set timer mode and mask interrupt */
    TIMx->ControlReg = (TIM_TimeBaseInitStruct->TIM_Mode << 1) | BIT(2); 
    
    /* set timer period */
    TIMx->LoadCount = TIM_TimeBaseInitStruct->TIM_Period;

    /* configure timer event duration */
    if(TIM_TimeBaseInitStruct->TIM_EventMode == ENABLE)
    {
        REG_PERI_TIM_EVT_CTRL = BIT(7 + (TIM_TimeBaseInitStruct->TIM_EventIndex << 3))
                                |(timerid << (4 + (TIM_TimeBaseInitStruct->TIM_EventIndex << 3)))
                                |(TIM_TimeBaseInitStruct->TIM_EventDuration << (TIM_TimeBaseInitStruct->TIM_EventIndex << 3));
    }

    /* Clear the IT status */
    TIMx->EOI;
}

/**
  * @brief  Fills each TIM_InitStruct member with its default value.
  * @param  TIM_InitStruct : pointer to a TIM_InitTypeDef structure which will be initialized.
  * @retval None
  */
void TIM_StructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
    TIM_TimeBaseInitStruct->TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_TimeBaseInitStruct->TIM_Mode= TIM_Mode_UserDefine;
    TIM_TimeBaseInitStruct->TIM_Period = 0xfff;
    TIM_TimeBaseInitStruct->TIM_EventMode = FALSE;
    TIM_TimeBaseInitStruct->TIM_EventIndex = 1;
    TIM_TimeBaseInitStruct->TIM_EventDuration = TIM_EventDuration_64us;
}

/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
  
    if (NewState != DISABLE)
    {
        /* Enable the TIM Counter */
        TIMx->ControlReg |= BIT(0);
    }
    else
    {
        /* Disable the TIM Counter */
        TIMx->ControlReg &= ~(BIT(0));
    }
}

/**
  * @brief  change TIM period value.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The new state of success or not  (SET or RESET).
  */
FlagStatus TIM_ChangePeriod(TIM_TypeDef* TIMx, uint32_t period)
{
    FlagStatus flagstatus = RESET;
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    if(~(TIMx->ControlReg & BIT(0)))
    {
        TIMx->LoadCount = period;
        flagstatus = SET;
    }
    return flagstatus;
}

/**
  * @brief  Enables or disables the specified TIMx interrupt.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_INTConfig(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

     if (NewState != DISABLE)
    {
        /* Enable the Interrupt sources */
        TIMx->ControlReg &= ~(BIT(2));
        
        /* If timer is used, set all the peripheral timer interrupt bit. 
            Control the timer interrupt via DW timer register. */
        REG_LOW_PRI_INT_EN |= 0x03E0;
    }
    else
    {
        /* Disable the Interrupt sources */
        TIMx->ControlReg |= BIT(2);
    }
}
  
/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

