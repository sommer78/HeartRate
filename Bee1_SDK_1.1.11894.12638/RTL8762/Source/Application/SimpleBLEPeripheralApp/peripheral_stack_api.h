/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      peripheral_stack_api.h
* @brief
* @details
* @author    ranhui
* @date      2015-3-27
* @version   v0.1
* *********************************************************************************************************
*/
#ifndef _PROFILE_INIT_H
#define _PROFILE_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

void peripheral_StartAdv_ind(void);
void peripheral_StartAdv_direct_ind_hd(void);
void peripheral_StartAdv_scan_ind(void);
void peripheral_StartAdv_noconn_ind(void);
void peripheral_StartAdv_direct_ind_ld(void);
void peripheral_StartAdv_ind_with_whitelist(void);


void peripheral_StopAdv(void);
void peripheral_EnablePairing(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROFILE_INIT_H */

