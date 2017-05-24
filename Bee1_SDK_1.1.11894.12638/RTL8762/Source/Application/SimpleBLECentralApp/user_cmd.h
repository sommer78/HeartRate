/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _USER_CMD_H_
#define _USER_CMD_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "data_uart.h"
#include "user_cmd_parse.h"

/****************************************************************************************************************
* exported variables other .c files may use all defined here.
****************************************************************************************************************/
extern const TUserCmdTableEntry UserCmdTable[];
extern TUserCmdIF    gUserCmdIF;

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif

