/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     data_uart.h
* @brief    Data uart operations for testing profiles.
* @details  Data uart init and print data through data uart.
* @author
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _DATA_UART_H_
#define  _DATA_UART_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "rtl_types.h"

/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/** @brief  Print data use Data UART. */
#define dataUARTCmdPrint(a, b...)             dataUART_Print(b)
#define dataUARTUserIFSendString(a, b, c)     dataUART_Print("%s", b)
#define dataUARTUserIFSendChar(a, b)          if (b != 0) dataUART_Print("%c", b)

/****************************************************************************************************************
* exported functions other .c files may use all defined here.
****************************************************************************************************************/
/* Print formated data use Data UART in test profile application */
extern int32_t dataUART_Print(IN  char *fmt, ...);
/* initiate Data UART in test profile application */
extern void dataUART_Init(void);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif

