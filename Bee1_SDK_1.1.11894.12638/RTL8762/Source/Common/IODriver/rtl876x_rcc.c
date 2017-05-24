/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_rcc.c
* @brief    This file provides all the IO clock firmware functions..
* @details
* @author   tifnan_ge
* @date     2015-05-16
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"

uint32_t PeriphSelectFlag = 0;  /* This should be stored in RAM ON area. */

/**
  * @brief  Enables or disables the APB peripheral clock.
  * @param  APBPeriph: specifies the APB peripheral to gates its clock.
  *      this parameter can be one of the following values:        
  *     @arg APBPeriph_TIMER
  *     @arg APBPeriph_GDMA
  *     @arg APBPeriph_SPI2W
  *     @arg APBPeriph_KEYSCAN
  *     @arg APBPeriph_QDEC
  *     @arg APBPeriph_I2C1    
  *     @arg APBPeriph_I2C0   
  *     @arg APBPeriph_IR
  *     @arg APBPeriph_SPI1
  *     @arg APBPeriph_SPI0    
  *     @arg APBPeriph_UART   
  *     @arg APBPeriph_GPIO
  *     @arg APBPeriph_ADC
  *     @arg APBPeriph_CODEC 
  * @param  APBPeriph_Clock: specifies the APB peripheral clock config.
  *      this parameter can be one of the following values(must be the same with APBPeriph):
  *     @arg APBPeriph_TIMER_CLOCK
  *     @arg APBPeriph_GDMA_CLOCK
  *     @arg APBPeriph_SPI2W_CLOCK
  *     @arg APBPeriph_KEYSCAN_CLOCK
  *     @arg APBPeriph_QDEC_CLOCK
  *     @arg APBPeriph_I2C1_CLOCK    
  *     @arg APBPeriph_I2C0_CLOCK   
  *     @arg APBPeriph_IR_CLOCK
  *     @arg APBPeriph_SPI1_CLOCK
  *     @arg APBPeriph_SPI0_CLOCK    
  *     @arg APBPeriph_UART_CLOCK   
  *     @arg APBPeriph_GPIO_CLOCK
  *     @arg APBPeriph_ADC_CLOCK
  *     @arg APBPeriph_CODEC_CLOCK
  * @param  NewState: new state of the specified peripheral clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_APB_PERIPH(APBPeriph));
    assert_param(IS_APB_PERIPH_CLOCK(APBPeriph_Clock));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    
    uint32_t apbRegOff = ((APBPeriph & (0x03 << 26)) >> 26);
    uint32_t clkRegOff = ((APBPeriph_Clock & (0x03 << 28)) >> 28);

    /* clear flag */
    APBPeriph &= (~(0x03 << 26));
    APBPeriph_Clock &= (~(0x03 << 28));

    if(NewState == ENABLE)
    {
        //enable peripheral
        *((uint32_t*)(&(SYSBLKCTRL->SOC_FUNC_EN)) + apbRegOff) |= APBPeriph;
        //enable peripheral clock
        *((uint32_t*)(&(SYSBLKCTRL->CLK_CTRL)) + clkRegOff - 1) |= APBPeriph_Clock;
		//enable peripheral clock in sleep mode
        *((uint32_t*)(&(SYSBLKCTRL->CLK_CTRL)) + clkRegOff - 1) |= (APBPeriph_Clock << 1);

        /* set PeriphSelectFlag */
        if(apbRegOff == 0x00 || apbRegOff == 0x03)
        {
            PeriphSelectFlag |= APBPeriph;
        }
        else
        {
            /* avoid conflict with other bits */
            if((APBPeriph & APBPeriph_SPI0) || (APBPeriph & APBPeriph_I2C0))
            {
                PeriphSelectFlag |= (APBPeriph >> 1);
            }
            else if(APBPeriph & APBPeriph_UART)
            {
                PeriphSelectFlag |= (APBPeriph << 1);
            }
            else
            {
                PeriphSelectFlag |= APBPeriph;
            }
        }
    }
    else
    {
        //disable peripheral
        *((uint32_t*)(&(SYSBLKCTRL->SOC_FUNC_EN)) + apbRegOff) &= (~APBPeriph);
        //disable peripheral clock
        *((uint32_t*)(&(SYSBLKCTRL->CLK_CTRL)) + clkRegOff - 1) &= (~APBPeriph_Clock);
		//disable peripheral clock in sleep mode
        *((uint32_t*)(&(SYSBLKCTRL->CLK_CTRL)) + clkRegOff - 1) &= (~(APBPeriph_Clock << 1));

        /* reset PeriphSelectFlag */
        if(apbRegOff == 0x00 || apbRegOff == 0x03)
        {
            PeriphSelectFlag &= (~APBPeriph);
        }
        else
        {
            /* avoid conflict with other bits */
            if((APBPeriph & APBPeriph_SPI0) || (APBPeriph & APBPeriph_I2C0))
            {
                PeriphSelectFlag &= (~(APBPeriph >> 1));
            }
            else if(APBPeriph & APBPeriph_UART)
            {
                PeriphSelectFlag &= (~(APBPeriph << 1));
            }
            else
            {
                PeriphSelectFlag &= (~APBPeriph);
            }
        }
    }
    
    return;
}


