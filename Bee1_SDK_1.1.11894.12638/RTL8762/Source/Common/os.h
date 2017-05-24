/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      os.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2015-4-18
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _OS_H
#define _OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "bee_message.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

uint8_t SendMessage(BEE_IO_MSG *msg);
uint8_t SendMessageFromISR(BEE_IO_MSG *msg, long *xHigherPriorityTaskWoken);


#ifdef __cplusplus
}
#endif

#endif /* _OS_H */

