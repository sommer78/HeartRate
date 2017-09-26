#ifndef _LL_DATA_H_
#define _LL_DATA_H_
#include "mem.h"
#include "ll_struct.h"

typedef enum
{
    PTYPE_ADV_IND = 0,
    PTYPE_ADV_DIRECT_IND = 1,
    PTYPE_ADV_NONCONN_IND = 2,
    PTYPE_SCAN_REQ = 3,
    PTYPE_SCAN_RSP = 4,
    PTYPE_CONNECT_REQ = 5,
    PTYPE_ADV_SCAN_IND = 6,
    PTYPE_AVD_CH_PDU_TYPE_Reserved = 7,     //7~15
} E_AVD_CH_PDU_TYPE;

typedef enum   //adv_type
{
    ADV_IND                                                     = 0x00,
    ADV_DIRECT_IND_HIGH_DUTY_CYCLE          = 0x01,
    ADV_SCAN_IND                                            = 0x02,
    ADV_NONCONN_IND                                         = 0x03,
    ADV_DIRECT_IND_LOW_DUTY_CYCLE           = 0x04,
    ADV_TYPE_RSVD = 0x05,
} E_ADV_TYPE_FROM_HCI;

typedef enum   //adb whiteList control
{
    WLC_CNNCT_ALL_SCAN_ALL  = 0,
    WLC_CNNCT_ALL_SCAN_WL   = 1,
    WLC_CNNCT_WL_SCAN_ALL   = 2,
    WLC_CNNCT_WL_SCAN_WL    = 3,
    WLC_RSVD                                = 4,
} E_WHITE_LIST_CTRL;

typedef struct
{
uint16 pduType4bit           :
    4;
uint16 rfu2bit               :
    2;
uint16 txAdd1bit             :
    1;
uint16 rxAdd1bit             :
    1;
uint16 len6bit             :
    6;
uint16 rfu2bit_A              :
    2;
} t_adv_ch_pdu_header;

typedef union
{
    t_adv_ch_pdu_header  _B;
    uint16 header16bit;
} ut_adv_ch_pdu_header;

typedef struct
{
    uint8 advA6oct[6];
    uint8 AdvData[31];

} t_adv_ind_payload, t_adv_nonconn_payload, t_adv_scan_payload, t_scan_rsp_payload;


typedef struct
{
    uint8 advA6oct[6];
    uint8 initA6oct[6];
} t_adv_direct_payload;

typedef struct
{
    uint8 scanA6oct[6];
    uint8 advA6oct[6];
} t_scan_req_payload;



typedef struct
{
    uint8 initA6oct[6];
    uint8 advA6oct[6];
    uint32 aa4oct ;
    uint8 crcInit3oct[3];
    uint8 winsize8bit;
    uint16 winOffset16bit;
    uint16 interval16bit;
    uint16 latency16bit;
    uint16 timeout16bit;
    uint8 chM5oct[5];
    uint8 hop5bitSca3bit;

} t_connect_req_payload;


typedef union
{
    t_adv_ind_payload  * adv_ind_payload;
    t_adv_nonconn_payload * adv_nonconn_payload;
    t_adv_scan_payload * adv_scan_payload;
    t_scan_rsp_payload * scan_rsp_payload;
    t_adv_direct_payload * adv_direct_payload;
    t_scan_req_payload * scan_req_payload;
    t_connect_req_payload * connect_req_payload;
} ut_adv_payload;

//Advertising channel
//Figure 2.4: ADV_IND PDU Payload
typedef struct
{
    ut_adv_ch_pdu_header header;
    ut_adv_payload advData;
} t_adv_pdu;



