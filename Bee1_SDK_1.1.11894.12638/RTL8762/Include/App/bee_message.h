/**
*****************************************************************************
This file is used temporary and will be replaced by each task's message header file.
Add by Lory
*****************************************************************************
*/
#ifndef _BEE_MESSAGE_H
#define _BEE_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl_types.h"
#include "common_defs.h"
#include "gap.h"

#define EVENT_NEWIODRIVER_TO_APP        0x02
#define EVENT_IODRIVER_TO_APP           0x05
#define EVENT_ALIGN_TO_APP    			0xAA



/* IO Type */
enum
{
    BT_STATUS_UPDATE,
    IO_KEYSCAN_MSG_TYPE,
    IO_QDECODE_MSG_TYPE,
    IO_UART_MSG_TYPE,
    IO_KEYPAD_MSG_TYPE,
    IO_IR_MSG_TYPE,
    IO_GDMA_MSG_TYPE,
    IO_ADC_MSG_TYPE,
    IO_D3DG_MSG_TYPE,
    IO_SPI_MSG_TYPE,
    IO_MOUSE_BUTTON_MSG_TYPE,
    IO_GPIO_MSG_TYPE,
    MOUSE_SENSOR_MSG,
    APP_TIMER_MSG,
    IO_WRISTBNAD_MSG_TYPE,
    GLC_MSG_TYPE,
    IO_KEYBOARD_BUTTON_MSG_TYPE,
    MESH_STATUS_UPDATE
};

typedef struct
{
    uint16_t IoType;
    uint16_t subType;
    union
    {
        uint32_t parm;
        void *pBuf;
    };
} BEE_IO_MSG;



/* IO Sub Type */
typedef enum
{
    MSG_BWPS_TX_VALUE              = 1,
    MSG_OS_TIMEROUT_SEND_DATA      = 2,
    MSG_OS_TIMEROUT_SEND_DELAY     = 3,
    MSG_OS_TIMEROUT_SENSOR         = 4,
    MSG_RTC_TIMEROUT_WALL_CLOCK    = 5,
    MSG_SENSOR_DOUBLE_CLICK        = 6,
    MSG_ADC_INT                    = 7,
    MSG_SENSOR_INT                 = 8,
	MSG_SENSOR_WAKE_UP			   = 9, 
	MSG_LED_TWINKLE				   = 10,
	MSG_MOTOR_VIBRATE			   = 11,	
	MSG_CHARGER_STATE			   = 12,
	MSG_RTC_LOW_BATTERY_RESTORE	   = 13,
	MSG_RTC_ALARM 				   = 14,
	MSG_ADC_SAMPLE_READY		   = 15,
	MSG_GDMA_RECV                  = 16,
	MSG_SENSOR_LEN                 = 17,
	MSG_BUTTON_EVENT			   = 18,	
} MSG_TYPE_WRISTBAND;


/* IO Sub Type */
typedef enum
{
    MSG_UART_RX             = 1,
    MSG_UART_RX_TIMEOUT     = 2,
    MSG_UART_RX_OVERFLOW = 3,
    MSG_UART_RX_TIMEOUT_OVERFLOW = 4,
    MSG_UART_RX_EMPTY = 5,
} MSG_TYPE_UART;

typedef enum
{
    MOUSE_BTN_LEFT_PRESS,
    MOUSE_BTN_LEFT_RELEASE,
    MOUSE_BTN_RIGHT_PRESS,
    MOUSE_BTN_RIGHT_RELEASE,
    MOUSE_BTN_MIDDLE_PRESS,
    MOUSE_BTN_MIDDLE_RELEASE
} MOUSE_BUTTON_SubType;

typedef enum
{
    KEYBOARD_SWITCH_BTN_PRESS,
    KEYBOARD_SWITCH_BTN_RELEASE,
} Keyboard_BUTTON_SubType;


typedef enum
{
    TIMER_ALARM,
    TIMER_DLPS
} APP_TIMER_SubType;

#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H */
