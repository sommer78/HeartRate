/**
****************************************************************************
* @file     hid_service.c
* @brief    hid service profile related definition
* @author   wenjie/xiaoyu/yanping
* @version  V1.0.0
* @date     2016/11/14
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#include "gatts_api.h"
#include "gatt_profile_uuid.h"

#include "hid_service.h"
#include "app_ble.h"

extern uint16 batt_server_hdl_idx[3];

// HID Information characteristic value
const uint8 hidInfo[HID_INFORMATION_LEN] =
{
    LO_UINT16(0x0111), HI_UINT16(0x0111),             // bcdHID (USB HID version)
    0x00,                                             // bCountryCode
    HID_KBD_FLAGS                                     // Flags
};


// HID Report Map characteristic value
// Keyboard report descriptor (using format for Boot interface descriptor)
const uint8 hidReportMap[] =
{
    0x05, 0x01,       //Usage Page(Generic Desktop)
    0x09, 0x06,       //Usage(Keyboard)
    0xA1, 0x01,       //Collection(Application)
    0x85, 0x02,       //Report Id(2)
    0x05, 0x07,       //Usage Page(Keypad)
    0x19, 0xE0,       //Usage Minimum(0xE0)
    0x29, 0xE7,       //Usage Maximum(0xE7)
    0x15, 0x00,       //Logical Minimum (0)
    0x25, 0x01,       //Logical Maximum (1)
    0x75, 0x01,       //Report Size (1)
    0x95, 0x08,       //Report Count (8)
    0x81, 0x02,       //Input(Data,Variable,Absolute)

    0x95, 0x01,       //Report Count (1)
    0x75, 0x08,       //Report Size (8)
    0x81, 0x01,       //Input(Constant)

    0x95, 0x05,       //Report Count (5)
    0x75, 0x01,       //Report Size (1)
    0x05, 0x08,       //Usage Page(LEDs)
    0x19, 0x01,       //Usage Minimum(Num Lock)
    0x29, 0x05,       //Usage Maximum(Kana)
    0x91, 0x02,       //Output (Data, Var, Abs)
    0x95, 0x01,       //Report Count (1)
    0x75, 0x03,       //Report Size (3)
    0x91, 0x01,       //Output (Constant)

    0x95, 0x06,       //Report Count (6)
    0x75, 0x08,       //Report Size (8)
    0x15, 0x00,       //Log Min (0)
    0x25, 0xFE,       //Log Max (254)
    0x05, 0x07,       //Usage Pg (Key Codes)
    0x19, 0x00,       //Usage Min (0)
    0x29, 0xFE,       //Usage Max (254)
    0x81, 0x00,       //Input: (Data, Array)
    0xC0,             //End Collection

    0x05,0x0C,        //Usage Page(Consumer)
    0x09,0x01,        //Usage Page(Consumer Control)
    0xA1,0x01,        //Collection(Application) 
    0x85,0x03,        //Report Id(3)

    0x09,0xE9,        	//USAGE (Volume Up)
    0x09,0xEA,        	//USAGE (Volume Down)
    0x09,0xE2,        	//USAGE (Mute)
    0x09,0x30,        	//USAGE (Power)
	0x09,0xCD,			//USAGE (play/pause)
	0x09,0xB5,			//USAGE (next track)
	0x09,0xB6,			//USAGE (previous track)
	0x09,0xB7,     		//USAGE (Stop)
    
    0x15,0x01,        //LOGICAL_MINIMUM (1)
    0x25,0x09,        //LOGICAL_MAXIMUM (8)
	0x75,0x08,        //REPORT_SIZE (8)
    0x95,0x08,        //REPORT_COUNT (8)
   
    0x81,0x00,        //Input: (Data, Array)
    0xC0,             //End Collection

};

/*********************************************************************
 * Profile Attributes - variables
 */
