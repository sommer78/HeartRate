/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap.h
  * @brief   This file contains all the constants and functions prototypes for GAP protocol. 
  * @details
  * @author  Ranhui
  * @date    2015-5-22
  * @version v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef GAP_H
#define GAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Add Includes here **/
#include "rtl_types.h"
#include "blueapi_types.h"
#include "bterrcod.h"

//#define ANCS_SUPPORT
#define BT_GAP_VENDOR_CMD_ADD

#ifdef BT_GAP_VENDOR_CMD_ADD
#define BT_GAP_pARAM_32K_EXT_LOCAL_SCA_SET
//#define BT_GAP_VENDOR_AES_API_SUPPORT
#endif

//#define BT_GAP_BOND_KEY_DISTRIBUTION
#define BT_GAP_LE_PRIVACY_RESOLVING
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
#define BT_GAP_VENDOR_AES_API_SUPPORT
#endif

#define HCI_READ_RSSI                                   0x1405
#define HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_OPCODE   0x2014
#define HCI_LE_TRANSMITTER_TEST_OPCODE                  0x201E
#define HCI_LE_TEST_END_OPCODE                          0x201F
#define HCI_VENDOR_ONOFF_SLAVE_LATENCY      0xFC7A

#ifdef BT_GAP_pARAM_32K_EXT_LOCAL_SCA_SET
#define HCI_VENDOR_LPS_SETTING                          0xFC6E
#endif
#define HCI_VENDOR_SET_ADV_FAST_DISABLE                 0xFC85
#define HCI_VENDOR_SET_RX_SENSITIVITY_REG		0xFC86
/** this MACRO used for internal debug. Normally closed. **/
//#define GAP_DEBUG_FLAG

/** @addtogroup RTK_GAP_MODULE
  * @{
  */

/** @defgroup GAP_COMMON_DEFINITIONS GAP Common Definitions
  * @{
  */ 

/** @defgroup GAP_Exported_Constants GAP Exported Constants
  * @{
  */

/** @defgroup GAP_Common_Constants GAP Common Constants
  * @{
  */
#define B_ADDR_LEN					6   //!< Default BT Public and Random Address Length
#define KEYLEN                      16  //!< Default key length
#define B_MAX_ADV_LEN               31  //!< BLE Maximum Advertising or Scan Response Packet Length, unit is byte.
#define GAP_DEVICE_NAME_LEN         (39+1)  //!< Max length of device name, if device name length exceeds it, it will be truncated.
#define ADV_CHANMAP_SIZE            5	//!<  Advertiser Channel Map Size.
#define GAP_PASSCODE_MAX            999999  //!< Maximum Pairing Passcode/Passkey value.  Range of a passkey can be 0 - 999,999.
/**
  * @}
  */

/** @defgroup GAP_BT_MSG_TYPE GAP BT Message Type Definitions
  * @{
  */
#define BT_MSG_TYPE_CONN_STATE_CHANGE       0x01   //!< Device state change message
#define BT_MSG_TYPE_BOND_STATE_CHANGE       0x02   //!< Bond state change message
#define BT_MSG_TYPE_BOND_PASSKEY_DISPLAY    0x03   //!< Inform app to show passkey
#define BT_MSG_TYPE_BOND_PASSKEY_INPUT      0x04   //!< Inform app to input passkey
#define BT_MSG_TYPE_BOND_OOB_INPUT          0x05   //!< Inform app to input OOB data
#define BT_MSG_TYPE_ENCRYPT_STATE_CHANGE    0x06   //!< Encrpt state change message
#define BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE 0x07   //!< Connection parameter update result message
#ifdef BT_GAP_VENDOR_CMD_ADD
#define BT_MSG_TYPE_PARAM_SET_RESULT        0x08   //!< Parameter set result message
#endif
/**
  * @}
  */

/** @defgroup GAP_PARAM GAP Parameters
  * @{
  */

/** @defgroup GAP_PARAM_ALL_ROLES All Roles
  * @{
  */
#define GAPPRRA_ROLE_STATE              0x200  //!< Device's current GAP role's state. Read/Write. Each role may have different states.
#define GAPPARA_PROFILE_ROLE            0x201  //!< Device's Profile Role. Read Only. Size is uint8_t.
#define GAPPARA_BD_ADDR                 0x202  //!< Local Device's Address. Read Only. Size is @ref B_ADDR_LEN. This item is read from the controller.
#define GAPPARA_BD_ADDR_TYPE            0x203  //!< Local Device's Address's Type. Read/Write. Size is uint8_t. @ref GAP_LOCAL_ADDR_TYPE_DEFINES
#define GAPPRRA_DEVICE_NAME             0x204  //!< Device's Name. Write Only. Name string length is @ref GAP_DEVICE_NAME_LEN.
#define GAPPRRA_APPEARANCE              0x205  //!< Device's Appearance. Read/Write. Size is uint16_t. Appearance value please refer to @ref GAP_APPEARANCE_VALUES.
#define GAPPARA_BLE_TX_POWER            0x206  //!< Device's Tx power. Read Only. Size is int8_t.
#define GAPPARA_BLE_PRIVACY_MODE        0x207  //!< Device's Privacy Mode. Read/Write. Size is uint8_t. Privacy Mode value please refer to @ref GAP_RPIVACY_DEFINES.

/**
  * @}
  */
  
/** @defgroup GAP_PARAM_PERIPH_BROAD Peripheral and Broadcaster
  * @{
  */
