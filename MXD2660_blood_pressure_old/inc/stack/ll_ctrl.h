#ifndef _LL_CTRL_H_
#define __LL_CTRL_H_
#include "ll_data.h"
#include "stack_ctrl.h"
#include "gap.h"
#include "ll_security.h"
#include "gatt_client.h"
#include "timer.h"
#include "ll_struct.h"



typedef struct
{
    t_adv_pdu adv_in_pdu;
    t_adv_pdu adv_out_pdu;
    t_adv_pdu scan_rsp_out_pdu;
    uint32 interval_us;    //us is unit
    uint32 tBtwn2AdvPduIn1Evt_us;
    uint32 adv_init_cnt_us_h;
    uint32 adv_init_cnt_us_l;
    uint32 adv_last_event_start_cnt_us;
    uint16 rand_delay;	
    uint8 advEvtChQueue[3]; //chMap       // ch37 ch38 ch39
    uint8 advEvtChCnt;
    uint8 advFltPolicy8bit;//0x00:all device      0x01:Cnnct all ScanWhiteLst       0x02:ScanAllCnnctWhiteLst     0x03:scanWhiteLstCnnctWhiteLst    0x04~0xff:Reserved
    uint8  ownAddrType8bit;
    uint8 drctAddr6oct[6]; //Public Device Address or Random Device Address of the device to be connected.
    uint8 drctAddrType8bit;//0x00Public Device Address;  0x01Random Device Address; 0x02 – 0xFFReserved for future use;
    uint8 advType;        //
    uint8 bufLen;
    uint8 rspLen;
    uint8 active;
    uint8 subState;
} t_adv_cfg;



typedef struct
{
    uint8   addr[6];
    uint8   addr_type;
} t_scan_list;

typedef struct
{
    t_adv_pdu   sReqPdu;
    uint32         interval_us ;
    uint32         win_us;
    uint32          upperlimit;
    uint32        scan_last_event_start_cnt_us;
    uint8          backoffcount;
    uint8          typePass0Act1;        //  0x00 : passive      0x01:active
    uint8          filterPolicyAll0Wl1;    //0x00:all device           0x01:ScanWhiteLst               0x02~0xff reserved
    uint8          ownAddrTypePub0Rdm1;   //0x00Public Device Address(default);          0x01Random Device Address; 0x02 – 0xFFReserved for future use;
    uint8          duplicateFilterEn;
    uint8          curCh;
    uint8          getrsp_fail_conut;
    uint8          getrsp_ok_conut;
    uint8          scan_sub_state;
    uint8          active;
} t_scan_cfg;


typedef struct
{
    t_adv_pdu   conn_req_out_pdu;
    uint32         accCode;//0-0xffff
    uint32         init_last_event_start_cnt_us;
    uint16         connIntv1250us ; // N*1.25ms range(6-0xc80)
    uint16         sTimeOut10ms;//Supervision_Timeout (0x0a-0xc80)
    uint16         conn_latency;

    uint16         scanIntvus ; // N*0.625us range(4-0x4000)
    uint16         scanWinus ; // N*0.625us range(4-0x4000)

    uint16         ceLength625us;//0-0xffff
    uint8           ownAddrType;
    uint8           peerAddrType;

    uint8           Peer_Address[6];
    uint8           connPolicy;//00  not filter  , 01 only conn to white list device

    uint16         winOffset16bit1250us ; // N*1.25ms range(6-0xc80)

    uint8           winsize8bit1250us;
    uint8           hop5bitSca3bit;

    uint8          chMap[5];
    uint8           crcInit[3];//0-0xffff
    uint8           curCh;
    uint8           active;
    //uint8           conn_created;
} t_init_cfg;




#define MAX_LLC_STATE_NUM 6


typedef struct
{
    uint8          peerMsgSta ;
    uint8          localMsgSta ;
    uint8          initDev ; // who init this ctrl (2 peer init this ctrl,1 local init this ctrl)
    uint8          sndSta ;

} t_ctrl_sta;


typedef struct
{
    t_ctrl_sta  ctrSta[MAX_LLC_STATE_NUM];
} t_ctrl_state;



