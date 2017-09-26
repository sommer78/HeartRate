/**
****************************************************************************
* @file     : hci_app.h
* @brief        :
* @author   : xiaoyu
* @version  : Initial Draft
* @date     : 2016/11/16
* @history  :
* @note     :
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

/** @defgroup  APPLICATION hci app
* @{
*/

#ifndef __HCI_APP_H__
#define __HCI_APP_H__

#include "system_config.h"
#include "app_message.h"
#include "hci.h"


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "system_config.h"
/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

typedef struct
{
    uint16 ogf6ocf10;
    uint8  length;
}   hci_cmd_header_t;



typedef struct
{
    uint8 event;
    uint8  length;
}   hci_event_header_t;

typedef struct
{
    union
    {
        hci_cmd_header_t hci_cmd_header;
        hci_acl_header_t hci_acl_header;
    } header;
    uint8 type;
}    hci_rx_header_t;

//peri buffer/event/command buffer
typedef struct
{
    uint16 wr;
    uint16 rd;
    uint8 * data_ptr;
} comm_fifo_mgr;
extern comm_fifo_mgr Peri_Data;
extern comm_fifo_mgr Event_Data;
extern uint32 g_peri_ctrl;

enum HCI_STATE
{
    HCI_STATE_RX_TYPE = 0,
    HCI_STATE_RX_COMMAND_OPCODE1,
    HCI_STATE_RX_COMMAND_OPCODE2,
    HCI_STATE_RX_COMMAND_LENGTH,
    HCI_STATE_RX_ACL_HEAD1,
    HCI_STATE_RX_ACL_HEAD2,
    HCI_STATE_RX_ACL_LENGTH1,
    HCI_STATE_RX_ACL_LENGTH2,
    HCI_STATE_RX_DATA_START,
    HCI_STATE_RX_DATA_CONT
};

#define HCI_POOL_SIZE (PERI_DATA_SIZE + EVENT_DATA_SIZE)

#ifdef HCI_FUNC_EN
    #define HCI_RX_TSK_SIZE 700
    #define HCI_TX_TSK_SIZE 300
#else
    #define HCI_RX_TSK_SIZE 0
    #define HCI_TX_TSK_SIZE 0
#endif

/** @} */

#ifdef HCI_FUNC_EN
    #define PERI_DATA_SIZE 1024
    #define EVENT_DATA_SIZE 256
#else
    #define PERI_DATA_SIZE 0//1024
    #define EVENT_DATA_SIZE 0//256
#endif


/*
 *  Opcode Group Fields for the HCI Command Groups
 */
#define LINK_CONTROL_OGF                0x01
#define LINK_POLICY_OGF                 0x02
#define HOST_CONTROL_OGF                0x03
#define LOCAL_INFO_OGF                  0x04
#define LOCAL_STATUS_OGF                0x05
#define TEST_COMMANDS_OGF               0x06
#define LE_CONTROLLER_OGF               0x08
#define EXTENDED_OGF                        0x3F

/*
 *  HCI link policy command opcodes - Opcode group subfield = 0x02
 */
#define LC_G    (LINK_CONTROL_OGF << 10)

#define HCI_DISCONNECT                                  (0x0006 + LC_G) //0x0406
#define HCI_READ_REMOTE_VER_INFO             (0x001D + LC_G) //0x041d



/***********************************************
 * Host Controller and Baseband command opcode
 *  - Opcode group subfield = 0x03
 ***********************************************/

#define HC_G    (HOST_CONTROL_OGF << 10)

#define HCI_SET_EVENT_MASK                              (0x0001 + HC_G) //0x0c01
#define HCI_RESET                                       (0x0003 + HC_G) //0x0c03


#define HCI_READ_TRANSMIT_POWER_LEVEL                   (0x002D + HC_G)//0x0c2d

#define HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL    (0x0031 + HC_G)//0x0c31
#define HCI_HOST_BUFFER_SIZE                            (0x0033 + HC_G)//0x0c33
#define HCI_HOST_NUMBER_OF_COMPLETED_PACKETS            (0x0035 + HC_G)//0x0c35

#define HCI_SET_EVENT_MASK_PAGE_2            (0X0063 + HC_G)//0x0c63

#define HCI_READ_LE_HOST_SUPPORT                        (0X006C + HC_G)//0x0c6c
#define HCI_WRITE_LE_HOST_SUPPORT                       (0X006D + HC_G)//0x0c6d

