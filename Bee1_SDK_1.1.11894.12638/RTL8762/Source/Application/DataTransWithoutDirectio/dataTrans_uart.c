enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dataTrans_uart.c
* @brief    Data uart operations for testing profiles.
* @details  Data uart init and print data through data uart.
* @author
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_it.h"

/* for freertos interface */
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <queue.h>
/* for test profile self definitions */
#include "dataTrans_uart.h"
#include "dataTrans_application.h"
#include "app_queue.h"
#include <string.h>
#include "dataTrans_profile.h"
#include "module_param_config.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"
#include "rtl876x_pinmux.h"

#include "dlps_platform.h"
#include "board.h"

/****************************************************************************/
/* retarget printf                                                          */
/****************************************************************************/
extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;
uint8_t UartRxTriggerLevel = 14;


static void UartSendMsgFromISR(BEE_IO_MSG *pBeeMsgBlk)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;
    portBASE_TYPE TaskWoken = pdFALSE;

    SendQueueResult = xQueueSendFromISR(hIoQueueHandle, pBeeMsgBlk, &TaskWoken);
    if (SendQueueResult != pdPASS)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "UartSendMsgFromISR fail2", 1, SendQueueResult);
#endif
    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSendFromISR(hEventQueueHandle, &Event, &TaskWoken);
    if (SendQueueResult != pdPASS)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "UartSendMsgFromISR fail", 1, SendQueueResult);
#endif
    }
}



void RxDataStatusUpdate(void)
{
    uint16_t Length;

    /* will not occur in our uart framework!!! */
    if (g_AppCB->rxBufferWriteOffsetOld == g_AppCB->rxBufferWriteOffset)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "RxDataStatusUpdate: no data", 0);
#endif
        return;       /* no data */

    }
    else
    {
        if (g_AppCB->rxBufferWriteOffsetOld > g_AppCB->rxBufferWriteOffset)
        {
            Length = UART_RX_BUFFER_LENGTH - g_AppCB->rxBufferWriteOffsetOld + g_AppCB->rxBufferWriteOffset;
        }
        else
        {
            Length = g_AppCB->rxBufferWriteOffset - g_AppCB->rxBufferWriteOffsetOld;
        }
        /*  check at cmd    */
        if ( ( (g_AppCB->rxBuffer[g_AppCB->rxBufferWriteOffsetOld] == 'A')  || ( g_AppCB->rxBuffer[g_AppCB->rxBufferWriteOffsetOld] == 'a' ) ) && (Length >= 4))
        {
            uint16_t index = g_AppCB->rxBufferWriteOffsetOld;
            index++;
            if (index == UART_RX_BUFFER_LENGTH)
            {
                index = 0;
            }
            if ((g_AppCB->rxBuffer[index] == 'T') || (g_AppCB->rxBuffer[index] == 't'))
            {
                /*  this is a AT cmd    */
                if (g_AppCB->isReceivingAtCmd)
                {
#ifndef CODE_SIZE_32K_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "RxDataStatusUpdate: previous AT cmd not handled", 0);
#endif
                    g_AppCB->isReceivingAtCmd = true;
                    g_AppCB->rxBufferCmdOffset = g_AppCB->rxBufferWriteOffsetOld;
                }
                else
                {
                    g_AppCB->isReceivingAtCmd = true;
                    g_AppCB->rxBufferCmdOffset = g_AppCB->rxBufferWriteOffsetOld;
                }
            }
        }

        g_AppCB->rxBufferWriteOffsetOld = g_AppCB->rxBufferWriteOffset;
    }

    if ((Length + g_AppCB->rxBufferDataLength) > UART_RX_BUFFER_LENGTH)   /* Rx overrun */
    {
//#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "RxDataStatusUpdate: Rx overrun (%d)", 1, \
                   Length + g_AppCB->rxBufferDataLength);
//#endif
        g_AppCB->rxBufferDataLength = UART_RX_BUFFER_LENGTH;
        g_AppCB->rxBufferReadOffset = g_AppCB->rxBufferWriteOffsetOld;
    }
    else
    {
        g_AppCB->rxBufferDataLength += Length;         /* update length */
    }
}