typedef struct
{
    gatt_client_state_t gatt_client_state;

    uint16 uuid16;
    uint8  uuid128[16];

    uint16 start_group_handle;
    uint16 end_group_handle;

    uint16 query_start_handle;
    uint16 query_end_handle;

    uint8  characteristic_properties;
    uint16 characteristic_start_handle;

    uint16 attribute_handle;
    uint16 attribute_offset;
    uint16 attribute_length;
    uint8* attribute_value;

    // read multiple characteristic values
    uint16    read_multiple_handle_count;
    uint16  * read_multiple_handles;

    uint16 client_characteristic_configuration_handle;
    uint8  client_characteristic_configuration_value[2];

    uint8  filter_with_uuid;
    uint8  send_confirmation;

    int      le_device_index;
    uint8  cmac[8];

} t_gatt_client;

typedef struct
{
    t_conn_para  para;
    t_ctrl_pdu      rx_ctrl_pdu;
    t_ctrl_pdu      tx_ctrl_pdu;
    t_data_pdu     rx_data_pdu;
    t_data_pdu     tx_data_pdu;

    uint16            ll_action_pending;
    uint16            current_security_state;
    /*
    PROCEDURE RESPONSE TIMEOUT
    If the procedure response timeout timer reaches 40 seconds, the connection is
    considered lost. The Link Layer exits the Connection State and shall transition
    to the Standby State. The Host shall be notified of the loss of connection.
    */
    uint32          timeout_for_ll_rsp;
    uint32          timeout_for_link_key_request;
    uint32          timeout_for_authenticatedPayload_ms ;
    uint32          timeout_timer_authenticted;
    uint32          timeout_for_supervision;
    uint32          timeout_for_terminate;
    uint32          sca_base_time;
    uint32          winWide;
    uint32          nextExpectedSeqNum;
    uint32          transmitSeqNum;


    uint32          anchor_upd;
    uint32          supervision_us;
    uint32          rxlost_util_timeout;
    uint32          rxlost_num;
    uint32          interval_us;
    uint32          timeout_us;
    uint32          conn_next_ce_start_cnt_us_l;

    t_connection_param_req_payload rx_connection_param;
    uint32          crcInit;
    uint32          accCode;
    uint32          lastUnmapCh;

    uint16          curCh;
    uint16          connHdl;

    uint16          winOffset16bit;
    uint16          interval1250us ; // N*1.25ms range(6-0xc80)
    uint16          sTimeOut10ms;//Supervision_Timeout
    uint16          conn_latency;
	
    uint16          instantUpdateCon;
    uint16          instantUpdateMap;

    uint16          instantUpdatePhy;
    uint16          curConnEvt;//evt counter, 0,1,2...
    uint16          New_interval1250us; // N*1.25ms range(6-0xc80)

    uint16          New_sTimeOut;//Supervision_Timeout
    uint16          New_conn_latency;

    uint16          cache_remote_compId16bit;
    uint16          cache_remote_subVersNr16bit;
    uint16          sca_val; // mst + slv   sca
    uint8           crc_error_cnt;
    uint8           disconn_reason ;

    uint8           ediv[2];
    uint8           iv[8];  //IV = IVm || IVs (LSO to MSO)
    uint8           rand[8];
    uint32           ltk[4];
    uint32           skd[4];//SKD = SKDm || SKDs SKD (LSO to MSO)
    uint32           sk[4];//SK = Encrypt(LTK, SKD)
    uint32           enc_packetCounter_l; //39 bit(1-32)
    uint32           enc_packetCounter_h; //39 bit(33-39)
    uint32           enc_rx_packetCounter_l; //39 bit(1-32)
    uint32           enc_rx_packetCounter_h; //39 bit(33-39)

    uint16           connMaxTxOctets;
    uint16           connMaxRxOctets;
    //uint16           connRemoteMaxTxOctets;
    //uint16           connRemoteMaxRxOctets;
    uint16           connEffectiveMaxTxOctets;
    uint16           connEffectiveMaxRxOctets;
    uint16           connMaxTxTime;
    uint16           connMaxRxTime;
    //uint16           connRemoteMaxTxTime;
    //uint16           connRemoteMaxRxTime;
    uint16           connEffectiveMaxTxTime;
    uint16           connEffectiveMaxRxTime;

    uint8           New_chMap[5];
    uint8           role;        //  0x00 : master      0x01:slave
    uint8           winsize8bit;
    uint8           hop5bitSca3bit;

    uint8           chMap[5];
    uint8           ownAddrType;
    uint8           peerAddrType;
    uint8           cache_remote_versNr8bit;

    uint8           peerAddr[6];
    uint8           conn_flag;
    uint8           conn_established;

    uint8           last_txdatatype ;  //bit7 bit6:  00 = null, 01 = data, 11 = ctrl
    uint8           last_rxdatalen ;
    uint8           num_of_pkt_send_cmpt;//one pkt send ok
    uint8           rxNesn;

//    uint8           rxSn;
//    uint8           rxMD;
//    uint8           txMD;
//    uint8           rxlen;

    uint8           txlen;
    uint8           latency;
    uint8           active;

    uint8           more_data;
    uint8           cache_remote_feature;
    uint8           need_notify_host;

    uint8          version_ind_status;
    uint8          acl_pkt_reside;
	
    uint8      *  tx_data_buff;
    uint16        sch_cancealed_ce_num;

    uint8          continous_null_pkt;
    uint8          encryp_status;//bit0 rx encryp status, bit1 tx encyp status
    uint8          timeout_status;//bit0 means normal ll_response_timeout, bit1 means ltk request timeout, bit2 mean le ping timeout
    uint8          priority_factor;//if cancealed, the priority increase
    uint8          phy;//bit0: 0-send 1m, 1-send 2m;;;bit1: 0-rx 1m, 1-rx 2m    
    uint8          phy_new;//bit0: 0-send 1m, 1-send 2m;;;bit1: 0-rx 1m, 1-rx 2m
    //for stack connection control variable, include att/sm

    //att part
    uint16 mtu;       // initialized to ATT_DEFAULT_MTU (23), negotiated during MTU exchange
    uint16 max_mtu;   // local maximal L2CAP_MTU, set to l2cap_max_le_mtu()
    uint8  encryption_key_size;
    uint8  authenticated;
    uint8  authorized;
    uint8  bonding_status;
    uint8 peerfeatureSet[8];
    //connection part
    // connection state
    CONNECTION_STATE state;

    // bonding
    //uint16 bonding_flags;
    // requested security level
   // gap_security_level requested_security_level;


    // errands
  //  uint32 authentication_flags;


    t_gatt_client  peripheral;

    // LE Security Manager
    sm_connection sm_connection;

    timer_node_t    gatt_timer_instance;
    uint32              gatt_timer_param;
	uint32    		lastUnmapCh_back;
	uint16          	curCh_back;
	uint16          latency_used;
	uint16          conn_latency_back;
	uint16          rssi_from_reg;

}   t_conn_cfg;



