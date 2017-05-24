/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     profileApi.h
  * @brief    Head file for profile structure.
  * @details  User can take advantage of theses structs and functions to implement specific service.
  * @author   Lilly_he
  * @date     2014-06-26
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef __PROFILE_API_H
#define __PROFILE_API_H
    
#ifdef  __cplusplus
    extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "profileConfig.h"
#include "rtl_types.h"
#include "endialig.h"
#include <blueapi_types.h>
#include <bterrcod.h>
#include <gatt.h>
#include "board.h"


#define ANCS_FROM_DATA_SOURCE 1
#define ANCS_FROM_NOTIFICATION_SOURCE 2
#define ANCS_DISCOVERY_COMPLETE       3
#define ANCS_NOTIFICATION_SOURCE_NOTIFY_ENABLE 4
#define ANCS_NOTIFICATION_SOURCE_NOTIFY_DISABLE 5
#define ANCS_DATA_SOURCE_NOTIFY_ENABLE 6
#define ANCS_DATA_SOURCE_NOTIFY_DISABLE 7

typedef enum{
    NS_EventIDNotificationAdded = 0,
    NS_EventIDNotificationModified= 1,
    NS_EventIDNotificationRemoved= 2,
    NS_EventIDNotificationReserved = 0xff    
}NS_EventIDNotification;

typedef enum{
    NS_EventFlagSilent = (1 << 0),
    NS_EventFlagImportant = (1 << 1),
    NS_EventFlagPreExisting = (1 << 2),
    NS_EventFlagPositiveAction= (1 << 3),
    NS_EventFlagNegativeAction = (1 << 4),
    NS_EventFlagReserved =  (1 << 7)
}NS_EventFlag;

typedef enum{
    NS_CategoryIDOther= 0,
    NS_CategoryIDIncomingCall = 1,
    NS_CategoryIDMissedCall = 2,
    NS_CategoryIDVoicemail= 3,
    NS_CategoryIDSocial= 4,
    NS_CategoryIDSchedule= 5,
    NS_CategoryIDEmail= 6,
    NS_CategoryIDNews= 7,
    NS_CategoryIDHealthAndFitness= 8,
    NS_CategoryIDBusinessAndFinance= 9,
    NS_CategoryIDLocation= 10,
    NS_CategoryIDEntertainment= 11,
    NS_CategoryIDReserved= 255
}NS_CategoryID;

typedef struct _ANCS_NOTIFICATION_SOURCE_DATA
{
    uint8_t EventID;
    uint8_t EventFlags;
    uint8_t CategoryID;
    uint8_t CategoryCount;
    uint32_t NotificationID;   
}TANCS_NOTIFICATION_SOURCE_DATA, *PANCS_NOTIFICATION_SOURCE_DATA;

typedef enum{
    CP_CommandIDGetNotificationAttributes = 0,
    CP_CommandIDGetAppAttributes = 1,
    CP_CommandIDPerformNotificationAction = 2,
    CP_ReservedCommandIDvalues = 255
}CP_CommandID;

typedef enum{
    DS_NotificationAttributeIDAppIdentifier = 0,
    DS_NotificationAttributeIDTitle = 1,// (Needs to be followed by a 2-bytes max length parameter)
    DS_NotificationAttributeIDSubtitle = 2, //(Needs to be followed by a 2-bytes max length parameter)
    DS_NotificationAttributeIDMessage = 3, //(Needs to be followed by a 2-bytes max length parameter)
    DS_NotificationAttributeIDMessageSize = 4,
    DS_NotificationAttributeIDDate = 5,
    DS_NotificationAttributeIDPositiveActionLabel = 6,
    DS_NotificationAttributeIDNegativeActionLabel = 7,
    DS_ReservedNotificationAttributeIDvalues = 255
}DS_NotificationAttributeID;

typedef enum{
    DS_AppAttributeIDDisplayName = 0,
    DS_AppAttributeIDvaluesReserved = 255
}DS_AppAttributeID;

