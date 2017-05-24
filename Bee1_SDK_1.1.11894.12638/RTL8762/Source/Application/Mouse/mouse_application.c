/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      mouse_application.c
* @brief     Mouse application implementation
* @details   mouse application implementation
* @author    Chuanguo Xue
* @date      2015-03-27
* @version   v0.3
* *********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "board.h"
#include "application.h"
#include "mouse_application.h"
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
#include "hids_mouse.h"
#include "profileApi.h"
#include "dlps_platform.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "adns3000.h"
#include "timers.h"

#if AUTOTEST
#include "mouse_autotest.h"
extern uint16_t test_count;
extern uint8_t mouse_xy_count;
uint16_t AutoTestTotalCount = 0;
#endif

/* Reference varibales statement -----------------------------------------------*/
extern uint16_t current_interval;
extern uint8_t gHIDServiceId;
extern uint8_t gBASServiceId;
extern uint8_t gDISServiceId;
extern uint8_t gSPSServiceId;

extern BOOL allowedLeftButton;
extern BOOL allowedRightButton;
extern BOOL allowedQDecoder;
extern BOOL allowedSensor;
extern BOOL allowedAutotest;

extern TimerHandle_t xTimersEnableSensor;

/* Global variables defines ----------------------------------------------------*/
uint8_t gProtocolMode;
uint8_t gSuspendMode;
BOOL SensorState = false;

/* HOGP Mouse test data
 * Mouse {Button, Delta-X, Delta-Y, Delta-Wheel}
 * Button: bit0 (Left), bit1 (Right), bit2 (Middle)
 */
const uint8_t MouseTestDataMoveUp[4] = {0x0, 0x0, 224, 0x0}; // (UINT8)224 = (INT8)-32
const uint8_t MouseTestDataMoveDown[4] = {0x0, 0x0, 32, 0x0};
const uint8_t MouseTestDataMoveRight[4] = {0x0, 32, 0x0, 0x0};
const uint8_t MouseTestDataMoveLeft[4] = {0x0, 224, 0x0, 0x0};
const uint8_t MouseTestDataRightClick[4] = {0x2, 0x0, 0x0, 0x0};
const uint8_t MouseTestDataLeftClick[4] = {0x1, 0x0, 0x0, 0x0};
#ifdef FIVE_KEY_MOUSE
const uint8_t MouseTestDataMiddleClick[4] = {0x4, 0x0, 0x0, 0x0};
const uint8_t MouseTestDataAdd1Click[4] = {0x8, 0x0, 0x0, 0x0};
const uint8_t MouseTestDataAdd2Click[4] = {0x10, 0x0, 0x0, 0x0};
#endif
const uint8_t MouseTestDataRelease[4] = {0x0, 0x0, 0x0, 0x0};
const uint8_t MouseTestDataWheelUp[4] = {0x0, 0x0, 0x0, 224};
const uint8_t MouseTestDataWheelDown[4] = {0x0, 0x0, 0x0, 32};

/* Pair / Connect Related Parameters */
/* JUST_WORKS: 3 (blueAPI_IOCapNoIO) */
#define PAIRING_MODE 3
BOOL IsAbleToSendAction = FALSE;
BOOL IsMouseEnabled = FALSE;
BOOL IsKeyboardEnabled = FALSE;

uint8_t gBASBatteryLevel = 100;
uint8_t BondFlag = 0;
gaprole_States_t gapProfileState = GAPSTATE_INIT;
uint8_t protocolmode = REPORT_PROCOCOL_MODE;
uint8_t suspendMode = ENTER_SUSPEND_MODE;
uint8_t MouseData[4] = {0, 0, 0, 0};
uint8_t mouse_data_count = 0;

