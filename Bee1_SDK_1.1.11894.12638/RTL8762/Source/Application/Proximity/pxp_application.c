/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    pxp_application.c
  * @brief   Proximitiy application implementation
    * @details Proximitiy application implementation
  * @author  Ken Mei
  * @date    16-April-2015
  * @version v0.3
  ******************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

#include "rtl876x.h"
#include "board.h"
#include "application.h"
#include "pxp_io_new.h"
#include "pxp_application.h"
#include <string.h>
#include "bee_message.h"
#include "trace.h"
#include "profileApi.h"
#include "profile_init.h"
#include "bee_message.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "ias.h"
#include "tps.h"
#include "lls.h"
#include "tps.h"
#include "pxp_ext.h"
#include "bas.h"
#include "dis.h"
#include "profileApi.h"

#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "rtl876x_adc.h"
#include "rtl876x_rtc.h"

#include "dlps_platform.h"
#include "timers.h"

extern UINT8 KEY_SEND_FLAG;

/* pxp application related services' ID. */
extern uint8_t gIASServiceId;
extern uint8_t gLLSServiceId;
extern uint8_t gTPPServiceId;

extern uint8_t gPXPEXTServiceId;
extern uint8_t gBASServiceId;
extern uint8_t gDISServiceId;
extern uint8_t gBASBatteryLevel;
extern uint8_t gPxp_linkloss_alert_level;
extern uint8_t gPxp_immediate_alert_level;
extern uint8_t gPxp_tx_power;
extern UINT32 TimeImmAct;
extern UINT32 gTimeParaValue;
extern BOOL allowedProximityEnterDlps;

/* pair / connect related parameters */
#define PAIRING_MODE 3 // JUST_WORKS: 3 (blueAPI_IOCapNoIO)
BOOL IsBeenPaired = FALSE;
UINT16 local_MDL_ID = 0;
UINT8 RemoteBd[6] = {0x9F, 0xE8, 0x09, 0x70, 0xF3, 0x5C};
uint8_t LocalDeviceName[] = "BLB_PROX";

BOOL bIsKeyPress = FALSE;

bool battery_notification_enable = false;
bool set_battery_para_enable = true;

gaprole_States_t gapProfileState = GAPSTATE_INIT;

uint8_t RepeatModeChange = FALSE;
uint8_t DLPSMode = FALSE;
uint8_t LongUpdateInterval = FALSE;
BeeLinkState PxpLinkPreState = BeeLinkStateIdle;
TimerHandle_t xTimersConParaUpdt = NULL;


extern UINT32 ImmActCnt;
extern UINT32 LssActCnt;
extern TimerHandle_t xTimersImmAlert;
extern TimerHandle_t xTimersLssAlert;

void Pxp_HandleButtonEvent(BEE_IO_MSG io_driver_msg_recv);
void peripheral_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);
void vTimerConParaCallback(xTimerHandle handle);

#if ADVERTISING_LED
bool IsAdvLED_ON = false;
TimerHandle_t xTimersAdvLED = NULL;
void vTimerAdvLEDCallback(xTimerHandle handle);
#endif

/**
  * @brief  All the application events are pre-handled in this function.
  *         All the IO MSGs are sent to this function.
  *         Then the event handling function shall be called according to the MSG type.
  * @param  io_driver_msg_recv: The BEE_IO_MSG from peripherals or BT stack state machine.
  * @retval None
  */
void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv)
{
    UINT16 msgtype = io_driver_msg_recv.IoType;

    switch (msgtype)
    {
        case IO_GPIO_MSG_TYPE:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, " ***IO_GPIO_MSG_TYPE MSG.", 0);
                Pxp_HandleButtonEvent(io_driver_msg_recv);
            }
            break;
        case BT_STATUS_UPDATE:
            {
                peripheral_HandleBtGapMessage(&io_driver_msg_recv);
            }
            break;
        default:
            break;
    }
}

