/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      keyboard_application.h
* @brief     keyboard app implementation
* @details   keyboard app implementation
* @author    Elliot
* @date      2015-07-16
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _KEYBOARD_APPLICATION__
#define _KEYBOARD_APPLICATION__
#include "bee_message.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "profileAPI.h"
#include "board.h"

/**
 * @brief  Memory Dump in keyboard App
 *
 * Debug Macros--For Memory Dump Tests
 */
/* Memory dump--interrupt maner */
#define MEM_DUMP_INT    0
#define MEM_DUMP_IO_GROUP GPIO_GROUP_B
#define MEM_DUMP_IO_INDEX 2

/**
 * @brief  HID Keyboard page
 *
 * Usage ID(Hex)
 */

typedef enum
{
    /****Keyboard*******/
    NO_EVENT                   = 0x00,
    KB_ErrorRollOver           = 0x01,
    KB_POSTFail                = 0x02,
    KB_ErrorUndefined          = 0x03,
    KB_a_A                     = 0x04,
    KB_b_B                     = 0x05,
    KB_c_C                     = 0x06,
    KB_d_D                     = 0x07,
    KB_e_E                     = 0x08,
    KB_f_F                     = 0x09,
    KB_g_G                     = 0x0A,
    KB_h_H                     = 0x0B,
    KB_i_I                     = 0x0C,
    KB_j_J                     = 0x0D,
    KB_k_K                     = 0x0E,
    KB_l_L                     = 0x0F,
    KB_m_M                     = 0x10,
    KB_n_N                     = 0x11,
    KB_o_O                     = 0x12,
    KB_p_P                     = 0x13,
    KB_q_Q                     = 0x14,
    KB_r_R                     = 0x15,
    KB_s_S                     = 0x16,
    KB_t_T                     = 0x17,
    KB_u_U                     = 0x18,
    KB_v_V                     = 0x19,
    KB_w_W                     = 0x1A,
    KB_x_X                     = 0x1B,
    KB_y_Y                     = 0x1C,
    KB_z_Z                     = 0x1D,
    KB_1                       = 0x1E,
    KB_2                       = 0x1F,
    KB_3                       = 0x20,
    KB_4                       = 0x21,
    KB_5                       = 0x22,
    KB_6                       = 0x23,
    KB_7                       = 0x24,
    KB_8                       = 0x25,
    KB_9                       = 0x26,
    KB_0                       = 0x27,
    KB_Return_ENTER            = 0x28,
    KB_ESCAPE                  = 0x29,
    KB_DELETE_Backspace        = 0x2A,
    KB_Tab                     = 0x2B,
    KB_Spacebar                = 0x2C,
    KB_Minus_Underscore        = 0x2D,
    KB_Equal_Plus              = 0x2E,
    KB_LeftBracket_LeftBrace   = 0x2F,
    KB_RightBracket_RightBrace = 0x30,
    KB_BackSlash_VerticalBar   = 0x31,
    KB_NON_US_Pound            = 0x32,
    KB_Semicolon_Colon         = 0x33,
    KB_QuotationMark           = 0x34,
    KB_GraveAccent_Tilde       = 0x35,
    KB_Comma_LessThan          = 0x36,
    KB_Dot_LargerThan          = 0x37,
    KB_Slash_QuestionMark      = 0x38,
    KB_CapsLock                = 0x39,
    KB_F1                      = 0x3A,
    KB_F2                      = 0x3B,
    KB_F3                      = 0x3C,
    KB_F4                      = 0x3D,
    KB_F5                      = 0x3E,
    KB_F6                      = 0x3F,
    KB_F7                      = 0x40,
    KB_F8                      = 0x41,
    KB_F9                      = 0x42,
    KB_F10                     = 0x43,
    KB_F11                     = 0x44,
    KB_F12                     = 0x45,
    KB_PrintScreen             = 0x46,
    KB_ScrollLock              = 0x47,
    KB_Pause                   = 0x48,
    KB_Insert                  = 0x49,
    KB_Home                    = 0x4A,
    KB_PageUp                  = 0x4B,
    KB_DeleteForward           = 0x4C,
    KB_End                     = 0x4D,
    KB_PageDown                = 0x4E,
    KB_RightArrow              = 0x4F,
    KB_LeftArrow               = 0x50,
    KB_DownArrow               = 0x51,
    KB_UpArrow                 = 0x52,

    /****Keypad*******/
    KP_NumLock_Clear           = 0x53,
    KP_Divide                  = 0x54,
    KP_Multiply                = 0x55,
    KP_Minus                   = 0x56,
    KP_Plus                    = 0x57,
    KP_Enter                   = 0x58,
    KP_1_End                   = 0x59,
    KP_2_DownArrow             = 0x5A,
    KP_3_PageDn                = 0x5B,
    KP_4_LeftArrow             = 0x5C,
    KP_5                       = 0x5D,
    KP_6_RightArrow            = 0x5E,
    KP_7_Home                  = 0x5F,
    KP_8_UpArrow               = 0x60,
    KP_9_PageUp                = 0x61,
    KP_0_Insert                = 0x62,
    KP_Dot_Delete              = 0x63,

    /****Keyboard*******/
    KB_NON_US_BackSlash        = 0x64,
    KB_Application             = 0x65,
    KB_Power                   = 0x66,

    /****Keypad*******/
    KP_Equal                   = 0x67,

    /****Keyboard*******/
    KB_F13                     = 0x68,
    KB_F14                     = 0x69,
    KB_F15                     = 0x6A,
    KB_F16                     = 0x6B,
    KB_F17                     = 0x6C,
    KB_F18                     = 0x6D,
    KB_F19                     = 0x6E,
    KB_F20                     = 0x6F,
    KB_F21                     = 0x70,
    KB_F22                     = 0x71,
    KB_F23                     = 0x72,
    KB_F24                     = 0x73,
    KB_Execute                 = 0x74,
    KB_Help                    = 0x75,
    KB_Menu                    = 0x76,
    KB_Select                  = 0x77,
    KB_Stop                    = 0x78,
    KB_Again                   = 0x79,
    KB_Undo                    = 0x7A,
    KB_Cut                     = 0x7B,
    KB_Copy                    = 0x7C,
    KB_Paste                   = 0x7D,
    KB_Find                    = 0x7E,
    KB_Mute                    = 0x7F,
    KB_VolumeUp                = 0x80,
    KB_VolumeDown              = 0x81,
    KB_LockingCapsLock         = 0x82,
    KB_LockingNumLock          = 0x83,
    KB_LockingScrollLock       = 0x84,
    KB_Comma                   = 0x85,
    KB_EqualSign               = 0x86,
    KB_International1          = 0x87,
    KB_International2          = 0x88,
    KB_International3          = 0x89,
    KB_International4          = 0x8A,
    KB_International5          = 0x8B,
    KB_International6          = 0x8C,
    KB_International7          = 0x8D,
    KB_International8          = 0x8E,
    KB_International9          = 0x8F,
    KB_LANG1                   = 0x90,
    KB_LANG2                   = 0x91,
    KB_LANG3                   = 0x92,
    KB_LANG4                   = 0x93,
    KB_LANG5                   = 0x94,
    KB_LANG6                   = 0x95,
    KB_LANG7                   = 0x96,
    KB_LANG8                   = 0x97,
    KB_LANG9                   = 0x98,
    KB_AlternateErase          = 0x99,
    KB_SysReq_Attention        = 0x9A,
    KB_Cancel                  = 0x9B,
    KB_Clear                   = 0x9C,
    KB_Prior                   = 0x9D,
    KB_Return                  = 0x9E,
    KB_Separator               = 0x9F,
    KB_Out                     = 0xA0,
    KB_Oper                    = 0xA1,
    KB_Clear_Again             = 0xA2,
    KB_CrSel_Props             = 0xA3,
    KB_ExSel                   = 0xA4,

    KB_LeftControl             = 0xE0,
    KB_LeftShift               = 0xE1,
    KB_LeftAlt                 = 0xE2,
    KB_LeftGUI                 = 0xE3,
    KB_RightControl            = 0xE4,
    KB_RightShift              = 0xE5,
    KB_RightAlt                = 0xE6,
    KB_RightGUI                = 0xE7,

#ifdef ATANS_KEYBOARD
    Fn                         = 0xF0,
    Fn_1                       = 0xF1,
    Fn_2                       = 0xF2,
    Fn_3                       = 0xF3,
    Fn_4                       = 0xF4,
    Fn_5                       = 0xF5,
    Fn_6                       = 0xF6,
    Fn_7                       = 0xF7,
    Fn_8                       = 0xF8,
    Fn_9                       = 0xF9,
    Fn_10                      = 0xFA,
    Fn_11                      = 0xFB,
    Fn_12                      = 0xFC,
    Fn_13                      = 0xFD,
#endif

    /****MultiMediaKey*******/
#ifdef ATANS_KEYBOARD
    MM_ScanNext                = 0x0101,
    MM_ScanPrevious            = 0x0102,
    MM_Stop                    = 0x0104,
    MM_Play_Pause              = 0x0108,
    MM_Mute                    = 0x0110,
    MM_BassBoost               = 0x0120,
    MM_Power                   = 0x0140,
    MM_VolumeIncrement         = 0x0180,

    MM_VolumeDecrement         = 0x0201,
    BrightnessIncrement        = 0x0202,
    BrightnessDecrement        = 0x0204,
    MM_TrebleIncrement         = 0x0208,
    MM_TrebleDecrement         = 0x0210,
    MM_AL_ConsumerControl      = 0x0220,
    MM_AL_KeyboardLayout       = 0x0240,
    MM_AL_Calculator           = 0x0280,

    MM_AL_ImageBrowser         = 0x0301,
    MM_AC_Search               = 0x0302,
    MM_AC_Home                 = 0x0304,
    MM_AC_Back                 = 0x0308,
    MM_AC_Forward              = 0x0310,
    MM_AC_Stop                 = 0x0320,
    MM_AC_Refresh              = 0x0340,
    MM_AC_Bookmarks            = 0x0380,
#else
    MM_ScanNext                = 0x0101,
    MM_ScanPrevious            = 0x0102,
    MM_Stop                    = 0x0104,
    MM_Play_Pause              = 0x0108,
    MM_Mute                    = 0x0110,
    MM_BassBoost               = 0x0120,
    MM_Loudness                = 0x0140,
    MM_VolumeIncrement         = 0x0180,

    MM_VolumeDecrement         = 0x0201,
    MM_BassIncrement           = 0x0202,
    MM_BassDecrement           = 0x0204,
    MM_TrebleIncrement         = 0x0208,
    MM_TrebleDecrement         = 0x0210,
    MM_AL_ConsumerControl      = 0x0220,
    MM_AL_EmailReader          = 0x0240,
    MM_AL_Calculator           = 0x0280,

    MM_AL_LocalMachineBrowser  = 0x0301,
    MM_AC_Search               = 0x0302,
    MM_AC_Home                 = 0x0304,
    MM_AC_Back                 = 0x0308,
    MM_AC_Forward              = 0x0310,
    MM_AC_Stop                 = 0x0320,
    MM_AC_Refresh              = 0x0340,
    MM_AC_Bookmarks            = 0x0380,
#endif

} VirtualKeyDefs;



/* ----------passkey struct for keyboard------------- */
typedef struct
{
    uint32_t    passKeyValue;
    uint8_t     passKeyCnt;
    BOOL        isAbleInput;
} PassKey_TypeDef;

/* Exported Functions ---------------------------------*/
extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
TAppResult AppProfileCallback(uint8_t serviceID, void *pData);


#ifdef __cplusplus
}
#endif

#endif