#define GAPPRRA_ADVERT_DATA             0x210  //!< Advertisement Data. Read/Write. Max size is uint8[B_MAX_ADV_LEN]. Default is "02:01:05", which means that it is a Limited Discoverable Advertisement.
#define GAPPRRA_SCAN_RSP_DATA           0x211  //!< Scan Response Data. Read/Write. Max size is uint8[B_MAX_ADV_LEN]. Defaults to all 0.
#define GAPPRRA_ADV_EVENT_TYPE      	0x212  //!< Advertisement Type. Read/Write. Size is uint8.  Default is GAP_ADTYPE_ADV_IND (@ref GAP_ADVERTISEMENT_TYPE_DEFINES).
#define GAPPRRA_ADV_DIRECT_ADDR_TYPE    0x213  //!< Direct Advertisement Address Type. Read/Write. Size is uint8. 
#define GAPPRRA_ADV_DIRECT_ADDR     	0x214  //!< Direct Advertisement Address. Read/Write. Size is uint8[B_ADDR_LEN]. Default is all 0.
#define GAPPRRA_ADV_CHANNEL_MAP     	0x215  //!< Which channels to advertise on. Read/Write Size is uint8. Default is GAP_ADVCHAN_ALL (@ref GAP_ADVCHAN_DEFINES)
#define GAPPRRA_ADV_FILTER_POLICY   	0x216  //!< Filter Policy. Ignored when directed advertising is used. Read/Write. Size is uint8. Default is GAP_FILTER_POLICY_ALL (@ref GAP_FILTER_POLICY_DEFINES).
#define GAPPRRA_ADV_INTERVAL_MIN     	0x217  //!< Minimum advertising interval for undirected and low duty cycle directed advertising. Value range: 0x0020 - 0x4000 (20ms - 10240ms 0.625ms/step),Read/Write Size is uint16_t.                                                                               
#define GAPPRRA_ADV_INTERVAL_MAX   		0x218  //!< Maximum advertising interval for undirected and low duty cycle directed  advertising. Value range: 0x0020 - 0x4000 (20ms - 10240ms 0.625ms/step)),Read/Write Size is uint16_t.         
#define GAPPRRA_ADV_ENABLE_DEFAULT   	0x219  //!< Advertising enable default. Read/Write. Size is uint8_t.
/**
  * @}
  */

/** @defgroup GAP_PARAM_CENTRAL_OBSERV Central and Observer
  * @{
  */
#define GAPPARA_SCANMODE                0x220 //!< Scan mode.Read/Write.Size is uint8. Default is GAP_SCAN_MODE_ACTIVE(@ref GAP_SCAN_MODE_VALUES)
#define GAPPARA_SCANINTERVAL            0x221 //!< Scan Interval.Read/Write. Size is uint16_t. Default is 0x10.
#define GAPPARA_SCANWINDOW              0x222 //!< Scan Window.Read/Write. Size is uint16_t. Default is 0x10.
#define GAPPARA_FILTERPOLICY            0x223 //!< Scan Filter Policy.Read/Write.Size is uint8_t. Default is GAP_SCAN_FILTER_ANY.(@ref GAP_SCAN_FILTER_POLICY_VALUES)
#define GAPPARA_FILTERDUPLICATES        0x224 //!< Scan Filter Duplicates.Read/Write.Size is uint8_t. Default is GAP_SCAN_FILTER_DUPLICATE_DISABLE(@ref GAP_SCAN_FILTER_DUPLICATE_VALUES)
#define GAPPRRA_SCAN_ENABLE_DEFAULT     0x225 //!< Scan Eable Default.Write only. Size is uint8_t.
/**
  * @}
  */

/** @defgroup GAP_PARAM_PERIPH_CENTRAL Peripheral and Central
  * @{
  */
#define GAPPRRA_MIN_CONN_INTERVAL   0x230  //!< Minimum Connection Interval to allow (n * 1.25ms).  Range: 7.5 msec to 4 seconds (0x0006 to 0x0C80). Read/Write. Size is uint16. Default is 7.5 milliseconds (0x0006).
#define GAPPRRA_MAX_CONN_INTERVAL   0x231  //!< Maximum Connection Interval to allow (n * 1.25ms).  Range: 7.5 msec to 4 seconds (0x0006 to 0x0C80). Read/Write. Size is uint16. Default is 4 seconds (0x0C80).
#define GAPPRRA_SLAVE_LATENCY       0x232  //!< Update Parameter Slave Latency. Range: 0 - 499. Read/Write. Size is uint16. Default is 0.
#define GAPPRRA_TIMEOUT_MULTIPLIER  0x233  //!< Update Parameter Timeout Multiplier (n * 10ms). Range: 100ms to 32 seconds (0x000a - 0x0c80). Read/Write. Size is uint16. Default is 1000.
#define GAPPRRA_CONN_BD_ADDR        0x234  //!< Address of connected device. Read only. Size is uint8[B_MAX_ADV_LEN]. 
#define GAPPRRA_CONN_BD_ADDR_TYPE 	0x235  //!< Address type of connected device. Read only. Size is uint8_t.
#define GAPPRRA_CONN_INTERVAL       0x236  //!< Current connection interval.  Read only. Size is uint16.  Range is 7.5ms to 4 seconds (0x0006 to 0x0C80).  Default is 0 (no connection).
#define GAPPRRA_CONN_LATENCY        0x237  //!< Current slave latency.  Read only.  Size is uint16.  Range is 0 to 499. Default is 0 (no slave latency or no connection).
#define GAPPRRA_CONN_TIMEOUT        0x238  //!< Current timeout value.  Read only.  size is uint16.  Range is 100ms to 32 seconds.  Default is 0 (no connection).
#define GAPPRRA_CONNHANDLE          0x240  //!< Connection Handle.  Read only.  size is uint16. 
#define GAPPRRA_DSPOOLID            0x241  //!< Downstream PoolID.  Read only.  size is uint16. 
#define GAPPRRA_DSDATAOFFSET        0x242  //!< Downstream Data Offset.  Read only.  size is uint16. 
#define GAPPRRA_MAXTPDUSIZE         0x243  //!< Max Tx pdu size.  Read only.  size is uint16. 
#define GAPPRRA_MAXTPDUDSCREDITS 	0x244  //!< Max Tx pdu ds credits.  Read only.  size is uint16. 
#define GAPPRRA_MTUSIZE             0x245  //!< MTU size.  Read only.  size is uint16. 
#define GAPPARA_BLE_RSSI            0x246  //!< BLE rssi.  Read only.  size is int8. 
/**
  * @}
  */

/** @defgroup GAP_PARAM_PERIPH_ONLY Peripheral Only
  * @{
  */
