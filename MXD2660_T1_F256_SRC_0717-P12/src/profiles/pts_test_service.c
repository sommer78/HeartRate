/**
****************************************************************************
* @file   pts_test_service.c
* @brief      pts_test_service
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
#include "hci.h"
#include "ll_struct.h"

extern uint16 att_db_util_add_attribute_uuid16(uint16 uuid16, uint16 flags, uint8 * data, uint16 data_len);
uint16  pts_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);
uint32 pts_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);
uint16  pts_get_attribute_len_CB(uint16 attribute_handle);

uint16 pts_server_hdl[6] = { 0 };
uint16 g_test_value0 = 0;
uint16 g_test_value1 = 0;
uint16 g_test_value2 = 0;
uint16 g_test_value3 = 0;
uint16 g_test_value4 = 0;
uint16 g_test_value5 = 0;


gatt_serviceCBs_t ptsCBs =
{
    pts_get_attribute_len_CB,
    pts_read_attrCB,  // Read callback function pointer
    pts_write_attrCB, // Write callback function pointer
};

uint32 pts_init_service( void )
{
    uint16 hdl = 0;
    uint8 value = 1;
    hdl = GATTS_addServiceStart(0, 0x1A00, NULL, &ptsCBs);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, 0x1A10, NULL, ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    pts_server_hdl[0] = hdl;

    hdl = GATTS_addChar(0, 0x1A11, NULL, ATT_PROPERTY_READ, &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, 0x1A12, NULL, ATT_PROPERTY_AUTHENTICATION_REQUIRED | ATT_PROPERTY_READ, &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, 0x1A13, NULL, ATT_PROPERTY_READ, &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, 0x1A14, NULL, ATT_PROPERTY_READ | (0x0F << 12), &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, 0x1A15, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_INDICATE, &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    pts_server_hdl[1] = hdl;

    hdl = GATTS_addChar(0, 0x1A16, NULL, (ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC | ATT_PROPERTY_READ), &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    pts_server_hdl[2] = hdl;


    hdl = GATTS_addChar(0, 0x1A17, NULL, (ATT_PROPERTY_WRITE | ATT_PROPERTY_READ), &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    pts_server_hdl[3] = hdl;


    hdl = GATTS_addChar(0, 0x1A18, NULL, (ATT_PROPERTY_WRITE | ATT_PROPERTY_AUTHENTICATION_REQUIRED | ATT_PROPERTY_DYNAMIC | ATT_PROPERTY_READ), &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    pts_server_hdl[4] = hdl;

    hdl = GATTS_addChar(0, 0x1A19, NULL, (ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC | ATT_PROPERTY_READ | (0x0F << 12)), &value, 1);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    pts_server_hdl[5] = hdl;

    hdl = att_db_util_add_attribute_uuid16(GATT_CHARACTERISTIC_USER_DESCRIPTION, ATT_PROPERTY_READ, (uint8 *)0x20001000, 64);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

#if 0
    hdl = att_db_util_add_attribute_uuid16(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION, ATT_PROPERTY_READ | (0x0F << 12), (uint8 *)0x20001000, 64);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
#endif

    GATTS_addServiceEnd();
    return  0;

}
uint16  pts_get_attribute_len_CB(uint16 attribute_handle)
{
    if(pts_server_hdl[0] == attribute_handle)
    {
        return 1;
    }
    if(pts_server_hdl[1] == attribute_handle)
    {
        return 2;
    }
    if(pts_server_hdl[2] == attribute_handle)
    {
        return 1;
    }
    if(pts_server_hdl[3] == attribute_handle)
    {
        return 1;
    }
    if(pts_server_hdl[4] == attribute_handle)
    {
        return 1;
    }
    if(pts_server_hdl[5] == attribute_handle)
    {
        return 1;
    }
    return 0;
}
uint16  pts_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size)
{
    if(pts_server_hdl[1] == attribute_handle)
    {
        buffer[0] = g_test_value1&0xff;
		        buffer[1] = (g_test_value1>>8)&0xff;

        return 2;
    }
    if(pts_server_hdl[2] == attribute_handle)
    {
        buffer[0] = g_test_value2;
        return 1;
    }
    if(pts_server_hdl[3] == attribute_handle)
    {
        buffer[0] = g_test_value3;
        return 1;
    }
    if(pts_server_hdl[4] == attribute_handle)
    {
        buffer[0] = g_test_value4;
        return 1;
    }
    if(pts_server_hdl[5] == attribute_handle)
    {
        buffer[0] = g_test_value5;
        return 1;
    }
    return 0;
}


uint32 pts_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size)
{
    if(pts_server_hdl[0] == attribute_handle)
    {
        g_test_value0 = buffer[0];
    }
    else if(pts_server_hdl[1] == attribute_handle)
    {
        g_test_value1 = ((buffer[1]<<8)| buffer[0]);
    }
    else if(pts_server_hdl[2] == attribute_handle)
    {
        g_test_value2 = buffer[0];
    }
    else if(pts_server_hdl[3] == attribute_handle)
    {
        g_test_value3 = buffer[0];
    }
    else if(pts_server_hdl[4] == attribute_handle)
    {
        g_test_value4 = buffer[0];
    }
    else if(pts_server_hdl[5] == attribute_handle)
    {
        g_test_value5 = buffer[0];
    }	
    return 0;
}



