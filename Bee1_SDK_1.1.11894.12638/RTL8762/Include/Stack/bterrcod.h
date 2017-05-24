/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     bterrcod.h
* @brief    SubCause error codes delivered by BlueAPI.
* @details   Highbyte defines Layer (BT_ERR_SRC_MASK)
*            Lowbyte defines Error Code within Layer.
* @author   ranhui & jane
* @date     2015-10-29
* @version  v1.2
*************************************************************************************************************
*/

#ifndef __BTERRCOD_H
#define __BTERRCOD_H

/** @addtogroup Sub_Cause SubCause Error Code
  * @{
  */

/** @defgroup highbyte_Cause Highbyte Error Codes
  * @brief The SubCause is defined as uint16_t with the MSB defining the layer of the cause.  
  * @{
  */

#define BLUEFACE_ERR_SRC_MASK             0xFF00 /**< @brief  Error Mask (Highbyte) to indicate Error Code */


#define BLUEFACE_ERR                0x000 /**< @brief BlueFace layer  */

#define HCI_ERR                     0x100 /**< @brief HCI layer */
#define L2CAP_ERR                   0x200 /**< @brief L2CAP layer */
#define ATT_ERR                     0x700 /**< @brief ATT layer */
#define SECMAN_ERR                  0xB00 /**< @brief SECMAN layer */
#define GATT_ERR                    0xC00 /**< @brief GATT layer */
#define APP_ERR                    0xD00 /**< @brief GATT layer */

#define BLUESECURE_ERR             0x7000  /**< @brief BlueSecure Error codes - above BlueFace   */
/** @} End of highbyte_Cause */

/** @defgroup Blueface_Error Blueface Error Codes
  * @brief Blueface Error Codes
  * @{
  */
#define BF_SUCCESS                              0x00
#define BF_ILL_VERSION                          0x01
#define BF_ILL_APPLICATION                      0x02
#define BF_NO_RESOURCES                         0x04
#define BF_ILL_PARAMETER                        0x05
#define BF_BUSY                                 0x08
#define BF_PSM_NOT_IMPLEMENTED                  0x0B
/** @} End of Blueface_Error */

/************************************************************************************/
/** @defgroup HCI_Error HCI Error Codes
  * @brief HCI Error Codes
  * @{
  */
