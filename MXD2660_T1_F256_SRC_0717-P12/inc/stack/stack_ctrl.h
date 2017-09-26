#ifndef _STACK_CTRL_H_
#define _STACK_CTRL_H_
#include "system_config.h"
#include "mem.h"
#include "sm.h"
typedef uint16 hci_con_handle_t;
#define BD_ADDR_LEN 6


typedef uint8 bd_addr_t[BD_ADDR_LEN];
typedef uint8 sm_key_t[16];
typedef uint8 sm_key24_t[3];
typedef uint8 sm_key56_t[7];
typedef uint8 sm_key256_t[32];
typedef enum
{
    BD_ADDR_TYPE_LE_PUBLIC = 0,
    BD_ADDR_TYPE_LE_RANDOM = 1,
    BD_ADDR_TYPE_SCO       = 0xfe,
    BD_ADDR_TYPE_CLASSIC   = 0xff,
    BD_ADDR_TYPE_UNKNOWN   = 0xfe
} bd_addr_type_t;


#define LINK_KEY_LEN 16
#define LINK_KEY_STR_LEN (LINK_KEY_LEN*2)
typedef uint8 link_key_t[LINK_KEY_LEN];

/**
 * @brief link key type
 */
typedef enum
{
    COMBINATION_KEY = 0,  // standard pairing
    LOCAL_UNIT_KEY,     // ?
    REMOTE_UNIT_KEY,    // ?
    DEBUG_COMBINATION_KEY,  // SSP with debug
    UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192, // SSP Simple Pairing
    AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192,   // SSP Passkey, Number confirm, OOB
    CHANGED_COMBINATION_KEY,               // Link key changed using Change Connection Lnk Key
    UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256, // SSP Simpe Pairing
    AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256,   // SSP Passkey, Number confirm, OOB
} link_key_type_t;



// packet header sizes
#define HCI_CMD_HEADER_SIZE          3
#define HCI_ACL_HEADER_SIZE          4
#define HCI_SCO_HEADER_SIZE          3
#define HCI_EVENT_HEADER_SIZE        2




// ERRORS

// last error code in 2.1 is 0x38 - we start with 0x50 for BTstack errors
#define BTSTACK_CONNECTION_TO_BTDAEMON_FAILED              0x50
#define BTSTACK_ACTIVATION_FAILED_SYSTEM_BLUETOOTH         0x51
#define BTSTACK_ACTIVATION_POWERON_FAILED                  0x52
#define BTSTACK_ACTIVATION_FAILED_UNKNOWN                  0x53
#define BTSTACK_NOT_ACTIVATED                              0x54
#define BTSTACK_BUSY                                       0x55
#define BTSTACK_MEMORY_ALLOC_FAILED                        0x56
#define BTSTACK_ACL_BUFFERS_FULL                           0x57

// l2cap errors - enumeration by the command that created them
#define L2CAP_COMMAND_REJECT_REASON_COMMAND_NOT_UNDERSTOOD 0x60
#define L2CAP_COMMAND_REJECT_REASON_SIGNALING_MTU_EXCEEDED 0x61
#define L2CAP_COMMAND_REJECT_REASON_INVALID_CID_IN_REQUEST 0x62

#define L2CAP_CONNECTION_RESPONSE_RESULT_SUCCESSFUL        0x63
#define L2CAP_CONNECTION_RESPONSE_RESULT_PENDING           0x64
#define L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_PSM       0x65
#define L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_SECURITY  0x66
#define L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_RESOURCES 0x67
#define L2CAP_CONNECTION_RESPONSE_RESULT_RTX_TIMEOUT       0x68

#define L2CAP_SERVICE_ALREADY_REGISTERED                   0x69
#define L2CAP_DATA_LEN_EXCEEDS_REMOTE_MTU                  0x6A

#define RFCOMM_MULTIPLEXER_STOPPED                         0x70
#define RFCOMM_CHANNEL_ALREADY_REGISTERED                  0x71
#define RFCOMM_NO_OUTGOING_CREDITS                         0x72
#define RFCOMM_AGGREGATE_FLOW_OFF                          0x73
#define RFCOMM_DATA_LEN_EXCEEDS_MTU                        0x74

#define SDP_HANDLE_ALREADY_REGISTERED                      0x80
#define SDP_QUERY_INCOMPLETE                               0x81
#define SDP_SERVICE_NOT_FOUND                              0x82
#define SDP_HANDLE_INVALID                                 0x83
#define SDP_QUERY_BUSY                                     0x84

#define ATT_HANDLE_VALUE_INDICATION_IN_PORGRESS            0x90
#define ATT_HANDLE_VALUE_INDICATION_TIMEOUT                0x91

