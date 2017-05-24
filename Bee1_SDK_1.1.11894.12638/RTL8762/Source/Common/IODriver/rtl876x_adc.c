/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_adc.c
* @brief    This file provides all the ADC firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-05-05
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_adc.h"
#include "string.h"
#include "tsmc_eflash.h"

/**
  * @brief Initializes the ADC peripheral according to the specified
  *   parameters in the ADC_InitStruct
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_InitStruct: pointer to a ADCInitTypeDef structure that
  *   contains the configuration information for the specified ADC peripheral
  * @retval None
  */
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct)
{
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_MODE(ADC_InitStruct->adcMode));
    assert_param(IS_ADC_CHANNEL_MAP(ADC_InitStruct->channelMap));
    assert_param(IS_ADC_REF_VOL_CONFIG(ADC_InitStruct->refVolCfg));
    assert_param(IS_ADC_FIFO_THRESHOLD(ADC_InitStruct->adcFifoThd));
    assert_param(IS_ADC_BURST_SIZE_CONFIG(ADC_InitStruct->adcBurstSize));
    assert_param(IS_ADC_AMIC_CONFIG(ADC_InitStruct->amicEn));
    assert_param(IS_ANALOG_GAIN(ADC_InitStruct->analogGain));
    assert_param(IS_ANALOG_BSTOP_CURRENT_CONFIG(ADC_InitStruct->analogBstopCur));
    assert_param(IS_ANALOG_BIAS_CURRENT_CONFIG(ADC_InitStruct->analogBisaCur));
    assert_param(IS_ANALOG_DIFF_MODE_CONFIG(ADC_InitStruct->diffModeEn));

    /* normal ADCx mode */
    if ((ADC_InitStruct->adcMode == ADC_One_Shot_Mode)\
            || ADC_InitStruct->adcMode == ADC_Auto_Sensor_Mode)
    {
        ADCx->CR |= ((ADC_InitStruct->adcFifoThd << 16) | (1 << 28));

        //enable channel, enable calibration VCC/GND switch, config reference voltage
        ADCx->ANP0 &= (~((1 << 0) | (1 << 8) | (0x3FF << 19)));
        ADCx->ANP0 |= ((ADC_InitStruct->channelMap << 19) | (1 << 28) | (1 << 29) | ADC_InitStruct->refVolCfg);

        //dsr<2:0>
        ADCx->ANP1 |= (0x0 << 6);

        ADCx->ANP6 |= (1 << 12);

        //clear IT status
        ADCx->INTCR |= (0xFF << 16);
    }

    /* audio mode */
    if (ADC_InitStruct->adcMode == ADC_Audio_Mode)
    {
        /* set fifo threshold and burst size, clear fifo */
        ADCx->CR &= (~((0x3F << 8) | (0x3F << 16)));
        ADCx->CR |= (ADC_InitStruct->adcFifoThd << 16)\
                    | (ADC_InitStruct->adcBurstSize << 8)\
                    | (1 << 28)\
                    | (1 << 0);

        /* power settings */
        if (ADC_InitStruct->amicEn)
        {
            ADCx->PWR |= (((uint32_t)1 << 31) | (ADC_InitStruct->amicEn));

            /* set anapar ad0, ADC manual enable ,audio enable, disable diff */
            ADCx->ANP0 &= (~(1 << 4));
            ADCx->ANP0 |= ((1 << 0) | (1 << 3) | (1 << 5));
            /* feedback capacitor value: 1x */
            ADCx->ANP0 &= (~(1 << 6));

            /* enable manual mode */
            ADCx->ANP1 |= (1 << 0);

            /* set anapar ad6, set gain/bstop current/bias current, enable PGA/PGA2, disable Diffential mode, ADC clock from PLL */
            ADCx->ANP6 &= (~0xFF0);
            ADCx->ANP6 |= (ADC_InitStruct->analogGain\
                           | ADC_InitStruct->analogBstopCur\
                           | ADC_InitStruct->analogBisaCur\
                           | ADC_InitStruct->diffModeEn\
                           | ((1 << 0) | (1 << 1) | (1 << 12)));

            /* set anapar ad5 */
            ADCx->ANP5 |= (1 << 9);
        }
        else
        {
#ifdef DMIC_USE_MIC_BIAS
            //workaround
            ADCx->PWR &= (~(1 << 28));
            ADCx->PWR |= ((uint32_t)1 << 31);

            //ADCx->LDO |= ((1 << 4) | (1 << 16) | (1 << 20) | (1 << 24) | (1 << 5));
            ADCx->PWR |= ((1 << 1) | (1 << 2) | (1 << 3) | (1 << 4));
            ADCx->ANP0 &= (~((1 << 0) | (1 << 18)));
            ADCx->CR |= (1 << 0);
            while (!(ADCx->PWR & (1 << 10)));
            //enable power manual mode
            ADCx->PWR |= (1 << 0);
            ADCx->ANP0 |= ((1 << 3));
#else
            ADCx->PWR &= (~(1 << 28));
            ADCx->ANP0 |= (1 << 3);
#endif

        }
    }

    return;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
    ADC_InitStruct->adcMode             = ADC_One_Shot_Mode;

    /* just for ADC */
    ADC_InitStruct->channelMap          = ADC_CH0;
    ADC_InitStruct->refVolCfg           = ADC_IN_Ref_Vol_En;

    /* just for audio */
    ADC_InitStruct->adcBurstSize        = 0x10;
    ADC_InitStruct->adcFifoThd          = 0x20;
    ADC_InitStruct->amicEn              = ADC_AMIC_Disable;
    ADC_InitStruct->diffModeEn          = ANA_DIFF_MODE_DISABLE;
    ADC_InitStruct->analogGain          = ANA_GAIN_40dB;
    ADC_InitStruct->analogBstopCur      = ANA_BSTOP_CUR_4U;
    ADC_InitStruct->analogBisaCur       = ANA_BIAS_CUR_16U;

    return;
}

