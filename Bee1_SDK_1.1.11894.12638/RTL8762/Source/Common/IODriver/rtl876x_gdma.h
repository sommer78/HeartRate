/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_gdma.h
* @brief     
* @details   
* @author    elliot chen
* @date      2015-05-08
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __RTL876X_GDMA_H
#define __RTL876X_GDMA_H

#ifdef __cpluspuls
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup GDMA GDMA
  * @{
  */

/** @defgroup GDMA_Exported_Types GDMA Exported Types
  * @{
  */

/** 
  * @brief  GDMA Init structure definition
  */
typedef struct
{
    uint8_t GDMA_ChannelNum;         /*!< Specifies channel number for GDMA which from 0 to 5. */
    
    uint8_t GDMA_DIR;                /*!< Specifies if the peripheral is the source or destination.
                                                    This parameter can be a value of @ref GDMA_data_transfer_direction */

    uint32_t GDMA_BufferSize;        /*!< Specifies the buffer size(<=4095), in data unit, of the specified Channel. 
                                                    The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                                    or DMA_MemoryDataSize members depending in the transfer direction. */

    uint8_t GDMA_SourceInc;          /*!< Specifies whether the source address register is incremented or not.
                                                    This parameter can be a value of @ref GDMA_source_incremented_mode */

    uint8_t GDMA_DestinationInc;     /*!< Specifies whether the destination address register is incremented or not.
                                                    This parameter can be a value of @ref DMA_destination_incremented_mode */

    uint32_t GDMA_SourceDataSize;    /*!< Specifies the source data width.
                                                    This parameter can be a value of @ref GDMA_source_data_size */

    uint32_t GDMA_DestinationDataSize;/*!< Specifies the Memory data width.
                                                    This parameter can be a value of @ref GDMA_destination_data_size */
                                                        
    uint32_t GDMA_SourceMsize;      /*!< Specifies the number of data items to be transferred.
                                                    This parameter can be a value of @ref GDMA_source_Msize */

    uint32_t GDMA_DestinationMsize; /*!< Specifies  the number of data items to be transferred.
                                                    This parameter can be a value of @ref GDMA_destinationMsize */  

    uint32_t GDMA_SourceAddr;       /*!< Specifies the source base address for GDMA Channelx. */

    uint32_t GDMA_DestinationAddr;  /*!< Specifies the destination base address for GDMA Channelx. */

    uint32_t GDMA_TransferType;      /*!< Specifies the transfer type for GDMA Channelx. */
    
    uint32_t GDMA_ChannelPriority;   /*!< Specifies the software priority for the DMAy Channelx.
                                                    This parameter can be a value of @ref DMA_priority_level */
}GDMA_InitTypeDef;

/**
  * @}
  */

/** @defgroup GDMA_Exported_Constants GDMA Exported Constants
  * @{
  */

#define IS_GDMA_ALL_PERIPH(PERIPH) (((PERIPH) == GDMA_Channel0) || \
                                   ((PERIPH) == GDMA_Channel1) || \
                                   ((PERIPH) == GDMA_Channel2) || \
                                   ((PERIPH) == GDMA_Channel3) || \
                                   ((PERIPH) == GDMA_Channel4) || \
                                   ((PERIPH) == GDMA_Channel5))
#define IS_GDMA_ChannelNum(NUM) ((NUM) < 6)


/** @defgroup GDMA_source_data_size GDMA Source Data Size 
  * @{
  */

#define GDMA_DataSize_Byte            ((uint32_t)0x00000000)
#define GDMA_DataSize_HalfWord        ((uint32_t)0x00000001)
#define GDMA_DataSize_Word            ((uint32_t)0x00000002)
#define IS_GDMA_DATA_SIZE(SIZE) (((SIZE) == GDMA_DataSize_Byte) || \
                                       ((SIZE) == GDMA_DataSize_HalfWord) || \
                                       ((SIZE) == GDMA_DataSize_Word))
/**
  * @}
  */

/** @defgroup GDMA_Msize GDMA Msize 
  * @{
  */    
  
