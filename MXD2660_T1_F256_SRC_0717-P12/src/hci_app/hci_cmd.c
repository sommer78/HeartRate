/**
****************************************************************************
* @file     hci_cmd.c
* @brief
* @author   xiaoyu
* @version  V0.0.0
* @date     2017/06/29
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/

#include "system_config.h"
#include "ll_struct.h"
#include "ll_ctrl.h"
#include "hci_api.h"
#include "gatts_api.h"
#include "ble_api.h"
#include "ble_msg_api.h"
#include "hci_cmd.h"
#include "mxd_trx_service.h"

#ifdef HCI_FUNC_EN

cmd_status_evt status_event_info;
uint8  g_hci_debug_buffer[256];

void hci_cmd_status_event(void)
{
    uint8 data[9];

    data[0] = MXD_CMD_STATUS_CODE;
    
    memcpy(&data[1], &status_event_info.status, sizeof(cmd_status_evt));
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, sizeof(cmd_status_evt) + 1, data);

}

uint16 hci_cmd_dispatch(uint8 * data, uint16 opcode, uint16 payload_length, uint16 tail_length)
{
    uint8 i, is_connection_exist;
    uint8 is_cmplt_event = 0;
    cmplt_event_info.cmdOpcodelo8 = (opcode & 0xFF);
    cmplt_event_info.cmdOpcodehi8 = ((opcode >> 8) & 0xFF);
    cmplt_event_info.numHciCmdPck = 1;

    status_event_info.cmdOpCodelo = opcode & 0xFF;
    status_event_info.cmdOpCodehi = (opcode >> 8) & 0xFF;

    status_event_info.numHciCmdPckts = 1;
    status_event_info.status = 0;
    status_event_info.para0 = 0;
    status_event_info.para1 = 0;
    status_event_info.para2 = 0;
    status_event_info.para3 = 0;
    cmplt_event_info.return_parameters.common_cmplt.status = 0;
    g_cmplt_event_length = 4;

    LOG(0xd103, opcode);
    is_connection_exist = ( g_common_cfg.connect_num > 0);
    switch(opcode)
    {
        case HCI_WRITE_SCAN_PARAMS_EXT:
        {
            t_scan_para *p_hci = (t_scan_para*)g_hci_debug_buffer;//osPoolAlloc(ble_pool_id);
            p_hci->leScanType8bit = data[TO_FIFO_INDEX(0, tail_length)];
            p_hci->leScanInterval16bit = (data[TO_FIFO_INDEX(2, tail_length)] << 8) | data[TO_FIFO_INDEX(1, tail_length)];
            p_hci->leScanWindow16bit = (data[TO_FIFO_INDEX(4, tail_length)] << 8) | data[TO_FIFO_INDEX(3, tail_length)];
            p_hci->ownAddrType8bit =  data[TO_FIFO_INDEX(5, tail_length)];
            p_hci->scanFltPolicy8bit = data[TO_FIFO_INDEX(6, tail_length)];
            ble_api_scan_set_para(p_hci);
            break;
        }
        case HCI_CREATE_CONNCT_EXT:
        {
            uint16  t1, t2;
            t_init_connct_para *p_hci = (t_init_connct_para*)g_hci_debug_buffer;// osPoolAlloc(ble_pool_id);
            p_hci->scanIntvus = ((data[TO_FIFO_INDEX(1, tail_length)] << 8) | data[TO_FIFO_INDEX(0, tail_length)]) * 625;
            p_hci->scanWinus = ((data[TO_FIFO_INDEX(3, tail_length)] << 8) | data[TO_FIFO_INDEX(2, tail_length)]) * 625;
            p_hci->connPolicy = data[TO_FIFO_INDEX(4, tail_length)];
            p_hci->peerAddrType = data[TO_FIFO_INDEX(5, tail_length)];
            for(i = 0; i < 6; i++)
            {
                p_hci->Peer_Address[i] = data[TO_FIFO_INDEX(6 + i, tail_length)];
            }
            p_hci->ownAddrType = data[TO_FIFO_INDEX(12, tail_length)];
            p_hci->connIntv1250us = (data[TO_FIFO_INDEX(14, tail_length)] << 8) | data[TO_FIFO_INDEX(13, tail_length)];
            t1 = (data[TO_FIFO_INDEX(16, tail_length)] << 8) | data[TO_FIFO_INDEX(15, tail_length)];
            p_hci->conn_latency = (data[TO_FIFO_INDEX(18, tail_length)] << 8) | data[TO_FIFO_INDEX(17, tail_length)];
            p_hci->sTimeOut10ms = (data[TO_FIFO_INDEX(20, tail_length)] << 8) | data[TO_FIFO_INDEX(19, tail_length)];
            p_hci->ceLength625us = (data[TO_FIFO_INDEX(22, tail_length)] << 8) | data[TO_FIFO_INDEX(21, tail_length)];
            t2 = (data[TO_FIFO_INDEX(24, tail_length)] << 8) | data[TO_FIFO_INDEX(23, tail_length)];

            p_hci->connIntv1250us = (p_hci->connIntv1250us + t1) / 2;
            p_hci->ceLength625us = (p_hci->ceLength625us + t2) / 2;

            ble_api_creat_conn(p_hci);
            break;
        }
        case HCI_RESET_EXT:
        {
            break;
        }
        case HCI_READ_BUFF_NUM:
        {
            status_event_info.para0 = 4;
            break;
        }
        case HCI_SET_ADV_PARAM_EXT:
        {
            uint8 * p_hci_msg = osPoolAlloc(ble_pool_id);
            for(i = 0; i < payload_length; i++)
            {
                p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
            }

            if(ble_msg_AppSendToBle(MSG_BLE_ADVERTISE_ENABLE_REQ, 0, payload_length, (void*)p_hci_msg))
            {
                return FALSE;
            }
            break;
        }
        case HCI_SET_ADV_STOP_EXT:
        {
            ble_api_adv_stop();
            break;
        }
        case HCI_WRITE_SCAN_ENABLE_EXT:
        {
            uint8 enable = data[TO_FIFO_INDEX(0, tail_length)];
            if(enable)
            {
                ble_api_scan_start();
            }
            else
            {
                ble_api_scan_stop();
            }
            break;
        }
        case HCI_DISCONNECT_EXT:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist) 
            {
                p_hci_msg = g_hci_debug_buffer;//osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }

                ble_api_terminate_conn(p_hci_msg[0], p_hci_msg[1]);
            }
            else
            {
                status_event_info.status = 1;
            }
            break;
        }
        case HCI_CONNCT_CANCEL_EXT:
            ble_msg_AppSendToBle(MSG_BLE_CREATE_CONNCT_CANCEL_REQ, 0, 0, NULL);
            break;

        case HCI_CONNCT_UPDATE_EXT:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_BLE_CONNECTION_UPD_REQ, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }
            break;
        }
        case HCI_SM_Para_Set:
        {
            uint8 * p_hci_msg = osPoolAlloc(ble_pool_id);
            for(i = 0; i < payload_length; i++)
            {
                p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
            }
            ble_msg_AppSendToBle(MSG_BLE_PAIR_PARA_SET_REQ, 0, payload_length, p_hci_msg);
            break;
        }
        case HCI_SM_Security_Request:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_BLE_SECURITY_REQ, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }
            break;
        }
        case HCI_SM_Send_PassKey:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_USER_PASSKEYREQ_CONF, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }
            break;
        }
        case HCI_GATT_CLIENT_INIT:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_INIT, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_DIS_PRI_SERVS:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_PRI_SERVS, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_DIS_PRI_SERVS_U16:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_PRI_SERVS_U16, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }
            break;
        }
        case HCI_GATT_CLIENT_DIS_PRI_SERVS_U128:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_PRI_SERVS_U128, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_DIS_CHAR_FOR_SERV:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_CHAR_FOR_SERV, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U16:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U16, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U128:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U128, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_DIS_CHAR_DESCRIPTOR:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_DIS_CHAR_DESCRIPTOR, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_FIND_INCLUDED_SERV:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_FIND_INCLUDED_SERV, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_RD_CHAR_VAL_BY_HANDLE:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_RD_CHAR_VAL_BY_HANDLE, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_RD_CHAR_VAL_BY_U16:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_RD_CHAR_VAL_BY_U16, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_RD_CHAR_VAL_BY_U128:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_RD_CHAR_VAL_BY_U128, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_RD_LONG_CHAR_VAL_BY_HANDLE_OFFSET:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_RD_LONG_CHAR_VAL_BY_HANDLE_OFFSET, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_WR_CHAR_VAL_WITHOUT_RESP:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_WR_CHAR_VAL_WITHOUT_RESP, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_WR_CHAR_VAL:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {

                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_WR_CHAR_VAL, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_CLIENT_WR_LONG_CHAR_VAL_OFFSET:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_CLIENT_WR_LONG_CHAR_VAL_OFFSET, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_SERVER_SEND_INDICATION:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_SERVER_SEND_INDICATION, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_GATT_SERVER_SEND_NOTIFICATION:
        {
            uint8 * p_hci_msg;
            if(is_connection_exist)
            {
                p_hci_msg = osPoolAlloc(ble_pool_id);
                for(i = 0; i < payload_length; i++)
                {
                    p_hci_msg[i] = data[TO_FIFO_INDEX(i, tail_length)];
                }
                ble_msg_AppSendToBle(MSG_GATT_SERVER_SEND_NOTIFICATION, 0, payload_length, p_hci_msg);
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_DATA_SEND:
        {
            uint8 ret = 0;
            if(g_conn_cfg[data[TO_FIFO_INDEX(0, tail_length)]].active)
            {
                //0 is the conn handler
                for(i = 0; i < payload_length - 1; i++)
                {
                    g_hci_debug_buffer[i] = data[TO_FIFO_INDEX(i + 1, tail_length)];
                }
                ret = txrx_server_transfer(g_hci_debug_buffer, hci_rx_header.header.hci_acl_header.length - 1, FROM_HCI);
                if(ret)
                {
                    status_event_info.status = ret;
                }
            }
            else
            {
                status_event_info.status = 1;
            }

            break;

        }
        case HCI_CONNCT_GET_MTU_EXT:
        {
            uint8 conn_handle;
            conn_handle = data[TO_FIFO_INDEX(0, tail_length)];

            if(g_conn_cfg[conn_handle].active)
            {
                status_event_info.para0 = g_conn_cfg[conn_handle].mtu & 0xFF;
                status_event_info.para1 = (g_conn_cfg[conn_handle].mtu >> 8) & 0xFF;
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        case HCI_PERI_PARA_SET:
        {
            break;
        }
        case HCI_MXD_READ_REG:
        {
            //update the reg value to para0,para1, para2,para3
            //status_event_info.para0 = g_conn_cfg[conn_handle].mtu & 0xFF;
            //status_event_info.para1 = (g_conn_cfg[conn_handle].mtu >> 8) & 0xFF;

            //user defined cmd, you can add your debug code here, little endian
            uint32 reg_val = 0;
            uint32 reg_addr = (data[TO_FIFO_INDEX(0, tail_length)] | (data[TO_FIFO_INDEX(1, tail_length)] << 8) | (data[TO_FIFO_INDEX(2, tail_length)] << 16) | (data[TO_FIFO_INDEX(3, tail_length)] << 24));
            if((reg_addr < 0x40000000) || (reg_addr > 0x40023000))
            {
                status_event_info.status = 1;
            }
            else
            {
                reg_val = *((volatile uint32*)reg_addr);
                status_event_info.para0 = reg_val & 0xff;
                status_event_info.para1 = (reg_val >> 8) & 0xff;
                status_event_info.para2 = (reg_val >> 16) & 0xff;
                status_event_info.para3 = (reg_val >> 24) & 0xff;
            }
            break;
        }
        case HCI_MXD_WRITE_REG:
        {
            //check whether write success, if success, update status
            //status_event_info.status = 0;
            //user defined cmd, you can add your debug code here, little endian
            uint32 read_val = 0;
            uint32 reg_addr = (data[TO_FIFO_INDEX(0, tail_length)] | (data[TO_FIFO_INDEX(1, tail_length)] << 8) | (data[TO_FIFO_INDEX(2, tail_length)] << 16) | (data[TO_FIFO_INDEX(3, tail_length)] << 24));
            uint32 reg_value = (data[TO_FIFO_INDEX(4, tail_length)] | (data[TO_FIFO_INDEX(5, tail_length)] << 8) | (data[TO_FIFO_INDEX(6, tail_length)] << 16) | (data[TO_FIFO_INDEX(7, tail_length)] << 24));
            if((reg_addr < 0x40000000) || (reg_addr > 0x40023000))
            {
                status_event_info.status = 1;
            }
            else
            {
                *((volatile uint32*)reg_addr) = reg_value;
                read_val = *((volatile uint32*)reg_addr);
                if(read_val == reg_value)
                {
                    status_event_info.status = 0;
                }
                else
                {
                    status_event_info.status = 1;
                }
            }
            break;
        }
        case HCI_CMD_USER_DEFINE:
        {
            //user defined cmd, you can add your debug code here, little endian
            uint32 reg_addr = 0;
            reg_addr = (data[TO_FIFO_INDEX(0, tail_length)] | (data[TO_FIFO_INDEX(1, tail_length)] << 8) | (data[TO_FIFO_INDEX(2, tail_length)] << 16) | (data[TO_FIFO_INDEX(3, tail_length)] << 24));
            reg_addr &= 0xFFFF0000;
            if(reg_addr == HCI_OTA_DEBUG_EN)
            {
                sys_debug_en(2);  //for secondary boot debug
            }
            else if(reg_addr == HCI_DSP_LOG_EN)
            {
                sys_debug_en(1);  //enable dsp log
            }
            else if(reg_addr == HCI_DEBUG_DIS)
            {
                sys_debug_en(0);  //disable boot & dsp log, only app debug can be used
            }
	
            else if(reg_addr == HCI_SYS_TEST)  
            {
            #ifdef SYSTEM_TEST_FUNC_EN
				extern void sys_test(uint8 test_num);
                uint16 case_num = 0;
                case_num = reg_addr & 0x0000FFFF;
                sys_test(case_num);
            #endif
            }
            break;
        }
        case HCI_RESET:
            //re-initialization, it's better not to re initialization right now, set a flag here and re-init before next sleep(this means all things have been processed)
            //cmd complete info will be reported to host after reset done.
            //hci_reset_flag = 1;
            hci_reset_flag_dtm = 1;
            is_cmplt_event = 2;
            break;
        case HCI_LE_RECEIVER_TEST:
            /*if (g_dtm_cfg.dtm_mod > 0)
            {
                LOG(0xde05, g_dtm_cfg.dtm_mod);
                status_event_info.status = DTM_STATUS_ERROR;
                hci_event_generate(CMD_STATUS_EVT_CODE, 4, (uint8 *)&status_event_info.status);
                is_cmplt_event = 2;
            }
            else*/
        {
            g_dtm_cfg.dtm_mod = 2;
            g_dtm_cfg.dtm_freq = data[TO_FIFO_INDEX(0, tail_length)];
            g_dtm_cfg.phy = 1;
            ll_set_dtm_para();
            cmplt_event_info.return_parameters.common_cmplt.status = 0;
            g_cmplt_event_length = 4;
            is_cmplt_event = 1;
        }
        break;
        case HCI_LE_TRANSMITTER_TEST:
            /*if (g_dtm_cfg.dtm_mod > 0)
            {
                LOG(0xde05, g_dtm_cfg.dtm_mod);
                status_event_info.status = DTM_STATUS_ERROR;
                hci_event_generate(CMD_STATUS_EVT_CODE, 4, (uint8 *)&status_event_info.status);
                is_cmplt_event = 2;
            }
            else*/
        {
            g_dtm_cfg.dtm_mod = 1;
            g_dtm_cfg.dtm_freq = data[TO_FIFO_INDEX(0, tail_length)];
            g_dtm_cfg.dtm_data_sz = data[TO_FIFO_INDEX(1, tail_length)];
            g_dtm_cfg.dtm_data_type =  data[TO_FIFO_INDEX(2, tail_length)];
            g_dtm_cfg.phy = 1;

            ll_set_dtm_para();
            cmplt_event_info.return_parameters.common_cmplt.status = 0;
            g_cmplt_event_length = 4;
            is_cmplt_event = 1;

        }
        break;
        case HCI_LE_TEST_END:
            /*if (g_dtm_cfg.dtm_mod == 0)   //if not idle, cannot goto dtm test
            {
                LOG(0xde05, g_dtm_cfg.dtm_mod);
                status_event_info.status = DTM_STATUS_ERROR;
                hci_event_generate(CMD_STATUS_EVT_CODE, 4, (uint8 *)&status_event_info.status);
                is_cmplt_event = 2;
            }
            else*/
        {
            ll_dtm_stop();
            g_dtm_cfg.dtm_mod = 0;
            //need to exit dtm mode, update future wenjie
            cmplt_event_info.return_parameters.le_test_end.status = 0;
            cmplt_event_info.return_parameters.le_test_end.num_of_pktslo8 = (g_dtm_cfg.dtm_recv_packet_cnt & 0xFF);
            cmplt_event_info.return_parameters.le_test_end.num_of_pktshi8 = ((g_dtm_cfg.dtm_recv_packet_cnt >> 8) & 0xFF);
            g_cmplt_event_length = 3 + sizeof(le_test_end_evt);
            is_cmplt_event = 1;

        }
        break;
        case HCI_LE_ENHANCED_RECEIVER_TEST:
            /*if (g_dtm_cfg.dtm_mod > 0)
            {
                LOG(0xde05, g_dtm_cfg.dtm_mod);
                status_event_info.status = DTM_STATUS_ERROR;
                hci_event_generate(CMD_STATUS_EVT_CODE, 4, (uint8 *)&status_event_info.status);
                is_cmplt_event = 2;
            }
            else*/
        {
            g_dtm_cfg.dtm_mod = 2;
            g_dtm_cfg.dtm_freq = data[TO_FIFO_INDEX(0, tail_length)];
            g_dtm_cfg.phy = data[TO_FIFO_INDEX(1, tail_length)];
            ll_set_dtm_para();
            cmplt_event_info.return_parameters.common_cmplt.status = 0;
            g_cmplt_event_length = 4;
            is_cmplt_event = 1;
        }
        break;
        case HCI_LE_ENHANCED_TRANSMITTER_TEST:
            /*if (g_dtm_cfg.dtm_mod > 0)
            {
                LOG(0xde05, g_dtm_cfg.dtm_mod);
                status_event_info.status = DTM_STATUS_ERROR;
                hci_event_generate(CMD_STATUS_EVT_CODE, 4, (uint8 *)&status_event_info.status);
                is_cmplt_event = 2;
            }
            else*/
        {
            g_dtm_cfg.dtm_mod = 1;
            g_dtm_cfg.dtm_freq = data[TO_FIFO_INDEX(0, tail_length)];
            g_dtm_cfg.dtm_data_sz = data[TO_FIFO_INDEX(1, tail_length)];
            g_dtm_cfg.dtm_data_type =  data[TO_FIFO_INDEX(2, tail_length)];
            g_dtm_cfg.phy = data[TO_FIFO_INDEX(3, tail_length)];

            ll_set_dtm_para();
            cmplt_event_info.return_parameters.common_cmplt.status = 0;
            g_cmplt_event_length = 4;
            is_cmplt_event = 1;

        }
        break;
        case HCI_SET_DEV_NAME_ADDR:
        {
            uint8 addr_type = data[TO_FIFO_INDEX(0, tail_length)];
            uint8 i;
            ble_api_adv_stop();
            g_gap_adv_para.ownAddrType8bit = addr_type;
            if(addr_type)
            {
                for ( i = 0 ; i < 6 ; i++ )
                {
                    g_common_cfg.randomAddr6oct[i]  = data[TO_FIFO_INDEX(1 + i, tail_length)];
                }
            }
            else
            {
                for ( i = 0 ; i < 6 ; i++ )
                {
                    g_common_cfg.bt_addr[i] = data[TO_FIFO_INDEX(1 + i, tail_length)];
                }
            }
            for ( i = 0 ; (i < (payload_length - 7)) && ( i < (sizeof(hci_scanData) - 2)) ; i++ )
            {
                hci_scanData[i + 2] = data[TO_FIFO_INDEX(7 + i, tail_length)];
            }
            hci_scanData[0] = i + 1;
            hci_scanData[1] = 0x9;//GAP_ADTYPE_LOCAL_NAME_COMPLETE;
            g_gap_adv_para.scanRspLen = i + 2;
            g_gap_adv_para.scanRspbuf = hci_scanData;

            status_event_info.status = 1;
            break;
        }
        case HCI_SET_ADV_DATA:
        {
            uint8 i;
            ble_api_adv_stop();

            for ( i = 0 ; (i < payload_length) && ( i < sizeof(hci_advData)) ; i++ )
            {
                hci_advData[i] = data[TO_FIFO_INDEX(i, tail_length)];
            }
            g_gap_adv_para.advDataLen = i;
            g_gap_adv_para.advDatabuf = hci_advData;
            break;
        }
        case HCI_SET_SCAN_RSP:
        {
            uint8 i;
            ble_api_adv_stop();

            for ( i = 0 ; (i < payload_length) && ( i < (sizeof(hci_scanData) - 1)) ; i++ )
            {
                hci_scanData[i + 1] = data[TO_FIFO_INDEX(i, tail_length)];
            }
            hci_scanData[0] = i;
            g_gap_adv_para.scanRspLen = i + 1;
            g_gap_adv_para.scanRspbuf = hci_scanData;
            break;
        }
        case HCI_PHY_UPDATE:
        {
            uint8 conn_handle;
            conn_handle = data[TO_FIFO_INDEX(0, tail_length)];

            if(g_conn_cfg[conn_handle].active)
            {
                if((data[TO_FIFO_INDEX(1, tail_length)] > 0) && (data[TO_FIFO_INDEX(1, tail_length)] < 3) && (data[TO_FIFO_INDEX(2, tail_length)] > 0) && (data[TO_FIFO_INDEX(2, tail_length)] < 3))
                {
                    uint8 phy_para = 0;
                    if(data[TO_FIFO_INDEX(1, tail_length)] & 0x2)
                    {
                        phy_para |= (1 << 0);
                    }
                    if(data[TO_FIFO_INDEX(2, tail_length)] & 0x2)
                    {
                        phy_para |= (1 << 1);
                    }
                    g_conn_cfg[conn_handle].phy_new = phy_para;
                    ll_ctrl_start_phy_update(conn_handle);
                }
            }
            else
            {
                status_event_info.status = 1;
            }

            break;
        }
        
#ifdef _PMU_CLK_TEST_CODE_
        case HCI_SET_CLK_TEST:
            {
                uint16 test_code;
                test_code = data[TO_FIFO_INDEX(0, tail_length)] | data[TO_FIFO_INDEX(1, tail_length)]<<8 ;
                pmu_clk_switch_test_func(test_code);
            }
            break;
#endif
        default:
            break;
    }
    return is_cmplt_event;

}

#endif