#define GATT_CLIENT_NOT_CONNECTED                          0x93
#define GATT_CLIENT_BUSY                                   0x94
#define GATT_CLIENT_IN_WRONG_STATE                         0x95
#define GATT_CLIENT_DIFFERENT_CONTEXT_FOR_ADDRESS_ALREADY_EXISTS 0x96
#define GATT_CLIENT_VALUE_TOO_LONG                         0x97
#define GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED 0x98
#define GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED   0x99

#define BNEP_SERVICE_ALREADY_REGISTERED                    0xA0
#define BNEP_CHANNEL_NOT_CONNECTED                         0xA1
#define BNEP_DATA_LEN_EXCEEDS_MTU                          0xA2


/**
 * L2CAP Layer
 */

#define L2CAP_HEADER_SIZE 4

#define L2CAP_SIG_ID_INVALID 0


// Minimum/default MTU
#define L2CAP_LE_DEFAULT_MTU  (DATA_PDU_DATA_PAYLOAD_SIZE -L2CAP_HEADER_SIZE)

// L2CAP Fixed Channel IDs
#define L2CAP_CID_SIGNALING                 0x0001
#define L2CAP_CID_CONNECTIONLESS_CHANNEL    0x0002
#define L2CAP_CID_ATTRIBUTE_PROTOCOL        0x0004
#define L2CAP_CID_SIGNALING_LE              0x0005
#define L2CAP_CID_SECURITY_MANAGER_PROTOCOL 0x0006

// L2CAP Configuration Result Codes
#define L2CAP_CONF_RESULT_UNKNOWN_OPTIONS   0x0003

// L2CAP Reject Result Codes
#define L2CAP_REJ_CMD_UNKNOWN               0x0000

// Response Timeout eXpired
#define L2CAP_RTX_TIMEOUT_MS   10000

// Extended Response Timeout eXpired
#define L2CAP_ERTX_TIMEOUT_MS 120000

// Fixed PSM numbers
#define PSM_SDP           0x01
#define PSM_RFCOMM        0x03
#define PSM_BNEP          0x0F
#define PSM_HID_CONTROL   0x11
#define PSM_HID_INTERRUPT 0x13



/* ------------------------------------------------------------------------------------------------
 *                                             Macros
 * ------------------------------------------------------------------------------------------------
 */

#define BV(n)      (1 << (n))
#define BF(x,b,s)  (((x) & (b)) >> (s))
//#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
//#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
//#define ABS(n)     (((n) < 0) ? -(n) : (n))

/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) \
          (uint8)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32)((uint32)((Byte0) & 0x00FF) \
          + ((uint32)((Byte1) & 0x00FF) << 8) \
          + ((uint32)((Byte2) & 0x00FF) << 16) \
          + ((uint32)((Byte3) & 0x00FF) << 24)))

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))


#define BUILD_UINT8(hiByte, loByte) \
          ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)

#define GET_BIT(DISCS, IDX)  (((DISCS)[((IDX) / 8)] & BV((IDX) % 8)) ? TRUE : FALSE)
#define SET_BIT(DISCS, IDX)  (((DISCS)[((IDX) / 8)] |= BV((IDX) % 8)))
#define CLR_BIT(DISCS, IDX)  (((DISCS)[((IDX) / 8)] &= (BV((IDX) % 8) ^ 0xFF)))

// HCI CMD OGF/OCF
#define READ_CMD_OGF(buffer) (buffer[1] >> 2)
#define READ_CMD_OCF(buffer) ((buffer[1] & 0x03) << 8 | buffer[0])


// hack: compilation with the android ndk causes an error as there's a reverse_64 macro
#undef reverse_64

// will be moved to daemon/btstack_device_name_db.h

/**
 * @brief The device name type
 */
#define DEVICE_NAME_LEN 248
typedef uint8 device_name_t[DEVICE_NAME_LEN + 1];





/** @defgroup BLE_STATUS_VALUES BLE Default BLE Status Values
 * returned as bStatus_t
 * @{
 */
#define bleInvalidTaskID                INVALID_TASK  //!< Task ID isn't setup properly
#define bleNotReady                     0x10  //!< Not ready to perform task
#define bleAlreadyInRequestedMode       0x11  //!< Already performing that task
#define bleIncorrectMode                0x12  //!< Not setup properly to perform that task
#define bleMemAllocError                0x13  //!< Memory allocation error occurred
#define bleNotConnected                 0x14  //!< Can't perform function when not in a connection
#define bleNoResources                  0x15  //!< There are no resource available
#define blePending                      0x16  //!< Waiting
#define bleTimeout                      0x17  //!< Timed out performing function
#define bleInvalidRange                 0x18  //!< A parameter is out of range
#define bleLinkEncrypted                0x19  //!< The link is already encrypted
#define bleProcedureComplete            0x1A  //!< The Procedure is completed
#define bleInvalidMtuSize               0x1B  //!< MTU size is out of range

