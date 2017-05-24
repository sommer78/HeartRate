/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      mouse_application.h
* @brief     mouse app implementation
* @details   mouse app implementation
* @author    Shawn
* @date      2014-10-06
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _MOUSE_APPLICATION__
#define _MOUSE_APPLICATION__
#include "bee_message.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "profileAPI.h"
/**
 * @brief  Memory Dump in Mouse App
 *
 * Debug Macros--For Memory Dump Tests
 */
/* Memory dump--interrupt maner */
#define MEM_DUMP_INT    0
#define MEM_DUMP_IO_GROUP GPIO_GROUP_B
#define MEM_DUMP_IO_INDEX 2

/**
 * @brief  HOGP report ID definition
 *
 * HOGP report ID definition
 */
#if 0   //Masked by Chuanguo 2015/4/11
#define HOGP_MOUSE_REPORT_ID    3
#define HOGP_KB_REPORT_ID       1
#define HOGP_VENDOR_REPORT_ID_1 0xfd
#define HOGP_VENDOR_REPORT_ID_2 0xfc
#endif

typedef enum
{
    VK_LBUTTON      = 0x01,
    VK_RBUTTON      = 0x02,
    VK_CANCEL       = 0x03,
    VK_MBUTTON      = 0x04,
    VK_XBUTTON1     = 0x05,
    VK_XBUTTON2     = 0x06,
    VK_POWER        = 0x07, // Using this key as power key
    VK_BACK         = 0x08,
    VK_TAB          = 0x09,
    VK_UNDEFINED_0A = 0x0A,
    VK_UNDEFINED_0B = 0x0B,
    VK_CLEAR        = 0x0C,
    VK_RETURN       = 0x0D,
    VK_UNDEFINED_0E = 0x0E,
    VK_UNDEFINED_0F = 0x0F,
    VK_SHIFT        = 0x10,
    VK_CONTROL      = 0x11,
    VK_MENU         = 0x12,
    VK_PAUSE        = 0x13,
    VK_CAPITAL      = 0x14,
    VK_KANA         = 0x15,
    VK_UNDEFINED_16 = 0x16,
    VK_JUNJA        = 0x17,
    VK_FINAL        = 0x18,
    VK_HANJA        = 0x19,
    VK_UNDEFINED_1A = 0x1A,
    VK_ESCAPE       = 0x1B,
    VK_CONVERT      = 0x1C,
    VK_NONCONVERT   = 0x1D,
    VK_ACCEPT       = 0x1E,
    VK_MODECHANGE   = 0x1F,
    VK_SPACE        = 0x20,
    VK_PRIOR        = 0x21, //PAGE UP key
    VK_NEXT         = 0x22, //PAGE DOWN key
    VK_END          = 0x23,
    VK_HOME         = 0x24,
    VK_LEFT         = 0x25,
    VK_UP           = 0x26,
    VK_RIGHT        = 0x27,
    VK_DOWN         = 0x28,
    VK_SELECT       = 0x29,
    VK_PRINT        = 0x2A,
    VK_EXECUTE      = 0x2B,
    VK_SNAPSHOT     = 0x2C,
    VK_INSERT       = 0x2D,
    VK_DELETE       = 0x2E,
    VK_HELP         = 0x2F,
    VK_0            = 0x30,
    VK_1            = 0x31,
    VK_2            = 0x32,
    VK_3            = 0x33,
    VK_4            = 0x34,
    VK_5            = 0x35,
    VK_6            = 0x36,
    VK_7            = 0x37,
    VK_8            = 0x38,
    VK_9            = 0x39,
    VK_UNDEFINED_3A = 0x3A,
    VK_UNDEFINED_3B = 0x3B,
    VK_UNDEFINED_3C = 0x3C,
    VK_UNDEFINED_3D = 0x3D,
    VK_UNDEFINED_3E = 0x3E,
    VK_UNDEFINED_3F = 0x3F,
    VK_UNDEFINED_40 = 0x40,
    VK_A            = 0x41,
    VK_B            = 0x42,
    VK_C            = 0x43,
    VK_D            = 0x44,
    VK_E            = 0x45,
    VK_F            = 0x46,
    VK_G            = 0x47,
    VK_H            = 0x48,
    VK_I            = 0x49,
    VK_J            = 0x4A,
    VK_K            = 0x4B,
    VK_L            = 0x4C,
    VK_M            = 0x4D,
    VK_N            = 0x4E,
    VK_O            = 0x4F,
    VK_P            = 0x50,
    VK_Q            = 0x51,
    VK_R            = 0x52,
    VK_S            = 0x53,
    VK_T            = 0x54,
    VK_U            = 0x55,
    VK_V            = 0x56,
    VK_W            = 0x57,
    VK_X            = 0x58,
    VK_Y            = 0x59,
    VK_Z            = 0x5A,
    VK_LWIN         = 0x5B,
    VK_RWIN         = 0x5C,
    VK_APPS         = 0x5D,
    VK_UNDEFINED_5E = 0x5E,
    VK_SLEEP        = 0x5F,
    VK_NUMPAD0      = 0x60,
    VK_NUMPAD1      = 0x61,
    VK_NUMPAD2      = 0x62,
    VK_NUMPAD3      = 0x63,
    VK_NUMPAD4      = 0x64,
    VK_NUMPAD5      = 0x65,
    VK_NUMPAD6      = 0x66,
    VK_NUMPAD7      = 0x67,
    VK_NUMPAD8      = 0x68,
    VK_NUMPAD9      = 0x69,
    VK_MULTIPLY     = 0x6A,
    VK_ADD          = 0x6B,
    VK_SEPARATOR    = 0x6C,
    VK_SUBTRACT     = 0x6D,
    VK_DECIMAL      = 0x6E,
    VK_DIVIDE       = 0x6F,
    VK_F1           = 0x70,
    VK_F2           = 0x71,
    VK_F3           = 0x72,
    VK_F4           = 0x73,
    VK_F5           = 0x74,
    VK_F6           = 0x75,
    VK_F7           = 0x76,
    VK_F8           = 0x77,
    VK_F9           = 0x78,
    VK_F10          = 0x79,
    VK_F11          = 0x7A,
    VK_F12          = 0x7B,
    VK_F13          = 0x7C,
    VK_F14          = 0x7D,
    VK_F15          = 0x7E,
    VK_F16          = 0x7F,
    VK_F17          = 0x80,
    VK_F18          = 0x81,
    VK_F19          = 0x82,
    VK_F20          = 0x83,
    VK_F21          = 0x84,
    VK_F22          = 0x85,
    VK_F23          = 0x86,
    VK_F24          = 0x87,
    VK_UNDEFINED_88 = 0x88,
    VK_UNDEFINED_89 = 0x89,
    VK_UNDEFINED_8A = 0x8A,
    VK_UNDEFINED_8B = 0x8B,
    VK_UNDEFINED_8C = 0x8C,
    VK_UNDEFINED_8D = 0x8D,
    VK_UNDEFINED_8E = 0x8E,
    VK_UNDEFINED_8F = 0x8F,
    VK_NUMLOCK      = 0x90,
    VK_SCROLL       = 0x91,
    VK_UNDEFINED_92 = 0x92,
    VK_UNDEFINED_93 = 0x93,
    VK_UNDEFINED_94 = 0x94,
    VK_UNDEFINED_95 = 0x95,
    VK_UNDEFINED_96 = 0x96,
    VK_UNDEFINED_97 = 0x97,
    VK_UNDEFINED_98 = 0x98,
    VK_UNDEFINED_99 = 0x99,
    VK_UNDEFINED_9A = 0x9A,
    VK_UNDEFINED_9B = 0x9B,
    VK_UNDEFINED_9C = 0x9C,
    VK_UNDEFINED_9D = 0x9D,
    VK_UNDEFINED_9E = 0x9E,
    VK_UNDEFINED_9F = 0x9F,
    VK_LSHIFT       = 0xA0,
    VK_RSHIFT       = 0xA1,
    VK_LCONTROL     = 0xA2,
    VK_RCONTROL     = 0xA3,
    VK_LMENU        = 0xA4, // Left ALT
    VK_RMENU        = 0xA5, // Right ALT
    VK_BROWSER_BACK      = 0xA6,
    VK_BROWSER_FORWARD   = 0xA7,
    VK_BROWSER_REFRESH   = 0xA8,
    VK_BROWSER_STOP      = 0xA9,
    VK_BROWSER_SEARCH    = 0xAA,
    VK_BROWSER_FAVORITES = 0xAB,
    VK_BROWSER_HOME      = 0xAC,
    VK_VOLUME_MUTE       = 0xAD,
    VK_VOLUME_DOWN       = 0xAE,
    VK_VOLUME_UP         = 0xAF,
    VK_MEDIA_NEXT_TRACK  = 0xB0,
    VK_MEDIA_PREV_TRACK  = 0xB1,
    VK_MEDIA_STOP        = 0xB2,
    VK_MEDIA_PLAY_PAUSE  = 0xB3,
    VK_LAUNCH_MAIL       = 0xB4,
    VK_LAUNCH_MEDIA_SELECT = 0xB5,
    VK_LAUNCH_APP1       = 0xB6,
    VK_LAUNCH_APP2       = 0xB7,
    VK_UNDEFINED_B8      = 0xB8,
    VK_UNDEFINED_B9      = 0xB9,
    VK_OEM_1             = 0xBA,
    VK_OEM_PLUS          = 0xBB,
    VK_OEM_COMMA         = 0xBC,
    VK_OEM_MINUS         = 0xBD,
    VK_OEM_PERIOD        = 0xBE,
    VK_OEM_2             = 0xBF,
    VK_OEM_3             = 0xC0,
    VK_UNDEFINED_C1      = 0xC1,
    VK_UNDEFINED_C2      = 0xC2,
    VK_UNDEFINED_C3      = 0xC3,
    VK_UNDEFINED_C4      = 0xC4,
    VK_UNDEFINED_C5      = 0xC5,
    VK_UNDEFINED_C6      = 0xC6,
    VK_UNDEFINED_C7      = 0xC7,
    VK_UNDEFINED_C8      = 0xC8,
    VK_UNDEFINED_C9      = 0xC9,
    VK_UNDEFINED_CA      = 0xCA,
    VK_UNDEFINED_CB      = 0xCB,
    VK_UNDEFINED_CC      = 0xCC,
    VK_UNDEFINED_CD      = 0xCD,
    VK_UNDEFINED_CE      = 0xCE,
    VK_UNDEFINED_CF      = 0xCF,
    VK_UNDEFINED_D0      = 0xD0,
    VK_UNDEFINED_D1      = 0xD1,
    VK_UNDEFINED_D2      = 0xD2,
    VK_UNDEFINED_D3      = 0xD3,
    VK_UNDEFINED_D4      = 0xD4,
    VK_UNDEFINED_D5      = 0xD5,
    VK_UNDEFINED_D6      = 0xD6,
    VK_UNDEFINED_D7      = 0xD7,
    VK_UNDEFINED_D8      = 0xD8,
    VK_UNDEFINED_D9      = 0xD9,
    VK_UNDEFINED_DA      = 0xDA,
    VK_OEM_4             = 0xDB,
    VK_OEM_5             = 0xDC,
    VK_OEM_6             = 0xDD,
    VK_OEM_7             = 0xDE,
    VK_OEM_8             = 0xDF,
    VK_UNDEFINED_E0      = 0xE0,
    VK_UNDEFINED_E1      = 0xE1,
    VK_OEM_102           = 0xE2,
    VK_UNDEFINED_E3      = 0xE3,
    VK_UNDEFINED_E4      = 0xE4,
    VK_PROCESSKEY        = 0xE5,
    VK_UNDEFINED_E6      = 0xE6,
    VK_PACKET            = 0xE7,
    VK_UNDEFINED_E8      = 0xE8,
    VK_UNDEFINED_E9      = 0xE9,
    VK_UNDEFINED_EA      = 0xEA,
    VK_UNDEFINED_EB      = 0xEB,
    VK_UNDEFINED_EC      = 0xEC,
    VK_UNDEFINED_ED      = 0xED,
    VK_UNDEFINED_EE      = 0xEE,
    VK_UNDEFINED_EF      = 0xEF,
    VK_UNDEFINED_F1      = 0xF1,
    VK_UNDEFINED_F2      = 0xF2,
    VK_UNDEFINED_F3      = 0xF3,
    VK_UNDEFINED_F4      = 0xF4,
    VK_UNDEFINED_F5      = 0xF5,
    VK_ATTN              = 0xF6,
    VK_CRSEL             = 0xF7,
    VK_EXSEL             = 0xF8,
    VK_EREOF             = 0xF9,
    VK_PLAY              = 0xFA,
    VK_ZOOM              = 0xFB,
    VK_NONAME            = 0xFC,
    VK_PA1               = 0xFD,
    VK_OEM_CLEAR         = 0xFE,
} VirtualKeyDefs;

extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
TAppResult AppProfileCallback(uint8_t serviceID, void *pData);


#ifdef __cplusplus
}
#endif

#endif