enum   //control pdu type
{
    //Control PDU Name              Opcode
    LL_CONNECTION_UPDATE_REQ        =       0x00    ,
    LL_CHANNEL_MAP_REQ                      =       0x01    ,
    LL_TERMINATE_IND                        =       0x02    ,
    LL_ENC_REQ      =                       0x03    ,
    LL_ENC_RSP      =                       0x04    ,
    LL_START_ENC_REQ        =       0x05    ,
    LL_START_ENC_RSP        =       0x06    ,
    LL_UNKNOWN_RSP  =               0x07    ,
    LL_FEATURE_REQ  =               0x08    ,
    LL_FEATURE_RSP  =               0x09    ,
    LL_PAUSE_ENC_REQ        =       0x0A    ,
    LL_PAUSE_ENC_RSP        =       0x0B    ,
    LL_VERSION_IND  =               0x0C    ,
    LL_REJECT_IND   =               0x0D    ,
    LL_SLAVE_FEATURE_REQ    =               0x0E    ,
    LL_CONNECTION_PARAM_REQ         =       0x0F    ,
    LL_CONNECTION_PARAM_RSP         =       0x10    ,
    LL_REJECT_IND_EXT       =       0x11    ,
    LL_PING_REQ     =               0x12    ,
    LL_PING_RSP     =               0x13    ,
    LL_LENGTH_REQ     =               0x14    ,
    LL_LENGTH_RSP     =               0x15    ,
    LL_PHY_REQ     =               0x16    ,
    LL_PHY_RSP     =               0x17    ,
    LL_PHY_UPDATE_IND     =               0x18    ,
    LL_CTRL_PDU_Reserved    =       0x19    ,       //0x14~0xff Reserved for Future Use
};


enum
{
    LL_DEV_NON       =       0x00   ,
    LL_DEV_OWN        =       0x01    ,
    LL_DEV_PEER                     =       0x02    ,
    LL_DEV_OWN_HOST           =       0x03
} ;
enum
{
    LL_ROLE_MST       =       0x00   ,
    LL_ROLE_SLV       =       0x01    ,
} ;
enum
{
    LL_CTR_PDU_IDLE      =       0x00 ,
    LL_CTR_PDU_NOT_SND      =       0x01 ,
    LL_CTR_PDU_WAIT_SND      =       0x02 ,
    LL_CTR_PDU_SND_WAIT_ACK        =       0x03    ,
    LL_CTR_PDU_ACKED                     =       0x04   ,
    LL_CTR_PDU_GET_RSP                    =       0x05  ,

} ;

enum
{
    T_LL_DATA_PDU_RESERVED = 0x0,
    T_LL_DATA_PDU_CONTINUE = 0x01,
    T_LL_DATA_PDU_START = 0x02,
    T_LL_DATA_PDU_CTRL = 0x03,
} ;

typedef struct
{
    uint16 llId2bit : 2;
    uint16 nesn1bit : 1;
    uint16 sn1bit : 1;
    uint16 md1bit : 1;
    uint16 rfu3bit : 3;
    uint16 length8bit : 8;
} t_data_ch_pdu_header;

//Figure 2.13: Data channel PDU header
typedef union
{
    t_data_ch_pdu_header  _B;
    uint16 header16bit;
} ut_data_ch_pdu_header;

typedef struct t_q_descr
{
    ut_data_ch_pdu_header header16bit;
    uint8 * data_ptr;
    struct t_q_descr *next;
    uint8   l2cap_processed;
    uint8   stack_feedback;
} data_ptr_list;

typedef struct
{
    data_ptr_list * data_list;
} t_data_pdu;

//Figure 2.15: CtrData field of the LL_CONNECTION_UPDATE_REQ PDU
typedef struct
{
    uint8 opcode;
    uint8 winsize8bit;             //(1 octet)
    uint16 winoffset16bit; // (2 octets)
    uint16 interval16bit;  //(2 octets)
    uint16 latency16bit;   //(2 octets)
    uint16 timeout16bit;   //(2 octets)
    uint16 instant16bit;   //(2 octets)
} t_connection_update_req_payload;

//Figure 2.16: CtrData field of the LL_CHANNEL_MAP_REQ_PDU
typedef struct
{
    uint8 opcode;
    uint8 chM5oct[5];
    uint16 instant16bit;
} t_channel_map_req_payload;

//Figure 2.17: CtrData field of the LL_TERMINATE_IND PDU
//Figure 2.24: CtrData field of the LL_ REJECT_IND              2.4.2.14  LL_REJECT_IND
typedef struct
{
    uint8 opcode;
    uint8 errCode8bit;
} t_terminate_ind_payload, t_reject_ind_payload;

//Figure 2.18: CtrData field of the LL_ENC_REQ PDU
typedef struct
{
    uint8 opcode;

    uint8 rand8oct[8];
    uint8 ediv2oct[2];
    uint8 skdm8oct[8];
    uint8 ivm4oct[4];
}  t_enc_req_payload;

