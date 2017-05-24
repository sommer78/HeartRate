/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    application.h
  * @brief   Application task related interfaces. 
  * @details
  * @author  ranhui
  * @date    11-March-2015
  * @version v0.2
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#ifdef __cplusplus
 extern "C" {
#endif

/** Add Includes here **/
#include "rtl_types.h"
#include "blueapi_types.h"

/** Add all public functions here **/
extern void application_task_init(void);
void centralBlueAPIMessageCB(PBlueAPI_UsMessage pMsg);

#ifdef __cplusplus
}
#endif

#endif

