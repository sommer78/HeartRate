/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		rtl876x_it.c
* @brief	IR demo--interrupt function implementation
* @details
* @author	tifnan
* @date 	2015-05-25
* @version	v0.1
*********************************************************************************************************
*/

#include "rtl876x_ir.h"
#include "ir_protocol.h"
#include "trace.h"

/* globals */
extern uint8_t RxCount;
extern uint8_t RxEndFlag;
extern IRDA_BUF IrBuf;

void IRIntrHandler(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IRIntrHandler", 0);
   
    uint8_t count = 0;
    
    /* rx start */
    if(IR_GetFlagState(IR, IR_FLAG_RX_START) == SET)
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IR rx start", 0);  
        IR_ClearINTPendingBit(IR, IR_INT_CLEAR_RX_START); 
    }
    
    /* rx threshold */
    if(IR_GetFlagState(IR, IR_FLAG_RX_TH) == SET)
    {
        count = IR_GetRxFifoSize(IR);
        IR_ReceiveData(IR, IrBuf.pBuf + RxCount, count);
        IrBuf.pBufLen+= count;
        RxCount+= count;
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IR rx thd : 0x%x", 1, count);
        IR_ClearINTPendingBit(IR, IR_INT_CLEAR_RX_TH);
    }
    
    /* rx end */
    if(IR_GetFlagState(IR, IR_FLAG_RX_END) == SET)
    {
        count = IR_GetRxFifoSize(IR);
        IR_ReceiveData(IR, IrBuf.pBuf + RxCount, count);
        IrBuf.pBufLen+= count;
        RxCount+= count;
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IR rx : 0x%x", 1, count);
        IR_ClearINTPendingBit(IR, IR_INT_CLEAR_RX_END);
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IR rx end", 0);
    
        RxEndFlag = 1;
    }
    
    NVIC_ClearPendingIRQ(IR_IRQ);

    return;
}

