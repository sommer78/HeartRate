/******************************************************************************

  Copyright (C), 2006-2015, Maxscend Co., Ltd.

 ******************************************************************************
  File Name     : gap.h
  Version       : Initial Draft
  Author        : maxscend\yanping.wang
  Created       : 2016/11/1
  Last Modified :
  Description   : gap.c header file
  Function List :
  History       :
  1.Date        : 2016/11/1
    Author      : maxscend\yanping.wang
    Modification: Created file

******************************************************************************/

#ifndef __GAP_H__
#define __GAP_H__

#include "ll_struct.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{

    // MITM protection not required
    // No encryption required
    // No user interaction required
    LEVEL_0 = 0,

    // MITM protection not required
    // No encryption required
    // Minimal user interaction desired
    LEVEL_1,

    // MITM protection not required
    // Encryption required
    LEVEL_2,

    // MITM protection required
    // Encryption required
    // User interaction acceptable
    LEVEL_3,

    // MITM protection required
    // Encryption required
    // 128-bit equivalent strength for link and encryption keys required (P-192 is not enough)
    // User interaction acceptable
    LEVEL_4,
} gap_security_level;

typedef enum
{
    GAP_SECURITY_NONE,
    GAP_SECUIRTY_ENCRYPTED,     // SSP: JUST WORKS
    GAP_SECURITY_AUTHENTICATED, // SSP: numeric comparison, passkey, OOB
    // GAP_SECURITY_AUTHORIZED
} gap_security_state;

typedef enum
{
    GAP_CONNECTION_INVALID,
    GAP_CONNECTION_ACL,
    GAP_CONNECTION_SCO,
    GAP_CONNECTION_LE
} gap_connection_type_t;


typedef enum
{
    GAP_RANDOM_ADDRESS_TYPE_OFF = 0,
    GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE,
} gap_random_address_type_t;

typedef enum GAP_EVENT_SM
{
    EVENT_CHANGE_MTU = 1,
    EVENT_SM_AES_START = 2,

} GAP_EVENT_SM_ENUM;

extern void gap_advertisements_enable(int32 enabled, void* adv_para);
extern void gap_discovery_enable(int32 enabled,  void * scan_para);
extern void gap_create_connection(void * p);
extern void gap_terminate_connection(uint8 idx, uint8 status);
extern void gap_notify_to_app(uint16 event);
extern void gap_random_address_timer(void);
extern void gap_random_address_set(uint8 * addr, uint8 set, gap_random_address_type_t random_address_type);
extern void gap_get_mac_address(t_device_addr * dev_addr);
extern uint8 gap_add_white_list(void* white_list);
extern uint8 gap_remove_white_list(void* white_list);
extern void gap_clear_white_list(void);
extern int8 mxd_calc_dbm_from_read_rssi(uint16 rssi);
extern int8  gap_get_con_rssi(uint8 idx);
extern void gap_get_con_para(uint8 idx,uint16* intv,uint16 * laty,uint16 *tout);
extern int gap_get_mtu(uint8 idx);

extern gap_random_address_type_t gap_random_adress_type;
extern uint32 gap_random_adress_update_period;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __GAP_H__ */