typedef enum{
    CP_ActionIDPositive = 0,
    CP_ActionIDNegative = 1,
    CP_ActionIDvaluesReserved  = 255
}CP_ActionIDvalues;



typedef enum
{
    ATTRIBUTE_PARSE_STATE_NOT_START = 0x00,
    COMMAND_ID_GET_NOTIFICATION_ATTR = 0x01,
    NOTIFICATION_UID1,
    NOTIFICATION_UID2,
    NOTIFICATION_UID3,
    NOTIFICATION_UID4,
    NOTIFICATION_ATTRIBUTE_ID,
    NOTIFICATION_ATTRIBUTE_LEN1,
    NOTIFICATION_ATTRIBUTE_LEN2,
    NOTIFICATION_ATTRIBUTE_READY
} NOTIFICATION_ATTRIBUTE_PARSE_STATE;


typedef enum
{
    COMMAND_ID_GET_APP_ATTR = 0x10,
    APP_IDENTIFIER_START,
    APP_IDENTIFIER_END,
    APP_ATTRIBUTE_ID,
    APP_ATTRIBUTE_LEN1,
    APP_ATTRIBUTE_LEN2,
    APP_ATTRIBUTE_READY
} APP_ATTRIBUTE_PARSE_STATE;

typedef struct {
    uint8_t     command_id;
    uint8_t    notification_uid[4];
    uint8_t     attribute_id;
    uint16_t    attribute_len;
    uint8_t     data[255];
} DS_NOTIFICATION_ATTR;


typedef struct {
    uint8_t     command_id;
    uint8_t     AppIdentifier[255];
    uint8_t     attribute_id;
    uint16_t    attribute_len;
    uint8_t     data[255];
} DS_APP_ATTR;


