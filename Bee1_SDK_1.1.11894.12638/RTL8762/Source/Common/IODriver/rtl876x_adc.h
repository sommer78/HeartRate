/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_adc.h
* @brief     header file of ADC driver.
* @details   
* @author    tifnan_ge
* @date      2015-05-04
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_ADC_H_
#define _RTL876X_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup ADC ADC
  * @breif ADC driver module
  * @{
  */

/** @defgroup ADC_Exported_Types ADC Exported Types
  * @{
  */

/**
 * @brief ADC initialize parameters
 *
 * ADC initialize parameters
 */
typedef struct
{
    uint16_t adcMode;
    uint16_t channelMap;    //use which channel
    uint16_t refVolCfg;   //use external reference voltage or internal reference voltage
    
    /* for audio, if not use, ignore parameters following */
    uint16_t adcBurstSize;  //used with DMA, level to trrigger DMA
    uint16_t adcFifoThd;    //fifo threshold
    uint32_t amicEn;        //if use dmic, disable it
    uint16_t diffModeEn;    //difference mode
    uint16_t analogGain;
    uint16_t analogBstopCur;
    uint16_t analogBisaCur;
}ADC_InitTypeDef, *PADC_InitTypeDef;

/**
  * @}
  */

/** @defgroup ADC_Exported_Constants ADC Exported Constants
  * @{
  */

#define IS_ADC_PERIPH(PERIPH) ((PERIPH) == ADC)

/** @defgroup ADC_Interrupts_Definition ADC Interrupts Definition 
  * @{
  */

#define ADC_INT_CMP_CH0                             ((uint32_t)(1 << 0))
#define ADC_INT_CMP_CH1                             ((uint32_t)(1 << 1))
#define ADC_INT_CMP_CH2                             ((uint32_t)(1 << 2))
#define ADC_INT_CMP_CH3                             ((uint32_t)(1 << 3))
#define ADC_INT_CMP_CH4                             ((uint32_t)(1 << 4))
#define ADC_INT_CMP_CH5                             ((uint32_t)(1 << 5))
#define ADC_INT_CMP_CH6                             ((uint32_t)(1 << 6))
#define ADC_INT_CMP_CH7                             ((uint32_t)(1 << 7))
#define ADC_INT_CMP_BAT                             ((uint32_t)(1 << 8))
#define ADC_INT_CMP_TEMP                            ((uint32_t)(1 << 9))
#define ADC_INT_ONE_SHOT_DONE                       ((uint32_t)(1 << 10))
#define ADC_INT_FIFO_FULL                           ((uint32_t)(1 << 11))
#define ADC_INT_FIFO_RD_REQ                         ((uint32_t)(1 << 12))
#define ADC_INT_FIFO_RD_ERR                         ((uint32_t)(1 << 13))
#define ADC_INT_FIFO_TH                             ((uint32_t)(1 << 14))
#define ADC_INT_CODEC                               ((uint32_t)(1 << 15))

#define IS_ADC_IT(IT) ((((IT) & 0xFFFF0000) == 0x00) && ((IT) != 0x00))

#define IS_ADC_GET_IT(IT) (((IT) == ADC_INT_CMP_CH0) || ((IT) == ADC_INT_CMP_CH1)\
                            || ((IT) == ADC_INT_CMP_CH2) || ((IT) == ADC_INT_CMP_CH3)\
                            || ((IT) == ADC_INT_CMP_CH4) || ((IT) == ADC_INT_CMP_CH5)\
                            || ((IT) == ADC_INT_CMP_CH6) || ((IT) == ADC_INT_CMP_CH7)\
                            || ((IT) == ADC_INT_CMP_BAT) || ((IT) == ADC_INT_CMP_TEMP)\
                            || ((IT) == ADC_INT_ONE_SHOT_DONE) || ((IT) == ADC_INT_FIFO_FULL)\
                            || ((IT) == ADC_INT_FIFO_RD_REQ) || ((IT) == ADC_INT_FIFO_RD_ERR)\
                            || ((IT) == ADC_INT_FIFO_TH) || ((IT) == ADC_INT_CODEC))