#define GAPPRRA_DISCONNECTED_REASON	0x250 //!< Disconnected Reason. Read only. Size is uint8. Refer to TBlueAPI_Cause.
#define GAPPRRA_ADV_WL_BD_ADDR      0x251 //!< Address of device in Whiter List. Read/Write. Size is uint8[B_ADDR_LEN]. 
#define GAPPRRA_ADV_WL_BD_ADDR_TYPE 0x252 //!< Address type of device in Whiter List. Read/Write. Size is uint8_t. 
/**
  * @}
  */

/** @defgroup GAP_PARAM_CENTRAL_ONLY Central Only
  * @{
  */
#define GAPPRRA_CONN_DEST_ADDR		0x253  //!< Destination address. Read/Write. Size is uint8[B_MAX_ADV_LEN].
#define GAPPRRA_CONN_DEST_ADDR_TYPE 0x254  //!< Destination address type. Read/Write.
#define GAPPRRA_CONN_SCAN_TIMEOUT   0x255  //!< Scanning timeout. Read/Write. Default is 0.
#define GAPPRRA_CE_LENGTH           0x256  //!< Connection Event length. Read/Write.
/**
  * @}
  */

/**
  * @} End GAP_PARAM
  */

///@cond
/* GAP Roles definition. */
#define GAP_PROFILE_BROADCASTER   0x01 //!< A device that sends advertising events only.
#define GAP_PROFILE_OBSERVER      0x02 //!< A device that receives advertising events only.
#define GAP_PROFILE_PERIPHERAL    0x04 //!< A device that accepts the establishment of an LE physical link using the connection establishment procedure
#define GAP_PROFILE_CENTRAL       0x08 //!< A device that supports the Central role initiates the establishment of a physical connection
///@endcond

/** @defgroup GAP_LOCAL_ADDR_TYPE_DEFINES GAP Address Types
 * @{
 */
#define LOCAL_ADDRTYPE_PUBLIC             blueAPI_LocalBDTypeLEPublic //!< Public address
#define LOCAL_ADDRTYPE_RANDOM             blueAPI_LocalBDTypeLERandom //!< Random address
/** @} End GAP_LOCAL_ADDR_TYPE_DEFINES */

/** @defgroup GAP_PEER_ADDR_TYPE_DEFINES GAP Peer Address Types
 * @{
 */
#define PEER_ADDRTYPE_PUBLIC                   blueAPI_RemoteBDTypeLEPublic  //!< Peer Device use public address.
#define PEER_ADDRTYPE_RANDOM                   blueAPI_RemoteBDTypeLERandom  //!< Peer Device use random address.
#define PEER_ADDRTYPE_PRIVATE_NONRESOLVE       0x02  //!< Peer Device use private nonresolve address.
#define PEER_ADDRTYPE_PRIVATE_RESOLVE          0x03  //!< Peer Device use private resolvable address.
#define PEER_ADDRTYPE_PRIVATE_IDENTITY_ADDRESS blueAPI_RemoteBDTypeLEResolved //!< Peer Device's Identity Address. Peer Device use private resolvable address.
/** @} End GAP_PEER_ADDR_TYPE_DEFINES */

#ifdef BT_GAP_LE_PRIVACY_RESOLVING
/** @defgroup GAP_RPIVACY_DEFINES GAP Privacy Mode Values
 * @{
 */
#define GAP_PRIVACY_MODE_DISABLE      0x00  //!< Disable Privacy Mode.
#define GAP_PRIVACY_MODE_ENABLE       0x01  //!< Enable Privacy Mode.
/** @} End GAP_RPIVACY_DEFINES */
#endif

/** @defgroup GAP_ADVERTISEMENT_TYPE_DEFINES GAP Advertising Event Types
 * for eventType field in gapAdvertisingParams_t
 * @{
 */
#define GAP_ADTYPE_ADV_IND                0x00  //!< Connectable undirected advertisement
#define GAP_ADTYPE_ADV_HDC_DIRECT_IND     0x01  //!< Connectable high duty cycle directed advertisement
#define GAP_ADTYPE_ADV_SCAN_IND           0x02  //!< Scannable undirected advertisement
#define GAP_ADTYPE_ADV_NONCONN_IND        0x03  //!< Non-Connectable undirected advertisement
#define GAP_ADTYPE_ADV_LDC_DIRECT_IND     0x04  //!< Connectable low duty cycle directed advertisement
/** @} End GAP_ADVERTISEMENT_TYPE_DEFINES */

/** @defgroup GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES GAP Advertising Report Event Types
 * for eventType field in gapDevRec_t and gapDeviceInfoEvent_t
 * @{
 */
#define GAP_ADRPT_ADV_IND                 0x00  //!< Connectable undirected advertisement
#define GAP_ADRPT_ADV_DIRECT_IND          0x01  //!< Connectable directed advertisement
#define GAP_ADRPT_ADV_SCAN_IND            0x02  //!< Scannable undirected advertisement
#define GAP_ADRPT_ADV_NONCONN_IND         0x03  //!< Non-Connectable undirected advertisement
#define GAP_ADRPT_SCAN_RSP                0x04  //!< Scan Response
/** @} End GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES */

/** @defgroup GAP_FILTER_POLICY_DEFINES GAP Advertiser Filter Scan Parameters
 * @{
 */
#define GAP_FILTER_POLICY_ALL         0x00 //!< Allow Scan Request from Any, Allow Connect Request from Any (default).
#define GAP_FILTER_POLICY_WHITE_SCAN  0x01 //!< Allow Scan Request from White List Only, Allow Connect from Any
#define GAP_FILTER_POLICY_WHITE_CON   0x02 //!< Allow Scan Request from Any, Connect from White List Only
#define GAP_FILTER_POLICY_WHITE       0x03 //!< Allow Scan Request and Connect from White List Only
/** @} End GAP_FILTER_POLICY_DEFINES */

/** @defgroup GAP_ADVCHAN_DEFINES GAP Advertisement Channel Map
 * @{
 */
#define GAP_ADVCHAN_37  0x01  //!< Advertisement Channel 37
#define GAP_ADVCHAN_38  0x02  //!< Advertisement Channel 38
#define GAP_ADVCHAN_39  0x04  //!< Advertisement Channel 39
#define GAP_ADVCHAN_ALL (GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39) //!< All Advertisement Channels Enabled
/** @} End GAP_ADVCHAN_DEFINES */



