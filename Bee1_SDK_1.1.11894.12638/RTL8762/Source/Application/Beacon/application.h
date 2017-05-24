
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "rtl_types.h"
//#include "basic_definitions.h"
#include "blueapi_types.h"


extern void application_task_init(void);
extern void appBlueAPICallback(PBlueAPI_UsMessage pMsg);



#endif

