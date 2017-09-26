/**
****************************************************************************
* @file    mxd2660_7816.h
* @brief   
* @author  wanli.han
* @version Initial Draft
* @date    2016/11/14
* @history
* @note    
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#ifndef __MXD2660_7816_H__
#define __MXD2660_7816_H__


#include "system_config.h"
#include "mxdaqua.h"
#include "mxd2660_dma.h"
#include "mxd2660_gpio.h"


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


typedef enum
{
	_7816_RESET_COLD = 0, /**< reset 7816 in cold mode */
	_7816_RESET_WARM = 1, /**< reset 7816 in warm mode*/
}_7816_RESET_MODE;	

typedef enum
{
	_7816_CLK_1M = 0,  /**< 7816 clk 1M */
	_7816_CLK_2M = 1,  /**< 7816 clk 2M */
	_7816_CLK_4M = 2,  /**< 7816 clk 4M */
}_7816_CLK_RATE;

typedef enum
{
	_7816_PARITY_ODD = 0,   /**< parity odd */
	_7816_PARITY_EVEN = 1,  /**< parity even */
}_7816_PARITY;

typedef enum
{
	_7816_LSB = 0,   /**< little endian */
	_7816_MSB = 1,   /**< bigger endian */
}_7816_ENDIAN_MODE;

/**
* @brief  use deault value to initialize 7816
* @param  none
* @return none
*/
extern void _7816_init_default(void);

/**
* @brief  configure 7816 clock
* @param  clk see enum _7816_CLK_RATE
* @return none
*/
extern void _7816_set_clk(_7816_CLK_RATE clk);

/**
* @brief  reset 7816
* @param  reset_mode see _7816_RESET_MODE
* @return none
*/
extern void _7816_reset(_7816_RESET_MODE mode);
/**
* @brief  deinitialize 7816, pull clk rst io to low
* @param  none
* @return none
*/
extern void _7816_deinit(void);
/**
* @brief  initialize 7816 pin
* @param  clk 7816 clock pin
* @param  rst 7816 reset pin
* @param  io 7816 io pin
* @return none
*/
extern void _7816_pin_init(GPIO_DEFINE clk, GPIO_DEFINE rst, GPIO_DEFINE io);
/**
* @brief  config 7816 parity mode
* @param  parity parity mode, _7816_PARITY_ODD or _7816_PARITY_EVEN
* @return none
*/
extern void _7816_config_parity(_7816_PARITY parity);
/**
* @brief  config 7816 data endian mode
* @param  parity parity mode, _7816_PARITY_ODD or _7816_PARITY_EVEN
* @return none
*/
extern void _7816_config_endian(_7816_ENDIAN_MODE endian);
/**
* @brief  config 7816 bauad
* @param  
* @return none
*/
extern void _7816_config_bauad(_7816_ENDIAN_MODE endian);
/**
* @brief  send data to device
* @param  txbuf data buffer
* @param  length send length
* @return send lenght
*/
extern uint16 _7816_tx(uint8 *txbuf, uint16 length);
/**
* @brief  receive data from device
* @param  rxbuf data buffer
* @param  length want receive length
* @return receive lenght
*/
extern uint16 _7816_rx(uint8 *rxbuf, uint16 length);
/**
* @brief  send and receive data
* @param  txbuf send data buffer
* @param  txlen send data length
* @param  rxbuf receive data buffer
* @param  rxlen receive data length
* @return receive lenght
*/
extern uint16 _7816_tx_rx(uint8 *txbuf, uint16 txlen, uint8 *rxbuf, uint16 rxlen);

/**
* @brief  ready buffer for receive use dma
* @param  rxbuf receive data buffer
* @param  rxlen receive data length
* @return none
*/
extern void _7816_dma_rx_ready(uint8 *rxbuf, uint8 len);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __IIS_H__ */

