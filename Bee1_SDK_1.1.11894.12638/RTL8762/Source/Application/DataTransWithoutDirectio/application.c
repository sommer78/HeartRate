/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     application.c
* @brief    .
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v0.2
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "application.h"
#include "simple_ble_central.h"
#include "simple_ble_central_api.h"
#include "SimpleBLEPeripheral.h"
#include "SimpleBLEPheripheral_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "blueapi.h"

#include "dlps_platform.h"
#include "module_param_config.h"
#include "dataTrans_application.h"

extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
extern void Driver_Init(void);
extern void PwrMgr_Init(void);

/****************************************************************************/
/* Events                                                                   */
/****************************************************************************/


#define BLUEAPI_MSG_EVENT        0x01
#define EVENT_IODRIVER_TO_APP                   0x05

#define BROADCASTER_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)   /* Task priorities. */
#define BROADCASTER_TASK_STACK_SIZE        1024*2

#define MAX_NUMBER_OF_RX_EVENT     0x40
#define MAX_NUMBER_OF_MESSAGE      0x20

xTaskHandle  hMouseAppTaskHandle;
xQueueHandle hEventQueueHandle;
xQueueHandle hMessageQueueHandle;
xQueueHandle hIoQueueHandle;


void peripheralBlueAPIMessageCB(PBlueAPI_UsMessage pMsg)
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

void bee_task_app(void *pvParameters );
void application_task_init()
{

    /* Create APP Task. */
    xTaskCreate(bee_task_app,    /* Pointer to the function that implements the task. */
                "APPTask",           /* Text name for the task.  This is to facilitate debugging only. */
                512,                 /* Stack depth in words. 1KB*/
                NULL,               /* We are not using the task parameter. */
                1,                  /* This task will run at priority 1. */
                &hMouseAppTaskHandle );             /*  the task handle. */
}


/**
* @brief
*
*
* @param   pvParameters
* @return  void
*/
void bee_task_app(void *pvParameters )
{
    char Event;

    hMessageQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE,
                                       sizeof(PBlueAPI_UsMessage));

    hIoQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE,
                                  sizeof(BEE_IO_MSG));

    hEventQueueHandle = xQueueCreate(MAX_NUMBER_OF_RX_EVENT,
                                     sizeof(unsigned char));

    peripheral_StartBtStack();
    Driver_Init();
    PwrMgr_Init();

    while (true)
    {
        if (xQueueReceive(hEventQueueHandle, &Event, portMAX_DELAY) == pdPASS)
        {
            if (Event == BLUEAPI_MSG_EVENT)  /* BlueAPI */
            {
                PBlueAPI_UsMessage pMsg;

                while (xQueueReceive(hMessageQueueHandle, &pMsg, 0) == pdPASS)
                {
                    if (gConfigParam->gaprole.gaprole == CENTRAL_ROLE)
                    {
                        central_HandleBlueAPIMessage(pMsg);
                    }
                    else if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
                    {
                        peripheral_HandleBlueAPIMessage(pMsg);
                    }
                }
            }
            else
            {
                BEE_IO_MSG io_driver_msg_recv;
                if (xQueueReceive( hIoQueueHandle, &io_driver_msg_recv, 0 ) == pdPASS)
                {
                    AppHandleIODriverMessage(io_driver_msg_recv);
                }
            }
        }

        if (g_AppCB->gapProfileState == GAPSTATE_CONNECTED)
        {
            if ((g_AppCB->rxBufferDataLength == 0) && (g_AppCB->txUartDataQueueFree.ElementCount == TX_PACKET_COUNT) && (gConfigParam->isTimerDLPSActive == FALSE) && (gConfigParam-> isDataAvailable == TRUE)) //replace "gConfigParam->isTimerDLPSActive"  with  "if ( xTimerIsTimerActive( pxTimer ) != pdFALSE )" ??
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "All Data have finished NOTIFYING, Reset the TimersEnterDLPSAfterConnected again !!", 0);
#endif
                xTimerReset(TimersEnterDLPSAfterConnected, 0);
                gConfigParam->isTimerDLPSActive = TRUE;
                gConfigParam-> isDataAvailable = FALSE;
            }
        }
    }
}

