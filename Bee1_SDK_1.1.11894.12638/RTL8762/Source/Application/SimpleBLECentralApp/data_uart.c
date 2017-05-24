enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     data_uart.c
* @brief    Data uart operations for testing profiles.
* @details  Data uart init and print data through data uart.
* @author
* @date     2015-03-19
* @version  v0.2
*********************************************************************************************************
*/
#include "bee_message.h"
#include "data_uart.h"
#include "os.h"
#include "rtl_types.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_io_dlps.h"
#include "rtl876x_nvic.h"
#include "board.h"

/** @brief  this external queue handle for UART to send data to Application. */
extern xQueueHandle hIoQueueHandle;
extern xQueueHandle hEventQueueHandle;

/**
 * @brief  retarge print.
 *
 * @param ch        char data to sent.
 * @return char data to sent.
*/
static int32_t dataUART_Sendchar(int32_t ch)
{
    while (UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    UART_SendData(UART, (uint8_t*)&ch, 1);
    return (ch);
}

/**
 * @brief  retarge print.
 *
 * @param buf
 * @param fmt
 * @param dp
 * @return.
*/
static int32_t dataUART_VSprintf(char *buf, const char *fmt, const int32_t *dp)
{
    char *p, *s;

    s = buf;
    for ( ; *fmt != '\0'; ++fmt)
    {
        if (*fmt != '%')
        {
            buf ? *s++ = *fmt : dataUART_Sendchar(*fmt);
            continue;
        }
        if (*++fmt == 's')
        {
            for (p = (char *)*dp++; *p != '\0'; p++)
                buf ? *s++ = *p : dataUART_Sendchar(*p);
        }
        else    /* Length of item is bounded */
        {
            char tmp[20], *q = tmp;
            int32_t alt = 0;
            int32_t shift = 28;

#if 1   //wei patch for %02x
            if ((*fmt  >= '0') && (*fmt  <= '9'))
            {
                int32_t width;
                unsigned char fch = *fmt;
                for (width = 0; (fch >= '0') && (fch <= '9'); fch = *++fmt)
                {
                    width = width * 10 + fch - '0';
                }
                shift = (width - 1) * 4;
            }
#endif

            /*
             * Before each format q points to tmp buffer
             * After each format q points past end of item
             */

            if ((*fmt == 'x') || (*fmt == 'X') || (*fmt == 'p') || (*fmt == 'P'))
            {
                /* With x86 gcc, sizeof(long) == sizeof(int32_t) */
                const long *lp = (const long *)dp;
                long h = *lp++;
                int32_t ncase = (*fmt & 0x20);
                dp = (const int32_t *)lp;
                if ((*fmt == 'p') || (*fmt == 'P'))
                    alt = 1;
                if (alt)
                {
                    *q++ = '0';
                    *q++ = 'X' | ncase;
                }
                for ( ; shift >= 0; shift -= 4)
                    * q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
            }
            else if (*fmt == 'd')
            {
                int32_t i = *dp++;
                char *r;
                if (i < 0)
                {
                    *q++ = '-';
                    i = -i;
                }
                p = q;      /* save beginning of digits */
                do
                {
                    *q++ = '0' + (i % 10);
                    i /= 10;
                }
                while (i);
                /* reverse digits, stop in middle */
                r = q;      /* don't alter q */
                while (--r > p)
                {
                    i = *r;
                    *r = *p;
                    *p++ = i;
                }
            }
            else if (*fmt == 'c')
                *q++ = *dp++;
            else
                *q++ = *fmt;
            /* now output the saved string */
            for (p = tmp; p < q; ++p)
                buf ? *s++ = *p : dataUART_Sendchar(*p);
        }
    }
    if (buf)
        *s = '\0';
    return (s - buf);
}

/**
 * @brief  retarge print.
 *
 * @param
 * @return
*/
int32_t dataUART_Print(IN  char *fmt, ...)
{
    (void)dataUART_VSprintf(0, fmt, ((const int32_t *)&fmt) + 1);
    return 0;
}

void UserCmdUARTIntrHandler(void)
{
    uint8_t rx_data = 0;
    portBASE_TYPE TaskWoken = pdFALSE;
    uint8_t event  = EVENT_NEWIODRIVER_TO_APP;
    uint32_t int_status = 0;
    BEE_IO_MSG io_driver_msg_send;
    io_driver_msg_send.IoType = IO_UART_MSG_TYPE;

    /* read interrupt id */
    int_status = UART_GetIID(UART);
    /* disable interrupt */
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, DISABLE);

    switch (int_status)
    {
    /* tx fifo empty */
    case UART_INT_ID_TX_EMPTY:
        /* do nothing */
        break;

    /* rx data valiable */
    case UART_INT_ID_RX_LEVEL_REACH:
    case UART_INT_ID_RX_TMEOUT:
        while (UART_GetFlagState(UART, UART_FLAG_RX_DATA_RDY) == SET)
        {
            UART_ReceiveData(UART, &rx_data, 1);
        }
        io_driver_msg_send.subType = (uint16_t)rx_data;

        xQueueSendFromISR(hIoQueueHandle, &io_driver_msg_send, &TaskWoken);
        xQueueSendFromISR(hEventQueueHandle, &event, &TaskWoken);
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

/**
 * @brief  initiate Data UART.
 *
 * @param none
 * @return none
*/
void dataUART_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    UART_InitTypeDef uartInitStruct;
    
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    
    DLPS_IO_Register();
    
    /* pinmux config */
    Pinmux_Config(DATA_UART_TX_PIN, DATA_UART_TX);
    Pinmux_Config(DATA_UART_RX_PIN, DATA_UART_RX);

    /* pad config */
    Pad_Config(DATA_UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(DATA_UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* NVIC config */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* uart init */
    UART_StructInit(&uartInitStruct);
    /* change default rx trigger level */
    uartInitStruct.rxTriggerLevel = UART_RX_FIFO_TRIGGER_LEVEL_1BYTE;
    UART_Init(UART, &uartInitStruct);
    /* enable rx interrupt and line status interrupt */
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
}