/** @defgroup GAP_ADTYPE_DEFINES GAP Advertisment Data Types
 * These are the data type identifiers for the data tokens in the advertisement data field.
 * @{
 */
#define GAP_ADTYPE_FLAGS                        0x01 //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES
#define GAP_ADTYPE_16BIT_MORE                   0x02 //!< Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_COMPLETE               0x03 //!< Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_32BIT_MORE                   0x04 //!< Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_COMPLETE               0x05 //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_128BIT_MORE                  0x06 //!< Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_COMPLETE              0x07 //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08 //!< Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09 //!< Complete local name
#define GAP_ADTYPE_POWER_LEVEL                  0x0A //!< TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_SM_TK                        0x10 //!< Security Manager TK Value
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11 //!< Secutiry Manager OOB Flags
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12 //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADTYPE_SIGNED_DATA                  0x13 //!< Signed Data field
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14 //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15 //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICE_DATA                 0x16 //!< Service Data
#define GAP_ADTYPE_APPEARANCE                   0x19 //!< Appearance
#define GAP_ADTYPE_MESH_NETWORK                         0xF0 //!< Mesh
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data
/** @} End GAP_ADTYPE_DEFINES */

/** @defgroup GAP_ADTYPE_FLAGS_MODES GAP ADTYPE Flags Discovery Modes
 * @{
 */
#define GAP_ADTYPE_FLAGS_LIMITED                0x01 //!< Discovery Mode: LE Limited Discoverable Mode
#define GAP_ADTYPE_FLAGS_GENERAL                0x02 //!< Discovery Mode: LE General Discoverable Mode
#define GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED    0x04 //!< Discovery Mode: BR/EDR Not Supported
/** @} End GAP_ADTYPE_FLAGS_MODES */

/** @defgroup GAP_APPEARANCE_VALUES GAP Appearance Values
 * @{
 */

/*GATT appearance definitions */
#define GAP_GATT_APPEARANCE_UNKNOWN                                GATT_APPEARANCE_UNKNOWN
#define GAP_GATT_APPEARANCE_GENERIC_PHONE                          GATT_APPEARANCE_GENERIC_PHONE
#define GAP_GATT_APPEARANCE_GENERIC_COMPUTER                       GATT_APPEARANCE_GENERIC_COMPUTER

#define GAP_GATT_APPEARANCE_GENERIC_WATCH                          GATT_APPEARANCE_GENERIC_WATCH
#define GAP_GATT_APPEARANCE_WATCH_SPORTS_WATCH                     GATT_APPEARANCE_WATCH_SPORTS_WATCH

#define GAP_GATT_APPEARANCE_GENERIC_CLOCK                          GATT_APPEARANCE_GENERIC_CLOCK
#define GAP_GATT_APPEARANCE_GENERIC_DISPLAY                        GATT_APPEARANCE_GENERIC_DISPLAY
#define GAP_GATT_APPEARANCE_GENERIC_REMOTE_CONTROL                 GATT_APPEARANCE_GENERIC_REMOTE_CONTROL
#define GAP_GATT_APPEARANCE_GENERIC_EYE_GLASSES                    GATT_APPEARANCE_GENERIC_EYE_GLASSES
#define GAP_GATT_APPEARANCE_GENERIC_TAG                            GATT_APPEARANCE_GENERIC_TAG
#define GAP_GATT_APPEARANCE_GENERIC_KEYRING                        GATT_APPEARANCE_GENERIC_KEYRING
#define GAP_GATT_APPEARANCE_GENERIC_MEDIA_PLAYER                   GATT_APPEARANCE_GENERIC_MEDIA_PLAYER
#define GAP_GATT_APPEARANCE_GENERIC_BARCODE_SCANNER                GATT_APPEARANCE_GENERIC_BARCODE_SCANNER

#define GAP_GATT_APPEARANCE_GENERIC_THERMOMETER                    GATT_APPEARANCE_GENERIC_THERMOMETER
#define GAP_GATT_APPEARANCE_THERMOMETER_EAR                        GATT_APPEARANCE_THERMOMETER_EAR

#define GAP_GATT_APPEARANCE_GENERIC_HEART_RATE_SENSOR              GATT_APPEARANCE_GENERIC_HEART_RATE_SENSOR
#define GAP_GATT_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT      GATT_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT

#define GAP_GATT_APPEARANCE_GENERIC_BLOOD_PRESSURE                 GATT_APPEARANCE_GENERIC_BLOOD_PRESSURE
#define GAP_GATT_APPEARANCE_BLOOD_PRESSURE_ARM                     GATT_APPEARANCE_BLOOD_PRESSURE_ARM
#define GAP_GATT_APPEARANCE_BLOOD_PRESSURE_WRIST                   GATT_APPEARANCE_BLOOD_PRESSURE_WRIST

#define GAP_GATT_APPEARANCE_HUMAN_INTERFACE_DEVICE                 GATT_APPEARANCE_HUMAN_INTERFACE_DEVICE
#define GAP_GATT_APPEARANCE_KEYBOARD                               GATT_APPEARANCE_KEYBOARD
#define GAP_GATT_APPEARANCE_MOUSE                                  GATT_APPEARANCE_MOUSE
#define GAP_GATT_APPEARANCE_JOYSTICK                               GATT_APPEARANCE_JOYSTICK
#define GAP_GATT_APPEARANCE_GAMEPAD                                GATT_APPEARANCE_GAMEPAD
#define GAP_GATT_APPEARANCE_DIGITIZER_TABLET                       GATT_APPEARANCE_DIGITIZER_TABLET
#define GAP_GATT_APPEARANCE_CARD_READER                            GATT_APPEARANCE_CARD_READER
#define GAP_GATT_APPEARANCE_DIGITAL_PEN                            GATT_APPEARANCE_DIGITAL_PEN
#define GAP_GATT_APPEARANCE_BARCODE_SCANNER                        GATT_APPEARANCE_BARCODE_SCANNER

#define GAP_GATT_APPEARANCE_GENERIC_GLUCOSE_METER                  GATT_APPEARANCE_GENERIC_GLUCOSE_METER