/**
  * @}
  */

/** @defgroup ADC_Flag ADC Flag
  * @{
  */

#define ADC_FLAG_ADC_HW_EN                          ((uint32_t)(1 << 10))
#define ADC_FLAG_ADC_FIFO_EN                        ((uint32_t)(1 << 11))

#define IS_ADC_GET_FLAG(FLAG) (((FLAG) == ADC_FLAG_ADC_HW_EN) || ((FLAG) == ADC_FLAG_ADC_FIFO_EN))

/**
  * @}
  */
	
/** @defgroup Temperature result type
  * @{
  */

#define CELSIUS                          						0
#define KELVIN                           						1

/**
  * @}
  */

/** @defgroup Current Vbat value
  * @{
  */

#define VBAT_28V_33V                          1
#define VBAT_20V_28V                          0

/**
  * @}
  */	


/** @defgroup ADC_Mode ADC Mode
  * @{
  */
  
#define ADC_Auto_Sensor_Mode                        ((uint16_t)(1 << 0))
#define ADC_One_Shot_Mode                           ((uint16_t)(1 << 1))
#define ADC_Audio_Mode                              ((uint16_t)(1 << 2))    //audio mode

#define IS_ADC_MODE(MODE) (((MODE) == ADC_Auto_Sensor_Mode) || ((MODE) == ADC_One_Shot_Mode))
/**
  * @}
  */

/** @defgroup ADC_Compare_Fifo_Config ADC Compare Fifo Config
  * @brief if use compare function, FIFO on or not.
  * @{
  */
  
#define ADC_Cmp_Fifo_Enable                          ((uint16_t)(0 << 2))
#define ADC_Cmp_Fifo_Disable                         ((uint16_t)(1 << 2))
#define IS_ADC_CMP_FIFO_CONFIG(CONFIG) (((CONFIG) == ADC_Cmp_Fifo_Enable) || ((CONFIG) == ADC_Cmp_Fifo_Disable))

/**
  * @}
  */

/** @defgroup ADC_Fifo_Endian ADC Fifo Endian
  * @brief only used in audio mode, PCM format data output.
  * @{
  */
  
#define ADC_FIFO_LITTLE_ENDIAN                       ((uint16_t)(1 << 4))
#define ADC_FIFO_BIG_ENDIAN                          ((uint16_t)(0 << 4))
#define IS_ADC_FIFO_ENDIAN(ENDIAN) (((ENDIAN) == ADC_FIFO_LITTLE_ENDIAN) || ((ENDIAN) == ADC_FIFO_BIG_ENDIAN))

/**
  * @}
  */

/** @defgroup ADC_Reference_Voltage_Config ADC Reference Voltage Config
  * @{
  */
  
#define ADC_Ext_Ref_Vol_En                          ((uint16_t)(1 << 8))     //external reference voltage
#define ADC_IN_Ref_Vol_En                           ((uint16_t)(0 << 8))     //internal reference voltage
#define IS_ADC_REF_VOL_CONFIG(CONFIG) (((CONFIG) == ADC_Ext_Ref_Vol_En) || ((CONFIG) == ADC_IN_Ref_Vol_En))

/**
  * @}
  */

/** @defgroup ADC_Compare_Config ADC Compare Config
  * @{
  */
  
#define ADC_CMP_CFG_LARGER                         ((uint16_t)(0 << 0))
#define ADC_CMP_CFG_SMALLER                        ((uint16_t)(1 << 0))
#define IS_ADC_CMP_CONFIG(CONFIG) (((CONFIG) == ADC_CMP_CFG_LARGER) || ((CONFIG) == ADC_CMP_CFG_SMALLER))

/**
  * @}
  */

/** @defgroup ADC_Channel ADC Channel
  * @{
  */
  