/**
  * @brief  Deinitializes the ADC peripheral registers to their default reset values(turn off ADC clock).
  * @param  ADCx: selected ADC peripheral.
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef *ADCx)
{
    assert_param(IS_ADC_PERIPH(ADCx));

    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, DISABLE);

    return;
}

/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_INT_CMP_CH0: ADC ch0 interrupt
  *     @arg ADC_INT_CMP_CH1: ADC ch1 interrupt
  *     @arg ADC_INT_CMP_CH2: ADC ch2 interrupt
  *     @arg ADC_INT_CMP_CH3: ADC ch3 interrupt
  *     @arg ADC_INT_CMP_CH4: ADC ch4 interrupt
  *     @arg ADC_INT_CMP_CH5: ADC ch5 interrupt
  *     @arg ADC_INT_CMP_CH6: ADC ch6 interrupt
  *     @arg ADC_INT_CMP_CH7: ADC ch7 interrupt
  *     @arg ADC_INT_CMP_BAT: ADC battery interrupt
  *     @arg ADC_INT_CMP_TEMP: ADC temperature interrupt
  *     @arg ADC_INT_ONE_SHOT_DONE: ADCx one convert end interrupt
  *     @arg ADC_INT_FIFO_FULL: ADCx FIFO full interrupt
  *     @arg ADC_INT_FIFO_RD_REQ: ADC read FIFO request interrupt
  *     @arg ADC_INT_FIFO_RD_ERR: ADC read FIFO error interrupt
  *     @arg ADC_INT_FIFO_TH: ADC fifo threshold reached interrupt
  *     @arg ADC_INT_CODEC: ADC codec interrupt
  * @param  NewState: new state of the specified ADC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_INTConfig(ADC_TypeDef *ADCx, uint32_t ADC_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_ADC_IT(ADC_IT));

    if (newState == ENABLE)
    {
        /* Enable the selected ADC interrupts */
        ADCx->INTCR |= ADC_IT;
    }
    else
    {
        /* Disable the selected ADC interrupts */
        ADCx->INTCR &= (uint32_t)~ADC_IT;
    }
}

/**
  * @brief  read ADC data according to specific channel.
  * @param  ADCx: selected ADC peripheral.
  * @param  channel: ADC channel to read.
  *   This parameter can be one of the following values:
  *     @arg ADC_CH0: ADC channel 0
  *     @arg ADC_CH1: ADC channel 1
  *     @arg ADC_CH2: ADC channel 2
  *     @arg ADC_CH3: ADC channel 3
  *     @arg ADC_CH4: ADC channel 4
  *     @arg ADC_CH5: ADC channel 5
  *     @arg ADC_CH6: ADC channel 6
  *     @arg ADC_CH7: ADC channel 6
  *     @arg ADC_CH_BAT: battery channel
  *     @arg ADC_CH_TMP: temperature channel
  * @retval The 12-bit converted ADC data.
  */
