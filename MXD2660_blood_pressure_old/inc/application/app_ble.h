/**
 *****************************************************************************
 * @file     ble_app.h
 * @author   Richard
 * @version  V1.0.0
 * @date     30-Sep-2016
 * @brief    ble key handle app
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

/**
 * @addtogroup Application
 * @{
 * @defgroup ble_app ble_app
 * @{
 */

#ifndef __BLE_APP_H__
#define __BLE_APP_H__

#include "system_config.h"
#include "app_message.h"
#include "ble_err.h"
#include "stack_ctrl.h"

/**
 * @brief  ble status related variable definition
 */
typedef struct
{
    uint8 is_ble_connected;  /**<ble connection flag, if ble is conncted please set*/
    uint8 is_ble_encrypted;  /**<ble encrypted flag*/

} BLE_STATUS;
extern BLE_STATUS g_ble_Status;


typedef struct bond_info
{

    // Identification
    uint8 saved;
    uint8 addr_type;
    bd_addr_t addr;
    sm_key_t irk;

    // Stored pairing information allows to re-establish an enncrypted connection
    // with a peripheral that doesn't have any persistent memory
    sm_key_t ltk;
    uint8  rand[8];
    uint16 ediv;
    uint16  key_size;


} bond_info_t;
#define LE_BOND_INFO_SIZE 4
#define INVALID_ADDR_TYPE 0xff
#define  SAVE_VALID_FLAG 0xAD


/**
 * @}
 */


/**
 * @name global functional definition
 * @{
 */

/**
 * @brief  send data to ble peripheral
 * @param  none
 * @return none
 */
extern void ble_app_init(void);

/**
 * @brief  peripheral send msg to app task
 * @param  msg_module_type: msg type
 * @param  sub_type: sub type of module type
 * @param  cmd: see cmd definition
 * @param  len len of *p point buf
 * @param  p point
 * @return BLE_ERR
 */
BLE_ERR ble_peri_sendMsgToApp(uint8 msg_module_type, uint8 sub_type, uint8 cmd, uint8 len, void * p);

/**
 * @brief  peripheral send msg to app task
 * @param  *data_buf data info related store here
 * @param  isr_flag 1:isr 0:general
 * @param  len len of *p point buf
 * @return BLE_ERR
 */
BLE_ERR ble_hid_sendMsgToApp(uint8 data_buf[2], uint8 len, void * p);


/**
 * @brief  ble msg handle function
 * @param  *msg the msg come from ble task
 * @return none
 */
extern void ble_msg_proc(BLE_TYPE_MSG *pMsg);

/**
 * @brief  send data to ble peripheral
 * @param  none
 * @return none
 */
void ble_app_start_adv_type(void);

/**
 * @brief  load bond device info
 * @param  none
 * @return none
 */
void le_bond_device_db_load_from_flash(void);

/**
 * @}
 */

#endif

/**
 * @}
 * @} //end module
 */