uint8 hidReportKeyIn;
uint8 hidControlPoint;
uint8 hidReportLedOut;
uint8 hidProtocolMode = HID_PROTOCOL_MODE_REPORT;
uint8 hidReportBootKeyOut;
uint8 hidReportBootMouseIn;
uint8 hidReportBootKeyIn;
uint8 hidReportFeature;
uint8 hidReportMouseIn;
uint8 hidReportCCIn;// HID Report characteristic, consumer control input

uint8 hid_mouse_in_ccd = 0;
uint8 hid_cc_in_ccd = 0;
uint8 hid_key_in_ccd = 0;
uint8 hid_boot_key_in_ccd = 0;
uint8 hid_boot_mouse_in_ccd = 0;


// HID Report Reference characteristic descriptor, key input
uint8 hidReportRefKeyIn[HID_REPORT_REF_LEN] =
{ HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT };

// HID Report Reference characteristic descriptor, LED output
uint8 hidReportRefLedOut[HID_REPORT_REF_LEN] =
{ HID_RPT_ID_LED_OUT, HID_REPORT_TYPE_OUTPUT };

// HID Report Reference characteristic descriptor, Feature
uint8 hidReportRefFeature[HID_REPORT_REF_LEN] =
{ HID_RPT_ID_FEATURE, HID_REPORT_TYPE_FEATURE };
// HID Report Reference characteristic descriptor, mouse input
uint8 hidReportRefMouseIn[HID_REPORT_REF_LEN] =
{ HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT };
// HID Report Reference characteristic descriptor, consumer control input
uint8 hidReportRefCCIn[HID_REPORT_REF_LEN] =
{ HID_RPT_ID_CC_IN, HID_REPORT_TYPE_INPUT };

// Service Callbacks
gatt_serviceCBs_t hidCBs =
{
    HidDev_GetAttrLenCB,
    HidDev_ReadAttrCB,  // Read callback function pointer
    HidDev_WriteAttrCB, // Write callback function pointer
};

uint16 hid_server_hdl[24];
HID_RPT_MAP hid_rpt_map[HID_NUM_REPORTS];

uint8 hid_get_handle_idx(uint16 attribute_handle)
{
    uint32 i = 0;
    for (i = 0; i < HID_MAX_IDX; i++)
    {
        if (hid_server_hdl[i] == attribute_handle)
            return i;

    }
    return HID_MAX_IDX;
}

HID_RPT_MAP *hid_get_rpt_by_handle(uint16 attribute_handle)
{
    uint32 i = 0;
    HID_RPT_MAP *p = NULL;

    for (i = 0; i < HID_NUM_REPORTS; i++)
    {
        if (hid_rpt_map[i].att_handle == attribute_handle)
            p = &hid_rpt_map[i];

    }
    return p;
}


/**
* @brief  get handle index from attribute handler
* @param  id
* @param  type
* @return index
*/
uint16 hid_get_rpt_att_handle(uint8 id, uint8 type)

{
    uint32 i = 0;
    for (i = 0; i < HID_NUM_REPORTS; i++)
    {
        if ((hid_rpt_map[i].id == id) && (hid_rpt_map[i].type == type))
            return hid_rpt_map[i].att_handle;

    }
    return 0xffff;
}

void hid_evt_deal(uint8 evt)
{

}

uint8 hid_rcv_report( uint8 len, uint8 *p_data )
{
    // verify data length
    if ( len == HID_LED_OUT_RPT_LEN )
    {
        return TRUE;
    }
    else
    {
        return ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
    }
}

