/**
****************************************************************************
* @file mxd2660_spi_slave.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2016/11/18
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#ifndef __MDC2660_SPI_SLAVE_H__
#define __MDC2660_SPI_SLAVE_H__
#include "mxd2660_spi.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct SPI_SLAVE_DATA
{
    uint8 *tx_buf;
    uint16 tx_len;
    uint16 tx_idx;
    uint8 *rx_buf;
    uint16 rx_len;
    uint16 rx_idx;
}SPI_SLAVE_DATA_STRU;

/**
* @brief  SPI0 Slave Interrupt Handler Function
* @param  status SPI0 Status Register
* @return none
*/
extern void SPI0_SLV_IRQHandler(uint32 status);
/**
* @brief  SPI1 Slave Interrupt Handler Function
* @param  status SPI0 Status Register
* @return none
*/
extern void SPI1_SLV_IRQHandler(uint32 status);

/**
* @brief  SPI0 Slave Mode Initialize Function
* @param  transfer_mode SPI Transceiver Mode, see enum SPI_TRX_MODE
* @param  spi_mode SPI Mode, see enum CPOL_CPHA_MODE
* @param  p_tx_buf The Buff Used To Write Data
* @param  p_rx_buf The Buff Used To Read Data
* @param  length The Length Of Bytes Writed and Received
* @return none
*/
extern void spi0_slave_init(SPI_TRX_MODE transfer_mode, CPOL_CPHA_MODE spi_mode, uint8* p_tx_buf, uint8* p_rx_buf, uint16 length);
/**
* @brief  SPI1 Slave Mode Initialize Function
* @param  transfer_mode SPI Transceiver Mode, see enum SPI_TRX_MODE
* @param  spi_mode SPI Mode, see enum CPOL_CPHA_MODE
* @param  p_tx_buf The Buff Used To Write Data
* @param  p_rx_buf The Buff Used To Read Data
* @param  length The Length Of Bytes Writed and Received
* @return none
*/
extern void spi1_slave_init(SPI_TRX_MODE transfer_mode, CPOL_CPHA_MODE spi_mode, uint8* p_tx_buf, uint8* p_rx_buf, uint16 length);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SPI_SLAVE_H__ */
