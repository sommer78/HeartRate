/**
****************************************************************************
* @file		 ancs_service.h
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


#ifndef __ANCS_SERVICE_H_
#define __ANCS_SERVICE_H_

extern void ancs_init(uint8 conn_hdl);
extern void ancs_discover_svc_by_uuid(uint8 idx, uint8 is_connection_encrypted);
extern void ancs_all_characteristic_found_handler(uint8 idx, uint8 result);
extern void ancs_source_ntf_handler(uint8 * data);
extern void ancs_data_ntf_handler(uint8 length, uint8 * data);
extern void ancs_server_config_notify(uint8 onOff);

#endif

