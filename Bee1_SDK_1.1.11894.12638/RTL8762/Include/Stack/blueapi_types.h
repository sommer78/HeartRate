/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     blueapi_types.h
* @brief    BlueAPI specific Data Type Definitions.
* @author   ranhui & jane
* @date     2015-10-29
* @version  v1.2
*************************************************************************************************************
*/

#if !defined(__BLUEAPI_TYPES_H)
#define      __BLUEAPI_TYPES_H

#include <stdint.h>
#include <stdbool.h>




#if defined (__cplusplus)
extern "C" {
#endif

/** @addtogroup RTK_BlueAPI
  * @{
  */
/** @defgroup BlueAPI_Exported_Constants BlueAPI Exported Constants
  * @brief Macros that other .c files may use all defined here
  * @{
  */
#if !defined(BLUE_API_DEVICE_NAME_LENGTH)
#define BLUE_API_DEVICE_NAME_LENGTH            40       /**< @brief Maximum Device Name Length */
#endif


#if !defined(BLUE_API_VERSION_LENGTH)
#define BLUE_API_VERSION_LENGTH                18        /**< @brief BlueAPI maximum version length */
#endif


#define BLUE_API_BD_SIZE                       6          /**< @brief Bluetooth address size */


#define BLUEAPI_API_VERSION                    0x20131212  /**< @brief BlueAPI version */

#define BLUE_API_USE_LE_FIXED_DISPLAY_VALUE 0x80000000  /**<  @brief Used by API blueAPI_DeviceConfigSecuritySetReq. The leFixedDisplayValue must be ORed with the constant BLUE_API_USE_LE_FIXED_DISPLAY_VALUE to take effect.*/

///@cond
/* constant definitions */
#define BLUE_API_ENABLE_ACL_INFO_CONNECTED_ACTIVE         0x0001
#define BLUE_API_ENABLE_ACL_INFO_CONNECTED_SNIFF          0x0002
#define BLUE_API_ENABLE_ACL_INFO_AUTH_STARTED             0x0004
#define BLUE_API_ENABLE_ACL_INFO_AUTH_SUCCESS             0x0008
#define BLUE_API_ENABLE_ACL_INFO_AUTH_FAILED              0x0010
#define BLUE_API_ENABLE_ACL_INFO_CONNECTION_ENCRYPTED     0x0020
#define BLUE_API_ENABLE_ACL_INFO_CONNECTION_DISCONNECTED  0x0040
#define BLUE_API_ENABLE_ACL_INFO_ADDRESS_RESOLVED         0x0080
#define BLUE_API_ENABLE_ACL_INFO_DEVICE_ROLE              0x0100

#define BLUE_API_ENABLE_ACL_INFO_ALL                      0x01FF

#define BLUE_API_ENABLE_MCL_INFO_IDLE                     0x0001
#define BLUE_API_ENABLE_MCL_INFO_CTRL_CONNECTING          0x0002
#define BLUE_API_ENABLE_MCL_INFO_CTRL_CONNECTED           0x0004
#define BLUE_API_ENABLE_MCL_INFO_CTRL_DISCONNECTING       0x0008
#define BLUE_API_ENABLE_MCL_INFO_CTRL_LISTEN              0x0010
#define BLUE_API_ENABLE_MCL_INFO_DATA_CONNECTING          0x0020
#define BLUE_API_ENABLE_MCL_INFO_DATA_CONNECTED           0x0040
#define BLUE_API_ENABLE_MCL_INFO_DATA_DISCONNECTING       0x0080
#define BLUE_API_ENABLE_MCL_INFO_DATA_LISTEN              0x0100
#define BLUE_API_ENABLE_MCL_INFO_CTRL_WAIT_FOR_RSP        0x0200
#define BLUE_API_ENABLE_MCL_INFO_WAIT_FOR_RSP             0x0400

#define BLUE_API_ENABLE_MCL_INFO_ALL                      0x07FF
///@endcond

/** @defgroup GATT_appearance_definitions GATT appearance definitions
  * @brief GATT appearance definitions that can used by blueAPI_DeviceConfigAppearanceSetReq.
  * @{
  */
/*GATT appearance definitions */
#define GATT_APPEARANCE_UNKNOWN                                0
#define GATT_APPEARANCE_GENERIC_PHONE                          64
#define GATT_APPEARANCE_GENERIC_COMPUTER                       128

#define GATT_APPEARANCE_GENERIC_WATCH                          192
#define GATT_APPEARANCE_WATCH_SPORTS_WATCH                     193

#define GATT_APPEARANCE_GENERIC_CLOCK                          256
#define GATT_APPEARANCE_GENERIC_DISPLAY                        320
#define GATT_APPEARANCE_GENERIC_REMOTE_CONTROL                 384
#define GATT_APPEARANCE_GENERIC_EYE_GLASSES                    448
#define GATT_APPEARANCE_GENERIC_TAG                            512
#define GATT_APPEARANCE_GENERIC_KEYRING                        576
#define GATT_APPEARANCE_GENERIC_MEDIA_PLAYER                   640
#define GATT_APPEARANCE_GENERIC_BARCODE_SCANNER                704

#define GATT_APPEARANCE_GENERIC_THERMOMETER                    768
#define GATT_APPEARANCE_THERMOMETER_EAR                        769

#define GATT_APPEARANCE_GENERIC_HEART_RATE_SENSOR              832
#define GATT_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT      833

#define GATT_APPEARANCE_GENERIC_BLOOD_PRESSURE                 896
#define GATT_APPEARANCE_BLOOD_PRESSURE_ARM                     897
#define GATT_APPEARANCE_BLOOD_PRESSURE_WRIST                   898

#define GATT_APPEARANCE_HUMAN_INTERFACE_DEVICE                 960
#define GATT_APPEARANCE_KEYBOARD                               961
#define GATT_APPEARANCE_MOUSE                                  962
#define GATT_APPEARANCE_JOYSTICK                               963
#define GATT_APPEARANCE_GAMEPAD                                964
#define GATT_APPEARANCE_DIGITIZER_TABLET                       965
#define GATT_APPEARANCE_CARD_READER                            966
#define GATT_APPEARANCE_DIGITAL_PEN                            967
#define GATT_APPEARANCE_BARCODE_SCANNER                        968

#define GATT_APPEARANCE_GENERIC_GLUCOSE_METER                  1024

#define GATT_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR         1088
#define GATT_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE         1089
#define GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE         1090
#define GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP          1091

#define GATT_APPEARANCE_GENERIC_CYCLING                        1152
#define GATT_APPEARANCE_CYCLING_CYCLING_COMPUTER               1153
#define GATT_APPEARANCE_CYCLING_SPEED_SENSOR                   1154
#define GATT_APPEARANCE_CYCLING_CADENCE_SENSOR                 1155
#define GATT_APPEARANCE_CYCLING_POWER_SENSOR                   1156
#define GATT_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR       1157

#define GATT_APPEARANCE_GENERIC_PULSE_OXIMETER                 3136
#define GATT_APPEARANCE_FINGERTIP                              3137
#define GATT_APPEARANCE_WRIST_WORN                             3138
#define GATT_APPEARANCE_GENERIC_WEIGHT_SCALE                   3200

#define GATT_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACTIVITY        5184
#define GATT_APPEARANCE_LOCATION_DISPLAY_DEVICE                5185
#define GATT_APPEARANCE_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE 5186
#define GATT_APPEARANCE_LOCATION_POD                           5187
#define GATT_APPEARANCE_LOCATION_AND_NAVIGATION_POD            5188
/** @} End of GATT_appearance_definitions */
/** @} End of BlueAPI_Exported_Constants */

/** @defgroup BlueAPI_Command_Definitions BlueAPI Command Definitions
  * @brief BlueAPI message type
  *
  * Communication between the application and BlueAPI uses the following general protocol:
  *
  * A message is generally followed by an appropriate reply. Messages going from an application to BlueAPI are called Requests; 
  * the corresponding answer from BlueAPI is called a Response. Messages initiated by BlueAPI are called Indications; 
  * the corresponding answer from an application is called a Confirmation. 
  * This is also reflected in the naming convention for messages: every message name ends with the appropriate suffix (Req, Rsp, Ind and Conf).
  *
  * Finally the message type Information is used by BlueAPI to indicate events to the application without expecting any reaction from application side. 
  * The appropriate suffix for this message type is Info.
  * @{
  */

/** @brief message type definitions */
typedef enum
{
    blueAPI_EventConnectMDLReq = 0x01,
    blueAPI_EventConnectMDLRsp,

    blueAPI_EventDisconnectMDLReq,
    blueAPI_EventDisconnectMDLRsp,
    blueAPI_EventDisconnectMDLInd,
    blueAPI_EventDisconnectMDLConf,
    blueAPI_EventCreateMDLInd,
    blueAPI_EventCreateMDLConf,
    blueAPI_EventConnectMDLInfo,
    blueAPI_EventDeleteMDLInfo,
    blueAPI_EventMCLStatusInfo,
    blueAPI_EventACLStatusInfo,

    blueAPI_EventRegisterReq,
    blueAPI_EventRegisterRsp,
    blueAPI_EventReleaseReq,
    blueAPI_EventReleaseRsp,
    blueAPI_EventActInfo,
    blueAPI_EventInternalEventInfo,

    blueAPI_EventDeviceConfigSetReq,
    blueAPI_EventDeviceConfigSetRsp,

    blueAPI_EventDeviceConfigAppearanceGetReq,
    blueAPI_EventDeviceConfigAppearanceGetRsp,
	blueAPI_EventDeviceConfigPerPrefConnParamGetReq,
	blueAPI_EventDeviceConfigPerPrefConnParamGetRsp,
	blueAPI_EventDeviceConfigDeviceNameGetReq,
    blueAPI_EventDeviceConfigDeviceNameGetRsp,

    /*--- GATT ---*/
    /* server */
    blueAPI_EventGATTServiceRegisterReq,
    blueAPI_EventGATTServiceRegisterRsp,

    blueAPI_EventGATTAttributeUpdateReq,
    blueAPI_EventGATTAttributeUpdateRsp,
    blueAPI_EventGATTAttributeUpdateStatusInd,
    blueAPI_EventGATTAttributeUpdateStatusConf,
    blueAPI_EventGATTAttributeReadInd,
    blueAPI_EventGATTAttributeReadConf,
    blueAPI_EventGATTAttributeWriteInd,
    blueAPI_EventGATTAttributeWriteConf,
    blueAPI_EventGATTAttributePrepareWriteInd,
    blueAPI_EventGATTAttributePrepareWriteConf,
    blueAPI_EventGATTAttributeExecuteWriteInd,
    blueAPI_EventGATTAttributeExecuteWriteConf,
    blueAPI_EventGATTAttributeWriteCommandInfo,

    blueAPI_EventGATTCCCDInfo,
    /* client */
    blueAPI_EventGATTDiscoveryReq,
    blueAPI_EventGATTDiscoveryRsp,
    blueAPI_EventGATTDiscoveryInd,
    blueAPI_EventGATTDiscoveryConf,
    blueAPI_EventGATTAttributeReadReq,
    blueAPI_EventGATTAttributeReadRsp,
    blueAPI_EventGATTAttributeReadMultipleReq,
    blueAPI_EventGATTAttributeReadMultipleRsp,
    blueAPI_EventGATTAttributeWriteReq,
    blueAPI_EventGATTAttributeWriteRsp,
    blueAPI_EventGATTAttributePrepareWriteReq,
    blueAPI_EventGATTAttributePrepareWriteRsp,
    blueAPI_EventGATTAttributeExecuteWriteReq,
    blueAPI_EventGATTAttributeExecuteWriteRsp,

    blueAPI_EventGATTAttributeInd,
    blueAPI_EventGATTAttributeConf,
    blueAPI_EventGATTAttributeNotificationInfo,

    blueAPI_EventGATTSecurityReq,
    blueAPI_EventGATTSecurityRsp,
    blueAPI_EventGATTServerStoreInd,
    blueAPI_EventGATTServerStoreConf,
    blueAPI_EventGATTMtuSizeInfo,       //0x40

    blueAPI_EventPairableModeSetReq,
    blueAPI_EventPairableModeSetRsp,


    blueAPI_EventUserPasskeyReqInd,
    blueAPI_EventUserPasskeyReqConf,
    blueAPI_EventUserPasskeyReqReplyReq,
    blueAPI_EventUserPasskeyReqReplyRsp,

    blueAPI_EventUserPasskeyNotificationInfo,

    blueAPI_EventRemoteOOBDataReqInd,
    blueAPI_EventRemoteOOBDataReqConf,


    blueAPI_EventAuthResultInd,
    blueAPI_EventAuthResultConf,
    blueAPI_EventAuthResultRequestInd,
    blueAPI_EventAuthResultRequestConf,

    blueAPI_EventLEAdvertiseReq,
    blueAPI_EventLEAdvertiseRsp,
    blueAPI_EventLEAdvertiseParameterSetReq,
    blueAPI_EventLEAdvertiseParameterSetRsp,
    blueAPI_EventLEAdvertiseDataSetReq,
    blueAPI_EventLEAdvertiseDataSetRsp,
    blueAPI_EventLEScanReq,
    blueAPI_EventLEScanRsp,
    blueAPI_EventLEScanInfo,
    blueAPI_EventLEModifyWhitelistReq,
    blueAPI_EventLEModifyWhitelistRsp,
    blueAPI_EventLEConnectionUpdateReq,
    blueAPI_EventLEConnectionUpdateRsp,
    blueAPI_EventLEConnectionUpdateInd,
    blueAPI_EventLEConnectionUpdateConf,
    blueAPI_EventLEConnectionParameterInfo,
    blueAPI_EventLEPrivacyModeReq,
    blueAPI_EventLEPrivacyModeRsp,

    blueAPI_EventCreateLEDataChannelReq,
    blueAPI_EventCreateLEDataChannelRsp,
    blueAPI_EventCreateLEDataChannelInd,
    blueAPI_EventCreateLEDataChannelConf,
    blueAPI_EventDisconnectLEDataChannelReq,
    blueAPI_EventDisconnectLEDataChannelRsp,
    blueAPI_EventDisconnectLEDataChannelInd,
    blueAPI_EventDisconnectLEDataChannelConf,
    blueAPI_EventSendLEFlowControlCreditReq,
    blueAPI_EventSendLEFlowControlCreditRsp,
    blueAPI_EventLEDataReq,
    blueAPI_EventLEDataRsp,
    blueAPI_EventLEDataInd,
    blueAPI_EventLEDataConf,
    blueAPI_EventLEDataChannelParameterInfo,
    blueAPI_EventLEDataChannelCreditsAlertInfo,
    blueAPI_EventLEDataChannelDeleteInfo,
    blueAPI_EventLEPsmSecuritySetReq,
    blueAPI_EventLEPsmSecuritySetRsp,
    blueAPI_EventVendorSetVoiceParaReq,
    blueAPI_EventVendorSetVoiceParaRsp,
    blueAPI_EventSetBleTxPowerReq,
    blueAPI_EventSetBleTxPowerRsp,
    blueAPI_EventSetRandomAddressReq,
    blueAPI_EventSetRandomAddressRsp,
    blueAPI_EventSetDataLengthReq,
    blueAPI_EventSetDataLengthRsp,
    blueAPI_EventDataLengthChangeInfo,

    blueAPI_EventIdle,
} TBlueAPI_Command;
typedef TBlueAPI_Command * PBlueAPI_Command;
/** @} End of BlueAPI_Command_Definitions */

/** @defgroup Specific_Data_Type BlueAPI Specific Data Type Definitions
  * @brief BlueAPI self defined basic data types
  * @{
  */
/** This enum type defines a generic enumeration for cause values. */
typedef enum
{
    blueAPI_CauseSuccess             = 0x00,     /**<  Indicates that an operation is completed successfully. */
    blueAPI_CauseAccept,                         /**<  Indicates that an operation is accepted. */
    blueAPI_CauseReject,                         /**<  Indicates that an operation is rejected. */
    blueAPI_CauseResourceError,                  /**<  Indicates that an operation could not be completed
                                                                                               due to resource constraints. */
    blueAPI_CauseInvalidParameter,               /**<  Indicates that at least one parameter of a request is invalid. */
    blueAPI_CauseInvalidState,                   /**<  Indicates that a requested operation cannot be processed
                                                                                                because the connection is not in the required state. */
    blueAPI_CauseConnectionDisconnect,           /**<  Indicates that a connection is terminated intentionally. */
    blueAPI_CauseConnectionLost,                 /**<  Indicates that a connection is terminated due to RF link loss. */
    blueAPI_CauseAuthenticationFailed,           /**<  Indicates that an authentication attempt failed. */
    blueAPI_CauseInitTimeout,                    /**<  Indicates that the initialization of the  Bluetooth protocol stack
                                                                                               could not be completed within a given time limit.  */
    blueAPI_CauseInitOutofSync,                  /**<  Indicates that the Bluetooth protocol stack detected
                                                                                               a communication failure with the HCI controller. */
    blueAPI_CauseInitHardwareFailure,            /**<  Indicates that the BlueAPI implementation or
                                                                                               the Bluetooth protocol stack detected a hardware failure during startup. */
    blueAPI_CauseLowerLayerError,                /**<  This cause indicates that a lower layer module
                                                                                               (L2CAP, ATT, HCI ..) detected a problem. */
    blueAPI_CauseAddressNotResolved,             /**<  Indicates that the Bluetooth protocol stack is not able to
                                                                                               map a blueAPI_RemoteBDTypeLEResolved address. */
    blueAPI_CauseUnspecified         = 0xFD,     /**<  Unspecified error occurred. */
    blueAPI_CauseNotSupported        = 0xFE      /**<  Indicated that the operation requested is not supported. */
} TBlueAPI_Cause;
typedef TBlueAPI_Cause * PBlueAPI_Cause;

typedef uint16_t TBlueAPI_SubCause;
typedef TBlueAPI_SubCause * PBlueAPI_SubCause;

  
/** This enum type define remote adress type */
typedef enum
{
    blueAPI_RemoteBDTypeClassic  = 0x00,         /**<  Classic BR/EDR Bluetooth address. */
    blueAPI_RemoteBDTypeLEPublic = 0x02,         /**<  Bluetooth low energy public address. */
    blueAPI_RemoteBDTypeLERandom = 0x03,         /**<  Bluetooth low energy random address. */
    blueAPI_RemoteBDTypeAny = 0x04,              /**<  Any Bluetooth address. */
    blueAPI_RemoteBDTypeLEResolved = 0x0A        /**<  Bluetooth low energy peer with resolvable private address
                                                       that distributed it's IRK. */
} TBlueAPI_RemoteBDType;
typedef TBlueAPI_RemoteBDType * PBlueAPI_RemoteBDType;

/** This enum type define local adress type */
typedef enum
{
    blueAPI_LocalBDTypeLEPublic = 0x00,         /**<  Bluetooth low energy public address. */
    blueAPI_LocalBDTypeLERandom = 0x01,         /**<  Bluetooth low energy random address. */
} TBlueAPI_LocalBDType;
typedef TBlueAPI_LocalBDType * PBlueAPI_LocalBDType;

/** This enum type describes the possible MCL statuses. */
typedef enum
{
    blueAPI_MCLIdle                   = 0x01,    /**<  MCL is allocated. */
    blueAPI_MCLControlConnected       = 0x03,    /**<  MCL has a connected control channel but no data link. */
    blueAPI_MCLDataConnecting         = 0x06,    /**<  MCL has a data link setup pending. */
    blueAPI_MCLDataConnected          = 0x07,    /**<  MCL has a data link connected. */
    blueAPI_MCLDataDisconnecting      = 0x08,    /**<  MCL has a data link disconnect pending. */
    blueAPI_MCLDataListening          = 0x09,    /**<  MCL waits for incoming data link. */
    blueAPI_MCLReleased               = 0x0C,    /**<  MCL is not allocated. */
} TBlueAPI_MCLStatus;
typedef TBlueAPI_MCLStatus * PBlueAPI_MCLStatus;

/** This enum type describes the possible ACL statuses. */
typedef enum
{
    blueAPI_ACLConnectedActive        = 0x01,    /**<  ACL connection is established and in active mode. */
    blueAPI_ACLAuthenticationStarted  = 0x03,    /**<  Authentication for this ACL connection has started. */
    blueAPI_ACLAuthenticationSuccess  = 0x04,    /**<  ACL connection is successfully authenticated. */
    blueAPI_ACLAuthenticationFailure  = 0x05,    /**<  Authentication for this ACL connection failed. */
    blueAPI_ACLConnectionEncrypted    = 0x06,    /**<  ACL connection data traffic is encrypted. */
    blueAPI_ACLConnectionDisconnected = 0x07,    /**<  ACL connection is no longer connected. */
    blueAPI_ACLConnectionNotEncrypted = 0x08,    /**<  ACL connection encryption is disabled. */
    blueAPI_ACLAddressResolved        = 0x09,    /**<  BLE ACL connection address resolved. */
    blueAPI_ACLConnectedLinkStatus    = 0x0D,    /**<  ACL Link Status Information availabe. */

} TBlueAPI_ACLStatus;
typedef TBlueAPI_ACLStatus * PBlueAPI_ACLStatus;

/** @brief define Link key type */
typedef enum
{
    blueAPI_LinkKeyTypeLELocalLTK       = 0x11,  /**<  BLE Long Term Key Blob  */
    blueAPI_LinkKeyTypeLERemoteLTK      = 0x12,  /**<  BLE Long Term Key Blob   */
    blueAPI_LinkKeyTypeLERemoteIRK      = 0x14,  /**<  BLE Identity Resolving Key  */
    blueAPI_LinkKeyTypeDeleted          = 0xFF   /**<  Link key is no longer valid and deleted  */
} TBlueAPI_LinkKeyType;


/**  @brief Application provided handle that will be stored by the BT Stack.*/
typedef void * TBlueAPIAppHandle;             /**<  app -> BlueAPI */
/** @} End of Specific_Data_Type */
///@cond
typedef void * TBlueAPIHandle;                 /**<   BlueAPI -> app */
///@endcond

///@cond
typedef struct _TBlueAPI_ConnectMDLReqGATT
{
    uint16_t scanInterval;
    uint16_t scanWindow;
    uint16_t scanTimeout;
    uint16_t connIntervalMin;
    uint16_t connIntervalMax;
    uint16_t connLatency;
    uint16_t supervisionTimeout;
	uint16_t CE_Length;
} TBlueAPI_ConnectMDLReqGATT;
typedef TBlueAPI_ConnectMDLReqGATT * PBlueAPI_ConnectMDLReqGATT;



typedef union _TBlueAPI_ConnectMDLReqParam
{
    TBlueAPI_ConnectMDLReqGATT  gatt;
} TBlueAPI_ConnectMDLReqParam;
typedef TBlueAPI_ConnectMDLReqParam * PBlueAPI_ConnectMDLReqParam;

typedef struct _TBlueAPI_ConnectMDLReq
{
    uint8_t          remote_BD[BLUE_API_BD_SIZE];
    TBlueAPI_RemoteBDType   remote_BD_Type;
	TBlueAPI_LocalBDType   local_BD_Type;
    TBlueAPI_ConnectMDLReqParam    p;
} TBlueAPI_ConnectMDLReq;
typedef TBlueAPI_ConnectMDLReq * PBlueAPI_ConnectMDLReq;
///@endcond

/** @defgroup Upstream_message Upstream Message
  * @brief BlueAPI-> APP.    The BlueAPI message is transferred to the application using callback function MDHmsgHandlerCallback registered by blueAPI_RegisterReq.
  * @{
  */
/** @defgroup Upstream_callback Callback function
  * @brief Callback function that handle upstream messages
  * @{
  */
  /** @} End of Upstream_callback */
/** @defgroup Upstream_message_structure Upstream Message Structure
  * @brief Upstream Message Structure
  * @{
  */
  /** @} End of Upstream_message_structure */
  
  /** @defgroup message_command_data TBlueAPI_UsCommandData 
  * @brief Struct TBlueAPI_UsCommandData Definitions
  * @{
  */
  /** @} End of message_command_data */
  /** @} End of Upstream_message */

/** @addtogroup message_command_data
  * @{
  */

/** @defgroup Connection_Management Connection Management
  * @brief Connection Management Command Data
  * @{
  */  
/**
 * @brief  blueAPI_EventConnectMDLRsp command data.
 *
 * This command is used by the BT Stack to respond to a blueAPI_ConnectGATTMDLReq initiated from the APP.
*/
typedef struct
{
    uint8_t    remote_BD[BLUE_API_BD_SIZE];   /**<  @brief Bluetooth device address of remote device to connect to. */
    TBlueAPI_RemoteBDType   remote_BD_type;   /**<  @brief Type of remote BD address. */
    uint16_t   local_MDL_ID;                  /**<  @brief Local link ID for this link. */
    TBlueAPI_Cause    cause;                  /**<  @brief Indicates the result of the connectMDL transaction. */
} TBlueAPI_ConnectMDLRsp;
typedef TBlueAPI_ConnectMDLRsp * PBlueAPI_ConnectMDLRsp;

/**
 * @brief  blueAPI_EventCreateMDLInd command data.
 *
 * This command is used by the BT Stack to indicate a creation of a new MDL to the APP.
 * This message is triggered by a connection that is either initiated from the local, or from the remote side.
*/
typedef struct
{
    uint8_t              remote_BD[BLUE_API_BD_SIZE];  /**<  @brief Bluetooth device address of remote device. */
    TBlueAPI_RemoteBDType   remote_BD_type;            /**<  @brief Type of remote BD address. */
    uint16_t             local_MDL_ID;                 /**<  @brief Local link ID to be created. */
}  TBlueAPI_CreateMDLInd;
typedef TBlueAPI_CreateMDLInd * PBlueAPI_CreateMDLInd;

///@cond
typedef struct
{
    uint16_t             local_MDL_ID;
    uint8_t              maxTPDUusCredits;
    TBlueAPI_Cause              cause;
}  TBlueAPI_CreateMDLConf;
typedef TBlueAPI_CreateMDLConf * PBlueAPI_CreateMDLConf;

typedef struct
{
    uint16_t            local_MDL_ID;
    TBlueAPI_Cause             cause;
}  TBlueAPI_DisconnectMDLReq;
typedef TBlueAPI_DisconnectMDLReq * PBlueAPI_DisconnectMDLReq;
///@endcond

/**
 * @brief  blueAPI_EventDisconnectMDLInd command data.
 *
 * This command is used by the BT Stack to indicate a disconnection of an established MDL to the APP.
 * This message is triggered by a disconnection that is either initiated from the local, or from the remote side.
 */
typedef struct
{
    uint16_t          local_MDL_ID;    /**<  @brief Local link ID to be disconnected. */
    TBlueAPI_Cause  cause;             /**<  @brief Indicates the cause for the disconnect. */
}  TBlueAPI_DisconnectMDLInd;
typedef TBlueAPI_DisconnectMDLInd * PBlueAPI_DisconnectMDLInd;

///@cond
typedef struct
{
    uint16_t          local_MDL_ID;
}  TBlueAPI_DisconnectMDLConf;
typedef TBlueAPI_DisconnectMDLConf * PBlueAPI_DisconnectMDLConf;
///@endcond


/**
 * @brief  blueAPI_EventDisconnectMDLRsp command data.
 *
 * This command is used by the BT Stack to respond to a DisconnectMDLReq of the APP.
 * This message is generated when the disconnection process is completed.
 */
typedef struct
{
    uint16_t          local_MDL_ID;    /**<  @brief Local link ID to be disconnected. */
    TBlueAPI_Cause           cause;    /**<  @brief Indicates the result of the DisconnectMDL transaction. */
}  TBlueAPI_DisconnectMDLRsp;
typedef TBlueAPI_DisconnectMDLRsp * PBlueAPI_DisconnectMDLRsp;

/**
 * @brief  blueAPI_EventConnectMDLInfo command data.
 *
 * To inform the APP about the parameters of the new MDL connection.
 */
typedef struct
{
    uint16_t    local_MDL_ID;    /**<  @brief Local link ID for this MDL. */
    uint16_t    dsPoolID;        /**<  @brief Pool ID that is assigned by BlueAPI for downstream data. */
    uint16_t    dsDataOffset;    /**<  @brief The minimum data offset that is required by BlueAPI for downstream data. */
    uint16_t    maxTPDUSize;     /**<  @brief For GATT based connections this parameter signals the MTU used on this link. */
    uint8_t     maxTPDUdsCredits;/**<  @brief For GATT connections this parameter is applicable only for:
                                                                GATTAttributeUpdateReq messages resulting in an ATT layer Handle Value Notification
                                                                GATTAttributeWriteReq messages resulting in an ATT layer Write Command */
} TBlueAPI_ConnectMDLInfo;
typedef TBlueAPI_ConnectMDLInfo * PBlueAPI_ConnectMDLInfo;

/**
 * @brief  blueAPI_EventDeleteMDLInfo command data.
 *
 * This command is used by the BT Stack to inform the APP that a local link ID is no longer valid. 
 */
typedef struct
{
    uint16_t    local_MDL_ID;    /**<  @brief Local link ID deleted. */
} TBlueAPI_DeleteMDLInfo;
typedef TBlueAPI_DeleteMDLInfo * PBlueAPI_DeleteMDLInfo;

/**
 * @brief  blueAPI_EventMCLStatusInfo command data.
 *
 * To inform the APP about the status of a given MCL connection. 
 * This fact is to be considered for a single MDL connection on a single MCL.
 */
typedef struct
{
    uint8_t           remote_BD[BLUE_API_BD_SIZE];   /**<  @brief Bluetooth device address of remote device. */
    uint16_t          local_MCL_ID;                  /**<  @brief Identifier to reference this MCL */
    TBlueAPI_MCLStatus           status;             /**<  @brief Status of referenced MCL */
} TBlueAPI_MCLStatusInfo;
typedef TBlueAPI_MCLStatusInfo * PBlueAPI_MCLStatusInfo;
/** @} End of Connection_Management */
/** @} End of message_command_data */

/** @addtogroup Specific_Data_Type
  * @{
  */
  
/** @brief Define key type */
typedef enum
{
    blueAPI_KeyType_Unauthenticated       = 0x04,  /**<  Unauthenticated no MITM protection. */
    blueAPI_KeyType_Authenticated         = 0x05,  /**<  Authenticated MITM protection. */
    blueAPI_KeyType_Delete                = 0xff,  /**<  The key is deleted. */
} TBlueAPI_KeyType;
/** @} End of Specific_Data_Type */

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup Connection_Management
  * @{
  */
/**  @brief union element in TBlueAPI_ACLStatusParam. */
typedef struct
{
    TBlueAPI_KeyType  keyType;
    uint8_t               keySize;
} TBlueAPI_ACLStatusParamAuth;
typedef TBlueAPI_ACLStatusParamAuth * PBlueAPI_ACLStatusParamAuth;
///@cond
typedef struct
{
    uint8_t           remote_BD[BLUE_API_BD_SIZE];
    TBlueAPI_RemoteBDType           remote_BD_type;
} TBlueAPI_ACLStatusParamResolve;
typedef TBlueAPI_ACLStatusParamResolve * PBlueAPI_ACLStatusParamResolve;



typedef struct
{
    uint8_t   linkQuality;
    int8_t    rssi;
    uint16_t  failedContacts;
    int8_t    txPower;
    int8_t    absoluteRssi;
} TBlueAPI_ACLStatusParamLinkStatus;
typedef TBlueAPI_ACLStatusParamLinkStatus  * PBlueAPI_ACLStatusParamLinkStatus;
///@endcond

/**  @brief element in TBlueAPI_ACLStatusInfo. */
typedef union
{
    TBlueAPI_ACLStatusParamAuth    auth;  /**<  @brief Valid when status is blueAPI_ACLAuthenticationStarted,blueAPI_ACLAuthenticationSuccess,
    *blueAPI_ACLAuthenticationFailure, blueAPI_ACLConnectionEncrypted or blueAPI_ACLConnectionNotEncrypted */
    TBlueAPI_ACLStatusParamResolve resolve;

    TBlueAPI_ACLStatusParamLinkStatus   linkStatus;
} TBlueAPI_ACLStatusParam;
typedef TBlueAPI_ACLStatusParam * PBlueAPI_ACLStatusParam;

/**
 * @brief  blueAPI_EventACLStatusInfo command data.
 *
 * Inform the APP about the status of a given ACL connection.
 */
typedef struct
{
    uint8_t       remote_BD[BLUE_API_BD_SIZE];     /**<  @brief Bluetooth device address of remote device. */
    TBlueAPI_RemoteBDType       remote_BD_type;    /**<  @brief Type of remote BD address.*/
    TBlueAPI_ACLStatus       status;               /**<  @brief Status of referenced ACL connection. */
    TBlueAPI_ACLStatusParam  p;
} TBlueAPI_ACLStatusInfo;
typedef TBlueAPI_ACLStatusInfo * PBlueAPI_ACLStatusInfo;
/** @} End of Connection_Management */

///@cond
typedef struct
{
    void *             MDHmsgHandlerCallback;
    uint32_t           apiVersion;
} TBlueAPI_RegisterReq;
typedef TBlueAPI_RegisterReq * PBlueAPI_RegisterReq;
///@endcond

/** @defgroup Application_Management Application Management
  * @brief Application Management Command Data
  * @{
  */
/**
 * @brief  blueAPI_EventRegisterRsp command data.
 *
 * This commond is used to respond to RegisterReq.
 */
typedef struct
{
    TBlueAPI_Cause     cause;    /**< @brief Indicates the result of the operation */
} TBlueAPI_RegisterRsp;
typedef TBlueAPI_RegisterRsp * PBlueAPI_RegisterRsp;
/** @} End of Application_Management */
///@cond
typedef struct
{
    uint8_t Reserved;
} TBlueAPI_ReleaseReq;
typedef TBlueAPI_ReleaseReq * PBlueAPI_ReleaseReq;

typedef struct
{
    TBlueAPI_Cause     cause;
} TBlueAPI_ReleaseRsp;
typedef TBlueAPI_ReleaseRsp * PBlueAPI_ReleaseRsp;
///@endcond

/** @defgroup Configuration_command System Activation, Configuration and Status
  * @brief System Activation, Configuration and Status Command Data
  * @{
  */
/**
 * @brief  blueAPI_EventActInfo command data.
 *
 * This command is used by BT Stack to inform the APP about the activation or reactivation of the BT Stack.
 * This message is the first message APP will receive from the BT Stack after registration.
*/
typedef struct
{
    uint8_t     local_BD[BLUE_API_BD_SIZE];       /**<  @brief Bluetooth device address of the local device. */
    uint8_t     version[BLUE_API_VERSION_LENGTH]; /**<  @brief UTF-8 formatted, zero terminated user friendly version string of this firmware. */
    uint16_t    systemPoolID;                     /**<  @brief Bluetooth stack system buffer pool ID. Buffers from this pool can be used in messages that are not connection related and that carry only a small amount of data. */
    TBlueAPI_Cause     cause;                     /**<  @brief Indicates the result of the system initialization. */
} TBlueAPI_ActInfo;
typedef TBlueAPI_ActInfo * PBlueAPI_ActInfo;
/** @} End of Configuration_command */
/** @} End of message_command_data */

/** @addtogroup Specific_Data_Type
  * @{
  */
  
/** This enum type describes possible causes for an InternalEventInfo. */
typedef enum
{
    blueAPI_InternalEventInvalidCreateMDLConf = 0x01,           /**<  The BT Stack received a CreateMDLConf message
                                                                                         from the application that could not be handled and that will be ignored. */
    blueAPI_InternalEventInvalidDisconnectMDLConf,              /**<  The BT Stack received a DisconnectMDLConf message
                                                                                         from the application that could not be handled and that will be ignored. */
    blueAPI_InternalEventInvalidSecurityConf,                   /**<The BT Stack received a security related confirmation  message
                                                                                         from the application that could not be handled and that will be ignored. */
    blueAPI_InternalEventInvalidRemoteEvent,                    /**<  The BT Stack received an invalid event or discovered
                                                                                         an invalid behavior from / of the remote device that can affect the functionality of the connection. */
    blueAPI_InternalEventCommunicationTimeout,                  /**<  The BT Stack implements several timeouts
                                                                                          that prevent the system to be driven into a deadlock situation
                                                                                          by a nonfunctional APP or remote device. This timeout should never occur during normal operation  */
    blueAPI_InternalEventInvalidGATTAttributeReadConf,          /**<  The BT Stack received a GATTAttributeReadConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidGATTAttributeWriteConf,         /**<  The BT Stack received a GATTAttributeWriteConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidGATTAttributePrepareWriteConf,  /**<  The BT Stack received a GATTAttributePrepareWriteConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidGATTExecuteWriteConf,           /**<  The BT Stack received a GATTExecuteWriteConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidGATTDiscoveryConf,              /**<  The BT Stack received a GATTDiscoveryConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidGATTAttributeConf,              /**<  The BT Stack received a GATTAttributeConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidLEConnectionUpdateConf,         /**<  The BT Stack received a LEConnectionUpdateConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidCreateLEDataChannelConf,        /**<  The BT Stack received a CreateLEDataChannelConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidDisconnectLEDataChannelConf,    /**<  The BT Stack received a DisconnectLEDataChannelConf message
                                                                                          from the application that could not be handled and that will be ignored */
    blueAPI_InternalEventInvalidLEDataConf,                     /**<  The BT Stack received a LEDataConf message
                                                                                          from the application that could not be handled and that will be ignored */
} TBlueAPI_InternalEventType;
/** @} End of Specific_Data_Type */

/** @addtogroup message_command_data
  * @{
  */
  /** @addtogroup Configuration_command
  * @{
  */
/**
 * @brief  blueAPI_EventInternalEventInfo command data.
 *
 * This command is used by the BT Stack to inform the APP about an illegal message
 * sent by the APP or regarding illegal events on the air interface.
*/
typedef struct
{
    TBlueAPI_InternalEventType    eventType;      /**< @brief Indicates the reason for this indication.  */
    uint32_t   eventInfo;      /**<  @brief Additional information for internal reproduction and tracing purpose.  */
    TBlueAPI_Cause    cause;   /**<  @brief Provides some additional information why this message is initiated.  */
} TBlueAPI_InternalEventInfo;
typedef TBlueAPI_InternalEventInfo * PBlueAPI_InternalEventInfo;
/** @} End of Configuration_command */
/** @} End of message_command_data */

/* **************************************************************** */
/* Secutity Management specific definitions                         */
/* **************************************************************** */
/** @addtogroup Specific_Data_Type
  * @{
  */
/** This enum type describes IO capabilities. */
typedef enum
{
    blueAPI_IOCapDisplayOnly,                        /**<  Only a Display present, no Keyboard or Yes/No Keys.  */
    blueAPI_IOCapDisplayYesNo,                       /**<  Display and Yes/No Keys present.  */
    blueAPI_IOCapKeyboardOnly,                       /**<  Only a Keyboard present, no Display.  */
    blueAPI_IOCapNoIO,                               /**<  No input/output capabilities.  */
    blueAPI_IOCapKeyboardDisplay                     /**<  BLE: Keyboard and Display present.  */
} TBlueAPI_IOCapabilities;

/** This enum type describes authentication requirements. */
typedef enum
{
    blueAPI_AuthNoMITMRequiredNoStore,     /**<  MITM protection not required, no bonding.  */
    blueAPI_AuthMITMRequiredNoStore,       /**<  MITM protection required, use IO capabilities, no bonding.  */
    blueAPI_AuthNoMITMRequiredBonding,     /**<  MITM protection not required, perform dedicated Bonding.  */
    blueAPI_AuthMITMRequiredBonding        /**<  MITM protection required, use IO capabilities, perform dedicated Bonding.  */
} TBlueAPI_AuthRequirements;

/** This enum type describes store bond modes. */
typedef enum
{
    blueAPI_StoreBondModeNoStore,                   /**<  Bonds persists for the duration of the authenticated connection.  */
    blueAPI_StoreBondModeNVStore,                   /**<  Bonds are permanently stored in the NVRAM of the Bluetooth device.  */
    blueAPI_StoreBondModeRAMStore,                  /**<  Bonds are stored in RAM of the Bluetooth device and will be lost after power cycle.  */
    blueAPI_StoreBondModeExtStore                   /**<  Bonds are expected to be stored externally. Please refer to blueAPI_EventAuthResultRequestInd.  */
} TBlueAPI_StoreBondModes;
/** @} End of Specific_Data_Type */

///@cond
typedef struct
{
    bool                       enablePairableMode;
    TBlueAPI_AuthRequirements  AuthRequirements;
    TBlueAPI_IOCapabilities    IOCapabilities;
    bool                       remoteOOBDataPresent;
} TBlueAPI_PairableModeSetReq;
typedef TBlueAPI_PairableModeSetReq * PBlueAPI_PairableModeSetReq;
///@endcond

/** @addtogroup Specific_Data_Type
  * @{
  */
/** @brief Device config opcode*/
typedef enum
{
    blueAPI_DeviceConfigDeviceName,       /**<  Response to blueAPI_DeviceConfigDeviceNameSetReq.  */
    blueAPI_DeviceConfigAppearance,       /**<  Response to blueAPI_DeviceConfigAppearanceSetReq.  */
    blueAPI_DeviceConfigPerPrefConnParam, /**<  Response to blueAPI_DeviceConfigPerPrefConnParamSetReq.  */
    blueAPI_DeviceConfigSecurity,         /**<  Response to blueAPI_DeviceConfigSecuritySetReq.  */
    blueAPI_DeviceConfigStore             /**<  Response to blueAPI_DeviceConfigStoreSetReq.  */
} TBlueAPI_DeviceConfigOpcode;
/** @} End of Specific_Data_Type */

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup Configuration_command
  * @{
  */
///@cond
typedef struct
{
    uint8_t   deviceName[BLUE_API_DEVICE_NAME_LENGTH];
} TBlueAPI_DeviceConfigDeviceName;
///@endcond

/**  @brief element in TBlueAPI_DeviceConfigAppearanceGetRsp. */
typedef struct
{
    uint16_t   appearance;
} TBlueAPI_DeviceConfigAppearance;
/**  @brief element in TBlueAPI_DeviceConfigPerPrefConnParamGetRsp. */
typedef struct
{
    uint16_t   connIntervalMin;
    uint16_t   connIntervalMax;
    uint16_t   slaveLatency;
    uint16_t   supervisionTimeout;
} TBlueAPI_DeviceConfigPerPrefConnParam;

///@cond
typedef struct
{
    uint32_t                leFixedDisplayValue;
} TBlueAPI_DeviceConfigSecurity;

typedef struct
{
    TBlueAPI_StoreBondModes storeBondMode;
    uint8_t                 storeBondSize;
} TBlueAPI_DeviceConfigStore;


typedef struct
{
    TBlueAPI_DeviceConfigOpcode         opCode;
    union
    {
        TBlueAPI_DeviceConfigDeviceName   device;
        TBlueAPI_DeviceConfigAppearance appearance;
        TBlueAPI_DeviceConfigPerPrefConnParam conn;
        TBlueAPI_DeviceConfigSecurity     security;
        TBlueAPI_DeviceConfigStore        store;
    } p;
} TBlueAPI_DeviceConfigSetReq;
typedef TBlueAPI_DeviceConfigSetReq * PBlueAPI_DeviceConfigSetReq;
///@endcond

/**
 * @brief  blueAPI_EventDeviceConfigSetRsp command data.
 *
 * This command is used by the BT Stack to respond to a DeviceConfigSetReq initiated from the APP.
*/
typedef struct
{
    TBlueAPI_DeviceConfigOpcode opCode;  /**<  @brief Device config opcode */
    TBlueAPI_Cause  cause;               /**<  @brief Indicates the result of DeviceConfigSetReq transaction. */
} TBlueAPI_DeviceConfigSetRsp;
typedef TBlueAPI_DeviceConfigSetRsp * PBlueAPI_DeviceConfigSetRsp;

/**
 * @brief  blueAPI_EventDeviceConfigAppearanceGetRsp command data.
 *
 *  This command is used by the BT Stack to respond to a DeviceConfigAppearanceGetReq of the APP.  
 */
typedef struct
{
    TBlueAPI_Cause  cause;                      /**<  @brief Indicates the result of DeviceConfigAppearanceGetReq transaction. */
	TBlueAPI_DeviceConfigAppearance appearance; /**<  @brief The External appearance of this device. */
} TBlueAPI_DeviceConfigAppearanceGetRsp;
typedef TBlueAPI_DeviceConfigAppearanceGetRsp * PBlueAPI_DeviceConfigAppearanceGetRsp;

/**
 * @brief  blueAPI_EventDeviceConfigPerPrefConnParamGetRsp command data.
 *
 *  This command is used by the BT Stack to respond to a DeviceConfigPerPrefConnParamGetReq of the APP.  
 */
typedef struct
{
	TBlueAPI_Cause	cause;	/**<  @brief Indicates the result of DeviceConfigPerPrefConnParamGetReq transaction. */
	TBlueAPI_DeviceConfigPerPrefConnParam conn; /**<  @brief Peripheral Preferred Connection Parameters. */
} TBlueAPI_DeviceConfigPerPrefConnParamGetRsp;
typedef TBlueAPI_DeviceConfigPerPrefConnParamGetRsp * PBlueAPI_DeviceConfigPerPrefConnParamGetRsp;

/**
 * @brief  blueAPI_EventDeviceConfigDeviceNameGetRsp command data.
 *
 *  This command is used by the BT Stack to respond to a DeviceConfigDeviceNameGetReq of the APP.  
 */
typedef struct
{
 
    TBlueAPI_Cause cause;                /**<  @brief Indicates the result of DeviceConfigDeviceNameGetReq transaction. */         
    uint16_t       nameLength;           /**<  @brief Device name length.  */
    uint16_t       gap;                  /**<  @brief Offset of value in deviceName[].  */
    uint8_t        deviceName[1];        /**<  @brief Device name.  */
} TBlueAPI_DeviceConfigDeviceNameGetRsp;
typedef TBlueAPI_DeviceConfigDeviceNameGetRsp * PBlueAPI_DeviceConfigDeviceNameGetRsp;
/** @} End of Configuration_command */

/** @defgroup Security_Management Security Management
  * @brief Security Management Command Data
  * @{
  */
/**
 * @brief  blueAPI_EventPairableModeSetRsp command data.
 *
 *  This command is used by the BT Stack to respond to a PairableModeSetReq of the APP.  
 */
typedef struct
{
    TBlueAPI_Cause     cause;    /**<  @brief Indicates the result of the requested operation. */
} TBlueAPI_PairableModeSetRsp;
typedef TBlueAPI_PairableModeSetRsp * PBlueAPI_PairableModeSetRsp;

/**
 * @brief  blueAPI_EventAuthResultRequestInd command data.
 *
 *  The BT Stack will send this message to the APP to request information of a prior performed successful bonding procedure 
 *  that was indicated by an AuthResultInd message. 
 *  This message will only be used by the BT Stack if the BT Stack is configured to store bonding results externally. 
 */
typedef struct
{
    uint8_t               remote_BD[BLUE_API_BD_SIZE];  /**<  @brief Bluetooth device address of remote device. */
    TBlueAPI_RemoteBDType remote_BD_Type;               /**<  @brief Type of remote BD address. */
    TBlueAPI_LinkKeyType  keyType;                      /**<  @brief Key type of link key. */
    uint16_t              restartHandle;                /**<  @brief Value != 0x0000 when requesting multiple keys. */
} TBlueAPI_AuthResultRequestInd;
typedef TBlueAPI_AuthResultRequestInd * PBlueAPI_AuthResultRequestInd;

///@cond
typedef struct
{
    uint8_t               remote_BD[BLUE_API_BD_SIZE];
    TBlueAPI_RemoteBDType remote_BD_Type;//TBlueAPI_RemoteBDType
    uint8_t               linkKeyLength;
    uint8_t               linkKey[28];
    TBlueAPI_LinkKeyType  keyType;//TBlueAPI_LinkKeyType
    uint16_t              restartHandle;
    TBlueAPI_Cause        cause;                
} TBlueAPI_AuthResultRequestConf;
typedef TBlueAPI_AuthResultRequestConf * PBlueAPI_AuthResultRequestConf;
///@endcond

/**
 * @brief  blueAPI_EventUserPasskeyReqInd command data.
 *
 *  If a SMP procedure is started that requires Keyboard functionality from the remote peer,
 *  and display functionality from the local device the BT Stack will send this message to the APP to request User interaction.
 */
typedef struct
{
    uint8_t            remote_BD[BLUE_API_BD_SIZE];         /**<  @brief Bluetooth device address of remote device. */
} TBlueAPI_UserPasskeyReqInd;
typedef TBlueAPI_UserPasskeyReqInd * PBlueAPI_UserPasskeyReqInd;

///@cond
typedef struct
{
    uint8_t        remote_BD[BLUE_API_BD_SIZE];        
    TBlueAPI_Cause        cause;               
} TBlueAPI_UserPasskeyReqConf;
typedef TBlueAPI_UserPasskeyReqConf * PBlueAPI_UserPasskeyReqConf;



typedef struct
{
    uint8_t        remote_BD[BLUE_API_BD_SIZE];        
    uint32_t       passKey;             
    TBlueAPI_Cause        cause;               
} TBlueAPI_UserPasskeyReqReplyReq;
typedef TBlueAPI_UserPasskeyReqReplyReq * PBlueAPI_UserPasskeyReqReplyReq;
///@endcond

/**
 * @brief  blueAPI_EventUserPasskeyReqReplyRsp command data.
 *
 *  With this message the BT Stack will respond to a SSP driven UserPasskeyReqReplyReq message of the APP. 
 */
typedef struct
{
    TBlueAPI_Cause     cause;                /**<  @brief Indicates the result of the operation.*/
} TBlueAPI_UserPasskeyReqReplyRsp;
typedef TBlueAPI_UserPasskeyReqReplyRsp * PBlueAPI_UserPasskeyReqReplyRsp;

/**
 * @brief  blueAPI_EventUserPasskeyNotificationInfo command data.
 *
 *  If a SMP procedure is completed that requires Keyboard functionality from the remote
 *  and display functionality from the local device, the BT Stack will send this message to notify
 *  the APP about the initial state of the display content.
 */
typedef struct
{
    uint8_t            remote_BD[BLUE_API_BD_SIZE];         /**<  @brief Bluetooth device address of remote device.  */
    uint32_t           displayValue;         /**<  @brief Value to be displayed as a 6 digit decimal number.  */
} TBlueAPI_UserPasskeyNotificationInfo;
typedef TBlueAPI_UserPasskeyNotificationInfo * PBlueAPI_UserPasskeyNotificationInfo;


/**
 * @brief  blueAPI_EventRemoteOOBDataReqInd command data.
 *
 *   If a SMP procedure is started that requires OOB functionality from the local and remote device, 
 *  the BT Stack will send this message to the APP to request OOB data from the remote device via OOB communication.
 */
typedef struct
{
    uint8_t     remote_BD[BLUE_API_BD_SIZE];                   /**<  @brief Bluetooth device address of remote device.  */
} TBlueAPI_RemoteOOBDataReqInd;
typedef TBlueAPI_RemoteOOBDataReqInd * PBlueAPI_RemoteOOBDataReqInd;

///@cond
typedef struct
{
    uint8_t               remote_BD[BLUE_API_BD_SIZE];         
    uint8_t               C[16];               
    TBlueAPI_Cause        cause;              
} TBlueAPI_RemoteOOBDataReqConf;
typedef TBlueAPI_RemoteOOBDataReqConf * PBlueAPI_RemoteOOBDataReqConf;
///@endcond

/**
 * @brief  blueAPI_EventAuthResultInd command data.
 *
 *  This command is used by the BT Stack to indicate the resulting information of a performed authentication. 
 */
typedef struct
{
    uint8_t               remote_BD[BLUE_API_BD_SIZE];    /**<  @brief Bluetooth device address of remote device.  */
    TBlueAPI_RemoteBDType remote_BD_Type;   /**<  @brief Remote address type */
    uint8_t               linkKeyLength;                  /**<  @brief Link key length  */
    TBlueAPI_LinkKeyType  keyType;                        /**<  @brief Key type of link key  */
    uint8_t               linkKey[28];          /**<  @brief Link Key  */
    TBlueAPI_Cause        cause;                /**<  @brief Indicates the result of the authentication.
                                    blueAPI_CauseSuccess    Authentication is completed successfully */
} TBlueAPI_AuthResultInd;
typedef TBlueAPI_AuthResultInd * PBlueAPI_AuthResultInd;

///@cond
typedef struct
{
    uint8_t               remote_BD[BLUE_API_BD_SIZE];         
    TBlueAPI_RemoteBDType remote_BD_Type;
    TBlueAPI_Cause               cause;                
} TBlueAPI_AuthResultConf;
typedef TBlueAPI_AuthResultConf * PBlueAPI_AuthResultConf;
///@endcond
/** @} End of Security_Management */

/*------------------------------ GATT messages -----------------------------*/
/*------------- GATT server application messages -------------*/
///@cond
typedef enum
{
    blueAPI_ServiceApplicationDefined = 0,   
} TBlueAPI_GATTServiceID;
///@endcond
/** @defgroup GATT_Server GATT Server
  * @brief GATT Server Command Data
  * @{
  */
  
///@cond
typedef struct
{
    uint16_t    nbrOfAttrib;
    void *      pService;
} TBlueAPI_GATTServiceRegisterReq;
typedef TBlueAPI_GATTServiceRegisterReq * PBlueAPI_GATTServiceRegisterReq;
///@endcond

/**
 * @brief  blueAPI_EventGATTServiceRegisterRsp command data.
 *
 *  This command is used by the BT Stack to respond to a GATTServiceRegisterReq initiated from the APP. . 
 */
typedef struct
{
    void *         serviceHandle;     /**<  @brief Handle supplied by BT Stack that is used in all subsequent service related transactions. */
    TBlueAPI_Cause cause;             /**<  @brief Indicates the result of the transaction.  */
    uint16_t       subCause;          /**<  @brief Cause from protocol layers.  */ 
} TBlueAPI_GATTServiceRegisterRsp;
typedef TBlueAPI_GATTServiceRegisterRsp * PBlueAPI_GATTServiceRegisterRsp;

///@cond
typedef struct
{
    void *         serviceHandle;
    void *         requestHandle;
    uint16_t       attribIndex;
    uint16_t       attribLength;
    uint16_t       gap;
    uint8_t        data[1];
} TBlueAPI_GATTAttributeUpdateReq;
typedef TBlueAPI_GATTAttributeUpdateReq * PBlueAPI_GATTAttributeUpdateReq;
///@endcond

/**
 * @brief  blueAPI_EventGATTAttributeUpdateRsp command data.
 *
 *  This command is used by the BT Stack to respond to a GATTAttributeUpdateReq initiated from the APP. 
 */
typedef struct
{
    void *         serviceHandle;  /**<  @brief Handle of service that contains the attribute to update.  */
    void *         requestHandle;  /**<  @brief Request handle as defined in attribute update request.  */
    uint16_t       attribIndex;    /**<  @brief Index of attribute in service descriptor array.  */
    TBlueAPI_Cause cause;          /**<  @brief Indicates the result of the transaction.  */
    uint16_t       subCause;       /**<  @brief More detailed result information from lower protocol layers.  */
    uint16_t       count;          /**<  @brief nbr. of elements in list (<> 0 only for some cause values .).  */
    uint16_t       gap;            /**<  @brief Offset of list in list[].  */
    uint8_t        list[1];        /**<  @brief <BD,type>, depending on cause value.  */
} TBlueAPI_GATTAttributeUpdateRsp;
typedef TBlueAPI_GATTAttributeUpdateRsp * PBlueAPI_GATTAttributeUpdateRsp;

/**  @brief list element in TBlueAPI_GATTAttributeUpdateRsp. */
typedef struct
{
    uint8_t remote_BD[BLUE_API_BD_SIZE];
    uint8_t remote_BD_Type;
} TBlueAPI_GATTAttributeUpdateListElement, * PBlueAPI_GATTAttributeUpdateListElement;

/**
 * @brief  blueAPI_EventGATTAttributeUpdateStatusInd command data.
 *
 *  This command be used by the BT Stack to report the status of an indication (ATT Handle Value Indication)
 *  triggered by a GATTAttributeUpdateReq from the APP.
 */
typedef struct
{
    void *         serviceHandle;         /**<  @brief Handle of service that contains the attribute to update. */      
    void *         requestHandle;         /**<  @brief Request handle as defined in attribute update request. */     
    uint16_t       attribIndex;           /**<  @brief Index of attribute in service descriptor array. */     
    TBlueAPI_Cause cause;                 /**<  @brief Indicates the result of the transaction. */     
    uint16_t       subCause;              /**<  @brief More detailed result information from lower protocol. */      
    uint8_t        remote_BD[BLUE_API_BD_SIZE]; /**<  @brief Bluetooth device address of remote device. */
    TBlueAPI_RemoteBDType remote_BD_Type;       /**<  @brief Remote device address type */      
} TBlueAPI_GATTAttributeUpdateStatusInd;
typedef TBlueAPI_GATTAttributeUpdateStatusInd * PBlueAPI_GATTAttributeUpdateStatusInd;
///@cond
typedef struct
{
    void *         serviceHandle;
    void *         requestHandle;
    uint16_t       attribIndex;
} TBlueAPI_GATTAttributeUpdateStatusConf;
typedef TBlueAPI_GATTAttributeUpdateStatusConf * PBlueAPI_GATTAttributeUpdateStatusConf;
///@endcond

/**
 * @brief  blueAPI_EventGATTAttributeReadInd command data.
 *
 *  The BT Stack uses this command to indicate that the APP should supply the attribute value of an attribute 
 *  which has set bit ATTRIB_FLAG_VALUE_APPL to 1 in the Flags field. 
 */
typedef struct
{
    uint16_t       local_MDL_ID;     /**<  @brief Local link ID for this link. */  
    void *         serviceHandle;    /**<  @brief Handle of service that contains the attribute to read. */  
    uint16_t       attribIndex;      /**<  @brief Index of attribute in service descriptor array. */  
    uint16_t       readOffset;       /**<  @brief Read offset in attribute. */  
} TBlueAPI_GATTAttributeReadInd;
typedef TBlueAPI_GATTAttributeReadInd * PBlueAPI_GATTAttributeReadInd;
///@cond
typedef struct
{
    void *         serviceHandle;
    uint16_t       local_MDL_ID;
    TBlueAPI_Cause cause;//TBlueAPI_Cause
    uint16_t       subCause;
    uint16_t       attribIndex;
    uint16_t       attribLength;
    uint16_t       gap;
    uint8_t        data[1];
} TBlueAPI_GATTAttributeReadConf;
typedef TBlueAPI_GATTAttributeReadConf * PBlueAPI_GATTAttributeReadConf;
///@endcond
/**
 * @brief  blueAPI_EventGATTAttributeWriteInd or blueAPI_EventGATTAttributePrepareWriteInd command data.
 *
 *  (triggered by ATT_WRITE_REQUEST or ATT_PREPARE_REQUEST)
 *
 *  blueAPI_EventGATTAttributeWriteInd: 
 *              The BT Stack uses this command to indicate that the APP should write data to the attribute value of an attribute 
 *              which has set bit ATTRIB_FLAG_VALUE_APPL to 1 in the Flags field.
 *
 *  blueAPI_EventGATTAttributePrepareWriteInd: 
 *              The BT Stack uses this command to indicate that the APP should prepare write data to the attribute value of an attribute 
 *              which has set bit ATTRIB_FLAG_VALUE_APPL to 1 or 0 in the Flags field.
 *  
 */
typedef struct
{
    uint16_t       local_MDL_ID;        /**<  @brief Local link ID for this link. */
    void *         serviceHandle;       /**<  @brief Handle of service that contains the attribute to write. */
    uint16_t       attribIndex;         /**<  @brief Index of attribute in service descriptor array. */
    uint16_t       attribLength;        /**<  @brief Attribute value/data length. */
    uint16_t       handle;              /**<  @brief Attribute handle. */
    uint16_t       writeOffset;         /**<  @brief Write offset in attribute  */
    uint16_t       gap;                 /**<  @brief Offset of attrib. value in data[]  */
    uint8_t        data[1];             /**<  @brief Attribute value/data to write. */
} TBlueAPI_GATTAttributeWriteInd;
typedef TBlueAPI_GATTAttributeWriteInd * PBlueAPI_GATTAttributeWriteInd;
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    void *         serviceHandle;
    TBlueAPI_Cause cause;
    uint16_t       subCause;
    uint16_t       attribIndex;
} TBlueAPI_GATTAttributeWriteConf;
typedef TBlueAPI_GATTAttributeWriteConf * PBlueAPI_GATTAttributeWriteConf;
///@endcond
/**
 * @brief  blueAPI_EventGATTAttributeWriteCommandInfo command data.
 *
 *  The BT Stack uses this command to indicate that the APP should write data to the attribute value of an attribute 
 *  which has set bit ATTRIB_FLAG_VALUE_APPL to 1 in the Flags field 
 *  triggered by ATT_WRITE_COMMAND
 */
typedef TBlueAPI_GATTAttributeWriteInd  TBlueAPI_GATTAttributeWriteCommandInfo;
typedef TBlueAPI_GATTAttributeWriteCommandInfo * PBlueAPI_GATTAttributeWriteCommandInfo;
///@cond
typedef struct
{
    void *         serviceHandle;
    uint16_t       local_MDL_ID;
    TBlueAPI_Cause cause;//TBlueAPI_Cause
    uint16_t       subCause;
    uint16_t       attribIndex;
    uint16_t       attribLength;
    uint16_t       gap;
    uint8_t        data[1];
} TBlueAPI_GATTAttributePrepareWriteConf;
typedef TBlueAPI_GATTAttributePrepareWriteConf * PBlueAPI_GATTAttributePrepareWriteConf;
///@endcond
/**
 * @brief  blueAPI_EventGATTAttributeExecuteWriteInd command data.
 *
 *  The BT Stack uses this command to indicate that the APP should write or cancel write of all the prepared values currently held in the prepare queue from this client.
 *  (triggered by ATT_EXECUTE_WRITE_REQUEST)
 */
typedef struct
{
    uint16_t       local_MDL_ID;       /**<  @brief Local link ID for this link. */
    uint8_t        flags;              /**<  @brief flags:0x00-Cancel all prepared writes
                                                   0x01-Immediately write all pending prepared values. */
} TBlueAPI_GATTAttributeExecuteWriteInd;
typedef TBlueAPI_GATTAttributeExecuteWriteInd * PBlueAPI_GATTAttributeExecuteWriteInd;
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    TBlueAPI_Cause cause;
    uint16_t       subCause;
    uint16_t       handle;
} TBlueAPI_GATTAttributeExecuteWriteConf;
typedef TBlueAPI_GATTAttributeExecuteWriteConf * PBlueAPI_GATTAttributeExecuteWriteConf;
///@endcond

/**
 * @brief  blueAPI_EventGATTCCCDInfo command data.
 *
 * The BT Stack uses this command to inform the APP about the current value of one or more client characteristic confirmation descriptors
 * that have bit ATTRIB_FLAG_CCCD_APPL in the attribute definition set to 1.
 * This message is sent by the BT Stack when a connection is established or whenever the value of a CCCD changes during a connection.
 */
typedef struct
{
    uint16_t       local_MDL_ID;       /**<  @brief Local ink ID for this link.  */
    void *         serviceHandle;      /**<  @brief Handle of service that contains the CCCD attributes.  */
    uint16_t       count;              /**<  @brief nbr. of attribIndex/CCCD value pairs.  */
    uint16_t       gap;                /**<  @brief Offset of first pair in data[].  */
    uint8_t        data[1];            /**<  @brief List of CCCD attribute index / value pairs.  */
} TBlueAPI_GATTCCCDInfo;
typedef TBlueAPI_GATTCCCDInfo * PBlueAPI_GATTCCCDInfo;
/** @} End of GATT_Server */
/** @} End of message_command_data */

/*------------- GATT client application messages -------------*/

/** @addtogroup Specific_Data_Type
  * @{
  */
/** @brief This enum type describes discovery type. */
typedef enum
{
    blueAPI_GATTDiscoveryServices = 0x01,    /**<  Discover all primary services  */
    blueAPI_GATTDiscoveryServiceByUUID,      /**<  Discover service by UUID  */
    blueAPI_GATTDiscoveryCharacteristics,    /**<  Discover all characteristics  */
    blueAPI_GATTDiscoveryCharacDescriptors,  /**<  Discover all characteristic descriptors  */
    blueAPI_GATTDiscoveryRelationship        /**<  Discover relationship (included services)  */
} TBlueAPI_GATTDiscoveryType;
/** @} End of Specific_Data_Type */
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    TBlueAPI_GATTDiscoveryType  discoveryType;
    uint16_t       startHandle;
    uint16_t       endHandle;
    uint16_t       UUID16;
    uint8_t        UUID128[16];
} TBlueAPI_GATTDiscoveryReq;
typedef TBlueAPI_GATTDiscoveryReq * PBlueAPI_GATTDiscoveryReq;
///@endcond