/* Function declaration -------------------------------------------------------*/
void Mouse_HandleQdecodeEvent(BEE_IO_MSG io_driver_msg_recv);
void Mouse_HandleButtonEvent(BEE_IO_MSG io_driver_msg_recv);
void peripheral_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);
void ChangeInterval(uint16_t interval_min, uint16_t interval_max);

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
        case IO_QDECODE_MSG_TYPE:
            {
                Mouse_HandleQdecodeEvent(io_driver_msg_recv);
                break;
            }
        case IO_MOUSE_BUTTON_MSG_TYPE:
            {
                Mouse_HandleButtonEvent(io_driver_msg_recv);
                break;
            }
        case MOUSE_SENSOR_MSG:
            {
                if (IsAbleToSendAction && IsMouseEnabled)
                {
                    /* used to control mouse data to stack freq */
                    mouse_data_count++;

                    MouseData[1] = (uint8_t)(io_driver_msg_recv.parm >> 8);
#if AUTOTEST
                    MouseData[2] = (uint8_t)(io_driver_msg_recv.parm);
#else
                    MouseData[2] = (~(uint8_t)(io_driver_msg_recv.parm)) + 1;
#endif
                    /* send sensor data */
                    if ((IsAbleToSendAction) & (mouse_data_count == 2))
                    {
                        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "MOUSE_DATA_UPDATE. delta_x = 0x%x, delta_y = 0x%x\n", 2, MouseData[1], MouseData[2]);
                        ProfileAPI_SendData(gHIDServiceId, GATT_SRV_HID_MOUSE_INPUT_INDEX, &MouseData[0], sizeof(MouseData));
                        mouse_data_count = 0;
                        MouseData[1] = 0;
                        MouseData[2] = 0;
                    }
                }
                allowedSensor = true;
                break;
            }
        case BT_STATUS_UPDATE:
            {
                peripheral_HandleBtGapMessage(&io_driver_msg_recv);
                break;
            }
        case APP_TIMER_MSG:
            {
                if (io_driver_msg_recv.subType == TIMER_DLPS)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DLPS Timer timeout\n", 0);
                    allowedAutotest = true;

                }
                break;
            }
        default:
            {
                break;
            }
    }
}

/**
* @brief    Send Mouse Wheel Data to Host
*
*
* @param    io_driver_msg_recv
* @return   void
*/
void Mouse_HandleQdecodeEvent(BEE_IO_MSG io_driver_msg_recv)
{
    uint16_t direction = io_driver_msg_recv.subType;
    int16_t delta = (uint16_t)io_driver_msg_recv.parm;

    delta = delta * 1;
    delta = delta < 120 ? delta : 120;

    /* direction:1--up; 0-- down */
    if (!direction)
        delta = -delta;

    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Qdecoder delta value: %d\n\n", 1, delta);

    /* Send Mouse Wheel Data to the Host */
    if (IsAbleToSendAction && IsMouseEnabled && delta != 0)
    {
        MouseData[3] = delta;
        ProfileAPI_SendData(gHIDServiceId, GATT_SRV_HID_MOUSE_INPUT_INDEX, MouseData, sizeof(MouseData));
        MouseData[3] = 0;
    }

    allowedQDecoder = true;
}

/**
* @brief   Send Mouse Button Data to Host
*
*
* @param   io_driver_msg_recv
* @return  void
*/
void Mouse_HandleButtonEvent(BEE_IO_MSG io_driver_msg_recv)
{
    if (IsAbleToSendAction && IsMouseEnabled)
    {
        if (io_driver_msg_recv.subType == MOUSE_BTN_LEFT_PRESS)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Left Button Press\n", 0);
            MouseData[0] = 0x01;
            ProfileAPI_SendData(gHIDServiceId, GATT_SRV_HID_MOUSE_INPUT_INDEX, MouseData, sizeof(MouseTestDataLeftClick));
        }
        if (io_driver_msg_recv.subType  == MOUSE_BTN_LEFT_RELEASE)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Left Button Release\n", 0);
            MouseData[0] = 0;
            ProfileAPI_SendData(gHIDServiceId, GATT_SRV_HID_MOUSE_INPUT_INDEX, MouseData, sizeof(MouseTestDataLeftClick));
        }
        if (io_driver_msg_recv.subType  == MOUSE_BTN_RIGHT_PRESS)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Right Button Press\n", 0);
            MouseData[0] = 0x02;
            ProfileAPI_SendData(gHIDServiceId, GATT_SRV_HID_MOUSE_INPUT_INDEX, MouseData, sizeof(MouseTestDataRightClick));
        }
        if (io_driver_msg_recv.subType  == MOUSE_BTN_RIGHT_RELEASE)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Right Button Release\n", 0);
            MouseData[0] = 0;
            ProfileAPI_SendData(gHIDServiceId, GATT_SRV_HID_MOUSE_INPUT_INDEX, MouseData, sizeof(MouseTestDataLeftClick));
        }
    }
    if ((io_driver_msg_recv.subType == MOUSE_BTN_LEFT_PRESS) || (io_driver_msg_recv.subType == MOUSE_BTN_LEFT_RELEASE))
        allowedLeftButton = true;
    else if ((io_driver_msg_recv.subType == MOUSE_BTN_RIGHT_PRESS) || (io_driver_msg_recv.subType == MOUSE_BTN_RIGHT_RELEASE))
        allowedRightButton = true;
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
                /* Disable Send Data */
                IsAbleToSendAction = 0;
                IsMouseEnabled = 0;
                BondFlag = 0;
                if (gapProfileState == GAPSTATE_CONNECTED)
                {
                    uint8_t disc_reason;
                    peripheralGetGapParameter(GAPPRRA_DISCONNECTED_REASON, &disc_reason);
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapMessage: disc_reason = %d", 1, disc_reason);

#if AUTOTEST
                    TIM_Cmd(AutoTest_Timer, DISABLE);
                    AutoTestTotalCount++;
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "AutoTest_count = %d", 1, AutoTestTotalCount);
#else
                    /* Disable Sensor and Button */
                    System_WakeUp_Pin_Disable(SENSOR_MOTION_INT);
                    System_WakeUp_Pin_Disable(MOUSE_LEFT_BUTTON_INDEX);
                    System_WakeUp_Pin_Disable(MOUSE_RIGHT_BUTTON_INDEX);

                    GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);
                    GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
                    GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);

                    GPIO_MaskINTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);
                    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
                    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);
                    SensorState = false;
                    MouseStartAdv();
