/**
****************************************************************************
* @file      mxd2660_spi_master.h
* @brief     peripheral spi master.h
* @author  maxscend\yanping.wang
* @version Initial Draft
* @date     2016/11/18
* @history  
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#ifndef __MXD2660_SPI_MASTER_H__
#define __MXD2660_SPI_MASTER_H__
#include "mxd2660_spi.h"
#include "mxd2660_spi_slave.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/**
* @brief get trans fifo data length
* @param none
* @return The Number Of Bytes In The Receive Buffer
*/
extern uint8 spi0_get_rfifo_cnt(void);
/**
* @brief get trans fifo data length
* @param none
* @return The Number Of Bytes In The Receive Buffer
*/
extern uint8 spi1_get_rfifo_cnt(void);
/**
* @brief get trans fifo data length
* @param none
* @return The Number Of Bytes In The Transmit Buffer
*/
extern uint8 spi0_get_tfifo_cnt(void);
/**
* @brief get trans fifo data length
* @param none
* @return The Number Of Bytes In The Transmit Buffer
*/
extern uint8 spi1_get_tfifo_cnt(void);
/**
 * @brief  initialize peripheral_spi configure
 * @param  transfer_mode communication mode, please see enum SPI_TRX_MODE
 * @param  spi_mode SPI mode, please see enum CPOL_CPHA_MODE
 * @param  clk_freq communication speed
 * @return NONE
 */
extern void spi0_master_init(SPI_TRX_MODE transfer_mode,CPOL_CPHA_MODE spi_mode,SPI_RATE clk_freq);
/**
 * @brief  initialize peripheral_spi configure
 * @param  transfer_mode communication mode, please see enum SPI_TRX_MODE
 * @param  spi_mode SPI mode, please see enum CPOL_CPHA_MODE
 * @param  clk_freq communication speed
 * @return NONE
 */
extern void spi1_master_init(SPI_TRX_MODE transfer_mode,CPOL_CPHA_MODE spi_mode,SPI_RATE clk_freq);
/**
 * @brief  spi master tx only mode tansfer
 * @param  transfer_len buff length
 * @param  *tx_buf  the buffer used to send data
 * @return ture or false
 */
extern bool spi0_master_tx_only(uint16 transfer_len, uint8* tx_buf);
/**
 * @brief  spi master tx only mode tansfer
 * @param  transfer_len buff length
 * @param  *tx_buf  the buffer used to send data
 * @return ture or false
 */
extern bool spi1_master_tx_only(uint16 transfer_len, uint8* tx_buf);
/**
 * @brief  spi0 master rx only mode receive data
 * @param  transfer_len  length
 * @param  *rx_buf  the buffer used to save read data
 * @return ture or false
 */
extern bool spi0_master_rx_only(uint16 transfer_len, uint8* rx_buf);
/**
 * @brief  spi1 master rx only mode receive data
 * @param  transfer_len  length
 * @param  *rx_buf  the buffer used to save read data
 * @return ture or false
 */
extern bool spi1_master_rx_only(uint16 transfer_len, uint8* rx_buf);
/**
 * @brief  spi0 master read and write one byte
 * @param  tx_date  byte to send
 * @return byte to receive
 */
extern uint8 spi0_master_tx_rx_byte(uint8 tx_date);
/**
 * @brief  spi1 master read and write one byte
 * @param  tx_date  byte to send
 * @return byte to receive
 */
extern uint8 spi1_master_tx_rx_byte(uint8 tx_date);
/**
 * @brief  spi0 master read and write function
 * @param  length  read-write length
 * @param  *p_tx_buf  the buffer used to send write data
 * @param  *p_rx_buf  the buffer used to save read data
 * @return  none
 */
extern void spi0_master_trx(uint16 length, uint8* p_tx_buf, uint8* p_rx_buf);
/**
 * @brief  spi1 master read and write function
 * @param  length  read-write length
 * @param  *p_tx_buf  the buffer used to send write data
 * @param  *p_rx_buf  the buffer used to save read data
 * @return  none
 */
extern void spi1_master_trx(uint16 length, uint8* p_tx_buf, uint8* p_rx_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SPI_MASTER_H__ */