void CopyFromFIFO(void)
{
    uint16_t len = 0;

    if ((g_AppCB->rxBufferWriteOffset + UartRxTriggerLevel)\
            <= UART_RX_BUFFER_LENGTH)
    {
        UART_ReceiveData(UART, (g_AppCB->rxBuffer + g_AppCB->rxBufferWriteOffset), UartRxTriggerLevel);

        g_AppCB->rxBufferWriteOffset += UartRxTriggerLevel;

    }
    else
    {
        len = UART_RX_BUFFER_LENGTH - g_AppCB->rxBufferWriteOffset;
        UART_ReceiveData(UART, (g_AppCB->rxBuffer + g_AppCB->rxBufferWriteOffset), len);

        g_AppCB->rxBufferWriteOffset = 0;
        UART_ReceiveData(UART, g_AppCB->rxBuffer, UartRxTriggerLevel - len);

        g_AppCB->rxBufferWriteOffset += (UartRxTriggerLevel - len);
    }
}



/****************************************************************************/
/* TASK                                                                     */
/****************************************************************************/
uint8_t TxCount = 0;
uint8_t TxTotalLength = 0;
uint8_t *pBuffer = NULL;
extern SemaphoreHandle_t xSemaphore;
void TxHandleTask(void *pParameters)
{
    uint32_t count = 0;
    uint32_t remainder = 0;
    PTxData pData = NULL;

    while (true)
    {
        if (g_AppCB->gapProfileState == GAPSTATE_CONNECTED)
        {
            if ((g_AppCB->rxBufferDataLength == 0) && (g_AppCB->txUartDataQueueFree.ElementCount == TX_PACKET_COUNT) && (gConfigParam->isTimerDLPSActive == FALSE) && (gConfigParam-> isDataAvailable == TRUE))
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "All Data have finished WRITING, Reset the TimersEnterDLPSAfterConnected again !!", 0);
#endif
                xTimerReset(TimersEnterDLPSAfterConnected, 0);
                gConfigParam->isTimerDLPSActive = TRUE;
                gConfigParam-> isDataAvailable = FALSE;
            }
        }

        if (xQueueReceive(g_AppCB->QueueHandleTxData, &pData, portMAX_DELAY) == pdPASS)
        {
            pBuffer = pData->tx_buffer;
            TxTotalLength = pData->length;
            count = pData->length / UART_TX_FIFO_SIZE;
            remainder = pData->length % UART_TX_FIFO_SIZE;

            //DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "total= %d, count = %d, remainder = %d", 3, TxTotalLength, count, remainder);

            if (count == 0)
            {
                UART_SendData(UART, &pBuffer[0], remainder);
                TxCount += remainder;
            }
            else
            {
                UART_SendData(UART, &pBuffer[0], UART_TX_FIFO_SIZE);
                TxCount += UART_TX_FIFO_SIZE;
            }
            UART_INTConfig(UART, UART_INT_FIFO_EMPTY, ENABLE);
            xSemaphoreTake(xSemaphore, portMAX_DELAY);
            UART_INTConfig(UART, UART_INT_FIFO_EMPTY, DISABLE);
            TxCount = 0;
            TxTotalLength = 0;

            AppQueueIn(&g_AppCB->txUartDataQueueFree, pData);
        }
        else
        {
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "TxHandleTask: xQueueReceive fail", 0);
#endif
        }
    }
}