#define ADC_CH0                                     ((uint16_t)(1 << 0))
#define ADC_CH1                                     ((uint16_t)(1 << 1))
#define ADC_CH2                                     ((uint16_t)(1 << 2))
#define ADC_CH3                                     ((uint16_t)(1 << 3))
#define ADC_CH4                                     ((uint16_t)(1 << 4))
#define ADC_CH5                                     ((uint16_t)(1 << 5))
#define ADC_CH6                                     ((uint16_t)(1 << 6))
#define ADC_CH7                                     ((uint16_t)(1 << 7))
#define ADC_CH_BAT                                  ((uint16_t)(1 << 8))
#define ADC_CH_TMP                                  ((uint16_t)(1 << 9))

#define IS_ADC_CHANNEL(CHA) (((CHA) == ADC_CH0) || ((CHA) == ADC_CH1)\
                                || ((CHA) == ADC_CH2) || ((CHA) == ADC_CH3)\
                                || ((CHA) == ADC_CH4) || ((CHA) == ADC_CH5)\
                                || ((CHA) == ADC_CH6) || ((CHA) == ADC_CH7)\
                                || ((CHA) == ADC_CH_BAT) || ((CHA) == ADC_CH_TMP))
                                
#define IS_ADC_CHANNEL_MAP(MAP) ((((MAP) & (~0x3FF)) == 0) && ((MAP) != 0))

/**
  * @}
  */
  
/** @defgroup ADC_Burst_Size ADC Burst Size
  * @{
  */
  
#define IS_ADC_BURST_SIZE_CONFIG(CONFIG) (((CONFIG) <= 0x3F) && ((CONFIG) >= 0x01))

/**
  * @}
  */
  
/** @defgroup ADC_FIFO_Threshold ADC FIFO Threshold
  * @{
  */
  
#define IS_ADC_FIFO_THRESHOLD(THD) (((THD) <= 0x3F) && ((THD) >= 0x01))

/**
  * @}
  */
  
/** @defgroup ADC_Amic_Config ADC Amic Config
  * @{
  */
  
#define ADC_AMIC_Enable                             ((uint32_t)(1 << 28))
#define ADC_AMIC_Disable                            ((uint32_t)(0 << 28))
#define IS_ADC_AMIC_CONFIG(CONFIG) (((CONFIG) == ADC_AMIC_Enable) || ((CONFIG) == ADC_AMIC_Disable))

/**
  * @}
  */
  
/************************************************************* analog parameters(anapar ad0)-- ad6(mostly for audio) *********************************************/
/** @defgroup ADC_Analog_Gain ADC Analog Gain
  * @{
  */
  
#define ANA_GAIN_0dB                                ((uint16_t)(0 << 4))
#define ANA_GAIN_14dB                              	((uint16_t)(1 << 4))
#define ANA_GAIN_20dB                               ((uint16_t)(2 << 4))
#define ANA_GAIN_24dB                               ((uint16_t)(3 << 4))
#define ANA_GAIN_30dB                               ((uint16_t)(4 << 4))
#define ANA_GAIN_35dB                               ((uint16_t)(5 << 4))
#define ANA_GAIN_40dB                               ((uint16_t)(6 << 4))
#define ANA_GAIN_44dB                               ((uint16_t)(7 << 4))
#define ANA_GAIN_50dB                               ((uint16_t)(8 << 4))


#define IS_ANALOG_GAIN(GAIN) (((GAIN) == ANA_GAIN_0dB) || ((GAIN) == ANA_GAIN_7_7dB)\
                               || ((GAIN) == ANA_GAIN_14dB) || ((GAIN) == ANA_GAIN_20dB)\
                               || ((GAIN) == ANA_GAIN_24dB) || ((GAIN) == ANA_GAIN_30dB)\
                               || ((GAIN) == ANA_GAIN_35dB) || ((GAIN) == ANA_GAIN_40dB)\
                               || ((GAIN) == ANA_GAIN_44dB))

/**
  * @}
  */
  
/** @defgroup ADC_Analog_Bstop_Current ADC Analog Bstop Current
  * @{
  */
  