#endif

#if AUTOTEST_PAIRING
                    Mouse_StartAdv_ind();
#endif

#if AUTOTEST_CONNECTION
                    Mouse_StartAdv_direct_ind_hd();
#endif
                }
                break;
            }
        case GAPSTATE_ADVERTISING:
            {
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
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_STARTED)", 0);
                break;
            }

        case GAPBOND_PAIRING_STATE_COMPLETE:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_COMPLETE)", 0);

                if (status == 0)
                {
                    //begin to add mouse handle code
                    IsAbleToSendAction = TRUE;
                    IsMouseEnabled = TRUE;

                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE pair success", 0);
                    /* Request parameter update after paired */
                    ChangeInterval(0x08, 0x08);

#if AUTOTEST
                    DLPS_TimerInit();
                    /*--------------DLPS timer handle---------------*/
                    allowedAutotest = false;
                    /* Disable DLPS_Timer */
                    DLPS_Timer->ControlReg &= ~(BIT(0));
                    /* 20s */
                    DLPS_Timer->LoadCount = 20000 * 10000;
                    /* Enable DLPS_Timer */
                    DLPS_Timer->ControlReg |= BIT(0);

                    /*---------------Auto test timer handle---------*/
                    test_count++;
                    mouse_xy_count = 0;
                    AutoTest_TimerInit();
                    /* send mouse sensor data */
                    TIM_Cmd(AutoTest_Timer, ENABLE);
#else
                    /* Bondflag == 0, This device has bonded before, Enable sensor and button */
                    if (BondFlag == 0)
                    {
                        Mouse_Sensor_Init();
                        GPIO_MaskINTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);
                        GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
                        GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);

                        GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);
                        GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
                        GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);

                        System_WakeUp_Pin_Enable(SENSOR_MOTION_INT, 0);
                        System_WakeUp_Pin_Enable(MOUSE_LEFT_BUTTON_INDEX, 0);
                        System_WakeUp_Pin_Enable(MOUSE_RIGHT_BUTTON_INDEX, 0);
                        SensorState = true;

                        QDEC->REG_CR2 = 0xFFFFFFFF;
                        QDEC->REG_CR1 |= (0x07 << 16);
                        QDEC->REG_CR1 &= (~(0x07 << 12));

                    }
                    else
                    {
                        /* Start a timer to enable sensor, in case the host doesn't write the ccc bits */
                        xTimerStart(xTimersEnableSensor, 0);
                    }
#endif
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
                BondFlag = 1;

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
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_INPUT", 0);

                uint32_t passKey = 888888;
                GAPBondMgr_SetParameter(GAPBOND_PASSKEY, sizeof(passKey), &passKey);
                GAPBondMgr_InputPassKey();
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
            }
            break;

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

    if (serviceID == ProfileAPI_ServiceUndefined)
    {
        TEventInfoCBs_t *pPara = (TEventInfoCBs_t *)pData;
        switch (pPara->eventId)
        {
            case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
                {
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "profile callback PROFILE_SRV_REGISTER_RESULT_EVT\n", 0);
                    {
                        peripheral_Init_StartAdvertising();
                    }
                    break;
                }
            case PROFILE_EVT_SEND_DATA_COMPLETE:
                {
                    uint16_t wCredits = pPara->sParas[0];
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, \
                               "profile callback PROFILE_EVT_SEND_DATA_COMPLETE: wCredits = 0x%x\n", 1, wCredits);
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
                        case HID_NOTIFY_INDICATE_MOUSE_ENABLE:
                            {
                                IsMouseEnabled = TRUE;
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback: HID_NOTIFY_INDICATE_MOUSE_ENABLE\n", 0);

#if AUTOTEST

#else
                                if (BondFlag && !SensorState)
                                {
                                    xTimerStop(xTimersEnableSensor, 0);

                                    /* Enable sensor and button */
                                    Mouse_Sensor_Init();
                                    GPIO_MaskINTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);
                                    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
                                    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);

                                    GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);
                                    GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
                                    GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);

                                    System_WakeUp_Pin_Enable(SENSOR_MOTION_INT, 0);
                                    System_WakeUp_Pin_Enable(MOUSE_LEFT_BUTTON_INDEX, 0);
                                    System_WakeUp_Pin_Enable(MOUSE_RIGHT_BUTTON_INDEX, 0);
                                    SensorState = true;

                                    QDEC->REG_CR2 = 0xFFFFFFFF;
                                    QDEC->REG_CR1 |= (0x07 << 16);
                                    QDEC->REG_CR1 &= (~(0x07 << 12));
                                }
#endif
                                break;
                            }
                        case HID_NOTIFY_INDICATE_MOUSE_DISABLE:
                            {
                                IsAbleToSendAction = FALSE;
                                IsMouseEnabled = FALSE;
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback: HID_NOTIFY_INDICATE_MOUSE_DISABLE\n", 0);
                                break;
                            }
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
                        case HIDS_READ_PROTOCOL_MODE:
                            {
                                HIDS_SetParameter(HIDS_PARAM_PROTOCOL_MODE, sizeof(protocolmode), (void *)&protocolmode);
                                break;
                            }

                        case HIDS_READ_SUSPEND_MODE:
                            {
                                HIDS_SetParameter(HIDS_PARAM_SUSPEND_MODE, sizeof(suspendMode), (void *)&suspendMode);
                                break;
                            }
                        case HIDS_READ_PARAM_REPORT:
                            {
                                HIDS_SetParameter(HIDS_PARAM_REPORT, sizeof(MouseData), (void *)&MouseData);
                                break;
                            }
                        case HIDS_READ_BOOT_MOUSE_DATA:
                            {
                                HIDS_SetParameter(HIDS_PARAM_PROTOCOL_MODE, sizeof(MouseData), &MouseData);
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
                        uint16_t scanInterval = pSpsCallbackData->msg_data.write.write_parameter.scan.scan_interval;
                        uint16_t scanWindow = pSpsCallbackData->msg_data.write.write_parameter.scan.scan_window;

                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, \
                                   "profile callback->ERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE: scanInterval =0x%x, scanWindow =0x%x\n", \
                                   2, scanInterval, scanWindow);
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
                        uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
                        DIS_SetParameter(DIS_PARAM_SYSTEM_ID,
                                         sizeof(DISSystemID),
                                         DISSystemID);

                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_IEEE_CERT_STR_INDEX)
                    {
                        //const uint8_t DISIEEEDataList[] = "RTKBeeIEEEDatalist";
                        uint8_t DISIEEEDataList[] = "RTKBeeIEEEDatalist";
                        DIS_SetParameter(DIS_PARAM_IEEE_DATA_LIST,
                                         sizeof(DISIEEEDataList),
                                         DISIEEEDataList);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_PNP_ID_INDEX)
                    {
                        uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0};
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

    current_interval = interval_min;

    peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
    peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
    peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
    peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
    peripheral_SendUpdateParam();

    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "current_interval = %d\n", 1, current_interval);
}
