/**
**********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_nvic.c
* @brief    This file provides all the NVIC firmware functions.
* @details
* @author   elliot chen
* @date            2015-05-19
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_nvic.h"

/**
  * @brief  Initializes the NVIC peripheral according to the specified
  *         parameters in the NVIC_InitStruct.
  * @param  NVIC_InitStruct: pointer to a NVIC_InitTypeDef structure that contains
  *         the configuration information for the specified NVIC peripheral.
  * @retval None
  */
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{   
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NVIC_InitStruct->NVIC_IRQChannelCmd));
    
    if (NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)
    {
        if(NVIC_InitStruct->NVIC_IRQChannel >= 64)
        {
            /*Clear the status of peripheral interrupt*/
             //*((uint32_t *)0x40008004);
            
            /*Enable Peripheral IRQ mask*/
            *((volatile uint32_t *)0x4000800c) |= BIT(NVIC_InitStruct->NVIC_IRQChannel - 64);
        
            NVIC_InitStruct->NVIC_IRQChannel = PERIPHERAL_IRQ;
        }
        
        NVIC_ClearPendingIRQ(NVIC_InitStruct->NVIC_IRQChannel);
        NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, NVIC_InitStruct->NVIC_IRQChannelPriority);
        NVIC_EnableIRQ(NVIC_InitStruct->NVIC_IRQChannel);       
    }
    else
    {
        /* Disable the Selected IRQ Channels */
        NVIC_DisableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
    }
}

  
/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

