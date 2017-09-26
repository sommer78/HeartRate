#ifndef __WRISTBAND_SERVICE_H__
#define __WRISTBAND_SERVICE_H__

#ifdef __cplusplus
	extern "C"{
#endif
		
#include "system_config.h"

#define BAND_TX_RX_SERVICE_UUID 0xFFD0
#define BAND_RX_UUID            0xFFD1 //ble-->apk nof
#define BAND_TX_UUID            0xFFD2 //apk-->ble
		
typedef enum
{
    WRISTBAND_TX_WRITE            = 0,
    WRISTBAND_RX_NOTIFY           = 1,
    WRISTBAND_RX_NOTIFY_CONFIG    = 2,
    WRISTBAND_MAX                 = 3
}WRISTBAND_SRV_HANDLE;

typedef enum
{
	WRISTBAND_CMD_MAIN = 0,
	WRISTBAND_CMD_LCM  = 1,  //control screen on or off
	WRISTBAND_CMD_3DH = 2,
	WRISTBAND_CMD_SET_DATA = 3,  //set data
	WRISTBAND_CMD_GET_BAT = 4,   //get battay
	WRISTBAND_CMD_MOTOR_CMD = 5, //motor start or stop
	WRISTBAND_CMD_7816_DATA = 6,
	WRISTBAND_CMD_1321 = 7,
	WRISTBAND_CMD_MAX
}WRISTBAND_CMD_TYPE;

typedef enum
{
	WRISTBAND_CMD_GET_STEP = 0,
	WRISTBAND_CMD_GET_POWER = 1,
	WRISTBAND_CMD_GET_DATE = 2,
	WRISTBAND_CMD_SET_DATE = 3,
	WRISTBAND_CMD_MSG_MAX
}WRISTBAND_MAIN_CMD_TYPE;

typedef struct tagWRISTBAND_CMD_HEAD
{
	uint8 cmd;   //data type
	uint8 length; //data length, just data
	uint8 buf[1];  //point to real data
}WRISTBAND_CMD_HEAD;
#define WRISTBAND_DATA_POS (2)

typedef uint32 (*wristband_msg_handler_t)(void *msg);
typedef struct
{
	WRISTBAND_CMD_TYPE type;
	wristband_msg_handler_t func;
}WRISTBAND_CMD_HANDLE;


#define WRISTBAND_DEBUG_LOG_BASE          0xDD00
#define WRISTBAND_DEBUG_RECV_MSG          WRISTBAND_DEBUG_LOG_BASE+0    //msg type is error, data is msg type
#define WRISTBAND_DEBUG_RECV_LEN_ERROR    WRISTBAND_DEBUG_LOG_BASE+1    //msg length is not match the function


#define WRISTBAND_DEBUG_EN 1
#if WRISTBAND_DEBUG_EN
	#define wristband_log(addr,data)    LOG(addr,data)
#else
	#define wristband_log(addr,data)    NULL
#endif
//notity mapping table
typedef struct
{
    uint16        att_handle;       // Handle of report characteristic
    uint8         length;
    void          *value_ptr;
}WBAND_NOTIFY_MAP;
extern uint8 wristband_notify_AppSendToBle(WRISTBAND_CMD_HEAD *msg);
extern uint32 wristband_init_service(void);
extern uint16 wristband_read_attrCB(uint16 attribute_handle, uint16 offset, uint8 *buffer, uint16 buffer_size);
extern uint32 wristband_write_attrCB(uint16 attribute_handle, uint16 transaction_mode, uint16 offset, uint8 *buffer, uint16 buffer_size);

#ifdef  __cplusplus
}
#endif //__cplusplus

#endif
