/**
****************************************************************************
* @file     hci_app.c
* @brief
* @author   xiaoyu
* @version  Initial Draft
* @date     2016/11/16
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#include "system_config.h"
#include "app_message.h"
#include "app_task.h"
#include "hci_api.h"
#include "mem.h"
#include "ll_ctrl.h"
#include "ll_phy.h"
#include "app_ble.h"
#include "ble_api.h"
#include "mxd2660_uart.h"
#include "mxd2660_gpio.h"
#include "mxd2660_dma.h"
#include "gatts_api.h"
#include "stack_task.h"
#include "ble_msg_api.h"
#include "mxd_trx_service.h"
#include "device_cfg.h"
#include "mem.h"
#include "hci_api.h"
#include "hci_event.h"

#ifdef HCI_FUNC_EN


uint8 hci_rx_dispatch_state;
hci_rx_header_t hci_rx_header;
uint8 g_cmplt_event_length;

cmd_cmplt_evt cmplt_event_info;

uint8 hci_reset_flag;
uint8 hci_reset_flag_dtm;

uint8 hci_scanData[31];
uint8 hci_advData[31];

//wyp   Dec 12, 2016  uart+dma for hci
uint8 g_dma_uart_rx_ch;
uint8 g_dma_uart_tx_ch;
uint16 g_dma_uart_tx_lock;

uint8 g_dma_lock;

comm_fifo_mgr Peri_Data;
comm_fifo_mgr Event_Data;
uint32 g_peri_ctrl = UART_USE_DMA;  //UART_USE_DMA
uint8 hci_data_pool[HCI_POOL_SIZE];


void hci_init(void)
{
    Peri_Data.wr = 0;
    if(g_peri_ctrl & UART_USE_DMA)
    {
        Peri_Data.rd = (uint16)(PERI_DATA_SIZE - 1);
    }
    else
    {
        Peri_Data.rd = 0;
    }
    Peri_Data.data_ptr = hci_data_pool;

    Event_Data.wr = 0;
    Event_Data.rd = 0;
    Event_Data.data_ptr = (uint8 *)(Peri_Data.data_ptr + PERI_DATA_SIZE);
    
    hci_rx_dispatch_state = HCI_STATE_RX_TYPE;
    hci_reset_flag = 0;
    hci_reset_flag_dtm = 0;
    memset(&hci_rx_header, 0, sizeof(hci_rx_header_t));
    
}


bool is_hci_sleep(void)
{
	uint16 rd = Peri_Data.rd;
    if(g_peri_ctrl & UART_USE_DMA)
    {
        rd = (rd + 1) & (PERI_DATA_SIZE - 1);
    }
    if( ( rd != Peri_Data.wr) || (Event_Data.rd != Event_Data.wr) )
    {
        return FALSE;
    }
    return TRUE;
}

uint8 hci_soft_reset(void)
{
    //reset
    if(1 == hci_reset_flag)
    {
        hci_init();
        soft_reset(1);
    }
    if(1 == hci_reset_flag_dtm)
    {
        cmplt_event_info.cmdOpcodelo8 = (HCI_RESET & 0xFF);
        cmplt_event_info.cmdOpcodehi8 = ((HCI_RESET >> 8) & 0xFF);
        cmplt_event_info.numHciCmdPck = 1;
        cmplt_event_info.return_parameters.common_cmplt.status = 0;

        soft_reset(2);

        //event rpt
        hci_event_generate(CMD_CMPLT_EVT_CODE, 4, (uint8 *)&cmplt_event_info.numHciCmdPck);
        hci_reset_flag_dtm = 0;
    }
    return NO_ERROR;
}



uint16 peri_hci_process(uint8 * data, uint16 data_length, uint16 tail_length)
{
    uint16 i = 0;
    int16  idx = 0;
    uint16 packet_length = 0;
    uint16 unused_data_length = 0;
    uint16 is_zero_length = 0;
    while(i < data_length)
    {
        idx = TO_FIFO_INDEX(i, tail_length);
        switch(hci_rx_dispatch_state)
        {
            case HCI_STATE_RX_TYPE:
                if(HCI_COMMAND == data[idx])
                {
                    hci_rx_dispatch_state = HCI_STATE_RX_COMMAND_OPCODE1;
                    hci_rx_header.type = HCI_COMMAND;
                }
                else if(HCI_ACLDATA == data[idx])
                {
                    hci_rx_dispatch_state = HCI_STATE_RX_ACL_HEAD1;
                    hci_rx_header.type = HCI_ACLDATA;
                }
                else
                {
                    //run to wrong branch
                    system_assert(_HCI_ + 0x0, 0, ( ((uint32)(&data[idx]) << 16) | data[idx]));
                }
                break;
            case HCI_STATE_RX_COMMAND_OPCODE1:
                hci_rx_header.header.hci_cmd_header.ogf6ocf10 = data[idx];
                hci_rx_dispatch_state = HCI_STATE_RX_COMMAND_OPCODE2;
                break;
            case HCI_STATE_RX_COMMAND_OPCODE2:
                hci_rx_header.header.hci_cmd_header.ogf6ocf10 |= (data[idx] << 8);
                hci_rx_dispatch_state = HCI_STATE_RX_COMMAND_LENGTH;
                break;
            case HCI_STATE_RX_COMMAND_LENGTH:
                hci_rx_header.header.hci_cmd_header.length = data[idx];
                hci_rx_dispatch_state = HCI_STATE_RX_DATA_START;
                if(0 == hci_rx_header.header.hci_cmd_header.length)
                {
                    is_zero_length = 1;
                }
                break;
            case HCI_STATE_RX_ACL_HEAD1:
                hci_rx_header.header.hci_acl_header.flag4handle12 = data[idx];
                hci_rx_dispatch_state = HCI_STATE_RX_ACL_HEAD2;
                break;
            case HCI_STATE_RX_ACL_HEAD2:
                hci_rx_header.header.hci_acl_header.flag4handle12 |= (data[idx] << 8);
                hci_rx_dispatch_state = HCI_STATE_RX_ACL_LENGTH1;
                break;
            case HCI_STATE_RX_ACL_LENGTH1:
                hci_rx_header.header.hci_acl_header.length = data[idx];
                hci_rx_dispatch_state = HCI_STATE_RX_ACL_LENGTH2;
                break;
            case HCI_STATE_RX_ACL_LENGTH2:
                hci_rx_header.header.hci_acl_header.length |= (data[idx] << 8);
                hci_rx_dispatch_state = HCI_STATE_RX_DATA_START;
                if(0 == hci_rx_header.header.hci_acl_header.length)
                {
                    is_zero_length = 1;
                }
                break;
            case HCI_STATE_RX_DATA_START:
            case HCI_STATE_RX_DATA_CONT:
                if(HCI_COMMAND == hci_rx_header.type)
                {
                    uint16 status;
                    uint16 cmd_tail_length;
                    packet_length = hci_rx_header.header.hci_cmd_header.length;
                    unused_data_length = data_length - i;
                    if(unused_data_length >= packet_length)
                    {
                        //for command, no special buffer for command, just in peri fifo, so when all the payload has already in peri fifo, the command dispatch can run
                        cmd_tail_length = ((i >= tail_length) ? unused_data_length : (tail_length - i));
                        status = hci_cmd_dispatch(&data[idx], hci_rx_header.header.hci_cmd_header.ogf6ocf10, hci_rx_header.header.hci_cmd_header.length, cmd_tail_length);
                        if(1 == status)
                        {
                            //command complete event
                            hci_event_generate(CMD_CMPLT_EVT_CODE, g_cmplt_event_length, (uint8 *)&cmplt_event_info.numHciCmdPck);

                        }
                        else if(0 == status)
                        {
                            hci_cmd_status_event();
                        }
                        hci_rx_dispatch_state = HCI_STATE_RX_TYPE;
                        i += (packet_length - 1);
                    }
                    else
                    {
                        hci_rx_dispatch_state = HCI_STATE_RX_DATA_CONT;
                        return i;
                    }

                }
                else if(HCI_ACLDATA == hci_rx_header.type)
                {
                    uint8 * data_out_ptr;
                    uint16 acl_tail_length;
                    data_ptr_list *allocated_list;
                    data_ptr_list **end_list;
                    uint8 j;
                    if(HCI_STATE_RX_DATA_START == hci_rx_dispatch_state)
                    {
                        packet_length = hci_rx_header.header.hci_acl_header.length;
                        //for acl data buffer, data pdu buffer can be used to store the data, so no need to wait the whole payload is received in peri fifo
                        data_out_ptr = get_avail_data_buffer(0);
                        if(NULL == data_out_ptr)
                        {
                            //no enough buffer
                            return 0xFFFF;
                        }
                        //fill hci acl packet header info
                        data_out_ptr[-5] =  HCI_ACLDATA;
                        data_out_ptr[-4] =  (hci_rx_header.header.hci_acl_header.flag4handle12 & 0xFF);
                        data_out_ptr[-3] =  ((hci_rx_header.header.hci_acl_header.flag4handle12 >> 8) & 0xFF);
                        data_out_ptr[-2] =  (hci_rx_header.header.hci_acl_header.length & 0xFF);
                        data_out_ptr[-1] =  ((hci_rx_header.header.hci_acl_header.length >> 8) & 0xFF);
                        LOG(0xd105, packet_length);
                    }
                    else
                    {
                        packet_length = g_common_cfg.hci_acl_rx_cont_rest_len;
                        //for acl data buffer, data pdu buffer can be used to store the data, so no need to wait the whole payload is received in peri fifo
                        data_out_ptr = (g_common_cfg.hci_acl_rx_cont_data_ptr + hci_rx_header.header.hci_acl_header.length - g_common_cfg.hci_acl_rx_cont_rest_len);

                    }
                    unused_data_length = data_length - i;
                    if(unused_data_length >= packet_length)
                    {
                        acl_tail_length = ((i >= tail_length) ? unused_data_length : (tail_length - i));
                        //no cross fifo end
                        if(acl_tail_length >= packet_length)
                        {
                            memcpy(data_out_ptr, &data[idx], packet_length);
                        }
                        //cross fifo end
                        else
                        {
                            //first copy the tail data
                            memcpy(data_out_ptr, &data[idx], acl_tail_length);

                            //second copy the rest data
                            memcpy(&data_out_ptr[acl_tail_length], &data[TO_FIFO_INDEX(tail_length, tail_length)], (packet_length - acl_tail_length));
                        }
                        i += (packet_length - 1);
                        allocated_list = get_free_data_ptr_list(0);
                        if(NULL == allocated_list)
                        {
                            system_assert(_HCI_ + 0x3, 0, hci_rx_dispatch_state);
                        }

                        allocated_list->next = NULL;
                        if(HCI_STATE_RX_DATA_START == hci_rx_dispatch_state)
                        {
                            allocated_list->data_ptr = data_out_ptr;
                        }
                        else
                        {
                            allocated_list->data_ptr = g_common_cfg.hci_acl_rx_cont_data_ptr;
                        }
                        j = ll_get_idx_from_conn_hdl(hci_rx_header.header.hci_acl_header.flag4handle12 & 0xFFF);
                        if (j == MAX_CONNECTIONS)
                        {
                            return 0xFFFF;
                        }
                        end_list = get_data_ptr_list_end(g_conn_cfg[j].tx_data_pdu.data_list);
                        if(end_list)
                        {
                            *end_list = allocated_list;
                        }
                        else
                        {
                            g_conn_cfg[j].tx_data_pdu.data_list = allocated_list;
                        }
                        hci_rx_dispatch_state = HCI_STATE_RX_TYPE;
                        g_common_cfg.hci_acl_rx_cont_exist = 0;
                        g_common_cfg.hci_acl_rx_cont_rest_len = 0;
                        g_common_cfg.hci_acl_rx_cont_data_ptr = NULL;
                    }
                    else
                    {
                        //no cross fifo end
                        if(i >= tail_length)
                        {
                            memcpy(data_out_ptr, &data[idx], unused_data_length);
                        }
                        //cross fifo end
                        else
                        {
                            acl_tail_length = (tail_length - i);
                            //first copy the tail data
                            memcpy(data_out_ptr, &data[idx], acl_tail_length);

                            //second copy the rest data
                            memcpy(&data_out_ptr[acl_tail_length], &data[TO_FIFO_INDEX(tail_length, tail_length)], (unused_data_length - acl_tail_length));
                        }
                        i += (unused_data_length - 1);
                        g_common_cfg.hci_acl_rx_cont_exist = 1;
                        g_common_cfg.hci_acl_rx_cont_rest_len = (packet_length - unused_data_length);
                        if(HCI_STATE_RX_DATA_START == hci_rx_dispatch_state)
                        {
                            g_common_cfg.hci_acl_rx_cont_data_ptr = data_out_ptr;
                        }
                        hci_rx_dispatch_state = HCI_STATE_RX_DATA_CONT;
                    }
                }
                break;
            default:
                break;
        }
        i++;
    }
    if(is_zero_length)
    {
        if(HCI_COMMAND == hci_rx_header.type)
        {
            uint16 status;
            status = hci_cmd_dispatch(&data[idx], hci_rx_header.header.hci_cmd_header.ogf6ocf10, hci_rx_header.header.hci_cmd_header.length, 0);
            if(1 == status)
            {
                //command complete event
                hci_event_generate(CMD_CMPLT_EVT_CODE, g_cmplt_event_length, (uint8 *)&cmplt_event_info.numHciCmdPck);

            }
            else if(0 == status)
            {
                hci_cmd_status_event();
            }
            hci_rx_dispatch_state = HCI_STATE_RX_TYPE;

        }
        else if(HCI_ACLDATA == hci_rx_header.type)
        {
            //acl data process for 0 length
        }
    }
    return data_length;
}




uint8 hci_event_generate(uint8 evtCode, uint8 length, uint8 * data)
{
    uint16 wr = Event_Data.wr;
    uint16 rd =Event_Data.rd ;
    uint16 idx = 0;
    uint16 tail_length = 0;
    uint16 cplen=0;

    //add event parameters
    if (((wr + 1) & (EVENT_DATA_SIZE - 1)) == rd)
    {
        //rd is too slow, wr overlap buffer
          LOG(0xdccd,0xf2);
        return OVERFLOW;
    }
    else
    {
        if (rd >((wr + 1) & (EVENT_DATA_SIZE - 1)))
        {
            cplen=rd - ((wr + 1) & (EVENT_DATA_SIZE - 1));
        }
        else
        {
            cplen=(rd + EVENT_DATA_SIZE - ((wr + 1) & (EVENT_DATA_SIZE - 1)));
        }

        //enough fifo space for event data
        //   if(length < (Event_Data.rd + EVENT_DATA_SIZE - ((wr + 1) & (EVENT_DATA_SIZE - 1))))  //writing-cross-border if rd>wr,
        if (length+3 < cplen)
        {
            idx=0;    //first add header info
            while (idx < 3)
            {
                wr = ((wr + 1) & (EVENT_DATA_SIZE - 1));
                Event_Data.data_ptr[wr] = HCI_EVENT * (idx == 0) + evtCode * (idx == 1) + length * (idx == 2);
                idx++;
            }
            //wr behind rd, no cross fifo end
            if (((wr + 1) & (EVENT_DATA_SIZE - 1)) < rd)
            {
                wr = ((wr + 1) & (EVENT_DATA_SIZE - 1));
                memcpy(&Event_Data.data_ptr[wr], data, length);
            }
            //cross fifo end
            else
            {
                //length to end of fifo
                tail_length = EVENT_DATA_SIZE - ((wr + 1) & (EVENT_DATA_SIZE - 1));
                if (tail_length >= length)
                {
                    wr = ((wr + 1) & (EVENT_DATA_SIZE - 1));
                    memcpy(&Event_Data.data_ptr[wr], data, length);
                }
                else
                {
                    //first copy the tail data
                    wr = ((wr + 1) & (EVENT_DATA_SIZE - 1));
                    memcpy(&Event_Data.data_ptr[wr], data, tail_length);

                    //second copy the rest data
                    memcpy(&Event_Data.data_ptr[0], data + tail_length, (length - tail_length));
                }
            }
            wr = ((wr + length - 1) & (EVENT_DATA_SIZE - 1));
            Event_Data.wr = wr;
            LOG(0xd104, data[0]);
            osSignalSet(hci_tx_taskId, SIGNAL_EVENT);
            return NO_ERROR;

        }
        else
        {
            //no enough buffer fifo for event data
             LOG(0xdccd,0xf3);
            return NO_ENOUGH_FIFO;
        }

    }

}


void peri_rx_process(void const * argument)
{
    uint16 rd;
    uint8  * peri_rx_data = Peri_Data.data_ptr;
    uint16 data_length = 0;
    uint16 tail_length = 0;
    uint16 processed_length = 0;

    while(1)
    {
        osSignalWait(0x1, osWaitForever);

        if(g_peri_ctrl & UART_USE_DMA)
        {
            uart_dma_rx_response();
        }
        else
        {
            rd = Peri_Data.rd;
            while(rd != Peri_Data.wr)
            {
                rd = ((rd + 1) & (PERI_DATA_SIZE - 1));
                data_length = (Peri_Data.wr - rd + 1 + PERI_DATA_SIZE) & (PERI_DATA_SIZE - 1);

                if(Peri_Data.wr < rd)
                {
                    tail_length = PERI_DATA_SIZE - rd;
                }
                else
                {
                    tail_length = data_length;
                }
                //call hci dispatch function here
                processed_length = peri_hci_process(&(peri_rx_data[rd]), data_length, tail_length);
                if(0xFFFF != processed_length)
                {
                    rd = ((rd + processed_length - 1) & (PERI_DATA_SIZE - 1));
                }
            }
            Peri_Data.rd = rd;
        }
    }

}

void peri_tx_process(void const * argument)
{
    uint16 rd;
    osEvent evt;
    uint8  * event_data = Event_Data.data_ptr;

    while(1)
    {
        evt = osSignalWait(0x0, osWaitForever);
        if(evt.value.signals & SIGNAL_EVENT)
        {
            if(g_peri_ctrl & UART_USE_DMA)
            {
                uart_dma_tx_event();
            }
            else
            {
                rd = Event_Data.rd;
                while(rd != Event_Data.wr)
                {
                    if(uart0_get_tfifo_cnt() <= TX_FIFO_THRD)
                    {
                        rd = ((rd + 1) & (EVENT_DATA_SIZE - 1));
                        uart0_put(event_data[rd]);
                    }

                }
                Event_Data.rd = rd;
            }
        }
    }
}


void uart_dma_rx_handler(uint8 isr_status)        //uart rx
{
    uint16 wr, rd;
    g_dma_lock = 0;

    wr = Peri_Data.wr;
    rd = Peri_Data.rd;

    dma_pause(g_dma_uart_rx_ch);
    dma_reg_read(g_dma_uart_rx_ch * 4 + 6, &wr);

    if( (wr < rd) && ((rd - wr) < (PERI_DATA_SIZE >> 2)) )             //compare wr/rd   check  rd too slow
    {
        g_dma_lock = 1;
    }
    else if((wr >= rd) && ((wr - rd) >= (PERI_DATA_SIZE - (PERI_DATA_SIZE >> 2))))
    {
        g_dma_lock = 1;
    }

    Peri_Data.wr = wr;  //(wr & (PERI_DATA_SIZE - 1));
    osSignalSet(hci_rx_taskId, 0x1);

    if(0 == g_dma_lock)
    {
        dma_continue(g_dma_uart_rx_ch);
    }

}

void uart_dma_tx_handler(uint8 isr_status)
{
    uint32 count;

    if( (1 << 1) &isr_status )  //4/4_done
    {
        dma_disable(g_dma_uart_tx_ch);
        if(g_dma_uart_tx_lock == 1)    //event
        {
            dma_reg_read(g_dma_uart_tx_ch * 4 + 3, &count);
            Event_Data.rd = ((Event_Data.rd + (count & 0xffff)) & (EVENT_DATA_SIZE - 1));
            g_dma_uart_tx_lock = 0;    //unlock
        }
        //uart_dma_tx_event();
        osSignalSet(hci_tx_taskId, SIGNAL_EVENT);
    }

}

void uart_dma_tx_event(void)
{
    uint16 count = 0, rd;
    uint16 bgn_addr;

    if (g_dma_uart_tx_lock == 0)
    {
        rd = Event_Data.rd;
        if(rd == Event_Data.wr)
        {
            dma_disable(g_dma_uart_tx_ch);
            return;
        }
        bgn_addr = ((rd + 1) & (EVENT_DATA_SIZE - 1));

        if( bgn_addr > Event_Data.wr)
        {
            count = EVENT_DATA_SIZE - bgn_addr ;
        }
        else
        {
            if(bgn_addr == 0) //rd +1 == EVENT_DATA_SIZE
            {
                count = Event_Data.wr + 1 ;
            }
            else
            {
                count = Event_Data.wr - rd;         //wr > rd
            }
        }
        g_dma_uart_tx_lock = 1;

        dma_init(g_dma_uart_tx_ch, UART0_TX, count, (uint32)&Event_Data.data_ptr[bgn_addr], (uint32)UART0_TX_ADDR);
        dma_enable(g_dma_uart_tx_ch);
    }

}

void uart_dma_rx_response(void)
{
    uint16 rd;
    uint8  * peri_rx_data = Peri_Data.data_ptr;
    uint16 data_length = 0;
    uint16 tail_length = 0;
    uint16 processed_length = 0;

    rd = Peri_Data.rd;
    while( ((rd + 1) & (PERI_DATA_SIZE - 1)) != Peri_Data.wr)
    {
        rd = ((rd + 1) & (PERI_DATA_SIZE - 1));
        data_length = (Peri_Data.wr - rd + PERI_DATA_SIZE) & (PERI_DATA_SIZE - 1);

        if(Peri_Data.wr < rd)
        {
            tail_length = PERI_DATA_SIZE - rd;
        }
        else
        {
            tail_length = data_length;
        }
        //call hci dispatch function here
        processed_length = peri_hci_process(&(peri_rx_data[rd]), data_length, tail_length);
        if(0xFFFF != processed_length)
        {
            rd = ((rd + processed_length - 1) & (PERI_DATA_SIZE - 1));
            Peri_Data.rd = rd;//((rd - 1) & (PERI_DATA_SIZE - 1));

            if( ((Peri_Data.wr < rd) && ((rd - Peri_Data.wr) >= (PERI_DATA_SIZE >> 2)) ) ||
                ((Peri_Data.wr >= rd) && ((Peri_Data.wr - rd) < (PERI_DATA_SIZE - (PERI_DATA_SIZE >> 2)))))
            {
                if(1 == g_dma_lock)
                {
                    dma_continue(g_dma_uart_rx_ch);
                }
            }
        }
    }

}

void uart_dma_mode_init(void)
{
    /*****DMA uart rx/tx to mem********************/
    g_dma_uart_rx_ch = dma_regsiter(uart_dma_rx_handler);
    g_dma_uart_tx_ch = dma_regsiter(uart_dma_tx_handler);
    if((g_dma_uart_rx_ch == 0xFF) || (g_dma_uart_tx_ch == 0xFF))
    {
        return;
    }
    dma_init(g_dma_uart_rx_ch, UART0_RX, PERI_DATA_SIZE, (uint32)UART0_RX_ADDR, (uint32)&Peri_Data.data_ptr[0]);
    dma_set_isr(g_dma_uart_rx_ch, 0xff);
    dma_enable(g_dma_uart_rx_ch);
    
    dma_init(g_dma_uart_tx_ch, UART0_TX, EVENT_DATA_SIZE, 0, (uint32)UART0_TX_ADDR);
    dma_set_isr(g_dma_uart_tx_ch, DMA_4_4_DONE);
}

