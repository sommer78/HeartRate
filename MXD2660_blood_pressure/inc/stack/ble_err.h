/**
 *****************************************************************************
 * @file     ble_err.h
 * @author   Richard
 * @version  V1.0.0
 * @date     08-March-2017
 * @brief    ble error declaration
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */
 
#ifndef __BLE_ERR_H__
#define __BLE_ERR_H__

typedef enum _BLE_ERR
{
    BLE_NO_ERR = 0,
    BLE_MSG_POOL_ALLOCATION_ERR,
    BLE_APP_MSG_POOL_ALLOCATION_ERR,
    BLE_POOL_ALLOCATION_ERR,
    BLE_APP_MSG_CREATION_ERR,
    BLE_MSG_CREATION_ERR,
    BLE_MSG_POOL_CREATION_ERR,
    BLE_POOL_CREATION_ERR,
    
    
}BLE_ERR;



#endif