// GAP Status Return Values - returned as bStatus_t
#define bleGAPUserCanceled              0x30  //!< The user canceled the task
#define bleGAPConnNotAcceptable         0x31  //!< The connection was not accepted
#define bleGAPBondRejected              0x32  //!< The bound information was rejected.

// ATT Status Return Values - returned as bStatus_t
#define bleInvalidPDU                   0x40  //!< The attribute PDU is invalid
#define bleInsufficientAuthen           0x41  //!< The attribute has insufficient authentication
#define bleInsufficientEncrypt          0x42  //!< The attribute has insufficient encryption
#define bleInsufficientKeySize          0x43  //!< The attribute has insufficient encryption key size



/*** Generic Status Return Values ***/
#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
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
/**************************************************************************************************
 */

#define ATT_BT_UUID_SIZE 2




/**
 * SDP Protocl
 */

// PDU Types
typedef enum
{
    SDP_Invalid = 0,
    SDP_ErrorResponse = 1,
    SDP_ServiceSearchRequest,
    SDP_ServiceSearchResponse,
    SDP_ServiceAttributeRequest,
    SDP_ServiceAttributeResponse,
    SDP_ServiceSearchAttributeRequest,
    SDP_ServiceSearchAttributeResponse
} SDP_PDU_ID_t;

// UNIVERSAL ATTRIBUTE DEFINITIONS
#define SDP_ServiceRecordHandle     0x0000
#define SDP_ServiceClassIDList      0x0001
#define SDP_ServiceRecordState      0x0002
#define SDP_ServiceID               0x0003
#define SDP_ProtocolDescriptorList  0x0004
#define SDP_BrowseGroupList         0x0005
#define SDP_LanguageBaseAttributeIDList 0x0006
#define SDP_ServiceInfoTimeToLive   0x0007
#define SDP_ServiceAvailability     0x0008
#define SDP_BluetoothProfileDescriptorList 0x0009
#define SDP_DocumentationURL        0x000a
#define SDP_ClientExecutableURL     0x000b
#define SDP_IconURL                 0x000c
#define SDP_AdditionalProtocolDescriptorList 0x000d
#define SDP_SupportedFormatsList    0x0303

// SERVICE CLASSES
#define SDP_OBEXObjectPush          0x1105
#define SDP_OBEXFileTransfer        0x1106
#define SDP_PublicBrowseGroup       0x1002
#define SDP_HSP                     0x1108
#define SDP_Headset_AG              0x1112
#define SDP_PANU                    0x1115
#define SDP_NAP                     0x1116
#define SDP_GN                      0x1117
#define SDP_Handsfree               0x111E
#define SDP_HandsfreeAudioGateway   0x111F
#define SDP_Headset_HS              0x1131
#define SDP_GenericAudio            0x1203


// PROTOCOLS
#define SDP_SDPProtocol       0x0001
#define SDP_UDPProtocol       0x0002
#define SDP_RFCOMMProtocol    0x0003
#define SDP_OBEXProtocol      0x0008
#define SDP_L2CAPProtocol     0x0100
#define SDP_BNEPProtocol      0x000F
#define SDP_AVDTPProtocol     0x0019

// OFFSETS FOR LOCALIZED ATTRIBUTES - SDP_LanguageBaseAttributeIDList
#define SDP_Offest_ServiceName      0x0000
#define SDP_Offest_ServiceDescription 0x0001
#define SDP_Offest_ProviderName     0x0002

// OBEX
#define SDP_vCard_2_1       0x01
#define SDP_vCard_3_0       0x02
#define SDP_vCal_1_0        0x03
#define SDP_iCal_2_0        0x04
#define SDP_vNote           0x05
#define SDP_vMessage        0x06
#define SDP_OBEXFileTypeAny 0xFF

/**
 * RFCOMM Protocol
 */

// Control field values      bit no.       1 2 3 4 PF 6 7 8
#define BT_RFCOMM_SABM       0x3F       // 1 1 1 1  1 1 0 0
#define BT_RFCOMM_UA         0x73       // 1 1 0 0  1 1 1 0
#define BT_RFCOMM_DM         0x0F       // 1 1 1 1  0 0 0 0
#define BT_RFCOMM_DM_PF      0x1F       // 1 1 1 1  1 0 0 0
#define BT_RFCOMM_DISC       0x53       // 1 1 0 0  1 0 1 0
#define BT_RFCOMM_UIH        0xEF       // 1 1 1 1  0 1 1 1
#define BT_RFCOMM_UIH_PF     0xFF       // 1 1 1 1  0 1 1 1