#define HCI_SUCCESS                             0x00
#define HCI_ERR_UNKNOWN_COMMAND                 0x01
#define HCI_ERR_NOCONNECTION                    0x02
#define HCI_ERR_HARDWARE_FAIL                   0x03
#define HCI_ERR_PAGE_TIMEOUT                    0x04
#define HCI_ERR_AUTHENTICATION_FAILED           0x05
#define HCI_ERR_KEY_MISSING                     0x06
#define HCI_ERR_MEMORY_FULL                     0x07
#define HCI_ERR_CONNECTION_TIMEOUT              0x08
#define HCI_ERR_MAX_NUM_CONNECTIONS             0x09
#define HCI_ERR_MAX_NUM_SCO_CONNECTIONS         0x0A  /* 10 */
#define HCI_ERR_ACL_CONN_ALREADY_EXISTS         0x0B  /* 11 */
#define HCI_ERR_COMMAND_DISALLOWED              0x0C  /* 12 */
#define HCI_ERR_HOST_REJECTED_0D                0x0D  /* 13 */
#define HCI_ERR_REJECT_LIMITED_RESOURCES HCI_ERR_HOST_REJECTED_0D
#define HCI_ERR_HOST_REJECTED_0E                0x0E  /* 14 */
#define HCI_ERR_REJECT_SECURITY_REASONS HCI_ERR_HOST_REJECTED_0E
#define HCI_ERR_HOST_REJECTED_0F                0x0F  /* 15 */
#define HCI_ERR_REJECT_UNACCEPTABLE BD_ADDR HCI_ERR_HOST_REJECTED_0F
#define HCI_ERR_HOST_TIMEOUT                    0x10  /* 16 */
#define HCI_ERR_UNSUPPORTED_PARAM_VALUE         0x11  /* 17 */
#define HCI_ERR_INVALID_HCI_PARAMETER_VALUE     0x12  /* 18 */
#define HCI_ERR_OTHER_END_TERMINATE_13          0x13  /* 19 */
#define HCI_ERR_OTHER_END_TERMINATE_14          0x14  /* 20 */
#define HCI_ERR_OTHER_END_TERMINATE_15          0x15  /* 21 */
#define HCI_ERR_CONNECTION_TERMINATE_LOCALLY    0x16  /* 22 */
#define HCI_ERR_REPEATED_ATTEMPTS               0x17  /* 23 */
#define HCI_ERR_PARING_NOT_ALLOWED              0x18  /* 24 */
#define HCI_ERR_UNKNOWN_LMP_PDU                 0x19  /* 25 */
#define HCI_ERR_UNSUPPORTED_REMOTE_FEATURE      0x1A  /* 26 */
#define HCI_ERR_SCO_OFFSET_REJECTED             0x1B  /* 27 */
#define HCI_ERR_SCO_INTERVAL_REJECTED           0x1C  /* 28 */
#define HCI_ERR_SCO_AIR_MODE_REJECTED           0x1D  /* 29 */
#define HCI_ERR_INVALID_LMP_PARAMETERS          0x1E  /* 30 */
#define HCI_ERR_UNSPECIFIED_ERROR               0x1F  /* 31 */
#define HCI_ERR_UNSUPPORTED_LMP_PARAMETER_VAL   0x20  /* 32 */
#define HCI_ERR_ROLE_CHANGE_NOT_ALLOWED         0x21  /* 33 */
#define HCI_ERR_LMP_RESPONSE_TIMEOUT            0x22  /* 34 */
#define HCI_ERR_LMP_ERROR_TRANSACTION_COLLISION 0x23  /* 35 */
#define HCI_ERR_LMP_PDU_NOT_ALLOWED             0x24  /* 36 */
/* the next ones are new in 1.1 */
#define HCI_ERR_ENCRYPTION_MODE_NOT_ACCEPTABLE  0x25  /* 37 */
#define HCI_ERR_UNIT_KEY_USED                   0x26  /* 38 */
#define HCI_ERR_QOS_NOT_SUPPORTED               0x27  /* 39 */
#define HCI_ERR_INSTANT_PASSED                  0x28  /* 40 */
#define HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPP  0x29  /* 41 */

#define HCI_ERR_DIFFERENT_TRANSACTION_COLLISION 0x2A  /* 42 */
#define HCI_ERR_QOS_UNACCEPTABLE_PARAMETER      0x2C  /* 44 */
#define HCI_ERR_QOS_REJECT                      0x2D  /* 45 */
#define HCI_ERR_CHANNEL_ASSESSMENT_NOT_SUPPORTED      0x2E  /* 46 */
#define HCI_ERR_INSUFFICIENT_SECURITY           0x2F  /* 47 */
#define HCI_ERR_PARAMETER_OUT_OF_MANDATORY      0x30  /* 48 */
#define HCI_ERR_ROLE_SWITCH_PANDING             0x32  /* 50 */
#define HCI_ERR_RESERVED_SLOT_VIOLATION         0x34  /* 52 */
#define HCI_ERR_ROLE_SWITCH_FAILED              0x35  /* 53 */
#define HCI_ERR_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE   0x36   /* 54 */
#define HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST  0x37   /* 55 */
#define HCI_ERR_HOST_BUSY_PAIRING               0x38  /* 56 */
#define HCI_ERR_CONNECTION_REJECTED_DUE_TO_NOT_SUITABLE_CHANNEL_FOUND 0x39  /* 57 */
#define HCI_ERR_CONTROLLER_BUSY                 0x3A  /* 58 */
#define HCI_ERR_UNACCEPTABLE_CONNECTION_INTERVAL          0x3B  /* 59 */
#define HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT              0x3C  /* 60 */
#define HCI_ERR_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE  0x3D  /* 61 */
#define HCI_ERR_CONNECTION_FAILED_TO_BE_ESTABLISHED       0x3E  /* 62 */
#define HCI_ERR_MAC_CONNECTION_FAILED                     0x3F  /* 63 */