#define HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT (0x007B + HC_G)//0x0c7b
#define HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT (0x007C + HC_G)//0x0c7c



/***********************************************
 *  HCI Informaional parameters command opcode
 *  - Opcode group subfield = 0x04
 ***********************************************/

#define HI_G    (LOCAL_INFO_OGF << 10)

#define HCI_READ_LOCAL_VER_INFO                         (0x0001 + HI_G)//0x1001
#define HCI_READ_LOCAL_SUPPORTED_COMMANDS                           (0x0002 + HI_G)//0x1002
#define HCI_READ_LOCAL_SUPPORTED_FEATURES                         (0x0003 + HI_G)//0x1003
#define HCI_READ_BUFFER_SIZE                            (0x0005 + HI_G)//0x1005
#define HCI_READ_BD_ADDR                                (0x0009 + HI_G)//0x1009


/***********************************************
 * HCI Status parameters command opcode
 *  - Opcode group subfield = 0x05
 ***********************************************/

#define HS_G    (LOCAL_STATUS_OGF << 10)

#define HCI_READ_RSSI                                   (0x0005 + HS_G)//0x1405


#define LE_G    (LE_CONTROLLER_OGF << 10)

#define HCI_LE_SET_EVENT_MASK                          (0x0001 + LE_G)//0x2001  
#define HCI_LE_READ_BUFFER_SIZE                        (0x0002 + LE_G)//0x2002  
#define HCI_LE_READ_LOCAL_SUPPORTED_FEATURES           (0x0003 + LE_G)//0x2003  
#define HCI_LE_SET_RANDOM_ADDRESS                      (0x0005 + LE_G)//0x2005
#define HCI_LE_SET_ADVERTISING_PARAMETERS              (0x0006 + LE_G)  //0x2006
#define HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER       (0x0007 + LE_G)  //0x2007
#define HCI_LE_SET_ADVERTISING_DATA                    (0x0008 + LE_G)  //0x2008
#define HCI_LE_SET_SCAN_RESPONSE_DATA                  (0x0009 + LE_G)//0x2009
#define HCI_LE_SET_ADVERTISE_ENABLE                    (0x000A + LE_G)  //0x200a
#define HCI_LE_SET_SCAN_PARAMETERS                     (0x000B + LE_G)//0x200b  
#define HCI_LE_SET_SCAN_ENABLE                         (0x000C + LE_G)//0x200c
#define HCI_LE_CREATE_CONNECTION                       (0x000D + LE_G)//0x200d  
#define HCI_LE_CREATE_CONNECTION_CANCEL                (0x000E + LE_G)//0x200e
#define HCI_LE_READ_WHITE_LIST_SIZE                    (0x000F + LE_G)//0x200F
#define HCI_LE_CLEAR_WHITE_LIST                        (0x0010 + LE_G)//0x2010
#define HCI_LE_ADD_DEVICE_TO_WHITE_LIST                (0x0011 + LE_G)//0x2011
#define HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST           (0x0012 + LE_G)//0x2012
#define HCI_LE_CONNECTION_UPDATE                       (0x0013 + LE_G)//0x2013
#define HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION         (0x0014 + LE_G)//0x2014
#define HCI_LE_READ_CHANNEL_MAP                        (0x0015 + LE_G)//0x2015
#define HCI_LE_READ_REMOTE_USED_FEATURES               (0x0016 + LE_G)  //0x2016
#define HCI_LE_ENCRYPT                                 (0x0017 + LE_G)  //0x2017
#define HCI_LE_RAND                                    (0x0018 + LE_G)//0x2018
#define HCI_LE_START_ENCRYPTION                        (0x0019 + LE_G)//0x20019
#define HCI_LE_LONG_TERM_KEY_REQUEST_REPLY             (0x001A + LE_G)  //0x201a
#define HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY    (0x001B + LE_G)//0x201b
#define HCI_LE_READ_SUPPORTED_STATES                   (0x001C + LE_G)//0x201c  
#define HCI_LE_RECEIVER_TEST                           (0x001D + LE_G)//0x201d  
#define HCI_LE_TRANSMITTER_TEST                        (0x001E + LE_G)//0x201e  
#define HCI_LE_TEST_END                                (0x001F + LE_G)//0x201f
#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY (0x0020 + LE_G)//0x2020
#define HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY (0x0021 + LE_G)//0x2021
#define HCI_LE_SET_DATA_LENGTH       (0x0022 + LE_G)//0x2022
#define HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH    (0x0023 + LE_G)//0x2023
#define HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH    (0x0024 + LE_G)//0x2024
#define HCI_LE_READ_LOCAL_P256_PUBLIC_KEY    (0x0025 + LE_G)//0x2025
#define HCI_LE_GENERATE_DH_KEY      (0x0026 + LE_G)//0x2026
#define HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST          (0x0027 + LE_G)//0x2027
#define HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST          (0x0028 + LE_G)//0x2028
#define HCI_LE_CLEAR_RESOLVING_LIST          (0x0029 + LE_G)//0x2029
#define HCI_LE_READ_RESOLVING_LIST_SIZE          (0x002A + LE_G)//0x202a
#define HCI_LE_READ_PEER_RESOLVABLE_ADDRESS          (0x002B + LE_G)//0x202b
#define HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS                   (0x002C + LE_G)//0x202c
#define HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE               (0x002D + LE_G)//0x202d
#define HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT              (0x002E + LE_G)//0x202e
#define HCI_LE_READ_MAXIMUM_DATA_LENGTH                                        (0x002F + LE_G)//0x202F

