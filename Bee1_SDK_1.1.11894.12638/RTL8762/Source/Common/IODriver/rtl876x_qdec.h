/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_qdec.h
* @brief     header file of uart driver.
* @details   
* @author    tifnan_ge
* @date      2015-05-15
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_QDECODER_H_
#define _RTL876X_QDECODER_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup QDEC QDEC
  * @brief Qdecoder driver module
  * @{
  */

/** @defgroup QDEC_Exported_Types QDEC Exported Types
  * @{
  */

/**
 * @brief Qdecoder initialize parameters
 *
 * Qdecoder initialize parameters
 */
typedef struct
{
    uint16_t sampleRate;
    uint16_t debounceTime;
    uint16_t accCount;      
    uint16_t timeoutCount;
    uint16_t axisConfig;
}QDEC_InitTypeDef, *PQDEC_InitTypeDef;

/**
  * @}
  */

/** @defgroup QDEC_Exported_Constants QDEC Exported Constants
  * @{
  */

#define IS_QDEC_PERIPH(PERIPH) ((PERIPH) == QDEC)

/** @defgroup QDEC_Interrupts_Definition QDEC Interrupts Definition 
  * @{
  */
 
#define QDEC_INT_NEW_DATA                             ((uint16_t)(1 << 0))       //get new data and state change
#define QDEC_INT_ACC_CT                               ((uint16_t)(1 << 1))       //accumulation conter reaches a specific number
#define QDEC_INT_TIMEOUT_CT                           ((uint16_t)(1 << 2))       //timeout counter reaches a specific number

#define IS_QDEC_INT_CONFIG(CONFIG) (((CONFIG) == QDEC_INT_NEW_DATA) || ((CONFIG) == QDEC_INT_ACC_CT)\
                            || ((CONFIG) == QDEC_INT_TIMEOUT_CT))
/**
  * @}
  */

/** @defgroup QDEC_Accumulation_Counter QDEC Accumulation Counter
  * @{
  */
 
#define QDEC_ACC_COUNT_1                             ((uint16_t)(0x00 << 4))       //every 1 state change samples
#define QDEC_ACC_COUNT_2                             ((uint16_t)(0x01 << 4))       //every 2 state change samples
#define QDEC_ACC_COUNT_4                             ((uint16_t)(0x02 << 4))       //every 4 state change samples
#define QDEC_ACC_COUNT_8                             ((uint16_t)(0x03 << 4))       //every 8 state change samples
#define QDEC_ACC_COUNT_16                            ((uint16_t)(0x04 << 4))       //every 16 state change samples
#define QDEC_ACC_COUNT_32                            ((uint16_t)(0x05 << 4))       //every 32 state change samples
#define QDEC_ACC_COUNT_64                            ((uint16_t)(0x06 << 4))       //every 64 state change samples
#define QDEC_ACC_COUNT_128                           ((uint16_t)(0x07 << 4))       //every 128 state change samples
#define QDEC_ACC_COUNT_256                           ((uint16_t)(0x08 << 4))       //every 256 state change samples
#define QDEC_ACC_COUNT_512                           ((uint16_t)(0x09 << 4))       //every 512 state change samples
#define QDEC_ACC_COUNT_1024                          ((uint16_t)(0x0a << 4))       //every 1024 state change samples
#define QDEC_ACC_COUNT_2048                          ((uint16_t)(0x0b << 4))       //every 2048 state change samples
#define QDEC_ACC_COUNT_4096                          ((uint16_t)(0x0c << 4))       //every 4096 state change samples
#define QDEC_ACC_COUNT_8192                          ((uint16_t)(0x0d << 4))       //every 8192 state change samples
#define QDEC_ACC_COUNT_16384                         ((uint16_t)(0x0e << 4))       //every 416384 state change samples
#define QDEC_ACC_COUNT_32768                         ((uint16_t)(0x0f << 4))       //every 32768 state change samples