/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup Profile_Common Profile Common
  * @brief Common Part of Profile, both service and client
  * @{
  */

/** @defgroup Profile_Common_Exported_Types Profile Common Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

/** @defgroup TProfileResult TProfileResult
  * @brief  profile procedure return results
  * @{
  */
typedef enum
{
    ProfileResult_Success               = ATT_OK,   //!<Profile procedure success.
    ProfileResult_InvalidOffset         = (ATT_ERR | ATT_ERR_INVALID_OFFSET),   //!< Read/Write value offset invalid. 
    ProfileResult_InvalidValueSize      = (ATT_ERR | ATT_ERR_INVALID_VALUE_SIZE),   //!< Read/Write value size invalid.
    ProfileResult_InvalidParameter      = (ATT_ERR | ATT_ERR_INVALID_PDU),  //!< Read/Write value parameter invalid.
    ProfileResult_AttrNotFound          = (ATT_ERR | ATT_ERR_ATTR_NOT_FOUND),   //!< Profile procedure attribute not found.
    ProfileResult_AttrNotLong           = (ATT_ERR | ATT_ERR_ATTR_NOT_LONG),    //!< Profile procedure attribute is not long characteristic.
    ProfileResult_OutOfResource         = (ATT_ERR | ATT_ERR_INSUFFICIENT_RESOURCES),   //!< Profile procedure no enough resource(RAM...).
    ProfileResult_Unlikely              = (ATT_ERR | ATT_ERR_UNLIKELY), //!< Profile procedure unlikely result.
    ProfileResult_AppErr                = (ATT_ERR | ATT_ERR_MIN_APPLIC_CODE),   //!< Profile procedure min application code.
    ProfileResult_PreWrQueueFull        = (ATT_ERR | ATT_ERR_PREPARE_QUEUE_FULL),    //!< Profile procedure app a.
    ProfileResult_AplicationErr         = (ATT_ERR | ATT_ERR_MIN_APPLIC_CODE),    //!< Profile procedure application error.
    ProfileResult_AppErr_CccdImproperlyConfigured = (ATT_ERR|ATT_ERR_CCCD_IMPROPERLY_CONFIGURED), //!< CCCD improperly configured.
    ProfileResult_AppErr_ProcAlreadyInProgress = (ATT_ERR|ATT_ERR_PROC_ALREADY_IN_PROGRESS), //!< Procedure already in progress.
    ProfileResult_AppErr_Pending = (APP_ERR | APP_ERR_PENDING),   //!< Pending, App confirm later.
    ProfileResult_AppErr_Have_Confirm = (APP_ERR| APP_ERR_HAVE_CONFIRM)  //!< Procedure already confirm.
} TProfileResult;
/** @} End of TProfileResult */

/** @} End of Profile_Common_Exported_Types */

/** @} End of Profile_Common */

/** @} End of RTK_Profile_Module */


/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup GATT_Service_Common GATT Service Common
  * @brief Common Part of Service
  * @{
  */

/** @defgroup Service_Common_Exported_Constants Service Common Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup CCCD_Bits CCCD Bits
  * @brief  CCCD related bits mask definition.
  * @{
  */
#define GATT_CCCD_NOTIFICATION_BIT              (1) /**< @brief Notification Bit in GATT. 1:Enable; 0:Disable */
#define GATT_CCCD_INDICATION_BIT                (1<<1) /**< @brief Indication Bit in GATT */
/** @} */

/** @defgroup Callback_Events Callback Events
  * @brief Callback events, used to notify application from profile layer.
  * @{
  */
#define PROFILE_EVT_SRV_REG_COMPLETE            0x01    //!< Service register complete event
#define PROFILE_EVT_SEND_DATA_COMPLETE          0x02    //!< Send data complete event
/** @} */

/** @defgroup General_Service_ID General Service ID
  * @brief service ID for general profile events.
  * @{
  */
#define ProfileAPI_ServiceUndefined             0xff
/** @} */

///@cond
/** @brief Max parameters of profile event to inform application. */
#define PROFILE_MAX_INFO_PARAM                  0x01
///@endcond

/** @} End of Service_Common_Exported_Constants */

/** @defgroup Service_Common_Exported_Types Service Common Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */

///@cond
/** @brief common service related data */
typedef struct _GATTDService
{
    bool         Used;
    uint8_t      ServiceIDx;
    void *       pServiceHandle;  /**< service handle provided by GATT server */
} TGATTDService, * PGATTDService;
///@endcond

/** @defgroup TEventInfoCBs_t TEventInfoCBs_t
  * @brief Data for profile to inform application.
  * @{
  */
typedef struct _TEventInfoCBs_t
{
    uint8_t     eventId;                        /**<  @brief EventId defined upper. Refer to @ref Callback_Events*/
    uint16_t    sParas[PROFILE_MAX_INFO_PARAM]; /**<  @brief automatically parsed parameters */
} TEventInfoCBs_t;
/** @} End of TEventInfoCBs_t */

/** @defgroup TSERVICE_CALLBACK_TYPE TSERVICE_CALLBACK_TYPE
  * @brief Event type to inform app.
  * @{
  */
typedef enum _TSERVICE_CALLBACK_TYPE
{
    SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION = 1, /**< CCCD update event */
    SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE = 2, /**< client read event */
    SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE = 3 /**< client write event */
} TSERVICE_CALLBACK_TYPE;
/** @} End of TSERVICE_CALLBACK_TYPE */

/** @defgroup TGATTDWriteIndPostProc TGATTDWriteIndPostProc
  * @brief call back function to execute some post procedure after handle write request from client.
  * @{
  */
typedef void (* TGATTDWriteIndPostProc)(uint8_t serviceID, uint16_t wLength, uint8_t * pValue);
/** @} End of TGATTDWriteIndPostProc */

///@cond
/** @brief Specific service related callback function type definition. */
typedef TProfileResult (*pfnGATTReadAttrCB_t)(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue );
typedef TProfileResult(*pfnGATTWriteAttrCB_t)(uint8_t ServiceId, uint16_t iAttribIndex,
        uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc);
typedef TAppResult(*pfnGATTPreWriteAttrCB_t)(uint8_t ServiceId, uint16_t handle, uint16_t iAttribIndex,
        uint16_t wLength, uint8_t * pValue , uint16_t offset);
typedef TAppResult(*pfnGATTExeWriteAttrCB_t)(uint8_t flag, uint16_t* handle);
typedef void (*pfnGATTUpdateCCCD_t)(uint8_t ServiceId, uint16_t wAttribIndex, uint16_t wCCCBits);
typedef void (*pfnGATTUpdateStatus)(uint8_t ServiceId, uint16_t wAttribIndex);
///@endcond

/** @defgroup pfnAPPHandleInfoCB_t pfnAPPHandleInfoCB_t
  * @brief Function pointer type used to general Callback, to send events to application.
  * @{
  */
typedef TAppResult (*pfnAPPHandleInfoCB_t)(uint8_t serviceId, void*pPara);
/** @} End of pfnAPPHandleInfoCB_t */

#ifdef ANCS_SUPPORT

typedef void (*pfnAPPHandleAncsCB_t)(uint8_t ancs_notification_type, uint8_t* pData, uint8_t len);
void ProfileAPI_RegisterAncsNotification(pfnAPPHandleAncsCB_t pFunc);

#endif
/** @defgroup gattServiceCBs_t gattServiceCBs_t
  * @brief service related callback functions struct.
  * @{
  */
typedef struct
{
    pfnGATTReadAttrCB_t pfnReadAttrCB;          //!< Read callback function pointer
    pfnGATTWriteAttrCB_t pfnWriteAttrCB;        //!< Write callback function pointer
    pfnGATTUpdateCCCD_t pfnUpdateCccdCB;        //!< update cccd callback function pointer
    pfnGATTUpdateStatus pfnUpdateStatusCB;       //!< update status function pointer
} gattServiceCBs_t;
/** @} End of gattServiceCBs_t */

///@cond
/** @brief prepare write and execute write callback functions struct
*   @note these two callback functions are provided by App, not specific service!
*/
typedef struct
{
    pfnGATTPreWriteAttrCB_t pfnPreWriteAttrCB;  //!< Prepare write callback function pointer
    pfnGATTExeWriteAttrCB_t pfnExeWriteAttrCB;      //!< Execute write callback function pointer
}gattPreExeWrCB_t;

/** @brief service related information, which maybe used when register to profile */
typedef struct
{
    uint8_t* pAttrTbl;
    uint16_t Length;
    TGATTDService    Service;
    gattServiceCBs_t cbInfo;
} gattServiceInfos_t;
///@endcond

/** @} End of Service_Common_Exported_Types */

/** @defgroup Service_Common_Exported_Functions Service Common Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
extern TProfileResult  ProfileAPI_GetProfileResult(TAppResult appResult);
extern bool ProfileAPI_AttribValueReadConf(uint8_t ServiceIdx,
                         uint16_t wAttribIndex,
                         uint8_t* pData,
                         uint16_t     wLength);
extern bool ProfileAPI_ExeWriteConf(void);
extern void ProfileAPI_RegisterCB(pfnAPPHandleInfoCB_t pFunc);
extern void ProfileAPI_RegisterPreExeWrCB(gattPreExeWrCB_t pFunc);
extern bool ProfileAPI_AttributeWriteConf(  uint16_t local_MDL_id,
                                            uint8_t ServiceIdx,
                                            uint16_t attribIndex,
                                            TProfileResult wCause );
extern bool ProfileAPI_SendData(uint8_t ServiceId, uint16_t Index, uint8_t* pData, uint16_t dataLen);
extern bool ProfileAPI_AddService(uint8_t* outServiceId, uint8_t* pDB, uint16_t Length, CONST gattServiceCBs_t cCBs);

/** @} End of Service_Common_Exported_Functions */

/** @} End of GATT_Service_Common */

/** @} End of RTK_Profile_Module */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* !defined (__PROFILE_API_H) */
