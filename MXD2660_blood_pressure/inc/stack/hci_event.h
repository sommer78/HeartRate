/**
****************************************************************************
* @file     hci_event.h
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

#ifndef __HCI_EVENT_H__
#define __HCI_EVENT_H__

#include "hci_api.h"

void hci_le_adv_report_event(void);
void hci_le_connection_complete_event( uint8 idx, uint8 status);
void hci_disconnect_complete_event(uint8 idx);
void hci_encryption_chg_event(uint8 idx, uint8 status, uint8  Encryption_Enabled);
void hci_le_cnnct_update_complete_event(uint8 idx, uint8 *p_status);
void hci_le_mtu_rpt_event(uint8 idx, uint8 status);
void hci_num_of_complete_pkt_event(uint8 idx);
void hci_data_rpt_event(uint8 idx, uint8 * value,  uint8 length);
void hci_cmd_status_event(void);


#endif


