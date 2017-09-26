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
#include "hci_event.h"
#include "gatts_api.h"
#include "gatt_client.h"
#include "hid_service.h"
#include "stack_task.h"
#include "app_task.h"
#include "gatt_profile_uuid.h"
#include "ancs_service.h"
#include "ancs.h"
#include "ble_msg_api.h"
#include "ble_api.h"
#include "ble_err.h"
#include "utility.h"
#include "wristband_service.h"
#include "timer.h"
#include "sys.h"

BLE_STATUS g_ble_Status;


#ifdef ANCS_FUNC_EN
    uint8           ancs_discover_flag = 0;
    uint8           ancs_cnt = 5;
    uint8		    ancs_hdl;
    timer_node_t    ancs_timer;

#endif


uint8 scanData[20] __attribute__((aligned(4))) =
{
    18,
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'm',
    'x',
    'd',
    'b',
    'l',
    'e',
    '-',
    'w',
    'r',
    'i',
    's',
    't',
    'b',
    'a',
    'n',
    'd',
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
    LO_UINT16(GAP_APPEARE_GENERIC_WATCH),
    HI_UINT16(GAP_APPEARE_GENERIC_WATCH),

    // service UUIDs
    0x05,   // length of this data
    GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(MXD_TX_RX_SERVICE_UUID),
    HI_UINT16(MXD_TX_RX_SERVICE_UUID),
    LO_UINT16(HEARTRATE_SERV_UUID),
    HI_UINT16(HEARTRATE_SERV_UUID),
    

};


#ifdef ANCS_FUNC_EN
void ancs_timeout_handler(void* p_time_out)
{
    
    if(ancs_discover_flag)
    {
        timer_stop(&ancs_timer);
        return;
    }
    
    if(ancs_cnt <= 0)
    {
        timer_stop(&ancs_timer);
        return;
    }
    
    ancs_cnt--;
    ancs_discover_again(ancs_hdl);
}
#endif


/**
* @brief  set adv/scan default configuration
* @param  none
* @return none
*/
void ble_app_default_init(void)
{
    uint8 random_addr[6] = {0xaa, 0xcd, 0x44, 0x33, 0x22, 0x11};
    g_gap_adv_para.advIntlMin16bit = 0x64;
    g_gap_adv_para.advIntlMax16bit = 0x64;
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
    char name[20] = "wristband device";
    g_ble_Status.is_ble_connected = FALSE;
    g_ble_Status.is_ble_encrypted = FALSE;

    ble_msg_api_init();
    ble_app_default_init();
    ble_api_device_config_device_name(name);
    ble_api_att_auto_exchange_mtu_onoff(1);
    ble_api_adv_start();
    
#ifdef ANCS_FUNC_EN
    //creat a timer
    timer_create(&ancs_timer, APP_TIMER_MODE_REPEATED, ancs_timeout_handler);
#endif
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
        #ifdef ANCS_FUNC_EN
            ancs_init(p_msg->connect_handle);
            ancs_hdl = p_msg->connect_handle;
        #endif
            ble_api_send_security_req(p_msg->connect_handle);
            APP_DBG(("ble is conected\n"));
            break;

        case MSG_BLE_DISCONNECTED_IND:
            g_ble_Status.is_ble_connected = FALSE;
            g_ble_Status.is_ble_encrypted = FALSE;
            ancs_discover_flag = 0;
            // if connected then disconnect
            GAPRole_TerminateConnection();
            ble_api_adv_start();
            APP_DBG(("ble is disconected\n"));
            break;
        case MSG_BLE_SCAN_REPORT_IND:
        {

            break;
        }
        case MSG_BLE_ENCRYPTED_IND:
            g_ble_Status.is_ble_encrypted = TRUE;
        #ifdef ANCS_FUNC_EN
            ancs_discover_svc_by_uuid(p_msg->connect_handle, 1);
            timer_start(&ancs_timer, 3000, NULL);
        #endif
            
            break;

        case MSG_BLE_UNENCRYPTED_IND:
            g_ble_Status.is_ble_encrypted = FALSE;
            break;


        case MSG_USER_PASSKEYREQ_IND:
        {
            //app decide how to get passkey and send it to stack
            break;
        }
        case MSG_USER_PASSKEY_DISPLAY_IND:
        {
            //app decide how to present the passkey
            break;
        }
        case MSG_USER_PASSKEY_DISPLAY_CANCEL_IND:
        {
            //app cancel passkey display
            break;
        }
        case MSG_KEYPRESS_NOTIFICATION:
        {
            //app
            break;
        }
        case MSG_PAIR_COMPLETED:
        {
            break;
        }
        case MSG_BLE_ANCS_RPT_REPORT_IND:
        {
            if(p_msg->p)
            {
            #ifdef ANCS_FUNC_EN
                ancs_data_ntf_handler(p_msg->length - 3, (uint8 *)(p_msg->p) + 3);
            #endif
            }
            break;
        }
        case MSG_BLE_ANCS_INQUIRY_RSLT:
        {
        #ifdef ANCS_FUNC_EN
            if(p_msg->p)
            {
                uint8 msg= ((uint8 *)(p_msg->p))[0];
                if (ANCS_SERVER_NOT_FOUND ==  msg)
                {
                    //not find ancs server
                }
                else if (ANCS_SERVER_FOUND ==  msg)
                {//find all char of ancs server
                    ancs_all_characteristic_found_handler(0,((uint8 *)(p_msg->p))[0]);
                    ancs_server_config_notify(1); //0 close , 1 open
                }
                else if (ANCS_SERVER_NOTIFY_OFF ==  msg)
                {
                    //ancs server notify disable
                }
                else if (ANCS_SERVER_NOTIFY_ON ==  msg)
                {
                    //ancs server notify enable
                }
            }
        #endif
            break;
        }
        case MSG_BLE_ANCS_SURCE_CHAR_RPT:
        {
            if(p_msg->p)
            {
            #ifdef ANCS_FUNC_EN
                ancs_source_ntf_handler((uint8 *)(p_msg->p));
            #endif
            }
            break;
        }
        case MSG_AES_RSLT_RPT:
        {
            //get aes result here, (uint8 *)(p_msg->p) point to the 128 bit aes result, it's little endian

            break;
        }		
        default:
            break;

    }
}
/**
 * @brief  peripheral send msg to app task
 * @param  msg_module_type: msg type
 * @param  sub_type: sub type of module type
 * @param  cmd: see cmd definition
 * @param  len len of *p point buf
 * @param  p point
 * @return BLE_ERR
 */
BLE_ERR ble_peri_sendMsgToApp(uint8 msg_module_type, uint8 sub_type, uint8 cmd, uint8 len, void * p)
{
    APP_PERIPHERAL_MSG *peri_Msg = NULL;

    peri_Msg = osPoolAlloc(app_msg_pool_id);
    if(NULL == peri_Msg)
    {
        return BLE_APP_MSG_POOL_ALLOCATION_ERR;
    }
    peri_Msg->msg_module_type = msg_module_type;
    peri_Msg->st_peri.sub_type =  sub_type;
    peri_Msg->st_peri.command =  cmd;
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
