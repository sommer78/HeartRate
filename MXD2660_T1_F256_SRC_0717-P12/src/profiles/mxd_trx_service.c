/**
****************************************************************************
* @file   mxdTxRxService.c
* @brief      profile for txrx
* @author  maxscend\yanping.wang
* @version  Initial Draft
* @date    2016/11/11
* @history
* @note
* detailed description

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#include "gatts_api.h"
#include "mxd_trx_service.h"
#include "mem.h"
#include "hci_event.h"
#include "ll_struct.h"

uint16 txrx_server_hdl[3];
uint8 rxClientCharCfg; 

gatt_serviceCBs_t txrxCBs =
{
    trx_get_attrLenCB,// get dynamic attr len
    trx_read_attrCB,  // Read callback function pointer
    trx_write_attrCB, // Write callback function pointer
};

uint32 trx_init_service( void )
{
    uint16 hdl = 0;
    hdl = GATTS_addServiceStart(0, MXD_TX_RX_SERVICE_UUID, NULL, &txrxCBs);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, TX_CUS_UUID, NULL, (ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    txrx_server_hdl[TX_WRITE] = hdl;

    hdl = GATTS_addChar(0, RX_CUS_UUID, NULL, (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    txrx_server_hdl[RX_NOTIFY] = hdl;

    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    txrx_server_hdl[RX_NOTIFY_CONFIG] = hdl;

    GATTS_addServiceEnd();
    return  0;

}
 uint16  trx_get_attrLenCB(uint16 attribute_handle)
{
    if (txrx_server_hdl[RX_NOTIFY_CONFIG] == attribute_handle)
    {
        return 2;
    }

    return 0;
}
uint16  trx_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size)
{
    if (txrx_server_hdl[RX_NOTIFY_CONFIG] == attribute_handle)
    {
        if (buffer)
        {
            buffer[0] = rxClientCharCfg;
	     buffer[1] = 0;
        }
        return 2;
    }
    return 0;
}


uint32 trx_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size)
{
    if (txrx_server_hdl[TX_WRITE] == attribute_handle)
    {
        txrx_server_receive(buffer, buffer_size);
    }
    else if (txrx_server_hdl[RX_NOTIFY_CONFIG] == attribute_handle)
    {
        rxClientCharCfg = buffer[0];
    }
    return 0;
}


/**
* @brief  server transfer data to client
* @param   uint8 *data databuffer, uinit16 databuffer length
* @return  0 succeed, 1 fail
*/
uint8 txrx_server_transfer(uint8 *data, uint16 length, uint8 source)
{
    uint8 ret_code = 0;

    if (rxClientCharCfg)
    {
        GATTS_send_notify(txrx_server_hdl[RX_NOTIFY], data, length, source);
    }
    else
    {
        ret_code = 0xFE; // notify disable
    }
    return ret_code;
}

/**
* @brief       server receice data from client
* @param
* @return
*/
uint16 txrx_server_receive(uint8 *data, uint16 length)
{
#ifdef HCI_FUNC_EN
    hci_data_rpt_event(0, data, length);
#endif
    return 0;
}