uint16_t ADC_Read(ADC_TypeDef *ADCx, uint16_t ADC_Channel)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_CHANNEL(ADC_Channel));

    uint8_t chIndex = 0;

    for (chIndex = 0; chIndex < 8; chIndex++)
    {
        if ( (1 << chIndex) & ADC_Channel)
        {
            break;
        }
    }

    if (ADC_Channel == ADC_CH_BAT)
    {
        return ADCx->BATD;
    }
    else if (ADC_Channel == ADC_CH_TMP)
    {
        return ADCx->TMPD;
    }
    else
    {
        return (*(uint32_t *)((uint32_t *)(&ADCx->CH0D) + chIndex));
    }

}

/**
  * @brief  Set ADC compare threshould.
  * @param  ADCx: selected ADC peripheral.
  * @param  channel: ADC channel to set compare threshold.
  *   This parameter can be one of the following values:
  *     @arg ADC_CH0: ADC channel 0
  *     @arg ADC_CH1: ADC channel 1
  *     @arg ADC_CH2: ADC channel 2
  *     @arg ADC_CH3: ADC channel 3
  *     @arg ADC_CH4: ADC channel 4
  *     @arg ADC_CH5: ADC channel 5
  *     @arg ADC_CH6: ADC channel 6
  *     @arg ADC_CH7: ADC channel 7
  *     @arg ADC_CH_BAT: battery channel
  *     @arg ADC_CH_TMP: temperature channel
  * @param  cmpThd: compare threshold value.
  * @retval None.
  */
void ADC_SetCmpThd(ADC_TypeDef *ADCx, uint16_t ADC_Channel, uint16_t cmpThd)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_CHANNEL(ADC_Channel));

    uint8_t chIndex = 0;

    for (chIndex = 0; chIndex <= 9; chIndex++)
    {
        if ( (1 << chIndex) & ADC_Channel)
        {
            break;
        }
    }

    if (chIndex <= 3)
    {
        *(uint32_t *)((uint32_t *)(&ADCx->CPT01) + (uint8_t)(chIndex / 2)) |= ((uint32_t)cmpThd << ((chIndex % 2) * 16));
    }
    else if (4 <= chIndex && chIndex <= 7)
    {
        *(uint32_t *)((uint32_t *)(&ADCx->CPT45) + (uint8_t)((chIndex - 4) / 2)) |= ((uint32_t)cmpThd << ((chIndex % 2) * 16));
    }
    else
    {
        *(uint32_t *)((uint32_t *)(&ADCx->CPTTB) + (uint8_t)((chIndex - 8) / 2)) |= ((uint32_t)cmpThd << ((chIndex % 2) * 16));
    }

    return;
}

/**
  * @brief  Config ADC compare rule.
  * @param  ADCx: selected ADC peripheral.
  * @param  channel: ADC channel to set compare rule.
  *   This parameter can be one of the following values:
  *     @arg ADC_CH0: ADC channel 0
  *     @arg ADC_CH1: ADC channel 1
  *     @arg ADC_CH2: ADC channel 2
  *     @arg ADC_CH3: ADC channel 3
  *     @arg ADC_CH4: ADC channel 4
  *     @arg ADC_CH5: ADC channel 5
  *     @arg ADC_CH6: ADC channel 6
  *     @arg ADC_CH7: ADC channel 7
  *     @arg ADC_CH_BAT: battery channel
  *     @arg ADC_CH_TMP: temperature channel
  *
  * @param  ADC_CMP_CFG: compare config.
  *   This parameter can be one of the following values:
  *     @arg ADC_CMP_CFG_LARGER: if ADC data is larger than threshold, trigger interrupt.
  *     @arg ADC_CMP_CFG_SMALLER: if ADC data is smaller than threshold, trigger interrupt.
  *
  * @param  ADC_FIFO_CFG: when use compare, fifo on or not.
  *   This parameter can be one of the following values:
  *     @arg ADC_Cmp_Fifo_Enable: Fifo on when use comapre fucntion.
  *     @arg ADC_Cmp_Fifo_Disable: Fifo off when use comapre fucntion.
  * @retval None.
  */