// Multiplexer message types
#define BT_RFCOMM_CLD_CMD    0xC3
#define BT_RFCOMM_FCON_CMD   0xA3
#define BT_RFCOMM_FCON_RSP   0xA1
#define BT_RFCOMM_FCOFF_CMD  0x63
#define BT_RFCOMM_FCOFF_RSP  0x61
#define BT_RFCOMM_MSC_CMD    0xE3
#define BT_RFCOMM_MSC_RSP    0xE1
#define BT_RFCOMM_NSC_RSP    0x11
#define BT_RFCOMM_PN_CMD     0x83
#define BT_RFCOMM_PN_RSP     0x81
#define BT_RFCOMM_RLS_CMD    0x53
#define BT_RFCOMM_RLS_RSP    0x51
#define BT_RFCOMM_RPN_CMD    0x93
#define BT_RFCOMM_RPN_RSP    0x91
#define BT_RFCOMM_TEST_CMD   0x23
#define BT_RFCOMM_TEST_RSP   0x21

// Line Status
#define LINE_STATUS_NO_ERROR       0x00
#define LINE_STATUS_OVERRUN_ERROR  0x03
#define LINE_STATUS_PARITY_ERORR   0x05
#define LINE_STATUS_FRAMING_ERROR  0x09

// Modem Status Flags
#define MODEM_STATUS_FC   0x02
#define MODEM_STATUS_RTC  0x04
#define MODEM_STATUS_RTR  0x08
#define MODEM_STATUS_IC   0x40
#define MODEM_STATUS_DV   0x80

typedef enum rpn_baud
{
    RPN_BAUD_2400 = 0,
    RPN_BAUD_4800,
    RPN_BAUD_7200,
    RPN_BAUD_9600,
    RPN_BAUD_19200,
    RPN_BAUD_38400,
    RPN_BAUD_57600,
    RPN_BAUD_115200,
    RPN_BAUD_230400
} rpn_baud_t;

typedef enum rpn_data_bits
{
    RPN_DATA_BITS_5 = 0,
    RPN_DATA_BITS_6 = 0,
    RPN_DATA_BITS_7 = 0,
    RPN_DATA_BITS_8 = 0
} rpn_data_bits_t;

typedef enum rpn_stop_bits
{
    RPN_STOP_BITS_1_0 = 0,
    RPN_STOP_BITS_1_5
} rpn_stop_bits_t;

typedef enum rpn_parity
{
    RPN_PARITY_NONE  = 0,
    RPN_PARITY_ODD   = 1,
    RPN_PARITY_EVEN  = 3,
    RPN_PARITY_MARK  = 5,
    RPN_PARITY_SPACE = 7,
} rpn_parity_t;

typedef enum rpn_flow_control
{
    RPN_FLOW_CONTROL_XONXOFF_ON_INPUT  = 1 << 0,
    RPN_FLOW_CONTROL_XONXOFF_ON_OUTPUT = 1 << 1,
    RPN_FLOW_CONTROL_RTR_ON_INPUT  = 1 << 2,
    RPN_FLOW_CONTROL_RTR_ON_OUTPUT = 1 << 3,
    RPN_FLOW_CONTROL_RTC_ON_INPUT  = 1 << 4,
    RPN_FLOW_CONTROL_RTC_ON_OUTPUT = 1 << 5,
} rpn_flow_control_t;

#define RPN_PARAM_MASK_0_BAUD             0x01
#define RPN_PARAM_MASK_0_DATA_BITS        0x02
#define RPN_PARAM_MASK_0_STOP_BITS        0x04
#define RPN_PARAM_MASK_0_PARITY           0x08
#define RPN_PARAM_MASK_0_PARITY_TYPE      0x10
#define RPN_PARAM_MASK_0_XON_CHAR         0x20
#define RPN_PARAM_MASK_0_XOFF_CHAR        0x40
#define RPN_PARAM_MASK_0_RESERVED         0x80

// @note: values are identical to rpn_flow_control_t
#define RPN_PARAM_MASK_1_XONOFF_ON_INPUT  0x01
#define RPN_PARAM_MASK_1_XONOFF_ON_OUTPUT 0x02
#define RPN_PARAM_MASK_1_RTR_ON_INPUT     0x04
#define RPN_PARAM_MASK_1_RTR_ON_OUTPUT    0x08
#define RPN_PARAM_MASK_1_RTC_ON_INPUT     0x10
#define RPN_PARAM_MASK_1_RTC_ON_OUTPUT    0x20
#define RPN_PARAM_MASK_1_RESERVED_0       0x40
#define RPN_PARAM_MASK_1_RESERVED_1       0x80

