/**
****************************************************************************
* @file     hci_msg_api.c
* @brief
* @author   xiaoyu
* @version  V0.0.0
* @date     2017/04/29
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#include "system_config.h"
#include "app_message.h"
#include "app_task.h"
#include "hci.h"
#include "app_ble.h"
#include "ble_api.h"
#include "mxd2660_uart.h"
#include "mxd2660_gpio.h"
#include "gatts_api.h"
#include "stack_task.h"
#include "ble_msg_api.h"
#include "hci_api.h"
#include "device_cfg.h"

#ifdef HCI_FUNC_EN


osThreadId hci_rx_taskId;
osThreadId hci_tx_taskId;

void peri_rx_process(void const * argument);
void peri_tx_process(void const * argument);

osThreadDef(peri_rx_process, osPriorityNormal, 1, HCI_RX_TSK_SIZE);
osThreadDef(peri_tx_process, osPriorityNormal, 1, HCI_TX_TSK_SIZE);

void task_init_for_reserved_task(void)
{
    hci_rx_taskId = osThreadCreate(osThread(peri_rx_process), NULL);
    if(NULL == hci_rx_taskId)
    {
        APP_DBG(("hci_rx_taskId create fail\n"));
    }
    hci_tx_taskId = osThreadCreate(osThread(peri_tx_process), NULL);
    if(NULL == hci_tx_taskId)
    {
        APP_DBG(("hci_tx_taskId create fail\n"));
    }

}


/**
* @brief  hci app init
* @param  none
* @return none
*/
void hci_app_init(void)
{
    hci_init();
  
    hci_api_uart_init();    //HCI function base uart0

    task_init_for_reserved_task();

}



#endif


