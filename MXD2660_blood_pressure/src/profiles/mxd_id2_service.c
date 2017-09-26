
//#include "stack_ctrl.h"
#include "gatts_api.h"

#include "mem.h"

#include "hci.h"
#include "Timer.h"


#define MXD_ID2_SERVICE_UUID 0x1D20
#define MXD_ID2_CONFIG_UUID 0x1D21
#define MXD_ID2_TRX_UUID 0x1D22
enum
{
    MSG_PHONE_GET_ID2_SIGN=0x10,
    MSG_DEV_SEND_ID2=0x11,
    MSG_DEV_SEND_SIGN=0x12,
    MSG_PHONE_SEND_SEED=0x13,
    MSG_DEV_SEND_SIGN_OF_SEED=0x14,
    MSG_PHONE_SEND_ENCRYPTED_LINKKEY=0x15,
    MSG_PHONE_SEND_PHONEID=0x16,

    MSG_INTER_CHECK_REQ=0x21,
    MSG_ID_CHECK_RSP=0x22,
    MSG_SEND_CONFIRM=0x23,
    MSG_SEND_RAND=0x24,
    MSG_CONFIRM_FAIL_WILL_DISCONNECT=0x25,
    MSG_INTER_CHECK_COMPLETE=0x26,
    MSG_IRK_REQ=0x31,
    MSG_SEND_IRK=0x32,
    MSG_UPDATE_IRK_REQ=0x33,
    MSG_UPDATA_IRK_RSP=0x34,


};
typedef struct
{
    uint8  phoneID[16] ;
    uint8  linkkey[16];
} phone_info_t;
typedef struct
{
    int  cnt;
    phone_info_t  phone[6];
} id2_info_t;
id2_info_t g_id2_info;
uint8 g_idx=0xff;
uint8 g_id_valid=0;
void reverse_array(uint8_t *src,  int len)
{
    int i;
    uint8_t tmp;
    for (i = 0; i < len/2; i++)
    {
        tmp=src[i];
        src[i]=src[len - 1 - i] ;
        src[len - 1 - i] =tmp;
    }
}
// general swap/endianess utils
void reverse_bytes(const uint8_t *src, uint8_t *dst, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[len - 1 - i] = src[i];
}
void reverse_128(const uint8_t * src, uint8_t * dst)
{
    reverse_bytes(src, dst, 16);
}
//static uint16  id2_read_attrCB(uint16  con_handle, uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);
static uint16  id2_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);
static uint32 id2_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);
//static int id2_write_attrCB(uint16  con_handle, uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);
static uint16  id2_getAttrLenCB(uint16 attribute_handle);
gatt_serviceCBs_t ididCBs =
{
	id2_getAttrLenCB,
    id2_read_attrCB,  // Read callback function pointer
    id2_write_attrCB, // Write callback function pointer
};
enum
{
    MXD_ID2_CONFIG_IDX=0,
    MXD_ID2_CONFIG_CCD_IDX,
    MXD_ID2_TRX_IDX,
    MXD_ID2_TRX_CCD_IDX,

};
typedef enum
{
    RUN_AES_IDLE=0,
    RUN_AES_GET_ENC,
    RUN_AES_W4_ENC,
    RUN_AES_OVER_ENC,

} aes_run_state_t;
enum
{
    APP_STATE_FOR_IDLE=0,
    APP_STATE_FOR_MSG_SEND_IRK=1,
    APP_STATE_FOR_MSG_UPDATA_IRK_RSP ,
    APP_STATE_FOR_calc_confirmB,
    APP_STATE_FOR_calc_confirmA,
    APP_STATE_FOR_calc_PRIVATE_ADDR,


}  ;
static int appaes_state=0;
static int app_state=0;
static int app_state_need_send_confirmA=0;
static uint16 id2_server_hdl[4];
static uint8 id2_config_ccd=0;
static uint8 id2_trx_ccd=0;

