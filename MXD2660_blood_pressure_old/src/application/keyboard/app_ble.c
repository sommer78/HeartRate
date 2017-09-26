/**
 *****************************************************************************
 * @file     app_ble.c
 * @author   Richard
 * @version  V1.0.0
 * @date     30-Sep-2016
 * @brief    ble key handle app
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

#include "string.h"
#include "cmsis_os.h"
#include "system_config.h"
#include "mxd2660_flash.h"
#include "app_message.h"
#include "app_ble.h"
#include "hci_api.h"
#include "gatts_api.h"
#include "gatt_client.h"
#include "hid_service.h"
#include "stack_task.h"
#include "app_task.h"
#include "gatt_profile_uuid.h"
#include "gatt_client.h"
#include "ancs_service.h"
#include "app_hid_kbd.h"
#include "ble_msg_api.h"
#include "ble_api.h"
#include "ble_err.h"
#include "utility.h"
#include "sys.h"

BLE_STATUS g_ble_Status;
uint8 scanData[12]  __attribute__((aligned(4))) =
{
    11,
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'h',
    'i',
    'd',
    ' ',
    'd',
    'e',
    'v',
    'i',
    'c',
    'e',
};

// Advertising data
uint8 advData[13] __attribute__((aligned(4))) =
{
    // flags
    0x02,   // length of this data
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    // appearance
    0x03,   // length of this data
    GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_KEYBOARD),
    HI_UINT16(GAP_APPEARE_HID_KEYBOARD),

    // service UUIDs
    0x05,   // length of this data
    GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(HID_SERV_UUID),
    HI_UINT16(HID_SERV_UUID),
    LO_UINT16(BATT_SERV_UUID),
    HI_UINT16(BATT_SERV_UUID)
};


/**
* @brief  set adv/scan default configuration
* @param  none
* @return none
*/
void ble_app_default_init(void)
{
    uint8 random_addr[6] = {0xaa, 0xcd, 0x74, 0x37, 0x23, 0x12};
    g_gap_adv_para.advIntlMin16bit = 0x20;
    g_gap_adv_para.advIntlMax16bit = 0x20;
    memcpy(g_gap_adv_para.drctAddr6oct, random_addr, sizeof(random_addr));
    g_gap_adv_para.ownAddrType8bit = 0;  // 0x00Public Device Address;  0x01Random Device Address;
    g_gap_adv_para.drctAddrType8bit = 0;  // 0x00Public Device Address;  0x01Random Device Address;
    g_gap_adv_para.advType8bit = 0;
    g_gap_adv_para.advChMap8bit = 0x7;
    g_gap_adv_para.advFltPolicy8bit = 0;
    g_gap_adv_para.advDataLen = sizeof(advData);
    g_gap_adv_para.advDatabuf = advData;
    g_gap_adv_para.scanRspLen = sizeof(scanData);
    g_gap_adv_para.scanRspbuf = scanData;

    g_gap_scan_para.leScanInterval16bit = 0x20;
    g_gap_scan_para.leScanWindow16bit = 0x20;
    g_gap_scan_para.leScanType8bit = 0x01;
    g_gap_scan_para.ownAddrType8bit = 0;
    g_gap_scan_para.scanFltPolicy8bit = 0;
    g_gap_scan_para.duplicateFilterEn = 0;

}

/**
 * @brief  send data to ble peripheral
 * @param  none
 * @return none
 */
void ble_app_init(void)
{
    g_ble_Status.is_ble_connected = FALSE;
    g_ble_Status.is_ble_encrypted = FALSE;

    ble_msg_api_init();
    ble_app_default_init();
    ble_api_device_config_device_name("hid device");
    ble_api_device_config_appearance_set(GAP_APPEARE_GENERIC_HID);
    ble_api_att_auto_exchange_mtu_onoff(1);
}

void ble_app_start_adv_type(void)
{
//    uint8 addr[6];
//    uint8  addr_type;
//    if (TRUE ==  ble_api_get_last_paired_dev(addr, &addr_type))
//    {
//        g_gap_adv_para.advType8bit = GAP_ADRPT_ADV_DIRECT_IND;
//        memcpy(g_gap_adv_para.drctAddr6oct,addr,6);
//        g_gap_adv_para.drctAddrType8bit= addr_type;
//    }
//    else
//    {
//        g_gap_adv_para.advType8bit = GAP_ADRPT_ADV_IND;
//    }
    ble_api_adv_start();
}

/**
 * @brief  handle all ble msg
 * @param  *p_msg pointer to BLE_TYPE_MSG
 * @return none
 */
void ble_msg_proc(BLE_TYPE_MSG *p_msg)
{
    switch(p_msg->command)
    {
        case MSG_BLE_CONNECTED_IND:
            g_ble_Status.is_ble_connected = TRUE;
            ble_api_send_security_req(p_msg->connect_handle);
            break;
        case MSG_BLE_HIGH_DUTY_ADV_END_IND:
            g_gap_adv_para.advType8bit = GAP_ADRPT_ADV_IND;
            ble_api_adv_start();
            break;
        case MSG_BLE_DISCONNECTED_IND:
            g_ble_Status.is_ble_connected = FALSE;
            g_ble_Status.is_ble_encrypted = FALSE;
            // if connected then disconnect
            break;
        case MSG_BLE_SCAN_REPORT_IND:
        {
            if(p_msg->p)
            {
                gap_scan_adv_report_CB(((uint8 *)(p_msg->p) + 2), p_msg->length - 2);
            }
            break;
        }
        case MSG_BLE_ENCRYPTED_IND:
            g_ble_Status.is_ble_encrypted = TRUE;
//            kbd_ble_connection_goto_encryted_handler();
            break;

        case MSG_BLE_UNENCRYPTED_IND:
            g_ble_Status.is_ble_encrypted = FALSE;
            break;


        case MSG_BLE_ANCS_RPT_REPORT_IND:
            break;
        
        case MSG_AES_RSLT_RPT:
            //get aes result here, (uint8 *)(p_msg->p) point to the 128 bit aes result, it's little endian
            //sm_aes_for_app_run((uint8 *)(p_msg->p));
            break;
                
        default:
            break;

    }
}
/**
 * @brief  peripheral send msg to app task
 * @param  *data_buf data info related store here
 * @param  isr_flag 1:isr 0:general
 * @param  len len of *p point buf
 * @return BLE_ERR
 */
BLE_ERR ble_hid_sendMsgToApp(uint8 data_buf[2], uint8 len, void * p)
{
    APP_PERIPHERAL_MSG *peri_Msg = NULL;

    peri_Msg = osPoolAlloc(app_msg_pool_id);
    if(NULL == peri_Msg)
    {
        return BLE_APP_MSG_POOL_ALLOCATION_ERR;
    }
    peri_Msg->msg_module_type = MSG_KEYBOARD;
    peri_Msg->hid_key[0] =  data_buf[0];
    peri_Msg->hid_key[1] =  data_buf[1];
    peri_Msg->length = len;
    peri_Msg->p = p;

    if (__get_IPSR() != 0U)
    {
        osMessagePut(app_msg_id, (uint32)peri_Msg, 0);
    }
    else
    {
        osMessagePut(app_msg_id, (uint32)peri_Msg, osWaitForever);
    }
    return BLE_NO_ERR;
}