/** @addtogroup message_command_data
  * @{
  */
/** @defgroup GATT_Client GATT Client
  * @brief GATT Client Command Data
  * @{
  */
/**
 * @brief  blueAPI_EventGATTDiscoveryRsp command data.
 *
 * The BT Stack uses this command to respond to a GATTDiscoveryReq received from the APP.
 * It is a local confirmation that the discovery procedure was initiated. 
 *  The discovery results are signaled in one or more GATTDiscoveryInd.
 */
typedef struct
{
    uint16_t       local_MDL_ID;                /**<  @brief Local link ID. */
    TBlueAPI_GATTDiscoveryType  discoveryType;  /**<  @brief Type of discovery  */
    TBlueAPI_Cause       cause;                 /**<  @brief Indicates the result of the transaction. */
    uint16_t       subCause;                    /**<  @brief More detailed result information for lower protocol layers. */
} TBlueAPI_GATTDiscoveryRsp;
typedef TBlueAPI_GATTDiscoveryRsp * PBlueAPI_GATTDiscoveryRsp;
/** @defgroup TBlueAPI_GATTDiscovery TBlueAPI_GATTDiscoveryInd
  * @brief TBlueAPI_GATTDiscoveryInd command data
  * @{
  */
/**
 *  @brief TBlueAPI_GATTDiscoveryInd list elements, some discovery type results
 *  share the same generic list element format:
 */
