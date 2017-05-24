/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_codec.h
* @brief     header file of codec driver.
* @details   
* @author    tifnan_ge
* @date      2015-05-26
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_CODEC_H_
#define _RTL876X_CODEC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup CODEC CODEC
  * @brief Codec driver module
  * @{
  */

/** @defgroup CODEC_Exported_Types CODEC Exported Types
  * @{
  */

/**
 * @brief CODEC initialize parameters
 *
 * codec initialize parameters
 */
typedef struct
{
    uint16_t micType;           /* dmic or amic */
    uint16_t micBiasTimeCtrl;   /* mic bias stable time config */
    uint16_t micVolume;         /* mic volume */
    uint16_t msbcEn;            /* enable mscb or not */
    
    /* only for amic */
    uint16_t hfpConfig;         /* High pass filter enable control */
    uint16_t hfp2ndConfig;      /* 2nd High pass filter enable control */
    uint16_t compGain;          /* ADCx compensate  gain */
    
    /* for msbc, enable default */
    uint16_t msbcEncodeScale;   /* msbc encoder output scale control */
    
    /* only for dmic, if micType == amic, all following parameters are ignored */
    uint16_t dmicClock;         /* dmic clock */
    uint16_t dmicLfpFc;         /* dmic lfp frequency */
    uint16_t dmicDataLatchCtrl; /* dmic data latching control */
    uint16_t dmicBoostCtrl;     /* dmic boost gain control */ 
}CODEC_InitTypeDef, *PCODEC_InitTypeDef;

/**
  * @}
  */

/** @defgroup CODEC_Exported_Constants CODEC Exported Constants
  * @{
  */

#define IS_CODEC_PERIPH(PERIPH) ((PERIPH) == CODEC)

/** @defgroup CODEC_Mic_Type CODEC Mic Type
  * @{
  */

#define CODEC_AMIC                                 ((uint16_t)(0x00))
#define CODEC_DMIC                                 ((uint16_t)(0x01))

#define IS_CODEC_MIC_TYPE(TYPE) (((TYPE) == CODEC_AMIC) || ((TYPE) == CODEC_DMIC))
/**
  * @}
  */

/** @defgroup DMIC_Clock DMIC Clock 
  * @{
  */

#define CODEC_DMIC_CLOCK_625K                      ((uint16_t)(0x03 << 9))  //625K
#define CODEC_DMIC_CLOCK_1_25M                     ((uint16_t)(0x02 << 9))  //1.25M
#define CODEC_DMIC_CLOCK_2_5M                      ((uint16_t)(0x01 << 9))  //2.5M
#define CODEC_DMIC_CLOCK_5M                        ((uint16_t)(0x00 << 9))  //5M

#define IS_CODEC_DMIC_CLOCK(CLOCK) (((CLOCK) == CODEC_DMIC_CLOCK_625K) || ((CLOCK) == CODEC_DMIC_CLOCK_1_25M)\
                            || ((CLOCK) == CODEC_DMIC_CLOCK_2_5M) || ((CLOCK) == CODEC_DMIC_CLOCK_5M))
/**
  * @}
  */
  
/** @defgroup CODEC_Compensate_Gain CODEC Compensate Gain
  * @{
  */

#define CODEC_COMP_GAIN_3dB                        ((uint16_t)(0x03 << 5))
#define CODEC_COMP_GAIN_2dB                        ((uint16_t)(0x02 << 5))
#define CODEC_COMP_GAIN_1dB                        ((uint16_t)(0x01 << 5))
#define CODEC_COMP_GAIN_0dB                        ((uint16_t)(0x00 << 5))

#define IS_CODEC_COMP_GAIN(GAIN) (((GAIN) == CODEC_COMP_GAIN_3dB) || ((GAIN) == CODEC_COMP_GAIN_2dB)\
                            || ((GAIN) == CODEC_COMP_GAIN_1dB) || ((GAIN) == CODEC_COMP_GAIN_0dB))
/**
  * @}
  */

/** @defgroup DMIC_Source_LPF_Frequency DMIC Source LPF Frequency
  * @{
  */

#define CODEC_DMIC_LPF_31_04KHZ                    ((uint16_t)(0x00 << 1))  //31.04khz
#define CODEC_DMIC_LPF_46_92KHZ                    ((uint16_t)(0x01 << 1))  //46.92khz
#define CODEC_DMIC_LPF_63_06KHZ                    ((uint16_t)(0x02 << 1))  //63.06khz
#define CODEC_DMIC_LPF_79_45KHZ                    ((uint16_t)(0x03 << 1))  //79.45khz

#define IS_CODEC_DMIC_LPF_CONFIG(CONFIG) (((CONFIG) == CODEC_DMIC_LPF_31_04KHZ) || ((CONFIG) == CODEC_DMIC_LPF_46_92KHZ)\
                            || ((CONFIG) == CODEC_DMIC_LPF_63_06KHZ) || ((CONFIG) == CODEC_DMIC_LPF_79_45KHZ))
/**
  * @}
  */
  
/** @defgroup HPF_Control HPF Control
  * @{
  */

#define CODEC_HFP_ENABLE                            ((uint16_t)(0x01 << 4))
#define CODEC_HFP_DISABLE                           ((uint16_t)(0x00 << 4))

#define IS_CODEC_HFP_CTRL(CTRL) (((CTRL) == CODEC_HFP_ENABLE) || ((CTRL) == CODEC_HFP_DISABLE))
/**
  * @}
  */
  
/** @defgroup Second_HPF_Cotrol Second HPF Cotrol
  * @{
  */

#define CODEC_2ND_HFP_ENABLE                        ((uint16_t)(0x01 << 4))
#define CODEC_2ND_HFP_DISABLE                       ((uint16_t)(0x00 << 4))

#define IS_CODEC_2ND_HFP_CTRL(CTRL) (((CTRL) == CODEC_2ND_HFP_ENABLE) || ((CTRL) == CODEC_2ND_HFP_DISABLE))
/**
  * @}
  */

/** @defgroup DMIC_Latching_Control DMIC Latching Control
  * @{
  */

#define CODEC_DMIC_LATCHING_CTRL_RISING             ((uint16_t)(0x00 << 0))
#define CODEC_DMIC_LATCHING_CTRL_FALLING            ((uint16_t)(0x01 << 0))

#define IS_CODEC_DMIC_LATCHNG_CTRL(CTRL) (((CTRL) == CODEC_DMIC_LATCHING_CTRL_RISING) || ((CTRL) == CODEC_DMIC_LATCHING_CTRL_FALLING))
/**
  * @}
  */
  
/** @defgroup MSBC_Config MSBC Config
  * @{
  */

#define CODEC_MSBC_ENABLE                           ((uint16_t)(0x01 << 0))
#define CODEC_MSBC_DISABLE                          ((uint16_t)(0x00 << 0))

#define IS_CODEC_MSBC_CONFIG(CONFIG) (((CONFIG) == CODEC_MSBC_ENABLE) || ((CONFIG) == CODEC_MSBC_DISABLE))
/**
  * @}
  */


/** @defgroup DMIC_Boost_Gain_Control DMIC Boost Gain Control
  * @{
  */

#define CODEC_DMIC_BOOST_GAIN_36dB                  ((uint16_t)(0x03 << 8))
#define CODEC_DMIC_BOOST_GAIN_24dB                  ((uint16_t)(0x02 << 8))
#define CODEC_DMIC_BOOST_GAIN_12dB                  ((uint16_t)(0x01 << 8))
#define CODEC_DMIC_BOOST_GAIN_0dB                   ((uint16_t)(0x00 << 8))

#define IS_CODEC_DMIC_BOOST_GAIN_CTRL(CTRL) (((CTRL) == CODEC_DMIC_BOOST_GAIN_36dB) || ((CTRL) == CODEC_DMIC_BOOST_GAIN_24dB)\
                                                || ((CTRL) == CODEC_DMIC_BOOST_GAIN_12dB) || ((CTRL) == CODEC_DMIC_BOOST_GAIN_0dB))
/**
  * @}
  */

/** @defgroup MIC_Volume MIC Volume
  * @{
  */

#define IS_CODEC_MIC_VOLUME_CONFIG(CONFIG) (CONFIG <= 0x7F)   //0x00---17.625dB, 0x2f--0dB, 0x7f--30dB
/**
  * @}
  */

/** @defgroup MIC_Bias_Stable_Time MIC Bias Stable Time
  * @{
  */

#define CODEC_MIC_BIAS_STABLE_TIME_2us              ((uint16_t)(0x00 << 6))
#define CODEC_MIC_BIAS_STABLE_TIME_4us              ((uint16_t)(0x01 << 6))
#define CODEC_MIC_BIAS_STABLE_TIME_8us              ((uint16_t)(0x02 << 6))
#define CODEC_MIC_BIAS_STABLE_TIME_12us             ((uint16_t)(0x03 << 6))

#define IS_CODEC_MIC_BIAS_STABLE_TIME_CONFIG(CONFIG) (((CONFIG) == CODEC_MIC_BIAS_STABLE_TIME_2us) || ((CONFIG) == CODEC_MIC_BIAS_STABLE_TIME_4us)\
                                                || ((CONFIG) == CODEC_MIC_BIAS_STABLE_TIME_8us) || ((CONFIG) == CODEC_MIC_BIAS_STABLE_TIME_12us))
/**
  * @}
  */

/** @defgroup MSBC_Encoder_Output_Scale MSBC Encoder Output Scale
  * @{
  */