#define ANA_BSTOP_CUR_2U                            ((uint16_t)(0 << 8))
#define ANA_BSTOP_CUR_3U                            ((uint16_t)(1 << 8))
#define ANA_BSTOP_CUR_4U                            ((uint16_t)(2 << 8))
#define ANA_BSTOP_CUR_6U                            ((uint16_t)(3 << 8))

#define IS_ANALOG_BSTOP_CURRENT_CONFIG(CONFIG) (((CONFIG) == ANA_BSTOP_CUR_2U) || ((CONFIG) == ANA_BSTOP_CUR_3U)\
                                                    || ((CONFIG) == ANA_BSTOP_CUR_4U) || ((CONFIG) == ANA_BSTOP_CUR_6U))

/**
  * @}
  */
  
/** @defgroup ADC_Aanalog_Bias_Current ADC Aanalog Bias Current
  * @{
  */
  
#define ANA_BIAS_CUR_8U                            ((uint16_t)(0 << 10))
#define ANA_BIAS_CUR_12U                           ((uint16_t)(1 << 10))
#define ANA_BIAS_CUR_16U                           ((uint16_t)(2 << 10))
#define ANA_BIAS_CUR_24U                           ((uint16_t)(3 << 10))

#define IS_ANALOG_BIAS_CURRENT_CONFIG(CONFIG) (((CONFIG) == ANA_BIAS_CUR_8U) || ((CONFIG) == ANA_BIAS_CUR_12U)\
                                                    || ((CONFIG) == ANA_BIAS_CUR_16U) || ((CONFIG) == ANA_BIAS_CUR_24U))

/**
  * @}
  */
  
/** @defgroup ADC_Difference_Mode_Config ADC Difference Mode Config
  * @{
  */
  
#define ANA_DIFF_MODE_ENABLE                         ((uint16_t)(1 << 2))
#define ANA_DIFF_MODE_DISABLE                        ((uint16_t)(0 << 2))


#define IS_ANALOG_DIFF_MODE_CONFIG(CONFIG) (((CONFIG) == ANA_DIFF_MODE_ENABLE) || ((CONFIG) == ANA_DIFF_MODE_DISABLE))

/**
  * @}
  */
/** @defgroup ADC_MIC_BIAS_Config ADC MIC BIAS Config
  * @{
  */
    
#define ANA_MIC_BIAS_VREF_1_25                       ((uint8_t)(0 << 6))
#define ANA_MIC_BIAS_VREF_1_47                       ((uint8_t)(1 << 6))
#define ANA_MIC_BIAS_VREF_2_5                        ((uint8_t)(2 << 6))
#define ANA_MIC_BIAS_VREF_2_94                       ((uint8_t)(3 << 6))

#define IS_ANALOG_MIC_BIAS_CONFIG(CONFIG) (((CONFIG) == ANA_MIC_BIAS_VREF_1_25) || ((CONFIG) == ANA_MIC_BIAS_VREF_1_47)\
                    ((CONFIG) == ANA_MIC_BIAS_VREF_2_5) || ((CONFIG) == ANA_MIC_BIAS_VREF_2_94))
    
/**
  * @}
  */


/**
  * @}
  */

/** @defgroup ADC_Exported_Variables ADC Exported Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup ADC_Exported_Functions ADC Exported Functions
  * @{
  */