typedef struct
{
    uint16_t   attHandle;
    uint16_t   endGroupHandle;
    uint16_t   UUID16;                
} TBlueAPI_GATTGenericElement16, * PBlueAPI_GATTGenericElement16;

typedef struct
{
    uint16_t   attHandle;
    uint16_t   endGroupHandle;
    uint8_t    UUID128[16];           
} TBlueAPI_GATTGenericElement128, * PTBlueAPI_GATTGenericElement128;

/**  @brief discoveryType = blueAPI_GATTDiscoveryServices */
typedef TBlueAPI_GATTGenericElement16        TBlueAPI_GATTServicesElement16;
typedef TBlueAPI_GATTServicesElement16  * PBlueAPI_GATTServicesElement16;
/**  @brief discoveryType = blueAPI_GATTDiscoveryServices */
typedef TBlueAPI_GATTGenericElement128       TBlueAPI_GATTServicesElement128;
typedef TBlueAPI_GATTServicesElement128 * PBlueAPI_GATTServicesElement128;

/**  @brief discoveryType = blueAPI_GATTDiscoveryServiceByUUID */
typedef struct
{
    uint16_t   attHandle;
    uint16_t   endGroupHandle;
} TBlueAPI_GATTServiceByUUIDElement, * PBlueAPI_GATTServiceByUUIDElement;

