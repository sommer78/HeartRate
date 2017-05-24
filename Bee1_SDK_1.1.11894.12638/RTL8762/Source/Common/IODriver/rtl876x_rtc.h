/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rtc.h
* @brief     
* @details   
* @author    Chuanguo Xue
* @date      2015-04-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _RTL876X_RTC_H_
#define _RTL876X_RTC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "legacyperiphapi.h"
	 
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup RTC RTC
  * @brief RTC driver module
  * @{
  */

/** @defgroup RTC_Exported_Types RTC Exported Types
  * @{
  */
  


/**
  * @}
  */

/** @defgroup RTC_Exported_Constants RTC Exported Constants
  * @{
  */
  
/** @defgroup RTC_interrupts_definition RTC Interrupts Definition 
  * @{
  */

#define RTC_INT_TICK                  ((uint32_t)(1 << 0))
#define RTC_INT_OVF                   ((uint32_t)(1 << 1))
#define RTC_INT_LPCMP                 ((uint32_t)(1 << 4))
#define RTC_INT_CMP0                  ((uint32_t)(1 << 16))
#define RTC_INT_CMP1                  ((uint32_t)(1 << 17))
#define RTC_INT_CMP2                  ((uint32_t)(1 << 18))
#define RTC_INT_CMP3                  ((uint32_t)(1 << 19))
#define RTC_INT_LPSRC                 ((uint32_t)(1 << 21))
#define IS_RTC_CONFIG_IT(IT) (((IT) == RTC_INT_TICK) || \
                                 ((IT) == RTC_INT_OVF) || \
                                 ((IT) == RTC_INT_LPCMP) || \
                                 ((IT) == RTC_INT_CMP0) || \
                                 ((IT) == RTC_INT_CMP1) || \
                                 ((IT) == RTC_INT_CMP2) || \
                                 ((IT) == RTC_INT_CMP3) || \
                                 ((IT) == RTC_INT_LPSRC))
/**
  * @}
  */

/** @defgroup TIM_Clock_Source TIM Clock Source
  * @{
  */

#define RTC_CLOCK_EXTERNAL_32768           ((uint32_t)(0x00))
#define RTC_CLOCK_INTERNAL_32000           ((uint32_t)(1 << 31))
#define IS_RTC_CLOCK_SRC(HZ) (((HZ) == RTC_CLOCK_INTERNAL_32000) || \
                             ((HZ) == RTC_CLOCK_INTERNAL_32000))

/**
  * @}
  */

/**
  * @}
  */ 

/** @defgroup RTC_Exported_Functions RTC Exported Functions
  * @{
  */

extern void RTC_Reset(void);
void RTC_ResetForInternal32K(void);
extern void RTC_SetCOMPValue(uint8_t COMPNum, uint32_t COMPValue);
extern uint32_t RTC_GetCOMPValue(uint8_t COMPNum);
extern void RTC_INTConfig(RTC_TypeDef* RTCx, uint32_t RTC_IT, FunctionalState NewState);
__STATIC_INLINE void RTC_SetCLKSource(uint32_t clockSrc);
__STATIC_INLINE void RTC_Enable(void);
__STATIC_INLINE void RTC_Start(void);
__STATIC_INLINE void RTC_Stop(void);
__STATIC_INLINE void RTC_SetPrescaler(uint32_t PrescaleValue);
__STATIC_INLINE void RTC_EnableCOMP(uint8_t COMPNum);
__STATIC_INLINE void RTC_DisableCOMP(uint8_t COMPNum);
__STATIC_INLINE void RTC_ClearCOMPEvent(uint8_t COMPNum);
__STATIC_INLINE void RTC_ClearAllCOMPEvent(void);
__STATIC_INLINE uint32_t RTC_GetCounter(void);
__STATIC_INLINE uint32_t RTC_GetEventStatus(void);
__STATIC_INLINE void RTC_EnableOverflow(void);
__STATIC_INLINE void RTC_DisableOverflow(void);
__STATIC_INLINE void RTC_ClearOverflowEvent(void);
__STATIC_INLINE void RTC_EnableTick(void);
__STATIC_INLINE void RTC_DisableTick(void);
__STATIC_INLINE void RTC_ClearTickEvent(void);
__STATIC_INLINE void RTC_EnableSystemWakeup(void);
__STATIC_INLINE void RTC_StartLpCounter(void);
__STATIC_INLINE uint16_t RTC_GetLpCounterVlaue(void);
__STATIC_INLINE void RTC_SetLpCmpVal(uint16_t cmpVal);
void RTC_SetLpBatteryDetectionVal(uint8_t Val);


/**
  * @brief  Select RTC clock source.
  * @param  clockSrc: specifies the RTC clock source which can be internal or external clock. 
  *   This parameter can be the following values:
  *     @arg RTC_CLOCK_EXTERNAL_32768: select external 32.768 KHz clock source.
  *     @arg RTC_CLOCK_INTERNAL_32000: select internal 30 KHz clock source
  * @retval None
  */
__STATIC_INLINE void RTC_SetCLKSource(uint32_t clockSrc)
{
	/* Check the parameters */
    assert_param(IS_RTC_CLOCK_SRC(clockSrc));
	
	RTC->CR0 &= ~(BIT(31));
	RTC->CR0 |= clockSrc;
}

