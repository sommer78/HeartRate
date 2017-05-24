/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_spi.h
* @brief     
* @details   
* @author    elliot chen
* @date      2015-5-6
* @version   v1.1
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTL876X_SPI_H
#define __RTL876X_SPI_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup SPI SPI
  * @{
  */ 

/** @defgroup SPI_Exported_Types SPI Exported Types
  * @{
  */

/** 
  * @brief  SPI Init structure definition  
  */

typedef struct
{
    uint16_t SPI_Direction;         /*!< Specifies the SPI unidirectional or bidirectional data mode.
                                                    This parameter can be a value of @ref SPI_data_direction */

    uint16_t SPI_Mode;              /*!< Specifies the SPI operating mode.
                                                    This parameter can be a value of @ref SPI_mode */

    uint16_t SPI_DataSize;          /*!< Specifies the SPI data size.
                                                    This parameter can be a value of @ref SPI_data_size */

    uint16_t SPI_CPOL;              /*!< Specifies the serial clock steady state.
                                                    This parameter can be a value of @ref SPI_Clock_Polarity */

    uint16_t SPI_CPHA;              /*!< Specifies the clock active edge for the bit capture.
                                                    This parameter can be a value of @ref SPI_Clock_Phase */
 
    uint32_t SPI_ClockSpeed;        /*!< Specifies SCK clock.
                                                    @note The communication clock is derived from the master
                                                    clock. The slave clock does not need to be set. */
    uint16_t SPI_FrameFormat;

    uint32_t SPI_TxThresholdLevel;  /* !< Specifies the transmit FIFO Threshold */
    
    uint32_t SPI_RxThresholdLevel;  /* !<Specifies the receive FIFO Threshold */
    
    uint32_t SPI_NDF;               /*!< Specifies the trigger condition in EEPROM mode. 
                                                    This parameter should be the value of the length of read data -1*/

} SPI_InitTypeDef;

/**
  * @}
  */

/** @defgroup SPI_Exported_Constants SPI Exported Constants
  * @{
  */
  
#define IS_SPI_ALL_PERIPH(PERIPH) (((PERIPH) == SPI0) || \
                                   ((PERIPH) == SPI1))
  
/** @defgroup SPI_clock_speed SPI Clock Speed 
  * @{
  */
  
#define IS_SPI_CLOCK_SPEED(SPEED) (((SPEED) >= 0x01) && ((SPEED) <= 40000000))
/**
  * @}
  */

/** @defgroup SPI_data_direction SPI Data Direction 
  * @{
  */

#define SPI_Direction_FullDuplex        ((uint16_t)0x0000)
#define SPI_Direction_TxOnly            ((uint16_t)0x0001)
#define SPI_Direction_RxOnly            ((uint16_t)0x0002)
#define SPI_Direction_EEPROM            ((uint16_t)0x0003)

#define IS_SPI_DIRECTION_MODE(MODE) (((MODE) == SPI_Direction_FullDuplex) || \
                                     ((MODE) == SPI_Direction_RxOnly) || \
                                     ((MODE) == SPI_Direction_TxOnly) || \
                                     ((MODE) == SPI_Direction_EEPROM))
/**
  * @}
  */
  
/** @defgroup SPI_mode SPI Mode 
  * @{
  */
  
#define SPI_Mode_Master                         ((uint16_t)0x0104)
#define SPI_Mode_Slave                          ((uint16_t)0x0000)  
#define IS_SPI_MODE(MODE) (((MODE) == SPI_Mode_Master) || \
                           ((MODE) == SPI_Mode_Slave))
/**
  * @}
  */

/** @defgroup SPI_data_size SPI Data Size 
  * @{
  */

#define SPI_DataSize_4b                 ((uint16_t)0x0003)
#define SPI_DataSize_5b                 ((uint16_t)0x0004)
#define SPI_DataSize_6b                 ((uint16_t)0x0005)
#define SPI_DataSize_7b                 ((uint16_t)0x0006)
#define SPI_DataSize_8b                 ((uint16_t)0x0007)
#define SPI_DataSize_9b                 ((uint16_t)0x0008)
#define SPI_DataSize_10b                 ((uint16_t)0x0009)
#define SPI_DataSize_11b                 ((uint16_t)0x000a)
#define SPI_DataSize_12b                 ((uint16_t)0x000b)
#define SPI_DataSize_13b                 ((uint16_t)0x000c)
#define SPI_DataSize_14b                 ((uint16_t)0x000d)
#define SPI_DataSize_15b                 ((uint16_t)0x000e)
#define SPI_DataSize_16b                 ((uint16_t)0x000f)
#define IS_SPI_DATASIZE(DATASIZE) (((DATASIZE) == SPI_DataSize_4b)  || \
                                   ((DATASIZE) == SPI_DataSize_5b)  || \
                                   ((DATASIZE) == SPI_DataSize_6b)  || \
                                   ((DATASIZE) == SPI_DataSize_7b)  || \
                                   ((DATASIZE) == SPI_DataSize_8b)  || \
                                   ((DATASIZE) == SPI_DataSize_9b)  || \
                                   ((DATASIZE) == SPI_DataSize_10b) || \
                                   ((DATASIZE) == SPI_DataSize_11b) || \
                                   ((DATASIZE) == SPI_DataSize_12b) || \
                                   ((DATASIZE) == SPI_DataSize_13b) || \
                                   ((DATASIZE) == SPI_DataSize_14b) || \
                                   ((DATASIZE) == SPI_DataSize_15b) || \
                                   ((DATASIZE) == SPI_DataSize_16b))
