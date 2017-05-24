/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_it.c
* @brief    uart demo--interrupt function implementation
* @details
* @author   tifnan
* @date     2015-05-25
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_uart.h"
#include "trace.h"

/* globals */
extern uint8_t RxBuffer[32];
extern uint8_t RxCount;
extern uint8_t RxEndFlag;

void DataUartIntrHandler(void)
{
    uint32_t int_status = 0;
    
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
            UART_ReceiveData(UART, &RxBuffer[RxCount], 14);
            RxCount+= 14;
            break;
        
        case UART_INT_ID_RX_TMEOUT:
            while(UART_GetFlagState(UART, UART_FLAG_RX_DATA_RDY) == SET)
            {
                RxBuffer[RxCount] = UART_ReceiveByte(UART);
                RxCount++;
            }
            if(RxBuffer[RxCount  - 1] == '\r')
            {
                RxEndFlag = 1;
            }
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
    
    return;
}
