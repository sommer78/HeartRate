
#ifndef __MEM_H_
#define __MEM_H_

#include "device_cfg.h"

/** @name system related configuration
* @{
*/

#ifndef MULTI_LINK_FUNC_EN
    #define MAX_CONNECTIONS 1
    #define MAX_DATA_PDU_BUFFER_NUM 6
#else
    #define MAX_CONNECTIONS 4
    #define MAX_DATA_PDU_BUFFER_NUM 8
#endif




#define ADV_IN_DATA_SIZE 40
#define ADV_OUT_DATA_SIZE 40
#define SCAN_RSP_OUT_DATA_SIZE 40
#define CONN_REQ_OUT_DATA_SIZE 36
#define CTRL_PDU_IN_DATA_NUM  2
#define CTRL_PDU_IN_DATA_SIZE_BLOCK 32
#define CTRL_PDU_OUT_DATA_NUM  2
#define CTRL_PDU_IN_DATA_SIZE_TOTAL (CTRL_PDU_IN_DATA_SIZE_BLOCK*CTRL_PDU_IN_DATA_NUM)
#define CTRL_PDU_OUT_DATA_SIZE_BLOCK 32
#define CTRL_PDU_OUT_DATA_SIZE_TOTAL (CTRL_PDU_OUT_DATA_SIZE_BLOCK*CTRL_PDU_OUT_DATA_NUM*MAX_CONNECTIONS)
#define DATA_PDU_DATA_PAYLOAD_SIZE 256//256 for payload, 8 for hci acl header
#define DATA_PDU_DATA_SIZE_BLOCK (DATA_PDU_DATA_PAYLOAD_SIZE + 8)//256 for payload, 8 for hci acl header
#define DATA_PDU_DATA_SIZE_TOTAL (DATA_PDU_DATA_SIZE_BLOCK*MAX_DATA_PDU_BUFFER_NUM)

#define DATA_POOL_SIZE (ADV_IN_DATA_SIZE + ADV_OUT_DATA_SIZE + SCAN_RSP_OUT_DATA_SIZE + CONN_REQ_OUT_DATA_SIZE + CTRL_PDU_IN_DATA_SIZE_TOTAL + \
                                                CTRL_PDU_OUT_DATA_SIZE_TOTAL + DATA_PDU_DATA_SIZE_TOTAL)


enum ASSERT_FILE_ID
{
    _MEM_ = 0x1000,
    _MAIN_ = 0x1100,
    _HCI_  = 0x1200,
    _LL_CTRL_ = 0x1300,
    _LL_DATA_ = 0x1400,
    _LL_SCH_   = 0x1500,
    _LL_PHY_   = 0x1600,
    _STACK_CTRL_   = 0x1700,
    _ATT_DB_  = 0x1800,
    _ATT_SERVER_  = 0x1900,
    _SM_  = 0x1A00,
    _RTOS_   = 0x1B00,
    _GATT_CLIENT_  = 0x1C00,
    _L2CAP_SIGNALING_  = 0x1D00,


};


extern uint8 data_pool[DATA_POOL_SIZE];


extern void mem_ctrl_init(void);
extern void data_mem_init(void);
extern void system_assert(uint16 address, bool expression, uint32 info);
extern void delay_cycles(uint32 cycles);
extern void delay_us(uint32 microseconds);

#endif  /* __MXDAQUA_H */
