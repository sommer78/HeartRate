/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     application.h
* @brief    .
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v0.2
*********************************************************************************************************
*/
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "rtl_types.h"
#include "blueapi_types.h"


extern void application_task_init(void);
extern void appBlueAPICallback(PBlueAPI_UsMessage pMsg);



#endif

