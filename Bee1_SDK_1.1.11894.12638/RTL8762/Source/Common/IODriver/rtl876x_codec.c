/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_codec.c
* @brief    This file provides all the CODEC firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-05-26
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_codec.h"

/**
  * @brief Initializes the CODEC peripheral according to the specified 
  *   parameters in the CODEC_InitStruct
  * @param  CODECx: selected CODEC peripheral.
  * @param  CODEC_InitStruct: pointer to a CODEC_InitTypeDef structure that
  *   contains the configuration information for the specified CODEC peripheral
  * @retval None
  */
void CODEC_Init(CODEC_TypeDef* CODECx, CODEC_InitTypeDef* CODEC_InitStruct)
{
    assert_param(IS_CODEC_PERIPH(CODECx));
    assert_param(IS_CODEC_MIC_TYPE(CODEC_InitStruct->micType));
    assert_param(IS_CODEC_COMP_GAIN(CODEC_InitStruct->compGain));
    assert_param(IS_CODEC_HFP_CTRL(CODEC_InitStruct->hfpConfig));
    assert_param(IS_CODEC_2ND_HFP_CTRL(CODEC_InitStruct->hfp2ndConfig));
    assert_param(IS_CODEC_MIC_BIAS_STABLE_TIME_CONFIG(CODEC_InitStruct->micBiasTimeCtrl));
    assert_param(IS_CODEC_MSBC_OUT_SCALE_CONFIG(CODEC_InitStruct->msbcEncodeScale));
    assert_param(IS_CODEC_DMIC_CLOCK(CODEC_InitStruct->dmicClock));
    assert_param(IS_CODEC_DMIC_LPF_CONFIG(CODEC_InitStruct->dmicLfpFc));
    assert_param(IS_CODEC_DMIC_LATCHNG_CTRL(CODEC_InitStruct->dmicDataLatchCtrl));
    assert_param(IS_CODEC_DMIC_BOOST_GAIN_CTRL(CODEC_InitStruct->dmicBoostCtrl));
    assert_param(IS_CODEC_MIC_VOLUME_CONFIG(CODEC_InitStruct->micVolume));
    assert_param(IS_CODEC_MSBC_CONFIG(CODEC_InitStruct->msbcEn));
    
    /* CODEC IP reset */
    CODECx->RESET &= (~(1 << 0));
    CODECx->RESET |= (1 << 0);
    
    /* amic */
    if(CODEC_InitStruct->micType == CODEC_AMIC)
    {
        /* disable dmic clock */
        CODECx->CR0 &= (~(1 << 8));
        /* manual control bit and amic clock */
        CODECx->CR0 |= ((1 << 11) | (1 << 1) | (1 << 0));
        
        /* analog ADCx input path mute on, set compensate  gain */
        CODECx->CR1 &= (~(1 << 4));
        CODECx->CR1 |= (CODEC_InitStruct->hfpConfig | CODEC_InitStruct->compGain);
        /* un-mute */
        CODECx->CR1 &= (~(1 << 3));
        
        /* set 2nd hfp */
        CODECx->CR2 &= (~(1 << 4));
        CODECx->CR2 |= CODEC_InitStruct->hfp2ndConfig;
        
        /* set dmic boost gain and dmic volume, un-mute */
        /*CODECx->CR3 &= (~((0x03 << 8) | (0x7f << 0) | (0x01 << 7)));
        CODECx->CR3 |= (CODEC_InitStruct->dmicBoostCtrl | CODEC_InitStruct->micVolume);*/ 
        
        if(CODEC_InitStruct->msbcEn == CODEC_MSBC_ENABLE)
        {
            /* set mic bias stable time and msbc encoder output scale, enable msbc, force enable ADCx FIFO */
            CODECx->CR4 |= ((1 << 0) | (1 << 4) | (1 << 5)| CODEC_InitStruct->micBiasTimeCtrl | CODEC_InitStruct->msbcEncodeScale);
        }
        else
        {
            /* set mic bias stable time and msbc encoder output scale, disable msbc, force enable ADCx FIFO */
            CODECx->CR4 |= ((1 << 0) | (1 << 3) | (1 << 4) | CODEC_InitStruct->micBiasTimeCtrl | CODEC_InitStruct->msbcEncodeScale);
        }
        
        //ADCx Left Channel Digital Mixer un-mute, set amic volume
        CODECx->CR5 &= (~((1 << 7) | (0x7f << 0)));
        CODECx->CR5 |= CODEC_InitStruct->micVolume;
    }
    
    /* dmic */
    if(CODEC_InitStruct->micType == CODEC_DMIC)
    {
        /* disable amic clock, clear dmic clock settings */
        CODECx->CR0 &= (~((1 << 0) | (1 << 1) | (0x03 << 9)));
        /* enable dmic clock and manual control bit */
        CODECx->CR0 |= ((1 << 8) | CODEC_InitStruct->dmicClock | ((1 << 11)));
        
        /* [0x58] analog ADC input path mute, set compensate gain and dmic LFP frequency */
        CODECx->CR1 &= (~(1 << 4));
        CODECx->CR1 |= (CODEC_InitStruct->hfpConfig | CODEC_InitStruct->compGain | CODEC_InitStruct->dmicLfpFc | (1 << 3));
        
        /* [0x5c]set 2nd hfp, dmic data latching, DMIC input path mute control Left Channel un-mute */
        CODECx->CR2 &= (~((1 << 4) | (1 << 3)));
        CODECx->CR2 |= (CODEC_InitStruct->hfp2ndConfig | CODEC_InitStruct->dmicLfpFc | CODEC_InitStruct->dmicDataLatchCtrl);
        
        /* [0x60] set dmic boost gain and dmic volume, un-mute */
        CODECx->CR3 &= (~((0x03 << 8) | (0x7f << 0) | (0x01 << 7)));
        CODECx->CR3 |= (CODEC_InitStruct->dmicBoostCtrl | CODEC_InitStruct->micVolume);
        
         /*  [0x6c] */
        CODECx->CR5 |= (1 << 8);
        
        if(CODEC_InitStruct->msbcEn == CODEC_MSBC_ENABLE)
        {
            /* set mic bias stable time and msbc encoder output scale, enable msbc, force enable ADCx FIFO */
            CODECx->CR4 |= ((1 << 0) | (1 << 4) | (1 << 5) | CODEC_InitStruct->micBiasTimeCtrl | CODEC_InitStruct->msbcEncodeScale);
        }
        else
        {
            /* bypass msbc, force enable ADCx FIFO */
            CODECx->CR4 |= ((1 << 0) | (1 << 4) | (1 << 3) | (1 << 5)\
                | CODEC_InitStruct->micBiasTimeCtrl | CODEC_InitStruct->msbcEncodeScale);
            
            //enable dicm no msbc must enable ad_clk
            CODECx->CR0 |= (1 << 1);
        }
    }

    return;
}

