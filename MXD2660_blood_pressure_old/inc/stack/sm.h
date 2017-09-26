

#ifndef __SM_H
#define __SM_H


#if defined __cplusplus
extern "C" {
#endif

    /*typedef struct {
        btstack_linked_item_t  item;
        bd_addr_t      address;
        bd_addr_type_t address_type;
    } sm_lookup_entry_t;*/


    typedef uint8 sm_pairing_packet_t[7];

//
// SM internal types and globals
//

    typedef enum
    {
        DKG_W4_WORKING,
        DKG_CALC_IRK,
        DKG_W4_IRK,
        DKG_CALC_DHK,
        DKG_W4_DHK,
        DKG_READY
    } derived_key_generation_t;

    typedef enum
    {
        RAU_W4_WORKING,
        RAU_IDLE,
        RAU_GET_RANDOM,
        RAU_W4_RANDOM,
        RAU_GET_ENC,
        RAU_W4_ENC,
        RAU_SET_ADDRESS,
    } random_address_update_t;

    typedef enum
    {
        CMAC_IDLE,
        CMAC_CALC_SUBKEYS,
        CMAC_W4_SUBKEYS,
        CMAC_CALC_MI,
        CMAC_W4_MI,
        CMAC_CALC_MLAST,
        CMAC_W4_MLAST
    } cmac_state_t;

    typedef enum
    {
        JUST_WORKS,
        PK_RESP_INPUT,  // Initiator displays PK, responder inputs PK
        PK_INIT_INPUT,  // Responder displays PK, initiator inputs PK
        OK_BOTH_INPUT,  // Only input on both, both input PK
        NK_BOTH_INPUT,  // Only numerical compparison (yes/no) on on both sides
        OOB             // OOB available on both sides
    } stk_generation_method_t;

    typedef enum
    {
        SM_USER_RESPONSE_IDLE,
        SM_USER_RESPONSE_PENDING,
        SM_USER_RESPONSE_CONFIRM,
        SM_USER_RESPONSE_PASSKEY,
        SM_USER_RESPONSE_DECLINE
    } sm_user_response_t;

    typedef enum
    {
        SM_AES128_IDLE,
        SM_AES128_ACTIVE
    } sm_aes128_state_t;

    typedef enum
    {
        ADDRESS_RESOLUTION_IDLE,
        ADDRESS_RESOLUTION_GENERAL,
        ADDRESS_RESOLUTION_FOR_CONNECTION,
    } address_resolution_mode_t;

    typedef enum
    {
        ADDRESS_RESOLUTION_SUCEEDED,
        ADDRESS_RESOLUTION_FAILED,
    } address_resolution_event_t;

    typedef enum
    {
        EC_KEY_GENERATION_IDLE,
        EC_KEY_GENERATION_ACTIVE,
        EC_KEY_GENERATION_DONE,
    } ec_key_generation_state_t;

    typedef enum
    {
        SM_STATE_VAR_DHKEY_COMMAND_RECEIVED = 1 << 0
    } sm_state_var_t;

//
// SM internal types and globals
//

    typedef enum
    {

        // general states
        // state = 0
        SM_GENERAL_IDLE,// = 0
        SM_GENERAL_SEND_PAIRING_FAILED,// = 1
        SM_GENERAL_TIMEOUT, // = 2, // no other security messages are exchanged

        // Phase 1: Pairing Feature Exchange
        SM_PH1_W4_USER_RESPONSE,// = 3,

        // Phase 2: Authenticating and Encrypting

        // get random number for use as TK Passkey if we show it
        SM_PH2_GET_RANDOM_TK,// = 4,
        SM_PH2_W4_RANDOM_TK,// = 5,

        // get local random number for confirm c1
        SM_PH2_C1_GET_RANDOM_A,// = 6,
        SM_PH2_C1_W4_RANDOM_A,// = 7,
        SM_PH2_C1_GET_RANDOM_B,// = 8,
        SM_PH2_C1_W4_RANDOM_B,// = 9,

        // calculate confirm value for local side
        // state = 10
        SM_PH2_C1_GET_ENC_A,// = 0xA,
        SM_PH2_C1_W4_ENC_A,// = 0xB,
        SM_PH2_C1_GET_ENC_B,// = 0xC,
        SM_PH2_C1_W4_ENC_B,// = 0xD,

        // calculate confirm value for remote side
        SM_PH2_C1_GET_ENC_C,// = 0xE,
        SM_PH2_C1_W4_ENC_C,// = 0xF,
        SM_PH2_C1_GET_ENC_D,// = 0x10,
        SM_PH2_C1_W4_ENC_D,// = 0x11,

        SM_PH2_C1_SEND_PAIRING_CONFIRM, // 0x12
        SM_PH2_SEND_PAIRING_RANDOM, //0x13

        // calc STK
        // state = 20
        SM_PH2_CALC_STK,//0x14
        SM_PH2_W4_STK,//0x15

        SM_PH2_W4_CONNECTION_ENCRYPTED,//0x16

        // Phase 3: Transport Specific Key Distribution
        // calculate DHK, Y, EDIV, and LTK
        SM_PH3_GET_RANDOM,//0x17
        SM_PH3_W4_RANDOM,//0x18
        SM_PH3_GET_DIV,//0x19
        SM_PH3_W4_DIV,//0x1A
        SM_PH3_Y_GET_ENC,//0x1B
        SM_PH3_Y_W4_ENC,//0x1C
        SM_PH3_LTK_GET_ENC,//0x1D
        // state = 30
        SM_PH3_LTK_W4_ENC,//0x1E
        SM_PH3_CSRK_GET_ENC,//0x1F
        SM_PH3_CSRK_W4_ENC,//0x20

        // exchange keys
        SM_PH3_DISTRIBUTE_KEYS,//0x21
        SM_PH3_RECEIVE_KEYS,//0x22

        // RESPONDER ROLE
        // state = 35
        SM_RESPONDER_IDLE,//0x23
        SM_RESPONDER_SEND_SECURITY_REQUEST,//0x24
        SM_RESPONDER_PH0_RECEIVED_LTK_REQUEST,//0x25
        SM_RESPONDER_PH0_SEND_LTK_REQUESTED_NEGATIVE_REPLY,//0x26
        SM_RESPONDER_PH1_W4_PAIRING_REQUEST,//0x27
        SM_RESPONDER_PH1_PAIRING_REQUEST_RECEIVED,//0x28
        SM_RESPONDER_PH1_SEND_PAIRING_RESPONSE,//0x29
        SM_RESPONDER_PH1_W4_PAIRING_CONFIRM,//0x2A
        SM_RESPONDER_PH2_W4_PAIRING_RANDOM,//0x2B
        SM_RESPONDER_PH2_W4_LTK_REQUEST,//0x2C
        SM_RESPONDER_PH2_SEND_LTK_REPLY,//0x2D

        // Phase 4: re-establish previously distributed LTK
        // state == 46
        SM_RESPONDER_PH4_Y_GET_ENC,//0x2E
        SM_RESPONDER_PH4_Y_W4_ENC,//0x2F
        SM_RESPONDER_PH4_LTK_GET_ENC,//0x30
        SM_RESPONDER_PH4_LTK_W4_ENC,//0x31
        SM_RESPONDER_PH4_SEND_LTK_REPLY,//0x32

        // INITITIATOR ROLE
        // state = 51
        SM_INITIATOR_CONNECTED,//0x33
        SM_INITIATOR_PH0_HAS_LTK,//0x34
        SM_INITIATOR_PH0_SEND_START_ENCRYPTION,//0x35
        SM_INITIATOR_PH0_W4_CONNECTION_ENCRYPTED,//0x36
        SM_INITIATOR_PH1_W2_SEND_PAIRING_REQUEST,//0x37
        SM_INITIATOR_PH1_SEND_PAIRING_REQUEST,//0x38
        SM_INITIATOR_PH1_W4_PAIRING_RESPONSE,//0x39
        SM_INITIATOR_PH2_W4_PAIRING_CONFIRM,//0x3A
        SM_INITIATOR_PH2_W4_PAIRING_RANDOM,//0x3B
        SM_INITIATOR_PH3_SEND_START_ENCRYPTION,//0x3C

        // LE Secure Connections
        SM_SC_SEND_PUBLIC_KEY_COMMAND,//0x3D
        SM_SC_W4_PUBLIC_KEY_COMMAND,//0x3E
        SM_SC_W2_GET_RANDOM_A,//0x3F
        SM_SC_W4_GET_RANDOM_A,//0x40
        SM_SC_W2_GET_RANDOM_B,//0x41
        SM_SC_W4_GET_RANDOM_B,//0x42
        SM_SC_W2_CMAC_FOR_CONFIRMATION,//0x43
        SM_SC_W4_CMAC_FOR_CONFIRMATION,//0x44
        SM_SC_SEND_CONFIRMATION,//0x45
        SM_SC_W2_CMAC_FOR_CHECK_CONFIRMATION,//0x46
        SM_SC_W4_CMAC_FOR_CHECK_CONFIRMATION,//0x47
        SM_SC_W4_CONFIRMATION,//0x48
        SM_SC_SEND_PAIRING_RANDOM,//0x49
        SM_SC_W4_PAIRING_RANDOM,//0x4A
        SM_SC_W2_CALCULATE_G2,//0x4B
        SM_SC_W4_CALCULATE_G2,//0x4C
        SM_SC_W2_CALCULATE_F5_SALT,//0x4D
        SM_SC_W4_CALCULATE_F5_SALT,//0x4E
        SM_SC_W2_CALCULATE_F5_MACKEY,//0x4F
        SM_SC_W4_CALCULATE_F5_MACKEY,//0x50
        SM_SC_W2_CALCULATE_F5_LTK,//0x51
        SM_SC_W4_CALCULATE_F5_LTK,//0x52
        SM_SC_W2_CALCULATE_F6_FOR_DHKEY_CHECK,//0x53
        SM_SC_W4_CALCULATE_F6_FOR_DHKEY_CHECK,//0x54
        SM_SC_W2_CALCULATE_F6_TO_VERIFY_DHKEY_CHECK,//0x55
        SM_SC_W4_CALCULATE_F6_TO_VERIFY_DHKEY_CHECK,//0x56
        SM_SC_W4_USER_RESPONSE,//0x57
        SM_SC_SEND_DHKEY_CHECK_COMMAND,//0x58
        SM_SC_W4_DHKEY_CHECK_COMMAND,//0x59
        SM_SC_W4_LTK_REQUEST_SC,//0x5A
        SM_SC_W2_CALCULATE_H6_ILK,//0x5B
        SM_SC_W4_CALCULATE_H6_ILK,//0x5C
        SM_SC_W2_CALCULATE_H6_BR_EDR_LINK_KEY,//0x5D
        SM_SC_W4_CALCULATE_H6_BR_EDR_LINK_KEY//0x5E
    } security_manager_state_t;

// Authorization state
    typedef enum
    {
        AUTHORIZATION_UNKNOWN,
        AUTHORIZATION_PENDING,
        AUTHORIZATION_DECLINED,
        AUTHORIZATION_GRANTED
    } authorization_state_t;

//
// SM internal types and globals
//

    typedef enum
    {
        IRK_LOOKUP_IDLE,
        IRK_LOOKUP_W4_READY,
        IRK_LOOKUP_STARTED,
        IRK_LOOKUP_SUCCEEDED,
        IRK_LOOKUP_FAILED
    } irk_lookup_state_t;

    typedef struct
    {
        uint8                  sm_role;   // 0 - IamMaster, 1 = IamSlave
        uint8                  sm_security_request_received;
        uint8                  sm_bonding_requested;
        uint8                  sm_peer_addr_type;
        uint8                  sm_peer_address[6];
        security_manager_state_t sm_engine_state;
        irk_lookup_state_t      sm_irk_lookup_state;
        uint8                  sm_connection_encrypted;
        uint8                  sm_connection_authenticated;   // [0..1]
        uint8                  sm_actual_encryption_key_size;
        uint8                  sm_m_preq[7];  // only used during c1
        authorization_state_t    sm_connection_authorization_state;
        uint16                 sm_local_ediv;
        uint8                  sm_local_rand[8];
        int32                      sm_le_db_index;
    } sm_connection;

    extern void sm_handle_encryption_result(uint8 idx, uint8 * data);
    extern void sm_run(uint8 idx);
    extern void sm_cmac_run(uint8 * data);
    extern void sm_dkg_run(uint8 * data);
    extern void sm_ecdh_run(void);
    extern void sm_rau_run(uint8 * data);
    extern void sm_csrk_run(uint8 idx, uint8 * data);
    extern void sm_event_disconn_complete(uint8 idx);
    extern void sm_event_conn_complete(uint8 idx);
    extern void sm_init(void);
    extern void sm_just_works_confirm(uint8 idx);
    extern uint8 sm_get_idx_from_sm_conn(sm_connection * sm_conn);
    extern void sm_event_encrypt_change(uint8 idx);
    extern void sm_event_key_refresh(uint8 idx);
    extern void sm_dhk_init(void);
    extern void sm_sc_calculate_dhkey(void);
    extern void sm_pdu_handler(uint8 idx, uint8 *packet, uint16 size);
    extern void sm_request_pairing(uint8 idx, uint8 iocap, uint8 authreq, uint8 keysize);
    extern void sm_event_long_term_key(uint8 idx);
    extern void sm_cmac_signed_write_start(uint8 * k, uint8 size, uint8 * packet, void (*done_handler)(uint8 * hash));
    extern void (*sm_cmac_done_handler)(uint8 * hash);
    extern int sm_cmac_ready(void);
    extern int sm_le_device_index(uint8 idx);
    extern void sm_send_security_request(uint8 idx);
    extern void sm_passkey_input(uint8 idx, uint32 passkey);
    extern void sm_para_update(uint8 iocap, uint8 authreq, uint8 keysize);
    extern void sm_authorization_decline(uint8 idx);
    extern void sm_authorization_grant(uint8 idx);
    extern void sm_bonding_decline(uint8 idx);
    extern void sm_update_irk(void);
    extern uint8* sm_get_irk(void);
    extern void sm_bond_info_save_by_app_config(uint32 onoff);

	
    extern sm_aes128_state_t  sm_aes128_state;
    extern uint8     sm_cmac_message_idx;
    extern uint8 * sm_cmac_message;
    extern uint16     sm_cmac_message_len;
    extern uint8 g_ec_enable;
    extern uint8 g_ec_is_pk;
    extern uint8 g_ec_idx;
    extern uint8 g_ec_d[32] ;
    extern cmac_state_t sm_cmac_state;


//#define ENABLE_LE_SECURE_CONNECTIONS


#if defined __cplusplus
}
#endif

#endif // __SM_H