#define IS_QDEC_ACC_COUNT(COUNT) (((COUNT) == QDEC_ACC_COUNT_1) || ((COUNT) == QDEC_ACC_COUNT_2)\
                                    || ((COUNT) == QDEC_ACC_COUNT_4) || ((COUNT) == QDEC_ACC_COUNT_8)\
                                    || ((COUNT) == QDEC_ACC_COUNT_16) || ((COUNT) == QDEC_ACC_COUNT_32)\
                                    || ((COUNT) == QDEC_ACC_COUNT_64) || ((COUNT) == QDEC_ACC_COUNT_128)\
                                    || ((COUNT) == QDEC_ACC_COUNT_256) || ((COUNT) == QDEC_ACC_COUNT_512)\
                                    || ((COUNT) == QDEC_ACC_COUNT_1024) || ((COUNT) == QDEC_ACC_COUNT_2048)\
                                    || ((COUNT) == QDEC_ACC_COUNT_4096) || ((COUNT) == QDEC_ACC_COUNT_8192)\
                                    || ((COUNT) == QDEC_ACC_COUNT_16384) || ((COUNT) == QDEC_ACC_COUNT_32768))
/**
  * @}
  */

/** @defgroup QDEC_Timeout_Count QDEC Timeout Count
  * @{
  */
 
#define QDEC_TIMEOUT_COUNT_100ms                     ((uint16_t)(0x00 << 8))
#define QDEC_TIMEOUT_COUNT_200ms                     ((uint16_t)(0x01 << 8))
#define QDEC_TIMEOUT_COUNT_400ms                     ((uint16_t)(0x02 << 8))
#define QDEC_TIMEOUT_COUNT_600ms                     ((uint16_t)(0x03 << 8))

#define IS_QDEC_TIMEOUT_COUNT(COUNT) (((COUNT) == QDEC_TIMEOUT_COUNT_100ms) || ((COUNT) == QDEC_TIMEOUT_COUNT_200ms)\
                            || ((COUNT) == QDEC_TIMEOUT_COUNT_400ms) || ((COUNT) == QDEC_TIMEOUT_COUNT_600ms))
/**
  * @}
  */

/** @defgroup QDEC_Axis_Cconfig QDEC Axis Cconfig
  * @{
  */
 
#define QDEC_Axis_Config_X_En                        ((uint16_t)(0x01 << 0))
#define QDEC_Axis_Config_Y_En                        ((uint16_t)(0x01 << 1))
#define QDEC_Axis_Config_Z_En                        ((uint16_t)(0x01 << 2))

#define IS_QDEC_AXIS_CONFIG(CONFIG) (((CONFIG) & (~(uint32_t)0x38) == 0x00) && ((CONFIG) != 0x00))

/**
  * @}
  */

/** @defgroup QDEC_Flag QDEC Flag
  * @{
  */

#define QDEC_FLAG_OVERFLOW_X                          ((uint32_t)(1 << 3))
#define QDEC_FLAG_OVERFLOW_Y                          ((uint32_t)(1 << 4))
#define QDEC_FLAG_OVERFLOW_Z                          ((uint32_t)(1 << 5))
#define QDEC_FLAG_ILLEGAL_X                           ((uint32_t)(1 << 6)) 
#define QDEC_FLAG_ILLEGAL_Y                           ((uint32_t)(1 << 7)) 
#define QDEC_FLAG_ILLEGAL_Z                           ((uint32_t)(1 << 8)) 
#define QDEC_FLAG_UNDERFLOW_X                         ((uint32_t)(1 << 15)) 
#define QDEC_FLAG_UNDERFLOW_Y                         ((uint32_t)(1 << 16)) 
#define QDEC_FLAG_UNDERFLOW_Z                         ((uint32_t)(1 << 17)) 
//interrupt flag
#define QDEC_FLAG_INT_X                               ((uint32_t)(1 << 20))
#define QDEC_FLAG_INT_Y                               ((uint32_t)(1 << 21))
#define QDEC_FLAG_INT_Z                               ((uint32_t)(1 << 22))

#define IS_QDEC_GET_FLAG(FLAG) (((FLAG) == QDEC_FLAG_OVERFLOW_X)   || ((FLAG) == QDEC_FLAG_UNDERFLOW_X)\
                                    || ((FLAG) == QDEC_FLAG_OVERFLOW_Y)|| ((FLAG) == QDEC_FLAG_UNDERFLOW_Y)\
                                    || ((FLAG) == QDEC_FLAG_OVERFLOW_Z) || ((FLAG) == QDEC_FLAG_UNDERFLOW_Z)\
                                    || ((FLAG) == QDEC_FLAG_INT_X) || ((FLAG) == QDEC_FLAG_INT_Y)\
                                    || ((FLAG) == QDEC_FLAG_INT_Z) || ((FLAG) == QDEC_FLAG_ILLEGAL_X)\
                                    || ((FLAG) == QDEC_FLAG_ILLEGAL_Y) || ((FLAG) == QDEC_FLAG_ILLEGAL_Z))
                                    