// Central Device db implemenation using static memory
typedef struct
{

    // Identification
    uint8 addr_type;
    uint8 addr[8];
    uint8 irk[16];

    // Stored pairing information allows to re-establish an enncrypted connection
    // with a peripheral that doesn't have any persistent memory
    uint8 ltk[16];
    uint16 ediv;
    uint8  rand[8];
    uint8  key_size;
    uint8  authenticated;
    uint8  authorized;

    // Signed Writes by remote
    uint8 remote_csrk[16];
    uint32 remote_counter;

    // Signed Writes by us
    uint8 local_csrk[16];
    uint32 local_counter;

} le_device_memory_db;

#define LE_DEVICE_MEMORY_SIZE 4

#define MAX_WHITE_LIST_NUM 6
typedef struct
{
    t_white_list       whiteList[MAX_WHITE_LIST_NUM];
    uint32              whiteListCount;
    uint32              connHdl;
    uint32              sca;       //
    uint32              compid;
    uint32              sch_timer_last_cnt_us_h;
    uint32              sch_timer_last_cnt_us_l;
    uint32               features_set;
    uint32               task_idx_indication;//bit0-7 ll_ctrl_task, bit 0~3 is the first choice, 4~7 the second choid, etc, max to support 2 idx stacked, bit8~15 st_ctrl_task  , other reserved
    uint16               evt_mask; //  timer_used;
    uint16               status_disable;
    uint16               connInitialMaxTxOctets;
    uint16               connInitialMaxTxTime;
    uint16               supportedMaxTxOctets;
    uint16               supportedMaxTxTime;
    uint16               supportedMaxRxOctets;
    uint16               supportedMaxRxTime;
    uint8                llcMode;
    uint8                bt_addr[6];
    uint8                randomAddr6oct[6];       //
    uint8                connect_num;
    uint8                supported_command[36];
    uint8                supported_le_feature;
    uint8                mst_chMap[5];
    uint8                hci_acl_rx_cont_exist;
    uint8                hci_acl_rx_cont_rest_len;
    uint8 *             hci_acl_rx_cont_data_ptr;
    uint8                adv_tx_mode; //0 is normal, interval between adv pdu is fixed, 1 means adv pdu interval is random, depend on rx status
    uint8                adv_continuous_mode;// 1 means adv rx fail, next adv tx  start immediately, 0 means adv tx with fixed interval
    uint32              aes128_status;//bit0-7 means corresponding link session key generating running, bit 8 means AES128 function, bit9 means cmac, bit15 means AES128 status, bit16~32 record indx


    //specially for stack
    // basic configuration
    const char         * local_name;
    uint32               class_of_device;
    uint8            ssp_enable;
    uint8            ssp_io_capability;
    uint8            ssp_authentication_requirement;
    uint8            ssp_auto_accept;


    /* bluetooth device information from hci read local version information */
    uint16 manufacturer;


    /* hci state machine */
    uint8   discoverable;
    uint8   connectable;
    uint8   bondable;
    uint8   aes_need_trigger;
    uint8   default_phy;//bit0: 0-send 1m, 1-send 2m;;;bit1: 0-rx 1m, 1-rx 2m

    le_connecting_state le_connecting_state;

    le_device_memory_db le_devices[LE_DEVICE_MEMORY_SIZE];

    int32 ifs_detal;
    uint16 rssi_from_reg;
} t_common_cfg;


