/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    IR demo--interrupt tx
* @details
* @author   tifnan
* @date     2015-05-25
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_ir.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "rtl876x_io_dlps.h"
#include "dlps_platform.h"
#include "ir_protocol.h"
#include <string.h>
#include "string.h"
#include "board.h"
#include "trace.h"

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

uint8_t DemoStrBuffer[100];

/* global */
uint8_t it_tx_count = 0;
IRDA_BUF IrBuf;
uint8_t can_dlps = 0;

static void uart_send_str(char* str, uint16_t str_len)
{
    uint8_t blk, remain, i;
    blk = str_len / UART_TX_FIFO_SIZE;
    remain = str_len % UART_TX_FIFO_SIZE;
    
    //send through uart
    for(i = 0; i < blk; i++)
    {
        UART_SendData(UART, (uint8_t*)&str[16 * i], 16);
        while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    }
    
    UART_SendData(UART, (uint8_t*)&str[16 * i], remain);
    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
}

BOOL dlps_enter_check_cb(void)
{
    if(can_dlps) 
        return TRUE;
    
    return FALSE;
}

void dlps_exit_cb(void)
{
    Pad_Config(P4_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    can_dlps = 0;
}

void dlps_enter_cb(void)
{
    Pad_Config(P4_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
}


int main(void)
{
    All_Pad_Config_Default();
    
    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(uint8_t));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(uint16_t));
    
    /* enable dlps and register dlps callback function */
    DLPS_IO_Register();
    //  LPS_MODE_Pause();
    DLPS_IO_RegUserDlpsExitCb(dlps_exit_cb);
    DLPS_IO_RegUserDlpsEnterCb(dlps_enter_cb);
    /* register enter dlps check function */
    DLPS_ENTER_CHECK_CB_REG(dlps_enter_check_cb);
    /* enable dlps*/
    LPS_MODE_Set(LPM_DLPS_MODE);
    /* enable uart rx pin wakeup */
    System_WakeUp_Pin_Enable(P4_2, 0);
    /* start task schedule */
    vTaskStartScheduler();
}

void IO_DemoTask(void* param)
{
    uint8_t event = 0;
    uint16_t rx_data;
    
    /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /******************* uart init *****************************/
    UART_InitTypeDef uartInitStruct;
    
    /* default 115200 baudrate */
    UART_StructInit(&uartInitStruct);
    
    UART_Init(UART, &uartInitStruct);
    
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
    
    /************************ ir init ****************************/
    IR_InitTypeDef irInitStruct;
    
    IR_StructInit(&irInitStruct);
    
    IR_Init(IR, &irInitStruct);

    /* set tx threshold */
    IR_SetTxThreshold(IR, 31);
    
    IR_Cmd(IR, IR_MODE_TX, ENABLE);
    
    while(1)
    {
        if(xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            if(event == IO_DEMO_EVENT_UART_RX)
            {
                if (xQueueReceive(DemoIOMessageQueue, &rx_data, portMAX_DELAY) == pdPASS)
                {
                    //char* str = "start IR send\r\n";
                    //uart_send_str(str, strlen(str));
                    /* encode NEC */
                    irda_nec_encode((uint8_t)(rx_data >> 8), (uint8_t)rx_data, &IrBuf);
                    /* start to send a NEC IR pkt */
                    IR_SendData(IR, IrBuf.pBuf, 31);
                    it_tx_count+= 31;
                    /* enable IR tx interrupt */
                    IR_INTConfig(IR, IR_INT_TF_EMPTY, ENABLE);
                }
            }
            
            if(event == IO_DEMO_EVENT_IR_TX_DONE)
            {
                IR_StopTx(IR);
                //char* str = "send IR done\r\n";
                //uart_send_str(str, strlen(str));
                UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
                can_dlps = 1;
            }
        }
    }
}

void PINMUX_Configuration(void)
{
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    Pinmux_Config(P4_1, IRDA_TX);
    
    return;
}

void PAD_Configuration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART and IR clock */
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    return;
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* IR IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = IR_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    /* UART IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    
    NVIC_Init(&NVIC_InitStruct);
    
    return;
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
