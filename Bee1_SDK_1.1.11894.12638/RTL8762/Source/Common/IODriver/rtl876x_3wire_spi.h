/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl8762_2wire_spi.h
* @brief     header file of 2wire spi driver.
* @details   
* @author    tifnan_ge
* @date      2015-10-28
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _RTL8762_3WIRE_SPI_H_
#define _RTL8762_3WIRE_SPI_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup THREE_WIRE_SPI 3WIRE SPI
  * @{
  */

/** @defgroup THREE_WIRE_SPI_Exported_Types 3WIRE SPI Exported Types
  * @{
  */

/**
 * @brief THREE_WIRE_SPI initialize parameters 3WIRE SPI initialize parameters
 *
 * adc initialize parameters
 */
typedef struct
{
    uint32_t mode;          /*!< Specifies SPI operation mode. 
								This parameter can be a value of SPI3WIRE_2WIRE_MODE or SPI3WIRE_3WIRE_MODE */
    uint32_t oeDelayCfg;	/*!< Specifies SPI output delay 1T or not. 
								This parameter can be a value of SPI3WIRE_OE_DELAY_1T or SPI3WIRE_OE_DELAY_NONE */
	uint32_t readDelay;		/*!< Specifies the delay time from the end of address phase to the start of read data phase.
                                                    This parameter can be a value of 0x00 to 0x1f, delay time = (readDelay+3)/(2*speed) */
    uint32_t endExtMode;    /*!< Specifies extended timing window for SPI output enable = 0. */
    uint32_t speed;			/*!< Specifies SPI clock. */
}SPI3WIRE_InitTypeDef, *PSPI3WIRE_InitTypeDef;

/**
  * @}
  */

/** @defgroup THREE_WIRE_SPI_Exported_Constants 3WIRE SPI Exported Constants
  * @{
  */

#define IS_SPI3WIRE_PERIPH(PERIPH) (PERIPH == SPI2WIRE)


/** @defgroup THREE_WIRE_SPI_mode 3wire SPI mode 
  * @{
  */

#define SPI3WIRE_2WIRE_MODE                     ((uint32_t)(0 << 22))    
#define SPI3WIRE_3WIRE_MODE                     ((uint32_t)(1 << 22))

#define IS_SPI3WIRE_MODE(MODE) ((MODE) == SPI3WIRE_2WIRE_MODE || (MODE) == SPI3WIRE_3WIRE_MODE)

/**
  * @}
  */

/** @defgroup THREE_WIRE_SPI_OE_delay_config 3WIRE SPI OE delay config
  * @{
  */

#define SPI3WIRE_OE_DELAY_1T                     ((uint32_t)(1 << 23))
#define SPI3WIRE_OE_DELAY_NONE                   ((uint32_t)(0 << 23))

#define IS_SPI3WIRE_OE_DELAY_CFG(CFG) ((CFG) == SPI3WIRE_OE_DELAY_1T || (CFG) == SPI3WIRE_OE_DELAY_NONE)

/**
  * @}
  */


/** @defgroup THREE_WIRE_SPI_end_extend_mode 3WIRE SPI end extend mode
  * @{
  */

#define SPI3WIRE_EXTEND_MODE                     ((uint32_t)(1 << 21))
#define SPI3WIRE_NORMAL_MODE                     ((uint32_t)(0 << 21))

#define IS_SPI3WIRE_END_EXTEND_MODE(MODE) ((MODE) == SPI3WIRE_EXTEND_MODE || (MODE) == SPI3WIRE_NORMAL_MODE)

/**
  * @}
  */

/** @defgroup THREE_WIRE_SPI_read_cycle_delay 3WIRE SPI read cycle delay
  * @{
  */

#define IS_SPI3WIRE_READ_CYCLE_DELAY(DELAY) (DELAY <= 0x1f)

/**
  * @}
  */


/** @defgroup THREE_WIRE_SPI_FLAG 3WIRE SPI FLAG
  * @{
  */

#define SPI3WIRE_FLAG_BUSY                       ((uint32_t)(1 << 4))
#define SPI3WIRE_FLAG_INT_IND                    ((uint32_t)(1 << 5))

#define IS_SPI3WIRE_FLAG(FLAG) ((FLAG) == SPI3WIRE_FLAG_BUSY || (FLAG) == SPI3WIRE_FLAG_INT_IND)

/**
  * @}
  */

/** @defgroup THREE_WIRE_SPI_interrupt_definition 3WIRE SPI interrupt definition
  * @{
  */

#define SPI3WIRE_IT_INT                          ((uint32_t)(1 << 0))   //only one interrupt source

#define IS_SPI3WIRE_IT(IT) ((IT) == SPI3WIRE_IT_INT)

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup THREE_WIRE_SPI_Exported_Macros 3WIRE SPI Exported Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup THREE_WIRE_SPI_Exported_Functions 3WIRE SPI Exported Functions
  * @{
  */

void SPI3WIRE_Init(SPI2WIRE_TypeDef* SPI3WIRE, SPI3WIRE_InitTypeDef* SPI3WIRE_InitStruct);
void SPI3WIRE_DeInit(SPI2WIRE_TypeDef* SPI3WIRE);
void SPI3WIRE_StructInit(SPI2WIRE_TypeDef* SPI3WIRE, SPI3WIRE_InitTypeDef* SPI3WIRE_InitStruct);
void SPI3WIRE_ITConfig(SPI2WIRE_TypeDef* SPI3WIRE, uint32_t SPI3WIRE_IT, FunctionalState newState);
FlagStatus SPI3WIRE_GetFlagStatus(SPI2WIRE_TypeDef* SPI3WIRE, uint32_t SPI3WIRE_FLAG);
void SPI3WIRE_ClearITPendingBit(SPI2WIRE_TypeDef* SPI3WIRE, uint32_t SPI3WIRE_IT);
uint16_t SPI3WIRE_GetReadDataNum(SPI2WIRE_TypeDef* SPI3WIRE);
void SPI3WIRE_ClearAllReadData(SPI2WIRE_TypeDef* SPI3WIRE);
void SPI3WIRE_ClearReadNumber(SPI2WIRE_TypeDef* SPI3WIRE);
void SPI3WIRE_StartWrite(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t address, uint8_t data);
void SPI3WIRE_ReadData(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t readNum, uint8_t* outBuf);
void SPI3WIRE_StartSignalRead(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t address);
void SPI3WIRE_StartBurstRead(SPI2WIRE_TypeDef* SPI3WIRE, uint8_t address, uint8_t readNum);

#ifdef __cplusplus
}
#endif

#endif /* _RTL8762_2WIRE_SPI_H_ */

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


