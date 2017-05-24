/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      legacyperiphapi.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2015-4-12
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _LEGACYPERIPHAPI_H
#define _LEGACYPERIPHAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl_types.h"

extern uint8_t bee_aon_gp_read(uint8_t offset);
extern void bee_aon_gp_write(uint8_t offset, uint8_t data);
extern VOID (*IO_Enter_DLPS_Hibernate)(VOID);
extern VOID (*IO_Exit_DLPS_Hibernate)(VOID);

#ifdef __cplusplus
}
#endif

#endif /* _LEGACYPERIPHAPI_H */