/**  @brief discoveryType = blueAPI_GATTDiscoveryCharacteristics */
typedef struct
{
    uint16_t   declHandle;
    uint16_t   properties;            
    uint16_t   valueHandle;
    uint16_t   UUID16;                
} TBlueAPI_GATTCharacteristicsElement16, * PBlueAPI_GATTCharacteristicsElement16;

/**  @brief discoveryType = blueAPI_GATTDiscoveryCharacteristics */
typedef struct
{
    uint16_t   declHandle;
    uint16_t   properties;            
    uint16_t   valueHandle;
    uint8_t    UUID128[16];           
} TBlueAPI_GATTCharacteristicsElement128, * PBlueAPI_GATTCharacteristicsElement128;

/**  @brief discoveryType = blueAPI_GATTDiscoveryCharacDescriptors */
typedef struct
{
    uint16_t   handle;
    uint16_t   UUID16;                
} TBlueAPI_GATTCharacDescriptorsElement16, * PBlueAPI_GATTCharacDescriptorsElement16;

/**  @brief discoveryType = blueAPI_GATTDiscoveryCharacDescriptors */
typedef struct
{
    uint16_t   handle;
    uint8_t    UUID128[16];           
} TBlueAPI_GATTCharacDescriptorsElement128, * PBlueAPI_GATTCharacDescriptorsElement128;

