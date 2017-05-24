/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    application.c
  * @brief   application task.
  * @details
  * @author  ranhui
  * @date    2015-03-27
  * @version v0.2
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include "rtl876x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "application.h"
#include "simple_ble_central.h"
#include "simple_ble_central_api.h"
#include "blueapi.h"

extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
void bee_task_app(void *pvParameters );

/** Add Macros here **/
/**<  Events. */
#define BLUEAPI_MSG_EVENT                   0x01
#define EVENT_IODRIVER_TO_APP          		0x05
/**<  App Task parameters. */
#define BROADCASTER_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)   /* Task priorities. */
#define BROADCASTER_TASK_STACK_SIZE        1024*2
/**<  Max counts of queues. */
#define MAX_NUMBER_OF_RX_EVENT     0x20
#define MAX_NUMBER_OF_MESSAGE      0x20
/**<  Handles of app task and queues. */
xTaskHandle  hMouseAppTaskHandle;
xQueueHandle hEventQueueHandle;
xQueueHandle hMessageQueueHandle;
xQueueHandle hIoQueueHandle;

/**
  * @brief  Callback function should be register to upper stack to send message to application.
  * @param  pMsg - message sent from upper stack.
  * @retval none
  */
void centralBlueAPIMessageCB(PBlueAPI_UsMessage pMsg)
{
    unsigned char Event = BLUEAPI_MSG_EVENT;

    if (xQueueSend(hMessageQueueHandle, &pMsg, 0) == errQUEUE_FULL)
    {
        blueAPI_BufferRelease(pMsg);
    }
    else if (xQueueSend(hEventQueueHandle, &Event, 0) == errQUEUE_FULL)
    {

    }
}

/**
  * @brief  Create application task.
  * @retval none
  */
void application_task_init()
{

	/* Create APP Task. */
    xTaskCreate(bee_task_app,    /* Pointer to the function that implements the task. */
                    "APPTask",           /* Text name for the task.  This is to facilitate debugging only. */
                    256,                 /* Stack depth in words. 1KB*/
                    NULL,               /* We are not using the task parameter. */
                    1,                  /* This task will run at priority 1. */
                    &hMouseAppTaskHandle );             /*  the task handle. */
}

/**
  * @brief  application task.
  * @param  pvParameters
  * @retval none
  */
void bee_task_app(void *pvParameters )
{
    char Event;
    /* Create app task related queues. */
    hMessageQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE,
                                       sizeof(PBlueAPI_UsMessage));
    hIoQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE,
                                       sizeof(BEE_IO_MSG));
    hEventQueueHandle = xQueueCreate(MAX_NUMBER_OF_RX_EVENT,
                                       sizeof(unsigned char));
    /* Register BT Stack. */
    central_StartBtStack();
    
    while (true)
    {
        if (xQueueReceive(hEventQueueHandle, &Event, portMAX_DELAY) == pdPASS)
        {
            if (Event == BLUEAPI_MSG_EVENT)  /* BlueAPI */
            {
                PBlueAPI_UsMessage pMsg;

                while (xQueueReceive(hMessageQueueHandle, &pMsg, 0) == pdPASS)
                {
                    central_HandleBlueAPIMessage(pMsg);
                }
            }
            else
            if(Event == EVENT_NEWIODRIVER_TO_APP)
            {
                BEE_IO_MSG io_driver_msg_recv;	
                while(xQueueReceive( hIoQueueHandle, &io_driver_msg_recv, 0 ) == pdPASS)
                {
                    AppHandleIODriverMessage(io_driver_msg_recv);
                }
            }
        }
    }
}

