/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		rtl876x_it.c
* @brief	Q-decoder demo-interrupt handle.
* @details
* @author	tifnan
* @date 	2015-06-25
* @version	v0.1
*********************************************************************************************************
*/

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "rtl876x_qdec.h"
#include "qdec.h"
#include "board.h"
#include "trace.h"
#include "rtl_memory.h"
#include "string.h"

void QdecodeIntrHandler()
{
    long xHigherPriorityTaskWoken = pdFALSE;

    /* Mask qdec interrupt */
    QDEC_INTMask(QDEC, QDEC_AXIS_Y, ENABLE);

    /* send message to app */
    QdecMsg msg;
    msg.msgType = IO_DEMO_EVENT_QDEC_VALUE_UPDATE;
    msg.parm = QDEC->REG_SR_Y;
    
    xQueueSend(DemoIOMessageQueue, &msg, 0);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

    /* Clear & enable interrupt */
    QDEC_ClearINTPendingBit(QDEC, QDEC_FLAG_INT_Y);
    QDEC_INTMask(QDEC, QDEC_AXIS_Y, DISABLE);
}