/**
 * BNEP Protocol
 */

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#ifndef ETHERTYPE_VLAN
#define ETHERTYPE_VLAN                                  0x8100 /* IEEE 802.1Q VLAN tag */
#endif

#define BNEP_MTU_MIN                                    1691

#define BNEP_EXT_FLAG                                   0x80
#define BNEP_TYPE_MASK                                  0x7F
#define BNEP_TYPE(header)                               ((header) & BNEP_TYPE_MASK)
#define BNEP_HEADER_HAS_EXT(x)                          (((x) & BNEP_EXT_FLAG) == BNEP_EXT_FLAG)

/* BNEP packet types */
#define BNEP_PKT_TYPE_GENERAL_ETHERNET                  0x00
#define BNEP_PKT_TYPE_CONTROL                           0x01
#define BNEP_PKT_TYPE_COMPRESSED_ETHERNET               0x02
#define BNEP_PKT_TYPE_COMPRESSED_ETHERNET_SOURCE_ONLY   0x03
#define BNEP_PKT_TYPE_COMPRESSED_ETHERNET_DEST_ONLY     0x04

/* BNEP control types */
#define BNEP_CONTROL_TYPE_COMMAND_NOT_UNDERSTOOD        0x00
#define BNEP_CONTROL_TYPE_SETUP_CONNECTION_REQUEST      0x01
#define BNEP_CONTROL_TYPE_SETUP_CONNECTION_RESPONSE     0x02
#define BNEP_CONTROL_TYPE_FILTER_NET_TYPE_SET           0x03
#define BNEP_CONTROL_TYPE_FILTER_NET_TYPE_RESPONSE      0x04
#define BNEP_CONTROL_TYPE_FILTER_MULTI_ADDR_SET         0x05
#define BNEP_CONTROL_TYPE_FILTER_MULTI_ADDR_RESPONSE    0x06

/* BNEP extension header types */
#define BNEP_EXT_HEADER_TYPE_EXTENSION_CONTROL          0x00

/* BNEP setup response codes */
#define BNEP_RESP_SETUP_SUCCESS                         0x0000
#define BNEP_RESP_SETUP_INVALID_DEST_UUID               0x0001
#define BNEP_RESP_SETUP_INVALID_SOURCE_UUID             0x0002
#define BNEP_RESP_SETUP_INVALID_SERVICE_UUID_SIZE       0x0003
#define BNEP_RESP_SETUP_CONNECTION_NOT_ALLOWED          0x0004

/* BNEP filter response codes */
#define BNEP_RESP_FILTER_SUCCESS                        0x0000
#define BNEP_RESP_FILTER_UNSUPPORTED_REQUEST            0x0001
#define BNEP_RESP_FILTER_ERR_INVALID_RANGE              0x0002
#define BNEP_RESP_FILTER_ERR_TOO_MANY_FILTERS           0x0003
#define BNEP_RESP_FILTER_ERR_SECURITY                   0x0004

/**
 * PAN Profile
 */

typedef enum
{
    PANU_UUID = 0x1115,
    NAP_UUID = 0x1116,
    GN_UUID = 0x1117
} bnep_service_uuid_t;

typedef enum
{
    BNEP_SECURITY_NONE = 0x0000,
    BNEP_SECURITY_SERVICE_LEVEL_ENFORCED,
    BNEP_SECURITY_802_1X
} security_description_t;

typedef enum
{
    PAN_NET_ACCESS_TYPE_PSTN = 0x0000,
    PAN_NET_ACCESS_TYPE_ISDN,
    PAN_NET_ACCESS_TYPE_DSL,
    PAN_NET_ACCESS_TYPE_CABLE_MODEM,
    PAN_NET_ACCESS_TYPE_10MB_ETHERNET,
    PAN_NET_ACCESS_TYPE_100MB_ETHERNET,
    PAN_NET_ACCESS_TYPE_4MB_TOKEN_RING,
    PAN_NET_ACCESS_TYPE_16MB_TOKEN_RING,
    PAN_NET_ACCESS_TYPE_100MB_TOKEN_RING,
    PAN_NET_ACCESS_TYPE_FDDI,
    PAN_NET_ACCESS_TYPE_GSM,
    PAN_NET_ACCESS_TYPE_CDMA,
    PAN_NET_ACCESS_TYPE_GPRS,
    PAN_NET_ACCESS_TYPE_3G,
    PAN_NET_ACCESS_TYPE_CELULAR,
    PAN_NET_ACCESS_TYPE_OTHER = 0xFFFE,
    PAN_NET_ACCESS_TYPE_NONE
} net_access_type_t;


