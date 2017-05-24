/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dataTrans_uart.h
* @brief    Data uart operations for testing profiles.
* @details  Data uart init and print data through data uart.
* @author
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/

#ifndef _DATA_TRANSMIT_UART_H_
#define  _DATA_TRANSMIT_UART_H_

#include "rtl_types.h"
#include "FreeRTOS.h"
#include "timers.h"

extern uint8_t UartRxTriggerLevel;
extern uint32_t num;

void RxDataStatusUpdate(void);
void CopyFromFIFO(void);
void TxHandleTask(void *pParameters);
void UARTChange(void);
void UARTInit(void);


#endif

