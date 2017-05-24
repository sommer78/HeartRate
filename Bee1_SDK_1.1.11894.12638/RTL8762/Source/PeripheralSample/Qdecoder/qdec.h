/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      qdec.h
* @brief     header file of Q-decoder demo
* @details   
* @author    tifnan_ge
* @date      2015-06-25
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

 /* event */
#define IO_DEMO_EVENT_QDEC_VALUE_UPDATE          0x01
     
/* keyscan message type */
typedef struct
{
    uint16_t msgType;
    union
    {
        uint32_t parm;
        void *pBuf;
    };
} QdecMsg; 

/* globals*/
extern xTaskHandle IODemoTaskHandle;
extern xQueueHandle DemoIOMessageQueue;

#ifdef __cplusplus
}
#endif

#endif  /* _KEYPAD_H_ */