#define GAP_GATT_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR         GATT_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR
#define GAP_GATT_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE         GATT_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE
#define GAP_GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE         GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE
#define GAP_GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP          GATT_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP

#define GAP_GATT_APPEARANCE_GENERIC_CYCLING                        GATT_APPEARANCE_GENERIC_CYCLING
#define GAP_GATT_APPEARANCE_CYCLING_CYCLING_COMPUTER               GATT_APPEARANCE_CYCLING_CYCLING_COMPUTER
#define GAP_GATT_APPEARANCE_CYCLING_SPEED_SENSOR                   GATT_APPEARANCE_CYCLING_SPEED_SENSOR
#define GAP_GATT_APPEARANCE_CYCLING_CADENCE_SENSOR                 GATT_APPEARANCE_CYCLING_CADENCE_SENSOR
#define GAP_GATT_APPEARANCE_CYCLING_POWER_SENSOR                   GATT_APPEARANCE_CYCLING_POWER_SENSOR
#define GAP_GATT_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR       GATT_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR

#define GAP_GATT_APPEARANCE_GENERIC_PULSE_OXIMETER                 GATT_APPEARANCE_GENERIC_PULSE_OXIMETER
#define GAP_GATT_APPEARANCE_FINGERTIP                              GATT_APPEARANCE_FINGERTIP
#define GAP_GATT_APPEARANCE_WRIST_WORN                             GATT_APPEARANCE_WRIST_WORN
#define GAP_GATT_APPEARANCE_GENERIC_WEIGHT_SCALE                   GATT_APPEARANCE_GENERIC_WEIGHT_SCALE

#define GAP_GATT_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACTIVITY        GATT_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACTIVITY
#define GAP_GATT_APPEARANCE_LOCATION_DISPLAY_DEVICE                GATT_APPEARANCE_LOCATION_DISPLAY_DEVICE
#define GAP_GATT_APPEARANCE_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE GATT_APPEARANCE_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE
#define GAP_GATT_APPEARANCE_LOCATION_POD                           GATT_APPEARANCE_LOCATION_POD
#define GAP_GATT_APPEARANCE_LOCATION_AND_NAVIGATION_POD            GATT_APPEARANCE_LOCATION_AND_NAVIGATION_POD
/** @} End GAP_APPEARANCE_VALUES */


/** @defgroup GAP_SCAN_MODE_VALUES GAP Scan Mode Values
 * @{
 */
#define GAP_SCAN_MODE_ACTIVE     blueAPI_LEScanActive  //!< Active scan
#define GAP_SCAN_MODE_PASSIVE   blueAPI_LEScanPassive  //!< Passive scan
/** @} End GAP_SCAN_MODE_VALUES */

/** @defgroup GAP_SCAN_FILTER_POLICY_VALUES GAP Scan Filter Policy Values
 * @{
 */
#define GAP_SCAN_FILTER_ANY   blueAPI_LEFilterAny              //!<  Remote devices are not filtered.
#define GAP_SCAN_FILTER_WHITE_LIST   blueAPI_LEFilterWhitelist //!<  Remote devices are filtered against the whitelist.
/** @} End GAP_SCAN_FILTER_POLICY_VALUES */

/** @defgroup GAP_SCAN_FILTER_DUPLICATE_VALUES GAP Scan Filter Duplicate Values
 * @{
 */
#define GAP_SCAN_FILTER_DUPLICATE_ENABLE        1   //!< Scan filter duplicate enable
#define GAP_SCAN_FILTER_DUPLICATE_DISABLE       0   //!< Scan filter duplicate disable
/** @} End GAP_SCAN_FILTER_DUPLICATE_VALUES */ 

/** @defgroup GAP_STATE GAP Role States
 * @{
 */
/** @defgroup GAP_STATE_Common Common Role States
 * @{ All GAP States used by all gap roles.
 */
#define    GAPSTATE_INIT                    0   //!< Waiting to be started
#define    GAPSTATE_STACK_READY             1   //!< Started but not advertising
#define    GAPSTATE_ADVERTISING             2   //!< Advertising
#define    GAPSTATE_CONNECTED               3   //!< Connected
#define    GAPSTATE_CONNECTED_ADV           4   //!< Connected and advertising
#define    GAPSTATE_IDLE_NO_ADV_NO_CONN     5   //!< Idle, no adverting and no connection
#define    GAPSTATE_IDLE_NO_SCAN_NO_CONN    6	//!< Idle, no scanning and no connection
#define    GAPSTATE_SCANNING                7	//!< Scanning
#define    GAPSTATE_CONNECTING              8	//!< Scanning
#define    GAPSTATE_IDLE                    9	//!< Idle
/** End of GAP_STATE_Common
  * @}
  */
  
/** @defgroup GAP_STATE_PERIPHERAL Peripheral Role States
 * @{
 */
#define    GAP_PERIPHERAL_STATE_INIT                    GAPSTATE_INIT
#define    GAP_PERIPHERAL_STATE_STACK_READY             GAPSTATE_STACK_READY
#define    GAP_PERIPHERAL_STATE_ADVERTISING             GAPSTATE_ADVERTISING
#define    GAP_PERIPHERAL_STATE_CONNECTED               GAPSTATE_CONNECTED
#define    GAP_PERIPHERAL_STATE_CONNECTED_ADV           GAPSTATE_CONNECTED_ADV
#define    GAP_PERIPHERAL_STATE_IDLE_NO_ADV_NO_CONN     GAPSTATE_IDLE_NO_ADV_NO_CONN
/** End of GAP_STATE_PERIPHERAL
  * @}
  */

/** @defgroup GAP_STATE_CENTRAL Central Role States
 * @{
 */
#define    GAP_CENTRAL_STATE_INIT                       GAPSTATE_INIT
#define    GAP_CENTRAL_STATE_STACK_READY                GAPSTATE_STACK_READY
#define    GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN       GAPSTATE_IDLE_NO_SCAN_NO_CONN
#define    GAP_CENTRAL_STATE_SCANNING                   GAPSTATE_SCANNING
#define    GAP_CENTRAL_STATE_CONNECTING                 GAPSTATE_CONNECTING
#define    GAP_CENTRAL_STATE_CONNECTED                  GAPSTATE_CONNECTED
/** End of GAP_STATE_CENTRAL
  * @}
  */

