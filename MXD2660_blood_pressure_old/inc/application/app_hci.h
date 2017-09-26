/**
****************************************************************************
* @file     : hci_app.h
* @brief        :
* @author   : xiaoyu
* @version  : Initial Draft
* @date     : 2016/11/16
* @history  :
* @note     :
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

/** @defgroup  APPLICATION hci app
* @{
*/

#ifndef __HCI_APP_H__
#define __HCI_APP_H__

#include "system_config.h"
#include "app_message.h"

#define HCI_MEM_POOL_UNIT_SIZE      (256)


extern osPoolId hci_mem_pool_id;
extern osMessageQId hci_msg_id;
extern osPoolId  hci_msg_pool_id;
extern osThreadId reserved_1_taskId, reserved_2_taskId;

/** @name GLOABL FUNCTION
* @{
*/

/**
* @brief  hci app init
* @param  none
* @return none
*/
void hci_app_init(void);


/**
* @brief handle msg from hci
* @param p_hci_msg hci msg point
* @return none
*/
void hci_msg_proc(HCI_TYPE_MSG *p_hci_msg);


/**
* @brief  hci send msg to app
* @param  command msg command
* @param  isr_flag  send in isr or not
* @param  len data length
* @param  data buf point
* @return none
*/
void hci_msg_send_to_app(uint8 command, uint8 len, void *p);


/**
* @brief  app send msg to hci
* @param  command msg command
* @param  len data length
* @param  data buf point
* @return none
*/

void hci_msg_AppSendToHci(uint8 command, uint8 len, void *p);




/** @} */



#endif


/** @} */ // 模块结尾,自行拷贝到末尾
