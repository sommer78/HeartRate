/**
 *****************************************************************************
 * @file     ble_msg_api.h
 * @author   Richard
 * @version  V1.0.0
 * @date     25-Jan-2017
 * @brief    ble msg handle 
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */
 
#ifndef __BLE_MSG_API_H__
#define __BLE_MSG_API_H__

#include "ble_err.h"

#define BLE_POOL_NUM 8

#define BLE_MEM_POOL_SIZE  256

extern osMessageQId app_msg_id;;
extern osMessageQId ble_msg_id;
extern osPoolId  ble_msg_pool_id;
extern osPoolId  ble_pool_id;

/**
 * @brief  init ble msg definition and creation
 * @param  none
 * @return BLE_ERR
 */
BLE_ERR ble_msg_api_init(void);

/**
 * @brief  App send msg to ble task
 * @param  command msg type
 * @param  index one of the connected path
 * @param  length len of the point buffer
 * @param  p the more info in the specified buffer needed to send
 * @return BLE_ERR
 */
BLE_ERR ble_msg_AppSendToBle(uint8 command, uint8 link_handle, uint8 length, void* p);

/**
 * @brief  bt send msg to app
 * @param  command msg type
 * @param  link_handle one of the connected path
 * @param  length len of the point buffer
 * @param  p the more info in the specified buffer needed to send
 * @return BLE_ERR
 */
BLE_ERR ble_sendMsgToApp(uint8 command, uint8 link_handle, uint8 length, void* p);

#endif


