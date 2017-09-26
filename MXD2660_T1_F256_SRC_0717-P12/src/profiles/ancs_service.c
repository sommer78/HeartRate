/**
****************************************************************************
* @file		 ancs_service.c
* @brief	 the common function all declaration here
* @author	 wenjie
* @version	 V1.0.0
* @date		 2017/01/19
* @history	 modification by xiaoyu 2017/03/02  
* @note		  
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/

#include "system_config.h"
#include "ancs.h"
#include "ancs_service.h"
#include "utility.h"
#include "device_cfg.h"

#ifdef ANCS_FUNC_EN
/*
Get Notification Attributes CMD
byte
0 cmd ==0
1-4:NotificationUID
5-more  : AttributeIDs
AttributeIDs:
NotificationAttributeIDAppIdentifier
= 0,
NotificationAttributeIDTitle
= 1, (Needs to be followed by a 2-bytes max length parameter)
NotificationAttributeIDSubtitle
= 2, (Needs to be followed by a 2-bytes max length parameter)
NotificationAttributeIDMessage
= 3, (Needs to be followed by a 2-bytes max length parameter)
NotificationAttributeIDMessageSize
= 4,
NotificationAttributeIDDate
= 5,
*/
uint8 get_notification_attributes[] = {0, 0, 0, 0, 0,  0,  1, 32, 0,  2, 32, 0,  4, 5};
extern uint8 ancs_discover_flag;

//in order to support ancs, ancs_init need to run when recevie MSG_BLE_CONNECTED message
//after running this function, device will trigger security request
void ancs_init(uint8 conn_hdl)
{
    ancs_connected_handler(conn_hdl);
}

//in order to support ancs, ancs_init need to run when recevie MSG_BLE_ENCRYPTED_IND message
//after running this function, device will inquiry to find the ancs uuid
void ancs_discover_svc_by_uuid(uint8 idx, uint8 is_connection_encrypted)
{
    ancs_encrypt_chg_handler(idx, is_connection_encrypted);
}
void ancs_server_config_notify(uint8 onOff)
{
	ancs_onoff_notify(  onOff);
}
//this is the report of ancs attributes inquiry result, result means all 3 characteristics are found, 0 means fail
void ancs_all_characteristic_found_handler(uint8 idx, uint8 result)
{
    if(result)
    {
        //all characteristcs found, ancs is supported
        APP_DBG(("\n<<ancs service has beed found>>\n"));
        ancs_discover_flag = 1;
    }
    else
    {
        //lack characteristics, not supoort ancs
        APP_DBG(("\n<<no ancs service found>>\n"));  
        ancs_init(idx);
        ancs_discover_svc_by_uuid(idx, 1);
    }
}

void ancs_write_get_data_ntf(uint32 ancs_uid)
{
    ancs_write_get_data(get_notification_attributes, sizeof(get_notification_attributes), ancs_uid);
}

//app can decide whether to recevie the data according to the source types
void ancs_source_ntf_handler(uint8 * data)
{
    /*uint8 conn_hdl = data[0];
    uint8 event_id = data[1];
    uint8 event_flags = data[2];
    uint8 category_id = data[3];
    uint8 category_cnt = data[4];*/
    uint32 ancs_uid = little_endian_read_32bit(data, 5);

    //check whether need to receive data notification
    //app can add code here to decide whether to run ancs_write_get_data_ntf
    ancs_write_get_data_ntf(ancs_uid);

}

//ancs data processing handler, app can decide whether to report or display
void ancs_data_ntf_handler(uint8 length, uint8 * data)
{
    /*uint8 attribute_id = data[0];
    uint16 attribute_length = little_endian_read_16(data, 1);*/
}

#endif

