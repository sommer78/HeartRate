/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     qdecoder.c
* @brief
* @details
* @author   Chuanguo Xue
* @date     2015-04-17
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "board.h"
#include "rtl876x_bitfields.h"
#include "qdecoder.h"
#include "bee_message.h"
#include <string.h>
#include "trace.h"
#include "os.h"
#include "rtl876x_qdec.h"

extern BOOL allowedQDecoder;

//#define DEFAULT_QDECODE_DEBOUNCE_INTERVAL 5500    //us
//#define DEFAULT_QDECODE_SAMPLE_RATE       128000  //Hz

typedef struct _qdec_ctx_t
{
    uint16_t pre_ct;     //previous counter value
    uint16_t cur_ct;     //current counter value
    uint16_t dir;        //1--up; 0-- down
    uint16_t count;      //counter value changes count since dlps back
}qdec_ctx_t;

qdec_ctx_t qdec_ctx;

//called when wake from dlps
void qdec_ctx_clear(void)
{
    memset(&qdec_ctx, 0, sizeof(qdec_ctx_t));
}

void QdecodeIntrHandler()
{
    long xHigherPriorityTaskWoken = pdFALSE;
    BEE_IO_MSG rtl_qdecode_msg;

    /* Mask interrupt */
    QDEC_INTMask(QDEC, QDEC_AXIS_Y, ENABLE);
    
    //qdecoder context
    qdec_ctx.count++;
    qdec_ctx.dir = QDEC_GetAxisDirection(QDEC, QDEC_AXIS_Y);
    qdec_ctx.pre_ct = qdec_ctx.cur_ct;
    qdec_ctx.cur_ct = QDEC_AccValue(QDEC, QDEC_AXIS_Y);
    
    //illegal state(00->11)
    if((QDEC->REG_SR_Y & (1 << 29)))
    {
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "QdecodeIntrHandler, illegal state, direction:%d, counter: 0x%x\n\n", 2, qdec_ctx.dir, qdec_ctx.cur_ct);
        QDEC->REG_CR2 |= ((1 << 10) | (1 << 7));  //clear llegal flag and illegal counter
    }
    
    //clear overflow and underflow flag
    if((QDEC->REG_SR_Y & (1 << 30)) || (QDEC->REG_SR_Y & (1 << 27)))
        QDEC->REG_CR2 |= ((1 << 4) | (1 << 16));
    
    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "QdecodeIntrHandler, direction:%d, counter: 0x%x, count: %d\n\n", 3, qdec_ctx.dir, qdec_ctx.cur_ct, qdec_ctx.count);
    
    //need at least 2 counter value to check direction
    if(qdec_ctx.count == 2)
    {
        rtl_qdecode_msg.IoType = IO_QDECODE_MSG_TYPE;
        rtl_qdecode_msg.subType = qdec_ctx.dir;
        rtl_qdecode_msg.parm = qdec_ctx.count;
		allowedQDecoder = false;
        if (!SendMessageFromISR(&rtl_qdecode_msg, &xHigherPriorityTaskWoken))
        {
            DBG_BUFFER(MODULE_QDECODE, LEVEL_ERROR, " ***Quad decoder SendQueue fail(MSG).\n", 0);
        }
    }
    //directly send to app
    else if(qdec_ctx.count > 2)
    {
        rtl_qdecode_msg.IoType = IO_QDECODE_MSG_TYPE;
        rtl_qdecode_msg.subType = qdec_ctx.dir;
        rtl_qdecode_msg.parm = 1; 
		allowedQDecoder = false;
        if (!SendMessageFromISR(&rtl_qdecode_msg, &xHigherPriorityTaskWoken))
        {
            DBG_BUFFER(MODULE_QDECODE, LEVEL_ERROR, " ***Quad decoder SendQueue fail(MSG).\n", 0);
        }
    }
    else
    {
        //do nothing
    }

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    
    QDEC_ClearINTPendingBit(QDEC, QDEC_FLAG_INT_Y);
    QDEC_INTMask(QDEC, QDEC_AXIS_Y, DISABLE);
}

