#include "wristband_service.h"
#include "system_config.h"
#include "Gatts_api.h"
#include "app_ble.h"
#include "app_task.h"
#include "ble_msg_api.h"
#include "App_message.h"

uint16 wrsitband_server_hdl[WRISTBAND_MAX] = { 0 };
uint8 wrist_buffer[128] = {0};
uint8  wristbandClientCharCfg;

uint16 wristband_getAttrLen_CB(uint16 attribute_handle);
gatt_serviceCBs_t wrsitbandTxRxCBs =
{
    wristband_getAttrLen_CB,
    wristband_read_attrCB,  // Read callback function pointer
    wristband_write_attrCB, // Write callback function pointer
};

uint32  wristband_init_service(void)
{
    uint16 hdl = 0;
    hdl = GATTS_addServiceStart(0, BAND_TX_RX_SERVICE_UUID, NULL, &wrsitbandTxRxCBs);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hdl = GATTS_addChar(0, BAND_TX_UUID, NULL, (ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    wrsitband_server_hdl[WRISTBAND_TX_WRITE] = hdl;

    hdl = GATTS_addChar(0, BAND_RX_UUID, NULL, (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    wrsitband_server_hdl[WRISTBAND_RX_NOTIFY] = hdl;

    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    wrsitband_server_hdl[WRISTBAND_RX_NOTIFY_CONFIG] = hdl;

    GATTS_addServiceEnd();
    return  0;
}

uint8 wristband_notify_AppSendToBle(WRISTBAND_CMD_HEAD *msg)
{

    uint16 att_handle;

    att_handle = wrsitband_server_hdl[WRISTBAND_RX_NOTIFY_CONFIG];
    if (att_handle == 0xffff)
    {
        return 0;
    }
    memcpy(wrist_buffer,msg,msg->length+2);
    GATTS_send_notify(att_handle, wrist_buffer, msg->length+2, 0);
    return msg->length+2;

}
uint16 wristband_getAttrLen_CB(uint16 attribute_handle)
{
    if (wrsitband_server_hdl[WRISTBAND_RX_NOTIFY_CONFIG] == attribute_handle)
    {

        return 2;
    }
    return 0;
}
uint16 wristband_read_attrCB(uint16 attribute_handle, uint16 offset, uint8 *buffer, uint16 buffer_size)
{
    if (wrsitband_server_hdl[WRISTBAND_RX_NOTIFY_CONFIG] == attribute_handle)
    {
        if (buffer)
        {
            buffer[0] = wristbandClientCharCfg;
            return 2;
        }
    }
    return 0;
}

uint32 wristband_write_attrCB(uint16 attribute_handle, uint16 transaction_mode, uint16 offset, uint8 *buffer, uint16 buffer_size)
{
    uint8 *data = NULL;

    if (attribute_handle == wrsitband_server_hdl[WRISTBAND_RX_NOTIFY_CONFIG])
    {
        wristbandClientCharCfg = buffer[0];
    }
    else if (wrsitband_server_hdl[WRISTBAND_TX_WRITE]  == attribute_handle)
    {
        if (buffer_size>20)
        {
            APP_DBG(("wristband msg more than 20 bytes\n"));
            return 0 ;
        }

        data = osPoolAlloc(wristband_data_pool_id);
        if (data == NULL)
        {
            APP_DBG(("wristband malloc data memery failed\n"));
            return 0;
        }

        memcpy(data, &buffer[2], buffer_size-2);
        ble_peri_sendMsgToApp(MSG_WRISTBAND, MSG_WB_PROFILE, buffer[0], buffer[1], data);
    }
    return 0;

}
