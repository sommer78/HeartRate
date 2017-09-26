#ifndef _GATTS_API_H
#define _GATTS_API_H
#include "gatt_profile_uuid.h"
#include "gap_api.h"


/**
 * ATT
 */

// Minimum/default MTU
#define ATT_DEFAULT_MTU               23

// MARK: Attribute PDU Opcodes
#define ATT_ERROR_RESPONSE              0x01

#define ATT_EXCHANGE_MTU_REQUEST        0x02
#define ATT_EXCHANGE_MTU_RESPONSE       0x03

#define ATT_FIND_INFORMATION_REQUEST    0x04
#define ATT_FIND_INFORMATION_REPLY      0x05
#define ATT_FIND_BY_TYPE_VALUE_REQUEST  0x06
#define ATT_FIND_BY_TYPE_VALUE_RESPONSE 0x07

#define ATT_READ_BY_TYPE_REQUEST        0x08
#define ATT_READ_BY_TYPE_RESPONSE       0x09
#define ATT_READ_REQUEST                0x0a
#define ATT_READ_RESPONSE               0x0b
#define ATT_READ_BLOB_REQUEST           0x0c
#define ATT_READ_BLOB_RESPONSE          0x0d
#define ATT_READ_MULTIPLE_REQUEST       0x0e
#define ATT_READ_MULTIPLE_RESPONSE      0x0f
#define ATT_READ_BY_GROUP_TYPE_REQUEST  0x10
#define ATT_READ_BY_GROUP_TYPE_RESPONSE 0x11

#define ATT_WRITE_REQUEST               0x12
#define ATT_WRITE_RESPONSE              0x13

#define ATT_PREPARE_WRITE_REQUEST       0x16
#define ATT_PREPARE_WRITE_RESPONSE      0x17
#define ATT_EXECUTE_WRITE_REQUEST       0x18
#define ATT_EXECUTE_WRITE_RESPONSE      0x19

#define ATT_HANDLE_VALUE_NOTIFICATION   0x1b
#define ATT_HANDLE_VALUE_INDICATION     0x1d
#define ATT_HANDLE_VALUE_CONFIRMATION   0x1e


#define ATT_WRITE_COMMAND                0x52
#define ATT_SIGNED_WRITE_COMMAND         0xD2

// MARK: ATT Error Codes
#define ATT_ERROR_INVALID_HANDLE                   0x01
#define ATT_ERROR_READ_NOT_PERMITTED               0x02
#define ATT_ERROR_WRITE_NOT_PERMITTED              0x03
#define ATT_ERROR_INVALID_PDU                      0x04
#define ATT_ERROR_INSUFFICIENT_AUTHENTICATION      0x05
#define ATT_ERROR_REQUEST_NOT_SUPPORTED            0x06
#define ATT_ERROR_INVALID_OFFSET                   0x07
#define ATT_ERROR_INSUFFICIENT_AUTHORIZATION       0x08
#define ATT_ERROR_PREPARE_QUEUE_FULL               0x09
#define ATT_ERROR_ATTRIBUTE_NOT_FOUND              0x0a
#define ATT_ERROR_ATTRIBUTE_NOT_LONG               0x0b
#define ATT_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE 0x0c
#define ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH   0x0d
#define ATT_ERROR_UNLIKELY_ERROR                   0x0e
#define ATT_ERROR_INSUFFICIENT_ENCRYPTION          0x0f
#define ATT_ERROR_UNSUPPORTED_GROUP_TYPE           0x10
#define ATT_ERROR_INSUFFICIENT_RESOURCES           0x11

// MARK: Attribute Property Flags
#define ATT_PROPERTY_BROADCAST           0x01
#define ATT_PROPERTY_READ                0x02
#define ATT_PROPERTY_WRITE_WITHOUT_RESPONSE 0x04
#define ATT_PROPERTY_WRITE               0x08
#define ATT_PROPERTY_NOTIFY              0x10
#define ATT_PROPERTY_INDICATE            0x20
#define ATT_PROPERTY_AUTHENTICATED_SIGNED_WRITE 0x40
#define ATT_PROPERTY_EXTENDED_PROPERTIES 0x80

