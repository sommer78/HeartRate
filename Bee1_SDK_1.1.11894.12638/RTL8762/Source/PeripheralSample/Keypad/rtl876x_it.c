/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		rtl876x_it.c
* @brief	Keypad demo.
* @details
* @author	tifnan
* @date 	2015-06-25
* @version	v0.1
*********************************************************************************************************
*/

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "rtl876x_adc.h"
#include "rtl876x_keyscan.h"
#include "board.h"
#include "trace.h"
#include "rtl_memory.h"
#include "string.h"
#include "keypad.h"

void KeyscanIntrHandler(void)
{   
    uint32_t fifoLength;
    long xHigherPriorityTaskWoken = pdFALSE;
    
    PKEYSCAN_DATA_STRUCT pKeyData = &CurKeyData;
    
    /* Read current keyscan interrupt status and mask interrupt */
    KeyScan_INTMask(KEYSCAN, ENABLE);
    KeyScan_ClearINTPendingBit(KEYSCAN, KEYSCAN_INT_SCAN_END);

    /* Check keyscan timer is created or not.
       If not, it means something wrong and we require to handle this error. */
    if (KeyScanTimer == NULL)
    {
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, "hKeyscan_Timer is NULL!", 0);
        goto UNMASK_INT;
    }

    /* Reset timer before use it. */
    xTimerResetFromISR(KeyScanTimer, &xHigherPriorityTaskWoken);

    memset(pKeyData, 0, sizeof(KEYSCAN_DATA_STRUCT));

    fifoLength = (uint32_t)KeyScan_GetFifoDataNum(KEYSCAN);
    pKeyData->length = fifoLength;

    /* If more than one data in FIFO */
    if (fifoLength > 0)
    {
        if (fifoLength > (sizeof(pKeyData->key) / sizeof(uint16_t)))
        {
            fifoLength = sizeof(pKeyData->key) / sizeof(uint16_t);
        }

        /* Read out all FIFO data */
        KeyScan_Read(KEYSCAN, (uint8_t*)(&pKeyData->key[0]), fifoLength);

    }

    if (!RepeatReport)
    {
        if (!memcmp(pKeyData, &PreKeyData, sizeof(KEYSCAN_DATA_STRUCT)))
        {
            goto UNMASK_INT;
        }
        else
        {
            memcpy(&PreKeyData, pKeyData, sizeof(KEYSCAN_DATA_STRUCT));
        }
    }

    /* Send event to app task */
    KeyscanMsg msg;
    msg.msgType = IO_DEMO_EVENT_KEY_VALUE_UPDATE;
    msg.pBuf = (void*)pKeyData;

    //send msg
    xQueueSendFromISR(DemoIOMessageQueue, &msg, 0);

UNMASK_INT:
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

    /* Clear and unmask interrupt */
    KeyScan_INTMask(KEYSCAN, DISABLE);   //unmask interrupt
    
    return;
}