#define GDMA_Msize_1            ((uint32_t)0x00000000)
#define GDMA_Msize_4            ((uint32_t)0x00000001)
#define GDMA_Msize_8            ((uint32_t)0x00000002)
#define GDMA_Msize_16           ((uint32_t)0x00000003)
#define GDMA_Msize_32           ((uint32_t)0x00000004)
#define GDMA_Msize_64           ((uint32_t)0x00000005)
#define GDMA_Msize_128          ((uint32_t)0x00000006)
#define GDMA_Msize_256          ((uint32_t)0x00000007)
#define IS_GDMA_MSIZE(SIZE) (((SIZE) == GDMA_Msize_1) || \
                                ((SIZE) == GDMA_Msize_4) || \
                                ((SIZE) == GDMA_Msize_8) || \
                                ((SIZE) == GDMA_Msize_16) || \
                                ((SIZE) == GDMA_Msize_32) || \
                                ((SIZE) == GDMA_Msize_64) || \
                                ((SIZE) == GDMA_Msize_128) || \
                                ((SIZE) == GDMA_Msize_256))

/**
  * @}
  */

/** @defgroup GDMA_transfer_Type GDMA Transfer Type 
  * @{
  */
#define GDMA_TransferType_UART0_TX          ((uint32_t)0x00000000)
#define GDMA_TransferType_UART0_RX          ((uint32_t)0x00000001)
#define GDMA_TransferType_SPI0_TX           ((uint32_t)(0x00000004 << 4))
#define GDMA_TransferType_SPI0_RX           ((uint32_t)(0x00000005 << 4))
#define GDMA_TransferType_SPI1_TX           ((uint32_t)(0x00000006 << 8))
#define GDMA_TransferType_SPI1_RX           ((uint32_t)(0x00000007 << 8))
#define GDMA_TransferType_I2C0_TX           ((uint32_t)(0x00000008 << 12))
#define GDMA_TransferType_I2C0_RX           ((uint32_t)(0x00000009 << 12))
#define GDMA_TransferType_I2C1_TX           ((uint32_t)(0x0000000a << 16))
#define GDMA_TransferType_I2C1_RX           ((uint32_t)(0x0000000b << 16))
#define GDMA_TransferType_ADC               ((uint32_t)(0x0000000c << 20))
#define GDMA_TransferType_Other             ((uint32_t)(0x0000000d << 24))

#define IS_GDMA_TransferType(Type) (((Type) == GDMA_TransferType_UART0_TX) || \
                                           ((Type) == GDMA_TransferType_UART0_RX) || \
                                           ((Type) == GDMA_TransferType_SPI0_TX) || \
                                           ((Type) == GDMA_TransferType_SPI0_RX) || \
                                           ((Type) == GDMA_TransferType_SPI1_TX) || \
                                           ((Type) == GDMA_TransferType_SPI1_RX) || \
                                           ((Type) == GDMA_TransferType_I2C0_TX) || \
                                           ((Type) == GDMA_TransferType_I2C0_RX) || \
                                           ((Type) == GDMA_TransferType_I2C1_TX) || \
                                           ((Type) == GDMA_TransferType_I2C1_RX) || \
                                           ((Type) == GDMA_TransferType_ADC))
/**
  * @}
  */

/** @defgroup GDMA_data_transfer_direction GDMA Data Transfer Direction 
  * @{
  */

#define GDMA_DIR_MemoryToMemory              ((uint32_t)0x00000000)
#define GDMA_DIR_MemoryToPeripheral          ((uint32_t)0x00000001)
#define GDMA_DIR_PeripheralToMemory          ((uint32_t)0x00000002)
#define GDMA_DIR_PeripheralToPeripheral      ((uint32_t)0x00000003)

#define IS_GDMA_DIR(DIR) (((DIR) == GDMA_DIR_MemoryToMemory) || \
                          ((DIR) == GDMA_DIR_MemoryToPeripheral) || \
                          ((DIR) == GDMA_DIR_PeripheralToMemory) ||\
                          ((DIR) == GDMA_DIR_PeripheralToPeripheral))

/**
  * @}
  */

/** @defgroup GDMA_source_incremented_mode GDMA Source Incremented Mode 
  * @{
  */

#define DMA_SourceInc_Inc          ((uint32_t)0x00000000)
#define DMA_SourceInc_Dec          ((uint32_t)0x00000001)
#define DMA_SourceInc_Fix          ((uint32_t)0x00000002)

#define IS_GDMA_SourceInc(STATE) (((STATE) == DMA_SourceInc_Inc) || \
                                            ((STATE) == DMA_SourceInc_Dec) || \
                                            ((STATE) == DMA_SourceInc_Fix))
/**
  * @}
  */

/** @defgroup GDMA_destination_incremented_mode GDMA Destination Incremented Mode 
  * @{
  */

#define DMA_DestinationInc_Inc          ((uint32_t)0x00000000)
#define DMA_DestinationInc_Dec          ((uint32_t)0x00000001)
#define DMA_DestinationInc_Fix          ((uint32_t)0x00000002)