typedef struct
{
    uint32   dtm_recv_packet_cnt;
    uint8     dtm_mod;//0 idle , 1 tx, 2  rx
    uint8     dtm_freq;//0-39(2402-2480)
    uint8     dtm_data_sz;  //0-255
    uint8     dtm_data_type;//0-7
    uint8     phy;
    uint8     active;
    /*   0x00 Pseudo-Random bit sequence 9
    0x01 Pattern of alternating bits ‘11110000
    0x02 Pattern of alternating bits ‘10101010’
    0x03 Pseudo-Random bit sequence 15
    0x04 Pattern of All ‘1’ bits
    0x05 Pattern of All ‘0’ bits
    0x06 Pattern of alternating bits ‘00001111
    0x07 Pattern of alternating bits ‘0101’
    */
} t_dtm_para;

enum
{
    LLC_MODE_IDLE = 0,
    LLC_MODE_ADV = 1,
    LLC_MODE_SCAN = 2,
    LLC_MODE_INIT = 3,
    LLC_MODE_SLV = 4,
    LLC_MODE_MST = 5,
    LLC_MODE_DTM = 6,

};



enum
{
    DISABLE = 0,
    ENABLE = 1,
};


enum
{
    LL_CONNECTION_UPDATE_PENDING  = 0x0001,
    LL_CHANNEL_MAP_REQ_PENDING      = 0x0002,
    LL_TERMINATE_PENDING                   = 0x0004,
    LL_ENC_REQ_PENDING                       = 0x0008,//for master/slave only encry procedure, no pause procedure
    LL_PAUSE_ENCRYPT                           = 0x0010,//for master/slave only pause procedure, no encry procedure
    LL_FEATURE_REQ_PENDING               = 0x0020,
    LL_PAUSE_ENC_REQ_PENDING           = 0x0040,
    LL_VERSION_IND_PENDING                = 0x0080,
    LL_CNNCT_PARAM_REQ_PENDING      = 0x0100,
    LL_SLV_FEATURE_REQ_PENDING       = 0x0200,
    LL_TX_ENCRYPTED                              = 0x0400,
    LL_RX_ENCRYPTED                              = 0x0800,
    LL_AUTONOMOUSLY_INITIATED         = 0x1000,
    LL_LENGTH_EXCHG                                 = 0x2000,
    LL_PHY_UPDATE                                 = 0x4000,
    LL_PHY_UPDATE_EXEC                                 = 0x8000,
    
    
};



