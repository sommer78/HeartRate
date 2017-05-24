/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      profile_init.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2015-3-27
* @version   v0.1
* *********************************************************************************************************
*/
#ifndef _PROFILE_INIT_H
#define _PROFILE_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

extern void MouseStartAdv(void);
extern void MouseStopAdv(void);
extern void MouseEnablePairing(void);
extern void Mouse_StartAdv_ind(void);
extern void Mouse_StartAdv_direct_ind_hd(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROFILE_INIT_H */

