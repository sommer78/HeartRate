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

extern uint16 little_endian_read_16(const uint8 * buffer, int32 pos);

BLE_STATUS g_ble_Status;
bond_info_t le_bond_devices[LE_BOND_INFO_SIZE] __attribute__((aligned(4)));//__attribute__ ST_RET_RAM_ZERO;
extern int le_bond_device_db_find_info(uint8 * rand, uint16 ediv);
extern void le_bond_device_db_clear_all(void);
extern int le_bond_device_db_add(uint8 * data);
extern void le_bond_device_db_remove(uint8 index);
uint32      g_flag_write_to_flash = 0;

#define USER_FLASH_SECTOR_NUM                   511
#define USER_FLASH_SECTOR_NUM_BOND_DEVICE_INFO  (USER_FLASH_SECTOR_NUM)
#define USER_FLASH_ADDRESS_BOND_DEVICE_INFO    (USER_FLASH_SECTOR_NUM << 7) // SectorNum * 512 / 4

#ifdef ANCS_FUNC_EN
    timer_node_t    ancs_timer;
    uint8           ancs_discover_flag = 0;
    uint8           ancs_cnt = 5;
    uint8		    ancs_hdl;
#endif


uint8 scanData[20] __attribute__((aligned(4))) =
{
    19,
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'm',
    'x',
    'd',
    'b',
    'l',
    'e',
    '-',
    'e',
    'v',
    'a',
    'l',
    'u',
    'a',
    't',
    'i',
    'o',
    'n',
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
    g_gap_adv_para.advType8bit = GAP_ADTYPE_ADV_IND;
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
 * @brief  send data to ble peripheral
 * @param  none
 * @return none
 */
void ble_app_init(void)
{
    char name[20] = "evaluation device";
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
    uint8* pdata=(uint8*)p_msg->p;
//    int i=0;
    switch (p_msg->command)
    {
        case MSG_BLE_CONNECTED_IND:
        {
            /*
            13 byte:
            0   :  peerAddrType
            1-6  :  peerAddr
            7-8:    interval1250us
            9-10:   conn_latency
            11-12:  sTimeOut10ms
            */
            g_ble_Status.is_ble_connected = TRUE;
        #ifdef ANCS_FUNC_EN
            ancs_init(p_msg->connect_handle);
            ancs_hdl = p_msg->connect_handle;
        #endif
        #ifdef HCI_FUNC_EN
            hci_le_connection_complete_event(p_msg->connect_handle, 0);
        #endif
            ble_api_send_security_req(p_msg->connect_handle);
            // ble_api_linklayer_version_req(p_msg->connect_handle);
            if (pdata)
            {
                APP_DBG(("ble is conected-peer addr:%x-%x-%x-%x-%x-%x\n",pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[6]));
            }
        }
            break;

        case MSG_BLE_DISCONNECTED_IND:
        #ifdef HCI_FUNC_EN
            hci_disconnect_complete_event(p_msg->connect_handle);
        #endif
            g_ble_Status.is_ble_connected = FALSE;
            g_ble_Status.is_ble_encrypted = FALSE;

            ancs_discover_flag = 0;
            ble_api_adv_start();
            break;
        case MSG_BLE_SCAN_REPORT_IND:
        {
        #ifdef HCI_FUNC_EN
            hci_le_adv_report_event(); 
        #endif
            break;
        }
        case MSG_BLE_ENCRYPTED_IND:
            g_ble_Status.is_ble_encrypted = TRUE;
        #ifdef ANCS_FUNC_EN
            ancs_discover_svc_by_uuid(p_msg->connect_handle, 1);
            timer_start(&ancs_timer, 3000, NULL);
        #endif
        #ifdef HCI_FUNC_EN
            hci_encryption_chg_event(p_msg->connect_handle, 0, 1);
        #endif    
            break;

        case MSG_BLE_UNENCRYPTED_IND:
            g_ble_Status.is_ble_encrypted = FALSE;
        #ifdef HCI_FUNC_EN
            hci_encryption_chg_event(p_msg->connect_handle, 0, 0);
        #endif  
            break;


        case MSG_USER_PASSKEYREQ_IND:
        {
            //app decide how to get passkey and send it to stack
            {
#ifdef HCI_FUNC_EN
                uint8   data[2];
                data[0] = MXD_PASSKEY_INPUT_IND;
                data[1] = p_msg->connect_handle;
                hci_event_generate(MXD_SPECIFIED_EVENT_CODE, 2, data);
#endif
            }
            break;
        }
        case MSG_USER_PASSKEY_DISPLAY_IND:
        {
            //app decide how to present the passkey

            {
#ifdef HCI_FUNC_EN
                uint8   data[6];
                data[0] = MXD_PASSKEY_DISPLAY_IND;
                data[1] = p_msg->connect_handle;
                memcpy(data, 2,p_msg->p);
                hci_event_generate(MXD_SPECIFIED_EVENT_CODE, 6,data);
#endif
            }
            break;
        }
        case MSG_USER_PASSKEY_DISPLAY_CANCEL_IND:
        {
            //app cancel passkey display

            {
#ifdef HCI_FUNC_EN
                uint8  data[2];

                data[0] = MXD_PASSKEY_DISPLAY_CANCEL_IND;
                data[1] = p_msg->connect_handle;
                hci_event_generate(MXD_SPECIFIED_EVENT_CODE, 2,data);
#endif
            }
            break;
        }
        case MSG_KEYPRESS_NOTIFICATION:
        {
            //app

            {
#ifdef HCI_FUNC_EN
                uint8   data[4];
                data[0] = MXD_KEYPRESS_NOTIFICATION;
                data[1] = p_msg->connect_handle;
                data[2] = p_msg->length;
                hci_event_generate(MXD_SPECIFIED_EVENT_CODE, 3,data);
#endif
            }
            break;
        }

        case MSG_BLE_ANCS_RPT_REPORT_IND:
        {
            if (p_msg->p)
            {
            #ifdef ANCS_FUNC_EN
                ancs_data_ntf_handler(p_msg->length - 3, (uint8 *)(p_msg->p) + 3);
            #endif
            #ifdef HCI_FUNC_EN
                hci_event_generate(MXD_SPECIFIED_EVENT_CODE, p_msg->length, (uint8 *)(p_msg->p));
            #endif
            }
            break;
        }
        case MSG_BLE_ANCS_INQUIRY_RSLT:
        {
        #ifdef ANCS_FUNC_EN
		if(pdata)
            {
                uint8 msg= pdata[0];
                if (ANCS_SERVER_NOT_FOUND ==  msg)
                {
                    //not find ancs server
                }
                else if (ANCS_SERVER_FOUND ==  msg)
                {//find all char of ancs server
                    ancs_all_characteristic_found_handler(p_msg->connect_handle,ANCS_SERVER_FOUND);
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
            if (p_msg->p)
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
        
        case MSG_BLE_HIGH_DUTY_ADV_END_IND:
        #ifdef HCI_FUNC_EN
            hci_le_connection_complete_event(p_msg->connect_handle, DIRECTED_ADVERTISING_TIMEOUT_HCE);
         #endif
            break;

#if 0
        case MSG_BLE_ADV_STATE_CHANGE_IND:
        {
            APP_DBG(("MSG_BLE_ADV_STATE_CHANGE_IND=%d-- \n",p_msg->para ));
        }
        break;
        case MSG_BLE_SCAN_STATE_CHANGE_IND:
        {
              APP_DBG(("MSG_BLE_SCAN_STATE_CHANGE_IND=%d-- \n",p_msg->para ));
        }
        break;	
#endif
		
        case MSG_LL_CONN_UPDATE_COMPLETE_EVENT:

        {
#ifdef HCI_FUNC_EN
         //   uint8 pa=p_msg->para;
          
            if (pdata)
            {
                /*
                6 byte:
                0-1:    interval1250us
                2-3:   conn_latency
                4-5:  sTimeOut10ms
                */
                  APP_DBG(("MSG_LL_CONN_UPDATE_COMPLETE_EVENT=%d\n",pdata[0]));
                APP_DBG(("new intv=0x%x, latcy=0x%x,tout=0x%x\n",little_endian_read_16(pdata, 1),little_endian_read_16(pdata, 3),little_endian_read_16(pdata, 5)));

            }
            hci_le_cnnct_update_complete_event(p_msg->connect_handle, pdata);
            #endif
            }
            break;
        
        case MSG_LL_CONN_PKT_EVENT_COMPLETE:
				{
        #ifdef HCI_FUNC_EN
            hci_num_of_complete_pkt_event(p_msg->connect_handle);
        #endif
        	}
            break;
            
        case MSG_NOTIFY_DATA_REPORT:
            
            if (p_msg->p)
            {
            #ifdef HCI_FUNC_EN
                hci_event_generate(MXD_SPECIFIED_EVENT_CODE, p_msg->length, p_msg->p);
            #endif
            }
            break;
            
        case MSG_MTU_EVENT_REPORT:
            APP_DBG(("MSG_MTU_EVENT_REPORT=%d\n",p_msg->length));
#ifdef HCI_FUNC_EN
            hci_le_mtu_rpt_event(p_msg->connect_handle, 0);
#endif
            break;
        case MSG_PAIR_COMPLETED:
        {
            //pair completed
            /*
            0: connect idx;
            1:    peer_addr_type,1BYTE ;;
              2-7:               peer_address,6BYTE ;
              8-23:               peer_irk,16BYTE ;
               24-39: local_ltk,16BYTE ;
               40-47:local_rand,8BYTE ;
               48-49:  local_ediv,2BYTE ;
                            */

            if (p_msg->p)
            {
                if (0<=   le_bond_device_db_add(pdata))
                {
                    //save bond info ok
                    APP_DBG(("MSG_PAIR_COMPLETED=save bond ok"));
                }
                else
                {
                    //save bond info  fail
                    //   LOG(0xd792,sizeof(bond_info_t));
                    APP_DBG(("MSG_PAIR_COMPLETED=save bond error---"));
                }

            }

            break;
        }
        case MSG_PAIR_FAIL:
        {
            //pair FAIL
            //  , FAIL REASON
            APP_DBG(("MSG_PAIR_FAIL  REASON=%d\n",p_msg->length));
            break;
        }
        case MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR:
        {
            //LTK REQ
            /*

            0-7:           local_rand,8BYTE ;
            8-9:            local_ediv,2BYTE ;
                */
            uint16 ediv=little_endian_read_16(pdata,8);

            uint8 idx=p_msg->connect_handle;
            int index=le_bond_device_db_find_info(pdata,   ediv);
            APP_DBG(("MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR=\n "));


            if (0 <=  index)
            {
                ble_api_long_term_key_request_reply(idx,le_bond_devices[index].ltk);
                APP_DBG(("MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR= OK_reply\n"));
            }
            else
            {
                ble_api_long_term_key_request_negative_replay(idx);
                APP_DBG(("MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR= negative_reply\n"));

            }
            break;
        }
#if 0
        case MSG_BLE_LL_VERSION_IND:
        {
            APP_DBG(("MSG_BLE_LL_VERSION_IND--version=%d--compID=0x%x\n",(p_msg->para>>16)&0xff,p_msg->para&0xffff));
        }
        break;
#endif
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

void le_bond_device_load_from_flash()
{
//to be done
}

void le_bond_device_save_to_flash()
{
//to be done
}

void le_bond_device_db_clear_all(void)
{
    uint32 i;
    for (i = 0; i < LE_BOND_INFO_SIZE; i++)
    {
        le_bond_device_db_remove(i);
    }
}

// @returns number of device in db
uint32 le_bond_device_db_count(void)
{
    int i;
    uint32 counter = 0;
    for (i = 0; i < LE_BOND_INFO_SIZE; i++)
    {
        if (le_bond_devices[i].saved == SAVE_VALID_FLAG) counter++;
    }
    return counter;
}

void le_bond_device_db_remove(uint8 index)
{
    le_bond_devices[index].saved= 0;
}

int le_bond_device_db_add(uint8 * data)
{
    /*
      0: idx;
      1:    peer_addr_type,1BYTE ;;
        2-7:               peer_address,6BYTE ;
        8-23:               peer_irk,16BYTE ;
         24-39: local_ltk,16BYTE ;
         40-47:local_rand,8BYTE ;
         48-49:  local_ediv,2BYTE ;
    */
    
    int index = -1;
#if 0
    int i;
    for (i = 0; i < LE_BOND_INFO_SIZE; i++)
    {
        if (le_bond_devices[i].saved != SAVE_VALID_FLAG)
        {
            index = i;

            break;
        }
    }
    if (index < 0) return -1;
#endif
index = 0;
    le_bond_devices[index].saved= SAVE_VALID_FLAG;
    le_bond_devices[index].addr_type = data[1];
    memcpy(le_bond_devices[index].addr, data+2, 6);
    memcpy(le_bond_devices[index].irk, data+8, 16);
    memcpy(le_bond_devices[index].ltk, data+24, 16);
    memcpy(le_bond_devices[index].rand, data+40, 8);
    le_bond_devices[index].ediv=little_endian_read_16(data,48);
     // g_flag_write_to_flash = 1;

    return index;
}
int le_bond_device_db_save_to_flash_when_change(void)
{
//  APP_DBG(("Save To Flash =%d\n",g_flag_write_to_flash));
    if (0 != g_flag_write_to_flash)
    {
        int ret =   ble_api_check_sleep_status_flash_can_earse();

        if (ret>0)
        {
#if 1
            user_flash_sector_erase(USER_FLASH_SECTOR_NUM_BOND_DEVICE_INFO);

            user_flash_write(USER_FLASH_ADDRESS_BOND_DEVICE_INFO,  sizeof(le_bond_devices) >>2 ,
                             (uint32 *)&(le_bond_devices[0].saved));
#endif

            APP_DBG(("Save To Flash Sucess\n"));
            g_flag_write_to_flash = 0;
            return 1;
        }
    }
    return 0;
}
void le_bond_device_db_load_from_flash(void)
{
    user_flash_read(USER_FLASH_ADDRESS_BOND_DEVICE_INFO,  sizeof(le_bond_devices)>>2  ,
                    (uint32 *)&(le_bond_devices[0].saved));
}

// get device information: addr type and address
int le_bond_device_db_find_info(uint8 * rand, uint16 ediv)
{
    int i;
    int index = -1;
    for (i = 0; i < LE_BOND_INFO_SIZE; i++)
    {
        if (le_bond_devices[i].saved == SAVE_VALID_FLAG)
        {
            if (ediv == le_bond_devices[i].ediv &&
                0==memcmp2(rand,le_bond_devices[i].rand,8))
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

