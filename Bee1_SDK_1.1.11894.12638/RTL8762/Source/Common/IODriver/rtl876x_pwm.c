/**
**********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_pwm.c
* @brief    This file provides all the PWM firmware functions.
* @details
* @author   elliot chen
* @date     2015-05-15
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_pwm.h"

/**
  * @brief  Initializes the PWMx peripheral according to 
  *         the specified parameters in the PWM_InitStruct.
  * @param  PWMx: where x can be 0 to 3 to select the PWM peripheral.
  * @param  PWM_InitStruct: pointer to a PWM_InitTypeDef
  *         structure that contains the configuration information for the 
  *         specified PWM peripheral.
  * @retval None
  */
void PWM_Init(PWM_TypeDef* PWMx, PWM_InitTypeDef* PWM_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_PWM_ALL_PERIPH(PWMx));
    
    /* Disable PWM */
    PWMx->CR0 &= ~(BIT(0));

    /* Configure PWM timer selection */
    PWMx->CR0 = PWM_InitStruct->PWM_TIMIndex << 1;

    /* Configure PWM period and duty */
    PWMx->CR1 = PWM_InitStruct->PWM_Duty << 16
                |(PWM_InitStruct->PWM_Period);      
}

/**
  * @brief  Fills each PWM_InitStruct member with its default value.
  * @param  PWM_InitStruct : pointer to a PWM_InitTypeDef structure which will be initialized.
  * @retval None
  */
void PWM_StructInit(PWM_InitTypeDef* PWM_InitStruct)
{
    PWM_InitStruct->PWM_TIMIndex = 6;
    PWM_InitStruct->PWM_Duty = 1;
    PWM_InitStruct->PWM_Period = 1;
}

/**
  * @brief  Enables or disables the specified PWMx peripheral.
  * @param PWMx: where x can be 0 to 3 to select the PWMx peripheral.
  * @param  NewState: new state of the PWMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWM_Cmd(PWM_TypeDef* PWMx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_PWM_ALL_PERIPH(PWMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
  
    if (NewState != DISABLE)
    {
        /* Enable the PWM */
        PWMx->CR0 |= BIT(0);
    }
    else
    {
        /* Disable the PWM */
        PWMx->CR0 &= ~(BIT(0));
    }
}
  
/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/


