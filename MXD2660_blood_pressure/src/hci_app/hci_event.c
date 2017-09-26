/**
****************************************************************************
* @file     hci_event.c
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
#include "hci_event.h"
#include "hci_api.h"
#include "ble_msg_api.h"
#include "gatts_api.h"
#include "ll_ctrl.h"

#ifdef HCI_FUNC_EN

uint8 data_rpt[256];

//special event
void hci_le_adv_report_event(void)
{
    le_adv_report_evt le_adv_report;
    uint16 evt_length; 
    uint8 *data;
    
    le_adv_report.subEvtCode = MXD_ADV_RPT_RESLT_CODE;
    le_adv_report.numReports = 0X01;      // our chip support 1 report
    le_adv_report.addrType = g_adv_cfg.adv_in_pdu.header._B.txAdd1bit;

    memcpy(le_adv_report.addr6Oct, g_adv_cfg.adv_in_pdu.advData.adv_ind_payload->advA6oct, 6) ;

    if(PTYPE_ADV_DIRECT_IND != g_adv_cfg.adv_in_pdu.header._B.pduType4bit)
    {
        le_adv_report.length_data =  g_adv_cfg.adv_in_pdu.header._B.len6bit - 6;      // -6 : !AdvA is not in the len!
        memcpy((uint8*)le_adv_report.data31Oct, (uint8*)g_adv_cfg.adv_in_pdu.advData.adv_ind_payload->AdvData, le_adv_report.length_data);
    }
    else
    {
        le_adv_report.length_data = 0;
    }
    le_adv_report.rssi_S8 = mxd_calc_dbm_from_read_rssi(g_common_cfg.rssi_from_reg);
    * ((uint8 *) le_adv_report.data31Oct + le_adv_report.length_data) = le_adv_report.rssi_S8; //the le_adv_report_evt is bigger than actual report evet, so copy rssi to the last byte so le_adv_report can be report directly

    evt_length =  12 + le_adv_report.length_data;

    le_adv_report.evtType = g_adv_cfg.adv_in_pdu.header._B.pduType4bit;

    if(PTYPE_ADV_SCAN_IND ==   le_adv_report.evtType)
    {
        le_adv_report.evtType = ADV_SCAN_IND;
    }
    else if(PTYPE_ADV_NONCONN_IND ==   le_adv_report.evtType)
    {
        le_adv_report.evtType = ADV_NONCONN_IND;
    }
    //consider how to send message or signal to stack task, , stack master role not support temporary by stack_transfer@wenjie
    //return hci_event_generate(LE_ADV_RPT_EVT_CODE, evt_length, (uint8 *)(&le_adv_report));
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, (uint8 *)&le_adv_report);
    
    data = (uint8*)&le_adv_report;
    gap_scan_adv_report_CB(data + 2, evt_length - 2);
}

void hci_le_connection_complete_event( uint8 idx, uint8 status)
{
    le_cnnct_cmplt_evt le_cnnct_cmplt;
    uint8 evt_length;

    le_cnnct_cmplt.subEvtCode = MXD_CNNCT_CMPLT_CODE;
    le_cnnct_cmplt.status = status;
    if(DIRECTED_ADVERTISING_TIMEOUT_HCE != status)
    {
        le_cnnct_cmplt.cnnctH16bitlo = idx;
        le_cnnct_cmplt.cnnctH16bithi = 0;

        le_cnnct_cmplt.role = (uint8)g_conn_cfg[idx].role;
        le_cnnct_cmplt.peerAddrType = g_conn_cfg[idx].peerAddrType;
        memcpy( le_cnnct_cmplt.peerAddr6Oct, g_conn_cfg[idx].peerAddr, 6);
        le_cnnct_cmplt.cnnctInterval16bitlo = g_conn_cfg[idx].interval1250us & 0xFF;
        le_cnnct_cmplt.cnnctInterval16bithi = (g_conn_cfg[idx].interval1250us >> 8) & 0xFF;

        le_cnnct_cmplt.cnnctLatency16bitlo = g_conn_cfg[idx].conn_latency & 0xFF;
        le_cnnct_cmplt.cnnctLatency16bithi = (g_conn_cfg[idx].conn_latency >> 8) & 0xFF;

        le_cnnct_cmplt.supervisionTimeout16bitlo = g_conn_cfg[idx].sTimeOut10ms & 0xFF;
        le_cnnct_cmplt.supervisionTimeout16bithi = (g_conn_cfg[idx].sTimeOut10ms >> 8) & 0xFF;

        le_cnnct_cmplt.masterClockAccuracy = (g_conn_cfg[idx].hop5bitSca3bit >> 5) & 0x7;
    }
    else
    {
        le_cnnct_cmplt.cnnctInterval16bitlo = 0;
        le_cnnct_cmplt.cnnctInterval16bithi = 0;
        le_cnnct_cmplt.role = 1;
        le_cnnct_cmplt.peerAddrType = g_adv_cfg.drctAddrType8bit;
        memcpy(le_cnnct_cmplt.peerAddr6Oct, g_adv_cfg.drctAddr6oct, 6);
        le_cnnct_cmplt.cnnctInterval16bitlo = 0;
        le_cnnct_cmplt.cnnctInterval16bithi = 0;
        le_cnnct_cmplt.cnnctLatency16bitlo = 0;
        le_cnnct_cmplt.cnnctLatency16bithi = 0;
        le_cnnct_cmplt.supervisionTimeout16bitlo = 0;
        le_cnnct_cmplt.supervisionTimeout16bithi = 0;
        le_cnnct_cmplt.masterClockAccuracy = 0;
    }
    evt_length = 19;
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, (uint8 *)&le_cnnct_cmplt);
}


void hci_disconnect_complete_event(uint8 idx)
{
    discnnt_cmplt_evt discnnt_cmplt;
    uint8 evt_length;

    discnnt_cmplt.subEvtCode = MXD_DISCNNCT_CMPLT_CODE;
    discnnt_cmplt.status = 0;
    discnnt_cmplt.cnnctHandlelo8 = idx;
    discnnt_cmplt.cnnctHandlehi8 = 0;
    discnnt_cmplt.reason = g_conn_cfg[idx].disconn_reason;
    evt_length = 5;
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, (uint8 *)&discnnt_cmplt);
}


void hci_encryption_chg_event(uint8 idx, uint8 status, uint8  Encryption_Enabled)
{

    enc_chg_evt enc_chg;
    uint8 evt_length;
    enc_chg.subEvtCode = MXD_ENCRYPTION_STATUS_CHG_CODE;
    enc_chg.status = status;
    enc_chg.cnnctHlo8 = idx;
    enc_chg.cnnctHhi8 = 0;
    enc_chg.Encryption_Enabled = Encryption_Enabled;
    evt_length = 5;
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, (uint8 *)&enc_chg);

}


void hci_le_cnnct_update_complete_event(uint8 idx, uint8 *p_status)
{
    le_connection_update_cmplt_evt le_connection_update_cmplt;
    uint8 evt_length;
    le_connection_update_cmplt.subEvtCode = MXD_CNNCT_UPDATE_CODE;
    le_connection_update_cmplt.status = p_status[0];
    le_connection_update_cmplt.cnnctH16bitlo = idx;
    le_connection_update_cmplt.cnnctH16bithi = 0;

    le_connection_update_cmplt.cnnctInterval16bitlo = g_conn_cfg[idx].interval1250us & 0xFF;
    le_connection_update_cmplt.cnnctInterval16bithi = (g_conn_cfg[idx].interval1250us >> 8) & 0xFF;

    le_connection_update_cmplt.cnnctLatency16bitlo = g_conn_cfg[idx].conn_latency & 0xFF;
    le_connection_update_cmplt.cnnctLatency16bithi = (g_conn_cfg[idx].conn_latency >> 8) & 0xFF;

    le_connection_update_cmplt.supervisionTimeout16bitlo = g_conn_cfg[idx].sTimeOut10ms & 0xFF;
    le_connection_update_cmplt.supervisionTimeout16bithi = (g_conn_cfg[idx].sTimeOut10ms >> 8) & 0xFF;

    evt_length = 10;
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, (uint8 *)&le_connection_update_cmplt);
}

void hci_le_mtu_rpt_event(uint8 idx, uint8 status)
{
    uint8 data[6];

    data[0] = MXD_MTU_RPT_CODE;
    data[1] = status;
    data[2] = idx;
    data[3] = 0;
    data[4] = g_conn_cfg[idx].mtu & 0xFF;
    data[5] = ((g_conn_cfg[idx].mtu >> 8) & 0xFF);
    LOG(0xd81a, g_conn_cfg[idx].mtu);

    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, 6, data);

}

void hci_num_of_complete_pkt_event(uint8 idx)
{
    num_of_cmplt_pkt_evt num_of_cmplt_pkt;
    uint8 evt_length;

    num_of_cmplt_pkt.subEvtCode = MXD_NUM_OF_CMPLT_PKTS_CODE;

    num_of_cmplt_pkt.Number_of_Handles = 1;
    num_of_cmplt_pkt.Connection_Handlelo8 = idx;
    num_of_cmplt_pkt.Connection_Handlehi8 = 0;
    __disable_irq();
    num_of_cmplt_pkt.HC_Num_Of_Completed_Packetslo8 = g_conn_cfg[idx].num_of_pkt_send_cmpt;
    num_of_cmplt_pkt.HC_Num_Of_Completed_Packetshi8 = 0;
    g_conn_cfg[idx].num_of_pkt_send_cmpt = 0;
    __enable_irq();
    evt_length = 6;
    
    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, (uint8 *)&num_of_cmplt_pkt);

}

void hci_data_rpt_event(uint8 idx, uint8 * value,  uint8 length)
{
    uint8 evt_length;

    data_rpt[0] = MXD_DATA_RPT_CODE;
    data_rpt[1] = idx;
    data_rpt[2] = 0;

    memcpy(&data_rpt[3], value, length);
    evt_length = length + 3;

    hci_event_generate(MXD_SPECIFIED_EVENT_CODE, evt_length, data_rpt);
}


#endif