#define HCI_LE_ENHANCED_RECEIVER_TEST                                        (0x0033 + LE_G)//0x2033
#define HCI_LE_ENHANCED_TRANSMITTER_TEST                                        (0x0034 + LE_G)//0x2034



#define E_G    (EXTENDED_OGF << 10)


//offset 0X for scan
#define HCI_WRITE_SCAN_PARAMS_EXT                                                    (0x0000 + E_G)
#define HCI_WRITE_SCAN_ENABLE_EXT                                                     (0x0001 + E_G)
#define HCI_RESET_EXT                                                                               (0x0002 + E_G)
#define HCI_READ_BUFF_NUM                                                                      (0x0003 + E_G)
#define HCI_SET_DEV_NAME_ADDR                                                                      (0x0004 + E_G)
//offset 1X for adv
#define HCI_SET_ADV_PARAM_EXT                                                              (0x0010 + E_G)
#define HCI_SET_ADV_STOP_EXT                                                              (0x0011 + E_G)
#define HCI_SET_ADV_DATA                                                                  (0x0012 + E_G)
#define HCI_SET_SCAN_RSP                                                                  (0x0013 + E_G)




//offset 2X for slave
#define HCI_GATT_SERVER_SEND_INDICATION                                         (0x0020 + E_G)
#define HCI_GATT_SERVER_SEND_NOTIFICATION                                         (0x0021 + E_G)




//offset 3X for master
#define HCI_CREATE_CONNCT_EXT                                                            (0x0030 + E_G)
#define HCI_DISCONNECT_EXT                                                                    (0x0031 + E_G)
#define HCI_CONNCT_CANCEL_EXT                                                             (0x0032 + E_G)
#define HCI_CONNCT_UPDATE_EXT                                                             (0x0033 + E_G)
#define HCI_CONNCT_GET_MTU_EXT                                                           (0x0034 + E_G)
#define HCI_PHY_UPDATE                                                                            (0x0035 + E_G)


#define HCI_SM_Para_Set                                                                            (0x0040 + E_G)
#define HCI_SM_Security_Request                                                              (0x0041 + E_G)
#define HCI_SM_Send_PassKey                                                                   (0x0042 + E_G)

#define HCI_GATT_CLIENT_INIT                                                                  (0x0050 + E_G)

#define HCI_GATT_CLIENT_DIS_PRI_SERVS                                                (0x0060 + E_G)
#define HCI_GATT_CLIENT_DIS_PRI_SERVS_U16                                        (0x0061 + E_G)
#define HCI_GATT_CLIENT_DIS_PRI_SERVS_U128                                      (0x0062 + E_G)

#define HCI_GATT_CLIENT_DIS_CHAR_FOR_SERV                                        (0x0070 + E_G)
#define HCI_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U16                             (0x0071 + E_G)
#define HCI_GATT_CLIENT_DIS_CHAR_FOR_RANGE_U128                           (0x0072 + E_G)
#define HCI_GATT_CLIENT_DIS_CHAR_DESCRIPTOR                                    (0x0073 + E_G)