uint8  randA[16]={0xbf,0x01,0xfb,0x9d,0x4e,0xf3,0xbc,0x36,0xd8,0x74,0xf5,0x39,0x41,0x38,0x68,0x4c};
uint8  randB[16];
uint8  ConfirmA[16];
uint8  ConfirmB[16];
uint8  ConfirmB_s[16];

uint8 private_addr[8];
extern unsigned char* sm_get_irk();
extern void sm_update_irk();
static char id2id[17];
static char sid[4];
static char seed[6];
static char sign[32];
static char sign_of_seed[32];
static char encrypt_linkkey[32];
static uint8  app_aes_key[16];
static uint8  app_aes_data[16];

void print_hex(char * msg,uint8 *data,uint8 len)
{
    int i=0;
    APP_DBG(("[%s][%d]:\n",msg,len));
    for (i=0;i<len;i++)
    {
        APP_DBG(("%x,",data[i]));
    }
    APP_DBG(("\n"));
}

void id2_test_set_phoneinfo();

extern uint8 timer_create(timer_node_t *timer, TIMER_MODE mode, timer_out_handler timeout_handler);
extern void mxd_gen_private_addr(uint8_t* paddr);
static timer_node_t timer_private_addr;
static timer_node_t timer2;
static timer_node_t timer8;

void timeout_handler2(void* para);
void timeout_handler8(void* para);
void timeout_handler_private(void* para);

uint32 id2_init_service(void)
{
    uint16 hdl = 0;
    hdl = GATTS_addServiceStart(0, MXD_ID2_SERVICE_UUID, NULL, &ididCBs);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, MXD_ID2_CONFIG_UUID, NULL, (ATT_PROPERTY_WRITE | ATT_PROPERTY_NOTIFY |ATT_PROPERTY_DYNAMIC), NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    id2_server_hdl[MXD_ID2_CONFIG_IDX] = hdl;

    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    id2_server_hdl[MXD_ID2_CONFIG_CCD_IDX] = hdl;
    hdl = GATTS_addChar(0, MXD_ID2_TRX_UUID, NULL, (ATT_PROPERTY_WRITE|ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    id2_server_hdl[MXD_ID2_TRX_IDX] = hdl;

    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    id2_server_hdl[MXD_ID2_TRX_CCD_IDX] = hdl;

    GATTS_addServiceEnd();


    id2_test_set_phoneinfo();
    timer_create(&timer_private_addr,APP_TIMER_MODE_REPEATED,timeout_handler_private);

    timer_create(&timer8,APP_TIMER_MODE_REPEATED,timeout_handler8);
    timer_create(&timer2,APP_TIMER_MODE_REPEATED,timeout_handler2);
    return  0;

}
static uint16  id2_getAttrLenCB(uint16 attribute_handle)
{
    if (id2_server_hdl[MXD_ID2_CONFIG_CCD_IDX] == attribute_handle)
    {

        return 2;
    }
    else if (id2_server_hdl[MXD_ID2_TRX_CCD_IDX] == attribute_handle)
    {

        return 2;

    }
    return 0;
}
static uint16  id2_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size)
{
    if (id2_server_hdl[MXD_ID2_CONFIG_CCD_IDX] == attribute_handle)
    {
        if (buffer)
        {
            buffer[0] = id2_config_ccd;
        }
        return 2;
    }
    else if (id2_server_hdl[MXD_ID2_TRX_CCD_IDX] == attribute_handle)
    {
        if (buffer)
        {
            buffer[0] = id2_trx_ccd;
        }
        return 2;

    }
//ble_chg_adv_data();
    return 0;
}
void id2_config_receive(uint8 *data, uint16 length);
void id2_trx_receive(uint8 *data, uint16 length);

void timeout_handler8(void* para)
{
    if (app_state  == APP_STATE_FOR_IDLE)
    {
        uint8 rsp[24];
        rsp[0]=MSG_SEND_CONFIRM;

        memcpy(rsp+1,ConfirmA,16);
        id2_config_transfer(rsp,17);
        print_hex("timeout_handler8 send MSG_SEND_CONFIRM ---ConfirmA ",rsp,17);
        timer_stop(&timer8);
        // ble_chg_adv_data(ConfirmA, 2,app_state);
        app_state_need_send_confirmA=0;
    }

}
static uint32 id2_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size)
{

    print_hex("id2_write_attrCB----in",buffer, buffer_size);
    if (id2_server_hdl[MXD_ID2_TRX_IDX] == attribute_handle)
    {

       // hci_data_id2_event(MXD_ID2_TRX_IDX, buffer, buffer_size);
    }
    else if (id2_server_hdl[MXD_ID2_CONFIG_IDX] == attribute_handle)
    {
        print_hex("id2_write_attrCB----MXD_ID2_CONFIG_IDX",buffer, buffer_size);

       // hci_data_id2_event(MXD_ID2_CONFIG_IDX, buffer, buffer_size);
    }
    else if (id2_server_hdl[MXD_ID2_CONFIG_CCD_IDX] == attribute_handle)
    {
        id2_config_ccd= buffer[0];
    }
    else if (id2_server_hdl[MXD_ID2_TRX_CCD_IDX] == attribute_handle)
    {

        id2_trx_ccd = buffer[0];
    }

    return 0;
}



