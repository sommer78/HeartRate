/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     os.c
* @brief
* @details
* @author   Chuanguo Xue
* @date     2015-04-18
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "bee_message.h"
#include <string.h>

#include "trace.h"

#include "os.h"


extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;

uint8_t SendMessage(BEE_IO_MSG *pmsg)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event;

    /* Send MSG to APP task */
    SendQueueResult = xQueueSend(hIoQueueHandle, pmsg, 0x0);
    if (SendQueueResult != pdPASS)
    {
        return pdFALSE;
    }

    /* Send EVENT to notify APP task */
    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event, 0x0);
    if (SendQueueResult != pdPASS)
    {
        return pdFALSE;
    }

    return pdTRUE;

}

uint8_t SendMessageFromISR(BEE_IO_MSG *pmsg, long *pxHigherPriorityTaskWoken)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event;

    /* Send MSG to APP task */
    SendQueueResult = xQueueSendFromISR(hIoQueueHandle, pmsg, pxHigherPriorityTaskWoken);
    if (SendQueueResult != pdPASS)
    {
        return pdFALSE;
    }

    /* Send EVENT to notify APP task */
    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSendFromISR(hEventQueueHandle, &Event, pxHigherPriorityTaskWoken);
    if (SendQueueResult != pdPASS)
    {
        return pdFALSE;
    }

    return pdTRUE;
}

/**
*********************************************************************************************************
*                            Add FreeRTOS APIs
**********************************************************************************************************
*/


/* The definition of the timers themselves. */
typedef struct tmrTimerControl
{
    const char				*pcTimerName;		/*<< Text name.  This is not used by the kernel, it is included simply to make debugging easier. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
    ListItem_t				xTimerListItem;		/*<< Standard linked list item as used by all kernel features for event management. */
    TickType_t				xTimerPeriodInTicks;/*<< How quickly and often the timer expires. */
    UBaseType_t				uxAutoReload;		/*<< Set to pdTRUE if the timer should be automatically restarted once expired.  Set to pdFALSE if the timer is, in effect, a one-shot timer. */
    void 					*pvTimerID;			/*<< An ID to identify the timer.  This allows the timer to be identified when the same callback is used for multiple timers. */
    TimerCallbackFunction_t	pxCallbackFunction;	/*<< The function that will be called when the timer expires. */
    struct tmrTimerControl *next;
    uint8_t my_entry_id;
    uint8_t rsvd[3];
} xTIMER;

typedef xTIMER Timer_t;

TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer )
{
    Timer_t * pxTimer = ( Timer_t * ) xTimer;
    TickType_t xReturn;
    configASSERT( xTimer );
    xReturn = listGET_LIST_ITEM_VALUE( &( pxTimer->xTimerListItem ) );
    return xReturn;
}
