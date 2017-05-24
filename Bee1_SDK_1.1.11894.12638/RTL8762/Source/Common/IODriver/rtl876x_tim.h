/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_tim.h
* @brief     
* @details   
* @author    elliot chen
* @date      2015-05-15
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __RTL876X_TIM_H
#define __RTL876X_TIM_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup TIM TIM
  * @brief TIM driver module
  * @{
  */ 

/** @defgroup TIM_Exported_Types TIM Exported Types
  * @{
  */

/** 
  * @brief  TIM Init structure definition
  */
typedef struct
{
    uint16_t TIM_ClockSrc;          /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Source */
                                    
    uint16_t TIM_Mode;              /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Mode */

    uint32_t TIM_Period;              /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFFFFFF.  */
    uint32_t TIM_EventMode;

    uint32_t TIM_EventIndex;

    uint32_t TIM_EventDuration;
} TIM_TimeBaseInitTypeDef;

/**
  * @}
  */

/** @defgroup TIM_Exported_constants TIM Exported Constants 
  * @{
  */

#define IS_TIM_ALL_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                   ((PERIPH) == TIM2) || \
                                   ((PERIPH) == TIM3) || \
                                   ((PERIPH) == TIM4) || \
                                   ((PERIPH) == TIM5) || \
                                   ((PERIPH) == TIM6) || \
                                   ((PERIPH) == TIM7))                                  
 

/** @defgroup TIM_Clock_Source TIM Clock Source
  * @{
  */

#define TIM_CLOCK_32KHZ                    ((uint16_t)0x0000)
#define TIM_CLOCK_10MHZ                    ((uint16_t)0x0001)
#define IS_TIM_CLOCK_SRC(HZ) (((HZ) == TIM_CLOCK_32KHZ) || \
                             ((HZ) == TIM_CLOCK_10MHZ))
/**
  * @}
  */ 

/** @defgroup TIM_Mode TIM Mode
  * @{
  */

#define TIM_Mode_FreeRun                    ((uint16_t)0x0000)
#define TIM_Mode_UserDefine                ((uint16_t)0x0001)
#define IS_TIM_MODE(mode) (((mode) == TIM_Mode_FreeRun) || \
                             ((mode) == TIM_Mode_UserDefine))
/**
  * @}
  */

/** @defgroup TIM_Event_Duration TIM Event Duration
  * @{
  */

#define TIM_EventDuration_32us                    ((uint16_t)0x0000)
#define TIM_EventDuration_64us                    ((uint16_t)0x0001)
#define TIM_EventDuration_128us                   ((uint16_t)0x0002)
#define TIM_EventDuration_256us                   ((uint16_t)0x0003)

#define IS_TIM_Event_DURATION(duration) (((duration) == TIM_EventDuration_32us) || \
                                        ((duration) == TIM_EventDuration_64us) || \
                                        ((duration) == TIM_EventDuration_128us) || \
                                        ((duration) == TIM_EventDuration_256us))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup TIM_Exported_Functions TIM Exported Functions
  * @{
  */

void TIM_DeInit(void);
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_StructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
FlagStatus TIM_ChangePeriod(TIM_TypeDef* TIMx, uint32_t period);
void TIM_INTConfig(TIM_TypeDef* TIMx, FunctionalState NewState);
__STATIC_INLINE uint32_t TIM_GetCurrentValue(TIM_TypeDef* TIMx);
__STATIC_INLINE ITStatus TIM_GetINTStatus(TIM_TypeDef* TIMx);
__STATIC_INLINE void TIM_ClearINT(TIM_TypeDef* TIMx);

/**
  * @brief  Get TIMx current value when timer is running.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The counter value.
  */
__STATIC_INLINE uint32_t TIM_GetCurrentValue(TIM_TypeDef* TIMx)
{  
	/* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	return TIMx->CurrentValue;
}

/**
  * @brief  Checks whether the TIM interrupt has occurred or not.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The new state of the TIM_IT(SET or RESET).
  */
__STATIC_INLINE ITStatus TIM_GetINTStatus(TIM_TypeDef* TIMx)
{
    ITStatus bitstatus = RESET;  
    uint16_t itstatus = (uint16_t)TIMx->IntStatus;
    
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
   
    if (itstatus != (uint16_t)RESET)
    {
        bitstatus = SET;
    }
    
    return bitstatus;
}

/**
  * @brief  Cclear the TIM interrupt has occurred or not.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval none
  */
__STATIC_INLINE void TIM_ClearINT(TIM_TypeDef* TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    /* Clear the IT */
    TIMx->EOI;
}

#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_TIM_H*/

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