// MARK: Attribute Property Flag, BTstack extension
// value is asked from client
#define ATT_PROPERTY_DYNAMIC             0x100
// 128 bit UUID used
#define ATT_PROPERTY_UUID128             0x200
// Authentication required
#define ATT_PROPERTY_AUTHENTICATION_REQUIRED 0x400
// Authorization from user required
#define ATT_PROPERTY_AUTHORIZATION_REQUIRED  0x800
// Encryption key size stored in upper 4 bits, 0 == no encryption, encryption key size - 1 otherwise

// ATT Transaction Timeout of 30 seconds for Command/Response or Incidationc/Confirmation
#define ATT_TRANSACTION_TIMEOUT_MS     30000

#define ATT_TRANSACTION_MODE_NONE      0x0
#define ATT_TRANSACTION_MODE_ACTIVE    0x1
#define ATT_TRANSACTION_MODE_EXECUTE   0x2
#define ATT_TRANSACTION_MODE_CANCEL    0x3

// MARK: GATT UUIDs
#define GATT_PRIMARY_SERVICE_UUID                   0x2800
#define GATT_SECONDARY_SERVICE_UUID                 0x2801
#define GATT_INCLUDE_SERVICE_UUID                   0x2802
#define GATT_CHARACTERISTICS_UUID                   0x2803
#define GATT_CHARACTERISTIC_EXTENDED_PROPERTIES     0x2900
#define GATT_CHARACTERISTIC_USER_DESCRIPTION        0x2901
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION   0x2902
#define GATT_SERVER_CHARACTERISTICS_CONFIGURATION   0x2903
#define GATT_CHARACTERISTIC_PRESENTATION_FORMAT     0x2904
#define GATT_CHARACTERISTIC_AGGREGATE_FORMAT        0x2905

#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NONE          0
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION  1
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION    2

// GAP Service and Characteristics
#define GAP_SERVICE_UUID               0x1800
#define GAP_DEVICE_NAME_UUID           0x2a00
#define GAP_APPEARANCE_UUID            0x2a01
#define GAP_PERIPHERAL_PRIVACY_FLAG    0x2a02
#define GAP_RECONNECTION_ADDRESS_UUID  0x2a03
#define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_UUID 0x2a04
#define GAP_SERVICE_CHANGED            0x2a05


// helper for BT little endian format
#define READ_BT_16( buffer, pos) ( ((uint16) buffer[pos]) | (((uint16)buffer[(pos)+1]) << 8))
#define READ_BT_24( buffer, pos) ( ((uint32) buffer[pos]) | (((uint32)buffer[(pos)+1]) << 8) | (((uint32)buffer[(pos)+2]) << 16))
#define READ_BT_32( buffer, pos) ( ((uint32) buffer[pos]) | (((uint32)buffer[(pos)+1]) << 8) | (((uint32)buffer[(pos)+2]) << 16) | (((uint32) buffer[(pos)+3])) << 24)

// helper for SDP big endian format
#define READ_NET_16( buffer, pos) ( ((uint16) buffer[(pos)+1]) | (((uint16)buffer[ pos   ]) << 8))
#define READ_NET_32( buffer, pos) ( ((uint32) buffer[(pos)+3]) | (((uint32)buffer[(pos)+2]) << 8) | (((uint32)buffer[(pos)+1]) << 16) | (((uint32) buffer[pos])) << 24)

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define MXD_SPECIFIED_EVENT_CODE 0x3F
#define MXD_CMD_STATUS_CODE 1
#define MXD_CNNCT_CMPLT_CODE 2
#define MXD_DISCNNCT_CMPLT_CODE 3
#define MXD_ADV_RPT_RESLT_CODE 4
#define MXD_ENCRYPTION_STATUS_CHG_CODE 5
#define MXD_CNNCT_UPDATE_CODE 6
#define MXD_ANCS_DATA_RPT_SUBEVT_CODE 7
#define MXD_MTU_RPT_CODE 8
#define MXD_NUM_OF_CMPLT_PKTS_CODE 9
#define MXD_DATA_RPT_CODE 10
#define MXD_PASSKEY_INPUT_IND 12
#define MXD_PASSKEY_DISPLAY_IND 13
#define MXD_PASSKEY_DISPLAY_CANCEL_IND 14
#define MXD_KEYPRESS_NOTIFICATION 15