enum
{
    LE_ENCRYPTION_NOT_ACTIVE     = 0x00,
    LE_ENCRYPTION_ACTIVE             = 0x01,
    LE_ENCRYPTION_PAUSED         = 0x02,//just for procedure control, if need to check whether need encryption, need to check LE_ENCRYPTION_ACTIVE bit
    LE_W4_ENC_REQ                    = 0x04,
    LE_W4_ENC_RSP                    = 0x08,
    LE_W4_START_ENC_REQ          = 0x10,
    LE_W4_START_ENC_RSP          = 0x20,
    LE_W4_PAUSE_ENC_RSP          = 0x40,
    LE_W4_LTK_REPLY              = 0x80
};

#define LL_VERSION_IND_SENT     0x01
#define LL_VERSION_IND_RECIEVED 0x02
/*
0xXXXXXXXXXXXXXXXX:
bit
0 LE Encryption Y Y
1 Connection Parameters Request Procedure Y Y
2 Extended Reject Indication Y Y
3 Slave-initiated Features Exchange Y Y
4 LE Ping Y N
5 LE data packet length extension Y Y
6 LL Privacy  Y N
7 Extended Scanner Filter Policies Y N
8 – 63  RFU
*/
#define MXD_LE_FEATRUE 0x25
#define MXD_LE_FEATRUE_ENC 0x01
#define MXD_LE_FEATRUE_CON_PARA_REQ 0x02
#define MXD_LE_FEATRUE_EXT_REJECT 0x04
#define MXD_LE_FEATRUE_SLV_FEATURE_EXCH 0x08
#define MXD_LE_FEATRUE_PING 0x10
#define MXD_LE_FEATRUE_DATA_LEN_EXT 0x20
#define MXD_LE_FEATRUE_LL_PRIVACY 0x40
#define MXD_LE_FEATRUE_EXT_SCANNER_FILTER_POLICIES 0x80
#define MXD_LE_FEATRUE_2M_PHY 0x100
//unit is 625us    8:(5ms) 12:(7.5ms)
#define _5_0_MS_625_    8
#define _7_5_MS_625_    12
#define _10_0_MS_625_   16
#define _3_75_MS_625_   6
#define _2_5_MS_625_    4
#define _1_25_MS_625_   2
#define _0_625_MS_625_  1

#define _1_28_S_625_    2048    /*1.28s */

// 1.28s
#define _T_US_EXIT_HDCDA  1280000
// <=3.75ms
#define _T_US_INTERVAL_HDCDA    3750
// <= 1.25ms
#define _T_US_BW_2_PDU_HDCDA  1250
#define _T_US_BW_2_PDU_NORMAL  2500         // ::todo ?


// time betweem 2 adv_x pdu in 1 adv event
#define _T_US_BW_2_ADV_PDU_IN_1_EVT_      7500       /*      _7_5_MS_625_*/
#define _T_US_ADV_1_CH_    7500
#define _1_28_S_US_    1280000

#define LL_RESPONSE_TIMEOUT 0x2625A00  //40s, us
#define LL_TERMINATE_TIMEOUT 100000 //100ms, 10us
#define LL_LINK_KEY_REQUEST_TIMEOUT  0x1312D00   //20s, us

//-------------ADV parameters define-------------//
//relate to ADV
//#define ADV_ADDR_LOW   0x8168
//#define ADV_ADDR_MID   0x6816
//#define ADV_ADDR_HI    0x1681
#define ADV_DIRECT_TYPE_TIME    1280000