uint8 hid_report(HID_RPT_MAP *p_rpt)
{
    uint8 status = TRUE;

    switch(p_rpt->operation)
    {
        case HID_DEV_OPER_WRITE:
            if(p_rpt->uuid == REPORT_UUID)
            {
                // process write to LED output report; ignore others
                if ( p_rpt->type == HID_REPORT_TYPE_OUTPUT )
                {
                    status = hid_rcv_report(p_rpt->length, p_rpt->value_ptr);
                }
            }
            break;

        case HID_DEV_OPER_READ:
            break;

        case HID_DEV_OPER_ENABLE:
            if((p_rpt->id == HID_RPT_ID_MOUSE_IN) && (p_rpt->type == HID_REPORT_TYPE_INPUT))
            {
                //mouse enable
            }
            break;

        case HID_DEV_OPER_DISABLE:
            if((p_rpt->id == HID_RPT_ID_MOUSE_IN) && (p_rpt->type == HID_REPORT_TYPE_INPUT))
            {
                //mouse disable
            }
            break;
    }
    return status;
}
uint16  HidDev_GetAttrLenCB(uint16 attribute_handle )
{
    uint32 idx = hid_get_handle_idx(attribute_handle);
    uint16 ret = 0;
    switch (idx)
    {

        case HID_BOOT_KEY_OUT_IDX:
        {

            ret = 1;
        }
        break;
        case HID_PROTOCOL_MODE_IDX:
        {

            ret =  1;
        }
        break;
        case HID_REPORT_MOUSE_IN_IDX:
        {
            ret = 1;
        }
        break;
        case HID_REPORT_KEY_IN_IDX:
        {

            ret = 8;
        }
        break;
        case HID_REPORT_CC_IN_IDX:
        {
            ret = 8;
        }
        break;
        case HID_REPORT_LED_OUT_IDX:
        {

            ret = 8;
        }
        break;
        case HID_BOOT_KEY_IN_IDX:
        {

            ret = 1;
        }
        break;
        case HID_BOOT_MOUSE_IN_IDX:
        {

            ret = 1;
        }
        break;
        case HID_FEATURE_IDX:
        {
            ret = 1;
        }
        break;
        case HID_REPORT_KEY_IN_CCCD_IDX:
        {

            ret = 2;
        }
        break;
        case HID_REPORT_MOUSE_IN_CCCD_IDX:
        {

            ret = 2;
        }
        break;
        case HID_REPORT_CC_IN_CCCD_IDX:
        {

            ret = 2;
        }
        break;
        case HID_BOOT_KEY_IN_CCCD_IDX:
        {

            ret = 2;
        }
        break;
        case HID_BOOT_MOUSE_IN_CCCD_IDX:
        {

            ret = 2;
        }
        break;
        default:
            break;
    }
    return ret;
}
uint16  HidDev_ReadAttrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size)
{
    uint32 idx = hid_get_handle_idx(attribute_handle);
    uint16 ret = 0;
    LOG(0xD300, idx);
    switch (idx)
    {

        case HID_BOOT_KEY_OUT_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidReportBootKeyOut;
            }
            ret = 1;
        }
        break;
        case HID_PROTOCOL_MODE_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidProtocolMode;
            }
            ret =  1;
        }
        break;
        case HID_REPORT_MOUSE_IN_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidReportMouseIn;
            }
            ret = 1;
        }
        break;
        case HID_REPORT_KEY_IN_IDX:
        case HID_REPORT_CC_IN_IDX:
        {
            if (buffer)
            {
               memset(buffer,0,buffer_size);  
            }
            ret = 8;

        }
        break;
        case HID_REPORT_LED_OUT_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidReportLedOut;
            }
            ret = 8;
        }
        break;
        case HID_BOOT_KEY_IN_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidReportBootKeyIn;
            }
            ret = 1;
        }
        break;
        case HID_BOOT_MOUSE_IN_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidReportBootMouseIn;
            }
            ret = 1;
        }
        break;
        case HID_FEATURE_IDX:
        {
            if (buffer)
            {
                buffer[0] = hidReportFeature;
            }
            ret = 1;
        }
        break;
        case HID_REPORT_KEY_IN_CCCD_IDX:
        {
            if (buffer)
            {
                buffer[0] = hid_key_in_ccd;
                buffer[1] = 0;
            }
            ret = 2;
        }
        break;
        case HID_REPORT_MOUSE_IN_CCCD_IDX:
        {
            if (buffer)
            {
                buffer[0] = hid_mouse_in_ccd;
                buffer[1] = 0;
            }
            ret = 2;
        }
        break;
        case HID_REPORT_CC_IN_CCCD_IDX:
        {
            if (buffer)
            {
                buffer[0] = hid_cc_in_ccd;
                buffer[1] = 0;
            }
            ret = 2;
        }
        break;
        case HID_BOOT_KEY_IN_CCCD_IDX:
        {
            if (buffer)
            {
                buffer[0] = hid_boot_key_in_ccd;
                buffer[1] = 0;
            }
            ret = 2;
        }
        break;
        case HID_BOOT_MOUSE_IN_CCCD_IDX:
        {
            if (buffer)
            {
                buffer[0] = hid_boot_mouse_in_ccd;
                buffer[1] = 0;
            }
            ret = 2;
        }
        break;
        default:
            break;
    }
    return ret;
}
uint32 HidDev_WriteAttrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size)
{
    uint32 idx = hid_get_handle_idx(attribute_handle);

    LOG(0xD301, idx);
    switch (idx)
    {
        case HID_CONTROL_POINT_IDX:
            hidControlPoint  =  buffer[0];
            if (buffer[0] == HID_CMD_SUSPEND ||  buffer[0] == HID_CMD_EXIT_SUSPEND)
            {
                if(buffer[0] == HID_CMD_SUSPEND)
                {
                    hid_evt_deal(HID_DEV_SUSPEND_EVT);
                }
                else
                {
                    hid_evt_deal(HID_DEV_EXIT_SUSPEND_EVT);
                }
            }
            break;

        case HID_PROTOCOL_MODE_IDX:
            if (buffer[0] == HID_PROTOCOL_MODE_BOOT ||
                buffer[0] == HID_PROTOCOL_MODE_REPORT)
            {
                hidProtocolMode  =  buffer[0];
                if(hidProtocolMode == HID_PROTOCOL_MODE_BOOT)
                {
                    hid_evt_deal(HID_DEV_SET_BOOT_EVT);
                }
                else
                {
                    hid_evt_deal(HID_DEV_SET_REPORT_EVT);
                }
            }
            break;

        case HID_BOOT_KEY_OUT_IDX:
            hidReportBootKeyOut =  buffer[0] ;
            break;

        case HID_REPORT_LED_OUT_IDX:
            hidReportLedOut =  buffer[0] ;
            break;

        case HID_FEATURE_IDX:
            hidReportFeature =  buffer[0] ;
            break;

        case HID_REPORT_MOUSE_IN_CCCD_IDX:
            hid_mouse_in_ccd =  buffer[0] ;
            break;

        case HID_REPORT_KEY_IN_CCCD_IDX:
            hid_key_in_ccd =  buffer[0] ;
            break;

        case HID_REPORT_CC_IN_CCCD_IDX:
            hid_cc_in_ccd = buffer[0];
            break;

        case HID_BOOT_KEY_IN_CCCD_IDX:
            hid_boot_key_in_ccd = buffer[0];
            break;

        case HID_BOOT_MOUSE_IN_CCCD_IDX:
            hid_boot_mouse_in_ccd = buffer[0];
            break;

        default:
            break;
    }

    if (HID_REPORT_MOUSE_IN_CCCD_IDX == idx ||
        HID_REPORT_KEY_IN_CCCD_IDX == idx ||
        HID_REPORT_CC_IN_CCCD_IDX == idx ||
        HID_BOOT_KEY_IN_CCCD_IDX == idx ||
        HID_BOOT_MOUSE_IN_CCCD_IDX == idx)
    {
        HID_RPT_MAP *p_rpt;
        // find report ID in table
        if ((p_rpt = hid_get_rpt_by_handle(attribute_handle)) != NULL)
        {
            p_rpt->uuid = GATT_CLIENT_CHAR_CFG_UUID;
            p_rpt->length = buffer_size;
            p_rpt->value_ptr = buffer;
            if(buffer[0] == GATT_CLIENT_CFG_NOTIFY)
            {
                p_rpt->operation = HID_DEV_OPER_ENABLE;
                hid_report(p_rpt);
            }
            else
            {
                p_rpt->operation = HID_DEV_OPER_DISABLE;
                hid_report(p_rpt);
            }
        }
    }
    if (HID_BOOT_KEY_OUT_IDX == idx ||
        HID_REPORT_KEY_IN_IDX == idx ||
        HID_FEATURE_IDX == idx)
    {
        HID_RPT_MAP *p_rpt;
        if ((p_rpt = hid_get_rpt_by_handle(attribute_handle)) != NULL)
        {
            p_rpt->uuid = REPORT_UUID;
            if (idx == HID_BOOT_KEY_OUT_IDX)
            {
                p_rpt->uuid = BOOT_KEY_OUTPUT_UUID;
            }
            // find report ID in table
            p_rpt->length = buffer_size;
            p_rpt->value_ptr = buffer;
            p_rpt->operation = HID_DEV_OPER_WRITE;
            hid_report(p_rpt);
        }


    }
    return 0;
}