#define HCI_GATT_CLIENT_FIND_INCLUDED_SERV                                      (0x0080 + E_G)

#define HCI_GATT_CLIENT_RD_CHAR_VAL_BY_HANDLE                               (0x0090 + E_G)
#define HCI_GATT_CLIENT_RD_CHAR_VAL_BY_U16                                     (0x0091 + E_G)
#define HCI_GATT_CLIENT_RD_CHAR_VAL_BY_U128                                   (0x0092 + E_G)
#define HCI_GATT_CLIENT_RD_LONG_CHAR_VAL_BY_HANDLE_OFFSET      (0x0093 + E_G)

#define HCI_GATT_CLIENT_WR_CHAR_VAL_WITHOUT_RESP                       (0x00a0 + E_G)
#define HCI_GATT_CLIENT_WR_CHAR_VAL                                                  (0x00a1 + E_G)
#define HCI_GATT_CLIENT_WR_LONG_CHAR_VAL_OFFSET                         (0x00a2 + E_G)


//offset for data
#define HCI_DATA_SEND                                                                              (0x00b0 + E_G)

//offset for others
#define HCI_OTA_PARA_SET                                          (0x00c0 + E_G)
#define HCI_PERI_PARA_SET                                          (0x00c1 + E_G)
#define HCI_MXD_READ_REG                (0x00c2 + E_G)
#define HCI_MXD_WRITE_REG               (0x00c3 + E_G)

#define HCI_CMD_USER_DEFINE                                           (0x00F0 + E_G)
#define HCI_OTA_DEBUG_EN                 0x55550000
#define HCI_DSP_LOG_EN                   0xAAAA0000
#define HCI_DEBUG_DIS                    0xBBBB0000
#define HCI_SYS_TEST                     0x55AA0000   //test for API


#define HCI_SET_CLK_TEST                                               (0x00F2 + E_G)

//event code
#define CMD_CMPLT_EVT_CODE                                0xE
#define CMD_STATUS_EVT_CODE                              0xF

#define LE_ADV_RPT_EVT_CODE                              0x3E
#define LE_ADV_RPT_SUBEVT_CODE                        0x2

#define DISCNNT_CMPLT_EVT_CODE                        0x5

#define LE_CNNCT_CMPLT_EVT_CODE                      0x3E
#define LE_CNNCT_CMPLT_SUBEVT_CODE                0x01

#define LE_CNNCT_UPDATE_CMPLT_EVT_CODE                 0x3E
#define LE_CNNCT_UPDATE_CMPLT_SUBEVT_CODE              0x03

#define LE_READ_RMT_USED_FEAURES_CMPLT_EVT_CODE              0x3E
#define LE_READ_RMT_USED_FEAURES_CMPLT_SUBEVT_CODE           0x04

#define NUM_OF_CPMLT_PKT_EVT_CODE 0x13

#define READ_RMT_VER_INFO_CMPLT_EVT_CODE  0xC

#define ENCRYPTION_CHG_EVT_CODE  0x8

#define ENCRYPTION_KEY_REFRESH_CMPLT_EVT_CODE  0x30

#define LE_LONG_TERM_KEY_REQUEST_EVT_CODE  0x3E
#define LE_LONG_TERM_KEY_REQUEST_SUBEVT_CODE  0x5

#define LE_RMT_CNNCT_PARA_REQ_EVT_CODE 0x3E
#define LE_RMT_CNNCT_PARA_REQ_SUBEVT_CODE 0x6

#define AUTHENTICATED_PAYLOAD_TIMOUT_EXPIRED_EVT_CODE  0x57

#define LE_DATA_LENGTH_CHG_EVT_CODE   0x3E
#define LE_DATA_LENGTH_CHG_SUBEVT_CODE   0x07

#define DATA_BUFFER_OVFL_EVT_CODE 0x1A

#define HW_ERR_EVT_CODE 0x10

#define LE_DIRECT_ADV_RPT_EVT_CODE  0x3E
#define LE_DIRECT_ADV_RPT_SUBEVT_CODE  0xB


#define LE_ENHANCED_CNNCT_CMPLT_EVT_CODE 0x3E
#define LE_ENHANCED_CNNCT_CMPLT_SUBEVT_CODE 0xA