/**
  * @}
  */

/** @defgroup SPI_Clock_Polarity SPI Clock Polarity 
  * @{
  */

#define SPI_CPOL_Low                    ((uint16_t)0x0000)
#define SPI_CPOL_High                   ((uint16_t)0x0001)
#define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_CPOL_Low) || \
                           ((CPOL) == SPI_CPOL_High))
/**
  * @}
  */

/** @defgroup SPI_Clock_Phase SPI Clock Phase 
  * @{
  */

#define SPI_CPHA_1Edge                  ((uint16_t)0x0000)
#define SPI_CPHA_2Edge                  ((uint16_t)0x0001)
#define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_CPHA_1Edge) || \
                           ((CPHA) == SPI_CPHA_2Edge))
/**
  * @}
  */

/** @defgroup SPI_frame_format SPI Frame Format 
  * @{
  */

#define SPI_Frame_Motorola              ((uint16_t)0x0000)
#define SPI_Frame_TI_SSP                ((uint16_t)0x0001)
#define SPI_Frame_NS_MICROWIRE          ((uint16_t)0x0002)
#define SPI_Frame_Reserve               ((uint16_t)0x0003)
#define IS_SPI_FRAME_FORMAT(FRAME) (((FRAME) == SPI_Frame_Motorola) || \
                                     ((FRAME) == SPI_Frame_TI_SSP) || \
                                     ((FRAME) == SPI_Frame_NS_MICROWIRE) || \
                                     ((FRAME) == SPI_Frame_Reserve))
/**
  * @}
  */

/** @defgroup SPI_flags_definition SPI Flags Definition 
  * @{
  */
  
#define SPI_FLAG_BUSY                   ((uint16_t)0x0001)
#define SPI_FLAG_TFNF                   ((uint16_t)0x0002)
#define SPI_FLAG_TFE                    ((uint16_t)0x0004)
#define SPI_FLAG_RFNE                   ((uint16_t)0x0008)
#define SPI_FLAG_RFF                    ((uint16_t)0x0010)
#define SPI_FLAG_TXE                    ((uint16_t)0x0020)
#define SPI_FLAG_DCOL                   ((uint16_t)0x0040)
#define IS_SPI_GET_FLAG(FLAG)   (((FLAG) == SPI_FLAG_DCOL) || ((FLAG) == SPI_FLAG_TXE) || \
                                   ((FLAG) == SPI_FLAG_RFF) || ((FLAG) == SPI_FLAG_RFNE) || \
                                   ((FLAG) == SPI_FLAG_TFE) || ((FLAG) == SPI_FLAG_TFNF) || \
                                   ((FLAG) == SPI_FLAG_BUSY))
/**
  * @}
  */                                   

/** @defgroup SPI_interrupts_definition SPI Interrupts Definition 
  * @{
  */

#define SPI_INT_TXE                  ((uint8_t)BIT(0))
#define SPI_INT_TXO                  ((uint8_t)BIT(1))
#define SPI_INT_RXU                  ((uint8_t)BIT(2))
#define SPI_INT_RXO                  ((uint8_t)BIT(3))
#define SPI_INT_RXF                  ((uint8_t)BIT(4))
#define SPI_INT_MST                  ((uint8_t)BIT(5))
#define IS_SPI_CONFIG_IT(IT) (((IT) == SPI_INT_TXE) || \
                                 ((IT) == SPI_INT_TXO) || \
                                 ((IT) == SPI_INT_RXU) || \
                                 ((IT) == SPI_INT_RXO) || \
                                 ((IT) == SPI_INT_RXF) || \
                                 ((IT) == SPI_INT_MST))
/**
  * @}
  */
  
