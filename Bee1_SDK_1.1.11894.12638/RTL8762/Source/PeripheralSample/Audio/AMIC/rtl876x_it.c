/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_it.c
* @brief    Amic demo interrupt handle
* @details
* @author   tifnan
* @date     2015-05-28
* @version  v0.1
*********************************************************************************************************
*/

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "rtl876x_uart.h"
#include "board.h"
#include "trace.h"

void DataUartIntrHandler(void)
{
    uint32_t int_status = 0;
    uint8_t event = 0;
    uint8_t rxByte = 0;
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
                rxByte = UART_ReceiveByte(UART);
            }
            
            event = IO_DEMO_EVENT_UART_RX;
            /* send message to DemoIO task */
            xQueueSendFromISR(DemoIOMessageQueue, &rxByte, &TaskWoken);
            xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);

        break;
        
        /* receive line status interrupt */
        case UART_INT_ID_LINE_STATUS:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Line status error!!!!\n", 0);
        break;      

        default:
        break;
    }
    
    /* enable interrupt again */
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}