//Figure 2.19: CtrData field of the LL_ENC_RSP PDU
typedef struct
{

    uint8 opcode;
    uint8 skds8oct[8];
    uint8 ivs4oct[4];
} t_enc_rsp_payload;

//2.4.2.6  LL_START_ENC_REQ
//2.4.2.7  LL_START_ENC_RSP
//2.4.2.11 LL_PAUSE_ENC_REQ
//2.4.2.12 LL_PAUSE_ENC_RSP
//2.4.2.19  LL_PING_REQ
//2.4.2.20  LL_PING_RSP
typedef struct
{

    uint8 opcode;
} t_start_enc_req_payload, t_start_enc_rsp_payload, t_pause_enc_req_payload, t_pause_enc_rsp_payload, t_ping_req_payload, t_ping_rsp_payload;

//Figure 2.20: CtrData field of the LL_UNKNOWN_RSP PDU
typedef struct
{
    uint8 opcode;
    uint8 unknowType8bit;
} t_unknown_rsp_payload;

//Figure 2.21: CtrData field of the LL_FEATURE_REQ PDU
//Figure 2.22: CtrData field of the LL_FEATURE_RSP PDU
//Figure 2.25: CtrData field of the LL_SLAVE_FEATURE_REQ PDU  2.4.2.15   T_LL_SLAVE_FEATURE_REQ_PDU
typedef struct
{
    uint8 opcode;
    uint8 featureSet8oct[8];
} t_feature_req_payload, t_feature_rsp_payload, t_slave_feature_req_payload;

//Figure 2.23: CtrData field of the LL_VERSION_IND PDU
typedef struct
{
    uint8 opcode;
    uint8 versNr8bit;
    uint16 compId16bit;
    uint16 subVersNr16bit;
} t_version_ind_payload;

//Figure 2.26: CtrData field of the LL_CONNECTION_PARAM_REQ PDU         2.4.2.16  LL_CONNECTION_PARAM_REQ
//Figure 2.27: CtrData field of the LL_CONNECTION_PARAM_RSP PDU         2.4.2.17  LL_CONNECTION_PARAM_RSP
typedef struct
{
    uint8 opcode;
    uint8 interval_Min16bitlo8;
    uint8 interval_Min16bithi8;
    uint8 interval_Max16bitlo8;
    uint8 interval_Max16bithi8;
    uint8 latency16bitlo8;
    uint8 latency16bithi8;
    uint8 timeout16bitlo8;
    uint8 timeout16bithi8;
    uint8 preferredPeriodicity8bit;
    uint8 referenceConnEventCount16bitlo8;
    uint8 referenceConnEventCount16bithi8;
    uint8 offset016bitlo8;
    uint8 offset016bithi8;
    uint8 offset116bitlo8;
    uint8 offset116bithi8;
    uint8 offset216bitlo8;
    uint8 offset216bithi8;
    uint8 offset316bitlo8;
    uint8 offset316bithi8;
    uint8 offset416bitlo8;
    uint8 offset416bithi8;
    uint8 offset516bitlo8;
    uint8 offset516bithi8;
}  t_connection_param_req_payload, t_connection_param_rsp_payload;

//Figure 2.28: CtrData field of the LL_REJECT_IND_EXT PDU
typedef struct
{
    uint8 opcode;
    uint8 rejectOpcode8bit;
    uint8 errorCode8bit;
}   t_reject_ind_ext_payload;


//Figure 2.29: CtrData field of the LL_LENGTH_REQ LL_LENGTH_RSP
typedef struct
{
    uint8 opcode;
    uint8  maxRxOctets16bitlo8;
    uint8  maxRxOctets16bithi8;
    uint8  maxRxTime16bitlo8;
    uint8  maxRxTime16bithi8;
    uint8  maxTxOctets16bitlo8;
    uint8  maxTxOctets16bithi8;
    uint8  maxTxTime16bitlo8;
    uint8  maxTxTime16bithi8;
}   t_length_req_payload, t_length_rsp_payload;

typedef struct
{
    uint8 opcode;
    uint8  txPhys;
    uint8  rxPhys;
}   t_phy_req_payload, t_phy_rsp_payload;