#define GATT_CLIENT_CHAR_CFG_UPDATED_EVENT  0x00 //!< Sent when a Client Characteristic Configuration is updated.  This event is sent as an OSAL message defined as gattCharCfgUpdatedEvent_t.


#define GATT_EVENT_QUERY_COMPLETE                                0xA0

/**
 * @format HX
 * @param handle
 * @param service
 */
#define GATT_EVENT_SERVICE_QUERY_RESULT                          0xA1

/**
 * @format HY
 * @param handle
 * @param characteristic
 */
#define GATT_EVENT_CHARACTERISTIC_QUERY_RESULT                   0xA2

/**
 * @format H2X
 * @param handle
 * @param include_handle
 * @param service
 */
#define GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT                 0xA3

/**
 * @format HZ
 * @param handle
 * @param characteristic_descriptor
 */
#define GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT   0xA4

/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT             0xA5

/**
 * @format H22LV
 * @param handle
 * @param value_handle
 * @param value_offset
 * @param value_length
 * @param value
 */
#define GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT        0xA6


/**
 * @format H2LV
 * @param handle
 * @param value_handle
 * @param value_length
 * @param value
 */
#define GATT_EVENT_INDICATION                                    0xA8

/**
 * @format H2LV
 * @param descriptor_handle
 * @param descriptor_length
 * @param descriptor
 */
#define GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT        0xA9

/**
 * @format H2LV
 * @param handle
 * @param descriptor_offset
 * @param descriptor_length
 * @param descriptor
 */
#define GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT   0xAA

/**
 * @format H2
 * @param handle
 * @param MTU
 */
#define GATT_EVENT_MTU                                           0xAB

/** @} End GATT_SERV_MSG_EVENT_DEFINES */


/** @defgroup GATT_PROP_BITMAPS_DEFINES GATT Characteristic Properties Bit Fields
 * @{
 */

#define GATT_PROP_BCAST                  0x01 //!< Permits broadcasts of the Characteristic Value
#define GATT_PROP_READ                   0x02 //!< Permits reads of the Characteristic Value
#define GATT_PROP_WRITE_NO_RSP           0x04 //!< Permits writes of the Characteristic Value without response
#define GATT_PROP_WRITE                  0x08 //!< Permits writes of the Characteristic Value with response
#define GATT_PROP_NOTIFY                 0x10 //!< Permits notifications of a Characteristic Value without acknowledgement
#define GATT_PROP_INDICATE               0x20 //!< Permits indications of a Characteristic Value with acknowledgement
#define GATT_PROP_AUTHEN                 0x40 //!< Permits signed writes to the Characteristic Value
#define GATT_PROP_EXTENDED               0x80 //!< Additional characteristic properties are defined in the Characteristic Extended Properties Descriptor

/** @} End GATT_PROP_BITMAPS_DEFINES */

/** @defgroup GATT_EXT_PROP_BITMAPS_DEFINES GATT Characteristic Extended Properties Bit Fields
 * @{
 */

#define GATT_EXT_PROP_RELIABLE_WRITE     0x0001 //!< Permits reliable writes of the Characteristic Value
#define GATT_EXT_PROP_WRITABLE_AUX       0x0002 //!< Permits writes to the characteristic descriptor

/** @} End GATT_EXT_PROP_BITMAPS_DEFINES */

/** @defgroup GATT_CLIENT_CFG_BITMAPS_DEFINES GATT Client Characteristic Configuration Bit Fields
 * @{
 */

#define GATT_CLIENT_CFG_NOTIFY           0x0001 //!< The Characteristic Value shall be notified
#define GATT_CLIENT_CFG_INDICATE         0x0002 //!< The Characteristic Value shall be indicated

/** @} End GATT_CLIENT_CFG_BITMAPS_DEFINES */