void Pxp_HandleButtonEvent(BEE_IO_MSG io_driver_msg_recv)
{
    uint8_t value_to_send;
    uint8_t keytype = io_driver_msg_recv.subType ;
    switch (keytype)
    {
        case KEY_FIND_ME:
            {
                if (KEY_SEND_FLAG )
                {
                    if (io_driver_msg_recv.parm == 1)
                    {
                        value_to_send = 1;
                        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Key 0 Press\n", 0);
                        ProfileAPI_SendData(gPXPEXTServiceId, SIMPLE_SKEY_VALUE_INDEX, &value_to_send, sizeof(uint8_t));
                    }
                    if (io_driver_msg_recv.parm  == 0)
                    {
                        value_to_send = 0;
                        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Key 0 Release\n", 0);
                        ProfileAPI_SendData(gPXPEXTServiceId, SIMPLE_SKEY_VALUE_INDEX, &value_to_send, sizeof(uint8_t));
                    }
                }
            }
            break;
        case KEY_ALERT_CANCEL:
            {
                if (io_driver_msg_recv.parm  == 1)
                {

                    if (ImmActCnt)
                    {
                        xTimerStop(xTimersImmAlert, 0);
                        GPIO_ResetBits(GPIO_GetPin(LED_0));
                        GPIO_ResetBits(GPIO_GetPin(BEEP));
                        ImmActCnt = 0;
                    }
                    if (LssActCnt)
                    {
                        xTimerStop(xTimersLssAlert, 0);
                        GPIO_ResetBits(GPIO_GetPin(LED_0));
                        GPIO_ResetBits(GPIO_GetPin(BEEP));
                        LssActCnt = 0;
                    }

                    value_to_send = 2;
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Cancel Press\n", 0);
                    ProfileAPI_SendData(gPXPEXTServiceId, SIMPLE_SKEY_VALUE_INDEX, &value_to_send, sizeof(uint8_t));
                }
                if (io_driver_msg_recv.parm  == 0)
                {
                    value_to_send = 0;
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Cancel Release\n", 0);
                    ProfileAPI_SendData(gPXPEXTServiceId, SIMPLE_SKEY_VALUE_INDEX, &value_to_send, sizeof(uint8_t));
                }
            }
            break;
        default:
            break;

    }

#if CONFIG_DLPS_EN
    allowedProximityEnterDlps = true;
#endif

}

void PXP_Battery_Level_Update()
{
    uint16_t adcConvertRes;
    //enable ADC read and wait for ADC data ready interrupt
    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);   //must disable first
    ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
    /*wait for adc sample ready*/
    while (ADC_GetIntFlagStatus(ADC, ADC_INT_ONE_SHOT_DONE) != SET);
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
    /*read sample result*/
    adcConvertRes = ADC_Read(ADC, ADC_CH_BAT);
    /* return voltage (mV)  */
    if (adcConvertRes < 0x2DF2)
    {
        gBASBatteryLevel = 0;
    }
    else
    {
        /*example formula to calculate battery level, full range is 3.3v*/
        gBASBatteryLevel = (100 * (20 * ((adcConvertRes - 0x2DF2) / 0x10A + 1))) / 0xCE4;
    }
    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
    BAS_SetParameter(BAS_PARAM_BATTERY_LEVEL, 1, &gBASBatteryLevel);
}



void vTimerConParaCallback(xTimerHandle handle)
{
    uint16_t desired_min_interval = 0xE0;
    uint16_t desired_max_interval = 0xFF;
    uint16_t desired_slave_latency = 0;
    uint16_t desired_conn_timeout = 500;

    peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
    peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
    peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
    peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
    peripheral_SendUpdateParam();
    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "****CON_Para_Update****\n\n", 0);
}

#if ADVERTISING_LED
void vTimerAdvLEDCallback(xTimerHandle handle)
{
    uint8_t value;
    value = GPIO_ReadOutputDataBit(GPIO_GetPin(ADV_LED));
    if ( value )
    {
        GPIO_ResetBits(GPIO_GetPin(ADV_LED));
        xTimerChangePeriod( xTimersAdvLED, 90, 0 );
        IsAdvLED_ON = false;
    }
    else
    {
        GPIO_SetBits(GPIO_GetPin(ADV_LED));
        xTimerChangePeriod( xTimersAdvLED, 10, 0 );
        IsAdvLED_ON = true;
    }
}
#endif