/****************************************************************************/
/* UART interrupt                                                           */
/****************************************************************************/
uint8_t int_en;
void DataUartIrqHandle(void)
{
    uint32_t int_status;
    int_en = 1;

    BEE_IO_MSG uart_msg;
    portBASE_TYPE TaskWoken = pdFALSE;

    /* read interrupt id */
    int_status = UART_GetIID(UART);
    /* disable interrupt */
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS | UART_INT_FIFO_EMPTY, DISABLE);

    //DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "ENTER ITR %d", 1, int_status);

    switch (int_status)
    {

        /* rx data valiable */
        case UART_INT_ID_RX_LEVEL_REACH:

            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "ENTER RX ISR", 0);
            if ((gConfigParam->fc_enable.fc_enable) && ((UartRxTriggerLevel + g_AppCB->rxBufferDataLength) > UART_RX_BUFFER_OVERFLOW_LENGTH))   /* if flow control is enabled */
            {
                uart_msg.IoType = IO_UART_MSG_TYPE;
                uart_msg.subType = MSG_UART_RX_OVERFLOW;
                UartSendMsgFromISR(&uart_msg);
                int_en = 0;     /* prevent interrupt is triggered continuously by the data remain in FIFO   */
            }
            else
            {

                CopyFromFIFO();

                /* update rx data length */
                RxDataStatusUpdate();

                /* notify ltp task */
                uart_msg.IoType = IO_UART_MSG_TYPE;
                uart_msg.subType = MSG_UART_RX;
                UartSendMsgFromISR(&uart_msg);

            }


            break;

        /* rx time out */
        case UART_INT_ID_RX_TMEOUT:

            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "ENTER RX TOUT ISR", 0);
            while (UART_GetFlagState(UART, UART_FLAG_RX_DATA_RDY) == SET)
            {
                if (g_AppCB->rxBufferWriteOffset == UART_RX_BUFFER_LENGTH)
                {
                    g_AppCB->rxBufferWriteOffset = 0;
                }

                UART_ReceiveData(UART, (g_AppCB->rxBuffer + g_AppCB->rxBufferWriteOffset), 1);
                g_AppCB->rxBufferWriteOffset++;

            }

            /* update rx data length */
            RxDataStatusUpdate();
            /* notify ltp task */
            uart_msg.IoType = IO_UART_MSG_TYPE;
            uart_msg.subType = MSG_UART_RX_TIMEOUT;
            UartSendMsgFromISR(&uart_msg);


            break;
        /* receive line status interrupt */
        case UART_INT_ID_LINE_STATUS:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Line status error:0x%x\n", 1, UART->LSR);
            break;

        case UART_INT_ID_TX_EMPTY:
            //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TX IntrHandler, total: %d,  TxCount:%d,  (TxTotalLength - TxCount): %d", 3, TxTotalLength, TxCount, (TxTotalLength - TxCount));

            if (UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) == SET)
            {
                if ((TxTotalLength - TxCount) >= UART_TX_FIFO_SIZE )
                {
                    UART_SendData(UART, pBuffer + TxCount, UART_TX_FIFO_SIZE);
                    TxCount += UART_TX_FIFO_SIZE;
                    UART_INTConfig(UART, UART_INT_FIFO_EMPTY, ENABLE);
                    //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TxCount 1 = %d", 1, TxCount);
                }
                else if ((TxTotalLength - TxCount) != 0)
                {
                    UART_SendData(UART, pBuffer + TxCount, TxTotalLength - TxCount);
                    TxCount += (TxTotalLength - TxCount);
                    UART_INTConfig(UART, UART_INT_FIFO_EMPTY, ENABLE);
                    //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TxCount 2 = %d", 1, TxCount);
                }
                else
                {
                    xSemaphoreGiveFromISR(xSemaphore, &TaskWoken);
                    //portEND_SWITCHING_ISR(TaskWoken);
                    //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TxCount 3 =%d", 1, TxCount);
                }

            }
            break;

        default:
            break;
    }



    if (gConfigParam->fc_enable.fc_enable)
    {
        if (int_en)
        {
            UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
        }
        else
        {
            UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, DISABLE);
        }
    }
    else
    {
        UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
    }


    //DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "EXIT  ISR", 0);

}