int id2_config_transfer(uint8 *data, uint16 length)
{
    int ret_code = GATTS_send_notify(id2_server_hdl[MXD_ID2_CONFIG_IDX],data,length);
    return ret_code;
}
int id2_trx_transfer(uint8 *data, uint16 length)
{
    int ret_code = GATTS_send_notify(id2_server_hdl[MXD_ID2_TRX_IDX],data,length);
    return ret_code;
}



void id2_update_phone_info_for_new_irk()
{
    if (0==g_idx)
    {
        g_id2_info.cnt=1;

    }
    else
    {
        memcpy(g_id2_info.phone[0].phoneID,g_id2_info.phone[g_idx].phoneID,16);
        memcpy(g_id2_info.phone[0].linkkey,g_id2_info.phone[g_idx].linkkey,16);
        g_idx=0;
        g_id2_info.cnt=1;
    }

}

void id2_test_set_phoneinfo()
{
    /*
        uint8 key[16]={0xbf,0x01,0xfb,0x9d,0x4e,0xf3,0xbc,0x36,0xd8,0x74,0xf5,0x39,0x41,0x38,0x68,0x4c};
        uint8 id[16]={0x1,0x02,0x3,0xe0,0xdf,0xce,0xbd,0xac,0x79,0x68,0x57,0x46,0x35,0xcc,0xdc,0xd2};
        g_id2_info.cnt=1;
        memcpy(g_id2_info.phone[0].linkkey,key,16);
        memcpy(g_id2_info.phone[0].phoneID,id,16);
    */
    char *id2 = "Y00F3400105328E71" ;
    char *si="reCjVR2p9gGdBOshRHAqZFp4bmzQgUUG";
    char *sign_of_sd="0CC175B9C0F1B6A831C399E269772661";
    memcpy(id2id,id2,17);
    memcpy(sign,si,32);
    memcpy(sign_of_seed,sign_of_sd,32);
    g_id2_info.cnt=0;
    // APP_DBG("id2=%s\n",id2id);
    //  APP_DBG("sign=%s\n",sign);

}
void id2_config_receive(uint8 *data, uint16 length)
{
    int i=0;
    uint8 opcode=data[0];
    uint8 rsp[24];
    print_hex("id2_config_receive",data,length);
    switch (opcode)
    {
        case     MSG_PHONE_GET_ID2_SIGN:
        {
            memcpy(sid,data+3,4);
            //todo SE -get id2id and sign   sign=3des(md5("id2id~sid"));
            print_hex("MSG_PHONE_GET_ID2_SIGN",data+3,4);
            {
                rsp[0]=MSG_DEV_SEND_ID2;
                rsp[1]=1;
                rsp[2]=1;
                memcpy(rsp+3,id2id,17);
                id2_config_transfer(rsp,20);
                print_hex("send id2 id",rsp,20);

            }
            {
                rsp[0]=MSG_DEV_SEND_SIGN;
                rsp[1]=1;
                rsp[2]=2;
                memcpy(rsp+3,sign,17);
                id2_config_transfer(rsp,20);
                print_hex("send sign 1",rsp,20);

                rsp[1]=2;
                rsp[2]=2;
                memcpy(rsp+3,sign+17,15);
                id2_config_transfer(rsp,18);
                print_hex("send sign 2",rsp,18);

            }

        }
        break;

        case  MSG_PHONE_SEND_SEED:
        {
            memcpy(seed,data+3,6);
            print_hex("MSG_PHONE_SEND_SEED",data+3,6);
            //todo SE -    sign_of_seed=3des(md5("sid~seed"));
            {
                rsp[0]=MSG_DEV_SEND_SIGN_OF_SEED;
                rsp[1]=1;
                rsp[2]=2;
                memcpy(rsp+3,sign_of_seed,17);
                id2_config_transfer(rsp,20);
                print_hex("send sign_of_seed 1",rsp,20);
                rsp[1]=2;
                rsp[2]=2;
                memcpy(rsp+3,sign_of_seed+17,15);
                id2_config_transfer(rsp,18);
                print_hex("send sign_of_seed 2",rsp,18);

            }

        }
        break;

        case MSG_PHONE_SEND_ENCRYPTED_LINKKEY:
        {
            uint8 seg_idx=data[1];
            //uint8 seg_total=data[2];
            if (1==seg_idx)
            {
                print_hex("MSG_PHONE_SEND_ENCRYPTED_LINKKEY 1",data+3,length-3);
                memcpy(encrypt_linkkey,data+3,length-3);
            }
            else if (2==seg_idx)
            {
                print_hex("MSG_PHONE_SEND_ENCRYPTED_LINKKEY 2",data+3,length-3);
                memcpy(encrypt_linkkey+32+3-length,data+3,length-3);
            }


        }
        break;
        case  MSG_PHONE_SEND_PHONEID:
        {
            //todo SE -    clear_linkkey=decrypt(encrypt_linkkey);
            int idx=g_id2_info.cnt;
            if (g_id2_info.cnt>=5)
            {
                idx=0; //replace first phone
            }
            print_hex("MSG_PHONE_SEND_PHONEID ",data + 3,16);
            memcpy(g_id2_info.phone[idx].phoneID,data + 3,16);
            //todo SE -    clear_linkkey=decrypt(encrypt_linkkey);
            //todo memcpy(g_id2_info.phone[idx].linkkey,clear_linkkey,16);
            {//this is test code
                uint8 testkey[16]={0xbf,0x01,0xfb,0x9d,0x4e,0xf3,0xbc,0x36,0xd8,0x74,0xf5,0x39,0x41,0x38,0x68,0x4c};
                memcpy(g_id2_info.phone[idx].linkkey,testkey,16);
                //todo: se decrypt encrypt_linkkey
            }

            if (g_id2_info.cnt < 5)
            {
                g_id2_info.cnt++;
            }
        }
        break;

        case MSG_INTER_CHECK_REQ:
        {
            rsp[0]=MSG_ID_CHECK_RSP;
            if (g_id2_info.cnt==0)
            {
                rsp[1]=1;
                g_idx=0xff;
                id2_config_transfer(rsp,2);
                APP_DBG(("MSG_INTER_CHECK_REQ error  g_id2_info.cnt==0"));
                return;
            }
            i=0;
            for (i=0;i<g_id2_info.cnt;i++)
            {
                if (0==memcmp(g_id2_info.phone[i].phoneID,data+1,16))
                {
                    rsp[1]=0;
                    id2_config_transfer(rsp,2);

                    g_idx=i;
                    app_state =  APP_STATE_FOR_calc_confirmA;
                    sm_start_aes_calc(g_id2_info.phone[g_idx].linkkey,randA);
                    print_hex("MSG_INTER_CHECK_REQ ---randA ",randA,16);
                    return;
                }

            }
            if (g_id2_info.cnt==i)
            {
                rsp[1]=1;
                g_idx=0xff;
                id2_config_transfer(rsp,2);
                print_hex(" send MSG_ID_CHECK_RSP ---randA ",rsp,2);
                return;
            }
        }
        break;
        case MSG_SEND_CONFIRM:
            rsp[0]=MSG_SEND_CONFIRM;
            memcpy(ConfirmB_s,data+1,16);
            print_hex("   MSG_SEND_CONFIRM ---ConfirmB ",data+1,16);
            if (app_state  == APP_STATE_FOR_calc_confirmA)
            {
                app_state_need_send_confirmA=1;
                timer_start(&timer8, 150, NULL);
                rsp[1]=appaes_state;
            }
            else
            {
                memcpy(rsp+1,ConfirmA,16);
                id2_config_transfer(rsp,17);
                app_state_need_send_confirmA=0;
                print_hex(" send MSG_SEND_CONFIRM ---ConfirmA ",rsp,17);
            }
            break;
        case MSG_SEND_RAND:
            rsp[0]=MSG_SEND_RAND;
            memcpy(randB,data+1,16);
            print_hex("   MSG_SEND_RAND ---randB ",data+1,16);
            app_state =  APP_STATE_FOR_calc_confirmB;
            sm_start_aes_calc(g_id2_info.phone[g_idx].linkkey,randB);

            break;
        case MSG_CONFIRM_FAIL_WILL_DISCONNECT:
            g_id_valid=0;
            break;
        case MSG_INTER_CHECK_COMPLETE:
            g_id_valid=1;
            break;

        case MSG_IRK_REQ:
            if (0==g_id_valid)
            {
                rsp[0]=MSG_ID_CHECK_RSP;
                rsp[1]=1;
                g_idx=0xff;
                id2_config_transfer(rsp,2);
                APP_DBG(("   MSG_IRK_REQ ---send error "));
                return;
            }
            {
                //uint8_t key[16];
                uint8_t * pkey=sm_get_irk();
                //   reverse_128(pkey, key);
                app_state =  APP_STATE_FOR_MSG_SEND_IRK;
                sm_start_aes_calc(g_id2_info.phone[g_idx].linkkey,pkey);

                APP_DBG(("   MSG_IRK_REQ --APP_STATE_FOR_MSG_SEND_IRK"));

            }

            break;
        case MSG_UPDATE_IRK_REQ:
            if (0==g_id_valid)
            {
                rsp[0]=MSG_ID_CHECK_RSP;
                rsp[1]=1;
                g_idx=0xff;
                id2_config_transfer(rsp,2);
                return;
            }
            {
                //uint8_t key[16];
                uint8_t * pkey;

                sm_update_irk();
                pkey=sm_get_irk();
                //  reverse_128(pkey, key);

                app_state =  APP_STATE_FOR_MSG_UPDATA_IRK_RSP;
                sm_start_aes_calc(g_id2_info.phone[g_idx].linkkey,pkey);

            }

            break;

        default:
            break;

    }

}
void id2_trx_receive(uint8 *data, uint16 length)
{


}