/** @defgroup GAP_STATE_BROADCASTER Broadcaster Role States
 * @{
 */
#define    GAP_BROADCASTER_STATE_INIT                   GAPSTATE_INIT
#define    GAP_BROADCASTER_STATE_STACK_READY            GAPSTATE_STACK_READY
#define    GAP_BROADCASTER_STATE_ADVERTISING            GAPSTATE_ADVERTISING
#define    GAP_BROADCASTER_STATE_IDLE                   GAPSTATE_IDLE
/** End of GAP_STATE_BROADCASTER
  * @}
  */

/** @defgroup GAP_STATE_OBSERVER Observer Role States
 * @{
 */
#define    GAP_OBSERVER_STATE_INIT                      GAPSTATE_INIT
#define    GAP_OBSERVER_STATE_STACK_READY               GAPSTATE_STACK_READY
#define    GAP_OBSERVER_STATE_IDLE_NO_SCAN_NO_CONN      GAPSTATE_IDLE_NO_SCAN_NO_CONN
#define    GAP_OBSERVER_STATE_SCANNING                  GAPSTATE_SCANNING
/** End of GAP_STATE_OBSERVER
  * @}
  */

/** @defgroup GAP_STATE_OB Observer Broadcaster Role States
 * @{
 */
#define    GAP_OB_STATE_INIT                      GAPSTATE_INIT
#define    GAP_OB_STATE_STACK_READY               GAPSTATE_STACK_READY
/** End of GAP_STATE_OB
  * @}
  */

/** End of GAP_STATE
  * @}
  */
/** @defgroup GAP_CONN_PARAM_UPDATE_STATUS GAP Connection Param Update States
 * @{
 */
#define    GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS         SUCCESS   //!< Connection parameter update success
#define    GAP_CONN_PARAM_UPDATE_STATUS_FAIL            0x01      //!< Connection parameter update fail
#define    GAP_CONN_PARAM_UPDATE_STATUS_PENDING         0x02      //!< Connection parameter update pending
/** End of GAP_CONN_PARAM_UPDATE_STATUS
  * @}
  */
/** End of GAP_Exported_Constants
  * @}
  */


/** @defgroup GAP_Exported_Types GAP Exported Types
  * @{
  */

/** @defgroup GAP_STATUS GAP Status
  * @{ GAP result struct
  */
typedef enum _TGAP_STATUS
{
    gapAPI_CauseSuccess = 0x00,
    gapAPI_AlreadyInRequestedMode = 0x01,
    gapAPI_IncorrectMode = 0x02,
    gapAPI_InvalidRange = 0x03,
    gapAPI_NotConnected = 0x04,
    gapAPI_ErrorUnknown = 0x05
} TGAP_STATUS;    
/**
  * @}
  */
/** @defgroup GAP_MSG_DATA GAP Message Data
  * @{
  */
/** @defgroup TGAPCONN_STATE_CHANGE TGAPCONN_STATE_CHANGE
  * @{ Message data of BT_MSG_TYPE_CONN_STATE_CHANGE
  */
typedef struct _TGAPCONN_STATE_CHANGE
{
    uint8_t newState;
} TGAPCONN_STATE_CHANGE;
/** End of TGAPCONN_STATE_CHANGE
  * @}
  */



/** @defgroup TGAPCONN_PARA_UPDATE_CHANGE TGAPCONN_PARA_UPDATE_CHANGE
  * @{ Message data of BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE
  */
typedef struct _TGAPCONN_PARA_UPDATE_CHANGE
{
    uint16_t connHandle;
    uint8_t status;
} TGAPCONN_PARA_UPDATE_CHANGE;
/** End of TGAPCONN_PARA_UPDATE_CHANGE
  * @}
  */

/** @defgroup TGAPBONDTATE_CHANGE TGAPBONDTATE_CHANGE
  * @{ Message data of BT_MSG_TYPE_BOND_STATE_CHANGE
  */
typedef struct _TGAPBOND_STATE_CHANGE
{
    uint16_t connHandle;
    uint8_t newState;
    uint8_t status;
} TGAPBONDTATE_CHANGE;
/** End of TGAPBONDTATE_CHANGE
  * @}
  */

/** @defgroup TGAPBOND_PASSKEY_DISPLAY TGAPBOND_PASSKEY_DISPLAY
  * @{ Message data of BT_MSG_TYPE_BOND_PASSKEY_DISPLAY
  */
typedef struct _TGAPBOND_PASSKEY_DISPLAY
{
    uint16_t connHandle;
} TGAPBOND_PASSKEY_DISPLAY;
/** End of TGAPBOND_PASSKEY_DISPLAY
  * @}
  */

/** @defgroup TGAPBOND_PASSKEY_INPUT TGAPBOND_PASSKEY_INPUT
  * @{ Message data of BT_MSG_TYPE_BOND_PASSKEY_INPUT
  */
typedef struct _TGAPBOND_PASSKEY_INPUT
{
    uint16_t connHandle;
} TGAPBOND_PASSKEY_INPUT;
/** End of TGAPBOND_PASSKEY_INPUT
  * @}
  */

/** @defgroup TGAPBOND_OOB_INPUT TGAPBOND_OOB_INPUT
  * @{ Message data of BT_MSG_TYPE_BOND_OOB_INPUT
  */
typedef struct _TGAPBOND_OOB_INPUT
{
    uint16_t connHandle;
} TGAPBOND_OOB_INPUT;
/** End of TGAPBOND_OOB_INPUT
  * @}
  */

/** @defgroup TGAPENCRYPT_STATE_CHANGE TGAPENCRYPT_STATE_CHANGE
  * @{ Message data of BT_MSG_TYPE_ENCRYPT_STATE_CHANGE
  */
typedef struct _TGAPENCRYPT_STATE_CHANGE
{
    uint16_t connHandle;
    uint8_t newState;
} TGAPENCRYPT_STATE_CHANGE;
/** End of TGAPENCRYPT_STATE_CHANGE
  * @}
  */

