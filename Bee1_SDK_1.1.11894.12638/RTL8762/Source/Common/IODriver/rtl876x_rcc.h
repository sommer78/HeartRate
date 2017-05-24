/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_rcc.h
* @brief     header file of reset and clock control driver..
* @details   
* @author    tifnan_ge
* @date      2015-05-16
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_RCC_H_
#define _RTL876X_RCC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup RCC RCC
  * @brief RCC driver module
  * @{
  */

/** @defgroup RCC_Exported_Types RCC Exported Types
  * @{
  */

/**
  * @}
  */

/** @defgroup RCC_Exported_Constants RCC Exported Constants
  * @{
  */


/** @defgroup RCC_Peripheral_Clock  RCC Peripheral Clock 
  * @{
  */

#define APBPeriph_GPIO_CLOCK              ((uint32_t)((1 << 24) | (0x01 << 28)))
#define APBPeriph_GDMA_CLOCK              ((uint32_t)((1 << 16) | (0x01 << 28)))
#define APBPeriph_TIMER_CLOCK             ((uint32_t)((1 << 14) | (0x01 << 28)))
//#define RCC_APBPeriph_LOG_UART          ((uint32_t)((1 << 12) | (0x01 << 28)))
//#define RCC_APBPeriph_FLASH             ((uint32_t)((1 << 8) | (0x01 << 28)))

#define APBPeriph_IR_CLOCK                ((uint32_t)((1 << 20) | (0x02 << 28)))
#define APBPeriph_SPI1_CLOCK              ((uint32_t)((1 << 18) | (0x02 << 28)))
#define APBPeriph_SPI0_CLOCK              ((uint32_t)((1 << 16) | (0x02 << 28)))
#define APBPeriph_UART_CLOCK              ((uint32_t)((1 << 0) | (0x02 << 28)))

#define APBPeriph_CODEC_CLOCK             ((uint32_t)((1 << 30) | ((uint32_t)1 << 31) | (0x03 << 28)))
#define APBPeriph_ADC_CLOCK               ((uint32_t)((1 << 24) | (0x03 << 28)))
#define APBPeriph_SPI2W_CLOCK             ((uint32_t)((1 << 16) | (0x03 << 28)))
#define APBPeriph_KEYSCAN_CLOCK           ((uint32_t)((1 << 6) | (0x03 << 28)))
#define APBPeriph_QDEC_CLOCK              ((uint32_t)((1 << 4) | (0x03 << 28)))
#define APBPeriph_I2C1_CLOCK              ((uint32_t)((1 << 2) | (0x03 << 28)))
#define APBPeriph_I2C0_CLOCK              ((uint32_t)((1 << 0) | (0x03 << 28)))

#define IS_APB_PERIPH_CLOCK(CLOCK) (((CLOCK) == APBPeriph_GPIO_CLOCK) || ((CLOCK) == APBPeriph_GDMA_CLOCK)\
                            || ((CLOCK) == APBPeriph_TIMER_CLOCK) || ((CLOCK) == APBPeriph_IR_CLOCK)\
                            || ((CLOCK) == APBPeriph_SPI1_CLOCK) || ((CLOCK) == APBPeriph_SPI0_CLOCK)\
                            || ((CLOCK) == APBPeriph_UART_CLOCK) || ((CLOCK) == APBPeriph_ADC_CLOCK)\
                            || ((CLOCK) == APBPeriph_SPI2W_CLOCK) || ((CLOCK) == APBPeriph_KEYSCAN_CLOCK)\
                            || ((CLOCK) == APBPeriph_QDEC_CLOCK) || ((CLOCK) == APBPeriph_I2C1_CLOCK)\
                            || ((CLOCK) == APBPeriph_I2C0_CLOCK) || ((CLOCK) == APBPeriph_CODEC_CLOCK))

/**
  * @}
  */

/** @defgroup APB_Peripheral_Define APB Peripheral Define
  * @{
  */

#define APBPeriph_TIMER                 ((uint32_t)((1 << 16) | (0x00 << 26)))
#define APBPeriph_GDMA                  ((uint32_t)((1 << 13) | (0x00 << 26)))

#define APBPeriph_SPI2W                 ((uint32_t)((1 << 24) | (0x02 << 26)))
#define APBPeriph_KEYSCAN               ((uint32_t)((1 << 19) | (0x02 << 26)))
#define APBPeriph_QDEC                  ((uint32_t)((1 << 18) | (0x02 << 26)))
#define APBPeriph_I2C1                  ((uint32_t)((1 << 17) | (0x02 << 26)))
#define APBPeriph_I2C0                  ((uint32_t)((1 << 16) | (0x02 << 26)))
#define APBPeriph_IR                    ((uint32_t)((1 << 10) | (0x02 << 26)))
#define APBPeriph_SPI1                  ((uint32_t)((1 << 9) | (0x02 << 26)))
#define APBPeriph_SPI0                  ((uint32_t)((1 << 8) | (0x02 << 26)))
#define APBPeriph_UART                  ((uint32_t)((1 << 0) | (0x02 << 26)))