#define LE_READ_LOCAL_P256_PUBLIC_KEY_CMPLT_EVT_CODE 0x3E
#define LE_READ_LOCAL_P256_PUBLIC_KEY_CMPLT_SUBEVT_CODE 0x8


#define LE_GEN_DHKEY_CMPLT_EVT_CODE 0x3E
#define LE_GEN_DHKEY_CMPLT_SUBEVT_CODE 0x9


//command complete event

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    int8 txPwr;
}  read_tx_pwr_level_cmplt_evt;






typedef struct
{
    uint8 status;
    uint8 wlSize;
}  read_wl_size_cmplt_evt;


typedef struct
{
    uint8 status;
    uint8 hciVer;
    /*
     HCI_Version    0       Bluetooth? Core Specification 1.0b
     1      Bluetooth Core Specification 1.1
     2      Bluetooth Core Specification 1.2
     3      Bluetooth Core Specification 2.0 + EDR
     4      Bluetooth Core Specification 2.1 + EDR
     5      Bluetooth Core Specification 3.0 + HS
     *****6 Bluetooth Core Specification 4.0
     7      ?Bluetooth Core Specification 4.1
     8 - 255        Reserved
     */
    uint8 hciReverlo8;
    uint8 hciReverhi8;
    uint8 lmpVer;                  //same as hciVer
    uint8 mfNamelo8;                 // manufacturer name
    uint8 mfNamehi8;
    uint8 lmpSubVerlo8;
    uint8 lmpSubVerhi8;

}  read_local_ver_info_cmplt_evt;




typedef struct
{
    uint8 status;
    uint8 lmpFeature[8];

}  read_local_supported_features_cmplt_evt;


typedef struct
{
    uint8 status;
    uint8 bdAddr[6];
}  read_bd_addr_cmplt_evt;


typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    int8 rssi;
} read_rssi_evt;

typedef struct
{
    uint8 status;
    uint8 hcAclDataPcktLenlo8;               //HC_ACL_Data_Packet_Length     2
    uint8 hcAclDataPcktLenhi8;               //HC_ACL_Data_Packet_Length     2
    uint8 hcSyncDataPckLen;                //HC_Synchronous_Data_Packet_Length     1
    uint16 hcTtlNumAclDataPckt;    //HC_Total_Num_ACL_Data_Packets 2
    uint16 hcTtlNumSyncDataPckt;   //HC_Total_Num_Synchronous_Data_Packets 2
}  read_buffer_size_cmplt_evt;

typedef struct
{
    uint8 status;
    uint8 HC_LE_Data_Packet_Lengthlo8;               //HC_ACL_Data_Packet_Length     2
    uint8 HC_LE_Data_Packet_Lengthhi8;               //HC_ACL_Data_Packet_Length     2
    uint8 HC_Total_Num_LE_Data_Packets;                //HC_Synchronous_Data_Packet_Length     1
}  le_read_buffer_size_cmplt_evt;




typedef struct
{
    uint8 status;
    uint8 packetRecvlo8;               //HC_ACL_Data_Packet_Length     2
    uint8 packetRecvhi8;               //HC_ACL_Data_Packet_Length     2
    uint8 packetdata1_byte0;               //HC_ACL_Data_Packet_Length     2
    uint8 packetdata1_byte1;               //HC_ACL_Data_Packet_Length     2
    uint8 packetdata1_byte2;               //HC_ACL_Data_Packet_Length     2
    uint8 packetdata1_byte3;               //HC_ACL_Data_Packet_Length     2
    uint8 packet_crcerror_countlo8;               //HC_ACL_Data_Packet_Length     2
    uint8 packet_crcerror_counthi8;               //HC_ACL_Data_Packet_Length     2

}  dtm_cmplt_evt;

typedef struct
{
    uint8 status;
    int8 advTxPwr;
}  le_read_adv_ch_tx_pwr_cmplt_evt;




typedef struct
{
    uint8 status;
    uint8 leSpptHost;
    uint8 smltLeHost;      //Simultaneous_LE_Host

}  read_le_host_supported_cmplt_evt;



typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    uint8 authenticatedPayloadTimeoutlo8;
    uint8 authenticatedPayloadTimeouthi8;

} read_authenticated_payload_timeout_evt;


typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;

} write_authenticated_payload_timeout_evt;


typedef struct
{
    uint8 status;
    uint8 white_list_size;
} le_read_white_list_size_evt;

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    uint8 chnlMap[5];
} le_read_channel_map_evt;


