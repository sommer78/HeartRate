/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dataTrans_profile.h
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/


#ifndef _DATA_TRANSMIT_PROFILE_H_
#define  _DATA_TRANSMIT_PROFILE_H_


#include "profileApi.h"

//#define GATT_UUID_DATA  0xFFE0
extern const uint8_t GATT_UUID_DATATRANSFER_SERVICE[16];
#define GATT_UUID_CHAR_DATA 0xFFE1

#define DATA_VALUE_INDEX  2


void Profile_DataTransSendData(void);
void DataTrans_AddService(void *pFunc);
TProfileResult ProfileHandleDataRx(uint16_t wLength, uint8_t *pValue);

#endif

