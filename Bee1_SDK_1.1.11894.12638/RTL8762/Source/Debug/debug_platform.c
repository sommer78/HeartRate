/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    Provide some debug APIs.
* @details
* @author   Susan Zhang
* @date     2016-05-20
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl_types.h"
#include "FreeRTOS.h"
#include "List.h"
#include "task.h"
#include "queue.h"

typedef struct QueueDefinition
{
	int8_t *pcHead;					/*< Points to the beginning of the queue storage area. */
	int8_t *pcTail;					/*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */
	int8_t *pcWriteTo;				/*< Points to the free next place in the storage area. */

	union							/* Use of a union is an exception to the coding standard to ensure two mutually exclusive structure members don't appear simultaneously (wasting RAM). */
	{
		int8_t *pcReadFrom;			/*< Points to the last place that a queued item was read from when the structure is used as a queue. */
		UBaseType_t uxRecursiveCallCount;/*< Maintains a count of the number of times a recursive mutex has been recursively 'taken' when the structure is used as a mutex. */
	} u;

	List_t xTasksWaitingToSend;		/*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
	List_t xTasksWaitingToReceive;	/*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

	volatile UBaseType_t uxMessagesWaiting;/*< The number of items currently in the queue. */
//	UBaseType_t uxLength;			/*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
//	UBaseType_t uxItemSize;			/*< The size of each items that the queue will hold. */

//	volatile BaseType_t xRxLock;	/*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
//	volatile BaseType_t xTxLock;	/*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */

//	#if ( configUSE_TRACE_FACILITY == 1 )
//		UBaseType_t uxQueueNumber;
//		uint8_t ucQueueType;
//	#endif

//	#if ( configUSE_QUEUE_SETS == 1 )
//		struct QueueDefinition *pxQueueSetContainer;
//	#endif

} xQUEUE;
typedef xQUEUE Queue_t;

typedef struct tskTaskControlBlock
{
	volatile StackType_t	*pxTopOfStack;	/*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

//	#if ( portUSING_MPU_WRAPPERS == 1 )
//		xMPU_SETTINGS	xMPUSettings;		/*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
//	#endif

//	ListItem_t			xGenericListItem;	/*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
//	ListItem_t			xEventListItem;		/*< Used to reference a task from an event list. */
//	UBaseType_t			uxPriority;			/*< The priority of the task.  0 is the lowest priority. */
//	StackType_t			*pxStack;			/*< Points to the start of the stack. */
//	char				pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

//	#if ( portSTACK_GROWTH > 0 )
//		StackType_t		*pxEndOfStack;		/*< Points to the end of the stack on architectures where the stack grows up from low memory. */
//	#endif

//	#if ( portCRITICAL_NESTING_IN_TCB == 1 )
//		UBaseType_t 	uxCriticalNesting; 	/*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
//	#endif

//	#if ( configUSE_TRACE_FACILITY == 1 )
//		UBaseType_t		uxTCBNumber;		/*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
//		UBaseType_t  	uxTaskNumber;		/*< Stores a number specifically for use by third party trace code. */
//	#endif

//	#if ( configUSE_MUTEXES == 1 )
//		UBaseType_t 	uxBasePriority;		/*< The priority last assigned to the task - used by the priority inheritance mechanism. */
//	#endif

//	#if ( configUSE_APPLICATION_TASK_TAG == 1 )
//		TaskHookFunction_t pxTaskTag;
//	#endif

//	#if ( configGENERATE_RUN_TIME_STATS == 1 )
//		uint32_t		ulRunTimeCounter;	/*< Stores the amount of time the task has spent in the Running state. */
//	#endif

//	#if ( configUSE_NEWLIB_REENTRANT == 1 )
//		/* Allocate a Newlib reent structure that is specific to this task.
//		Note Newlib support has been included by popular demand, but is not
//		used by the FreeRTOS maintainers themselves.  FreeRTOS is not
//		responsible for resulting newlib operation.  User must be familiar with
//		newlib and must provide system-wide implementations of the necessary
//		stubs. Be warned that (at the time of writing) the current newlib design
//		implements a system-wide malloc() that must be provided with locks. */
//		struct 	_reent xNewLib_reent;
//	#endif

} tskTCB;