/**  @brief discoveryType = blueAPI_GATTDiscoveryRelationship */
typedef struct
{
    uint16_t   declHandle;
    uint16_t   attHandle;
    uint16_t   endGroupHandle;
    uint16_t   UUID16;               
} TBlueAPI_GATTRelationshipElement16, * PBlueAPI_GATTRelationshipElement16;

/**  @brief discoveryType = blueAPI_GATTDiscoveryRelationship */
typedef struct
{
    uint16_t   declHandle;
    uint16_t   attHandle;
    uint16_t   endGroupHandle;
    uint8_t    UUID128[16];           
} TBlueAPI_GATTRelationshipElement128, * PBlueAPI_GATTRelationshipElement128;

/** @brief list element overlay */
typedef union
{
    TBlueAPI_GATTGenericElement16             generic16[1];
    TBlueAPI_GATTGenericElement128            generic128[1];
    TBlueAPI_GATTServicesElement16            services16[1];
    TBlueAPI_GATTServicesElement128           services128[1];
    TBlueAPI_GATTServiceByUUIDElement         serviceByUUID[1];
    TBlueAPI_GATTCharacteristicsElement16     characteristics16[1];
    TBlueAPI_GATTCharacteristicsElement128    characteristics128[1];
    TBlueAPI_GATTCharacDescriptorsElement16   characDescriptors16[1];
    TBlueAPI_GATTCharacDescriptorsElement128  characDescriptors128[1];
    TBlueAPI_GATTRelationshipElement16        relationship16[1];
    TBlueAPI_GATTRelationshipElement128       relationship128[1];
} TBlueAPI_GATTDiscoveryListElement, * PBlueAPI_GATTDiscoveryListElement;

/**
 * @brief  blueAPI_EventGATTDiscoveryInd command data.
 *
 * The BT Stack uses this command to signal partial discovery results to the APP. 
 * Command GATTDiscoveryConf can be used to continue or terminate the search.
 */
typedef struct
{
    uint16_t       local_MDL_ID;               /**<  @brief Local link ID. */
    TBlueAPI_GATTDiscoveryType discoveryType;  /**<  @brief Type of discovery  */
    TBlueAPI_Cause       cause;                /**<  @brief Indicates the result of the transaction.*/
    uint16_t       subCause;                   /**<  @brief More detailed result information for lower protocol layers. */
    uint16_t       elementCount;               /**<  @brief Number of elements in array elementList. */
    uint16_t       elementLength;              /**<  @brief Length of a single element.
                                                                     (needed to distinguish between 16 or 128 bit UUIDs)*/
    uint16_t       gap;                        /**<  @brief Offset of first element in elementList[].  */
    uint8_t        list[1];                    /**<  @brief Discovery results, the format of an element depends on discoveryType. */
} TBlueAPI_GATTDiscoveryInd;
typedef TBlueAPI_GATTDiscoveryInd * PBlueAPI_GATTDiscoveryInd;
/** @} End of TBlueAPI_GATTDiscoveryInd */
/** @} End of GATT_Client */
/** @} End of message_command_data */
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    TBlueAPI_GATTDiscoveryType  discoveryType;
    uint16_t       startHandle;
    uint16_t       endHandle;
} TBlueAPI_GATTDiscoveryConf;
typedef TBlueAPI_GATTDiscoveryConf * PBlueAPI_GATTDiscoveryConf;
///@endcond

/** @addtogroup Specific_Data_Type
  * @{
  */
/** @brief This enum type describes read type */
typedef enum
{
    blueAPI_GATTReadTypeBasic = 0x01,  /**<  ATT "Read (Blob) Request"  */
    blueAPI_GATTReadTypeByUUID         /**<  ATT "Read By Type Request" */
} TBlueAPI_GATTReadType;
/** @} End of Specific_Data_Type */

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup GATT_Client
  * @{
  */
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    TBlueAPI_GATTReadType   readType;
    uint16_t       readOffset;
    uint16_t       startHandle;
    uint16_t       endHandle;
    uint16_t       UUID16;
    uint8_t        UUID128[16];
} TBlueAPI_GATTAttributeReadReq;
typedef TBlueAPI_GATTAttributeReadReq * PBlueAPI_GATTAttributeReadReq;
///@endcond

/**
 * @brief  blueAPI_EventGATTAttributeReadRsp command data.
 *
 * The BT Stack uses this command to signal the result of a GATTAttributeReadReq to the APP. 
 */
typedef struct
{
    uint16_t       local_MDL_ID;            /**<  @brief Local link ID. */
    TBlueAPI_GATTReadType       readType;   /**<  @brief Type of read  */     
    TBlueAPI_Cause       cause;             /**<  @brief Indicates the result of the transaction. */
    uint16_t       subCause;                /**<  @brief More detailed result information for lower protocol layers. */
    uint16_t       readOffset;              /**<  @brief readType = blueAPI_GATTReadTypeBasic:
                                                                     The first readOffset bytes of the attribute value have been skipped
                                                                     before reading started. */
    uint16_t       totalLength;             /**<  @brief Total number of bytes stored in the handlesData[] array. */
    uint16_t       attribLength;            /**<  @brief Length of a single attribute value stored in the handlesData[] array. */
    uint16_t       nbrOfHandles;            /**<  @brief Number of handles stored in the handlesData[] array. */
    uint16_t       gap;                     /**<  @brief Offset of first handle in handlesData[].  */
    uint8_t        handlesData[1];          /**<  @brief Array of attribute handles and values read. */
} TBlueAPI_GATTAttributeReadRsp;
typedef TBlueAPI_GATTAttributeReadRsp * PBlueAPI_GATTAttributeReadRsp;

///@cond
typedef struct
{
    uint16_t       local_MDL_ID;        
    uint16_t       nbrOfHandles;
    uint16_t       pHandles[11];
} TBlueAPI_GATTAttributeReadMultipleReq;
typedef TBlueAPI_GATTAttributeReadMultipleReq * PBlueAPI_GATTAttributeReadMultipleReq;
///@endcond
/**
 * @brief  blueAPI_EventGATTAttributeReadMultipleRsp command data.
 *
 * The BT Stack uses this command to signal the result of a GATTAttributeReadMultipleReq to the APP. 
 */
typedef struct
{
    uint16_t       local_MDL_ID;     /**<  @brief Local link ID. */
    TBlueAPI_Cause cause;            /**<  @brief Indicates the result of the transaction. */
    uint16_t       subCause;         /**<  @brief More detailed result information for lower protocol layers. */
    uint16_t       totalLength;      /**<  @brief Total number of bytes stored in the SetOfValues[] array. */
    uint16_t       gap;              /**<  @brief Offset of first handle in SetOfValues[] */
    uint8_t        SetOfValues[1];   /**<  @brief Array of attribute values read. */
} TBlueAPI_GATTAttributeReadMultipleRsp;
typedef TBlueAPI_GATTAttributeReadMultipleRsp * PBlueAPI_GATTAttributeReadMultipleRsp;
/** @} End of GATT_Client */
/** @} End of message_command_data */

/** @addtogroup Specific_Data_Type
  * @{
  */
