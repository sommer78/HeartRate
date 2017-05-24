/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_rtc.c
* @brief    This file provides all the RTC firmware functions.
* @details
* @author   Chuanguo Xue
* @date     2015-04-27
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl_endian.h"
#include "legacyperiphapi.h"
#include "rtl876x_rtc.h"

/**
  * @brief  Reset RTC and initialize RTC which select external 32768 as clock source.
  * @param  None.
  * @retval None
  */
void RTC_Reset(void)
{
	/* Set RTC_BTAON_POWERCUT_BIT if use RTC */
    *((uint32_t *)0x40000014) |= BIT(9);
	/* Release isolatle core power to RTC module */
    bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)&(~BIT(4)));
	/* Enable off-domain RTC interrupt control */
    RTC->SR_OFF |= BIT(31);

	/* Default using external 32kHz LFOSC*/
    RTC->CR0 = 0x9C;
	/* Disable all type of interrupts and RTC wake up function */
    RTC->CR1 = 0x00;
	/* Clear all type of flags */
    RTC->SR = 0xFFFFFFFF;
    RTC->RTC_PRESCALER = 0x00;
    RTC->COMP0 = 0x00;
    RTC->COMP1 = 0x00;
    RTC->COMP2 = 0x00;
    RTC->COMP3 = 0x00;
    RTC->LPCOMP_CMP = 0x00;

    RTC->SR_OFF = 0xFFFFFFFF;
    RTC->SR_OFF &= ~(BIT(31));

    *((uint32_t *)0x40000014) &= ~(BIT(9));
	/* Isolatle core power to RTC module */
    bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)|BIT(4));
}

/**
  * @brief  Reset RTC and initialize RTC which select internal 32K as clock source.
  * @param  None.
  * @retval None
  */
void RTC_ResetForInternal32K(void)
{
    *((uint32_t *)0x40000014) |= BIT(9);
	/* Power on 32K DSM power domain and disable ISO */
    bee_aon_gp_write(0x1C, bee_aon_gp_read(0x1C)|(BIT(8)));
	/* Power on RTC power domain and disable ISO */
    bee_aon_gp_write(0x1C, bee_aon_gp_read(0x1C)|(BIT(9)));
    /* Release isolatle core power to RTC module */
    bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)&(~BIT(4)));
    /* Release isolate core power to DSM module */
	bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)&(~BIT(3)));
    RTC->SR_OFF |= BIT(31);
    
    RTC->CR0 = 0x1C;
    /* Enable internal 32K */
	RTC->CR0 |= BIT(31);            
	
    RTC->CR1 = 0x00;
    RTC->SR = 0xFFFFFFFF;
    RTC->RTC_PRESCALER = 0x00;
    RTC->COMP0 = 0x00;
    RTC->COMP1 = 0x00;
    RTC->COMP2 = 0x00;
    RTC->COMP3 = 0x00;
    RTC->LPCOMP_CMP = 0x00;

    RTC->SR_OFF = 0xFFFFFFFF;
    RTC->SR_OFF &= ~BIT(31);
}

/**
  * @brief  Set RTC comparator value.
  * @param  COMPNum: the comparator number.
  * @param  COMPValue: the comparator value to be set.
  * @retval None
  */
void RTC_SetCOMPValue(uint8_t COMPNum, uint32_t COMPValue)
{
    switch(COMPNum)
    {
        case 0:
            RTC->COMP0 = COMPValue;
            break;
        case 1:
            RTC->COMP1 = COMPValue;
            break;
        case 2:
            RTC->COMP2 = COMPValue;
            break;
        case 3:
            RTC->COMP3 = COMPValue;
            break;
        default:
            break;
    }
}

/**
  * @brief  Get RTC comparator value.
  * @param  COMPNum: the comparator number.
  * @retval the comparator value.
  */
uint32_t RTC_GetCOMPValue(uint8_t COMPNum)
{
    uint32_t COMPValue;
    switch(COMPNum)
    {
        case 0:
            COMPValue = RTC->COMP0;
            break;
        case 1:
            COMPValue = RTC->COMP1;
            break;
        case 2:
            COMPValue = RTC->COMP2;
            break;
        case 3:
            COMPValue = RTC->COMP3;
            break;
        default:
            break;
    }
    return COMPValue;
}

/**
  * @brief  Enables or disables the specified RTC interrupts.
  * @param  RTCx: RTC peripheral
  * @param  RTC_IT: specifies the RTC interrupt source to be enabled or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg RTC_INT_TICK: RTC tick interrupt
  *     @arg RTC_INT_OVF: RT counter overflow interrupt
  *     @arg RTC_INT_LPCMP: LPCOMP comapre interrupt
  *     @arg RTC_INT_CMP0: compare 0 interrupt
  *     @arg RTC_INT_CMP1: compare 1 interrupt
  *     @arg RTC_INT_CMP2: compare 2 interrupt
  *     @arg RTC_INT_CMP3: compare 3 interrupt
  *     @arg RTC_INT_LPSRC: LPCOMP source interrupt(used in voltage compare)
  * @param  NewState: new state of the specified RTC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_INTConfig(RTC_TypeDef* RTCx, uint32_t RTC_IT, FunctionalState NewState)
{
    if (NewState ==ENABLE)
    {
        /* Enable the selected RTC interrupt */
        RTCx->CR1 |= RTC_IT;
    }
    else
    {
        /* Disable the selected RTC interrupt */
        RTCx->CR1 &= (~(RTC_IT));
    }
}

/**
  * @brief  Set RTC lpcomp detect voltage value.
                Always enter AnalogComparatorIntrHandler interrupt if connect P4_3 with voltage which higher than threshold voltage.
                you can mask interrupt by calling code (RTC->CR1 &= ~BIT(21);)
                if voltage of P4_3 is lower than threshold voltage, it will stop enter interrupt immediately no matter mask interrupt or not
  * @param  cmpVal: trigger voltage value = (cmpVal*0.05 + 0.05) * 4(v).
  * @param  COMPValue: the comparator value to be set.
  * @retval None
  */
void RTC_SetLpBatteryDetectionVal(uint8_t Val)
{
    UINT8 aon_reg_value = 0x0;
    /*---------------set threshold voltage of low power comparator(user can set threshold voltage here)-----------*/
    aon_reg_value = bee_aon_gp_read(0x8E);
    /* threshold voltage = (0x10*0.05 + 0.05)*4V = 0.6V */
    aon_reg_value &= ~(0X0F);
    aon_reg_value |= Val;
    bee_aon_gp_write(0x8E, aon_reg_value);

}