#define IS_GDMA_DestinationInc(STATE) (((STATE) == DMA_DestinationInc_Inc) || \
                                            ((STATE) == DMA_DestinationInc_Dec) || \
                                            ((STATE) == DMA_DestinationInc_Fix))
/**
  * @}
  */

/** @defgroup DMA_interrupts_definition DMA Interrupts Definition 
  * @{
  */

#define GDMA_INT_Transfer                           ((uint32_t)0x00000001)
#define GDMA_INT_Block                              ((uint32_t)0x00000002)
#define GDMA_INT_SrcTransfer                        ((uint32_t)0x00000004)
#define GDMA_INT_DstTransfer                        ((uint32_t)0x00000008)
#define GDMA_INT_Error                              ((uint32_t)0x000000010)
#define IS_GDMA_CONFIG_IT(IT) ((((IT) & 0xFFFFFFF1) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */

/** @defgroup DMA_interrupts_definition DMA Interrupts Definition 
  * @{
  */

#define GDMA_SUSPEND_TRANSMISSSION					(BIT(8))
#define GDMA_FIFO_STATUS							(BIT(9))

#define IS_GDMA_CONFIG_IT(IT) ((((IT) & 0xFFFFFFF1) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup GDMA_Exported_Functions GDMA Exported Functions
  * @{
  */

void GDMA_DeInit(void);
void GDMA_Init(GDMA_ChannelTypeDef* GDMA_Channelx, GDMA_InitTypeDef* GDMA_InitStruct);
void GDMA_StructInit(GDMA_InitTypeDef* GDMA_InitStruct);
void GDMA_Cmd(uint8_t GDMA_ChannelNum, FunctionalState NewState);
void GDMA_INTConfig(uint8_t GDMA_ChannelNum, uint32_t GDMA_IT, FunctionalState NewState);
void GDMA_ClearINTPendingBit(uint8_t GDMA_ChannelNum, uint32_t GDMA_IT);
__STATIC_INLINE FlagStatus GDMA_GetChannelStatus(uint8_t GDMA_Channel_Num);
__STATIC_INLINE ITStatus GDMA_GetTransferINTStatus(uint8_t GDMA_Channel_Num);
__STATIC_INLINE void GDMA_ClearAllTypeINT(uint8_t GDMA_Channel_Num);
__STATIC_INLINE void GDMA_SetSourceAddress(GDMA_ChannelTypeDef* GDMA_Channelx, uint32_t Address);
__STATIC_INLINE void GDMA_SetDestinationAddress(GDMA_ChannelTypeDef* GDMA_Channelx, uint32_t Address);
__STATIC_INLINE void GDMA_SetBufferSize(GDMA_ChannelTypeDef* GDMA_Channelx, uint32_t size);
__STATIC_INLINE uint32_t GDMA_GetCurrentSourceAddress(GDMA_ChannelTypeDef* GDMA_Channelx);
__STATIC_INLINE uint32_t GDMA_GetCurrentDestinationAddress(GDMA_ChannelTypeDef* GDMA_Channelx);
__STATIC_INLINE void GDMA_SuspendCmd(GDMA_ChannelTypeDef* GDMA_Channelx, FunctionalState NewState);
__STATIC_INLINE FlagStatus GDMA_GetFIFOStatus(GDMA_ChannelTypeDef* GDMA_Channelx);

/**
  * @brief  Checks selected GDMA Channel status.
  * @param  GDMA_Channel_Num: GDMA channel number.
  * @return GDMA channel status: SET: channel is be used, RESET: channel is free.
  */
__STATIC_INLINE FlagStatus GDMA_GetChannelStatus(uint8_t GDMA_Channel_Num)
{
	FlagStatus bit_status = RESET;
	
	/* Check the parameters */
    assert_param(IS_GDMA_ChannelNum(GDMA_Channel_Num));

    if((GDMA_BASE->ChEnReg & BIT(GDMA_Channel_Num)) != (uint32_t)RESET)
    {
    
        bit_status = SET;
    }
    
    /* Return the selected channel status */
    return  bit_status;
}

/**
  * @brief  Checks GDMA Channel transfer interrupt.
  * @param  GDMA_Channel_Num: GDMA channel number.
  * @return transfer type interrupt status value. 
  */
__STATIC_INLINE ITStatus GDMA_GetTransferINTStatus(uint8_t GDMA_Channel_Num)
{
	ITStatus bit_status = RESET;
	
	/* Check the parameters */
    assert_param(IS_GDMA_ChannelNum(GDMA_Channel_Num));

    if((GDMA_BASE->STATUS_TFR & BIT(GDMA_Channel_Num)) != (uint32_t)RESET)
    {
    
        bit_status = SET;
    }
    
    /* Return the transfer interrupt status */
    return  bit_status;
}

/**
  * @brief  clear the GDMA Channelx all interrupt.
  * @param  GDMA_Channelx: where x can be 0 to 5  to select the GDMA Channel.
  * @retval None
  */
__STATIC_INLINE void GDMA_ClearAllTypeINT(uint8_t GDMA_Channel_Num)
{   
    /* Check the parameters */
    assert_param(IS_GDMA_ChannelNum(GDMA_Channel_Num));
    
    GDMA_BASE->CLEAR_TFR = BIT(GDMA_Channel_Num);
    GDMA_BASE->CLEAR_BLOCK = BIT(GDMA_Channel_Num);
    GDMA_BASE->CLEAR_DST_TRAN = BIT(GDMA_Channel_Num);
    GDMA_BASE->CLEAR_SRC_TRAN = BIT(GDMA_Channel_Num);
    GDMA_BASE->CLEAR_ERR = BIT(GDMA_Channel_Num);
}

/**
  * @brief  set GDMA source address .
  * @param  GDMA_Channelx: where x can be 0 to 5  to select the GDMA Channel.
  * @retval None
  */
__STATIC_INLINE void GDMA_SetSourceAddress(GDMA_ChannelTypeDef* GDMA_Channelx, uint32_t Address)
{
	/* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
	
	GDMA_Channelx->SAR = Address;
}

/**
  * @brief  set GDMA destination address .
  * @param  GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  * @param Adderss: destination address.
  * @retval None
  */
__STATIC_INLINE void GDMA_SetDestinationAddress(GDMA_ChannelTypeDef* GDMA_Channelx, uint32_t Address)
{
	/* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
	
	GDMA_Channelx->DAR = Address;
}

/**
  *@brief set GDMA buffer size.
  *@param GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  *@param buffer_size: set size of GDMA_BufferSize.
  *@retval None. 
  */
__STATIC_INLINE void GDMA_SetBufferSize(GDMA_ChannelTypeDef* GDMA_Channelx, uint32_t buffer_size)
{
	/* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
	
	/* configure high 32 bit of CTL register */
    GDMA_Channelx->CTL_HIGH = buffer_size;
}

/**
  *@brief Get current source address.
  *@param GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  *@retval current source address. 
  */
__STATIC_INLINE uint32_t GDMA_GetCurrentSourceAddress(GDMA_ChannelTypeDef* GDMA_Channelx)
{
	/* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
	
    return GDMA_Channelx->SAR;
}

/**
  *@brief Get current destination address.
  *@param GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  *@retval current destination address. 
  */
__STATIC_INLINE uint32_t GDMA_GetCurrentDestinationAddress(GDMA_ChannelTypeDef* GDMA_Channelx)
{
	/* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
	
    return GDMA_Channelx->DAR;
}

/**
  *@brief  Suspend GDMA transmission from the source.Please check GDMA FIFO empty to guarnatee without losing data.
  *@param GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  *@param  NewState: new state of the DMA Channelx. 
  *   This parameter can be: ENABLE or DISABLE.
  *@retval None. 
  */
__STATIC_INLINE void GDMA_SuspendCmd(GDMA_ChannelTypeDef* GDMA_Channelx, FunctionalState NewState)
{
	/* Check the parameters */  
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Not suspend transmission*/
        GDMA_Channelx->CFG_LOW &= ~(GDMA_SUSPEND_TRANSMISSSION);
    }
    else
    {
        /* Suspend transmission */
        GDMA_Channelx->CFG_LOW |= GDMA_SUSPEND_TRANSMISSSION;
    }
}

/**
  *@brief  Check GDMA FIFO status.
  *@param GDMA_Channelx: where x can be 0 to 5 to select the GDMA Channel.
  *@return GDMA FIFO status: SET: not empty, RESET: empty.
  */
__STATIC_INLINE FlagStatus GDMA_GetFIFOStatus(GDMA_ChannelTypeDef* GDMA_Channelx)
{
	FlagStatus bit_status = RESET;
	
	/* Check the parameters */
    assert_param(IS_GDMA_ALL_PERIPH(GDMA_Channelx));

    if((GDMA_Channelx->CFG_LOW & GDMA_FIFO_STATUS) != (uint32_t)RESET)
    {
    
        bit_status = SET;
    }
    
    /* Return the selected channel status */
    return  bit_status;
}

#ifdef __cplusplus
}
#endif

#endif /*__RTL8762X_GDMA_H*/

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