void peripheral_HandleBtGapStateChangeEvt(uint8_t newState)
{
    switch ( newState )
    {
        case GAPSTATE_IDLE_NO_ADV_NO_CONN:
            {
#if ADVERTISING_LED
                if ( gapProfileState == GAPSTATE_ADVERTISING)
                {
                    xTimerStop(xTimersAdvLED, 0);
                    GPIO_ResetBits(GPIO_GetPin(ADV_LED));
                    IsAdvLED_ON = false;

                }
#endif
                if (PxpLinkPreState == BeeLinkStateLink)
                {
                    PxpLinkPreState = BeeLinkStateIdle;

                    GPIO_ResetBits(GPIO_GetPin(BEEP));
                    GPIO_ResetBits(GPIO_GetPin(KEY_0));
                    xTimerStop(xTimersImmAlert, 0);

                    if (gPxp_linkloss_alert_level)
                    {
#if CONFIG_DLPS_EN
                        allowedProximityEnterDlps = false;
#endif
                        LssActCnt = 0;
                        xTimerChangePeriod( xTimersLssAlert, 10, 0 );
                    }
                }
                if (gapProfileState == GAPSTATE_CONNECTED)
                {

                    if (gPxp_linkloss_alert_level == 0)
                    {
#if CONFIG_DLPS_EN
                        allowedProximityEnterDlps = true;
#endif
                    }
                    battery_notification_enable = false;
                    RTC_Stop();
                    KEY_SEND_FLAG = false;
                    peripheral_StartAdvertising();
                }
            }
            break;

        case GAPSTATE_ADVERTISING:
            {

#if ADVERTISING_LED
                xTimerStart(xTimersAdvLED, 0);
#endif
            }
            break;

        case GAPSTATE_CONNECTED:
            {

#if ADVERTISING_LED
                if ( gapProfileState == GAPSTATE_ADVERTISING)
                {
                    xTimerStop(xTimersAdvLED, 0);
                    GPIO_ResetBits(GPIO_GetPin(ADV_LED));
                    IsAdvLED_ON = false;

                }
#endif
                xTimerStart(xTimersConParaUpdt, 0);

                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "****GAPSTATE_CONNECTED****\n\n", 0);
                PxpLinkPreState = BeeLinkStateLink;

#if CONFIG_DLPS_EN
                allowedProximityEnterDlps = true;
#endif

                xTimerStop(xTimersLssAlert, 0);
                GPIO_ResetBits(GPIO_GetPin(LED_0));
                GPIO_ResetBits(GPIO_GetPin(BEEP));
                LssActCnt = 0;
                ImmActCnt = 0;
            }
            break;

        case GAPSTATE_CONNECTED_ADV:
            {

            }
            break;

        default:
            {

            }
            break;

    }

    gapProfileState = (gaprole_States_t)newState;
}

void peripheral_HandleBtGapBondStateChangeEvt(uint8_t newState, uint8_t status)
{
    switch (newState)
    {
        case GAPBOND_PAIRING_STATE_STARTED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_STARTED)", 0);
            }
            break;

        case GAPBOND_PAIRING_STATE_COMPLETE:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_COMPLETE)", 0);

                IsBeenPaired = TRUE;

#if AUTO_RECONNECT_DEBUG
                if (autoReconnect_Timer != NULL) {
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO,  "stop autoReconnect_Timer", 0);
                    xTimerDelete(autoReconnect_Timer, 0);
                    autoReconnect_Timer = NULL;
                }
#endif

            }
            break;

        case GAPBOND_PAIRING_STATE_BONDED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_BONDED)", 0);
            }
            break;

        default:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(unknown newstate: %d)", 1, newState);
            }
            break;
    }
}

void peripheral_HandleBtGapEncryptStateChangeEvt(uint8_t newState)
{
    switch (newState)
    {
        case GAPBOND_ENCRYPT_STATE_ENABLED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "GAPBOND_ENCRYPT_STATE_ENABLED", 0);
            }
            break;

        case GAPBOND_ENCRYPT_STATE_DISABLED:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "GAPBOND_ENCRYPT_STATE_DISABLED", 0);
            }
            break;

        default:
            break;
    }
}

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

            }
            break;

        case BT_MSG_TYPE_BOND_STATE_CHANGE:
            {
                peripheral_HandleBtGapBondStateChangeEvt(BtStackMsg.msgData.gapBondStateChange.newState,
                        BtStackMsg.msgData.gapBondStateChange.status);

            }
            break;

        case BT_MSG_TYPE_BOND_PASSKEY_DISPLAY:
            {
                uint32_t displayValue = 0;
                peripheralGetGapParameter(GAPBOND_PASSKEY, &displayValue);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_DISPLAY: %d", 1, displayValue);
            }
            break;

        case BT_MSG_TYPE_BOND_PASSKEY_INPUT:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_INPUT", 0);

                uint32_t passKey = 888888;
                peripheralSetGapParameter(GAPBOND_PASSKEY, sizeof(passKey), &passKey);
                GAPBondMgr_InputPassKey();
            }
            break;

        case BT_MSG_TYPE_BOND_OOB_INPUT:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_OOB_INPUT", 0);
                uint8_t ooBData[KEYLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                peripheralSetGapParameter(GAPBOND_OOB_DATA, KEYLEN, ooBData);
                GAPBondMgr_InputOobData();
            }
            break;

        case BT_MSG_TYPE_ENCRYPT_STATE_CHANGE:
            {
                peripheral_HandleBtGapEncryptStateChangeEvt(BtStackMsg.msgData.gapEncryptStateChange.newState);
            }
            break;

        case BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE:
            {
                peripheral_HandleBtGapConnParaChangeEvt(BtStackMsg.msgData.gapConnParaUpdateChange.status);

            }
            break;

        default:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "peripheral_HandleBtGapMessage unknown subtype", 1, pBeeIoMsg->subType);

            break;

    }

}

