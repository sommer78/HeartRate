/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      keyboard_application.c
* @brief     keyboard application implementation
* @details   keyboard application implementation
* @author    Elliot Chen
* @date      2015-07-16
* @version   v1.0
* *********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "board.h"
#include "application.h"
#include "keyboard_application.h"
#include "bee_message.h"
#include "trace.h"
#include "profile_init.h"
#include "bee_message.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include <string.h>
#include "sps.h"
#include "dis.h"
#include "bas.h"
#include "hids_keyboard.h"
#include "profileApi.h"
#include "dlps_platform.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "keyscan.h"
#include "timers.h"

/* Reference varibales statement ----------------------------------------------*/
extern uint8_t gHIDServiceId;
extern uint8_t gBASServiceId;
extern uint8_t gDISServiceId;
extern uint8_t gSPSServiceId;
extern BOOL allowedKeyboardEnterDlps;
extern TimerHandle_t xTimersPairing;
extern TimerHandle_t xTimersEnableSend;

#define NoneKey   0x0000

/* Keypad key mapping ---------------------------------------------------------*/
/* VK_DELETE key is equal to Backspace key*/
#ifdef ATANS_KEYBOARD
const uint16_t Keyboard_Keymap[KEYBOARD_KEYPAD_ROW_SIZE][KEYBOARD_KEYPAD_COLUMN_SIZE] =
{
    {NoneKey,     KB_q_Q,               KB_w_W,      KB_e_E,  KB_r_R, KB_u_U, KB_i_I,                     KB_o_O,            KB_p_P,                   NoneKey,        NoneKey,                  NoneKey,      NoneKey,       NoneKey, NoneKey,      NoneKey,       NoneKey,    NoneKey},
    {NoneKey,     KB_Tab,               KB_CapsLock, Fn_3,    KB_t_T, KB_y_Y, KB_RightBracket_RightBrace, Fn_7,              KB_LeftBracket_LeftBrace, NoneKey,        KB_DELETE_Backspace,      NoneKey,      NoneKey,       NoneKey, NoneKey,      KB_LeftShift,  KB_LeftAlt, NoneKey},
    {NoneKey,     KB_a_A,               KB_s_S,      KB_d_D,  KB_f_F, KB_j_J, KB_k_K,                     KB_l_L,            KB_Semicolon_Colon,       KB_LeftControl, KB_BackSlash_VerticalBar, NoneKey,      NoneKey,       NoneKey, NoneKey,      KB_RightShift, NoneKey,    NoneKey},
    {NoneKey,     MM_AC_Home,           NoneKey,     Fn_4,    KB_g_G, KB_h_H, Fn_6,                       NoneKey,           KB_QuotationMark,         KB_LeftGUI,     Fn_11,                    KB_Spacebar,  NoneKey,       NoneKey, KB_UpArrow,   NoneKey,       NoneKey,    NoneKey},
    {KB_RightAlt, KB_z_Z,               KB_x_X,      KB_c_C,  KB_v_V, KB_m_M, KB_Comma_LessThan,          KB_Dot_LargerThan, NoneKey,                  NoneKey,        KB_Return_ENTER,          NoneKey,      NoneKey,       NoneKey, NoneKey,      NoneKey,       NoneKey,    NoneKey},
    {NoneKey,     NoneKey,              NoneKey,     NoneKey, KB_b_B, KB_n_N, NoneKey,                    NoneKey,           KB_Slash_QuestionMark,    KB_RightGUI,    Fn_12,                    KB_DownArrow, KB_RightArrow, NoneKey, KB_LeftArrow, NoneKey,       NoneKey,    NoneKey},
    {Fn,          KB_GraveAccent_Tilde, Fn_1,        Fn_2,    KB_5,   KB_6,   KB_Equal_Plus,              Fn_8,              KB_Minus_Underscore,      NoneKey,        Fn_9,                     Fn_13,        NoneKey,       NoneKey, NoneKey,      NoneKey,       NoneKey,    NoneKey},
    {Fn_5,        KB_1,                 KB_2,        KB_3,    KB_4,   KB_7,   KB_8,                       KB_9,              KB_0,                     NoneKey,        Fn_10,                    NoneKey,      NoneKey,       NoneKey, NoneKey,      NoneKey,       NoneKey,    NoneKey},
};
const uint16_t FunctionKeys[2][13] =
{
    {KB_F1,               KB_F2,               KB_F3,        KB_F4,              KB_F5,                KB_F6,   KB_F7,           KB_F8,         KB_F9,       KB_F10,  KB_F11,             KB_F12,             KB_DeleteForward},
    {BrightnessDecrement, BrightnessIncrement, MM_AC_Search, MM_AL_ImageBrowser, MM_AL_KeyboardLayout, NoneKey, MM_ScanPrevious, MM_Play_Pause, MM_ScanNext, MM_Mute, MM_VolumeDecrement, MM_VolumeIncrement, MM_Power        },
};
#else
const uint16_t Keyboard_Keymap[KEYBOARD_KEYPAD_ROW_SIZE][KEYBOARD_KEYPAD_COLUMN_SIZE] =
{
    {NoneKey,       KB_c_C,                   KB_Slash_QuestionMark, NoneKey,     KB_Home,             KB_9,              KP_Divide,        KP_Minus, KB_LeftGUI, KB_0,                NoneKey,                  KB_F7,        KP_4_LeftArrow, NoneKey,         KB_F8,        NoneKey,     NoneKey,         KP_Multiply,                NoneKey, NoneKey},
    {NoneKey,       KB_x_X,                   KB_o_O,                NoneKey,     KB_y_Y,              KB_8,              KB_PageDown,      KB_Tab,   NoneKey,    KB_f_F,              KB_F5,                    NoneKey,      KB_i_I,         NoneKey,         KP_8_UpArrow, KB_RightAlt, NoneKey,         KB_1,                       NoneKey, NoneKey},
    {KB_RightShift, KB_DeleteForward,         KB_p_P,                NoneKey,     KB_t_T,              KB_m_M,            KB_Return_ENTER,  KB_h_H,   NoneKey,    KB_n_N,              KB_j_J,                   KB_LeftArrow, KB_e_E,         KP_6_RightArrow, KB_ESCAPE,    NoneKey,     NoneKey,         KB_RightBracket_RightBrace, NoneKey, NoneKey},
    {NoneKey,       KB_BackSlash_VerticalBar, KP_1_End,              NoneKey,     KB_r_R,              KB_Equal_Plus,     KP_NumLock_Clear, KB_q_Q,   NoneKey,    KB_Minus_Underscore, KB_l_L,                   KB_a_A,       KB_F11,         KB_DownArrow,    KB_F10,       NoneKey,     KB_LeftControl,  KB_PageUp,                  NoneKey, NoneKey},
    {NoneKey,       NoneKey,                  KB_d_D,                NoneKey,     KB_6,                KP_3_PageDn,       KB_CapsLock,      KB_g_G,   NoneKey,    KB_F2,               KB_LeftBracket_LeftBrace, NoneKey,      KB_u_U,         KB_F3,           KB_F1,        KB_LeftAlt,  NoneKey,         KB_b_B,                     NoneKey, NoneKey},
    {KB_LeftShift,  KP_Dot_Delete,            KB_QuotationMark,      NoneKey,     KB_5,                KB_Comma_LessThan, KP_0_Insert,      KB_4,     NoneKey,    KB_Semicolon_Colon,  KB_k_K,                   KP_5,         KB_UpArrow,     KB_F4,           KB_w_W,       NoneKey,     NoneKey,         KB_GraveAccent_Tilde,       NoneKey, NoneKey},
    {NoneKey,       KP_9_PageUp,              KB_RightArrow,         NoneKey,     KP_Plus,             KB_7,              KB_End,           KB_3,     NoneKey,    KP_2_DownArrow,      KB_Dot_LargerThan,        KB_F6,        KB_F12,         KB_s_S,          KB_F9,        NoneKey,     KB_RightControl, KB_2,                       NoneKey, NoneKey},
    {NoneKey,       KB_Spacebar,              KB_Pause,              KB_RightGUI, KB_DELETE_Backspace, KB_v_V,            KB_Application,   NoneKey,  NoneKey,    KB_z_Z,              NoneKey,                  KP_7_Home,    KB_Insert,      KB_PrintScreen,  KP_Enter,     NoneKey,     NoneKey,         KB_ScrollLock,              NoneKey, NoneKey},
};
#endif

/* Global variables defines -----------------------------------------------------------*/
uint8_t gProtocolMode;
uint8_t gSuspendMode;
uint8_t gBASBatteryLevel = 0;
gaprole_States_t gapProfileState = GAPSTATE_INIT;

/* store passkey for keyboard */
PassKey_TypeDef passKeyStructure = {0, 0, FALSE};
/* store status of keyboard */
BOOL IsAbleToSendAction = FALSE;
BOOL IsKeyboardEnabled = FALSE;

#ifdef MULTIMEDIA_KEYBOARD
BOOL IsMmKbToSendAction = FALSE;
BOOL IsMmKeyboardEnabled = FALSE;
#endif

uint8_t protocolmode = REPORT_PROCOCOL_MODE;
uint8_t suspendMode = ENTER_SUSPEND_MODE;
uint8_t KeyboardData = HID_OUTPUT_TYPE;

uint8_t MediaKey = 0;
uint8_t NormalKey = 0;

/* Function declaration -------------------------------------------------*/
void Keyboard_HandleButtonEvent(BEE_IO_MSG io_driver_msg_recv);
void Keyboard_HandleKeyEvent(BEE_IO_MSG io_driver_msg_recv);
void peripheral_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);
void ChangeInterval(uint16_t interval_min, uint16_t interval_max);
static void Keyboard_GetPassKey(BEE_IO_MSG io_driver_msg_recv);

/**
* @brief  All the application events are pre-handled in this function.
*
* All the IO MSGs are sent to this function.
* Then the event handling function shall be called according to the MSG type.
*
* @param   io_driver_msg_recv  The BEE_IO_MSG from peripherals or BT stack state machine.
* @return  void
*/
void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv)
{
    UINT16 msgtype = io_driver_msg_recv.IoType;

    switch (msgtype)
    {
#if ENABLE_SWITCH_BUTTON
        case IO_KEYBOARD_BUTTON_MSG_TYPE:
            {
                if (IsAbleToSendAction && IsKeyboardEnabled)
                {
                    Keyboard_HandleButtonEvent(io_driver_msg_recv);
                }

                allowedKeyboardEnterDlps = true;

                break;
            }
#endif
        case IO_KEYSCAN_MSG_TYPE:
            {
                if (io_driver_msg_recv.subType == MSG_KEYSCAN_RX_PKT)
                {
                    if (IsAbleToSendAction && IsKeyboardEnabled)
                    {
                        Keyboard_HandleKeyEvent(io_driver_msg_recv);
                    }
                    else
                    {
                        if (passKeyStructure.isAbleInput == TRUE)
                        {
                            Keyboard_GetPassKey(io_driver_msg_recv);
                        }
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKeyStructure.isAbleInput = 0x%x\n", 1,
                                   passKeyStructure.isAbleInput);
                    }

                }
                else if (io_driver_msg_recv.subType == MSG_KEYSCAN_ALLKEYRELEASE)
                {
                    if (IsAbleToSendAction && IsKeyboardEnabled)
                    {
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Key Release\n", 0);
                        //send release message to BT
                        if (NormalKey)
                        {
                            uint8_t KeyboardSendData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                            NormalKey = 0;
                            ProfileAPI_SendData(gHIDServiceId,
                                                GATT_SRV_HID_KB_INPUT_INDEX,
                                                (uint8_t *)(KeyboardSendData),
                                                sizeof(KeyboardSendData));
                        }

                        if (MediaKey)
                        {
                            uint8_t MultiMediaKey[3] = {0x00, 0x00, 0x00};
                            MediaKey = 0;
                            ProfileAPI_SendData(gHIDServiceId,
                                                GATT_SRV_HID_MM_KB_INPUT_INDEX,
                                                (uint8_t *)(MultiMediaKey),
                                                sizeof(MultiMediaKey));
                        }
                    }
                    else
                    {
                        //key release for inputting passkey
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Key Release which input passkey\n", 0);
                    }
                }

                allowedKeyboardEnterDlps = true;
                break;
            }
        case BT_STATUS_UPDATE:
            {
                peripheral_HandleBtGapMessage(&io_driver_msg_recv);
                break;
            }
        case APP_TIMER_MSG:
            {

                break;
            }
        default:
            {
                break;
            }
    }
}

/**
* @brief
*
*
* @param   io_driver_msg_recv
* @return  void
*/
void Keyboard_HandleButtonEvent(BEE_IO_MSG io_driver_msg_recv)
{
    if (io_driver_msg_recv.subType == KEYBOARD_SWITCH_BTN_PRESS)
    {
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Switch Button Press\n", 0);
        peripheral_Disconnect();
        GAPBondMgr_EraseAllBondings();
        KeyboardEnablePairing();
    }
    if (io_driver_msg_recv.subType == KEYBOARD_SWITCH_BTN_RELEASE)
    {
        //KeyboardEnablePairing();
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Switch Button Release\n", 0);
    }
}

/**
* @brief  Handle to key event
*
*
* @param   io_driver_msg_recv  The BEE_IO_MSG from peripherals or BT stack state machine.
* @return  void
*/
void Keyboard_HandleKeyEvent(BEE_IO_MSG io_driver_msg_recv)
{
    uint8_t i;
    uint8_t keycount = 0;
    uint16_t KeyCode;
    KEYSCAN_DATA_STRUCT *pKey_Data = io_driver_msg_recv.pBuf;
    uint8_t KeyboardSendData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t MultiMediaKey[3] = {0x00, 0x00, 0x00};

    /******check Fn key************/
#ifdef ATANS_KEYBOARD
    uint8_t FnKey = 0;
    for (i = 0; i < pKey_Data->Length; i++)
    {
        if (Keyboard_Keymap[pKey_Data->key[i].row][pKey_Data->key[i].column] == 0xF0)
        {
            FnKey = 1;
            break;
        }
    }
#endif

    for (i = 0; i < pKey_Data->Length; i++)
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "%d/%d: (row, column) = (%d, %d) n", 4, i + 1, pKey_Data->Length, pKey_Data->key[i].row, pKey_Data->key[i].column);
        KeyCode = Keyboard_Keymap[pKey_Data->key[i].row][pKey_Data->key[i].column];

        /********Normal Keys**********/
        if (KeyCode <= 0xA4)
        {
            if (keycount < 6)
            {
                KeyboardSendData[2 + keycount] = KeyCode;
                keycount++;
            }
            NormalKey = 1;
        }
        /*********Modifier Keys********/
        else if ((KeyCode >= 0xE0) && (KeyCode <= 0xE7))
        {
            KeyboardSendData[0] |= 1 << (KeyCode & 0x07);
            NormalKey = 1;
        }
#ifdef ATANS_KEYBOARD
        else if ((KeyCode >= 0xF0) && (KeyCode <= 0xFD))
        {
            if (KeyCode > 0xF0)
            {
                if (!FnKey)
                {
                    if (FunctionKeys[1][KeyCode - 0xF1])
                    {
                        uint8_t MMIndex = 0;
                        MMIndex = FunctionKeys[1][KeyCode - 0xF1] >> 8;
                        MultiMediaKey[MMIndex - 1] = FunctionKeys[1][KeyCode - 0xF1];
                        MediaKey = 1;
                        ProfileAPI_SendData(gHIDServiceId,
                                            GATT_SRV_HID_MM_KB_INPUT_INDEX,
                                            (uint8_t *)(MultiMediaKey),
                                            sizeof(MultiMediaKey));
                    }

                }
                else
                {
                    if (keycount < 6)
                    {
                        KeyboardSendData[2 + keycount] = FunctionKeys[0][KeyCode - 0xF1];
                        keycount++;
                    }
                    NormalKey = 1;
                }
            }

        }
#endif
        /*********MultiMedia Keys******/
        else
        {
            uint8_t MMIndex = 0;
            MMIndex = KeyCode >> 8;
            MultiMediaKey[MMIndex - 1] = KeyCode;
            MediaKey = 1;
            ProfileAPI_SendData(gHIDServiceId,
                                GATT_SRV_HID_MM_KB_INPUT_INDEX,
                                (uint8_t *)(MultiMediaKey),
                                sizeof(MultiMediaKey));
        }
    }

    if (NormalKey)
    {
        ProfileAPI_SendData(gHIDServiceId,
                            GATT_SRV_HID_KB_INPUT_INDEX,
                            (uint8_t *)(KeyboardSendData),
                            sizeof(KeyboardSendData));
    }

}

void vTimerPairingCallback(xTimerHandle handle)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Pairing timeout,disconnect", 0);
    peripheral_Disconnect();
}

void vTimerEnableSendCallback(xTimerHandle handle)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Enable send action", 0);

    IsAbleToSendAction = TRUE;
#ifdef MULTIMEDIA_KEYBOARD
    IsMmKbToSendAction = TRUE;
#endif
}


/******************************************************************
 * @fn          peripheral_HandleBtGapStateChangeEvt
 * @brief      All the gaprole_States_t events are pre-handled in this function.
 *                Then the event handling function shall be called according to the newState.
 *
 * @param    newState  - new gap state
 * @return     void
 */
void peripheral_HandleBtGapStateChangeEvt(uint8_t newState)
{
    switch (newState)
    {
        case GAPSTATE_IDLE_NO_ADV_NO_CONN:
            {
                IsAbleToSendAction = FALSE;
                IsKeyboardEnabled = FALSE;
#ifdef MULTIMEDIA_KEYBOARD
                IsMmKbToSendAction = FALSE;
                IsMmKeyboardEnabled = FALSE;
#endif
                if (gapProfileState == GAPSTATE_CONNECTED)
                {
                    uint8_t disc_reason;
                    peripheralGetGapParameter(GAPPRRA_DISCONNECTED_REASON, &disc_reason);
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapMessage: disc_reason = %d", 1, disc_reason);
                    KeyboardStartAdv();
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapMessage: gapProfileState = %d", 1, gapProfileState);
                }
                break;
            }
        case GAPSTATE_ADVERTISING:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapMessage state: GAPSTATE_ADVERTISING", 0);
                break;
            }
        case GAPSTATE_CONNECTED:
            {
                //ChangeInterval(0x08, 0x08);
                break;
            }
        case GAPSTATE_CONNECTED_ADV:
            {
                break;
            }
        default:
            {
                break;
            }
    }
    gapProfileState = (gaprole_States_t)newState;
}

/******************************************************************
 * @fn          peripheral_HandleBtGapBondStateChangeEvt
 * @brief      All the bonding state change  events are pre-handled in this function.
 *                Then the event handling function shall be called according to the newState.
 *
 * @param    newState  - new bonding state
 * @return     void
 */
void peripheral_HandleBtGapBondStateChangeEvt(uint8_t newState, uint8_t status)
{
    switch (newState)
    {
        case GAPBOND_PAIRING_STATE_STARTED:
            {
                IsAbleToSendAction = false;
#ifdef MULTIMEDIA_KEYBOARD
                IsMmKbToSendAction = FALSE;
#endif
                xTimerStart(xTimersPairing, 0);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_STARTED)", 0);
                break;
            }
        case GAPBOND_PAIRING_STATE_COMPLETE:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_COMPLETE)", 0);

                if (status == 0)
                {

                    xTimerStop(xTimersPairing, 0);
                    xTimerStart(xTimersEnableSend, 0);
                    /* disable passkey input function */
                    passKeyStructure.isAbleInput = FALSE;

                    /* enable Keyboard sending key value */
//                    IsAbleToSendAction = TRUE;
//#ifdef MULTIMEDIA_KEYBOARD
//                    IsMmKbToSendAction = TRUE;
//#endif
//                    ChangeInterval(0x08, 0x08);
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE pair success", 0);
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "BT_MSG_TYPE_BOND_STATE_CHANGE pair failed", 0);
                }
                break;
            }
        case GAPBOND_PAIRING_STATE_BONDED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_BONDED)", 0);
                break;
            }
        default:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(unknown newstate: %d)", 1, newState);
                break;
            }
    }
}

/******************************************************************
 * @fn          peripheral_HandleBtGapEncryptStateChangeEvt
 * @brief      All the encrypt state change  events are handled in this function.
 *                Then the event handling function shall be called according to the status.
 *
 * @param    newState  - new encryption state
 * @return     void
 */
void peripheral_HandleBtGapEncryptStateChangeEvt(uint8_t newState)
{
    switch (newState)
    {
        case GAPBOND_ENCRYPT_STATE_ENABLED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "GAPBOND_ENCRYPT_STATE_ENABLED", 0);
                break;
            }
        case GAPBOND_ENCRYPT_STATE_DISABLED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "GAPBOND_ENCRYPT_STATE_DISABLED", 0);
                break;
            }
        default:
            {
                break;
            }
    }
}

/******************************************************************
 * @fn          peripheral_HandleBtGapConnParaChangeEvt
 * @brief      All the connection parameter update change  events are pre-handled in this function.
 *                Then the event handling function shall be called according to the status.
 *
 * @param    status  - connection parameter result, 0 - success, otherwise fail.
 * @return     void
 */
void peripheral_HandleBtGapConnParaChangeEvt(uint8_t status)
{
    switch (status)
    {
        case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
            {
                uint16_t con_interval;
                uint16_t conn_slave_latency;
                uint16_t conn_supervision_timeout;

                peripheralGetGapParameter(GAPPRRA_CONN_INTERVAL, &con_interval);
                peripheralGetGapParameter(GAPPRRA_CONN_LATENCY, &conn_slave_latency);
                peripheralGetGapParameter(GAPPRRA_CONN_TIMEOUT, &conn_supervision_timeout);

                DBG_BUFFER(MODULE_APP, LEVEL_INFO,
                           "BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE update success, con_interval = 0x%x, conn_slave_latency = 0x%x, conn_supervision_timeout = 0x%x",
                           3, con_interval, conn_slave_latency, conn_supervision_timeout);
            }
            break;
        case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE failed.", 0);
            }
            break;
        case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE param request success.", 0);
            }
            break;
        default:
            break;
    }
}

void PassKey_HandleKeyData(uint8_t key_num, PassKey_TypeDef *pPassKeyStructure)
{
    if (key_num < 10)
    {
        if (passKeyStructure.passKeyCnt < 6)
        {
            pPassKeyStructure->passKeyValue = pPassKeyStructure->passKeyValue * 10;
            pPassKeyStructure->passKeyValue += key_num;
            pPassKeyStructure->passKeyCnt++;
        }
    }
    else if (key_num == 10)
    {
        //delete the last input key value
        pPassKeyStructure->passKeyValue = (pPassKeyStructure->passKeyValue) / 10;
        if (pPassKeyStructure->passKeyCnt > 0)
        {
            pPassKeyStructure->passKeyCnt--;
        }
    }
}

void Keyboard_GetPassKey(BEE_IO_MSG io_driver_msg_recv)
{
    UINT32 i;
    UINT8 keyCode;
    KEYSCAN_DATA_STRUCT *pKey_Data = io_driver_msg_recv.pBuf;

    for (i = 0; i < pKey_Data->Length; i++)
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "%d/%d: (row, column) = (%d, %d) n", 4, i + 1, pKey_Data->Length, pKey_Data->key[i].row, pKey_Data->key[i].column);
        keyCode = Keyboard_Keymap[pKey_Data->key[i].row][pKey_Data->key[i].column];

        switch (keyCode)
        {
            case KB_0:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_0\n\n", 0);
                    PassKey_HandleKeyData(0, &passKeyStructure);
                    break;
                }
            case KB_1:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_1\n\n", 0);
                    PassKey_HandleKeyData(1, &passKeyStructure);
                    break;
                }
            case KB_2:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_2\n\n", 0);
                    PassKey_HandleKeyData(2, &passKeyStructure);
                    break;
                }
            case KB_3:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_3\n\n", 0);
                    PassKey_HandleKeyData(3, &passKeyStructure);
                    break;
                }
            case KB_4:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_4\n\n", 0);
                    PassKey_HandleKeyData(4, &passKeyStructure);
                    break;
                }
            case KB_5:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_5\n\n", 0);
                    PassKey_HandleKeyData(5, &passKeyStructure);
                    break;
                }

            case KB_6:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey =  KB_6\n\n", 0);
                    PassKey_HandleKeyData(6, &passKeyStructure);
                    break;
                }
            case KB_7:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_7\n\n", 0);
                    PassKey_HandleKeyData(7, &passKeyStructure);
                    break;
                }
            case KB_8:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_8\n\n", 0);
                    PassKey_HandleKeyData(8, &passKeyStructure);
                    break;
                }
            case KB_9:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_9\n\n", 0);
                    PassKey_HandleKeyData(9, &passKeyStructure);
                    break;
                }
            case KB_Return_ENTER:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "passKey = KB_Return_ENTER\n\n", 0);
                    if (passKeyStructure.passKeyCnt == 6)
                    {
                        GAPBondMgr_SetParameter(GAPBOND_PASSKEY, sizeof(passKeyStructure.passKeyValue), &passKeyStructure.passKeyValue);
                        GAPBondMgr_InputPassKey();
                    }
                    else
                    {
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Error: The passkey length is not equal to six, please input again.\n", 0);
                    }
                    // clear passkey data
                    passKeyStructure.passKeyValue = 0;
                    passKeyStructure.passKeyCnt = 0;

                    break;
                }

            case KB_DELETE_Backspace:
                {
                    PassKey_HandleKeyData(10, &passKeyStructure);
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Delete the last input key value\n\n", 0);
                    break;
                }

            default:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Invalid Passkey keyCode: 0x%x\n\n", 1, keyCode);
                    break;
                }
        }
    }
}

/******************************************************************
 * @fn          peripheral_HandleBtGapMessage
 * @brief      All the bt gap msg  events are pre-handled in this function.
 *                Then the event handling function shall be called according to the subType
 *                of BEE_IO_MSG.
 *
 * @param    pBeeIoMsg  - pointer to bee io msg
 * @return     void
 */
void peripheral_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapMessage subType = %d", 1, pBeeIoMsg->subType);
    BT_STACK_MSG BtStackMsg;
    memcpy(&BtStackMsg, &pBeeIoMsg->parm, sizeof(pBeeIoMsg->parm));

    switch (pBeeIoMsg->subType)
    {
        case BT_MSG_TYPE_CONN_STATE_CHANGE:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_CONN_STATE_CHANGE:(%d->%d)",
                           2, gapProfileState, BtStackMsg.msgData.gapConnStateChange.newState);

                peripheral_HandleBtGapStateChangeEvt(BtStackMsg.msgData.gapConnStateChange.newState);
                break;
            }
        case BT_MSG_TYPE_BOND_STATE_CHANGE:
            {
                peripheral_HandleBtGapBondStateChangeEvt(BtStackMsg.msgData.gapBondStateChange.newState,
                        BtStackMsg.msgData.gapBondStateChange.status);
                break;
            }
        case BT_MSG_TYPE_BOND_PASSKEY_DISPLAY:
            {
                uint32_t displayValue = 0;
                GAPBondMgr_GetParameter(GAPBOND_PASSKEY, &displayValue);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_DISPLAY: %d", 1, displayValue);
                break;
            }
        case BT_MSG_TYPE_BOND_PASSKEY_INPUT:
            {
                /* enable to input passkey */
                passKeyStructure.passKeyValue = 0;
                passKeyStructure.passKeyCnt = 0;
                passKeyStructure.isAbleInput = TRUE;
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_INPUT", 0);
                break;
            }
        case BT_MSG_TYPE_BOND_OOB_INPUT:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_OOB_INPUT", 0);
                uint8_t ooBData[KEYLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                GAPBondMgr_SetParameter(GAPBOND_OOB_DATA, KEYLEN, ooBData);
                GAPBondMgr_InputOobData();
                break;
            }
        case BT_MSG_TYPE_ENCRYPT_STATE_CHANGE:
            {
                peripheral_HandleBtGapEncryptStateChangeEvt(BtStackMsg.msgData.gapEncryptStateChange.newState);
                break;
            }
        case BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE:
            {
                peripheral_HandleBtGapConnParaChangeEvt(BtStackMsg.msgData.gapConnParaUpdateChange.status);
                break;
            }
        default:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "peripheral_HandleBtGapMessage unknown subtype", 1, pBeeIoMsg->subType);
                break;
            }
    }
}

TAppResult AppProfileCallback(uint8_t serviceID, void *pData)
{
    TAppResult appResult = AppResult_Success;
    uint16_t sRetValue __attribute__((unused)) = 0;
    if (serviceID == ProfileAPI_ServiceUndefined)
    {
        TEventInfoCBs_t *pPara = (TEventInfoCBs_t *)pData;
        switch (pPara->eventId)
        {
            case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
                {
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "profile callback PROFILE_SRV_REGISTER_RESULT_EVT\n", 0);
                    peripheral_Init_StartAdvertising();
                    break;
                }
            case PROFILE_EVT_SEND_DATA_COMPLETE:
                {
                    uint16_t wCredits __attribute__((unused)) = pPara->sParas[0];
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "profile callback PROFILE_EVT_SEND_DATA_COMPLETE\n", 0);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    else if (serviceID == gHIDServiceId)
    {
        THID_CALLBACK_DATA *pHidCallback_data = (THID_CALLBACK_DATA *)pData;

        switch (pHidCallback_data->msg_type )
        {
            case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
                {
                    switch (pHidCallback_data->msg_data.notification_indification_index)
                    {
                        case HID_NOTIFY_INDICATE_KB_ENABLE:
                            {
                                IsKeyboardEnabled = TRUE;
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HID_NOTIFY_INDICATE_KB_ENABLE!!", 0);
                                break;
                            }
                        case HID_NOTIFY_INDICATE_KB_DISABLE:
                            {
                                IsAbleToSendAction = FALSE;
                                IsKeyboardEnabled = FALSE;
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HID_NOTIFY_INDICATE_KB_DISABLE!!", 0);
                                break;
                            }
#ifdef MULTIMEDIA_KEYBOARD
                        case HID_NOTIFY_INDICATE_MM_KB_ENABLE:
                            {
                                IsMmKeyboardEnabled = TRUE;
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HID_NOTIFY_INDICATE_MM_KB_ENABLE!!", 0);
                                break;
                            }
                        case HID_NOTIFY_INDICATE_MM_KB_DISABLE:
                            {
                                IsMmKbToSendAction = FALSE;
                                IsMmKeyboardEnabled = FALSE;
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HID_NOTIFY_INDICATE_MM_KB_DISABLE!!", 0);
                                break;
                            }
#endif
                        default:
                            {
                                break;
                            }
                    }
                    break;
                }
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    switch (pHidCallback_data->msg_data.notification_indification_index)
                    {
                        case HIDS_READ_PARAM_PROTOCOL_MODE:
                            {
                                HIDS_SetParameter(HIDS_READ_PARAM_PROTOCOL_MODE, sizeof(protocolmode), (void *)&protocolmode);
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HIDS_READ_PARAM_PROTOCOL_MODE!!", 0);
                                break;
                            }
                        case HIDS_READ_PARAM_SUSPEND_MODE:
                            {
                                HIDS_SetParameter(HIDS_READ_PARAM_SUSPEND_MODE, sizeof(suspendMode), (void *)&suspendMode);
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HIDS_READ_PARAM_SUSPEND_MODE!!", 0);
                                break;
                            }
                        case HIDS_READ_PARAM_REPORT:
                            {
                                //HIDS_SetParameter(HIDS_READ_PARAM_REPORT,sizeof(KeyboardData),(void*)&KeyboardData);
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HIDS_READ_PARAM_REPORT!!", 0);
                                break;
                            }
                        case HID_READ_OUTPUT_KEYBOARD:
                            {
                                //HIDS_SetParameter(HID_READ_OUTPUT_KEYBOARD,sizeof(KeyboardData),&KeyboardData);
                                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "HID_READ_OUTPUT_KEYBOARD!!", 0);
                                break;
                            }
                        default:
                            {
                                break;
                            }
                    }
                    break;
                }

            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
                {
                    if (pHidCallback_data->msg_data.write.write_type == HID_WRITE_PROTOCOL_MODE)
                    {
                        gProtocolMode = pHidCallback_data->msg_data.write.write_parameter.protocol_mode;
                        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, "HID_WRITE_PROTOCOL MODE %d\n", 1, gProtocolMode);
                    }

                    if (pHidCallback_data->msg_data.write.write_type == HID_WRITE_SUSPEND_MODE)
                    {
                        gSuspendMode = pHidCallback_data->msg_data.write.write_parameter.suspend_mode;
                        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, "HID_WRITE_suspend MODE %d\n", 1, gSuspendMode);
                    }
                    break;
                }
            default:
                {
                    break;
                }

        }
    }
    else if (serviceID == gBASServiceId)
    {
        TBAS_CALLBACK_DATA *pBasCallbackData = (TBAS_CALLBACK_DATA *)pData;
        switch (pBasCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
                {
                    if (pBasCallbackData->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_ENABLE)
                    {
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback BAS_NOTIFY_BATTERY_LEVEL_ENABLE\n", 0);
                    }
                    else if (pBasCallbackData->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_DISABLE)
                    {

                    }
                    break;
                }
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    BAS_SetParameter(BAS_PARAM_BATTERY_LEVEL, 1, &gBASBatteryLevel);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    else if (serviceID == gDISServiceId)
    {

    }
    else if (serviceID == gSPSServiceId)
    {
        TSPS_CALLBACK_DATA *pSpsCallbackData  = (TSPS_CALLBACK_DATA *)pData;

        switch (pSpsCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
                {
                    if (pSpsCallbackData->msg_data.notification_indification_index == SPS_NOTIFY_INDICATE_SCAN_REFRESH_ENABLE)
                    {

                    }
                    else if (pSpsCallbackData->msg_data.notification_indification_index == SPS_NOTIFY_INDICATE_SCAN_REFRESH_DISABLE)
                    {

                    }
                    break;
                }
            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
                {
                    if (pSpsCallbackData->msg_data.write.write_type == SPS_WRITE_SCAN_INTERVAL_WINDOW)
                    {
                        uint16_t scanInterval __attribute__((unused)) = pSpsCallbackData->msg_data.write.write_parameter.scan.scan_interval;
                        uint16_t scanWindow __attribute__((unused)) = pSpsCallbackData->msg_data.write.write_parameter.scan.scan_window;
                    }
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    else if (serviceID == gDISServiceId)
    {
        TDIS_CALLBACK_DATA *pDisCallbackData = (TDIS_CALLBACK_DATA *)pData;
        switch (pDisCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    if (pDisCallbackData->msg_data.read_value_index == DIS_READ_MANU_NAME_INDEX)
                    {
                        const uint8_t DISManufacturerName[] = "Realtek BT";
                        DIS_SetParameter(DIS_PARAM_MANUFACTURER_NAME,
                                         sizeof(DISManufacturerName),
                                         (void *)DISManufacturerName);

                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_MODEL_NUM_INDEX)
                    {
                        const uint8_t DISModelNumber[] = "Model Nbr 0.9";
                        DIS_SetParameter(DIS_PARAM_MODEL_NUMBER,
                                         sizeof(DISModelNumber),
                                         (void *)DISModelNumber);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_SERIAL_NUM_INDEX)
                    {
                        const uint8_t DISSerialNumber[] = "RTKBeeSerialNum";
                        DIS_SetParameter(DIS_PARAM_SERIAL_NUMBER,
                                         sizeof(DISSerialNumber),
                                         (void *)DISSerialNumber);

                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_HARDWARE_REV_INDEX)
                    {
                        const uint8_t DISHardwareRev[] = "RTKBeeHardwareRev";
                        DIS_SetParameter(DIS_PARAM_HARDWARE_REVISION,
                                         sizeof(DISHardwareRev),
                                         (void *)DISHardwareRev);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_FIRMWARE_REV_INDEX)
                    {
                        const uint8_t DISFirmwareRev[] = "RTKBeeFirmwareRev";
                        DIS_SetParameter(DIS_PARAM_FIRMWARE_REVISION,
                                         sizeof(DISFirmwareRev),
                                         (void *)DISFirmwareRev);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_SOFTWARE_REV_INDEX)
                    {
                        const uint8_t DISSoftwareRev[] = "RTKBeeSoftwareRev";
                        DIS_SetParameter(DIS_PARAM_SOFTWARE_REVISION,
                                         sizeof(DISSoftwareRev),
                                         (void *)DISSoftwareRev);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_SYSTEM_ID_INDEX)
                    {
                        uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0x5d, 0x00, 0x00, 0x00, 0x00, 3, 4, 5};
                        DIS_SetParameter(DIS_PARAM_SYSTEM_ID,
                                         sizeof(DISSystemID),
                                         DISSystemID);

                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_IEEE_CERT_STR_INDEX)
                    {
                        const uint8_t DISIEEEDataList[] = "RTKBeeIEEEDatalist";
                        DIS_SetParameter(DIS_PARAM_IEEE_DATA_LIST,
                                         sizeof(DISIEEEDataList),
                                         (void *)DISIEEEDataList);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_PNP_ID_INDEX)
                    {
                        uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0x01, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x01};
                        DIS_SetParameter(DIS_PARAM_PNP_ID,
                                         sizeof(DISPnpID),
                                         DISPnpID);
                    }
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    return appResult;
}

void ChangeInterval(uint16_t interval_min, uint16_t interval_max)
{
    uint16_t desired_min_interval = interval_min;
    uint16_t desired_max_interval = interval_max;
    uint16_t desired_slave_latency = 10;
    uint16_t desired_conn_timeout = 500;

    peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
    peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
    peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
    peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
    peripheral_SendUpdateParam();

    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "current_interval = %d\n", 1, interval_min);
}