#define CODEC_mSBC_OUT_SCALE_X1                     ((uint16_t)(0x00 << 1))
#define CODEC_mSBC_OUT_SCALE_X1_2                   ((uint16_t)(0x01 << 1))
#define CODEC_mSBC_OUT_SCALE_X4                     ((uint16_t)(0x02 << 1))
#define CODEC_mSBC_OUT_SCALE_X2                     ((uint16_t)(0x03 << 1))

#define IS_CODEC_MSBC_OUT_SCALE_CONFIG(CONFIG) (((CONFIG) == CODEC_mSBC_OUT_SCALE_X1) || ((CONFIG) == CODEC_mSBC_OUT_SCALE_X1_2)\
                                                || ((CONFIG) == CODEC_mSBC_OUT_SCALE_X4) || ((CONFIG) == CODEC_mSBC_OUT_SCALE_X2))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup CODEC_Exported_Global_Variables CODEC Exported Global Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup CODEC_Exported_Functions CODEC Exported Functions
  * @{
  */

extern void CODEC_Init(CODEC_TypeDef* CODECx, CODEC_InitTypeDef* CODEC_InitStruct);
extern void CODEC_DeInit(CODEC_TypeDef* CODECx);
extern void CODEC_StructInit(CODEC_InitTypeDef* CODEC_InitStruct);
__STATIC_INLINE void CODEC_Reset(CODEC_TypeDef* CODECx);
__STATIC_INLINE void CODEC_SetAmicMute(CODEC_TypeDef* CODECx, FunctionalState newState);
__STATIC_INLINE void CODEC_SetAmicVolume(CODEC_TypeDef* CODECx, uint16_t volume);
__STATIC_INLINE void CODEC_SetDmicVolume(CODEC_TypeDef* CODECx, uint16_t volume);
__STATIC_INLINE void CODEC_Amic_Cmd(CODEC_TypeDef* CODECx, FunctionalState NewState);
__STATIC_INLINE void CODEC_Dmic_Cmd(CODEC_TypeDef* CODECx, FunctionalState NewState);

/**
  * @brief  Reset CODEC. 
  * @param  CODECx: selected CODEC peripheral..
  * @retval None
  */
__STATIC_INLINE void CODEC_Reset(CODEC_TypeDef* CODECx)
{
    assert_param(IS_CODEC_PERIPH(CODECx));
    
    /* CODEC IP reset */
    CODECx->RESET &= (~(1 << 0));    /* keep 6T at least */
    CODECx->RESET &= (~(1 << 0));
    CODECx->RESET |= (1 << 0);
}

/**
  * @brief  Set Analog mic mute.
  * @param  CODECx: selected CODEC peripheral.
  * @param  NewState: new state of the specified CODEC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void CODEC_SetAmicMute(CODEC_TypeDef* CODECx, FunctionalState newState)
{
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    
    if(newState == ENABLE)
    {
        CODECx->CR5 |= (1 << 7);
    }
    else
    {
        CODECx->CR5 &= (~(1 << 7));
    }
    
    return;
}

/**
  * @brief  Set Amic volume
  * @param  CODECx: selected CODEC peripheral.
  * @param  volume: mic volume.
  * @retval None
  */
__STATIC_INLINE void CODEC_SetAmicVolume(CODEC_TypeDef* CODECx, uint16_t volume)
{
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_CODEC_MIC_VOLUME_CONFIG(volume));
    
    /* set amic volume */
    CODECx->CR5 &= (~(0x7f << 0));
    CODECx->CR5 |= (volume << 0);
}

/**
  * @brief  Set Dmic volume
  * @param  CODECx: selected CODEC peripheral.
  * @param  volume: dmic volume.
  * @retval None
  */
__STATIC_INLINE void CODEC_SetDmicVolume(CODEC_TypeDef* CODECx, uint16_t volume)
{
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_CODEC_MIC_VOLUME_CONFIG(volume));
    
    /* set amic volume */ 
    CODECx->CR3 &= (~(0x7f << 0));
    CODECx->CR3 |= (volume << 0);
}

/**
  * @brief  Enables or disables the Amic peripheral.
  * @param  CODECx: selected CODEC peripheral.
  * @param  NewState: new state of the CODEC peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void CODEC_Amic_Cmd(CODEC_TypeDef* CODECx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE)
    {
        /* enable amic clock */
        CODECx->CR0 |= ((1 << 1) | (1 << 0));
    }
    else
    {
        /* disable amic clock */
        CODECx->CR0 &= (~((1 << 1) | (1 << 0)));
    }

    return;
}

/**
  * @brief  Enables or disables the Dmic peripheral.
  * @param  CODECx: selected CODEC peripheral.
  * @param  NewState: new state of the CODEC peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void CODEC_Dmic_Cmd(CODEC_TypeDef* CODECx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE)
    {
        /* enable dmic clock */
        CODECx->CR0 |= (1 << 8);
    }
    else
    {
        /* disable dmic clock */
        CODECx->CR0 &= (~(1 << 8));
    }

    return;
}


#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_CODEC_H_ */

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



