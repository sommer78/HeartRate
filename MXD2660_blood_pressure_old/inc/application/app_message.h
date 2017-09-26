/**
 *****************************************************************************
 * @file     app_message.h
 * @author   Richard
 * @version  V1.0.0
 * @date     20-Sep-2016
 * @brief    all application message definition
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

/**
 * @addtogroup Application
 * @{
 * @defgroup app_message app_message
 * @{
 */

#ifndef __APP_MESSAGE_H__
#define __APP_MESSAGE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "system_config.h"

/**
 * @name global variable related definition
 * @{
 */


#define MSG_MAX_NUM                 (10)
#define WRISTBAND_MSG_MAX_NUM       (10)


typedef struct
{
    uint8 sub_type;
    uint8 command;
} PERIPHERAL_TYPE;

/**
 * @brief  peripheral  message structure definition
 */
typedef struct
{
    uint8 msg_module_type;
    union
    {
        uint8 hid_key[2];          /**<hid keyboard*/
        PERIPHERAL_TYPE st_peri;
    };
    uint8 length;
    void *p;
} APP_PERIPHERAL_MSG;

/**
 * @brief  BLE message structure definition
 */
typedef struct
{
    uint8 msg_module_type;
    uint8 command;
    uint8 connect_handle;
    uint8 length;
    void *p;
} BLE_TYPE_MSG;

/**
 * @brief  HCI message structure definition
 */
typedef struct
{
    uint8 msg_module_type;
    uint8 command;
    uint8 reserved;
    uint8 length;
    void *p;
} HCI_TYPE_MSG;

/**
 * @brief  message type definition
 */
typedef enum
{
    MSG_KEYBOARD = 1,
    MSG_MOUSE,
    MSG_BLE,
    MSG_APP,                /**<message come from app task*/
    MSG_HCI,
    MSG_WRISTBAND,
} MSG_MODULE_TYPE;

/**
 * @brief  peripheral sub type definition
 */
typedef enum
{
    //wristband SUB TYPE peripheral
    MSG_WB_TOUCH = 1,
    MSG_WB_3DH = 2,
    MSG_WB_LCM = 3,
    MSG_WB_PROFILE = 4,

} WB_SUB_TYPE;

/**
 * @brief  peripheral msg type definition
 */
typedef enum
{
    //wristband msg
    MSG_WB_SCREEN_UP = 1,
    MSG_WB_SCREEN_OFF = 2,

} WB_MSG_COMMAND;

/**
 * @brief  ble to app message type definition
 */
typedef enum
{
    //ADV MSG command connection
    MSG_BLE_RESET = 0,
    MSG_BLE_ADVERTISE_ENABLE_REQ,
    MSG_BLE_ADVERTISE_DISABLE_REQ,

    MSG_BLE_SCAN_ENABLE_REQ,
    MSG_BLE_SCAN_DISABLE_REQ,
    MSG_BLE_SCAN_REPORT_IND,
    MSG_BLE_APPEND_WHITELIST_REQ,
    MSG_BLE_REMOVE_WHITELIST_REQ=7,

    MSG_BLE_INIT_REQ,
    MSG_BLE_TERMINATE_REQ,
    MSG_BLE_CREATE_CONNCT_CANCEL_REQ,
    MSG_BLE_LINKLAYER_VERSION_REQ , 
        MSG_BLE_LINKLAYER_LONG_PACKET_REQ ,
        MSG_BLE_LINKLAYER_1M2M_PHY_UPDATE , 
        MSG_BLE_LONG_TERM_KEY_REQ_REPLY ,    
        MSG_BLE_LONG_TERM_KEY_REQ_NEGATIVE_REPLY ,    
        MSG_BLE_MTU_EXCHANGE ,    
MSG_BLE_WAKEUP_DURING_LATENCY,
    //connection MSG command definition
    MSG_BLE_RELEASE_A_TX_BUF_IND = 18,
    MSG_BLE_HIGH_DUTY_ADV_END_IND = 19,
    MSG_BLE_CONNECTED_IND = 20,
    MSG_BLE_DISCONNECTED_IND = 21,
    MSG_BLE_CONNECTION_UPD_REQ = 22,
    MSG_BLE_ENCRYPTED_IND = 23,
    MSG_BLE_UNENCRYPTED_IND = 24,
    
    MSG_BLE_PAIR_PARA_SET_REQ = 25,
    MSG_BLE_PARA_CHANGE = 26,
    MSG_BLE_SECURITY_REQ = 28,

    MSG_GATT_CHANGE_MTU,    //?
    MSG_GATT_CLIENT_INIT,
    MSG_GATT_CLIENT_DIS_PRI_SERVS,
    MSG_GATT_CLIENT_DIS_PRI_SERVS_U16,
    MSG_GATT_CLIENT_DIS_PRI_SERVS_U128,
    MSG_GATT_CLIENT_DIS_CHAR_FOR_SERV,
    MSG_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U16,
    MSG_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U128,
    MSG_GATT_CLIENT_DIS_CHAR_DESCRIPTOR,
    MSG_GATT_CLIENT_FIND_INCLUDED_SERV,
    MSG_GATT_CLIENT_RD_CHAR_VAL_BY_HANDLE,
    MSG_GATT_CLIENT_RD_CHAR_VAL_BY_U16,
    MSG_GATT_CLIENT_RD_CHAR_VAL_BY_U128,
    MSG_GATT_CLIENT_RD_LONG_CHAR_VAL_BY_HANDLE_OFFSET,
    MSG_GATT_CLIENT_WR_CHAR_VAL_WITHOUT_RESP,
    MSG_GATT_CLIENT_WR_CHAR_VAL,
    MSG_GATT_CLIENT_WR_LONG_CHAR_VAL_OFFSET,
    MSG_GATT_SERVER_SEND_INDICATION,
    MSG_GATT_SERVER_SEND_NOTIFICATION,
    MSG_BLE_ANCS_INQUIRY_RSLT,
    MSG_BLE_ANCS_RPT_REPORT_IND,
    MSG_BLE_ANCS_SURCE_CHAR_RPT,    

    MSG_DEVICE_CONFIG_SET_REQ,
    MSG_DEVICE_CONFIG_SET_RSP,
    MSG_DEVICE_CONFIG_APPEARANCE_GET_REQ,
    MSG_DEVICE_CONFIG_APPEARANCE_GET_RSP,

    MSG_USER_PASSKEYREQ_IND,
    MSG_USER_PASSKEYREQ_CONF,
    MSG_USER_PASSKEY_DISPLAY_IND,
    MSG_USER_PASSKEY_DISPLAY_CANCEL_IND,
    MSG_REMOTE_OOB_DATA_REQ_IND,
    MSG_AUTHRESULT_IND,
    MSG_AUTHRESULT_REQ_IND,
    MSG_LL_CONNECTION_PARA_INFO,
    MSG_LL_PRIVACY_MODE_REQ,
    MSG_LL_PRIVACY_MODE_RSP,
    MSG_LL_CONN_UPDATE_COMPLETE_EVENT,
    MSG_LL_CONN_PKT_EVENT_COMPLETE,
    MSG_NOTIFY_DATA_REPORT,
    MSG_MTU_EVENT_REPORT,
    MSG_SET_TXPOWER_REQ,
    MSG_SET_TXPOWER_RSP,
    MSG_SET_ADDRESS_REQ,
    MSG_SET_RANDOM_ADDRESS_RSP,
    MSG_KEYPRESS_NOTIFICATION,
    MSG_PAIR_COMPLETED,
    MSG_AUTHORIZATION_SET,
    MSG_BOND_DECLINE,
    MSG_AES_RSLT_RPT,
    MSG_PAIR_FAIL,
    MSG_LTK_REQ_WHEN_RECONECT_AFTER_PAIR,
    MSG_BLE_LL_VERSION_IND,
    MSG_BLE_ADV_STATE_CHANGE_IND,    
    MSG_BLE_SCAN_STATE_CHANGE_IND,    
    MSG_BLE_TEMP_CHG_LATENCY,


//    MSG_DATA_LENGTH_CHANGE_INFO,
} BLE_MSG_COMMAND;

