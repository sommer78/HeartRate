/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_gpio.c
* @brief    This file provides all the GPIO firmware functions.
* @details
* @author   elliot chen
* @date     2015-05-20
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"

/**
  * @brief  Deinitializes the GPIO peripheral registers to their default reset values.
  * @param  None.
  * @retval None.
  */
void GPIO_DeInit(void)
{
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, DISABLE);
}

/**
  * @brief  Initializes the GPIO peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_InitTypeDef* GPIO_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin)); 
    assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
    assert_param(IS_GPIOIT_LEVEL_TYPE(GPIO_InitStruct->GPIO_ITTrigger));
    assert_param(IS_GPIOIT_POLARITY_TYPE(GPIO_InitStruct->GPIO_ITPolarity));
    assert_param(IS_GPIOIT_DEBOUNCE_TYPE(GPIO_InitStruct->GPIO_ITDebounce));

    /* GPIO configure */
    if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT)
    {
        GPIO->DATADIR |= GPIO_InitStruct->GPIO_Pin;
    }
    else
    {
        /*Configure GPIO input mode */
        GPIO->DATADIR = GPIO->DATADIR&(~GPIO_InitStruct->GPIO_Pin);
        
        if(GPIO_InitStruct->GPIO_ITCmd == ENABLE)
        {
            
            GPIO->INTMASK = ~GPIO_Pin_All;
			
			/* Config vendor register */
            if((GPIO_InitStruct->GPIO_Pin) & 0x3C)
            {
                PERIPHINT->MODE &= ~(((GPIO_InitStruct->GPIO_Pin)&0x3C) >> 1);
            }
            
            if((GPIO_InitStruct->GPIO_Pin) & 0xFFFFFFC0)
            {
                PERIPHINT->MODE &= ~(BIT(16));
            }

            /* configure GPIO interrupt trigger type */
            if(GPIO_InitStruct->GPIO_ITTrigger == GPIO_INT_Trigger_LEVEL)
            {
                GPIO->INTTYPE = GPIO->INTTYPE & (~GPIO_InitStruct->GPIO_Pin);
                
				/* Level-sensitive synchronization enable register */
				GPIO->LSSYNC |= GPIO_InitStruct->GPIO_Pin; 
            }
            else
            {
                GPIO->INTTYPE = (GPIO->INTTYPE & (~GPIO_InitStruct->GPIO_Pin))
                                |GPIO_InitStruct->GPIO_Pin;
            }
            
            /* configure Interrupt polarity register */
            if(GPIO_InitStruct->GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_LOW)
            {
                GPIO->INTPOLARITY = GPIO->INTPOLARITY & (~GPIO_InitStruct->GPIO_Pin);
            }
            else
            {
                GPIO->INTPOLARITY = (GPIO->INTPOLARITY & (~GPIO_InitStruct->GPIO_Pin))
                                    |GPIO_InitStruct->GPIO_Pin;
            }
            /* Configure Debounce enable register */
            if(GPIO_InitStruct->GPIO_ITDebounce == GPIO_INT_DEBOUNCE_DISABLE)
            {
                GPIO->DEBOUNCE = GPIO->DEBOUNCE & (~GPIO_InitStruct->GPIO_Pin);
            }
            else
            {
                GPIO->DEBOUNCE = (GPIO->DEBOUNCE & (~GPIO_InitStruct->GPIO_Pin))
                                 |GPIO_InitStruct->GPIO_Pin;
            }

            /* Configure Interrupt enable register */
            GPIO->INTEN |= GPIO_InitStruct->GPIO_Pin;
        }
    }   
}

/**
  * @brief    Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure which will
  *    be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
    /* Reset GPIO init structure parameters values */
    GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;
    GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct->GPIO_ITCmd = DISABLE;
    GPIO_InitStruct->GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
    GPIO_InitStruct->GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_InitStruct->GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
}

/**
  * @brief enable the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIO_INTConfig(uint32_t GPIO_Pin, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
        
    if (NewState != DISABLE)
    {
        /* Enable the selected GPIO pin interrupts */
        GPIO->INTEN |= GPIO_Pin;

		/* Configure vendor register to enable interrupt */
        if(GPIO_Pin & 0x3C)
        {
            PERIPHINT->EN |= (GPIO_Pin & 0x3c) >> 1;
        }
        if(GPIO_Pin & 0xFFFFFFC0)
        {
            PERIPHINT->EN |= BIT(16);
        }
    }
    else
    {
        /* Disable the selected GPIO pin interrupts */
        GPIO->INTEN &= ~GPIO_Pin;

		/* Configure vendor register to disable interrupt */
        if(GPIO_Pin & 0x3C)
        {
            PERIPHINT->EN &= ~((GPIO_Pin & 0x3C) >> 1);
        }
    }
}

/**
  * @brief clear the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIO_ClearINTPendingBit(uint32_t GPIO_Pin)
{   
    /* Check the parameters */
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));
    
    GPIO->INTCLR = GPIO_Pin;
    
#if 0   
        if(GPIO_Pin & 0x3C)
        {
            *((volatile uint32_t *)0x40008004) = (GPIO_Pin&0x3C) >> 1;
        }
        
        if(GPIO_Pin & 0xFFFFFFC0)
        {
            *((volatile uint32_t *)0x40008004) = BIT(16);
        }
#endif
}

/**
  * @brief mask the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIO_MaskINTConfig(uint32_t GPIO_Pin, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
        
    if (NewState != DISABLE)
    {
        GPIO->INTMASK |= GPIO_Pin;
    }
    else
    {
        GPIO->INTMASK &= ~(GPIO_Pin);
    }
}

/**
  * @brief get the specified GPIO pin.
  * @param  Pin_num: which can be P0_0 to P3_3 or P4_0 to P4_3.
  * @retval  GPIO pin for GPIO initialization.
  */
uint32_t GPIO_GetPin(uint8_t Pin_num)
{
    /* Check the parameters */
    assert_param(IS_PIN_NUM(Pin_num));
    
    if(Pin_num <= P3_3)
    {
        return BIT(Pin_num);
    }
    if((Pin_num <= P4_3)&&(Pin_num >= P4_0))
    {
        return BIT(Pin_num - 4);
    }
    return 0xFF;    
}

/**
  * @brief get the specified GPIO pin number.
  * @param  Pin_num: which can be P0_0 to P3_3 or P4_0 to P4_3.
  * @retval  GPIO pin number.
  */
uint8_t GPIO_GetNum(uint8_t Pin_num)
{
    /* Check the parameters */
    assert_param(IS_PIN_NUM(Pin_num));
    
    if(Pin_num <= P3_3)
    {
        return Pin_num;
    }
    if((Pin_num <= P4_3)&&(Pin_num >= P4_0))
    {
        return Pin_num - 4;
    }
    return 0xFF;            
}
  
/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