#define IS_QDEC_INT_FLAG(FLAG)   (((FLAG) == QDEC_FLAG_INT_X)   || ((FLAG) == QDEC_FLAG_INT_Y)\
                                    || ((FLAG) == QDEC_FLAG_INT_Z))

#define IS_QDEC_STATUS_FLAG(FLAG) (((FLAG) == QDEC_FLAG_OVERFLOW_X) || ((FLAG) == QDEC_FLAG_UNDERFLOW_X)\
                                    || ((FLAG) == QDEC_FLAG_OVERFLOW_Y) || ((FLAG) == QDEC_FLAG_UNDERFLOW_Y)\
                                    || ((FLAG) == QDEC_FLAG_OVERFLOW_Z) || ((FLAG) == QDEC_FLAG_UNDERFLOW_Z)\
                                    || ((FLAG) == QDEC_FLAG_ILLEGAL_X) || ((FLAG) == QDEC_FLAG_ILLEGAL_Y)\
                                    || ((FLAG) == QDEC_FLAG_ILLEGAL_Z))

/**
  * @}
  */

/** @defgroup QDEC_Axis QDEC Axis
  * @{
  */
  
#define QDEC_AXIS_X                                     ((uint32_t)(1 << 0))
#define QDEC_AXIS_Y                                     ((uint32_t)(1 << 1))
#define QDEC_AXIS_Z                                     ((uint32_t)(1 << 2))

#define IS_QDEC_AXIS(AXIS)   (((AXIS) == QDEC_AXIS_X)   || ((AXIS) == QDEC_AXIS_Y) || ((AXIS) == QDEC_AXIS_Z))
#define IS_QDEC_ALL_AXIS_DIR(AXIS)  (((AXIS) & 0x08) == 0 && (AXIS) != 0)

/**
  * @}
  */

/** @defgroup QDEC_Axis_Direction QDEC Axis Direction
  * @{
  */
  
#define QDEC_AXIS_DIR_UP                                 ((uint16_t)0x01)
#define QDEC_AXIS_DIR_DOWN                               ((uint16_t)0x00)

#define IS_QDEC_AXIS_DIR(DIR)   (((DIR) == QDEC_AXIS_DIR_UP)   || ((DIR) == QDEC_AXIS_DIR_DOWN))

/**
  * @}
  */


/**
  * @}
  */

/** @defgroup QDEC_Exported_Macros QDEC Exported Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup QDEC_Exported_Functions QDEC Exported Functions
  * @{
  */
  
extern void QDEC_Init(QDEC_TypeDef* QDECx, QDEC_InitTypeDef* QDEC_InitStruct);
extern void QDEC_DeInit(QDEC_TypeDef* QDECx);
extern void QDEC_StructInit(QDEC_InitTypeDef* QDEC_InitStruct);
extern void QDEC_INTConfig(QDEC_TypeDef* QDECx, uint32_t QDEC_IT, FunctionalState newState);
extern FlagStatus QDEC_GetFlagState(QDEC_TypeDef* QDECx, uint32_t QDEC_FLAG);
extern void QDEC_INTMask(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS, FunctionalState newState);
__STATIC_INLINE void QDEC_ClearINTPendingBit(QDEC_TypeDef* QDECx, uint32_t QDEC_INT_FLAG);
__STATIC_INLINE void QDEC_ClearFlags(QDEC_TypeDef* QDECx, uint32_t QDEC_FLAG);
__STATIC_INLINE uint16_t QDEC_GetAxisDirection(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS);
__STATIC_INLINE void QDEC_ClearAccValue(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS);
__STATIC_INLINE uint16_t QDEC_AccValue(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS);

/**
  * @brief  Clears the Qdecoder interrupt pending bits.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_INT_FLAG: specifies the interrupt pending bit to clear. 
  *   This parameter can be any combination of the following values:
  *     @arg QDEC_FLAG_INT_X: X axis interrupt.
  *     @arg QDEC_FLAG_INT_Y: Y axis interrupt.
  *     @arg QDEC_FLAG_INT_Z: Z axis interrupt.
  * @retval None
  */