/**
  * @brief  Enable RTC function.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void RTC_Enable(void)
{
    *((uint32_t *)0x40000014) |= BIT(9);
    bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)&(~BIT(4)));
    RTC->SR_OFF |= BIT(31);
}

/**
  * @brief  Start RTC.
  * @param  None.
  * @retval None
  */
__STATIC_INLINE void RTC_Start(void)
{
    RTC->CR0 |= 0x01; 
}

/**
  * @brief  Stop RTC.
  * @param  None.
  * @retval None
  */
__STATIC_INLINE void RTC_Stop(void)
{
    RTC->CR0 &= ~0x01;
}

/**
  * @brief  Set RTC prescaler value.
  * @param  PrescaleValuee: the prescaler value to be set.
  * @retval None
  */
__STATIC_INLINE void RTC_SetPrescaler(uint32_t PrescaleValue)
{
    RTC->RTC_PRESCALER = PrescaleValue;
}

/**
  * @brief  Enable one comparator of RTC.
  * @param  COMPNum: the comparator number.
  * @retval None.
  */
__STATIC_INLINE void RTC_EnableCOMP(uint8_t COMPNum)
{
    RTC->CR0 |= BIT(16 + COMPNum);
    RTC->CR1 |= BIT(16 + COMPNum);
}

/**
  * @brief  Disable one comparator of RTC.
  * @param  COMPNum: the comparator number.
  * @retval None
  */
__STATIC_INLINE void RTC_DisableCOMP(uint8_t COMPNum)
{
    RTC->CR0 &= ~BIT(16 + COMPNum);
    RTC->CR1 &= ~BIT(16 + COMPNum); 
}

/**
  * @brief  Clear event of one comparator of RTC.
  * @param  COMPNum: the comparator number.
  * @retval None
  */
void RTC_ClearCOMPEvent(uint8_t COMPNum)
{
    RTC->SR_OFF = BIT(31)|BIT(16 + COMPNum);
    RTC->SR = BIT(16 + COMPNum);
}

/**
  * @brief  Clear all events of one all comparators of RTC.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void RTC_ClearAllCOMPEvent(void)
{
    RTC->SR |= 0xF<<16;
    RTC->SR_OFF |= 0xF<<16;
}

/**
  * @brief  Get counter value of RTC.
  * @param  None
  * @retval the counter value.
  */
__STATIC_INLINE uint32_t RTC_GetCounter(void)
{
    return RTC->CNT;
}

/**
  * @brief  Get the event status of RTC.
  * @param  None
  * @retval the event status.
  */
__STATIC_INLINE uint32_t RTC_GetEventStatus(void)
{
    return RTC->SR_OFF;
}

/**
  * @brief  Enable overflow event of RTC.
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_EnableOverflow(void)
{
    RTC->CR0 |= BIT(7);
    RTC->CR1 |= BIT(1);
}

/**
  * @brief  Disable overflow event of RTC.
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_DisableOverflow(void)
{
    RTC->CR0 &= ~BIT(7);
    RTC->CR1 &= ~BIT(1);    
}

/**
  * @brief  Clear overflow event of RTC.
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_ClearOverflowEvent(void)
{
    RTC->SR_OFF = BIT(31)|BIT(1);
    RTC->SR = BIT(1);
}

/**
  * @brief  Enable tick function of RTC.
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_EnableTick(void)
{
    RTC->CR0 |= BIT(30);
    RTC->CR1 |= BIT(0);
}

/**
  * @brief  Disable tick function of RTC.
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_DisableTick(void)
{
    RTC->CR0 &= ~BIT(30);
    RTC->CR1 &= ~BIT(0);    
}

/**
  * @brief  Clear tick event of RTC.
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_ClearTickEvent(void)
{
    RTC->SR_OFF = BIT(31)|BIT(0);
    RTC->SR = BIT(0);
}

/**
  * @brief  Enable RTC wakeup function when enter DLPS
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_EnableSystemWakeup(void)
{
    RTC->CR1 |= BIT(20);
}

/**
  * @brief  start LPCOMP counter
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_StartLpCounter(void)
{
    UINT8 aon_reg_value = 0x0;
    RTC->CR0 |= (1 << 6);
    aon_reg_value = bee_aon_gp_read(0x9e);
    aon_reg_value |= BIT(0);
    bee_aon_gp_write(0x8E, aon_reg_value);
    
    /* Enable signal detector from BTAON */
    //*((volatile uint32_t*)(0x40000000 + 0x9e)) |= (1 << 0);
}

/**
  * @brief  get LPCOMP counter value
  * @param  None
  * @retval None.
  */
__STATIC_INLINE uint16_t RTC_GetLpCounterVlaue(void)
{
    return RTC->LPCOMP_CNT;
}

/**
  * @brief  set LPCOMP compare value
  * @param  None
  * @retval None.
  */
__STATIC_INLINE void RTC_SetLpCmpVal(uint16_t cmpVal)
{
    RTC->LPCOMP_CMP = cmpVal;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_RTC_H_ */

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