#define PDU_HEADER_ADV_SCAN_IND   0x1506
#define PDU_HEADER_SCAN_REQ       0x0c03
#define PDU_HEADER_SCAN_RSP       0x2504
#define ADV_CRC_INT 0x555555
#define ADV_CRC_INT_UNNORMAL 0x555551
#define ADV_ChIdx 0x25
#define ADV_ADV_ACCODE 0x8e89bed6
#define ADV_ADV_ACCODE_UNNORMAL 0x8e89bed4
#define ADV_ADV_H 0x8e89
#define ADV_ADV_L 0xbed6
#define ADV_PDU_TYPE 0x01
#define ADV_TimerInit 0x1ffff01
//#define ADV_PDU_TYPE 0x01
#define DATA_PDU_TYPE 0x00

#define DTM_ACCCODE  0x71764129

#define ADV_TIMER_NO 0
#define ADV_TIMER_INTVAL_VAL (5*1000)
#define ADV_TIMER_LSN_BGN ( 1000)
#define ADV_TIMER_LSN_END ( 1002)
#define ADV_TIMER_WAKEUP (2000)


//-------------Scan parameters define-------------//
#define SCAN_ADDR_LOW   0x0000
#define SCAN_ADDR_MID   0xffff
#define SCAN_ADDR_HI    0x0000

#define SCAN_TIMER_NO 1
#define SCAN_TIMER_INTVAL_VAL (10*1000)
#define SCAN_TIMER_LSN_BGN ( 1000)
#define SCAN_TIMER_LSN_END (5000)
#define SCAN_TIMER_WAKEUP (1)

//-------------Slave parameters define-------------//

#define SLAVE_ADV_ADDR_LOW   0x8168
#define SLAVE_ADV_ADDR_MID   0x6816
#define SLAVE_ADV_ADDR_HI    0x1681

#define SLAVE_TIMER_NO 0
#define SLAVE_ADV_TIMER_INTVAL_VAL (10*1000)
#define SLAVE_ADV_TIMER_LSN_BGN (1*1000)
#define SLAVE_ADV_TIMER_LSN_END (5*1000)
#define SLAVE_ADV_TIMER_WAKEUP (0.001*1000)


#define PDU0_DATA_SLAVE_LLID 1
#define PDU0_DATA_SLAVE_NESN 0
#define PDU0_DATA_SLAVE_SN 1
#define PDU0_DATA_SLAVE_MD 0
#define PDU0_DATA_SLAVE_LENGTH 8

#define PDU1_DATA_SLAVE_LLID 1
#define PDU1_DATA_SLAVE_NESN 0
#define PDU1_DATA_SLAVE_SN 1
#define PDU1_DATA_SLAVE_MD 1
#define PDU1_DATA_SLAVE_LENGTH 20
//-------------Master parameters define-------------//
#define PDU0_HEADER_SLAVE_DATA  (PDU0_DATA_SLAVE_LLID|PDU0_DATA_SLAVE_NESN<<2|PDU0_DATA_SLAVE_SN<<3|PDU0_DATA_SLAVE_MD<<4|PDU0_DATA_SLAVE_LENGTH<<8)&0xffff;
#define PDU1_HEADER_SLAVE_DATA  (PDU1_DATA_SLAVE_LLID|PDU1_DATA_SLAVE_NESN<<2|PDU1_DATA_SLAVE_SN<<3|PDU1_DATA_SLAVE_MD<<4|PDU1_DATA_SLAVE_LENGTH<<8)&0xffff;

#define PDU0_SLAVE_DATA      0x0101
#define PDU1_SLAVE_DATA      0x0202


#define MASTER_TIMER_NO 0

#define MASTER_TIMER_INIT_INTVAL_VAL 10*1000
#define MASTER_TIMER_INIT_WIN_SIZE 1*1000
#define MASTER_TIMER_INIT_LSN_END 5*1000
#define MASTER_TIMER_INIT_WAKEUP 0.001*1000

#define PDU0_DATA_MASTER_LLID 1
#define PDU0_DATA_MASTER_NESN 0
#define PDU0_DATA_MASTER_SN 1
#define PDU0_DATA_MASTER_MD 1
#define PDU0_DATA_MASTER_LENGTH 8