/** @defgroup GATT_SERV_CFG_BITMAPS_DEFINES GATT Server Characteristic Configuration Bit Fields
 * @{
 */

#define GATT_SERV_CFG_BCAST              0x0001 //!< The Characteristic Value shall be broadcast when the server is in the broadcast procedure if advertising data resources are available

/** @} End GATT_SERV_CFG_BITMAPS_DEFINES */

#define GATT_CFG_NO_OPERATION            0x0000 // No operation

/** @defgroup GATT_FORMAT_TYPES_DEFINES GATT Characteristic Format Types
 * @{
 */

#define GATT_FORMAT_BOOL                 0x01 //!< Unsigned 1 bit; 0 = false, 1 = true
#define GATT_FORMAT_2BIT                 0x02 //!< Unsigned 2 bit integer
#define GATT_FORMAT_NIBBLE               0x03 //!< Unsigned 4 bit integer
#define GATT_FORMAT_UINT8                0x04 //!< Unsigned 8 bit integer
#define GATT_FORMAT_UINT12               0x05 //!< Unsigned 12 bit integer
#define GATT_FORMAT_UINT16               0x06 //!< Unsigned 16 bit integer
#define GATT_FORMAT_UINT24               0x07 //!< Unsigned 24 bit integer
#define GATT_FORMAT_UINT32               0x08 //!< Unsigned 32 bit integer
#define GATT_FORMAT_UINT48               0x09 //!< Unsigned 48 bit integer
#define GATT_FORMAT_UINT64               0x0a //!< Unsigned 64 bit integer
#define GATT_FORMAT_UINT128              0x0b //!< Unsigned 128 bit integer
#define GATT_FORMAT_SINT8                0x0c //!< Signed 8 bit integer
#define GATT_FORMAT_SINT12               0x0d //!< Signed 12 bit integer
#define GATT_FORMAT_SINT16               0x0e //!< Signed 16 bit integer
#define GATT_FORMAT_SINT24               0x0f //!< Signed 24 bit integer
#define GATT_FORMAT_SINT32               0x10 //!< Signed 32 bit integer
#define GATT_FORMAT_SINT48               0x11 //!< Signed 48 bit integer
#define GATT_FORMAT_SINT64               0x12 //!< Signed 64 bit integer
#define GATT_FORMAT_SINT128              0x13 //!< Signed 128 bit integer
#define GATT_FORMAT_FLOAT32              0x14 //!< IEEE-754 32 bit floating point
#define GATT_FORMAT_FLOAT64              0x15 //!< IEEE-754 64 bit floating point
#define GATT_FORMAT_SFLOAT               0x16 //!< IEEE-11073 16 bit SFLOAT
#define GATT_FORMAT_FLOAT                0x17 //!< IEEE-11073 32 bit FLOAT
#define GATT_FORMAT_DUINT16              0x18 //!< IEEE-20601 format
#define GATT_FORMAT_UTF8S                0x19 //!< UTF-8 string
#define GATT_FORMAT_UTF16S               0x1a //!< UTF-16 string
#define GATT_FORMAT_STRUCT               0x1b //!< Opaque structure

/** @} End GATT_FORMAT_TYPES_DEFINES */

/** @defgroup GATT_NS_TYPES_DEFINES GATT Namespace Types
 * @{
 */

#define GATT_NS_NONE                     0x00 //!< No namespace
#define GATT_NS_BT_SIG                   0x01 //!< Bluetooth SIG namespace



/*** Generic Status Return Values ***/
#define RTN_OK                   0x00
#define RTN_FAIL                   0x01
#define RTN_INSUFF_MEM          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C



#define MAX_ATT_SERVER_NUM 12

typedef struct
{
    uint16      start_handle;
    uint16      end_handle;

} serv_hdl_st;



#define LEN_UUID_16     2
#define LEN_UUID_128    16
#define MAX_UUID_SIZE  16

/**
 * GATT Attribute Type format.
 */
typedef struct
{
    uint8           len;
    union
    {
        uint16       uuid16;
        uint8       uuid128[MAX_UUID_SIZE];
    } uuid;

} attrType_t;
/**
 * GATT Attribute Type format.
 */