/*HCI locally generated Error Codes */
#define HCI_ERR_ILLEGAL_HANDLE                0x64  /* 100 */
#define HCI_ERR_TIMEOUT                       0x65  /* 101 */
#define HCI_ERR_OUTOFSYNC                     0x66  /* 102 */
#define HCI_ERR_NO_DESCRIPTOR                 0x67  /* 103 */
#define HCI_ERR_INFOPAGE_MISSING              0x68  /* 104 */
/** @} End of HCI_Error */

/** @defgroup L2CAP_Error L2CAP Error Codes
  * @brief L2CAP Error Codes
  * @{
  */
#define L2CAP_NO_CAUSE                        0x00
#define L2CAP_CONNECTION_ACCEPT               0x00
#define L2CAP_ERR_PENDING                     0x01
#define L2CAP_ERR_REFUS_INV_PSM               0x02
#define L2CAP_ERR_REFUS_SEC_BLOCK             0x03
#define L2CAP_ERR_REFUS_NO_RESOURCE           0x04
#define L2CAP_ERR_ILLEGAL_PARAMETER           0x05

#define L2CAP_ERR_INSUFFICIENT_AUTHENTICATION 0x10
#define L2CAP_ERR_INSUFFICIENT_AUTHORIZATION  0x11
#define L2CAP_ERR_INSUFFICIENT_KEY_SIZE       0x12
#define L2CAP_ERR_INSUFFICIENT_ENCRYPTION     0x13
#define L2CAP_ERR_CREDITS_EXCEED_RANGE        0x14
#define L2CAP_ERR_INVAILD_PDU                 0x15
#define L2CAP_ERR_CREDITS_LACK                0x16
#define L2CAP_ERR_NO_RESOURCE                 0x17
#define L2CAP_ERR_BUSY                        0x18
#define L2CAP_ERR_LE_CHANNEL_NOT_OPEN         0x19
#define L2CAP_ERR_ILLEGAL_STATE               0x20
#define L2CAP_ERR_LINK_NOT_EXIST              0x21
#define L2CAP_ERR_CMD_NOT_UNDERSTOOD          0x22
#define L2CAP_ERR_QUEUE_IS_FULL               0x23
#define L2CAP__ERR_INVAlID_SOURCE_CID         0x24
#define L2CAP__ERR_SOURCE_CID_ALREADY_ALLOCATED  0x25



#define L2CAP_ERR_OUT_OF_RANGE                0xED

#define L2CAP_ERR_TIMEOUT_EXTERNAL            0xEE
/** @} End of L2CAP_Error */

/** @defgroup ATT_Error ATT Error Codes
  * @brief ATT Error Codes (see ATT spec. table 3.3)
  * @{
  */
#define ATT_OK                              0    /**< @brief internal value . */
#define ATT_ERR_INVALID_HANDLE              0x01 /**< @brief Attribute handle value given was not valid on this attribute server */
#define ATT_ERR_READ_NOT_PERMITTED          0x02 /**< @brief Attribute cannot be read */
#define ATT_ERR_WRITE_NOT_PERMITTED         0x03 /**< @brief Attribute cannot be written */
#define ATT_ERR_INVALID_PDU                 0x04 /**< @brief The attribute PDU was invalid */
#define ATT_ERR_INSUFFICIENT_AUTHENTICATION 0x05 /**< @brief The attribute requires authentication before it can be read or written */
#define ATT_ERR_UNSUPPORTED_REQUEST         0x06 /**< @brief Attribute server doesn't support the request received from the attribute client */
#define ATT_ERR_INVALID_OFFSET              0x07 /**< @brief Offset specified was past the end of the attribute */
#define ATT_ERR_INSUFFICIENT_AUTHORIZATION  0x08 /**< @brief The attribute requires an authorization before it can be read or written */
#define ATT_ERR_PREPARE_QUEUE_FULL          0x09 /**< @brief Too many prepare writes have been queued */
#define ATT_ERR_ATTR_NOT_FOUND              0x0A /**< @brief No attribute found within the given attribute handle range */
#define ATT_ERR_ATTR_NOT_LONG               0x0B /**< @brief Attribute cannot be read or written using the Read Blob Request or Prepare Write Request */
#define ATT_ERR_INSUFFICIENT_KEY_SIZE       0x0C /**< @brief The Encryption Key Size used for encrypting this link is insufficient */
#define ATT_ERR_INVALID_VALUE_SIZE          0x0D /**< @brief The attribute value length is invalid for the operation */
#define ATT_ERR_UNLIKELY                    0x0E /**< @brief The attribute request that was requested has encountered an error that was very unlikely, and therefore could not be completed as requested */
#define ATT_ERR_INSUFFICIENT_ENCRYPTION     0x0F /**< @brief The attribute requires encryption before it can be read or written */
#define ATT_ERR_UNSUPPORTED_GROUP_TYPE      0x10 /**< @brief The attribute type is not a supported grouping attribute as defined by a higher layer specification */
#define ATT_ERR_INSUFFICIENT_RESOURCES      0x11 /**< @brief Insufficient Resources to complete the request */