#define PDU1_DATA_MASTER_LLID 1
#define PDU1_DATA_MASTER_NESN 0
#define PDU1_DATA_MASTER_SN 1
#define PDU1_DATA_MASTER_MD 1
#define PDU1_DATA_MASTER_LENGTH 20

#define PDU0_HEADER_MASTER_DATA  (PDU0_DATA_MASTER_LLID|PDU0_DATA_MASTER_NESN<<2|PDU0_DATA_MASTER_SN<<3|PDU0_DATA_MASTER_MD<<4|PDU0_DATA_MASTER_LENGTH<<8)&0xffff;
#define PDU1_HEADER_MASTER_DATA  (PDU1_DATA_MASTER_LLID|PDU1_DATA_MASTER_NESN<<2|PDU1_DATA_MASTER_SN<<3|PDU1_DATA_MASTER_MD<<4|PDU1_DATA_MASTER_LENGTH<<8)&0xffff;

#define PDU0_DATA_MASTER_MD_TEST 1
#define PDU1_DATA_MASTER_MD_TEST 1
#define PDU0_HEADER_MASTER_MD0_DATA  (PDU0_DATA_MASTER_LLID|PDU0_DATA_MASTER_NESN<<2|PDU0_DATA_MASTER_SN<<3|PDU0_DATA_MASTER_MD_TEST<<4|PDU0_DATA_MASTER_LENGTH<<8)&0xffff;
#define PDU1_HEADER_MASTER_MD0_DATA  (PDU1_DATA_MASTER_LLID|PDU1_DATA_MASTER_NESN<<2|PDU1_DATA_MASTER_SN<<3|PDU1_DATA_MASTER_MD_TEST<<4|PDU1_DATA_MASTER_LENGTH<<8)&0xffff;

//==============from liujun end================//

#define LLID_C_L2CAP_OR_EMPTY   1
#define LLID_S_L2CAP_OR_WHOLE   2
#define LLID_CTROL  3

#define CON_ROLE_MASTER  0
#define CON_ROLE_SLAVE  1

enum
{
    ADV_IDLE = 0x0,
    ADV_TX_PKT1 = 0x1,
    ADV_TX_PKT2 = 0x2,
    ADV_TX_PKT3 = 0x3,
    ADV_EVENT_END = 0x4
};

enum
{
    SCAN_IDLE = 0x0,
    SCAN_RX_ADV = 0x1,
    SCAN_RX_RSP = 0x2,
};

/** 枚举常量 */
typedef enum ON_TIME_MODE
{
    MODE_DTM = 1,
    MODE_ADV = 2,
    MODE_SLV = 3,
}ON_TIME_MODE_ENUM;
/** 定义类型 ON_TIME_MODE_ENUM */

//T_16MHz = 62.5ns  T_8MHz = 125ns
//T_32k  = 31.25us
//count = ts/T_32k
//         =  ts /(1/(32*k))
//         = ts*32*k
//         = tus/(k*k) *32*k
//        = tus*32/1000
//= (tus<<5)/1000


//update con para or channel map, The master should allow a minimum of 6 connection events
//that the slave will be listening for before the instant occurs.
#define MXD_UPDATE_PARA_INSTANT_INC 8

#define MXD_DEFAULT_TRANSMIT_WINDOW_OFFSET   4
#define MXD_DEFAULT_TRANSMIT_WINDOW_SIZE 4

#define MXD_ENC_START_PREPARE 1
#define MXD_ENC_START 2
#define MXD_ENC_PAUSE_PREPARE 3
#define MXD_ENC_PAUSE 4
#define MXD_ENC_PAUSE_STATE 5