/******************************************************************
 * @fn       AppHandleGATTCallback
 * @brief    All the bt profile callbacks are handled in this function.
 *           Then the event handling function shall be called according to the serviceID
 *           of BEE_IO_MSG.
 *
 * @param    serviceID  -  service id of profile
 * @param    pData  - pointer to callback data
 * @return   void
 */
TAppResult AppHandleGATTCallback(uint8_t serviceID, void *pData)
{
    TAppResult appResult = AppResult_Success;
    if (serviceID == ProfileAPI_ServiceUndefined)
    {
        TEventInfoCBs_t *pPara = (TEventInfoCBs_t *)pData;
        switch (pPara->eventId)
        {
            case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "profile callback PROFILE_EVT_SRV_REG_COMPLETE\n", 0);
                {
                    peripheral_StartAdvertising();
                }
                break;
            case PROFILE_EVT_SEND_DATA_COMPLETE:
                break;
        }
    }
    else if (serviceID == gIASServiceId)
    {
        TIAS_CALLBACK_DATA *pIasCallbackData = (TIAS_CALLBACK_DATA *)pData;
        switch (pIasCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:

                gPxp_immediate_alert_level = pIasCallbackData->msg_data.write_alert_level;

                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "AlertLevel = 0x%x ", 1, gPxp_immediate_alert_level);
                ImmActCnt = 0;

#if CONFIG_DLPS_EN
                allowedProximityEnterDlps = false;
#endif
                TimeImmAct = gTimeParaValue;
                xTimerStop(xTimersImmAlert, 0);
                GPIO_ResetBits(GPIO_GetPin(LED_0));
                GPIO_ResetBits(GPIO_GetPin(BEEP));
                xTimerChangePeriod( xTimersImmAlert, 10, 0 );
                break;
            default:
                break;
        }
    }

    else if (serviceID == gLLSServiceId)
    {
        TLLS_CALLBACK_DATA *pllsCallbackData = (TLLS_CALLBACK_DATA *)pData;
        switch (pllsCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    LLS_SetParameter(LLS_PARAM_LINK_LOSS_ALERT_LEVEL, 1, &gPxp_linkloss_alert_level);
                }
                break;

            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
                gPxp_linkloss_alert_level = pllsCallbackData->msg_data.write_alert_level;
                break;

            default:
                break;
        }
    }

    else if (serviceID == gTPPServiceId)
    {
        TTPS_CALLBACK_DATA *ptpsCallbackData = (TTPS_CALLBACK_DATA *)pData;
        switch (ptpsCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    TPS_SetParameter(TPS_PARAM_TX_POWER, 1, &gPxp_tx_power);
                }

                break;
            default:
                break;
        }
    }

    else if (serviceID == gPXPEXTServiceId)
    {

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
                        battery_notification_enable = true;
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Battery level notification enable", 0);
                        RTC_Start();

                    }
                    else if (pBasCallbackData->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_DISABLE)
                    {
                        battery_notification_enable = false;
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Battery level notification disable", 0);
                        RTC_Stop();
                    }
                }
                break;
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {

                    PXP_Battery_Level_Update();
                }

                break;
            default:
                break;
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
                        const uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
                        DIS_SetParameter(DIS_PARAM_SYSTEM_ID,
                                         sizeof(DISSystemID),
                                         (void *)DISSystemID);

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
                        uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0};
                        DIS_SetParameter(DIS_PARAM_PNP_ID,
                                         sizeof(DISPnpID),
                                         DISPnpID);
                    }


                }
                break;
            default:
                break;
        }
    }
    return appResult;
}