void ADC_SetCmpConfig(ADC_TypeDef *ADCx, uint16_t ADC_Channel, uint16_t ADC_CMP_CFG, uint16_t ADC_FIFO_CFG)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_CHANNEL(ADC_Channel));
    assert_param(IS_ADC_CMP_CONFIG(ADC_CMP_CFG));
    assert_param(IS_ADC_CMP_FIFO_CONFIG(ADC_FIFO_CFG));


    if (ADC_CMP_CFG == ADC_CMP_CFG_LARGER)
    {
        ADCx->CPCFG |= ADC_Channel;
    }
    else
    {
        ADCx->CPCFG &= ~ADC_Channel;
    }

    /* config fifo */
    ADCx->CR &= (~(1 << 2));
    ADCx->CR |= ADC_FIFO_CFG;

    return;
}

/**
  * @brief  Enables or disables the ADC peripheral.
  * @param  ADCx: selected ADC peripheral.
  * @param  adcMode: adc mode select.
        This parameter can be one of the following values:
  *     @arg ADC_One_Shot_Mode: one shot mode.
  *     @arg ADC_Auto_Sensor_Mode: compare mode.
  * @param  NewState: new state of the ADC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef *ADCx, uint8_t adcMode, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_ADC_MODE(adcMode));

    if (adcMode == ADC_One_Shot_Mode)
    {
        if (NewState == ENABLE)
        {
            /* enable oneshot mode power, auto power mode */
#ifdef ADC_POWER_AUTO
            ADCx->ANP0 |= (1 << 0);
            ADCx->CR |= (1 << 1);
            ADCx->PWR |= ((1 << 26) | ((uint32_t)1 << 31));
#else
            ADCx->PWR |= ((1 << 26) | ((uint32_t)1 << 31));
            ADCx->PWR |= BIT0;
            ADC_delayUS(10);
            ADCx->PWR |= BIT1 | BIT2 | BIT3;
            ADC_delayUS(10);
            ADCx->LDO |= BIT16;
            ADCx->LDO |= (0x2 << 21); // delay 1.3ms;//([22:21] = 10)
            ADCx->LDO &= ~BIT20;  // enable VREF fast settling
            ADC_delayUS(10);
            ADCx->LDO |= BIT24;
            ADC_delayUS(10);

            ADCx->ANP0 |= BIT18;
            ADC_delayUS(1200);
            ADCx->ANP0 |= BIT0;
            ADCx->CR |= BIT1;
#endif
        }
        else
        {
#ifdef ADC_POWER_AUTO
            ADCx->ANP0 &= (~(1 << 0));
            ADCx->CR &= (~(1 << 1));
            ADCx->PWR &= (~((1 << 26) | ((uint32_t)1 << 31)));
#else
            ADC_delayUS(10);
            ADCx->PWR &= ~(BIT1 | BIT2 | BIT3);
            ADC_delayUS(10);
            ADCx->LDO &= ~BIT16;
            ADCx->LDO |= (0x2 << 21);
            ADCx->LDO |= BIT20;
            //ADC_delayUS(1000);
            ADCx->LDO &= ~BIT24;
            //ADC_delayUS(5000);

            ADCx->ANP0 &= ~BIT18;
            ADC_delayUS(1);
            ADCx->ANP0 &= ~BIT0;
            ADCx->CR &= ~BIT1;
            ADCx->PWR &= (~((1 << 26) | ((uint32_t)1 << 31)));
#endif
        }
    }

    if (adcMode == ADC_Auto_Sensor_Mode)
    {
        if (NewState == ENABLE)
        {
            ADCx->ANP0 |= (1 << 0);
            //Enable FIFO overwrite mode when FIFO is full, enable auto sensor mode
            ADCx->CR |= (1 << 0) | (1 << 3);
            ADCx->PWR |= ((uint32_t)1 << 31);
        }
        else
        {
            ADCx->ANP0 &= (~(1 << 0));
            ADCx->CR &= (~((1 << 0) | (1 << 3)));
            ADCx->PWR &= (~((uint32_t)1 << 31));
        }
    }

    return;
}