/*********************************************************************
 * @fn      hid_init_service
 *
 * @brief   Initializes the HID Service by registering
 *          GATT attributes with the GATT server.
 *
 * @return  Success or Failure
 */
uint32 hid_init_service(void)
{
    uint16 hdl = 0;
    //add hid service
    hdl = GATTS_addServiceStart(0, HID_SERV_UUID, NULL, &hidCBs);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_SERVICE_IDX] = hdl;

    hdl = GATTS_addChar(0, GATT_INCLUDE_UUID, NULL, ATT_PROPERTY_READ, (uint8*)batt_server_hdl_idx, sizeof(batt_server_hdl_idx));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_INCLUDED_SERVICE_IDX] = hdl;

    //add char: hid info
    hdl = GATTS_addChar(0, HID_INFORMATION_UUID, NULL, ATT_PROPERTY_READ, (uint8*)hidInfo, sizeof(hidInfo));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_INFO_IDX] = hdl;
    //add char: hid control point
    hdl = GATTS_addChar(0, HID_CTRL_PT_UUID, NULL, ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_CONTROL_POINT_IDX] = hdl;


    //add char: hid protocol mode
    hdl = GATTS_addChar(0, PROTOCOL_MODE_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_PROTOCOL_MODE_IDX] = hdl;

    //add char: hid report map
    hdl = GATTS_addChar(0, REPORT_MAP_UUID, NULL, ATT_PROPERTY_READ, (uint8*)hidReportMap, sizeof(hidReportMap));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_MAP_IDX] = hdl;