#ifdef BT_GAP_VENDOR_CMD_ADD
/** @defgroup BLE_PARAM_TYPE BLE_PARAM_TYPE
  * @{
  */
typedef enum _TBLE_PARAM_TYPE
{
    BLE_PARAM_TYPE_TX_POWER,
    BLE_PARAM_TYPE_RSSI,
    BLE_PARAM_TYPE_SET_CHANNEL_CLASS,
    BLE_PARAM_TYPE_SET_LOCAL_SCA,
    BLE_PARAM_TYPE_ONOFF_SLAVE_LATENCY
}TBLE_PARAM_TYPE;
/** End of BLE_PARAM_TYPE
  * @}
  */
#endif

#ifdef BT_GAP_VENDOR_CMD_ADD
/** @defgroup TGAPBT_PARAM_SET_RESULT TGAPBT_PARAM_SET_RESULT
  * @{ Message data of BT_MSG_TYPE_PARAM_SET_RESULT
  */
typedef struct 
{
    TBLE_PARAM_TYPE bleParamType;
    BOOL    result;
}TGAPBT_PARAM_SET_RESULT;
/** End of TGAPBT_PARAM_SET_RESULT
  * @}
  */
#endif

/** @defgroup TBT_STACK_MSG_DATA TBT_STACK_MSG_DATA
  * @{
  */
typedef union
{
    TGAPCONN_STATE_CHANGE           gapConnStateChange;    
    TGAPBONDTATE_CHANGE               gapBondStateChange;
    TGAPBOND_PASSKEY_DISPLAY       gapBondPasskeyDisplay;
    TGAPBOND_PASSKEY_INPUT           gapBondPasskeyInput;
    TGAPBOND_OOB_INPUT                  gapBondOobInput;
    TGAPENCRYPT_STATE_CHANGE      gapEncryptStateChange;
    TGAPCONN_PARA_UPDATE_CHANGE gapConnParaUpdateChange;
#ifdef BT_GAP_VENDOR_CMD_ADD
    TGAPBT_PARAM_SET_RESULT            gapBTParamSetResult;
#endif
} TBT_STACK_MSG_DATA;
/** End of TBT_STACK_MSG_DATA
  * @}
  */

/** @defgroup BT_STACK_MSG BT_STACK_MSG
  * @{
  */
typedef struct _BT_STACK_MSG
{
    TBT_STACK_MSG_DATA msgData;
} BT_STACK_MSG;
/** End of BT_STACK_MSG
  * @}
  */
/** End of GAP_MSG_DATA
  * @}
  */
/** @defgroup BLE_TX_POWER_INDEX BLE_TX_POWER_INDEX
  * @{
  */
typedef enum _TBLE_TX_POWER_INDEX
{
    TX_POWER_MINUS_16_DBM = 0x00,   //!<-16 dbm  
    TX_POWER_0_DBM = 0x06,          //!<0 dbm        
    TX_POWER_3_DBM = 0x07,          //!<3 dbm         
    TX_POWER_UNKNOWN = 0xFF         //!<unknown        
} TBLE_TX_POWER_INDEX;    
/**
  * @}
  */

/** @defgroup _LL_TEST_PKT_TYPE_SET
  * @{
  */
typedef enum _LL_TEST_PKT_TYPE_SET {
    LL_TEST_PKT_TYPE_PRBS9,
    LL_TEST_PKT_TYPE_REPEAT_11110000,
    LL_TEST_PKT_TYPE_REPEAT_10101010,
    LL_TEST_PKT_TYPE_PRBS15,
    LL_TEST_PKT_TYPE_ALL_1, 
    LL_TEST_PKT_TYPE_ALL_0,
    LL_TEST_PKT_TYPE_REPEAT_00001111,
    LL_TEST_PKT_TYPE_REPEAT_01010101
}LL_TEST_PKT_TYPE_SET;
/**
  * @}
  */
/** @defgroup gaprole_States_t gaprole_States_t
  * @{
  */
typedef uint8_t gaprole_States_t;
/** End of gaprole_States_t
  * @}
  */
///@cond
enum {
    API_TYPE_READ_PATCH_VERSION = 1,
    API_TYPE_GET_LE_CON_HDL_OF_MDL = 2,
    API_TYPE_GATT_SEND_VENDOR_HCI_CMD = 3,
    API_TYPE_FMC_IOCTL = 4,
    API_TYPE_SET_SEC_CONN_POLICY = 5,
    API_TYPE_INIT_SEC_KEY = 6,
    API_TYPE_ENCRYPT_DATA = 7,
    API_TYPE_DECRYPT_DATA = 8,
    API_TYPE_SET_PAIRING_DBG_MODE = 9,              /* Used to En/Disable Simple Pairing Debug Mode api. */
    API_TYPE_SEND_HCI_VENDOR_ACL_DATA = 10,
    API_TYPE_AES_SET_KEY = 11,                      /* AES set key api, used for AES 128/192/256 algorithm. */
    API_TYPE_AES_ENCRYPT = 12,                      /* AES encrypt api, used for AES 128/192/256 algorithm. */
    API_TYPE_AES_DECRYPT = 13,                      /* AES decrypt api, used for AES 128/192/256 algorithm. */
    API_TYPE_SMP_KEY_DISTRIBUTION = 14,
    API_TYPE_SMP_LOCAL_IRK = 15,
    API_TYPE_RESERVED = 0xFFFF
};

typedef enum{
    LL_SCA_251_TO_500_PPM   = 0,
    LL_SCA_151_TO_250_PPM   = 1,
    LL_SCA_101_TO_150_PPM   = 2,
    LL_SCA_76_TO_100_PPM    = 3,
    LL_SCA_51_TO_75_PPM     = 4,
    LL_SCA_31_TO_50_PPM     = 5
}TSca32kExt;

/* HCI vendor command parameters. */
typedef struct _TApiBufVendorCmd{
    uint16_t opCode;
    uint8_t   len;
    uint8_t   *para;    
}TApiBufVendorCmd;

/* FMC IO ctl vendor command parameters. */
typedef struct _TApiBufFMCIoctl{
    uint32_t cmd;
    uint32_t p1;
    uint32_t p2; 
}TApiBufFMCIoctl;