typedef struct
{
    uint8 opcode;
    uint8  M2SPhy;
    uint8  S2MPhy;
    uint8 instant16bitlo;   //(2 octets)
    uint8 instant16bithi;   //(2 octets)    
}   t_phy_update_ind_payload;

typedef union
{
    t_connection_update_req_payload * connection_update_req_payload;
    t_channel_map_req_payload * channel_map_req_payload;
    t_terminate_ind_payload * terminate_ind_payload;
    t_reject_ind_payload *reject_ind_payload;
    t_enc_req_payload * enc_req_payload;
    t_enc_rsp_payload * enc_rsp_payload;
    t_start_enc_req_payload * start_enc_req_payload;
    t_start_enc_rsp_payload *start_enc_rsp_payload;
    t_pause_enc_req_payload *pause_enc_req_payload;
    t_pause_enc_rsp_payload * pause_enc_rsp_payload;
    t_ping_req_payload * ping_req_payload;
    t_ping_rsp_payload * ping_rsp_payload;
    t_unknown_rsp_payload * unknown_rsp_payload;
    t_feature_req_payload *feature_req_payload;
    t_feature_rsp_payload * feature_rsp_payload;
    t_slave_feature_req_payload * slave_feature_req_payload;
    t_version_ind_payload * version_ind_payload;
    t_connection_param_req_payload * connection_param_req_payload;
    t_connection_param_rsp_payload * connection_param_rsp_payload;
    t_reject_ind_ext_payload * reject_ind_ext_payload;
    t_length_req_payload * length_req_payload;
    t_length_rsp_payload * length_rsp_payload;
    t_phy_req_payload *  phy_req_payload;
    t_phy_rsp_payload *  phy_rsp_payload;
    t_phy_update_ind_payload * 	phy_update_ind_payload;
	

} ut_ctrl_pdu_payload;


typedef struct
{
    ut_data_ch_pdu_header header16bit;
    ut_ctrl_pdu_payload      payload;
} t_ctrl_pdu;




//adv pdu mgr
typedef struct
{
    uint8 * adv_in_ptr;
    uint8 * adv_out_ptr;
    uint8 * scan_rsp_out_ptr;
    uint8 * conn_req_out_ptr;
} adv_data_mgr;

//control pdu buffer
typedef struct
{
    uint8 * ctrl_pdu_in_ptr[CTRL_PDU_IN_DATA_NUM];
    uint8 * ctrl_pdu_out_ptr[MAX_CONNECTIONS][CTRL_PDU_OUT_DATA_NUM];
    uint16    ctrl_pdu_out_used;
    uint16    ctrl_pdu_out_used_commit;
    uint8    ctrl_pdu_in_used;
} ctrl_pdu_mgr;


//data pdu mgr
typedef struct
{
    uint8 * data_ptr[MAX_DATA_PDU_BUFFER_NUM];
    uint8    data_in_used;
    uint8    data_out_used;
    data_ptr_list data_list[MAX_DATA_PDU_BUFFER_NUM];
    data_ptr_list *free_list;
    uint8    data_in_list_used_num;
    uint8    data_out_list_used_num;
} data_pdu_mgr;


extern adv_data_mgr  Adv_Data;
extern ctrl_pdu_mgr Ctrl_PDU_Data;
extern data_pdu_mgr Data_PDU_Data;
extern void release_data_ptr_list(data_ptr_list ** data_ptr_list, uint8 is_data_in);
extern uint8 * get_avail_data_buffer(uint8 is_data_in);
extern void release_data_buffer(uint8 * data_ptr, uint8 is_data_in);
extern data_ptr_list * get_free_data_ptr_list(uint8 is_data_in);
extern data_ptr_list ** get_data_ptr_list_end(data_ptr_list * data_list);
extern void delete_data_ptr_list(data_ptr_list * former_list, data_ptr_list ** delete_list);
extern void ll_tx_second_ctrl_pdu(uint8 idx, uint8 type);
extern void ll_lsn_end_dispatch(void);
extern void ll_tx_end_dispatch(void);
extern void ll_rx_crc_ok_dispatch(void);
extern void ll_rx_crc_err_dispatch(void);
extern void ll_rx_lost_dispatch(void);
extern uint8 get_avail_data_num(void);

extern uint8 g_conn_new_itv;

#endif