/**
 * SM - LE Security Manager
 */
// Bluetooth Spec definitions
typedef enum
{
    SM_CODE_PAIRING_REQUEST = 0X01,
    SM_CODE_PAIRING_RESPONSE,
    SM_CODE_PAIRING_CONFIRM,
    SM_CODE_PAIRING_RANDOM,
    SM_CODE_PAIRING_FAILED,
    SM_CODE_ENCRYPTION_INFORMATION,
    SM_CODE_MASTER_IDENTIFICATION,
    SM_CODE_IDENTITY_INFORMATION,
    SM_CODE_IDENTITY_ADDRESS_INFORMATION,
    SM_CODE_SIGNING_INFORMATION,
    SM_CODE_SECURITY_REQUEST,
    SM_CODE_PAIRING_PUBLIC_KEY,
    SM_CODE_PAIRING_DHKEY_CHECK,
    SM_CODE_KEYPRESS_NOTIFICATION,
} SECURITY_MANAGER_COMMANDS;

// IO Capability Values
typedef enum
{
    IO_CAPABILITY_DISPLAY_ONLY = 0,
    IO_CAPABILITY_DISPLAY_YES_NO,
    IO_CAPABILITY_KEYBOARD_ONLY,
    IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
    IO_CAPABILITY_KEYBOARD_DISPLAY, // not used by secure simple pairing
} io_capability_t;

// Authentication requirement flags
#define SM_AUTHREQ_NO_BONDING        0x00
#define SM_AUTHREQ_BONDING           0x01
#define SM_AUTHREQ_MITM_PROTECTION   0x04
#define SM_AUTHREQ_SECURE_CONNECTION 0x08
#define SM_AUTHREQ_KEYPRESS          0x10

// Key distribution flags used by spec
#define SM_KEYDIST_ENC_KEY  0x01
#define SM_KEYDIST_ID_KEY   0x02
#define SM_KEYDIST_SIGN     0x04
#define SM_KEYDIST_LINK_KEY 0x08

// Key distribution flags used internally
#define SM_KEYDIST_FLAG_ENCRYPTION_INFORMATION       0x01
#define SM_KEYDIST_FLAG_MASTER_IDENTIFICATION        0x02
#define SM_KEYDIST_FLAG_IDENTITY_INFORMATION         0x04
#define SM_KEYDIST_FLAG_IDENTITY_ADDRESS_INFORMATION 0x08
#define SM_KEYDIST_FLAG_SIGNING_IDENTIFICATION       0x10

// STK Generation Methods
#define SM_STK_GENERATION_METHOD_JUST_WORKS          0x01
#define SM_STK_GENERATION_METHOD_OOB                 0x02
#define SM_STK_GENERATION_METHOD_PASSKEY             0x04
#define SM_STK_GENERATION_METHOD_NUMERIC_COMPARISON  0x08

// Pairing Failed Reasons
#define SM_REASON_RESERVED                     0x00
#define SM_REASON_PASSKEYT_ENTRY_FAILED        0x01
#define SM_REASON_OOB_NOT_AVAILABLE            0x02
#define SM_REASON_AUTHENTHICATION_REQUIREMENTS 0x03
#define SM_REASON_CONFIRM_VALUE_FAILED         0x04
#define SM_REASON_PAIRING_NOT_SUPPORTED        0x05
#define SM_REASON_ENCRYPTION_KEY_SIZE          0x06
#define SM_REASON_COMMAND_NOT_SUPPORTED        0x07
#define SM_REASON_UNSPECIFIED_REASON           0x08
#define SM_REASON_REPEATED_ATTEMPTS            0x09
#define SM_REASON_INVALID_PARAMETERS           0x0a
#define SM_REASON_DHKEY_CHECK_FAILED           0x0b
#define SM_REASON_NUMERIC_COMPARISON_FAILED    0x0c

// also, invalid parameters
// and reserved

// Keypress Notifications
#define SM_KEYPRESS_PASSKEY_ENTRY_STARTED      0x00
#define SM_KEYPRESS_PASSKEY_DIGIT_ENTERED      0x01
#define SM_KEYPRESS_PASSKEY_DIGIT_ERASED       0x02
#define SM_KEYPRESS_PASSKEY_CLEARED            0x03
#define SM_KEYPRESS_PASSKEY_ENTRY_COMPLETED    0x04


