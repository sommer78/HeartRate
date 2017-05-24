/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_it.c
* @brief    IR demo--interrupt function implementation
* @details
* @author   tifnan
* @date     2015-05-25
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_ir.h"
#include "ir_protocol.h"
#include "trace.h"

/* globals */
extern uint8_t TxCount ;
extern uint8_t TxEndFlag;
extern IRDA_BUF IrBuf;

void IRIntrHandler(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IRIntrHandler", 0);
    
    if(IR_GetFlagState(IR, IR_FLAG_TX_FIFO_EMPTY) == SET)
    {
        /* continue tx */
        if((NEC_LENGTH - TxCount) >= IR_TX_FIFO_SIZE)
        {
            IR_SendData(IR, IrBuf.pBuf + TxCount, 31);
            TxCount+= 31;
            /* clear interrupt flag */
            IR_ClearINTPendingBit(IR, IR_INT_CLEAR_TX_FIFO_EMPTY);
        }
        else if((NEC_LENGTH - TxCount) != 0)
        {
            IR_SetTxThreshold(IR, NEC_LENGTH - TxCount);
            IR_SendData(IR, IrBuf.pBuf + TxCount, NEC_LENGTH - TxCount);
            TxCount+= (NEC_LENGTH - TxCount);
            /* clear interrupt flag */
            IR_ClearINTPendingBit(IR, IR_INT_CLEAR_TX_FIFO_EMPTY);
        }
        else
        {
            /* tx completed */
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IRIntrHandler: tx completed", 0);
            /* disable IR tx empty interrupt */
            IR_INTConfig(IR, IR_INT_TF_EMPTY, DISABLE);
            TxEndFlag = 1;
            /* clear interrupt flag */
            IR_ClearINTPendingBit(IR, IR_INT_CLEAR_TX_FIFO_EMPTY);
        }
    }
}

