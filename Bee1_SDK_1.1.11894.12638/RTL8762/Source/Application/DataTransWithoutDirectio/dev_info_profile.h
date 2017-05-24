/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     dev_info_profile.h
* @brief    Head file for using dis service.
* @details  DIS data structs and external functions declaration.
* @author
* @date     2015-5-12
* @version  v0.1
*************************************************************************************************************
*/
#ifndef _DEVICE_INFO_SERVICE_H
#define _DEVICE_INFO_SERVICE_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/** @brief  Type of parameters set/got from application. */
#define DIS_PARAM_MANUFACTURER_NAME         1
#define DIS_PARAM_MODEL_NUMBER              2
#define DIS_PARAM_SERIAL_NUMBER             3
#define DIS_PARAM_HARDWARE_REVISION         4
#define DIS_PARAM_FIRMWARE_REVISION         5
#define DIS_PARAM_SOFTWARE_REVISION         6
#define DIS_PARAM_SYSTEM_ID                 7
#define DIS_PARAM_IEEE_DATA_LIST            8
#define DIS_PARAM_PNP_ID                    9


#define GATT_SVC_DIS_FIRMWARE_REV_INDEX     2
#define GATT_SVC_DIS_SYSTEM_ID_INDEX        4

#define DIS_SYSTEM_ID_LENGTH                8

/****************************************************************************************************************
* exported functions other .c files may use all defined here.
****************************************************************************************************************/

/*--- services ---*/
/* DIS Device Information Service */
#define GATT_UUID_DEVICE_INFORMATION_SERVICE   0x180A

#define GATT_UUID_CHAR_SYSTEM_ID               0x2A23
#define GATT_UUID_CHAR_MODEL_NUMBER            0x2A24
#define GATT_UUID_CHAR_SERIAL_NUMBER           0x2A25
#define GATT_UUID_CHAR_FIRMWARE_REVISION       0x2A26
#define GATT_UUID_CHAR_HARDWARE_REVISION       0x2A27
#define GATT_UUID_CHAR_SOFTWARE_REVISION       0x2A28
#define GATT_UUID_CHAR_MANUFACTURER_NAME       0x2A29
#define GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST   0x2A2A
#define GATT_UUID_CHAR_PNP_ID                  0x2A50


/* set DIS Service related data from application */
bool DIS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr );
/* get DIS Service related data from application */
bool DIS_GetParameter( uint8_t param_type, void *value_ptr );

uint8_t DIS_AddService(void *pFunc);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
