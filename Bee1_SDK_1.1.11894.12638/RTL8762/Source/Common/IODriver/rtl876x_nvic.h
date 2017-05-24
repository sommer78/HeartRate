/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_nvic.h
* @brief     
* @details   
* @author    elliot chen
* @date      2015-05-19
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __RTL876X_NVIC_H
#define __RTL876X_NVIC_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup NVIC NVIC
  * @{
  */ 

/** @defgroup NVIC_Exported_Types NVIC Exported Types
  * @{
  */

/** 
  * @brief  NVIC Init Structure definition  
  */

typedef struct
{
    IRQn_Type NVIC_IRQChannel;                  /*!< Specifies the IRQ channel to be enabled or disabled.
                                                                    This parameter can be a value of @ref IRQn_Type*/

    uint32_t NVIC_IRQChannelPriority;           /*!< Specifies the priority for the IRQ channel
                                                                    specified in NVIC_IRQChannel. This parameter can be a value
                                                                    between 0 and x as described in the table @ref NVIC_Priority_Table */

    FunctionalState NVIC_IRQChannelCmd;         /*!< Specifies whether the IRQ channel defined in NVIC_IRQChannel
                                                                     will be enabled or disabled. 
                                                                    This parameter can be set either to ENABLE or DISABLE */   
} NVIC_InitTypeDef;

/**
  * @}
  */

/** @defgroup NVIC_Exported_Functions NVIC Exported Functions
  * @{
  */
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);

#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_NVIC_H*/

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

