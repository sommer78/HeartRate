/**
****************************************************************************
* @file     battery_service.c
* @brief
* @author   wenjie
* @version  v1.0.1
* @date     2016/11/14
* @history  modify the coding style
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/


#include "gatts_api.h"
#include "battery_service.h"
#include "mxd2660_adc.h"


// Battery level characteristic
uint8 batt_level = 0;
uint8 batt_level_clientCharCfg;
uint16 batt_server_hdl[2];
uint16 batt_server_hdl_idx[3];


uint16  batt_getAttrLen_CB(uint16 attribute_handle );
// Battery Service Callbacks
gatt_serviceCBs_t battCBs =
{	
	batt_getAttrLen_CB,
	batt_read_attrCB,  // Read callback function pointer
	batt_write_attrCB, // Write callback function pointer
};


uint32 batt_init_service(void)
{
    uint16 hdl = 0;

    hdl = GATTS_addServiceStart(0, BATT_SERV_UUID, NULL, &battCBs);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    batt_server_hdl_idx[0] = hdl;


    hdl = GATTS_addChar(0, BATT_LEVEL_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    batt_server_hdl[BATT_LEVEL_VALUE_IDX] = hdl;
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    batt_server_hdl[BATT_LEVEL_VALUE_CLIENT_CFG_IDX] = hdl;

    hdl = GATTS_addCharDescrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    GATTS_addServiceEnd();

    batt_server_hdl_idx[1] = hdl;
    batt_server_hdl_idx[2] = BATT_SERV_UUID;


    return  0;
}
//int GATTS_send_notify(uint16  handle, uint8  *value, uint16  value_len)
void battNotifyLevel(void)
{
    if (batt_level_clientCharCfg  & GATT_CLIENT_CFG_NOTIFY)
    {
        GATTS_send_notify(batt_server_hdl[BATT_LEVEL_VALUE_IDX], &batt_level, 1, 0);
    }
}


/**
* @brief    Measure the battery level and update the battery
*              level value in the service characteristics.  If
*              the battery level-state characteristic is configured
*              for notification and the battery level has changed
*              since the last measurement, then a notification
*              will be sent.
* @param    none
* @return   level percentage
*/
uint16 batt_level_measured(void)
{
    uint16 level = 0;

    level = adc_vbat_read();
    level = (level * 100) >> 10;   //convert to 0~100%

    // If level has gone down
    if (level < batt_level)
    {
        // Update level
        batt_level = level;

        // Send a notification
        battNotifyLevel();
    }

    return level;
}
uint16  batt_getAttrLen_CB(uint16 attribute_handle )
{
    // Make sure it's not a blob operation (no attributes in the profile are long)
    if (batt_server_hdl[BATT_LEVEL_VALUE_IDX] == attribute_handle)
    {
        return 1;
    }
    else if (batt_server_hdl[BATT_LEVEL_VALUE_CLIENT_CFG_IDX] == attribute_handle)
    {
        return 2;
    }

    return 0;
}
uint16  batt_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size)
{
    // Make sure it's not a blob operation (no attributes in the profile are long)
    if (batt_server_hdl[BATT_LEVEL_VALUE_IDX] == attribute_handle)
    {
        if (buffer)
        {
            batt_level = adc_vbat_read();
            batt_level = (batt_level * 100) >> 10;   //convert to 0~100%
            buffer[0] = batt_level;
        }
        return 1;
    }
    else if (batt_server_hdl[BATT_LEVEL_VALUE_CLIENT_CFG_IDX] == attribute_handle)
    {
        if (buffer)
        {
            buffer[0] = batt_level_clientCharCfg;
            buffer[1] = 0;
        }
        return 2;
    }

    return 0;
}


uint32 batt_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size)
{
    if (batt_server_hdl[BATT_LEVEL_VALUE_CLIENT_CFG_IDX] == attribute_handle)
    {
        batt_level_clientCharCfg = buffer[0];
    }

    return 0;
}
