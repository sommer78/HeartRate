/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    Uart demo(dlps on)
* @details
* @author   tifnan
* @date     2015-06-03
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "rtl876x_io_dlps.h"
#include "dlps_platform.h"
#include "string.h"
#include "trace.h"
#include "board.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

/* queue size & stak size & task priority */
#define IO_DEMO_EVENT_QUEUE_SIZE        0x10
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

#define ENTER_DLPS_CMD                  0x01

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoIOEventQueue;

extern IRQ_FUN UserIrqFunTable[32+17];
void DataUartIntrHandler(void);
void IO_DemoTask(void* param);

/* globals */
uint8_t DemoStrLen = 0;
uint32_t DlpsCount = 0;
uint8_t CanEnterDlps = 0;
uint8_t TxBuf[100];

/* hex to Ascii*/
uint8_t HexToAscii(uint8_t hex)
{
    if(hex <= 9)
    {
        return (hex - 0x00 + 0x30);
    }
    else if(hex >= 0x0A && hex <= 0x0F)
    {
        return (hex - 0x0A + 0x41);
    }
    else
    {
        /* wrong */
        return 0xFF;
    }
}

void DlpsExitCallback(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DlpsExitCallback", 0);
    
    CanEnterDlps  = 0;
    
    PAD_Configuration();
    
    uint8_t event = IO_DEMO_EVENT_SYSTEM_WAKE;
    
    xQueueSend(DemoIOEventQueue, &event, 0);
    
    return;
}

void DlpsEnterCallback(void)
{
    Pad_Config(P4_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(P4_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

BOOL DlpsEnterCheck(void)
{
    if(CanEnterDlps)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int main(void) 
{
    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(uint8_t));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(uint8_t));
    
    /* enable dlps and register dlps callback function */
    DLPS_IO_Register();
    //  LPS_MODE_Pause();
    DLPS_IO_RegUserDlpsExitCb(DlpsExitCallback);
    DLPS_IO_RegUserDlpsEnterCb(DlpsEnterCallback);
    /* register enter dlps check function */
    DLPS_ENTER_CHECK_CB_REG(DlpsEnterCheck);
    /* enable dlps*/
    LPS_MODE_Set(LPM_DLPS_MODE);
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux */
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    
    return;
}

void PAD_Configuration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    
    return;
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* UART IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);
    
    return;
}

void DataUartIntrHandler(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DataUartIntrHandler", 0);
    
    uint32_t int_status = 0;
    uint8_t event  = IO_DEMO_EVENT_UART_RX;
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
            break;
        
        case UART_INT_ID_RX_TMEOUT:
            while(UART_GetFlagState(UART, UART_FLAG_RX_DATA_RDY) == SET)
            {
                rxByte = UART_ReceiveByte(UART);
            }
            
            /* send convert result to task*/
            xQueueSendFromISR(DemoIOMessageQueue, &rxByte, &TaskWoken);
            /* send event to DemoIO task */
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

void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t i = 0;
    uint8_t event = 0;
    uint8_t rxByte = 0;

    char* demoStr = "### Uart Demo (DLPS ON) ###\r\n";
    char* demoStr1 = "## Exit Dlps: 0000st ##\r\n";
    
    /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /******************* uart init *****************************/
    UserIrqFunTable[UART_IRQ]               = (IRQ_FUN)DataUartIntrHandler;
    
    UART_InitTypeDef uartInitStruct;
    
    /* default 115200 baudrate */
    UART_StructInit(&uartInitStruct);
    
    UART_Init(UART, &uartInitStruct);
    
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
    
    System_WakeUp_Pin_Enable(P4_2, 0);

    /* send tips through uart */
    DemoStrLen = strlen(demoStr);       
    memcpy(TxBuf, demoStr, DemoStrLen);
    blkCount = DemoStrLen / UART_TX_FIFO_SIZE;
    remainder = DemoStrLen % UART_TX_FIFO_SIZE;
    
    for(i = 0; i < blkCount; i++)
    {
        /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
        UART_SendData(UART, &TxBuf[16 * i], 16);
        /* wait tx fifo empty */
        while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    }
    
    /* send left bytes */
    UART_SendData(UART, &TxBuf[16 * i], remainder);
    /* wait tx fifo empty */
    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    
    DemoStrLen = strlen(demoStr1);    
    memcpy(TxBuf, demoStr1, DemoStrLen);
    
    blkCount = DemoStrLen / UART_TX_FIFO_SIZE;
    remainder = DemoStrLen % UART_TX_FIFO_SIZE;
    
    while(1)
    {
        if (xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            /* system wakeup event */
            if (event == IO_DEMO_EVENT_SYSTEM_WAKE)
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "system wakeup event received", 0);
                
                DlpsCount++;
                
                TxBuf[14] = HexToAscii(DlpsCount / 1000);
                TxBuf[15] = HexToAscii((DlpsCount / 100) % 10);
                TxBuf[16] = HexToAscii((DlpsCount / 10) % 100 % 10);
                TxBuf[17] = HexToAscii(DlpsCount % 1000 % 100 % 10);
                
                for(i = 0; i < blkCount; i++)
                {
                    /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
                    UART_SendData(UART, &TxBuf[16 * i], 16);
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                }
    
                /* send left bytes */
                UART_SendData(UART, &TxBuf[16 * i], remainder);
                /* wait tx fifo empty */
                while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
            }
            
            if(event == IO_DEMO_EVENT_UART_RX)
            {
                if (xQueueReceive(DemoIOMessageQueue, &rxByte, portMAX_DELAY) == pdPASS)
                {
                    if(rxByte == ENTER_DLPS_CMD)
                    {
                        CanEnterDlps = 1;
                        /* @note: this log may lost!!!!!!!!!! */
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ENTER_DLPS_CMD received", 0);
                        DBG_DIRECT("ENTER_DLPS_CMD received");
                        DBG_DIRECT("ENTER_DLPS_CMD received");
                    }
                }
            }
        }
    }
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}

