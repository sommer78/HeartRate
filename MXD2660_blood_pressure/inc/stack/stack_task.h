/**
****************************************************************************
* @file      stack_task.h
* @brief     the task system have always
* @author    xiaoyu
* @version   V1.0.0
* @date      2016/12/16
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/


#ifndef __STACK_TASK_H__
#define __STACK_TASK_H__

#include "device_cfg.h"

#define ST_TASK_ST_SIZE       500
#define ENCRYPT_TASK_ST_SIZE  248
#define LL_CTRL_TSK_SIZE      216

#ifdef RELEASE_RAM_20KB_VERSION
    #define APP_LAYER_TASK_SIZE   1536
#endif

#ifdef RELEASE_DEFAULT_VERSION
    #define APP_LAYER_TASK_SIZE   2000
#endif


enum
{
    SIGNAL_PENDING_ENC_REQ = (0x1 << 0),
    SIGNAL_PENDING_REJECT = (0x1 << 1),
    SIGNAL_ENC_RSP = (0x1 << 2),
    SIGNAL_CNNCT_SET_IDLE = (0x1 << 3),
    SIGNAL_LL_ACL = (0x1 << 4),
    SIGNAL_LL_RESERVED = (0x1 << 5)
};


enum
{
    SIGNAL_EVENT = (0x1 << 0),
    SIGNAL_ACL_PKT = (0x1 << 1),
};


enum
{
    SIGNAL_CONNECTION_COMPLETE_EVT = (0x1 << 0),
    SIGNAL_DISCNNT_COMPLETE_EVT = (0x1 << 1),
    SIGNAL_CONNECTION_PARAM_REQ_EVT = (0x1 << 2),
    SIGNAL_ENCRYPT_CHG_EVT = (0x1 << 3),
    SIGNAL_ENCRYPT_KEY_REFRESH_EVT = (0x1 << 4),
    SIGNAL_LTK_REQ_EVT = (0x1 << 5),
    SIGNAL_ACL_DATA_RPT = (0x1 << 6),
    SIGNAL_TRIGGER_SM_RUN = (0x1 << 7),
    SIGNAL_TRIGGER_SM_CSRK_RUN = (0x1 << 8),
    SIGNAL_A_TX_BUF_SEND_OVER = (0x1 << 9),
    SIGNAL_ADV_DISABLE_IND = (0x1 << 10),
    SIGNAL_SCAN_DISABLE_IND = (0x1 << 11),
    SIGNAL_TRIG_MSG_RECV = (0x1 << 15),

	
};

enum
{
    SIGNAL_SESSION_KEY = (0x1 << 0),
    SIGNAL_CMAC = (0x1 << 1),
    SIGNAL_AES_ONLY = (0x1 << 2),

};

extern void soft_reset(uint8 reset_flag);
extern void stack_task_init(void);
extern osThreadId ll_ctrl_taskId, st_ctrl_taskId, encrypt_taskId;


#endif