void id2_receive_handler(uint8 *data, uint16 length)
{
    uint8 type=data[0];
    print_hex("id2_receive_handler",data,length);
    if (MXD_ID2_TRX_IDX == type)
    {
        id2_trx_receive(data+1,   length-1);

    }
    else if (MXD_ID2_CONFIG_IDX  == type)
    {

        id2_config_receive(data+1,   length-1);

    }
}
#if 1

void timeout_handler2(void* para)
{


    if (RUN_AES_GET_ENC == appaes_state)
    {

        uint8 sta= gap_aes_encryption(app_aes_key,  app_aes_data);


        if (sta == TRUE)
        {
            appaes_state = RUN_AES_W4_ENC;
            timer_stop(&timer2);

        }
        else
        {
            appaes_state = RUN_AES_GET_ENC;
        }
    }
    else
    {
        timer_stop(&timer2);
    }
}
void sm_start_aes_calc(uint8* key,uint8* data)
{


    memcpy(app_aes_key,key, 16);
    memcpy(app_aes_data,data,16);

    uint8 sta= gap_aes_encryption(key,  data);


    if (sta == TRUE)
    {
        appaes_state = RUN_AES_W4_ENC;

    }
    else
    {
        appaes_state = RUN_AES_GET_ENC;
        timer_start(&timer2, 190, NULL);
    }
}

