/**
****************************************************************************
* @file		 app_uart_update.h
* @brief	 provide uart update API to user
* @author	 xiaoyu
* @version	 V1.0.0
* @date		 2017/24/6
* @history	  
* @note		  
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/


#ifndef __APP_UART_UPDATE_H__
#define __APP_UART_UPDATE_H__

#define UART_UPDATE_PIN GPIO_29
#define UART_UPDATE_PIN2 GPIO_30

/**
 * @param  uart update configuration API
 * @param  uart_sel 0 uart0; 1 uart1
 * @param  txd gpio number
 * @param  rxd gpio number
 * @param  baud_rate BAUDRATE_115200
 * @return NONE
 */
extern void uart_update_trigger(uint8 uart_sel, uint8 txd, uint8 rxd, uint32 baud_rate);


#endif