extern void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
extern void ADC_DeInit(ADC_TypeDef* ADCx);
extern void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);
extern void ADC_INTConfig(ADC_TypeDef* ADCx, uint32_t ADC_IT, FunctionalState newState);\
extern uint16_t ADC_Read(ADC_TypeDef* ADCx, uint16_t channel);
extern void ADC_SetCmpThd(ADC_TypeDef* ADCx, uint16_t ADC_Channel, uint16_t  cmpThd);
extern void ADC_SetCmpConfig(ADC_TypeDef* ADCx, uint16_t ADC_Channel, uint16_t ADC_CMP_CFG, uint16_t ADC_FIFO_CFG);
extern void ADC_Cmd(ADC_TypeDef* ADCx, uint8_t adcMode, FunctionalState NewState);
__STATIC_INLINE FlagStatus ADC_GetIntFlagStatus(ADC_TypeDef* ADCx, uint32_t ADC_INT_FLAG);
__STATIC_INLINE void ADC_ClearINTPendingBit(ADC_TypeDef* ADCx, uint32_t ADC_IT);
__STATIC_INLINE void ADC_ChannelConfig(ADC_TypeDef* ADCx, uint16_t channelMap, FunctionalState NewState);
__STATIC_INLINE void ADC_SetFifoDataEndian(ADC_TypeDef* ADCx, uint16_t endian);
__STATIC_INLINE void ADC_DiffModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
__STATIC_INLINE void ADC_SetMICBIAS(ADC_TypeDef* ADCx, uint8_t data);
__STATIC_INLINE void ADC_Cmd_MICBIAS(ADC_TypeDef* ADCx, FunctionalState NewState);
__STATIC_INLINE void ADC_delayUS(uint32_t t);

void ADC_delayUS(uint32_t t)
{
    // tmp
    t = t*2;

    uint32_t i;
    for(i=0;i<t;++i)
    {
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
        __asm volatile("nop");
    }
}


/**
  * @brief  Clears the ADC interrupt pending bits.
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_IT: specifies the interrupt pending bit to clear. 
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
  *     @arg ADC_INT_ONE_SHOT_DONE: ADC one convert end interrupt
  *     @arg ADC_INT_FIFO_FULL: ADC FIFO full interrupt
  *     @arg ADC_INT_FIFO_RD_REQ: ADC read FIFO request interrupt
  *     @arg ADC_INT_FIFO_RD_ERR: ADC read FIFO error interrupt
  *     @arg ADC_INT_FIFO_TH: ADC codec interrupt
  *     @arg ADC_INT_CODEC: ADC codec interrupt
  * @retval None
  */
__STATIC_INLINE void ADC_ClearINTPendingBit(ADC_TypeDef* ADCx, uint32_t ADC_IT)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_IT(ADC_IT));

    ADCx->INTCR |= (ADC_IT << 16);
    
    return;
}

/**
  * @brief  Checks whether the specified ADC flag is set or not.
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_CMP_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg ADC_FLAG_ADC_HW_EN: ADC hardware is enabled
  *     @arg ADC_FLAG_ADC_FIFO_EN: ADC FIFO is enables
  * @retval The new state of ADC_CMP_FLAG (SET or RESET).
  */
__STATIC_INLINE FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint32_t ADC_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_GET_FLAG(ADC_FLAG));

    if((ADCx->PWR & ADC_FLAG) != 0)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Checks whether the specified ADC interrupt status flag is set or not.
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_INT_FLAG: specifies the interrupt status flag to check. 
  *   This parameter can be one of the following values:
  *     @arg ADC_INT_CMP_CH0: ADC ch0 interrupt flag
  *     @arg ADC_INT_CMP_CH1: ADC ch1 interrupt flag
  *     @arg ADC_INT_CMP_CH2: ADC ch2 interrupt flag
  *     @arg ADC_INT_CMP_CH3: ADC ch3 interrupt flag
  *     @arg ADC_INT_CMP_CH4: ADC ch4 interrupt flag
  *     @arg ADC_INT_CMP_CH5: ADC ch5 interrupt flag
  *     @arg ADC_INT_CMP_CH6: ADC ch6 interrupt flag
  *     @arg ADC_INT_CMP_CH7: ADC ch7 interrupt flag
  *     @arg ADC_INT_CMP_BAT: ADC battery interrupt flag
  *     @arg ADC_INT_CMP_TEMP: ADC temperature interrupt flag
  *     @arg ADC_INT_ONE_SHOT_DONE: ADC one convert end interrupt flag
  *     @arg ADC_INT_FIFO_FULL: ADC FIFO full interrupt flag
  *     @arg ADC_INT_FIFO_RD_REQ: ADC read FIFO request interrupt flag
  *     @arg ADC_INT_FIFO_RD_ERR: ADC read FIFO error interrupt flag
  *     @arg ADC_INT_FIFO_TH: ADC codec interrupt flag
  *     @arg ADC_INT_CODEC: ADC codec interrupt flag
  * @retval The new state of ADC_CMP_FLAG (SET or RESET).
  */
