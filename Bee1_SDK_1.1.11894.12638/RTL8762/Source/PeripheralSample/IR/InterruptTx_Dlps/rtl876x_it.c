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
#include "freeRTOS.h"
#include "rtl876x_uart.h"
#include "task.h"
#include "queue.h"
#include "trace.h"
#include "board.h"

/* globals */
extern uint8_t it_tx_count ;
extern IRDA_BUF IrBuf;

//globals
extern xQueueHandle DemoIOMessageQueue;
extern xQueueHandle DemoIOEventQueue;

void IRIntrHandler(void)
{
    uint8_t event;
    portBASE_TYPE TaskWoken = pdFALSE;
    
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IRIntrHandler", 0);
    
    if(IR_GetFlagState(IR, IR_FLAG_TX_FIFO_EMPTY) == SET)
    {
        /* continue tx */
        if((NEC_LENGTH - it_tx_count) >= IR_TX_FIFO_SIZE)
        {
            IR_SendData(IR, IrBuf.pBuf + it_tx_count, 31);
            it_tx_count+= 31;
        }
        else if((NEC_LENGTH - it_tx_count) != 0)
        {
            IR_SetTxThreshold(IR, NEC_LENGTH - it_tx_count);
            IR_SendData(IR, IrBuf.pBuf + it_tx_count, NEC_LENGTH - it_tx_count);
            it_tx_count+= (NEC_LENGTH - it_tx_count);
           
        }
        else
        {
            /* tx completed */
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IRIntrHandler: tx completed", 0);
            it_tx_count = 0;
            event = IO_DEMO_EVENT_IR_TX_DONE;
            /* send message to DemoIO task */
            xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
            /* disable IR tx interrupt */
            IR_INTConfig(IR, IR_INT_TF_EMPTY, DISABLE);
        }
        
         /* clear interrupt flag */
        IR_ClearINTPendingBit(IR, IR_INT_CLEAR_TX_FIFO_EMPTY);
    }
    
    portEND_SWITCHING_ISR(TaskWoken);
}

void DataUartIntrHandler(void)
{
    uint32_t int_status = 0;
    uint8_t event;
    uint8_t rx_data[2];
    uint8_t rx_count = 0;
    uint16_t ir_data;
    portBASE_TYPE TaskWoken = pdFALSE;
    
    /* read interrupt id */
    int_status = UART_GetIID(UART);
    /* disable interrupt */
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, DISABLE);
    
    switch(int_status)
    {
        /* tx fifo empty, not enable */
        case UART_INT_ID_TX_EMPTY:
        break;

        /* rx data valiable */
        case UART_INT_ID_RX_LEVEL_REACH:
            /* rx trigger is 14, not possible */
            break;
        
        case UART_INT_ID_RX_TMEOUT:
            while(UART_GetFlagState(UART, UART_FLAG_RX_DATA_RDY) == SET)
            {
                rx_data[rx_count++] = UART_ReceiveByte(UART);
            }
            
            ir_data = ((uint16_t)(rx_data[0]) << 8) | rx_data[1];
             
            event = IO_DEMO_EVENT_UART_RX;
            /* send message to DemoIO task */
            xQueueSendFromISR(DemoIOMessageQueue, &ir_data, &TaskWoken);
            xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
            
        break;
        
        /* receive line status interrupt */
        case UART_INT_ID_LINE_STATUS:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Line status error!!!!\n", 0);
        break;      

        default:
        break;
    }
    
    /* enable interrupt here */
    //UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
    
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}


