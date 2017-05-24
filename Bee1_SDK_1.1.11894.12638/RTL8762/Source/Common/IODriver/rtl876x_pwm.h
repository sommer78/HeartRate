/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_pwm.h
* @brief     
* @details   
* @author    elliot chen
* @date      2015-05-15
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __RTL876X_PWM_H
#define __RTL876X_PWM_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_tim.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */
  
/** @addtogroup PWM PWM
  * @{
  */ 

/** @defgroup PWM_Exported_Types PWM Exported Types
  * @{
  */

/** 
  * @brief  PWM Init structure definition
  */
typedef struct
{
    uint16_t PWM_TIMIndex;          /*!< Specifies the clock index.
                                      This parameter can be a value of 0 to 7 */
                                    
    uint32_t PWM_Duty;              /*!< Specifies the on-duty duration of PWM pulse */

    uint32_t PWM_Period;            /*!< Specifies the the period of PWM pulse. */
    
} PWM_InitTypeDef;

/**
  * @}
  */ 

/** @defgroup PWM_Exported_constants PWM Exported Constants 
  * @{
  */

#define IS_PWM_ALL_PERIPH(PERIPH) (((PERIPH) == PWM0) || \
                                   ((PERIPH) == PWM1) || \
                                   ((PERIPH) == PWM2) || \
                                   ((PERIPH) == PWM3))
/**
  * @}
  */  

/** @defgroup PWM_Exported_Functions PWM Exported Functions
  * @{
  */

void PWM_Init(PWM_TypeDef* PWMx, PWM_InitTypeDef* PWM_InitStruct);
void PWM_Cmd(PWM_TypeDef* PWMx, FunctionalState NewState);
__STATIC_INLINE void PWM_ChangeDuty(PWM_TypeDef* PWMx, uint32_t data);
__STATIC_INLINE void PWM_ChangePeriod(PWM_TypeDef* PWMx, uint32_t data);

/**
  * @brief  Set PWM duty time through the PWMx peripheral.
  * @param  PWMx: where x can be 0 to 3
  * @param  data:duty time can be set from 0 to 0xffff.
  * @retval None
  */
__STATIC_INLINE void PWM_ChangeDuty(PWM_TypeDef* PWMx, uint32_t data)
{
	/* Check the parameters */
    assert_param(IS_PWM_ALL_PERIPH(PWMx));
	
    /* Set duty time */
    PWMx->CR1 = (PWMx->CR1 & 0xffff) | (data << 16);
}

/**
  * @brief  Set PWM period time through the PWMx peripheral.
  * @param  PWMx: where x can be 0 to 3
  * @param  data:period time can be set from 0 to 0xffff.
  * @retval None
  */
__STATIC_INLINE void PWM_ChangePeriod(PWM_TypeDef* PWMx, uint32_t data)
{
	/* Check the parameters */
    assert_param(IS_PWM_ALL_PERIPH(PWMx));
	
    /* Set duty time */
    PWMx->CR1 = (PWMx->CR1 & 0xffff0000) | data;
}


#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_PWM_H*/

/** 
  * @}
  */
  
/** 
  * @}
  */

/** 
  * @}
  */

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