/* profile dependent application error codes >= 0x80: */
#define ATT_ERR_MIN_APPLIC_CODE          0x80 /**< @brief minimal application error code */
#define ATT_ERR_INVALID_VALUE            0x80 /**< @brief The attribute value is invalid for the operation */


/* manufacturer specific error codes that are "missing" in GATT spec. >= 0xC0:   */
#define ATT_ERR_INVALID_CCC_BITS         0xC0 /**< @brief invalid client char. config. bits */


/* error codes common to various profiles (see "CSS v2.pdf"), >= 0xE0*/
#define ATT_ERR_CCCD_IMPROPERLY_CONFIGURED  0xFD
#define ATT_ERR_PROC_ALREADY_IN_PROGRESS    0xFE
#define ATT_ERR_OUT_OF_RANGE                0xFF
/** @} End of ATT_Error */

/** @defgroup GATT_Error GATT Error Codes
  * @brief GATT Error Codes 
  * @{
  */
#define GATT_SUCCESS                        0x00
#define GATT_ERR_OUT_OF_RESOURCE            (GATT_ERR | 0x01) /**< @brief Insufficient internal resources. */
#define GATT_ERR_UNSUPPORTED                (GATT_ERR | 0x02) /**< @brief The APP submitted an unsupported request. */
#define GATT_ERR_ILLEGAL_ROLE               (GATT_ERR | 0x03) /**< @brief The APP submitted a request that causes a conflict with the current Bluetooth stack GATT module role (e.g. a GATTAttributeUpdateReq without having registered a GATT service). */
#define GATT_ERR_ILLEGAL_STATE              (GATT_ERR | 0x04) /**< @brief The APP submitted a request that is not allowed in the current state. */
#define GATT_ERR_ILLEGAL_CODING             (GATT_ERR | 0x05) /**< @brief The registered GATT service comprises attributes with an illegal coding. */
#define GATT_ERR_ILLEGAL_HANDLE             (GATT_ERR | 0x06) /**< @brief An internal message contained an illegal handle. */
#define GATT_ERR_ILLEGAL_PARAMETER          (GATT_ERR | 0x07) /**< @brief The APP submitted a request with an illegal parameter. */
#define GATT_ERR_INTERNAL                   (GATT_ERR | 0x08) /**< @brief An internal error occurred. */
#define GATT_ERR_NOT_ALLOWED                (GATT_ERR | 0x09) /**< @brief A GATTAttributeUpdateReq was attempted for a characteristic value that cannot be indicated or notified. */
#define GATT_ERR_NOTIF_IND_NOT_CFG          (GATT_ERR | 0x0A) /**< @brief A GATTAttributeUpdateReq was attempted for an attribute value that is currently not configured for indications or notifications (GATT Client Characteristic Configuration does not exist or is improperly configured). */
#define GATT_ERR_NOTIF_IND_CFG              (GATT_ERR | 0x0B) /**< @brief A GATTAttributeUpdateReq was attempted while no connection exists, but in the Bluetooth stacks internal bonding information table devices are found that configured the characteristic value for indications or notifications. */
#define GATT_ERR_NOTIF_IND_CONF_PD          (GATT_ERR | 0x0C) /**< @brief A characteristic value indication was successfully initiated while a connection exists and the confirmation of the remote device is pending. */
#define GATT_ERR_TIMEOUT                    (GATT_ERR | 0x0D) /**< @brief A timeout occurred during an ATT transaction or GATT procedure. */
#define GATT_ERR_LINK_DEACTIVATED           (GATT_ERR | 0x0E) /**< @brief The APP submitted a request on a link where a timeout has previously occurred. The link must be reestablished before transactions can be performed. */
#define GATT_ERR_NOT_AUTHENTICATED          (GATT_ERR | 0x0F) /**< @brief A GATTAttributeUpdateReq was attempted whele no authenticated. */
/** @} End of GATT_Error */