/** @defgroup SPI_GDMA_transfer_requests  SPI GDMA transfer requests
  * @{
  */
	
#define SPI_GDMAReq_Tx               ((uint16_t)0x0002)
#define SPI_GDMAReq_Rx               ((uint16_t)0x0001)
#define IS_SPI_GDMAREQ(GDMAREQ) ((((GDMAREQ) & (uint16_t)0xFFFC) == 0x00) && ((GDMAREQ) != 0x00))

	/**
	  * @}
	  */


/**
  * @}
  */

/** @defgroup SPI_Exported_functions SPI Exported Functions
 * @{
 */
 
void SPI_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_SendBuffer(SPI_TypeDef* SPIx, UINT8 *pBuf, UINT16 len);
void SPI_INTConfig(SPI_TypeDef* SPIx, uint8_t SPI_IT, FunctionalState NewState);
void SPI_ClearINTPendingBit(SPI_TypeDef* SPIx, uint16_t SPI_IT);
__STATIC_INLINE void SPI_SendData(SPI_TypeDef* SPIx, uint16_t Data);
__STATIC_INLINE uint16_t SPI_ReceiveData(SPI_TypeDef* SPIx);
__STATIC_INLINE uint8_t SPI_GetRxFIFOLen(SPI_TypeDef* SPIx);
__STATIC_INLINE uint8_t SPI_GetTxFIFOLen(SPI_TypeDef* SPIx);
__STATIC_INLINE void SPI_ChangeDirection(SPI_TypeDef* SPIx, uint16_t dir);
__STATIC_INLINE void SPI_SetReadLen(SPI_TypeDef* SPIx, uint16_t len);
__STATIC_INLINE void SPI_SetCSNumber(SPI_TypeDef* SPIx, uint8_t number);
__STATIC_INLINE FlagStatus SPI_GetFlagState(SPI_TypeDef* SPIx, uint8_t SPI_FLAG);
__STATIC_INLINE ITStatus SPI_GetINTStatus(SPI_TypeDef* SPIx, uint32_t SPI_IT);
__STATIC_INLINE void SPI_GDMACmd(SPI_TypeDef* SPIx, uint16_t SPI_GDMAReq, FunctionalState NewState);


/**
  * @brief  Transmits a Data through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @param  Data : Data to be transmitted.
  * @retval None
  */
__STATIC_INLINE void SPI_SendData(SPI_TypeDef* SPIx, uint16_t Data)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    
    SPIx->DR[0] = Data;
    while(!(SPIx->SR & BIT(1)));
}

/**
  * @brief  Returns the most recent received data by the SPIx/I2Sx peripheral. 
  * @param  SPIx: where x can be 0 or 1
  * @retval The value of the received data.
  */
__STATIC_INLINE uint16_t SPI_ReceiveData(SPI_TypeDef* SPIx)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    
    return (UINT16)SPIx->DR[0];
}

/**
  * @brief  read  data length in Rx FIFO through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @retval None
  */
__STATIC_INLINE uint8_t SPI_GetRxFIFOLen(SPI_TypeDef* SPIx)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    return (uint8_t)SPIx->RXFLR;
}

/**
  * @brief  read data length in Tx FIFO through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @retval None
  */
__STATIC_INLINE uint8_t SPI_GetTxFIFOLen(SPI_TypeDef* SPIx)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    
    return (uint8_t)SPIx->TXFLR;
}

/**
 *@brief change SPI direction mode.
 *@param SPIx: where x can be 0 or 1
 *@param dir: parameter of direction mode
 *@retval None
 */
__STATIC_INLINE void SPI_ChangeDirection(SPI_TypeDef* SPIx, uint16_t dir)
{
	/* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_SPI_DIRECTION_MODE(dir));

	/* Disable the selected SPI peripheral */
    SPIx->SSIENR &= ~0x01;  
	
    /* Change SPI direction mode */
    SPIx->CTRLR0 &= ~(0x03<<8);
	SPIx->CTRLR0 |= dir<<8;
	
    /* Enable the selected SPI peripheral */
    SPIx->SSIENR |= 0x01;   
}

/**
  * @brief  set read Data length only in EEPROM mode through the SPIx peripheral,which 
    enables you to receive up to 64 KB of data in a continuous transfer.
  * @param  SPIx: where x can be 0 or 1
  * @param  len : length of read data which can be 1 to 65536.
  * @retval None
  */
__STATIC_INLINE void SPI_SetReadLen(SPI_TypeDef* SPIx, uint16_t len)
{
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    
    /* Disable the selected SPI peripheral */
    SPIx->SSIENR &= ~0x01;  
    /* set read length in SPI EEPROM mode */
    SPIx->CTRLR1 = len - 1;
    /* Enable the selected SPI peripheral */
    SPIx->SSIENR |= 0x01;   
}