// Company identifiers / manufacturers
#define COMPANY_ID_CAMBRIDGE_SILICON_RADIO     0x000A
#define COMPANY_ID_TEXAS_INSTRUMENTS_INC       0x000D
#define COMPANY_ID_BROADCOM_CORPORATION        0x000F
#define COMPANY_ID_ST_MICROELECTRONICS         0x0030
#define COMPANY_ID_EM_MICROELECTRONICS_MARIN   0x005A


// Authentication flags
typedef enum
{
    AUTH_FLAGS_NONE                = 0x0000,
    RECV_LINK_KEY_REQUEST          = 0x0001,
    HANDLE_LINK_KEY_REQUEST        = 0x0002,
    SENT_LINK_KEY_REPLY            = 0x0004,
    SENT_LINK_KEY_NEGATIVE_REQUEST = 0x0008,
    RECV_LINK_KEY_NOTIFICATION     = 0x0010,
    DENY_PIN_CODE_REQUEST          = 0x0040,
    RECV_IO_CAPABILITIES_REQUEST   = 0x0080,
    SEND_IO_CAPABILITIES_REPLY     = 0x0100,
    SEND_USER_CONFIRM_REPLY        = 0x0200,
    SEND_USER_PASSKEY_REPLY        = 0x0400,

    // pairing status
    LEGACY_PAIRING_ACTIVE          = 0x2000,
    SSP_PAIRING_ACTIVE             = 0x4000,

    // connection status
    CONNECTION_ENCRYPTED           = 0x8000,
} hci_authentication_flags;

/**
 * Connection State
 */
typedef enum
{
    NOT_CONNECTED        =   0,
    SEND_CREATE_CONNECTION,
    SENT_CREATE_CONNECTION,
    SEND_CANCEL_CONNECTION,
    SENT_CANCEL_CONNECTION,
    RECEIVED_CONNECTION_REQUEST,
    ACCEPTED_CONNECTION_REQUEST,
    REJECTED_CONNECTION_REQUEST,
    OPEN,
    SEND_DISCONNECT,
    SENT_DISCONNECT,
    RECEIVED_DISCONNECTION_COMPLETE
} CONNECTION_STATE;

// bonding flags
enum
{
    BONDING_REQUEST_REMOTE_FEATURES   = 0x01,
    BONDING_RECEIVED_REMOTE_FEATURES  = 0x02,
    BONDING_REMOTE_SUPPORTS_SSP       = 0x04,
    BONDING_DISCONNECT_SECURITY_BLOCK = 0x08,
    BONDING_DISCONNECT_DEDICATED_DONE = 0x10,
    BONDING_SEND_AUTHENTICATE_REQUEST = 0x20,
    BONDING_SEND_ENCRYPTION_REQUEST   = 0x40,
    BONDING_DEDICATED                 = 0x80,
    BONDING_EMIT_COMPLETE_ON_DISCONNECT = 0x100
};

typedef enum
{
    BLUETOOTH_OFF = 1,
    BLUETOOTH_ON,
    BLUETOOTH_ACTIVE
} BLUETOOTH_STATE;

// le central scanning state
typedef enum
{
    LE_SCAN_IDLE,
    LE_START_SCAN,
    LE_SCANNING,
    LE_STOP_SCAN,
} le_scanning_state;

typedef enum
{
    LE_CONNECTING_IDLE,
    LE_CONNECTING_DIRECT,
    LE_CONNECTING_WHITELIST,
} le_connecting_state;

enum
{
    DISABLE_ADVERTISEMENTS   = 1 << 0,
    SET_ADVERTISEMENT_PARAMS = 1 << 1,
    SET_ADVERTISEMENT_DATA   = 1 << 2,
    SET_SCAN_RESPONSE_DATA   = 1 << 3,
    ENABLE_ADVERTISEMENTS    = 1 << 4,
    SET_RADOM_ADDR    = 1 << 5,
};