/* The old tskTCB name is maintained above then typedefed to the new TCB_t name
below to enable the use of older kernel aware debuggers. */
typedef tskTCB TCB_t;

extern xQueueHandle hEventQueueHandle;
extern xTaskHandle  hAppTaskHandle;

void vTaskSwitchContext_imp( void )
{		
		Queue_t *pxEventQueue = hEventQueueHandle;
		Queue_t *pxTimerQueue = *(Queue_t **)0x20000560;		
		
		if(pxEventQueue)
		{
			if(pxEventQueue->uxMessagesWaiting >= 10 || pxTimerQueue->uxMessagesWaiting >= 4)/*app queue and timer queue threshold */
			{
					DBG_DIRECT("susan debug");
					DBG_DIRECT("Event:%d  Timer:%d", 
						pxEventQueue->uxMessagesWaiting, 
						pxTimerQueue->uxMessagesWaiting);
				
					uint32_t *pxTopOfTimerStack =  *(uint32_t **)(*(uint32_t*)0x20000564);//timer
					uint32_t *pxTopOfLowerStack =  *(uint32_t **)(*(uint32_t*)0x20001894);//lowerstack
					uint32_t *pxTopOfUpperStack =  *(uint32_t **)(*(uint32_t*)0x20001898);//upperstack

					uint32_t *pxTopOfAPPStack = *(uint32_t **)hAppTaskHandle;/*app task handle*/
				
					pxTopOfTimerStack += 8;
					pxTopOfLowerStack += 8;
					pxTopOfUpperStack += 8;
					pxTopOfAPPStack += 8;
				
					DBG_DIRECT("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", 
						*(pxTopOfTimerStack+5), *(pxTopOfTimerStack+6),
						*(pxTopOfLowerStack+5), *(pxTopOfLowerStack+6), 
						*(pxTopOfUpperStack+5), *(pxTopOfUpperStack+6),
						*(pxTopOfAPPStack+5), *(pxTopOfAPPStack+6)
						);
			}
		}
    
    *((uint32_t*)0x200018d8) = NULL;
    vTaskSwitchContext();
    *((uint32_t*)0x200018d8) = (uint32_t)vTaskSwitchContext_imp;
}

void DebugTaskHang(void)
{
	*((uint32_t*)0x200018d8) = (uint32_t)vTaskSwitchContext_imp;
}

void save_log_to_flash(void)
{
		/* init log_addr 
	   * 0x20005F0C:log_addr
	   */
		int i;
		for(i = 0x00523000; i<0x0053c000 - 4; i += 4)
		{
			uint32_t pattern1 = 0;
			uint32_t pattern2 = 0;					
			
			pattern1 = FMC_Read(i);
			pattern2 = FMC_Read(i + 4);
			if((pattern1 == 0xFFFFFFFF) && (pattern2 == 0xFFFFFFFF))
			{
				*(uint32_t*)0x20005F0C = i;
				
				break;
			}			
		}
		
		if(i == 0x0053c000 - 4)
		{			
			*(uint32_t*)0x20005F0C = 0x00523000;
			FMC_Erase_Page(0x523000);
		}
		
		
	/* begin to patch CycQueueReadandPrint so as to print log to flash instead of to Log Uart
	 * 0x200018e8: pPatch_CycQueueReadandPrint
	 * 0x20005F08:	pPatch_CycQueueReadandPrint_pointer
	 */
	*(uint32_t*)0x200018e8 = *(uint32_t*)0x20005F08;
}

void stop_save_log_to_flash(void)
{	
	/* stop patching CycQueueReadandPrint so as to print log to Log Uart instead of to flash
	 * 0x200018e8: pPatch_CycQueueReadandPrint
	 */
	*(uint32_t*)0x200018e8 = NULL;
}

void erase_ota_flash(void)
{
		/*erase ota region*/
		uint32_t addr = 0x00523000;
		int numPage = 50;
		for(uint32_t i=0;i<numPage;++i)
		{
				FMC_Erase_Page(addr);
				addr += 2 * 1024;
		}
}