void UARTConfig(void)
{
    UART_InitTypeDef uartInitStruct;
    UART_StructInit(&uartInitStruct);


    if (gConfigParam->fc_enable.fc_enable)
    {
        uartInitStruct.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_14BYTE;
        uartInitStruct.autoFlowCtrl = UART_AUTO_FLOW_CTRL_EN;
        UartRxTriggerLevel = 14;
    }
    else
    {
        uartInitStruct.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_8BYTE;
        uartInitStruct.autoFlowCtrl = UART_AUTO_FLOW_CTRL_DIS;
        UartRxTriggerLevel = 8;
    }


    switch (gConfigParam->baudrate.baudrate)
    {
        case 1200:
            uartInitStruct.div = 2589;
            uartInitStruct.ovsr = 7;
            uartInitStruct.ovsr_adj = 0x7F7;
            break;
        case 2400:
            uartInitStruct.div = 1295;
            uartInitStruct.ovsr = 7;
            uartInitStruct.ovsr_adj = 0x7F7;
            break;
        case 4800:
            uartInitStruct.div = 542;
            uartInitStruct.ovsr = 10;
            uartInitStruct.ovsr_adj = 0x24A;
            break;
        case 9600:
            uartInitStruct.div = 271;
            uartInitStruct.ovsr = 10;
            uartInitStruct.ovsr_adj = 0x24A;
            break;
        case 19200:
            uartInitStruct.div = 165;
            uartInitStruct.ovsr = 7;
            uartInitStruct.ovsr_adj = 0x5AD;
            break;
        case 38400:
            uartInitStruct.div = 85;
            uartInitStruct.ovsr = 7;
            uartInitStruct.ovsr_adj = 0x222;
            break;
        case 57600:
            uartInitStruct.div = 55;
            uartInitStruct.ovsr = 7;
            uartInitStruct.ovsr_adj = 0x5AD;
            break;
        case 115200:
            uartInitStruct.div = 20;
            uartInitStruct.ovsr = 12;
            uartInitStruct.ovsr_adj = 0x252;
            break;
        case 230400:
            uartInitStruct.div = 10;
            uartInitStruct.ovsr = 12;
            uartInitStruct.ovsr_adj = 0x252;
            break;
        default:
            uartInitStruct.div = 271;
            uartInitStruct.ovsr = 10;
            uartInitStruct.ovsr_adj = 0x24A;
            break;
    }

    switch (gConfigParam->stopbit.stopbit)
    {
        case 1:
            uartInitStruct.stopBits = UART_STOP_BITS_1;
            break;
        case 2:
            uartInitStruct.stopBits = UART_STOP_BITS_2;
            break;
        default:
            uartInitStruct.stopBits = UART_STOP_BITS_1;
            break;
    }

    switch (gConfigParam->paribit.paribit)
    {

        case 0:
            uartInitStruct.parity = UART_PARITY_NO_PARTY;
            break;
        case 1:
            uartInitStruct.parity = UART_PARITY_ODD;
            break;
        case 2:
            uartInitStruct.parity = UART_PARITY_EVEN;
            break;
        default:
            uartInitStruct.parity = UART_PARITY_NO_PARTY;
            break;
    }


    UART_Init(UART, &uartInitStruct);
    /*  enable line status interrupt and rx data avaliable interrupt    */
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS,  ENABLE);
}






void UARTChange(void)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "UARTChange ", 0);
#endif
    UARTConfig();

    return;
}



/****************************************************************************/
/* UART init                                                                */
/****************************************************************************/
extern IRQ_FUN UserIrqFunTable[32 + 17];
void UARTInit(void)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "UARTInit: baudrate %d", 1, gConfigParam->baudrate.baudrate);
#endif
    /*  pinmux config   */
    Pinmux_Config(P3_0, DATA_UART_TX);
    Pinmux_Config(P3_1, DATA_UART_RX);
    Pinmux_Config(P3_2, DATA_UART_CTS);
    Pinmux_Config(P2_1, DATA_UART_RTS);

    /*  pad config  */
    Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P2_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);


    System_WakeUp_Pin_Enable(DATA_RX_IN, 0);

    /*  uart interrupt config   */
    UserIrqFunTable[UART_IRQ]               = (IRQ_FUN)DataUartIrqHandle;
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    /*  uart init   */
    UARTConfig();

    /*  Enable UART IRQ  */
    NVIC_ClearPendingIRQ(UART_IRQ);
    NVIC_SetPriority(UART_IRQ, 0);
    NVIC_EnableIRQ(UART_IRQ);

    return;
}
