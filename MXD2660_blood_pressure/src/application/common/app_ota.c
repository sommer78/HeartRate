/**
****************************************************************************
* @file		 app_ota.c
* @brief	 this file reserved for user how to decrypt their encryption image code
* @author	 xiaoyu
* @version	 V1.0.0
* @date		 2017/23/1
* @history	  
* @note		  
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/


#include "system_config.h"
#include "ble_api.h"
#include "app_ota.h"
#include "call_back.h"


extern uint32 ota_init_service(void);
void ota_init(void) 
{
//    g_image_key = 0x12345678;
//    fp_decrypt_func = ota_image_decrypt;
    g_image_key = 0;
    fp_decrypt_func = NULL;
    
    ota_init_service();
}

uint32 ota_image_decrypt(uint32 addr, uint32 code, uint32 key)  
{  
    //TO DO
    uint32 skey;
	uint32 tkey_4,tkey_3,tkey_2,tkey_1,temp_key,encry_code;
	skey = key;

	tkey_4 = (( addr & 0x000000ff) ^ (skey & 0x000000ff)) << 16;
	tkey_3 = (( addr & 0x0000ff00) ^ (skey & 0x0000ff00));
	tkey_2 = (( addr & 0x00ff0000) ^ (skey & 0x00ff0000)) << 8;
	tkey_1 = (( addr & 0xff000000) ^ (skey & 0xff000000)) >> 24;
	temp_key     = tkey_4 | tkey_3 | tkey_2 | tkey_1;
	encry_code   = temp_key ^ code;
    
    return encry_code;
} 