/**
 * @brief  HCI MSG command definition
 */
typedef enum
{
    MSG_HCI_WRITE_SCAN_PARAM = 1,
    MSG_HCI_WRITE_SCAN_ENABLE,
    MSG_HCI_RESET,
    MSG_HCI_SET_ADV_PARAM,
    MSG_HCI_SET_ADV_STOP,
    MSG_HCI_WRITE_SCAN_DISABLE,
    MSG_HCI_CREATE_CONNECTION,
    MSG_HCI_DISCONNT,
    MSG_HCI_CREATE_CONNECTION_CANCEL,
    MSG_HCI_CONNECTION_UPDATE,
    MSG_HCI_SM_PARA_SET,
    MSG_HCI_GATT_CLIENT_INIT,
    MSG_HCI_GATT_CLIENT_DIS_PRI_SERVS,
    MSG_HCI_GATT_CLIENT_DIS_PRI_SERVS_U16,
    MSG_HCI_GATT_CLIENT_DIS_PRI_SERVS_U128,
    MSG_HCI_GATT_CLIENT_DIS_CHAR_FOR_SERV,
    MSG_HCI_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U16,
    MSG_HCI_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U128,
    MSG_HCI_GATT_CLIENT_DIS_CHAR_DESCRIPTOR,
    MSG_HCI_GATT_CLIENT_FIND_INCLUDED_SERV,
    MSG_HCI_GATT_CLIENT_RD_CHAR_VAL_BY_HANDLE,
    MSG_HCI_GATT_CLIENT_RD_CHAR_VAL_BY_U16,
    MSG_HCI_GATT_CLIENT_RD_CHAR_VAL_BY_U128,
    MSG_HCI_GATT_CLIENT_RD_LONG_CHAR_VAL_BY_HANDLE_OFFSET,
    MSG_HCI_GATT_CLIENT_WR_CHAR_VAL_WITHOUT_RESP,
    MSG_HCI_GATT_CLIENT_WR_CHAR_VAL,
    MSG_HCI_GATT_CLIENT_WR_LONG_CHAR_VAL_OFFSET,
    MSG_HCI_GATT_SERVER_SEND_INDICATION,
    MSG_HCI_GATT_SERVER_SEND_NOTIFICATION,
    MSG_HCI_USER_DEFINE,
    MSG_HCI_MXD_SPECIFIED_REPORT,
    MSG_HCI_SECURITY_REQ,
    MSG_HCI_PASSKEY_VALUE,



} HCI_MSG_COMMAND;



/**
 * @}
 */

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif


/**
 * @}
 * @} //end module
 */



