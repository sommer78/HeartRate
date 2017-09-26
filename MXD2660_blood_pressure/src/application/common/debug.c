/**
 *****************************************************************************
 * @file     debug.c
 * @author   Richard
 * @version  V1.0.0
 * @date     03-Aug-2016
 * @brief    use uart help to debug
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

#include "system_config.h"
#include "mxd2660_uart.h"
#include "device_cfg.h"

#ifdef _TRUNK_DEBUG__
#define LOG_BUFFER_SIZE 256
uint32  log_buffer[LOG_BUFFER_SIZE];
t_log_ctrl  g_log_ctrl;
uint8  g_log_pattern;


void sys_debug_init(void)
{  
    g_log_ctrl.wr = 0;
    g_log_ctrl.rd = 0;
}


void uart1_Log_output(uint16 address, uint16 data)
{
    if(g_log_pattern == 1)
    {
        uint32 outdata = 0;
        outdata += address;
        outdata &= 0xFFFF;
        outdata += (data << 16);
        log_buffer[g_log_ctrl.wr++] = outdata;
        g_log_ctrl.wr &= (LOG_BUFFER_SIZE - 1);
    }

}


void uart1_Log_print(void)
{
    while(g_log_ctrl.rd != g_log_ctrl.wr)
    {
        if((UART1->UART_FIFO_CNT & 0xF) <= 4)
        {
            uint32 log32bit = log_buffer[g_log_ctrl.rd];
            UART1->UART_FIFO_WR = (log32bit >> 8) & 0xFF;
            UART1->UART_FIFO_WR = (log32bit & 0xFF);
            UART1->UART_FIFO_WR = (log32bit >> 24) & 0xFF;
            UART1->UART_FIFO_WR = ((log32bit >> 16) & 0xFF);
            g_log_ctrl.rd++;
            g_log_ctrl.rd &= (LOG_BUFFER_SIZE - 1);

        }
    }
}

bool is_log_canbe_sleep(void)
{
    if( g_log_ctrl.rd != g_log_ctrl.wr)
    {
        return FALSE;
    }
    return TRUE;
}



void sys_debug_en(uint8 debug_en)
{
    g_log_pattern = debug_en;
}

#endif

#ifdef LOG_FUNC_EN

int fputc(int c, FILE *f)
{
    if(c == '\n')
    {
        c = 0x0D;
        uart1_send((uint8*)&c, 1);                          //output CR
        c = 0x0A;
        uart1_send((uint8*)&c, 1);  
    }
    else
    {
        uart1_send((uint8*)&c, 1);
    }

	return c;
}

// get a char from UART.
// Return: Return input data
int fgetc(FILE *f)
{
	int c;

	return c;
}

#endif