#define SCAN_INTERVAL_MARGIN   (150+22*8+150+47*8+150)  // ifs+scanreq+ifs+scanrsp+rf guard
#define INIT_INTERVAL_MARGIN   (150+44*8+100)  // ifs+connreq+rf guard
#define DTM_RX_INTV                        0x0FFFFFFF //dtm rx interval
#define DTM_RX_WINSZIE                  (DTM_RX_INTV - SCAN_INTERVAL_MARGIN)//dtm rx interval
#define TIMER_ADVANCE_CNT   400//200 us advance for immediate action
#define TIMER_ADV_ADVANCE_CNT   150//200 us advance for immediate action
#define INTVL_PROTECT_MARGIN 200
#define EVENT_CLOSE_MARGIN_CNT  1000
#define CONN_REQ_RELOAD_VALUE_US 0x7FFFF
#define BLE_MTU_BT40   27
#define BLE_MTU_BT42   251
#define BLE_TIME_BT40  384
#define BLE_TIME_BT42  2120

#define MIN(a,b) ((a)<(b)?(a):(b))

#define LE_PING_TIMEOUT  0x7530  //30s, per ms
extern t_adv_cfg g_adv_cfg;
extern t_scan_cfg g_scan_cfg;
extern t_init_cfg g_init_cfg;
extern t_conn_cfg g_conn_cfg[MAX_CONNECTIONS];
extern t_common_cfg g_common_cfg;
extern t_dtm_para g_dtm_cfg;

extern uint8 const gChMapLl2Rf[40];

#define FEATURE_SCAN_ADV_COEX                0x1
#define FEATURE_LINK_ADV_SCAN_COEX       0x2


extern void ll_ctrl_task(void const *argument);

extern void ll_init(void);

extern void adv_scan_para_init(void);

extern void ll_adv_state_ctrl(void);

extern void  ll_set_adv_para(t_adv_para * adv_para);

extern void ll_adv_enable(uint8 enable);

extern void ll_set_scan_para(t_scan_para  * scan_para);

extern void  ll_scan_enable(uint8 enable);

extern void ll_set_dtm_para(void);

extern uint8 ll_init_enable(t_init_connct_para * p);

extern uint8 *ll_get_avail_ctrl_out_buf(uint8 conn_idx, uint8 * bit_index);

extern void ll_ctrl_out_buf_commit(uint8 conn_idx, uint8 index);

extern uint8 ll_get_idx_from_conn_hdl(uint16 hdl);

extern void ll_conn_goto_idle(uint8 idx);

extern uint16 ll_set_intv(uint16 preferredPeriodicity8bit, uint16 interval_Min16bit, uint16 interval_Max16bit);

extern void ll_set_winsize_and_offset(t_connection_update_req_payload* pdu,  uint16 referenceConnEventCount, uint16 offset0, uint16 oldintv);

extern void ll_dtm_stop(void);

extern uint32 ll_hop_channel(uint8 * channelMap, uint32 *lastUnmapCh, uint32 hop);

extern void ll_conn_mst_configure(uint8 is_event_end);

extern void ll_conn_rx_crc_error_process(void);

extern void ll_conn_rx_lost_process(void);

extern void ll_init_goto_mst(void);

extern void ll_release_ctrl_rx_buffer(uint32 ctrl_rx_addr);

extern void ll_release_ctrl_tx_buffer(uint32 ctrl_tx_addr, uint8 idx);

extern uint8 ll_conn_check_event_closed(void);

extern uint8 ll_conn_update_process(uint8 idx);

extern void ll_adv_goto_slv(void);

extern void  ll_conn_slv_tx_prepare(void);

extern uint32 ll_conn_get_wide_window(uint8 idx);

extern uint32 ll_instant_left(uint16 instant, uint16 curEvt);

extern void ll_release_buff_rx_fail(void);

extern void ll_conn_slv_configure(uint8 is_event_closed);


extern void ll_conn_tx_ctrl_acked_process(uint8 idx);

extern void ll_ctrl_start_length_exchange(uint8 idx);

extern void ll_update_random_addr(void);

extern void ll_ctrl_start_length_exchange(uint8 idx);

extern void ll_update_random_addr(void);

extern uint8 ll_get_idx_in_task(uint8 byte_pos);

extern void ll_set_idx_in_task(uint8 byte_pos, uint8 idx);

extern void ll_ctrl_start_phy_update(uint8 idx);

extern void ll_ctrl_start_ll_version_req(uint8 idx);
#endif



