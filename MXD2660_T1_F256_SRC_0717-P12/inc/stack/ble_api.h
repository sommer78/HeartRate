/**
****************************************************************************
* @file ble_api.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2016/12/19
* @history
* @note
* detailed description

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
/** @defgroup 模块名 模块的说明文字
* @{
*/
#ifndef __BLE_API_H__
#define __BLE_API_H__

#include "ll_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

extern void ble_api_adv_set_para(t_adv_para* adv_para);
extern bool ble_api_adv_start(void);
extern void ble_api_adv_stop(void);
extern void ble_api_append_whitelist(t_white_list *addr);
extern void ble_api_clear_whitelist(void);
extern void ble_api_conn_update(t_connct_update_para* update_para);
extern void ble_api_creat_conn(t_init_connct_para* conn_para);
extern uint16 ble_api_device_config_appearance_get(void);
extern void ble_api_device_config_appearance_set(uint16 appearance);
extern void ble_api_device_config_device_name(char* dev_name);
extern void ble_api_encrypt_enable(void);
extern void ble_api_encrypt_oob(void);
extern void ble_api_encrypt_passkey(void);
extern void ble_api_get_device_addr(t_device_addr *addr);
extern void ble_api_privacy_mode_enable(uint8 idx);
extern void ble_api_remove_whitelist(t_white_list *addr);
extern void ble_api_scan_set_para(t_scan_para* scan_para);
extern void ble_api_scan_start(void);
extern void ble_api_scan_stop(void);
extern void ble_api_set_device_addr(t_device_addr *addr);
extern void ble_api_set_tx_power(uint8 tx_idx);
extern void ble_api_terminate_conn(uint8 idx, uint8 status);
extern void ble_api_send_security_req(uint8 conn_idx);

/**
* @brief set notify flag, after tx buf send over ll will send a msg to APP
* @param flag 1:ll will send msg after tx buf send over
* @return NONE
*/
extern void ble_api_notify_flag_set(uint8 flag);

/**
* @brief  update IRK
* @param none
*/
extern void ble_api_update_irk(void);

/**
* @brief  get IRK
* @param none
* @return uint8 [16]
*/
extern uint8*   ble_api_get_irk(void);

extern void ble_api_long_term_key_request_reply(uint8 conn_idx,uint8* ltk);
extern void ble_api_long_term_key_request_negative_replay(uint8 conn_idx);

extern void ble_api_sm_bond_info_save_by_app_config(uint32 onoff);
extern void ble_api_att_exchange_mtu_request(uint8 conn_idx);
extern void ble_api_att_auto_exchange_mtu_onoff(int onoff);

extern void ble_api_linklayer_start_long_length_packet_req(uint8 conn_idx);
extern void ble_api_linklayer_1m2m_phy_updata(uint8 conn_idx,uint8 tx_mode,uint8 rx_mode);
extern void ble_api_linklayer_version_req(uint8 conn_idx);

extern int ble_api_check_sleep_status_flash_can_earse(void);
extern void ble_api_wakeup_during_latency_onoff(int onoff);

extern t_adv_para g_gap_adv_para;
extern t_scan_para g_gap_scan_para;
extern t_init_connct_para g_gap_connect_para;
extern char gap_device_name[20];
extern uint16 gap_appearance;
extern uint8 g_notify_flag;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLE_API_H__ */
/** @} */ // 模块结尾