typedef struct
{
    uint8 len;         //!< Length of UUID
    const uint8 *uuid; //!< Pointer to UUID
} gattAttrType_t;

/**
 * GATT Attribute format.
 */
typedef struct attAttribute_t
{
    gattAttrType_t type; //!< Attribute type (2 or 16 octet UUIDs)
    uint16  permissions;   //!< Attribute permissions
    uint16  handle;       //!< Attribute handle - assigned internally by attribute server
    uint8 * const pValue; //!< Attribute value - encoding of the octet array is defined in
    //!< the applicable profile. The maximum length of an attribute
    //!< value shall be 512 octets.
} gattAttribute_t;

typedef uint16 (*att_get_attribute_length_callback_t)(uint16 attribute_handle);
typedef uint16 (*att_read_callback_t)(uint16 attribute_handle, uint16 offset, uint8 * buffer, uint16 buffer_size);

typedef uint32 (*att_write_callback_t)(uint16 attribute_handle, uint16 transaction_mode, uint16 offset, uint8 *buffer, uint16 buffer_size);

/**
 * GATT Structure for service callback functions - must be setup by the application
 * and used when GATTServApp_RegisterService() is called.
 */
typedef struct
{
    att_get_attribute_length_callback_t pfnGetAttributeLenCB;         //!<get attribute len callback function pointer
    att_read_callback_t pfnReadAttrCB;           //!< Read callback function pointer
    att_write_callback_t pfnWriteAttrCB;         //!< Write callback function pointer
} gatt_serviceCBs_t;
#pragma pack(push)
#pragma pack(1)
/**
 * GATT Structure for Characteristic Presentation Format Value.
 */
typedef struct
{
    uint8 format;    //!< Format of the value of this characteristic
    int8 exponent;   //!< A sign integer which represents the exponent of an integer
    uint16 unit;     //!< Unit of this attribute as defined in the data dictionary
    uint8 nameSpace; //!< Name space of the description
    uint16 desc;     //!< Description of this attribute as defined in a higher layer profile
} gattCharFormat_t;

/**
 * GATT Structure for Client Characteristic Configuration.
 */
typedef struct
{
    uint16 connHandle; //!< Client connection handle
    uint8  value;      //!< Characteristic configuration value for this client
} gattCharCfg_t;
#pragma pack(pop)

typedef struct
{
    uint16       start_handle;
    uint16      end_handle;
    gatt_serviceCBs_t  *pfnCB;

} serv_info_st;


extern serv_info_st gatt_serv_info[MAX_ATT_SERVER_NUM];

extern uint16 mxd_att_get_attribute_length_callback(uint16 attribute_handle);
extern uint16 mxd_att_read_callback(uint16 attribute_handle, uint16 offset, uint8 * buffer, uint16 buffer_size);
extern uint32 mxd_att_write_callback(uint16 attribute_handle, uint16 transaction_mode, uint16 offset, uint8 *buffer, uint16 buffer_size);
extern void gatts_init(void);
extern void GAPRole_StartDevice(gapRolesCBs_t *pAppCallbacks);

extern void GAPRole_TerminateConnection(void);

extern void GAPRole_reset(void);

extern void GAPRole_SendSecurityReq(void);

extern uint16 GATTS_addServiceStart(bool is_uuid128, uint16 uuid16, uint8* uuid128,
                                    // uint16 numAttrs, uint8 encKeySize,
                                    gatt_serviceCBs_t  *pServiceCBs);

extern uint16 GATTS_addServiceEnd(void);


extern uint16 GATTS_addChar(bool is_uuid128, uint16 uuid16, uint8* uuid128, uint16  properties, uint8  * data, uint16  data_len);


extern uint16 GATTS_addCharDescrip(uint16 uuid16, uint16  properties, uint8  * data, uint16  data_len);

extern uint16 GATTS_addCharDescrip_client_config(void);

extern uint32 GATTS_send_notify(uint16  attribute_handle, uint8  *value, uint16  value_len, uint8 source);

extern uint32 GATTS_send_indicate(uint16  attribute_handle, uint8  *value, uint16  value_len);



#endif