/**
  * @brief  Deinitializes the CODEC peripheral registers to their default reset values(turn off CODEC clock).
  * @param  CODECx: selected CODEC peripheral.
  * @retval None
  */
void CODEC_DeInit(CODEC_TypeDef* CODECx)
{
    assert_param(IS_CODEC_PERIPH(CODECx));
    
    RCC_PeriphClockCmd(APBPeriph_CODEC, APBPeriph_CODEC_CLOCK, DISABLE);

    return;
}

/**
  * @brief  Fills each CODEC_InitStruct member with its default value.
  * @param  CODEC_InitStruct: pointer to an CODEC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void CODEC_StructInit(CODEC_InitTypeDef* CODEC_InitStruct)
{
    CODEC_InitStruct->micType           = CODEC_AMIC;
    CODEC_InitStruct->compGain          = CODEC_COMP_GAIN_0dB;
    CODEC_InitStruct->hfpConfig         = CODEC_HFP_ENABLE;
    CODEC_InitStruct->hfp2ndConfig      = CODEC_2ND_HFP_ENABLE;
    CODEC_InitStruct->micBiasTimeCtrl   = CODEC_MIC_BIAS_STABLE_TIME_2us;
    CODEC_InitStruct->msbcEncodeScale   = CODEC_mSBC_OUT_SCALE_X1;
    CODEC_InitStruct->dmicClock         = CODEC_DMIC_CLOCK_2_5M;
    CODEC_InitStruct->dmicLfpFc         = CODEC_DMIC_LPF_31_04KHZ;
    CODEC_InitStruct->dmicDataLatchCtrl = CODEC_DMIC_LATCHING_CTRL_FALLING;
    CODEC_InitStruct->dmicBoostCtrl     = CODEC_DMIC_BOOST_GAIN_0dB;
    CODEC_InitStruct->msbcEn            = CODEC_MSBC_DISABLE;                   /* disable msbc */
    CODEC_InitStruct->micVolume         = 0x2f;                                 /* 0dB */
    
    return;
}

