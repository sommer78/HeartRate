/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      board.h
* @brief     header file of Keypad demo.
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

 /* keyscan message type */
#define IO_DEMO_EVENT_KEY_VALUE_UPDATE          0x01
#define IO_DEMO_EVENT_KEY_RELEASE               0x02
     
typedef struct
{
    uint32_t length;
    struct
    {
        uint8_t column: 5;   /**< keyscan column buffer data   */
        uint8_t row: 3;      /**< keyscan raw buffer data      */
    } key[26];
}KEYSCAN_DATA_STRUCT, *PKEYSCAN_DATA_STRUCT;

/* keyscan message type */
typedef struct
{
    uint16_t msgType;
    union
    {
        uint32_t parm;
        void *pBuf;
    };
} KeyscanMsg; 

/* globals*/
extern KEYSCAN_DATA_STRUCT  CurKeyData;
extern KEYSCAN_DATA_STRUCT  PreKeyData;
extern xTimerHandle KeyScanTimer;
extern uint8_t RepeatReport;
extern xTaskHandle IODemoTaskHandle;
extern xQueueHandle DemoIOMessageQueue;

/* export functions */
extern void KeyPadInit(void);

#ifdef __cplusplus
}
#endif

#endif  /* _KEYPAD_H_ */