#if 0
    hdl = GATTS_addChar(0, GATT_EXT_REPORT_REF_UUID, NULL, ATT_PROPERTY_READ, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_EXT_REF_IDX] = hdl;

    //add char:   HID Report characteristic, mouse input
    hdl = GATTS_addChar(0, REPORT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_MOUSE_IN_IDX] = hdl;

    // HID Report characteristic client characteristic configuration
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_MOUSE_IN_CCCD_IDX] = hdl;
    // HID Report Reference characteristic descriptor, key input
    hdl = GATTS_addCharDescrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, hidReportRefMouseIn, sizeof(hidReportRefMouseIn));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
#endif

    //add char:   HID Report characteristic, key input
    hdl = GATTS_addChar(0, REPORT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_KEY_IN_IDX] = hdl;
    // HID Report characteristic client characteristic configuration
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_KEY_IN_CCCD_IDX] = hdl;
    // HID Report Reference characteristic descriptor, key input
    hdl = GATTS_addCharDescrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, hidReportRefKeyIn, sizeof(hidReportRefKeyIn));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    //add char:   HID Report characteristic,consumer control input
    hdl = GATTS_addChar(0, REPORT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_CC_IN_IDX] = hdl;
    // HID Report characteristic client characteristic configuration
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_CC_IN_CCCD_IDX] = hdl;
    // HID Report Reference characteristic descriptor, consumer control input
    hdl = GATTS_addCharDescrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, hidReportRefCCIn, sizeof(hidReportRefCCIn));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