__STATIC_INLINE void QDEC_ClearINTPendingBit(QDEC_TypeDef* QDECx, uint32_t QDEC_INT_FLAG)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_INT_FLAG(QDEC_INT_FLAG));

    QDECx->REG_CR1 |= (QDEC_INT_FLAG >> 4);
    
    return;
}

/**
  * @brief  Clears Qdecoder pending flags.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_FLAG: specifies the flag to clear. 
  *   This parameter can be one of the following values:
  *     @arg QDEC_FLAG_OVERFLOW_X: The overflow flag for accunulation counter for X axis.
  *     @arg QDEC_FLAG_UNDERFLOW_X: The underflow flag for accunulation counter for X axis.
  *     @arg QDEC_FLAG_OVERFLOW_Y: The overflow flag for accunulation counter for Y axis.
  *     @arg QDEC_FLAG_UNDERFLOW_Y: The underflow flag for accunulation counter for Y axis.
  *     @arg QDEC_FLAG_OVERFLOW_Z: The overflow flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_UNDERFLOW_Z: The underflow flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_ILLEGAL_X: The illegal flag for accunulation counter for Y axis.
  *     @arg QDEC_FLAG_ILLEGAL_X: The illegal flag for accunulation counter for Z axis.
  *     @arg QDEC_FLAG_ILLEGAL_X: The illegal flag for accunulation counter for Z axis.
  * @retval None.
  */
__STATIC_INLINE void QDEC_ClearFlags(QDEC_TypeDef* QDECx, uint32_t QDEC_FLAG)
{ 
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_STATUS_FLAG(QDEC_FLAG));

    QDECx->REG_CR2 |= QDEC_FLAG;

    return;
}

/**
  * @brief  Get Qdecoder Axis(x/y/z) direction.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_AXIS: specifies the Qdecoder axis. 
  *   This parameter can be one of the following values:
  *     @arg QDEC_AXIS_X: The qdecoder X axis.
  *     @arg QDEC_AXIS_Y: The qdecoder Y axis.
  *     @arg QDEC_AXIS_Z: The qdecoder Z axis.
  * @retval The direction of the axis.
  *   This parameter can be one of the following values:
  *     @arg QDEC_AXIS_DIR_UP: The axis is rolling up.
  *     @arg QDEC_AXIS_DIR_DOWN: The axis is rolling down.
  */
__STATIC_INLINE uint16_t QDEC_GetAxisDirection(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    return ((*((uint32_t*)(&QDECx->REG_SR_X) + QDEC_AXIS / 2) & (1 << 28)) == (1 << 28));
}

/**
  * @brief  Clear Axis(x/y/z) accumulation value.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_AXIS: specifies the Qdecoder axis. 
  *   This parameter can be one of the following values:
  *     @arg QDEC_AXIS_X: The qdecoder X axis.
  *     @arg QDEC_AXIS_Y: The qdecoder Y axis.
  *     @arg QDEC_AXIS_Z: The qdecoder Z axis.
  * @retval None.
  */
__STATIC_INLINE void QDEC_ClearAccValue(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

    QDECx->REG_CR2 |= (1 << (QDEC_AXIS / 2));

    return;
}

/**
  * @brief  Get axis(x/y/z) accumulation value.
  * @param  QDECx: selected Qdecoder peripheral.
  * @param  QDEC_AXIS: specifies the Qdecoder axis. 
  *   This parameter can be one of the following values:
  *     @arg QDEC_AXIS_X: The qdecoder X axis.
  *     @arg QDEC_AXIS_Y: The qdecoder Y axis.
  *     @arg QDEC_AXIS_Z: The qdecoder Z axis.
  * @retval The axis(x/y/z) accumulation value.
  */
__STATIC_INLINE uint16_t QDEC_AccValue(QDEC_TypeDef* QDECx, uint32_t QDEC_AXIS)
{
    /* Check the parameters */
    assert_param(IS_QDEC_PERIPH(QDECx));
    assert_param(IS_QDEC_AXIS_DIR(QDEC_AXIS));

   return (uint16_t)(*((uint32_t*)(&QDECx->REG_SR_X) + QDEC_AXIS / 2) & 0xFFFF);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_QDECODER_H_ */

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