void sm_aes_for_app_run(uint8 * data)
{
    uint8 rsp[24];
    {


        appaes_state = RUN_AES_IDLE;

        switch (app_state)
        {
            case APP_STATE_FOR_MSG_UPDATA_IRK_RSP:
            {
                rsp[0]=MSG_UPDATA_IRK_RSP;
                memcpy(rsp+1,data,16);
                id2_config_transfer(rsp,17);
                id2_update_phone_info_for_new_irk();
                app_state= APP_STATE_FOR_IDLE;
            }
            break;
            case APP_STATE_FOR_MSG_SEND_IRK:
            {
                memcpy(rsp+1,data,16);//reverse_128(data, rsp+1);
                rsp[0]=MSG_SEND_IRK;

                id2_config_transfer(rsp,17);
                print_hex(" send  MSG_SEND_IRK  ",rsp,17);
                app_state= APP_STATE_FOR_IDLE;
            }
            break;

            case APP_STATE_FOR_calc_confirmB:
            {
                memcpy(ConfirmB,data,16);// reverse_128(data,ConfirmB);
                print_hex("   APP_STATE_FOR_calc_confirmB ---ConfirmB ",data,16);
                if (0==memcmp(ConfirmB,ConfirmB_s,16))
                {
                    rsp[0]=MSG_SEND_RAND;
                    memcpy(rsp+1,randA,16);
                    id2_config_transfer(rsp,17);

                    print_hex(" send  MSG_SEND_RAND ---randA ",rsp,17);
                    g_id_valid=1;
                }
                else
                {
                    rsp[0]=MSG_CONFIRM_FAIL_WILL_DISCONNECT;
                    id2_config_transfer(rsp,1);
                    print_hex(" send  MSG_CONFIRM_FAIL_WILL_DISCONNECT ---ConfirmB_s ",ConfirmB_s,16);
                    g_id_valid=0;
                }
                app_state= APP_STATE_FOR_IDLE;
            }
            break;

            case APP_STATE_FOR_calc_confirmA:
            {
                memcpy(ConfirmA,data,16);//  reverse_128(data,ConfirmA);
                app_state= APP_STATE_FOR_IDLE;

            }
            break;
            case APP_STATE_FOR_calc_PRIVATE_ADDR:
            {

                private_addr[3]=data[2];
                private_addr[4]=data[1];
                private_addr[5]=data[0];
                print_hex("private addr",private_addr,6);
                app_state= APP_STATE_FOR_IDLE;

            }
            break;
            default:
                break;

        }

    }

}

void id2_gen_private_addr()
{
    uint8_t plaint[16];


    uint8_t * pkey=sm_get_irk();
    print_hex("sm_get_irk",pkey,16);
    memset(plaint,0,16);
    get_rand_128(plaint);

    plaint[2]=0x40|(0x3f&plaint[2]);
    private_addr[0]=plaint[2];
    private_addr[1]=plaint[1];
    private_addr[2]=plaint[0];

    app_state =  APP_STATE_FOR_calc_PRIVATE_ADDR;
    sm_start_aes_calc(pkey,plaint);




}
void timeout_handler_private(void* para)
{
    // timer_start(&timer_private_addr, 5000, NULL);
}
#endif

