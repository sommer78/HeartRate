/**
 *****************************************************************************
 * @file     app_task.h
 * @author   Richard
 * @version  V1.0.0
 * @date     20-Sep-2016
 * @brief    all app task process function related definition 
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */
 
 
 /**
  * @addtogroup Application
  * @{
  * @defgroup key_board_app key_board_app
  * @{
  */

#ifndef __APP_TASK_H__
#define __APP_TASK_H__

#include "system_config.h"

#define APP_TASK_SIZE       APP_LAYER_TASK_SIZE - (HCI_RX_TSK_SIZE + HCI_TX_TSK_SIZE)

extern osPoolId  app_msg_pool_id; 
extern osPoolId  wristband_data_pool_id;

void app_task(void const *pArgument);
void app_task_init(void);



#endif
  
  
/** 
 * @} 
 * @} //end module
 */ 
 
 