/**
  * @brief  set cs number through the SPIx peripheral.
  * @param  SPIx: where x can be 0 or 1
  * @param  number:if SPIx is SPI0, number must be 0. if SPIx is SPI1, number can be 0 to 2.
  * @retval None
  */
__STATIC_INLINE void SPI_SetCSNumber(SPI_TypeDef* SPIx, uint8_t number)
{
	/* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    
    /* set cs number */
    SPIx->SER = BIT(number);
}

/**
  * @brief  Checks whether the specified SPI interrupt is set or not.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_IT: specifies the SPI interrupt to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI_INT_MST: Multi-Master Contention Interrupt.
  *     @arg SPI_INT_RXF: Receive FIFO Full Interrupt.
  *     @arg SPI_INT_RXO: Receive FIFO Overflow Interrupt.
  *     @arg SPI_INT_RXU: Receive FIFO Underflow Interrupt.
  *     @arg SPI_INT_TXO: Transmit FIFO Overflow Interrupt .
  *     @arg SPI_INT_TXE: Transmit FIFO Empty Interrupt.
  * @retval The new state of SPI_IT (SET or RESET).
  */
__STATIC_INLINE ITStatus SPI_GetINTStatus(SPI_TypeDef* SPIx, uint32_t SPI_IT)
{
	ITStatus bit_status = RESET;
	
	/* Check the parameters */
	assert_param(IS_SPI_ALL_PERIPH(SPIx));
	assert_param(IS_SPI_CONFIG_IT(SPI_IT));

	if((SPIx->ISR & SPI_IT) != (uint32_t)RESET)
	{
		bit_status = SET;
	}
    
    /* Return the SPI_IT status */
    return  bit_status;	
}

/**
  * @brief  Checks whether the specified SPI flag is set or not.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_FLAG: specifies the SPI flag to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI_FLAG_DCOL: Data Collision Error flag.Set if it is actively transmitting in master mode when another master selects this device as a slave.
  *     @arg SPI_FLAG_TXE: Transmission error flag.Set if the transmit FIFO is empty when a transfer is started in slave mode.
  *     @arg SPI_FLAG_RFF: Receive FIFO full flag. Set if the receive FIFO is completely full.
  *     @arg SPI_FLAG_RFNE: Receive FIFO Not Empty flag.Set if receive FIFO is not empty.
  *     @arg SPI_FLAG_TFE: Transmit FIFO Empty flag.Set if transmit FIFO is empty.
  *     @arg SPI_FLAG_TFNF: Transmit FIFO Not Full flag.Set if transmit FIFO is not full.
  *     @arg SPI_FLAG_BUSY: SPI Busy flag.Set if it is actively transferring data.reset if it is idle or disabled.
  * @retval The new state of SPI_FLAG (SET or RESET).
  */
__STATIC_INLINE FlagStatus SPI_GetFlagState(SPI_TypeDef* SPIx, uint8_t SPI_FLAG)
{
    FlagStatus bitstatus = RESET;
    
    /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_SPI_GET_FLAG(SPI_FLAG));

    /* Check the status of the specified SPI flag */
    if ((SPIx->SR & SPI_FLAG) != (uint8_t)RESET)
    {
        /* SPI_FLAG is set */
        bitstatus = SET;
    }
    
    /* Return the SPI_FLAG status */
    return  bitstatus;
}

/**
  * @brief  Enables or disables the SPIx GDMA interface.
  * @param  SPIx: where x can be 0 or 1
  * @param  SPI_GDMAReq: specifies the SPI GDMA transfer request to be enabled or disabled.
  *   This parameter can be one of the following values:
  *     @arg SPI_GDMAReq_Tx: Tx buffer DMA transfer request
  *     @arg SPI_GDMAReq_Rx: Rx buffer DMA transfer request
  * @param  NewState: new state of the selected SPI GDMA transfer request.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void SPI_GDMACmd(SPI_TypeDef* SPIx, uint16_t SPI_GDMAReq, FunctionalState NewState)
{
	 /* Check the parameters */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_SPI_GDMAREQ(SPI_GDMAReq));

    if (NewState != DISABLE)
    {
        /* Enable the selected SPI GDMA request */
        SPIx->DMACR |= SPI_GDMAReq;
    }
    else
    {
        /* Disable the selected SPI GDMA request */
        SPIx->DMACR &= (uint16_t)~(SPI_GDMAReq);
    }

}


#ifdef __cplusplus
}
#endif

#endif /*__RTL876X_SPI_H*/

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