/** @defgroup SECMAN_Error SECMAN Error Codes
  * @brief SECMAN Error Codes
  * @{
  */
#define SECMAN_SUCCESS                        0
#define SECMAN_ERR_NOENTRY                    1      /**< @brief tried to remove entry, but entry was not found / identified */
#define SECMAN_ERR_DBFULL                     2      /**< @brief tried to insert table of security settings is full */
#define SECMAN_ERR_INVALID_PARAMETER          3      /**< @brief tried to use invalid parameters in reqmessage */
#define SECMAN_ERR_LINKKEY_NOT_SUFFICIENT     4      /**< @brief linkkey is not sufficient for this connection */
#define SECMAN_ERR_LE_BD_NOT_RESOLVED         5      /**< @brief random resolvable address not found */
/** @} End of SECMAN_Error */

/** @defgroup BlueSecure_Error BlueSecure Error Codes
  * @brief BlueSecure Error Codes
  * @{
  */
#define BTSEC_SUCCESS                          0
#define BTSEC_ERR_UNSPECIFIED                  (BLUESECURE_ERR| 1)
#define BTSEC_ERR_ILLEGAL_COMMAND              (BLUESECURE_ERR| 2)
#define BTSEC_ERR_ACTION_FAILED                (BLUESECURE_ERR| 3)
#define BTSEC_ERR_INVALID_PARAMETER            (BLUESECURE_ERR| 4)
#define BTSEC_ERR_BUSY                         (BLUESECURE_ERR| 5)
#define BTSEC_ERR_RESOURCE_ERROR               (BLUESECURE_ERR| 6)
#define BTSEC_ERR_FEATURE_NOT_IMPLEMENTED      (BLUESECURE_ERR| 7)
#define BTSEC_ERR_READ_ONLY                    (BLUESECURE_ERR| 8)
#define BTSEC_ERR_VOTING_FAILED                (BLUESECURE_ERR| 9)
#define BTSEC_ERR_ITEM_NOT_FOUND               (BLUESECURE_ERR|10)
#define BTSEC_ERR_ITEM_EXISTS                  (BLUESECURE_ERR|11)
/** @} End of BlueSecure_Error */

/** @defgroup APP_Error APP Error Codes
  * @brief APP Error Codes
  * @{
  */
#define APP_SUCCESS                             0
#define APP_ERR_PENDING                         1      /**< @brief app procedure pending */
#define APP_ERR_HAVE_CONFIRM                    2      /**< @brief confirm has done by app */
#define APP_ERR_ACCEPT                          3      /**< @brief accept some request from lower layer */
#define APP_ERR_REJECT                          4      /**< @brief reject some request from lower layer */

/** @defgroup TAppResult TAppResult
  * @brief  profile procedure application return results
  * @{
  */
typedef enum
{
    AppResult_Success         = (APP_SUCCESS),
    AppResult_Pending         = (APP_ERR | APP_ERR_PENDING),
    //for prepare write & execute write
    AppResult_PreWrQueueFull  = (ATT_ERR | ATT_ERR_PREPARE_QUEUE_FULL),
    AppResult_AplicationErr   = (ATT_ERR | ATT_ERR_MIN_APPLIC_CODE),
    AppResult_InvalidOffset   = (ATT_ERR | ATT_ERR_INVALID_OFFSET),
    AppResult_InvalidValueSize = (ATT_ERR | ATT_ERR_INVALID_VALUE_SIZE),
    AppResult_InvalidParameter = (ATT_ERR | ATT_ERR_INVALID_PDU),
    AppResult_HaveConfirm      = (APP_ERR| APP_ERR_HAVE_CONFIRM),
    AppResult_Accept           = (APP_ERR| APP_ERR_ACCEPT),
    AppResult_Reject           = (APP_ERR| APP_ERR_REJECT),
    AppResult_UnknownError
} TAppResult;
/** @} End of TAppResult */
/** @} End of APP_Error */

/** @} End of Sub_Cause */



#endif