typedef struct
{
    uint8 status;
    uint8 encrypted_data[16];
} le_encrypt_evt;

typedef struct
{
    uint8 status;
    uint8 random_number[8];
} le_rand_evt;

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
} le_long_term_key_request_reply_evt;

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
} le_long_term_key_request_negative_reply_evt;


typedef struct
{
    uint8 status;
    uint8 le_states[8];
} le_supported_states_evt;

typedef struct
{
    uint8 status;
    uint8 num_of_pktslo8;
    uint8 num_of_pktshi8;
} le_test_end_evt;

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
} le_remote_cnnct_parameter_request_reply_evt;

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
} le_remote_cnnct_parameter_request_negative_reply_evt;

typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
} le_set_data_length_evt;

typedef struct
{
    uint8 status;
    uint8 SuggestedMaxTxOctetslo8;
    uint8 SuggestedMaxTxOctetshi8;
    uint8 SuggestedMaxTxTimelo8;
    uint8 SuggestedMaxTxTimehi8;
} le_read_suggested_default_data_length_evt;

typedef struct
{
    uint8 status;
    uint8 Resolving_List_Size;
} le_read_resolving_list_size_evt;

typedef struct
{
    uint8 status;
    uint8 Peer_Resolvable_Address[6];
} le_read_peer_resolvable_address_evt;

typedef struct
{
    uint8 status;
    uint8 Local_Resolvable_Address[6];
} le_read_local_resolvable_address_evt;

typedef struct
{
    uint8 status;
    uint8 supportedMaxTxOctetslo8;
    uint8 supportedMaxTxOctetshi8;
    uint8 supportedMaxTxTimelo8;
    uint8 supportedMaxTxTimehi8;
    uint8 supportedMaxRxOctetslo8;
    uint8 supportedMaxRxOctetshi8;
    uint8 supportedMaxRxTimelo8;
    uint8 supportedMaxRxTimehi8;
} le_read_maximum_data_length_evt;

typedef struct
{
    uint8 status;

}  common_cmplt_evt;












typedef struct
{
    uint8 status;
    uint8 numHciCmdPckts;
    uint8 cmdOpCodelo;
    uint8 cmdOpCodehi;
    uint8 para0;
    uint8 para1;
    uint8 para2;
    uint8 para3;
} cmd_status_evt;



typedef struct
{
    // 12+n byte
    uint8  subEvtCode; //=0x02 // 1byte
    uint8  numReports;             // 1byte
    uint8  evtType;                   // 1byte//depend on report number
    uint8  addrType;                 // 1byte
    uint8  addr6Oct[6];              // 6byte
    uint8  length_data;                    // 1byte
    uint8  data31Oct[31];           // N byte
    int8  rssi_S8;        //signed 8  //1byte
} le_adv_report_evt;


typedef struct
{
    uint8  subEvtCode; //=0x02 // 1byte
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    uint8 reason;
} discnnt_cmplt_evt;


//zl @11.24
typedef struct
{
    uint8  subEvtCode; // =0x01

    uint8  status;
    uint8 cnnctH16bitlo;
    uint8 cnnctH16bithi;


    uint8  role;
    uint8  peerAddrType;

    uint8  peerAddr6Oct[6];

    uint8 cnnctInterval16bitlo;
    uint8 cnnctInterval16bithi;

    uint8 cnnctLatency16bitlo;
    uint8 cnnctLatency16bithi;

    uint8 supervisionTimeout16bitlo;
    uint8 supervisionTimeout16bithi;

    uint8  masterClockAccuracy;
}  le_cnnct_cmplt_evt;



typedef struct
{
    uint8   subEvtCode;
    uint8  status;
    uint8 cnnctH16bitlo;
    uint8 cnnctH16bithi;
    uint8 cnnctInterval16bitlo;
    uint8 cnnctInterval16bithi;
    uint8 cnnctLatency16bitlo;
    uint8 cnnctLatency16bithi;
    uint8 supervisionTimeout16bitlo;
    uint8 supervisionTimeout16bithi;


} le_connection_update_cmplt_evt;




typedef struct
{
    uint8 subEvtCode;
    uint8  status;
    uint8 cnnctH16bitlo;
    uint8 cnnctH16bithi;
    uint8  leFeatures8Oct[8];

} le_read_rmt_used_features_cmplt_evt;