#define APBPeriph_GPIO                  ((uint32_t)((1 << 8) | (0x03 << 26)))
#define APBPeriph_ADC                   ((uint32_t)((1 << 0) | (0x03 << 26)))
#define APBPeriph_CODEC                 ((uint32_t)((1 << 2) | (0x03 << 26)))   /* not actually exist, a reserved address */

#define IS_APB_PERIPH(PERIPH) (((PERIPH) == APBPeriph_TIMER) || ((PERIPH) == APBPeriph_GDMA)\
                            || ((PERIPH) == APBPeriph_SPI2W) || ((PERIPH) == APBPeriph_KEYSCAN)\
                            || ((PERIPH) == APBPeriph_QDEC) || ((PERIPH) == APBPeriph_I2C1)\
                            || ((PERIPH) == APBPeriph_I2C0) || ((PERIPH) == APBPeriph_IR)\
                            || ((PERIPH) == APBPeriph_SPI1) || ((PERIPH) == APBPeriph_SPI0)\
                            || ((PERIPH) == APBPeriph_UART) || ((PERIPH) == APBPeriph_GPIO)\
                            || ((PERIPH) == APBPeriph_ADC) || ((PERIPH) == APBPeriph_CODEC))

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup RCC_Exported_Macros RCC Exported Macros
  * @{
  */
  
extern uint32_t PeriphSelectFlag;

/* mark whcih IO is used */
#define Bit_Periph_ADC          (APBPeriph_ADC & (~(0x03 << 26)))               // (1 << 0)
#define Bit_Periph_UART         ((APBPeriph_UART & (~(0x02 << 26))) << 1)       // (1 << 1)
#define Bit_Periph_CODEC        (APBPeriph_CODEC & (~(0x03 << 26)))             // (1 << 2)
#define Bit_Periph_NA1          (1 << 3)                                        // (1 << 3)
#define Bit_Periph_NA2          (1 << 4)                                        // (1 << 4)
#define Bit_Periph_NA3          (1 << 5)                                        // (1 << 5)
#define Bit_Periph_NA4          (1 << 6)                                        // (1 << 6)
#define Bit_Periph_SPI0         ((APBPeriph_SPI0 & (~(0x02 << 26))) >> 1)       // (1 << 7)
#define Bit_Periph_GPIO         (APBPeriph_GPIO & (~(0x03 << 26)))              // (1 << 8) 
#define Bit_Periph_SPI1         (APBPeriph_SPI1 & (~(0x02 << 26)))              // (1 << 9)
#define Bit_Periph_IR           (APBPeriph_IR & (~(0x02 << 26)))                // (1 << 10)
#define Bit_Periph_NA5          (1 << 11)                                       // (1 << 11)
#define Bit_Periph_NA6          (1 << 12)                                       // (1 << 12)
#define Bit_Periph_GDMA         (APBPeriph_GDMA & (~(0x00 << 26)))              // (1 << 13)
#define Bit_Periph_NA7          (1 << 14)                                       // (1 << 14)
#define Bit_Periph_I2C0         ((APBPeriph_I2C0 & (~(0x02 << 26))) >>1)        // (1 << 15)
#define Bit_Periph_TIMER        (APBPeriph_TIMER & (~(0x00 << 26)))             // (1 << 16)
#define Bit_Periph_I2C1         (APBPeriph_I2C1 & (~(0x02 << 26)))              // (1 << 17)
#define Bit_Periph_QDEC         (APBPeriph_QDEC & (~(0x02 << 26)))              // (1 << 18)
#define Bit_Periph_KEYSCAN      (APBPeriph_KEYSCAN & (~(0x02 << 26)))           // (1 << 19)
#define Bit_Periph_NA8          (1 << 20)                                       // (1 << 20)
#define Bit_Periph_NA9          (1 << 21)                                       // (1 << 21)
#define Bit_Periph_NA10         (1 << 22)                                       // (1 << 22)
#define Bit_Periph_NA11         (1 << 23)                                       // (1 << 23)
#define Bit_Periph_SPI2W        (APBPeriph_SPI2W & (~(0x02 << 26)))             // (1 << 24)
#define Bit_Periph_GDMA_CH0     (1 << 25)                                       // (1 << 25)
#define Bit_Periph_GDMA_CH1     (1 << 26)                                       // (1 << 26)
#define Bit_Periph_GDMA_CH2     (1 << 27)                                       // (1 << 27)
#define Bit_Periph_GDMA_CH3     (1 << 28)                                       // (1 << 28)
#define Bit_Periph_GDMA_CH4     (1 << 29)                                       // (1 << 29)
#define Bit_Periph_GDMA_CH5     (1 << 30)                                       // (1 << 30)
#define Bit_Periph_GDMA_CH6     ((uint32_t)1 << 31)                             // (1 << 31)

/**
  * @}
  */

/** @defgroup RCC_Exported_Functions RCC Exported Functions
  * @{
  */
  
extern void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_RCC_H_ */

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