__STATIC_INLINE FlagStatus ADC_GetIntFlagStatus(ADC_TypeDef* ADCx, uint32_t ADC_INT_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_GET_IT(ADC_INT_FLAG));

    if((ADCx->INTSR & ADC_INT_FLAG) != 0)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Enable or disable adc channel.
  * @param  ADCx: selected ADC peripheral.
  * @param  channelMap: ADC channel map. 
  *   This parameter can be one or any combination of the following values:
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
  * @param  NewState: new state of the ADC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @return none.
  */
__STATIC_INLINE void ADC_ChannelConfig(ADC_TypeDef* ADCx, uint16_t channelMap, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_CHANNEL_MAP(channelMap));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE)
    {
        ADCx->ANP0 |= (channelMap << 19);
    }
    else
    {
        ADCx->ANP0 &= (~(channelMap << 19));
    }

    return;
}

/**
  * @brief  Enable or disable adc channel.
  * @param  ADCx: selected ADC peripheral.
  * @param  endian: fifo data endian setting. 
  *   This parameter can be one of the following values:
  *     @arg ADC_FIFO_LITTLE_ENDIAN: fifo data will be little endian.
  *     @arg ADC_FIFO_BIG_ENDIAN:  fifo data will be big endian.
  * @note do not use this function when enable msbc.
  * @return none.
  */
__STATIC_INLINE void ADC_SetFifoDataEndian(ADC_TypeDef* ADCx, uint16_t endian)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_FIFO_ENDIAN(endian));

    ADCx->CR &= (~(1 << 4));
    ADCx->CR |= endian;

    return;
}

/**
  * @brief  Enable or disable adc diffrent mode.
  * @param  ADCx: selected ADC peripheral.
  * @param  NewState: new state of the ADC peripheral.
  *   This parameter can be: ENABLE or DISABLE..
  * @return none.
  */
__STATIC_INLINE void ADC_DiffModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE)
    {
        ADCx->ANP0 |= (1 << 4);
    }
    else
    {

        ADCx->ANP0 &= (~(1 << 4));
    }

    return;
}

/**
  * @brief  configure MIC BIAS Vref voltage.
  * @param  ADCx: selected ADC peripheral.
  * @param  data: new value of MIC BIAS.
  *   This parameter can be one of the following values:
  *     @arg ANA_MIC_BIAS_VREF_1_25: Vref voltage is 1.25V.
  *     @arg ANA_MIC_BIAS_VREF_1_47:  Vref voltage is 1.47V.  
  *     @arg ANA_MIC_BIAS_VREF_2_5: Vref voltage is 2.5V.
  *     @arg ANA_MIC_BIAS_VREF_2_94: Vref voltage is 2.94V.  
  * @return none.
  */
__STATIC_INLINE void ADC_SetMICBIAS(ADC_TypeDef* ADCx, uint8_t data)

{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ANALOG_MIC_BIAS_CONFIG(data));
    
    ADCx->LDO &= ~(3 << 6);
    ADCx->LDO |= data;
}

/**
  * @brief  Enable or disable mic_bias output.
  * @param  ADCx: selected ADC peripheral.
  * @param  NewState: new state of the ADC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @return none.
  */
__STATIC_INLINE void ADC_Cmd_MICBIAS(ADC_TypeDef* ADCx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    
    if(NewState == ENABLE)
    {
        ADCx->LDO |= ((1 << 4) | (1 << 5) | (1 << 24) | (1 << 20) | (1 << 16)); 
    }
    else
    {
        ADCx->LDO &= (~((1 << 4) | (1 << 5) | (1 << 16) | (1 << 20) | (1 << 24))); 
    }
}


#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_ADC_H_ */

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