/** @brief This enum type describes write type */
typedef enum
{
    blueAPI_GATTWriteTypeRequest = 0x01,   /**<  ATT "Write Request"  */
    blueAPI_GATTWriteTypeCommand           /**<  ATT "Write Command"  */
} TBlueAPI_GATTWriteType;
/** @} End of Specific_Data_Type */
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    TBlueAPI_GATTWriteType        writeType;
    uint16_t       attribHandle;
    uint16_t       attribLength;
    uint16_t       writeOffset;
    uint16_t       gap;
    uint8_t        data[1];
} TBlueAPI_GATTAttributeWriteReq;
typedef TBlueAPI_GATTAttributeWriteReq * PBlueAPI_GATTAttributeWriteReq;
///@endcond

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup GATT_Client
  * @{
  */
/**
 * @brief  blueAPI_EventGATTAttributeWriteRsp command data.
 *
 * The BT Stack uses this command to signal the result of a GATTAttributeWriteReq to the APP.
 */
typedef struct
{
    uint16_t       local_MDL_ID;        /**<  @brief Local link ID.  */ 
    TBlueAPI_GATTWriteType  writeType;  /**<  @brief Type of write.  */          
    TBlueAPI_Cause cause;               /**<  @brief Indicates the result of the transaction.  */ 
    uint16_t       subCause;            /**<  @brief More detailed result information from lower protocol layers. */ 
    uint16_t       attribLength;        /**<  @brief attrib. value length.  */ 
    uint16_t       gap;                 /**<  @brief Offset of attrib. value in data[].  */
    uint8_t        data[1];             /**<  @brief attrib. value in data[].  */
} TBlueAPI_GATTAttributeWriteRsp;
typedef TBlueAPI_GATTAttributeWriteRsp * PBlueAPI_GATTAttributeWriteRsp;

/**
 * @brief  blueAPI_EventGATTAttributePrepareWriteRsp command data.
 *
 * The BT Stack uses this command to the result of a GATTAttributePrepareWriteReq to the APP.
 */
typedef struct
{
    uint16_t       local_MDL_ID;         /**<  @brief Local link ID.  */ 
    TBlueAPI_Cause cause;                /**<  @brief Indicates the result of the transaction.  */ 
    uint16_t       subCause;             /**<  @brief More detailed result information from lower protocol layers. */ 
    uint16_t       writeOffset;          /**<  @brief The offset of the first octet to be written.  */
    uint16_t       attribLength;         /**<  @brief attrib. value length.  */ 
    uint16_t       gap;                  /**<  @brief Offset of attrib. value in data[].  */
    uint8_t        data[1];              /**<  @brief attrib. value in data[].  */
} TBlueAPI_GATTAttributePrepareWriteRsp;
typedef TBlueAPI_GATTAttributePrepareWriteRsp * PBlueAPI_GATTAttributePrepareWriteRsp;
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    uint8_t        flags;
} TBlueAPI_GATTAttributeExecuteWriteReq;
typedef TBlueAPI_GATTAttributeExecuteWriteReq * PBlueAPI_GATTAttributeExecuteWriteReq;
///@endcond

/**
 * @brief  blueAPI_EventGATTAttributeExecuteWriteRsp command data.
 *
 * The BT Stack uses this command to signal the result of a GATTAttributeExecuteWriteReq to the APP. 
 */
typedef struct
{
    uint16_t       local_MDL_ID;      /**<  @brief Local link ID.  */   
    TBlueAPI_Cause        cause;      /**<  @brief Indicates the result of the transaction.  */           
    uint16_t       subCause;          /**<  @brief More detailed result information from lower protocol layers. */   
} TBlueAPI_GATTAttributeExecuteWriteRsp;
typedef TBlueAPI_GATTAttributeExecuteWriteRsp * PBlueAPI_GATTAttributeExecuteWriteRsp;

/**
 * @brief  blueAPI_EventGATTAttributeInd or blueAPI_EventGATTAttributeNotificationInfo command data.
 *
 * blueAPI_EventGATTAttributeInd: This command is used by the BT Stack to signal to APP that the remote GATT server has sent a "Characteristic Value Indication". 
 * blueAPI_EventGATTAttributeNotificationInfo: This command is used by the BT Stack to signal to APP that the remote GATT server has sent a "Characteristic Value Notification".
 */
typedef struct
{
    uint16_t       local_MDL_ID;      /**<  @brief Local link ID.  */   
    uint16_t       attribHandle;      /**<  @brief Attribute handle.  */   
    uint16_t       attribLength;      /**<  @brief Length of attribute value.  */   
    uint16_t       gap;               /**<  @brief Offset of attrib. value in data[].  */   
    uint8_t        data[1];           /**<  @brief Attribute value.  */   
} TBlueAPI_GATTAttributeInd;
typedef TBlueAPI_GATTAttributeInd * PBlueAPI_GATTAttributeInd;
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
} TBlueAPI_GATTAttributeConf;
typedef TBlueAPI_GATTAttributeConf * PBlueAPI_GATTAttributeConf;
///@endcond

typedef TBlueAPI_GATTAttributeInd  TBlueAPI_GATTAttributeNotificationInfo;
typedef TBlueAPI_GATTAttributeNotificationInfo * PBlueAPI_GATTAttributeNotificationInfo;
/** @} End of GATT_Client */

///@cond
typedef enum
{
    blueAPI_GATTSecurityRequireMITM = 0x0001,
    blueAPI_GATTSecurityForcePairing = 0x0010,
} TBlueAPI_GATTSecurityRequirement;
///@endcond

/** @addtogroup Security_Management
  * @{
  */
///@cond
typedef struct
{
    uint16_t       local_MDL_ID;
    uint16_t       requirements;
    uint8_t        minKeySize;
} TBlueAPI_GATTSecurityReq;
typedef TBlueAPI_GATTSecurityReq * PBlueAPI_GATTSecurityReq;
///@endcond

/**
 * @brief  blueAPI_EventGATTSecurityRsp command data.
 *
 *  This command is used by the BT Stack to respond to a GATTSecurityRsp of the APP. 
 */
typedef struct
{
    uint16_t       local_MDL_ID;       /**<  @brief Local link ID for this link.  */
    TBlueAPI_KeyType        keyType;            /**<  @brief Key type of generated link key.  */
    uint8_t        keySize;            /**<  @brief Length of generated link key (range: 7 - 16).  */
    TBlueAPI_Cause        cause;              /**<  @brief Indicates the result of the requested operation.  */
} TBlueAPI_GATTSecurityRsp;
typedef TBlueAPI_GATTSecurityRsp * PBlueAPI_GATTSecurityRsp;
/** @} End of Security_Management */
/** @} End of message_command_data */

/** @addtogroup Specific_Data_Type
  * @{
  */
/** This enum type describes gatt store opertion. */
typedef enum
{
    blueAPI_GATTStoreOpGetCCCBits,             /**<  Get CCC Bits for one <bd,bdtype>.  */
    blueAPI_GATTStoreOpSetCCCBits,             /**<  Set CCC Bits for one <bd,bdtype>.  */
    blueAPI_GATTStoreOpGetAllCCCBits,          /**<  Get CCC Bits for all peers.  */
    blueAPI_GATTStoreOpDeleteAllCCCBits        /**<  Delete all CCC Bits for all peers.  */
} TBlueAPI_GATTStoreOpCode;
/** @} End of Specific_Data_Type */

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup GATT_Server
  * @{
  */
/**
 * @brief  blueAPI_EventGATTServerStoreInd command data.
 *
 *  The BT Stack uses this command to access an external data store for persistent GATT data.
 *  This message will only be used by the BT Stack if the BT Stack is configured to store bonding results externally. 
 *
 */
typedef struct
{
    TBlueAPI_GATTStoreOpCode       opCode;                      /**<  @brief Requested operation.  */
    TBlueAPI_RemoteBDType       remote_BD_Type;              /**<  @brief Remote device address type */
    uint8_t       remote_BD[BLUE_API_BD_SIZE]; /**<  @brief Bluetooth device address of remote device. */
    uint16_t      restartHandle;               /**<  @brief Used to request multiple data chunks. */
    uint8_t       dataLength;                  /**<  @brief Length of supplied data. */
    uint8_t       data[32];                    /**<  @brief Up to 32 bytes of data. */
} TBlueAPI_GATTServerStoreInd;
typedef TBlueAPI_GATTServerStoreInd * PBlueAPI_GATTServerStoreInd;
/** @} End of GATT_Server */
///@cond
typedef struct
{
    TBlueAPI_GATTStoreOpCode      opCode;//TBlueAPI_GATTStoreOpCode
    uint8_t                       remote_BD[BLUE_API_BD_SIZE];
    TBlueAPI_RemoteBDType         remote_BD_Type;//TBlueAPI_RemoteBDType
    uint16_t                      restartHandle;
    uint8_t                       dataLength;
    uint8_t                       data[32];
    TBlueAPI_Cause                cause;
} TBlueAPI_GATTServerStoreConf;
typedef TBlueAPI_GATTServerStoreConf * PBlueAPI_GATTServerStoreConf;
///@endcond

/*------------- other GATT messages -------------*/


/** @defgroup GATT_Miscellaneous GATT Miscellaneous
  * @brief GATT Miscellaneous Command Data
  * @{
  */ 
/**
 * @brief  blueAPI_EventGATTMtuSizeInfo command data.
 *
 *  This command is sent by the BT Stack if on an Bluetooth Low Energy (LE) link an MTU size has been negotiated
 */
typedef struct
{
    uint16_t       local_MDL_ID;   /**<  @brief Local link ID.  */
    uint16_t       mtuSize;        /**<  @brief MTU size used for this link.  */
} TBlueAPI_GATTMtuSizeInfo;
typedef TBlueAPI_GATTMtuSizeInfo * PBlueAPI_GATTMtuSizeInfo;
/** @} End of GATT_Miscellaneous */
/** @} End of message_command_data */

/** @addtogroup Specific_Data_Type
  * @{
  */
/** This enum type describes adv mode for blueAPI_LEAdvertiseReq. */
typedef enum
{
    blueAPI_LEAdvModeDisabled = 0,      /**<  Advertising disabled. */
    blueAPI_LEAdvModeEnabled,           /**<  Advertising  enabled. */
    blueAPI_LEAdvModeDirectedHighDuty   /**<  Connectable high duty cycle directed advertising enabled. */
} TBlueAPI_LEAdvMode;

/** This enum type describes adv type for blueAPI_LEAdvertiseParameterSetReq. */
typedef enum
{
    blueAPI_LEAdvTypeUndirected = 0,       /**<  Connectable  undirected advertising. */
	blueAPI_LEAdvTypeDirectedHighDuty = 1, /**<  Connectable high duty cycle directed advertising. */ 
    blueAPI_LEAdvTypeScannable = 2,        /**<  Scanable undirected advertising. */
    blueAPI_LEAdvTypeNonConnectable,       /**<  Nonconnectable undirected advertising. */
    blueAPI_LEAdvTypeDirectedLowDuty       /**<  Connectable low duty cycle directed advertising. */
} TBlueAPI_LEAdvType;

/** This enum type describes adv type for blueAPI_EventLEScanInfo. */
typedef enum
{
    blueAPI_LEAdvEventTypeUndirected = 0,    /**<  Connectable undirected advertising. */
	blueAPI_LEAdvEventTypeDirected = 1,      /**<  Connectable directed advertising. */ 
    blueAPI_LEAdvEventTypeScannable = 2,     /**<  Scanable undirected advertising. */
    blueAPI_LEAdvEventTypeNonConnectable,    /**<  Nonconnectable undirected advertising. */
    blueAPI_LEAdvEventTypeScanResponse       /**<  Scan response. */
} TBlueAPI_LEAdvEventType;


/** This enum type uses to set filter. */
typedef enum
{
    blueAPI_LEFilterAny = 0,            /**<  Remote devices are not filtered. */
    blueAPI_LEFilterWhitelist           /**<  Remote devices are filtered against the whitelist. */
} TBlueAPI_LEFilterPolicy;

/** This enum type describes date type for blueAPI_LEAdvertiseDataSetReq. */
typedef enum
{
    blueAPI_LEDataTypeAdvertisingData = 0, /**<  Advertising data. */
    blueAPI_LEDataTypeScanResponseData     /**<  Scan response data. */
} TBlueAPI_LEDataType;

/**This enum type describes scan mode for API_LEScanReq . */
typedef enum
{
    blueAPI_LEScanDisabled = 0,            /**<  Scan disabled. */
    blueAPI_LEScanPassive,                 /**<  Passive scan enabled. */
    blueAPI_LEScanActive                   /**<  Active scan enabled. */
} TBlueAPI_LEScanMode;

/** This enum type describes whitelist opertion for blueAPI_LEModifyWhitelistReq. */
typedef enum
{
    blueAPI_LEWhitelistOpClear = 0,       /**<  Clear whitelist. */
    blueAPI_LEWhitelistOpAdd,             /**<  Add a device address to the whitelist. */
    blueAPI_LEWhitelistOpRemove           /**<  Remove a device address from the whitelist */
} TBlueAPI_LEWhitelistOp;
/** @} End of Specific_Data_Type */


///@cond
typedef struct
{
    TBlueAPI_LEAdvMode      advMode;
} TBlueAPI_LEAdvertiseReq;
typedef TBlueAPI_LEAdvertiseReq * PBlueAPI_LEAdvertiseReq;
///@endcond
/** @addtogroup message_command_data
  * @{
  */
/** @defgroup Bluetooth_LE_generic Bluetooth LE generic
  * @brief Bluetooth LE generic Command Data
  * @{
  */
/**
 * @brief  blueAPI_EventLEAdvertiseRsp command data.
 *
 * This command is used by the BT Stack to respond to a LEAdvertiseReq of the APP. 
 * If the APP issued a LEAdvertiseReq with advMode = blueAPI_LEAdvModeDirectedHighDuty
 * and the advertising failed to create a connection, a LEAdvertiseRsp with cause = blueAPI_CauseConnectionDisconnect is generated after the high duty cycle direct advertising timeout.
 */
typedef struct
{
    TBlueAPI_LEAdvMode     advMode;   /**<  @brief Advertising mode */
    TBlueAPI_Cause      cause;     /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_LEAdvertiseRsp;
typedef TBlueAPI_LEAdvertiseRsp * PBlueAPI_LEAdvertiseRsp;

///@cond
typedef struct
{
    uint8_t      advType;//TBlueAPI_LEAdvType
    TBlueAPI_LEFilterPolicy filterScanReq;
    TBlueAPI_LEFilterPolicy filterConnectReq;
    uint16_t                minAdvInterval;
    uint16_t                maxAdvInterval;
    uint8_t                 remote_BD[BLUE_API_BD_SIZE];
    uint8_t                 remote_BD_type;//TBlueAPI_RemoteBDType
    uint8_t                 local_BD_type;
} TBlueAPI_LEAdvertiseParameterSetReq;
typedef TBlueAPI_LEAdvertiseParameterSetReq * PBlueAPI_LEAdvertiseParameterSetReq;
///@endcond

/**
 * @brief  blueAPI_EventLEAdvertiseParameterSetRsp command data.
 *
 * This command is used by the BT Stack to respond to an LEAdvertiseParameterSetReq of the APP. 
 */
typedef struct
{
    TBlueAPI_Cause          cause;   /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_LEAdvertiseParameterSetRsp;
typedef TBlueAPI_LEAdvertiseParameterSetRsp * PBlueAPI_LEAdvertiseParameterSetRsp;

///@cond
typedef struct
{
    uint8_t     dataType;//TBlueAPI_LEDataType
    uint8_t     dataLength;
    uint8_t     *pDataBuffer;
} TBlueAPI_LEAdvertiseDataSetReq;
typedef TBlueAPI_LEAdvertiseDataSetReq * PBlueAPI_LEAdvertiseDataSetReq;
///@endcond

/**
 * @brief  blueAPI_EventLEAdvertiseDataSetRsp command data.
 *
 * This command is used by the BT Stack to respond to an LEAdvertiseDataSetReq of the APP. 
 */
typedef struct
{
    TBlueAPI_LEDataType     dataType;   /**< @brief Type of data to be set */
    TBlueAPI_Cause          cause;      /**< @brief Indicates the result of the transaction. */
} TBlueAPI_LEAdvertiseDataSetRsp;
typedef TBlueAPI_LEAdvertiseDataSetRsp * PBlueAPI_LEAdvertiseDataSetRsp;

///@cond
typedef struct
{
    TBlueAPI_LEScanMode     scanMode;
    uint16_t                scanInterval;
    uint16_t                scanWindow;
    TBlueAPI_LEFilterPolicy filterPolicy;
	TBlueAPI_LocalBDType     local_BD_Type;
    uint8_t                 filterDuplicates;
} TBlueAPI_LEScanReq;
typedef TBlueAPI_LEScanReq * PBlueAPI_LEScanReq;
///@endcond

/**
 * @brief  blueAPI_EventLEScanRsp command data.
 *
 * This command is used by the BT Stack to respond to an LEScanReq of the APP. 
 */
typedef struct
{
    TBlueAPI_Cause          cause; /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_LEScanRsp;
typedef TBlueAPI_LEScanRsp * PBlueAPI_LEScanRsp;

/**
 * @brief  blueAPI_EventLEScanInfo command data.
 *
 *  This command is used by the BT Stack to inform the APP of a received advertising event
 *  during a passive scan or a scan response to an active scan.
 */
typedef struct
{
    uint8_t      remote_BD[BLUE_API_BD_SIZE];  /**< @brief Bluetooth device address of remote device found*/
    TBlueAPI_RemoteBDType      remote_BD_type;  /**< @brief Remote address type*/
    TBlueAPI_LEAdvEventType      advType;      /**< @brief Advertising event type. */
    int8_t       rssi;                         /**< @brief Remote signal strength indication */
    uint8_t      dataLength;                   /**< @brief Value range: 0x00 - 0x1F */
    uint8_t      data[31];                     /**< @brief Advertising Data / Scan Response Data */
} TBlueAPI_LEScanInfo;
typedef TBlueAPI_LEScanInfo * PBlueAPI_LEScanInfo;

///@cond
typedef struct
{
    TBlueAPI_LEWhitelistOp  operation;
    uint8_t                 remote_BD[BLUE_API_BD_SIZE];
    TBlueAPI_RemoteBDType   remote_BD_type;
} TBlueAPI_LEModifyWhitelistReq;
typedef TBlueAPI_LEModifyWhitelistReq * PBlueAPI_LEModifyWhitelistReq;
///@endcond

/**
 * @brief  blueAPI_EventLEModifyWhitelistRsp command data.
 *
 * This command is used by the BT Stack to respond to a LEModifyWhitelistReq of the APP. 
 */
typedef struct
{
    TBlueAPI_LEWhitelistOp  operation; /**<  @brief Whitelist operation requested. */
    TBlueAPI_Cause          cause;     /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_LEModifyWhitelistRsp;
typedef TBlueAPI_LEModifyWhitelistRsp * PBlueAPI_LEModifyWhitelistRsp;

///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                connIntervalMin;
    uint16_t                connIntervalMax;
    uint16_t                connLatency;
    uint16_t                supervisionTimeout;
} TBlueAPI_LEConnectionUpdateReq;
typedef TBlueAPI_LEConnectionUpdateReq * PBlueAPI_LEConnectionUpdateReq;
///@endcond

/**
 * @brief  blueAPI_EventLEConnectionUpdateRsp command data.
 *
 * This command is used by the BT Stack to respond to an LEConnectionUpdateRsp of the APP. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;/**<  @brief Local link ID.  */
    TBlueAPI_Cause          cause;    /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_LEConnectionUpdateRsp;
typedef TBlueAPI_LEConnectionUpdateRsp * PBlueAPI_LEConnectionUpdateRsp;

/**
 * @brief  blueAPI_EventLEConnectionUpdateInd command data.
 *
 * This command is used by the BT Stack to indicate that a BLE slave initiated a Connection Parameter Update on a link. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;      /**<  @brief Local link ID. */
    uint16_t                connIntervalMin;   /**<  @brief Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step) */
    uint16_t                connIntervalMax;   /**<  @brief Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step) */
    uint16_t                connLatency;       /**<  @brief Value range: 0x0000 - 0x01F3 */
    uint16_t                supervisionTimeout;/**<  @brief Value range: 0x000A - 0x0C80 (100ms - 32000ms, 10ms/step) */
} TBlueAPI_LEConnectionUpdateInd;
typedef TBlueAPI_LEConnectionUpdateInd * PBlueAPI_LEConnectionUpdateInd;

///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    TBlueAPI_Cause                 cause;
} TBlueAPI_LEConnectionUpdateConf;
typedef TBlueAPI_LEConnectionUpdateConf * PBlueAPI_LEConnectionUpdateConf;
///@endcond

/**
 * @brief  blueAPI_EventLEConnectionParameterInfo command data.
 *
 * This command is used by the BT Stack to inform the APP about updated Connection parameters. 
 */
typedef struct
{

    uint16_t                local_MDL_ID; /**<  @brief Local link ID.  */
    uint16_t                connInterval; /**<  @brief The connection event interval.  */
    uint16_t                connLatency;  /**<  @brief Slave latency for the connection in number of connection events.  */
    uint16_t                supervisionTimeout;/**<  @brief Supervision timeout for the LE Link. */
} TBlueAPI_LEConnectionParameterInfo;
typedef TBlueAPI_LEConnectionParameterInfo * PBlueAPI_LEConnectionParameterInfo;
/** @} End of Bluetooth_LE_generic */
/** @} End of message_command_data */
///@cond
#define BLUEAPI_VENDOR_VOHOGP_MAX_EXT_LEN   24

typedef struct
{
    void* pServiceHandle;
    uint16_t local_MDL_ID;
    uint16_t attributeIndex;
    uint16_t length;
    uint16_t cid;
    bool bEnable;
    uint8_t ATT_Opcode;
    uint8_t Report_Id;
    uint8_t extLen;
    uint8_t* pExtData;

} TBlueAPI_VendorSetVoiceParaReq;
typedef TBlueAPI_VendorSetVoiceParaReq * PBlueAPI_VendorSetVoiceParaReq;

typedef struct
{
    uint16_t   local_MDL_ID;
    TBlueAPI_Cause     cause;
} TBlueAPI_VendorSetVoiceParaRsp;
typedef TBlueAPI_VendorSetVoiceParaRsp * PBlueAPI_VendorSetVoiceParaRsp;


typedef struct
{
    uint8_t  tx_power_index;
} TBlueAPI_SetBleTxPowerReq;
typedef TBlueAPI_SetBleTxPowerReq * PBlueAPI_SetBleTxPowerReq;
///@endcond

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup Bluetooth_LE_generic
  * @{
  */
/**
 * @brief  blueAPI_EventSetBleTxPowerRsp command data.
 *
 * This command is used by the BT Stack to respond to a SetBleTxPowerReq of the APP. 
 */
typedef struct
{
	uint8_t          tx_power_index; /**<  @brief The Tx power actually used. */
    TBlueAPI_Cause   cause;          /**<  @brief Indicates the result of the transaction. */
    uint16_t         subCause;       /**<  @brief More detailed result information from lower protocol layers. */
} TBlueAPI_SetBleTxPowerRsp;
typedef TBlueAPI_SetBleTxPowerRsp * PBlueAPI_SetBleTxPowerRsp;

///@cond
typedef struct
{
    uint8_t          random_BD[BLUE_API_BD_SIZE];
} TBlueAPI_SetRandomAddressReq;
typedef TBlueAPI_SetRandomAddressReq * PBlueAPI_SetRandomAddressReq;
///@endcond

/**
 * @brief  blueAPI_EventSetRandomAddressRsp command data.
 *
 * This command is used by the BT Stack to respond to an SetRandomAddressReq of the APP. 
 */
typedef struct
{
    TBlueAPI_Cause   cause;          /**<  @brief Indicates the result of the transaction. */
    uint16_t         subCause;       /**<  @brief More detailed result information from lower protocol layers. */ 
} TBlueAPI_SetRandomAddressRsp;
typedef TBlueAPI_SetRandomAddressRsp * PBlueAPI_SetRandomAddressRsp;
/** @} End of Bluetooth_LE_generic */

///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                le_psm;
    uint16_t                mtu;
    uint16_t                mps;
    uint16_t                initialCredits;
    uint16_t                creditsIncrease;
} TBlueAPI_CreateLEDataChannelReq;
typedef TBlueAPI_CreateLEDataChannelReq * PBlueAPI_CreateLEDataChannelReq;
///@endcond
 /** @defgroup BT41_Cmd LE credit based flow control mode
  * @brief LE credit based flow control mode Command Data
  * @{
  */
/**
 * @brief  blueAPI_EventCreateLEDataChannelRsp command data.
 *
 * This command is used by the BT Stack to respond to an CreateLEDataChannelReq of the APP. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;   /**<  @brief Local link ID.  */
    uint16_t                channel;        /**<  @brief Channel ID. Identifies the destination channel endpoint. */
    TBlueAPI_Cause          cause;          /**<  @brief Indicates the result of the transaction. */
    uint16_t                subCause;       /**<  @brief More detailed result information from lower protocol layers. */ 
} TBlueAPI_CreateLEDataChannelRsp;
typedef TBlueAPI_CreateLEDataChannelRsp * PBlueAPI_CreateLEDataChannelRsp;


/**
 * @brief  blueAPI_EventCreateLEDataChannelInd command data.
 *
 * This command is used by the BT Stack to indicate a creation of LE credit based connection to the APP.
 */
typedef struct
{
    uint16_t                local_MDL_ID; /**<  @brief Local link ID.  */
    uint16_t                channel;      /**<  @brief channel ID.  */
} TBlueAPI_CreateLEDataChannelInd;
typedef TBlueAPI_CreateLEDataChannelInd * PBlueAPI_CreateLEDataChannelInd;
///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
    uint16_t                mtu;
    uint16_t                mps;
    uint16_t                initialCredits;
    uint16_t                creditsIncrease;
    TBlueAPI_Cause          cause;
} TBlueAPI_CreateLEDataChannelConf;
typedef TBlueAPI_CreateLEDataChannelConf * PBlueAPI_CreateLEDataChannelConf;

typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
} TBlueAPI_DisconnectLEDataChannelReq;
typedef TBlueAPI_DisconnectLEDataChannelReq * PBlueAPI_DisconnectLEDataChannelReq;
///@endcond

/**
 * @brief  blueAPI_EventDisconnectLEDataChannelRsp command data.
 *
 * This command is used by the BT Stack to respond to an DisconnectLEDataChannelReq of the APP. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;  /**<  @brief Local link ID.  */
    uint16_t                channel;       /**<  @brief Channel ID.  */
    TBlueAPI_Cause          cause;         /**<  @brief IIndicates the result of the transaction. */
    uint16_t                subCause;      /**<  @brief More detailed result information from lower protocol layers. */ 
} TBlueAPI_DisconnectLEDataChannelRsp;
typedef TBlueAPI_DisconnectLEDataChannelRsp * PBlueAPI_DisconnectLEDataChannelRsp;

/**
 * @brief  blueAPI_EventDisconnectLEDataChannelInd command data.
 *
 * This command is used by the BT Stack to indicate a disconnection of an established LE credit based connection to the APP.
 */
typedef struct
{
    uint16_t                local_MDL_ID;  /**<  @brief Local link ID.  */
    uint16_t                channel;       /**<  @brief Channel ID to be disconnected.  */
    TBlueAPI_Cause          cause;         /**<  @brief Indicates the result of the transaction. */
    uint16_t                subCause;      /**<  @brief More detailed result information from lower protocol layers. */ 
} TBlueAPI_DisconnectLEDataChannelInd;
typedef TBlueAPI_DisconnectLEDataChannelInd * PBlueAPI_DisconnectLEDataChannelInd;
///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
    TBlueAPI_Cause          cause;
} TBlueAPI_DisconnectLEDataChannelConf;
typedef TBlueAPI_DisconnectLEDataChannelConf * PBlueAPI_DisconnectLEDataChannelConf;


typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
    uint16_t                credits;
} TBlueAPI_SendLEFlowControlCreditReq;
typedef TBlueAPI_SendLEFlowControlCreditReq * PBlueAPI_SendLEFlowControlCreditReq;
///@endcond
/**
 * @brief  blueAPI_EventSendLEFlowControlCreditRsp command data.
 *
 * This command is used by the BT Stack to respond to an SendLEFlowControlCreditReq of the APP. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;   /**<  @brief Local link ID.  */
	uint16_t                channel;        /**<  @brief Channel ID  */
    TBlueAPI_Cause          cause;          /**<  @brief Indicates the result of the transaction. */
    uint16_t                subCause;       /**<  @brief More detailed result information from lower protocol layers. */ 
} TBlueAPI_SendLEFlowControlCreditRsp;
typedef TBlueAPI_SendLEFlowControlCreditRsp * PBlueAPI_SendLEFlowControlCreditRsp;
///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
    uint16_t                valueLength;
    uint16_t                gap;
    uint8_t                 data[1];
} TBlueAPI_LEDataReq;
typedef TBlueAPI_LEDataReq * PBlueAPI_LEDataReq;
///@endcond
/**
 * @brief  blueAPI_EventLEDataRsp command data.
 *
 * This command is used by the BT Stack to respond to an LEDataReq of the APP. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;   /**<  @brief Local link ID.  */
    uint16_t                channel;        /**<  @brief Channel ID  */
    TBlueAPI_Cause          cause;          /**<  @brief Indicates the result of the transaction. */
    uint16_t                subCause;       /**<  @brief More detailed result information from lower protocol layers. */ 
} TBlueAPI_LEDataRsp;
typedef TBlueAPI_LEDataRsp * PBlueAPI_LEDataRsp;

/**
 * @brief  blueAPI_EventLEDataInd command data.
 *
 * This command is used by the BT Stack to indicate LE Data coming from the remote device.
 */
typedef struct
{
    uint16_t                local_MDL_ID;   /**<  @brief Local link ID.  */
    uint16_t                channel;        /**<  @brief Channel ID  */
    uint16_t                valueLength;    /**<  @brief Value length  */
    uint16_t                gap;            /**<  @brief Offset of value in data[].  */   
    uint8_t                 data[1];        /**<  @brief Value.  */  
} TBlueAPI_LEDataInd;
typedef TBlueAPI_LEDataInd * PBlueAPI_LEDataInd;
///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
    TBlueAPI_Cause          cause;
} TBlueAPI_LEDataConf;
typedef TBlueAPI_LEDataConf * PBlueAPI_LEDataConf;
///@endcond
/**
 * @brief  blueAPI_EventLEDataChannelParameterInfo command data.
 *
 * This command is used by the BT Stack to inform the APP about updated LE credit based Connection parameters. 
 */
typedef struct
{
    uint16_t                local_MDL_ID;  /**<  @brief Local link ID.  */
    uint16_t                channel;       /**<  @brief Channel id to be created  */
    uint16_t                remote_mtu;    /**<  @brief Remote Maximum Transmission Unit */
    uint16_t                remote_mps;     /**<  @brief Remote Maximum PDU Size */
    uint16_t                remote_initial_credits;   /**<  @brief Remote initial credits*/
	uint16_t                maxDsCredits;    /**<  @brief Max downstream credits */
} TBlueAPI_LEDataChannelParameterInfo;
typedef TBlueAPI_LEDataChannelParameterInfo * PBlueAPI_LEDataChannelParameterInfo;

/**
 * @brief  blueAPI_EventLEDataChannelCreditsAlertInfo command data.
 *
 * This command is used by the BT Stack to inform the APP about local credits is zero. Remote device can not send LE-frame.
 * please use blueAPI_SendLEFlowControlCreditReq to increase credits.
 */
typedef struct
{
    uint16_t                local_MDL_ID;   /**<  @brief Local link ID.  */
    uint16_t                channel;        /**<  @brief Channel ID  */
} TBlueAPI_LEDataChannelCreditsAlertInfo;
typedef TBlueAPI_LEDataChannelCreditsAlertInfo * PBlueAPI_LEDataChannelCreditsAlertInfo;
///@cond
typedef struct
{
    uint16_t                local_MDL_ID;
    uint16_t                channel;
} TBlueAPI_LEDataChannelDeleteInfo;
typedef TBlueAPI_LEDataChannelDeleteInfo * PBlueAPI_LEDataChannelDeleteInfo;
///@endcond
/** @} End of BT41_Cmd */
/** @} End of message_command_data */

/** @addtogroup Specific_Data_Type
  * @{
  */