typedef struct _TAclVendorData{
    uint16_t local_MDL_ID;
    uint8_t   len;
    uint8_t   *pData;
}TAclVendorData;

#ifdef BT_GAP_VENDOR_AES_API_SUPPORT
/* AES algorithm context. */
typedef struct
{
    uint32_t erk[64];     /* encryption round keys */
    uint32_t drk[64];     /* decryption round keys */
    int nr;             /* number of rounds */
}aes_context;

/* AES set key vendor command parameters. */
typedef struct _TApiBufAesSetKeyParam{
    aes_context *ctx;
    uint8_t *key;
    int nbits; 
}TApiBufAesSetKeyParam;

/* AES encrypt vendor command parameters. */
typedef struct _TApiBufAesEncryptParam{
    aes_context *ctx;
    uint8_t *input;
    uint8_t *output; 
}TApiBufAesEncryptParam;

/* AES decrypt vendor command parameters. */
typedef struct _TApiBufAesDecryptParam{
    aes_context *ctx;
    uint8_t *input;
    uint8_t *output; 
}TApiBufAesDecryptParam;
#endif

typedef struct _TApiBufUserDefined{
    uint16_t Type;
    union{
        TApiBufVendorCmd ApiBufVendorCmd;
        uint16_t *pPatchVersion;
        uint16_t *pLEConnHdl;
        TApiBufFMCIoctl fmcIoctl;
        uint8_t *pSecConnPolicy;
        uint8_t *pEncryptData;
        uint8_t *pSimplePairDbgMode;
        TAclVendorData AclVendorData;
#ifdef BT_GAP_VENDOR_AES_API_SUPPORT
        TApiBufAesSetKeyParam aesSetKeyParam;
        TApiBufAesEncryptParam aesEncryptParam;
        TApiBufAesDecryptParam aesDecryptParam;
#endif
#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
        uint8_t key_mode;
        uint8_t *pLocalIRK;
#endif
    }p;
}TApiBufUserDefined;
///@endcond

/** End of GAP_Exported_Types
  * @}
  */
/** @defgroup GAP_Exported_API_Functions Exported blueAPI Functions
  * @{
  */
extern bool blueAPI_ReadPatchVersion(uint16_t * PatchVersion);
extern bool blueAPI_OnOffLatency(uint8_t latency_onoff);
bool blueAPI_SetAdvFastDisable(uint8_t enable);
bool blueAPI_HandShakeInit(void);
bool blueAPI_HandShakeGetResponse(uint8_t *pData, uint8_t len);

#ifdef BT_GAP_VENDOR_AES_API_SUPPORT
bool blueAPI_AES_SetKey(aes_context *ctx, uint8_t *key, int nbits);
bool blueAPI_AES_Encrypt(aes_context *ctx, uint8_t input[16], uint8_t output[16]);
bool blueAPI_AES_Decrypt(aes_context *ctx, uint8_t input[16], uint8_t output[16]);
#endif

#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
bool blueAPI_SetSMPKeyDistribution(uint8_t mode);
bool blueAPI_SetSMPLocalIRK(uint8_t local_irk[16]);
#endif

bool RapidEnterDLPS(uint16_t time);
bool SetDLPSThreshold(uint16_t time);
#define blueAPI_RapidEnterDLPS    RapidEnterDLPS
#define blueAPI_SetDLPSThreshold  SetDLPSThreshold

bool blueAPI_SetRXSensitivityReg(uint8_t bEnable);

/**
  * @brief  Start LE TX Test.
  * @param  tx_channel - Range 0-39.
  * @param  length_of_test_data - Range 0-0xFF.
  * @param  packet_payload - LL_TEST_PKT_TYPE_SET.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_LETXTestStart(uint8_t tx_channel, uint8_t length_of_test_data, LL_TEST_PKT_TYPE_SET packet_payload);

/**
  * @brief  Stop LE TX Test.
  * @param  none.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_LETXTestStop(void);
///@cond
extern uint16_t blueAPI_GetLEConnHandle(uint16_t local_MDL_ID);
extern bool blueAPI_GattSendVendorHciCmd(uint16_t opCode, uint8_t len, uint8_t *para);
extern bool blueAPI_ReadRSSI(uint16_t local_MDL_ID);
extern bool blueAPI_LESetHostChanClass(uint8_t *pChannelMap);
extern bool blueAPI_SetLocalSca32kExt(TSca32kExt ppmSet);
bool blueAPI_IOCTL_AdvertisingEnable(uint8_t bEnable);
bool blueAPI_IOCTL_SetAdvertisingData(uint8_t *pData, uint8_t len);
bool blueAPI_SendVendorAclData(uint16_t local_MDL_ID, uint8_t *pData, uint8_t len);
///@endcond
/** End of GAP_Exported_API_Functions
* @}
*/

///@cond
/* Callback when the connection parameteres are updated. */
typedef void (*gapRolesParamUpdateCB_t)(uint16_t connHandle, uint8_t status);
/* Callback when the device state changed. */
typedef void (*gapDeviceStateChangeCb)( uint8_t newState );
///@endcond



/** End of GAP_COMMON_DEFINITIONS
  * @}
  */

/** End of RTK_GAP_MODULE
  * @}
  */

///@cond
/* application handle provided by BlueAPI, platform compatibility. */
extern TBlueAPIHandle   GAP_BlueAPIHandle;
/* GAP common APIs declared here for all roles to use. */
extern void GAP_ParaInit(uint8_t gapRole);
extern TGAP_STATUS GAP_SetParameter( uint16_t param, uint8_t len, void *pValue );
extern TGAP_STATUS GAP_GetParameter( uint16_t param, void *pValue );
extern bool GAP_StartBtStack(void);
extern bool GAP_ProcessGapEvent( PBlueAPI_UsMessage pMsg );
/* Application callbacks declaration used by peripheral and central roles. */
extern gapRolesParamUpdateCB_t pGapRoles_ParamUpdateCB;
extern gapDeviceStateChangeCb pGapDeviceStateChangeCb;

extern uint16_t gapPara_ConnHandle;
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
extern uint8_t  gapPara_PrivacyMode;
#endif
///@endcond

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_H */
