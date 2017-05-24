/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_queue.h
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/

#if !defined (__APP_QUEUE_H)
#define      __APP_QUEUE_H
#define __APP_QUEUE__

#include <rtl_types.h>



struct QueueElement                                     /* dummy definition      */
{
    struct QueueElement *Next;                          /* point to next element */
    uint8_t  data[2];                                               /* user data             */
};
typedef struct QueueElement ELEMENT_T;

typedef ELEMENT_T   *ELEMENT_P;

typedef struct
{
    ELEMENT_P First;                                 /* first element         */
    ELEMENT_P Last;                                  /* last element          */
    uint16_t      ElementCount;                      /* element count         */
} QUEUE_T, *QUEUE_P;




#if defined (__cplusplus)
extern "C" {
#endif

void  AppQueueIn     (QUEUE_P QueuePtr, void *QueueElementPtr);
void *AppQueueOut    (QUEUE_P QueuePtr);
void  AppQueueInsert (QUEUE_P QueuePtr, void *QueueElementPtr, void *NewQueueElementPtr);



#if defined (__cplusplus)
}
#endif

#endif /* if !defined (__QUEUE_H) */