/** This enum type describes sercurity mode for blueAPI_LEPsmSecuritySetReq. */
typedef enum
{
    blueAPI_SecNoSecurity,                      /**<  No security requirement  */
    blueAPI_SecUnauthenticatedEncryption,       /**<  MITM protection not required, encryption required  */
    blueAPI_SecAuthenticatedEncryption,         /**<  MITM protection required, encryption required  */
    blueAPI_SecUnauthenticatedDataSigning,      /**<  MITM protection not required, data signing required  */
    blueAPI_SecAuthenticatedDataSigning,        /**<  MITM protection required, data signing required  */
    blueAPI_SecAuthorization                    /**<  Authorization required  */
} TBlueAPI_LESecurityMode;
/** @} End of Specific_Data_Type */
///@cond
typedef struct
{
    uint16_t  le_psm;
    bool      active;
    TBlueAPI_LESecurityMode      secMode;
    uint8_t   keySize;
} TBlueAPI_LEPsmSecuritySetReq;
typedef TBlueAPI_LEPsmSecuritySetReq * PBlueAPI_LEPsmSecuritySetReq;
///@endcond

/** @addtogroup message_command_data
  * @{
  */
/** @addtogroup BT41_Cmd
  * @{
  */
/**
 * @brief  blueAPI_EventLEPsmSecuritySetRsp command data.
 *
 * This command is used by the BT Stack to respond to an LEPsmSecuritySetReq of the APP. 
 */
typedef struct
{
    TBlueAPI_Cause          cause;   /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_LEPsmSecuritySetRsp;
typedef TBlueAPI_LEPsmSecuritySetRsp * PBlueAPI_LEPsmSecuritySetRsp;
/** @} End of BT41_Cmd */

///@cond
typedef struct
{
    uint16_t    local_MDL_ID;
    uint16_t    txOctets;
    uint16_t    txTime;  
} TBlueAPI_SetDataLengthReq;
typedef TBlueAPI_SetDataLengthReq * PBlueAPI_SetDataLengthReq;
///@endcond

/** @addtogroup Bluetooth_LE_generic
  * @{
  */
/**
 * @brief  blueAPI_EventSetDataLengthRsp command data.
 *
 * This command is used by the BT Stack to respond to a SetDataLengthReq of the APP.
 */
typedef struct
{
    uint16_t    local_MDL_ID;   /**<  @brief Local link ID for this link. */
    TBlueAPI_Cause    cause;    /**<  @brief Indicates the result of the transaction. */
} TBlueAPI_SetDataLengthRsp;
typedef TBlueAPI_SetDataLengthRsp * PBlueAPI_SetDataLengthRsp;

/**
 * @brief  blueAPI_EventDataLengthChangeInfo command data.
 *
 * This command is used by the BT Stack to inform the APP about data length change information.
 */
typedef struct
{
    uint16_t    local_MDL_ID;   /**<  @brief Local link ID for this link. */
    uint16_t MaxTxOctets;       /**<  @brief The maximum number of payload octets in a Link Layer Data Channel PDU that the local Controller will send on this connection. */
    uint16_t MaxTxTime;         /**<  @brief The maximum time that the local Controller will take to send a Link Layer Data Channel PDU on this connection. */
    uint16_t MaxRxOctets;       /**<  @brief The maximum number of payload octets in a Link Layer Data Channel PDU that the local controller expects to receive on this connection. */
    uint16_t MaxRxTime;         /**<  @brief The maximum time that the local Controller expects to take to receive a Link Layer Data Channel PDU on this connection. */
} TBlueAPI_DataLengthChangeInfo;
typedef TBlueAPI_DataLengthChangeInfo * PBlueAPI_DataLengthChangeInfo;
/** @} End of Bluetooth_LE_generic */
/** @} End of message_command_data */

///@cond
typedef union
{
    TBlueAPI_ConnectMDLReq       ConnectMDLReq;
    TBlueAPI_CreateMDLConf       CreateMDLConf;
    TBlueAPI_DisconnectMDLReq    DisconnectMDLReq;
    TBlueAPI_DisconnectMDLConf   DisconnectMDLConf;




    TBlueAPI_RegisterReq         RegisterReq;
    TBlueAPI_ReleaseReq          ReleaseReq;

    TBlueAPI_DeviceConfigSetReq  DeviceConfigSetReq;

    TBlueAPI_GATTServiceRegisterReq         GATTServiceRegisterReq;
    TBlueAPI_GATTAttributeUpdateReq         GATTAttributeUpdateReq;
    TBlueAPI_GATTAttributeUpdateStatusConf  GATTAttributeUpdateStatusConf;
    TBlueAPI_GATTAttributeReadConf          GATTAttributeReadConf;
    TBlueAPI_GATTAttributeWriteConf         GATTAttributeWriteConf;
    TBlueAPI_GATTAttributePrepareWriteConf  GATTAttributePrepareWriteConf;
    TBlueAPI_GATTAttributeExecuteWriteConf  GATTAttributeExecuteWriteConf;

    TBlueAPI_GATTDiscoveryReq               GATTDiscoveryReq;
    TBlueAPI_GATTDiscoveryConf              GATTDiscoveryConf;
    TBlueAPI_GATTAttributeReadReq           GATTAttributeReadReq;
    TBlueAPI_GATTAttributeReadMultipleReq   GATTAttributeReadMultipleReq;
    TBlueAPI_GATTAttributeWriteReq          GATTAttributeWriteReq;
    TBlueAPI_GATTAttributeExecuteWriteReq   GATTAttributeExecuteWriteReq;

    TBlueAPI_GATTAttributeConf              GATTAttributeConf;

    TBlueAPI_GATTSecurityReq                GATTSecurityReq;

    TBlueAPI_GATTServerStoreConf            GATTServerStoreConf;


    TBlueAPI_PairableModeSetReq           PairableModeSetReq;


    TBlueAPI_UserPasskeyReqConf           UserPasskeyReqConf;
    TBlueAPI_UserPasskeyReqReplyReq       UserPasskeyReqReplyReq;

    TBlueAPI_RemoteOOBDataReqConf         RemoteOOBDataReqConf;

    TBlueAPI_AuthResultConf               AuthResultConf;
    TBlueAPI_AuthResultRequestConf        AuthResultRequestConf;

    TBlueAPI_LEAdvertiseReq               LEAdvertiseReq;
    TBlueAPI_LEAdvertiseParameterSetReq   LEAdvertiseParameterSetReq;
    TBlueAPI_LEAdvertiseDataSetReq        LEAdvertiseDataSetReq;
    TBlueAPI_LEScanReq                    LEScanReq;
    TBlueAPI_LEModifyWhitelistReq         LEModifyWhitelistReq;
    TBlueAPI_LEConnectionUpdateReq        LEConnectionUpdateReq;
    TBlueAPI_LEConnectionUpdateConf       LEConnectionUpdateConf;
    TBlueAPI_VendorSetVoiceParaReq        VendorSetVoiceParaReq;
	TBlueAPI_SetBleTxPowerReq             SetBleTxPowerReq;
	TBlueAPI_SetRandomAddressReq          SetRandomAddressReq;

    TBlueAPI_CreateLEDataChannelReq       CreateLEDataChannelReq;
    TBlueAPI_CreateLEDataChannelConf      CreateLEDataChannelConf;
    TBlueAPI_DisconnectLEDataChannelReq   DisconnectLEDataChannelReq;
    TBlueAPI_DisconnectLEDataChannelConf  DisconnectLEDataChannelConf;
    TBlueAPI_SendLEFlowControlCreditReq   SendLEFlowControlCreditReq;
    TBlueAPI_LEDataReq                    LEDataReq;
    TBlueAPI_LEDataConf                   LEDataConf;
    TBlueAPI_LEPsmSecuritySetReq          LEPsmSecuritySetReq;
    TBlueAPI_SetDataLengthReq           SetDataLengthReq;
    
} TBlueAPI_DsCommandData;
typedef TBlueAPI_DsCommandData * PBlueAPI_DsCommandData;
///@endcond

/** @addtogroup Upstream_message_structure
  * @{
  */
typedef union
{
    TBlueAPI_ConnectMDLRsp       ConnectMDLRsp;
    TBlueAPI_CreateMDLInd        CreateMDLInd;
    TBlueAPI_DeleteMDLInfo       DeleteMDLInfo;
    TBlueAPI_ConnectMDLInfo      ConnectMDLInfo;
    TBlueAPI_DisconnectMDLRsp    DisconnectMDLRsp;
    TBlueAPI_DisconnectMDLInd    DisconnectMDLInd;

    TBlueAPI_MCLStatusInfo       MCLStatusInfo;
    TBlueAPI_ACLStatusInfo       ACLStatusInfo;



    TBlueAPI_RegisterRsp         RegisterRsp;
    TBlueAPI_ReleaseRsp          ReleaseRsp;
    TBlueAPI_ActInfo             ActInfo;
    TBlueAPI_InternalEventInfo   InternalEventInfo;

    TBlueAPI_DeviceConfigSetRsp  DeviceConfigSetRsp;
	TBlueAPI_DeviceConfigAppearanceGetRsp        DeviceConfigAppearanceGetRsp;
	TBlueAPI_DeviceConfigPerPrefConnParamGetRsp  DeviceConfigPerPrefConnParamGetRsp;
	TBlueAPI_DeviceConfigDeviceNameGetRsp        DeviceConfigDeviceNameGetRsp;     


    TBlueAPI_GATTServiceRegisterRsp         GATTServiceRegisterRsp;
    TBlueAPI_GATTAttributeUpdateRsp         GATTAttributeUpdateRsp;
    TBlueAPI_GATTAttributeUpdateStatusInd   GATTAttributeUpdateStatusInd;
    TBlueAPI_GATTAttributeReadInd           GATTAttributeReadInd;
    TBlueAPI_GATTAttributeWriteInd          GATTAttributeWriteInd;
    TBlueAPI_GATTAttributeExecuteWriteInd   GATTAttributeExecuteWriteInd;
    TBlueAPI_GATTAttributeWriteCommandInfo  GATTAttributeWriteCommandInfo;

    TBlueAPI_GATTCCCDInfo                   GATTCCCDInfo;

    TBlueAPI_GATTDiscoveryRsp               GATTDiscoveryRsp;
    TBlueAPI_GATTDiscoveryInd               GATTDiscoveryInd;
    TBlueAPI_GATTAttributeReadRsp           GATTAttributeReadRsp;
    TBlueAPI_GATTAttributeReadMultipleRsp   GATTAttributeReadMultipleRsp;
    TBlueAPI_GATTAttributeWriteRsp          GATTAttributeWriteRsp;
    TBlueAPI_GATTAttributePrepareWriteRsp   GATTAttributePrepareWriteRsp;
    TBlueAPI_GATTAttributeExecuteWriteRsp   GATTAttributeExecuteWriteRsp;

    TBlueAPI_GATTAttributeInd               GATTAttributeInd;
    TBlueAPI_GATTAttributeNotificationInfo  GATTAttributeNotificationInfo;

    TBlueAPI_GATTSecurityRsp                GATTSecurityRsp;

    TBlueAPI_GATTServerStoreInd             GATTServerStoreInd;

    TBlueAPI_GATTMtuSizeInfo                GATTMtuSizeInfo;

    TBlueAPI_PairableModeSetRsp           PairableModeSetRsp;
    TBlueAPI_AuthResultRequestInd         AuthResultRequestInd;
    TBlueAPI_UserPasskeyReqInd            UserPasskeyReqInd;
    TBlueAPI_UserPasskeyReqReplyRsp       UserPasskeyReqReplyRsp;
    TBlueAPI_UserPasskeyNotificationInfo  UserPasskeyNotificationInfo;

    TBlueAPI_RemoteOOBDataReqInd          RemoteOOBDataReqInd;


    TBlueAPI_AuthResultInd                AuthResultInd;

    TBlueAPI_LEAdvertiseRsp               LEAdvertiseRsp;
    TBlueAPI_LEAdvertiseParameterSetRsp   LEAdvertiseParameterSetRsp;
    TBlueAPI_LEAdvertiseDataSetRsp        LEAdvertiseDataSetRsp;
    TBlueAPI_LEScanRsp                    LEScanRsp;
    TBlueAPI_LEScanInfo                   LEScanInfo;
    TBlueAPI_LEModifyWhitelistRsp         LEModifyWhitelistRsp;
    TBlueAPI_LEConnectionUpdateRsp        LEConnectionUpdateRsp;
    TBlueAPI_LEConnectionUpdateInd        LEConnectionUpdateInd;
    TBlueAPI_LEConnectionParameterInfo    LEConnectionParameterInfo;
    TBlueAPI_VendorSetVoiceParaRsp        VendorSetVoiceParaRsp;
    TBlueAPI_SetBleTxPowerRsp             SetBleTxPowerRsp;
    TBlueAPI_SetRandomAddressRsp		  SetRandomAddressRsp;

    TBlueAPI_CreateLEDataChannelRsp       CreateLEDataChannelRsp;
    TBlueAPI_CreateLEDataChannelInd       CreateLEDataChannelInd;
    TBlueAPI_DisconnectLEDataChannelRsp   DisconnectLEDataChannelRsp;
    TBlueAPI_DisconnectLEDataChannelInd   DisconnectLEDataChannelInd;
    TBlueAPI_SendLEFlowControlCreditRsp   SendLEFlowControlCreditRsp;
    TBlueAPI_LEDataRsp                    LEDataRsp;
    TBlueAPI_LEDataInd                    LEDataInd;
    TBlueAPI_LEDataChannelParameterInfo    LEDataChannelParameterInfo;
    TBlueAPI_LEDataChannelCreditsAlertInfo LEDataChannelCreditsAlertInfo;
    TBlueAPI_LEDataChannelDeleteInfo       LEDataChannelDeleteInfo;
    TBlueAPI_LEPsmSecuritySetRsp           LEPsmSecuritySetRsp;
    TBlueAPI_SetDataLengthRsp                SetDataLengthRsp;   
    TBlueAPI_DataLengthChangeInfo          DataLengthChangeInfo;

} TBlueAPI_UsCommandData;
typedef TBlueAPI_UsCommandData * PBlueAPI_UsCommandData;
/** @} End of Upstream_message_structure */

///@cond
typedef struct _TBlueAPI_DsMessage
{
    uint16_t     Command;
    uint16_t     Length;
    TBlueAPI_DsCommandData p;
} TBlueAPI_DsMessage;
typedef TBlueAPI_DsMessage * PBlueAPI_DsMessage;
///@endcond
/** @addtogroup Upstream_message_structure
  * @{
  */
/**
 * @brief  Upstream message structure.
 *
 * This structure is used to send messages to APP.
 */
typedef struct _TBlueAPI_UsMessage
{
    uint16_t     Command;   /**<  @brief Use TBlueAPI_Command @ref BlueAPI_Command_Definitions */
    uint16_t     Length;    /**<  @brief Message length */
    TBlueAPI_UsCommandData p;/**<  @brief Message command data */
} TBlueAPI_UsMessage;
typedef TBlueAPI_UsMessage * PBlueAPI_UsMessage;
/** @} End of Upstream_message_structure */

/** @addtogroup Upstream_callback
  * @{
  */
typedef void ( * PBlueAPI_CallBack)(PBlueAPI_UsMessage pMsg);/**<  @brief Function pointer Type used to handle upstream messages. 
                                                               * APP can use this function pointer to define a function and register this callback function by blueAPI_RegisterReq.
                                                               */
/** @} End of Upstream_callback */


/** @} End of RTK_BlueAPI */

#if defined (__cplusplus)
}
#endif

#endif  /* !defined(__BLUEAPI_TYPES_H) */
