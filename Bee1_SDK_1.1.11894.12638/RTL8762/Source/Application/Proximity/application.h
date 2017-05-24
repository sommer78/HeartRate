/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    application.h
  * @brief   Routines to access hardware
  * @details
  * @author  Chuanguo Xue
  * @date    27-March-2015
  * @version v0.2
  * *************************************************************************************
  */

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bee_message.h"
#include "semphr.h"
#include "blueapi_types.h"
#include "rtl_types.h"


extern void application_task_init(void);
extern void peripheralBlueAPIMessageCB(PBlueAPI_UsMessage pMsg);

#ifdef __cplusplus
}
#endif

#endif /* __APPLICATION_H */
