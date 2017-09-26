/**
****************************************************************************
* @file		 app_ota.h
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


#ifndef __APP_OTA_H__
#define __APP_OTA_H__

#include "system_config.h"


void ota_init(void);
uint32 ota_image_decrypt(uint32 addr, uint32 code, uint32 key); 



#endif

