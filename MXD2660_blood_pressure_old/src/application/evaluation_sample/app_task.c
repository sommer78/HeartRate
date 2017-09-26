/**
 *****************************************************************************
 * @file     app_task.c
 * @author   Richard
 * @version  V1.0.0
 * @date     20-Sep-2016
 * @brief    all app task process function related definition
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */
#include "cmsis_os.h"
#include "system_config.h"
#include "app_message.h"
#include "stack_task.h"
#include "app_task.h"
#include "app_ble.h"
#include "hci_api.h"
#include "ble_msg_api.h"
#include "device_cfg.h"

osThreadId  app_taskid;
void app_task(void const *pArgument);
osThreadDef(app_task, osPriorityNormal, 1, APP_TASK_SIZE);

osPoolId  app_msg_pool_id;
osPoolDef(app_msg_pool, MSG_MAX_NUM, APP_PERIPHERAL_MSG);


void app_task_init(void)
{
    //msg memory pool create
    app_msg_pool_id = osPoolCreate(osPool(app_msg_pool));

    app_taskid = osThreadCreate(osThread(app_task), NULL);
    if(NULL == app_taskid)
    {
        APP_DBG(("\napp task create fail\n"));
    }
}

void app_task(void const *pArgument)
{
    APP_PERIPHERAL_MSG *p_msg = NULL;
    BLE_TYPE_MSG *p_ble_msg = NULL;
    osEvent app_event;

    while(1)
    {
        app_event = osMessageGet(app_msg_id, osWaitForever);
        switch(app_event.status)
        {
            case osOK:
                break;

            case osEventSignal:
                break;

            case osEventMessage:
                p_msg =  (APP_PERIPHERAL_MSG*)app_event.value.p;
                switch(p_msg->msg_module_type)
                {
                    case MSG_BLE:
                        p_ble_msg = (BLE_TYPE_MSG*)app_event.value.p;
                        //function ble message processing
                        ble_msg_proc(p_ble_msg);
                        //release ble data buffer
                        if(NULL != p_ble_msg->p)
                        {
                            osPoolFree(ble_pool_id, p_ble_msg->p);
                        }
                        //release ble message buffer
                        if(NULL != p_ble_msg)
                        {
                            osPoolFree(ble_msg_pool_id, p_ble_msg);
                        }
                        break;
         
                }
                break;

            default:
                break;

        }

    }
}