typedef struct
{
    uint8 subEvtCode;
    uint8  Number_of_Handles; //=0x05
    uint8 Connection_Handlelo8; //depend on the connection number
    uint8 Connection_Handlehi8; //depend on the connection number
    uint8 HC_Num_Of_Completed_Packetslo8;
    uint8 HC_Num_Of_Completed_Packetshi8;
}  num_of_cmplt_pkt_evt;



typedef struct
{
    uint8 status;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    uint8 version;
    uint16 mnftName;
    uint16 subVer;
} read_rmt_ver_info_cmplt_evt;




typedef struct
{
    uint8  subEvtCode; // =0x01
    uint8 status;
    uint8 cnnctHlo8;
    uint8 cnnctHhi8;
    uint8   Encryption_Enabled;
} enc_chg_evt;




typedef struct
{
    uint8 status;
    uint8 cnnctHlo8;
    uint8 cnnctHhi8;
} enc_key_fresh_evt;




typedef struct
{
    uint8  subEvtCode;
    uint8 cnnctHlo8;
    uint8 cnnctHhi8;
    uint8  randomNumber8Oct[8];
    uint8 encryptionDiversifier2oct[2];
}  le_long_term_key_request_evt;





typedef struct
{
    uint8 subEvtCode;
    uint8 cnnctHlo8;
    uint8 cnnctHhi8;
    uint8 intv_minlo8;
    uint8 intv_minhi8;
    uint8 intv_maxlo8;
    uint8 intv_maxhi8;
    uint8 lantencylo8;
    uint8 lantencyhi8;
    uint8 timeOutlo8;
    uint8 timeOuthi8;

} le_rmt_cnnct_para_req_evt;




typedef struct
{
    uint16 cnnctHandle;
}  authenticated_payload_timeout_expired_evt;




typedef struct
{
    uint8 subEvtCode;
    uint8 cnnctHandlelo8;
    uint8 cnnctHandlehi8;
    uint8 maxtxoctetslo8;
    uint8 maxtxoctetshi8;
    uint8 maxtxtimelo8;
    uint8 maxtxtimehi8;
    uint8 maxrxoctetslo8;
    uint8 maxrxoctetshi8;
    uint8 maxrxtimelo8;
    uint8 maxrxtimehi8;
}  le_data_len_chg_evt;




typedef struct
{
    uint8 subEvtCode;
    uint8 numReports;
    uint8 eventType;
    uint8 dataAddrType;
    uint8 directAddr[6];
    uint8 AddrType;
    uint8 Addr[6];
    int8 rssi;
}  le_direct_adv_rpt_evt;




typedef struct
{
    uint8 subEvtCode;
    uint8 status;
    uint16 cnnctHandle;
    uint8 role;
    uint8 peerAddrType;
    uint8 peerAddr[6];
    uint8 localResvPriAddr[6];
    uint8 peerResvPruAddr[6];
    uint16 cnnctItv;
    uint16 cnnctLatency;
    uint16 SupTimeout;
    uint8 mstClkAccu;
}  le_enhanced_cnnct_cmplt_evt;




typedef struct
{
    uint8 linkType;
}  data_buffer_overflow_evt;




typedef struct
{
    uint8 hw_code;
}  hw_err_evt;

typedef struct
{
    uint8 subEvtCode;
    uint8 status;
    uint16 dhkey[64];
} le_read_local_P256_public_key_cmplt_evt;


typedef struct
{
    uint8 subEvtCode;
    uint8 status;
    uint16 dhkey[32];
}  le_gen_dhkey_cmplt_evt;


typedef struct
{
    uint8 status;
    uint8 supported_command[64];
} read_local_supported_command_cmplt_evt;

typedef struct
{
    uint8 status;
    uint8 le_features[8];
} le_read_local_supported_features_cmplt_evt;