void hci_uart_rx_buf_clear_cnt(void)
{
    if(g_peri_ctrl & UART_USE_DMA)
    {
        Peri_Data.wr = 0;
        Peri_Data.rd = (PERI_DATA_SIZE - 1);
    }
}

void hci_uart0_recv_handle(void)
{
    uint16 wr = Peri_Data.wr;
    uint8  * peri_rx_data = Peri_Data.data_ptr;
    uint16 rx_data_not_empty = 0;

    while(uart0_get_rfifo_cnt())
    {
        if(((wr + 1) & (PERI_DATA_SIZE - 1)) == Peri_Data.rd)
        {
            //rd is too slow, wr overlap buffer
            break;
        }
        else
        {
            wr = ((wr + 1) & (PERI_DATA_SIZE - 1));
            rx_data_not_empty = 1;
            peri_rx_data[wr] = uart0_get();
        }
    }
    
    Peri_Data.wr = wr;
    //uart rx fifo timeout
    if(1 == rx_data_not_empty)
    {
        osSignalSet(hci_rx_taskId, 0x1);
    }

}


void hci_api_uart_init(void)
{
    uint32 uart_int_mask ;
    ut_uart_config uart0;

    if(g_peri_ctrl & UART_USE_DMA)
    {
        uart0._B.tfifo_thld_4bit = DMA_FIFO_THRD;
        uart0._B.rfifo_thld_4bit = DMA_FIFO_THRD;
        uart0._B.to_thld_8bit = RX_TIMEOUT;
        uart0._B.stop_bit_2bit = UART_STOPBITS_1;
        uart0._B.byte_size_2bit = UART_CHARFORMAT_8;
        uart0._B.parity_mode_3bit = UART_NO_PARITY;
        uart0._B.msb_1bit = UART_LITTLE;
        uart0._B.lb_snd_1bit = NO_SND_LINE_BREAK;
        uart0._B.flow_ctrl_en_1bit = NO_HW_FLOW;
        uart0._B.rts_thld_4bit = HW_FC_THRD;
        uart0._B.err_discard_1bit = ERROR_DISCARD_DIS;
        uart_int_mask = 0;
        uart0_init(BAUDRATE_921600);
        uart0_set_config(uart0.uart_config);    //uart0.uart_config
        uart0_set_mask(uart_int_mask);
    }
    else
    {
        uart0_init(BAUDRATE_921600);
    }

    hci_dma_init();
}

void hci_dma_init(void)
{
    gDMA.status = 0;

    if(g_peri_ctrl & UART_USE_DMA)
    {
        uart_dma_mode_init();
    }
    dma_reg_write(dma_to_thld, 10);
}



#endif

