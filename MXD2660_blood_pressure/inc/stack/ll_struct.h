/**
****************************************************************************
* @file ll_struct.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2017/1/19
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
/** @defgroup 模块名 模块的说明文字
* @{
*/
#ifndef __LL_STRUCT_H__
#define __LL_STRUCT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/** 枚举常量 */
typedef enum ADDR_TYPE
{
	PUBLIC_ADDR = 0,
    RANDOM_ADDR = 1,           
}ADDR_TYPE_ENUM;
/** 定义类型 ADDR_TYPE_ENUM */

typedef struct
{
    uint16 advIntlMin16bit;// 0x0020 to 0x4000      Default: N = 0x0800 (1.28 seconds)
    uint16 advIntlMax16bit;// 0x0020 to 0x4000  Default: N = 0x0800 (1.28 seconds) < 20 ms to 10.24 sec>  Time = N * 0.625 msec
    uint8 advType8bit;                     //T_ADVING_TYPE
    uint8 ownAddrType8bit; //0x00Public Device Address;  0x01Random Device Address; 0x02 – 0xFFReserved for future use;
    uint8 drctAddrType8bit;//0x00Public Device Address;  0x01Random Device Address; 0x02 – 0xFFReserved for future use;
    uint8 drctAddr6oct[6]; //Public Device Address or Random Device Address of the device to be connected.
    uint8 advChMap8bit;    // bit0 37ch; bit1 38ch; bit2 39ch; dft: 111b;
    uint8 advFltPolicy8bit;//0x00:all device               0x01:Cnnct all ScanWhiteLst             0x02:ScanAllCnnctWhiteLst               0x03:scanWhiteLstCnnctWhiteLst          0x04~0xff:Reserved
    uint8 none;
    uint8 advDataLen;
    uint8 *advDatabuf;
    uint8 scanRspLen;
    uint8 *scanRspbuf;
} t_adv_para;

typedef struct
{
    uint16      leScanInterval16bit;      // 0x04 to 0x4000(2.5ms to 10.24s)  Default: N = 0x0010 (10ms)          Time = N * 0.625 msec,
    uint16      leScanWindow16bit;       // 0x04 to 0x4000(2.5ms to 10.24s)  Default: N = 0x0010 (10ms)           Time = N * 0.625 msec,
    uint8        leScanType8bit;             // 0x00 : passive      0x01:active
    uint8        ownAddrType8bit;          //0x00Public Device Address(default);          0x01Random Device Address; 0x02 – 0xFFReserved for future use;
    uint8        scanFltPolicy8bit;           //0x00:all device           0x01:ScanWhiteLst               0x02~0xff reserved
    uint8        duplicateFilterEn;         //Duplicate filtering   1:enable.  0:disable
} t_scan_para;

typedef struct
{

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
} t_init_connct_para;

//subt
typedef   struct
{
    uint16          leScanInterval16bit;
    uint16          leScanWindow16bit;
    uint16          connIntervalMin16bit;
    uint16          connIntervalMax16bit;
    uint16          connLatency16bit;
    uint16          supervisionTimeout16bit;
    uint16          minimumCELength16bit;
    uint16          maximumCELength_16bit;
    uint8            peerAddress6oct[6];
    uint8            initiatorFilterPolicy8bit;
    uint8            peerAddressType8bit;
    uint8            ownAddressType8bit;
} t_conn_para;
#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint8  idx;
    uint16 intmin;
    uint16 intmax;
    uint16 latcy;
    uint16 tout;
} t_connct_update_para;


typedef struct
{
    uint16 addrType;	
    uint8  addr[6];
} t_white_list,t_device_addr;

#pragma pack(pop)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __LL_STRUCT_H__ */
/** @} */ // 模块结尾
