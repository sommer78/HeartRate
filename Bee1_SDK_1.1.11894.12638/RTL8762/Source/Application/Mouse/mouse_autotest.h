/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file        mouse_autotest.c
* @brief      mouse auto test application implementation
* @details   mouse auto test application implementation
* @author    Elliot Chen
* @date      2015-08-11
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _MOUSE_AUTOTEST_H_
#define _MOUSE_AUTOTEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl_types.h"
#include "blueapi_types.h"

typedef struct
{
    uint8_t buffer[2];
    uint8_t send_count;
    uint8_t test_count;
} Mouse_AutoTestTypeDef;

extern void AutoTest_TimerInit(void);

#ifdef __cplusplus
}
#endif

#endif