// from Bluetooth Core Specification
#define ERROR_CODE_SUCCESS                                 0x00
#define ERROR_CODE_UNKNOWN_HCI_COMMAND                     0x01
#define ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER           0x02
#define ERROR_CODE_HARDWARE_FAILURE                        0x03
#define ERROR_CODE_PAGE_TIMEOUT                            0x04
#define ERROR_CODE_AUTHENTICATION_FAILURE                  0x05
#define ERROR_CODE_PIN_OR_KEY_MISSING                      0x06
#define ERROR_CODE_MEMORY_CAPACITY_EXCEEDED                0x07
#define ERROR_CODE_CONNECTION_TIMEOUT                      0x08
#define ERROR_CODE_CONNECTION_LIMIT_EXCEEDED               0x09
#define ERROR_CODE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED  0x0A
#define ERROR_CODE_ACL_CONNECTION_ALREADY_EXISTS           0x0B
#define ERROR_CODE_COMMAND_DISALLOWED                      0x0C
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES 0x0D
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS  0x0E
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR 0x0F
#define ERROR_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED      0x10
#define ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE  0x11
#define ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS          0x12
#define ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION       0x13
#define ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES 0x14
#define ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF     0x15
#define ERROR_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST     0x16
#define ERROR_CODE_REPEATED_ATTEMPTS                       0x17
#define ERROR_CODE_PAIRING_NOT_ALLOWED                     0x18
#define ERROR_CODE_UNKNOWN_LMP_PDU                         0x19
#define ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE_UNSUPPORTED_LMP_FEATURE 0x1A
#define ERROR_CODE_SCO_OFFSET_REJECTED                     0x1B
#define ERROR_CODE_SCO_INTERVAL_REJECTED                   0x1C
#define ERROR_CODE_SCO_AIR_MODE_REJECTED                   0x1D
#define ERROR_CODE_INVALID_LMP_PARAMETERS_INVALID_LL_PARAMETERS 0x1E
#define ERROR_CODE_UNSPECIFIED_ERROR                       0x1F
#define ERROR_CODE_UNSUPPORTED_LMP_PARAMETER_VALUE_UNSUPPORTED_LL_PARAMETER_VALUE 0x20
#define ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED                 0x21
#define ERROR_CODE_LMP_RESPONSE_TIMEOUT_LL_RESPONSE_TIMEOUT 0x22
#define ERROR_CODE_LMP_ERROR_TRANSACTION_COLLISION         0x23
#define ERROR_CODE_LMP_PDU_NOT_ALLOWED                     0x24
#define ERROR_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE          0x25
#define ERROR_CODE_LINK_KEY_CANNOT_BE_CHANGED              0x26
#define ERROR_CODE_REQUESTED_QOS_NOT_SUPPORTED             0x27
#define ERROR_CODE_INSTANT_PASSED                          0x28
#define ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED     0x29
#define ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION         0x2A
#define ERROR_CODE_RESERVED                                0x2B
#define ERROR_CODE_QOS_UNACCEPTABLE_PARAMETER              0x2C
#define ERROR_CODE_QOS_REJECTED                            0x2D
#define ERROR_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED    0x2E
#define ERROR_CODE_INSUFFICIENT_SECURITY                   0x2F
#define ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE        0x30
// #define ERROR_CODE_RESERVED
#define ERROR_CODE_ROLE_SWITCH_PENDING                     0x32
// #define ERROR_CODE_RESERVED
#define ERROR_CODE_RESERVED_SLOT_VIOLATION                 0x34
#define ERROR_CODE_ROLE_SWITCH_FAILED                      0x35
#define ERROR_CODE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE     0x36
#define ERROR_CODE_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST 0x37
#define ERROR_CODE_HOST_BUSY_PAIRING                       0x38
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND 0x39
#define ERROR_CODE_CONTROLLER_BUSY                         0x3A
#define ERROR_CODE_UNACCEPTABLE_CONNECTION_PARAMETERS      0x3B
#define ERROR_CODE_DIRECTED_ADVERTISING_TIMEOUT            0x3C
#define ERROR_CODE_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE 0x3D
#define ERROR_CODE_CONNECTION_FAILED_TO_BE_ESTABLISHED     0x3E
#define ERROR_CODE_MAC_CONNECTION_FAILED                   0x3F
#define ERROR_CODE_COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_TO_ADJUST_USING_CLOCK_DRAGGING 0x40


#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GAP_APPEARANCE_VALUE_HANDLE 0x0005
#define ATT_CHARACTERISTIC_GATT_SERVER_CHANGE_VALUE_HANDLE 0x000E
#define ATT_CHARACTERISTIC_GATT_SERVER_CHANGE_CLIENT_CFG_HANDLE 0x000F

#define GAP_PERIOD_MS    1000

typedef struct advertising_report
{
    uint8   type;
    uint8   event_type;
    uint8   address_type;
    bd_addr_t address;
    uint8   rssi;
    uint8   length;
    uint8 * data;
} advertising_report_t;
#define MAX_SCAN_RESULT 6
typedef  struct
{
    uint8 Event_Type;
    uint8 Address_Type; //00 public  01 random
    bd_addr_t Address;
    uint8 Length;//0x00 - 0x1F
    uint8 data[32];
    char   rssi;
} bt_gap_adv_rpt;


typedef  struct
{
    uint8 adv_rpt_cnt;
    bt_gap_adv_rpt  adv_rpt[MAX_SCAN_RESULT];
} bt_gap_scan_result_t;


extern void st_init(void);

#endif