#if 0
    // HID Report characteristic, LED output
    hdl = GATTS_addChar(0, REPORT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_REPORT_LED_OUT_IDX] = hdl;
    // HID Report Reference characteristic descriptor, LED output
    hdl = GATTS_addCharDescrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, hidReportRefLedOut, sizeof(hidReportRefLedOut));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
	
    // HID Boot Keyboard Input Report
    hdl = GATTS_addChar(0, BOOT_KEY_INPUT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_BOOT_KEY_IN_IDX] = hdl;
    // HID Boot Keyboard Input Report characteristic client characteristic configuration
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_BOOT_KEY_IN_CCCD_IDX] = hdl;

    // HID Boot Keyboard output Report
    hdl = GATTS_addChar(0, BOOT_KEY_OUTPUT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_BOOT_KEY_OUT_IDX] = hdl;

    // HID Boot Mouse Input Report
    hdl = GATTS_addChar(0, BOOT_MOUSE_INPUT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_BOOT_MOUSE_IN_IDX] = hdl;
    // HID Boot Mouse Input Report characteristic client characteristic configuration
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_BOOT_MOUSE_IN_CCCD_IDX] = hdl;
	
    // Feature Report
    hdl = GATTS_addChar(0, REPORT_UUID, NULL, ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hid_server_hdl[HID_FEATURE_IDX] = hdl;
    // HID Report Reference characteristic descriptor, feature
    hdl = GATTS_addCharDescrip(GATT_REPORT_REF_UUID, ATT_PROPERTY_READ, hidReportRefFeature, sizeof(hidReportRefFeature));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
#endif

    hid_server_hdl[HID_MAX_IDX] = GATTS_addServiceEnd();

    // Mouse input report
    hid_rpt_map[0].id = hidReportRefMouseIn[0];
    hid_rpt_map[0].type = hidReportRefMouseIn[1];
    hid_rpt_map[0].att_handle = hid_server_hdl[HID_REPORT_MOUSE_IN_IDX];

    // Key input report
    hid_rpt_map[1].id = hidReportRefKeyIn[0];
    hid_rpt_map[1].type = hidReportRefKeyIn[1];
    hid_rpt_map[1].att_handle = hid_server_hdl[HID_REPORT_KEY_IN_IDX] ;

// Consumer Control input report
    hid_rpt_map[2].id = hidReportRefCCIn[0];
    hid_rpt_map[2].type = hidReportRefCCIn[1];
    hid_rpt_map[2].att_handle = hid_server_hdl[HID_REPORT_CC_IN_IDX];

    // LED output report
    hid_rpt_map[3].id = hidReportRefLedOut[0];
    hid_rpt_map[3].type = hidReportRefLedOut[1];
    hid_rpt_map[3].att_handle = hid_server_hdl[HID_REPORT_LED_OUT_IDX];


    // Boot keyboard input report
    // Use same ID and type as key input report
    hid_rpt_map[4].id = hidReportRefKeyIn[0];
    hid_rpt_map[4].type = hidReportRefKeyIn[1];
    hid_rpt_map[4].att_handle = hid_server_hdl[HID_BOOT_KEY_IN_IDX] ;


    // Boot keyboard output report
    // Use same ID and type as LED output report
    hid_rpt_map[5].id = hidReportRefLedOut[0];
    hid_rpt_map[5].type = hidReportRefLedOut[1];
    hid_rpt_map[5].att_handle = hid_server_hdl[HID_BOOT_KEY_OUT_IDX];

    // Boot mouse input report
    hid_rpt_map[6].id = HID_RPT_ID_MOUSE_IN;
    hid_rpt_map[6].type = HID_REPORT_TYPE_INPUT;
    hid_rpt_map[6].att_handle = hid_server_hdl[HID_BOOT_MOUSE_IN_IDX];

    // Feature report
    hid_rpt_map[7].id = hidReportRefFeature[0];
    hid_rpt_map[7].type = hidReportRefFeature[1];
    hid_rpt_map[7].att_handle = hid_server_hdl[HID_FEATURE_IDX];

    return  0;
}


