

#include "mxd_app_cmd.h"
#include "mem.h"
#include "mxd2660_uart.h"
#include "mxd2660_dma.h"
#include "mxd_trx_service.h"
#ifdef ACI_CMD_SUPPORT

uint16 uart0_send(uint8 *data, uint16 data_length)
{
    uint16 scan = 0;
    uint16 timeout = 0;
#if 0 // use dma
    if (g_dma_uart_tx_lock == 0)
    {
        uint32 dma_config = 0;
        g_dma_uart_tx_lock = 1;
        dma_config = ( (data_length) | (CIRC_NOT << 16) | (PRI_LOW << 17) | (UART0_TX << 18) | (MEM_TO_PERI << 22)
                       | (DST_SIZE_BYTE << 24) | (DST_NOT_INC << 26) | (SRC_SIZE_BYTE << 27) | (SRC_INC << 29) );
        dma_config_init(g_dma_uart_tx_ch, dma_config, (uint32)data, (uint32)UART0_TX_ADDR);
        dma_enable(g_dma_uart_tx_ch);
    }
#else
    while (scan < data_length)
    {
        if (true == uart_put(UART0, data[scan]))
        {
            scan++;
            timeout = 0;
        }
        else if (timeout < 100)
        {
            timeout++;
            delay_us(10);
        }
        else
        {
            break;
        }
    }
#endif
    return scan; //sucess_length;
}


#if 0
uint16 data_send(uint8 *data, uint16 data_length)
{
    uart0_send(data, data_length);
}
#endif

uint16 app_cmd_plus(uint8 * data, uint16 data_length, uint16 tail_length)
{
    uint16 processed_length = 0;
    uint8  packet_type = 0;
    uint8  annly_size = 0;
    uint8 *para_data = 0;
    uint16 para_length = 0;

    if (data_length <= 2)
        return processed_length;

    packet_type = data[0];
    para_length = data[1];
    para_data   = &data[2];

#if 1
    if (packet_type == _API_DATA_PACKET_)
    {
        annly_size  = 2 + para_length;

        if (data_length >= annly_size)
        {
            txrx_server_transfer(para_data, para_length);
        }

        processed_length = annly_size;
    }
    else
    {
        // hci
        processed_length = peri_hci_process(data, data_length, tail_length);
    }

#else
    switch (packet_type)
    {
        case _API_RD_CC_PACKET_ : // api read cc code
        {

        }
        break;

        case _API_DATA_PACKET_ : // api data packet
        {
            annly_size  = 2 + para_length;

            if (data_length >= annly_size)
            {
                txrx_server_transfer(para_data, para_length);
            }

            processed_length = annly_size;
        }
        break;

        case _API_CTRL_PACKET_ : // api control packet
        {

        }
        break;

        case _PHY_WRITE_PACKET_ : // api write phy
        {

        }
        break;

        case _PHY_READ_CMD_PACKET_ : // api read phy
        {

        }
        break;

        default :
        {
            // hci
            processed_length = peri_hci_process(data, data_length, tail_length);
        }
        break;
    }
#endif
    return processed_length;
}


uint8 send_event_to_host(uint8 ctrl_code, uint8 value)
{
    uint8 event_buffer[3] = { 0x26, 0x00, 0x00 };
    uint8 event_length = 3;

    event_buffer[1] = ctrl_code;
    event_buffer[2] = value;

    data_send(event_buffer, event_length);
    return 0;
}

uint8 send_datapacket_to_host(uint8 *data, uint16 data_length)
{
    uint8 datapacket_head_buffer[3] = { 0x22, 0x00 };
    uint8 datapacket_head_length = 2;

    datapacket_head_buffer[1] = data_length;
    data_send(datapacket_head_buffer, datapacket_head_length);
    data_send(data, data_length);
    return 0;
}
#endif