typedef union
{
    read_tx_pwr_level_cmplt_evt read_tx_pwr_levl_cmplt;
    read_wl_size_cmplt_evt     read_wl_size_cmplt;
    read_local_ver_info_cmplt_evt read_local_ver_info_cmplt;
    read_local_supported_command_cmplt_evt read_local_supported_command_cmplt;
    read_local_supported_features_cmplt_evt read_local_supported_features_cmplt;
    read_bd_addr_cmplt_evt  read_bd_addr_cmplt;
    read_rssi_evt read_rssi;
    read_buffer_size_cmplt_evt read_buffer_size_cmplt;
    le_read_buffer_size_cmplt_evt le_read_buffer_size_cmplt;
    le_read_local_supported_features_cmplt_evt le_read_local_supported_features_cmplt;
    dtm_cmplt_evt dtm_cmplt;
    le_read_adv_ch_tx_pwr_cmplt_evt le_read_adv_ch_tx_pwr_cmplt;
    read_le_host_supported_cmplt_evt read_le_host_supported_cmplt;
    read_authenticated_payload_timeout_evt read_authenticated_payload_timeout;
    write_authenticated_payload_timeout_evt write_authenticated_payload_timeout;
    le_read_white_list_size_evt le_read_white_list_size;
    le_read_channel_map_evt le_read_channel_map;
    le_encrypt_evt le_encrypt;
    le_rand_evt le_rand;
    le_long_term_key_request_reply_evt le_long_term_key_request_reply;
    le_long_term_key_request_negative_reply_evt le_long_term_key_request_negative_reply;
    le_supported_states_evt  le_supported_states;
    le_test_end_evt le_test_end;
    le_remote_cnnct_parameter_request_reply_evt le_remote_cnnct_parameter_request_reply;
    le_remote_cnnct_parameter_request_negative_reply_evt le_remote_cnnct_parameter_request_negative_reply;
    le_set_data_length_evt le_set_data_length;
    le_read_suggested_default_data_length_evt le_read_suggested_default_data_length;
    le_read_resolving_list_size_evt le_read_resolving_list_size;
    le_read_peer_resolvable_address_evt le_read_peer_resolvable_address;
    le_read_local_resolvable_address_evt le_read_local_resolvable_address;
    le_read_maximum_data_length_evt le_read_maximum_data_length;
    common_cmplt_evt common_cmplt;
    uint8 bytes[80];
} cmd_return_parameters;

typedef struct
{
    uint8 reserved;//for 32 bit align
    uint8 numHciCmdPck;
    uint8 cmdOpcodelo8;
    uint8  cmdOpcodehi8;
    cmd_return_parameters return_parameters;
}  cmd_cmplt_evt;

typedef enum TXRX_DATA_SOURCE
{
    FROM_PERI           =  1,
    FROM_HCI             = 2,
    FROM_OTHERS       = 3
} TXRX_DATA_SOURCE;


#define TO_FIFO_INDEX(idx, tail_length)   ((idx >= tail_length) ? (idx - PERI_DATA_SIZE):idx)


extern osThreadId hci_rx_taskId, hci_tx_taskId;

extern uint8 hci_reset_flag;
extern uint8 hci_reset_flag_dtm;
extern uint16 g_dma_uart_tx_lock;
extern uint8 g_dma_uart_rx_ch, g_dma_uart_tx_ch;
extern cmd_cmplt_evt cmplt_event_info;


extern uint8 g_cmplt_event_length;
extern hci_rx_header_t hci_rx_header;
extern uint8 hci_scanData[31];
extern uint8 hci_advData[31];

/** @name GLOABL FUNCTION
* @{
*/

/**
* @brief  hci app init
* @param  none
* @return none
*/
void hci_app_init(void);

extern uint8 hci_soft_reset(void);
extern void hci_init(void);
extern uint16 hci_cmd_dispatch(uint8 * data, uint16 opcode, uint16 payload_length, uint16 tail_length);
extern uint8 hci_event_generate(uint8 evtCode, uint8 length, uint8 * data);
extern uint16 peri_hci_process(uint8 * data, uint16 data_length, uint16 tail_length);
extern void peri_rx_process(void const *argument);
extern void peri_tx_process(void const *argument);
extern void uart_dma_mode_init(void);
extern void uart_dma_rx_handler(uint8 isr_status);
extern void uart_dma_rx_response(void);
extern void uart_dma_tx_acldata(void);
extern void uart_dma_tx_event(void);
extern void uart_dma_tx_handler(uint8 isr_status);
extern void ancs_data_report(uint8 idx);
extern void hci_api_uart_init(void);
extern void hci_dma_init(void);
extern void hci_uart0_recv_handle(void);
extern void hci_uart_rx_buf_clear_cnt(void);
extern bool is_hci_sleep(void);

/** @} */



#endif


/** @} */ // 模块结尾,自行拷贝到末尾
