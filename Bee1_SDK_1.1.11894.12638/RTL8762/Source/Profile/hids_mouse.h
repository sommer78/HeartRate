/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hids_mouse.h
  * @brief    Head file for using Human Interface Device Service Mouse.
  * @details  HIDS Mouse data structs and external functions declaration.
  * @author   Lilly_he
  * @date     2015-5-12
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _HIDS_MOUSE_H
#define _HIDS_MOUSE_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileAPI.h"

#define FIVE_KEY_MOUSE

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup HIDS_Mouse HIDS Mouse
  * @brief Human Interface Device Service Mouse
  * @{
  */ 

/** @defgroup HIDS_Mouse_Exported_Constants HIDS Mouse Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */ 
  
/** @defgroup HIDS_Mouse_Application_Parameters HIDS Mouse Application Parameters
  * @brief  Type of parameters set/got from application.
  * @{
  */
#define HIDS_PARAM_PROTOCOL_MODE      1
#define HIDS_PARAM_SUSPEND_MODE       2
#define HIDS_PARAM_HID_INFO           3
#define HIDS_PARAM_REPORT             4
/** @} */

///@cond
/** @brief  HIDS_Mouse related UUIDs. */
#define GATT_UUID_HID 						   0x1812

#define GATT_UUID_CHAR_HID_CONTROL_POINT       0x2A4C
#define GATT_UUID_CHAR_HID_INFO                0x2A4A
#define GATT_UUID_CHAR_REPORT                  0x2A4D
#define GATT_UUID_CHAR_REPORT_MAP              0x2A4B
#define GATT_UUID_CHAR_RECORD_ACCESS_POINT     0x2A52
#define GATT_UUID_CHAR_PROTOCOL_MODE           0x2A4E
#define GATT_UUID_CHAR_BOOT_KB_IN_REPORT       0x2A22
#define GATT_UUID_CHAR_BOOT_KB_OUT_REPORT      0x2A32
#define GATT_UUID_CHAR_BOOT_MOUSE_IN_REPORT    0x2A33

#define GATT_UUID_CHAR_REPORT_REFERENCE        0x2908

#define HOGP_MOUSE_REPORT_ID 1
///@endcond

/** @defgroup HIDS_Mouse_Attribute_Index HIDS Mouse Attribute Index
  * @brief  Index defines for Characteristic's value in HID Device
  * @{
  */
#define GATT_SRV_HID_INDEX_MAX                (15)

#define GATT_SRV_HID_MOUSE_BOOT_INPUT_INDEX   (10)
#define GATT_SRV_HID_MOUSE_BOOT_CCCD_INDEX    (GATT_SRV_HID_MOUSE_BOOT_INPUT_INDEX+1)

#define GATT_SRV_HID_MOUSE_INPUT_INDEX        (13)
#define GATT_SRV_HID_MOUSE_CCCD_INDEX (GATT_SRV_HID_MOUSE_INPUT_INDEX+1)

#define GATT_SVC_HID_INFO_INDEX               (2) /**<  @brief  Index for HID Information chars's value      */
#define GATT_SVC_HID_CONTROL_POINT_INDEX      (4) /**<  @brief  Index for HID Control Point chars's value      */
#define GATT_SVC_HID_PROTOCOL_MODE_INDEX      (6) /**<  @brief  Index for HID Protocol Mode chars's value      */
#define GATT_SVC_HID_REPORT_MAP_INDEX         (8) /**<  @brief  Index for HID Report Map chars's value      */
/** @} */

/** @defgroup HIDS_Mouse_Upstream_Message HIDS Mouse Upstream Message
  * @brief  Upstream message used to inform application.
  * @{
  */
  
/** @defgroup HIDS_Mouse_Read_Info HIDS Mouse Read Info
  * @brief  Parameter for read characteristic value.
  * @{
  */
#define HIDS_READ_PROTOCOL_MODE     1
#define HIDS_READ_SUSPEND_MODE      2
#define HIDS_READ_PARAM_REPORT      3
#define HIDS_READ_BOOT_MOUSE_DATA   4


/** @} */

/** @defgroup HIDS_Mouse_Write_Info HIDS Mouse Write Info
  * @brief  Parameter for write characteristic value.
  * @{
  */
#define HID_WRITE_PROTOCOL_MODE     1
#define HID_WRITE_SUSPEND_MODE      2
#define HID_WRITE_INPUT_REPORT      3
/** @} */

/** @defgroup HIDS_Mouse_Notify_Indicate_Info HIDS Mouse Notify Indicate Info
  * @brief  Parameter for enable or disable notification or indication.
  * @{
  */
#define HID_NOTIFY_INDICATE_MOUSE_ENABLE     1
#define HID_NOTIFY_INDICATE_MOUSE_DISABLE    2
/** @} */

/** @} End of HIDS_Mouse_Upstream_Message */

/** @} End of HIDS_Mouse_Exported_Constants */

/** @defgroup HIDS_Mouse_Exported_Types HIDS Mouse Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */
  
/* Add all public types here */

/** @struct _HID_INFO_ATTRB hids_mouse.h "hids_mouse.h"
  * @brief type define for value of HID information 
  */
typedef struct _HID_INFO_ATTRB
{
	  uint16_t BcdHID;
    uint8_t  CountryCode;
    uint8_t  bFlags;
} HID_INFO_ATTRB;

/** @brief enum for TProfile_HIDReportType*/
typedef enum
{
    HID_INPUT_TYPE   = 1, /**< @enum Input report */
    HID_OUTPUT_TYPE  = 2, /**< @enum Output report */
    HID_FEATURE_TYPE = 3 /**< @enum Feature report */
} TProfile_HIDReportType;

/** @struct _TProfile_REPORT_REFE_TABLE hids_mouse.h "hids_mouse.h"
  * @brief type define for info reference to report data 
  */
typedef struct _TProfile_REPORT_REFE_TABLE
{
    uint8_t report_size;  /**<  @brief  report size according to value of Report Map value, byte */
    uint8_t report_id;    /**<  @brief  report ID defined in value of Report Map Char, byte */
    uint8_t report_type;  /**<  @brief  report Type(Input,Output,Feature), ref: @ref TProfile_HIDReportType */
} TProfile_REPORT_REFE_TABLE;

/** @defgroup HIDS_Mouse_Callback_Data HIDS Mouse Callback Data
  * @brief HIDS Mouse data struct for notification data to application.
  * @{
  */

/** @brief value for HID protocol mode characteristic */
enum TProfile_HID_PROTOCOL_MODE
{
    BOOT_PROTOCOL_MODE = 0, /**< @enum  boot protocol mode */
    REPORT_PROCOCOL_MODE = 1 /**< @enum report protocol mode */
};

enum THID_SUSPEND_MODE
{
	ENTER_SUSPEND_MODE = 0, /**< @enum enter suspend mode */
	EXIT_SUSPEND_MODE /**< @enum exit suspend mode */
};

/** @union _THID_WRITE_PARAMETER hids_mouse.h "hids_mouse.h" */
typedef union _THID_WRITE_PARAMETER
{
    uint8_t protocol_mode;
    uint8_t suspend_mode;
    struct
	{
		uint8_t reportLen;
		uint8_t *report;
	} report_data;
} THID_WRITE_PARAMETER;

/** @struct _THID_WRITE_MSG hids_mouse.h "hids_mouse.h" 
  * @brief write message
  */
typedef struct _THID_WRITE_MSG
{
    uint8_t write_type; /**< ref: @ref HIDS_Mouse_Write_Info */
    THID_WRITE_PARAMETER write_parameter;
} THID_WRITE_MSG;

typedef union _THID_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index; /**< ref: @ref HIDS_Mouse_Notify_Indicate_Info */
	uint8_t read_value_index; /**< ref: @ref HIDS_Mouse_Read_Info */
    THID_WRITE_MSG write;
} THID_UPSTREAM_MSG_DATA;

/** @struct _THID_CALLBACK_DATA hids_mouse.h "hids_mouse.h"
  * @brief callback data
  */
typedef struct _THID_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE     msg_type;
    THID_UPSTREAM_MSG_DATA    msg_data;
} THID_CALLBACK_DATA;
/** @} End of HIDS_Mouse_Callback_Data */

/** @} End of HIDS_Mouse_Exported_Types */

/** @defgroup HIDS_Mouse_Exported_Functions HIDS Mouse Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
bool HID_SendReport(uint8_t ServiceId, uint16_t Index, uint8_t* pData, uint16_t dataLen);
uint8_t HIDS_AddService(void* pFunc);
bool HIDS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr );
/** @} End of HIDS_Mouse_Exported_Functions */

/** @} End of HIDS_Mouse */

/** @} End of RTK_Profile_Module */
  
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* _HIDS_MOUSE_H */
