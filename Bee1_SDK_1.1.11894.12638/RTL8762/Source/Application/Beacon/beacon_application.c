/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     beacon_application.c
* @brief
* @details
* @author   hunter_shuai
* @date     23-June-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/
#include "rtl876x.h"
#include "application.h"
#include "bee_message.h"
#include "trace.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "profileApi.h"
#include "beacon_service.h"
#include "beacon_application.h"
#include "string.h"
#include "bas.h"
#include "dis.h"
#include "rtl876x_adc.h"
#include "rtl_types.h"
#include "ota_api.h"
#include "hal_wdg.h"
#include "version.h"
#include "stdio.h"
#include "board.h"
#include "dlps_platform.h"
#include "ota_service.h"

// gap state
gaprole_States_t gapProfileState = GAPSTATE_INIT;

/* import service id */
extern uint8_t gBeaconServiceId;
extern uint8_t gBASServiceId;
extern uint8_t gDISServiceId;
extern uint8_t gOTAServiceId;
/* import advdata */
extern uint8_t advertData[30];
extern uint16_t adv_interval;


uint8_t gBASBatteryLevel = 0;

/* flag  */
bool is_battery_notification_enabled = false;

void peripheral_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);

static bool gSwitchIntoOtaPending;

/******************************************************************
 * @fn         AppHandleIODriverMessage
 * @brief      All the application events are pre-handled in this function.
 *                All the IO MSGs are sent to this function, Then the event handling function
 *                shall be called according to the MSG type.
 *
 * @param    io_driver_msg_recv  - bee io msg data
 * @return     void
 */
void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv)
{
    UINT16 msgtype = io_driver_msg_recv.IoType;

    switch (msgtype)
    {
        case BT_STATUS_UPDATE:
            {
                peripheral_HandleBtGapMessage(&io_driver_msg_recv);
            }
            break;

        //add more IO driver message type case here
        //IO driver messge tye should be defined in bee_message.h

        default:
            break;
    }
}

/**@brief end current mode & begin enten OTA mode*/
static void SwitchIntoOTAmode(void)
{
    /*prepare to enter into OTA mode*/
    /* enable OTA mode and preparing to restart system:#include "ota_api.h""hal_wdg.h" */
    dfuSetOtaMode(ENABLE);
    HalWatchDogConfig(0, 5, 1);
    HalWatchDogEnable();
}
/******************************************************************
 * @fn       peripheral_HandleBtGapStateChangeEvt
 * @brief    All the gaprole_States_t events are pre-handled in this function.
 *           Then the event handling function shall be called according to the newState.
 *
 * @param    newState  - new gap state
 * @return   void
 */
void peripheral_HandleBtGapStateChangeEvt(uint8_t newState)
{

    switch ( newState )
    {
        //connection is disconnected or idle with no advertising
        case GAPSTATE_IDLE_NO_ADV_NO_CONN:
            {
                if (gapProfileState == GAPSTATE_CONNECTED)
                {
                    uint8_t disc_reason;
                    peripheralGetGapParameter(GAPPRRA_DISCONNECTED_REASON, &disc_reason);
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapStateChangeEvt: disc_reason = %d", 1, disc_reason);
#if CONFIG_DLPS_EN
                    /*keep active when in connection*/
                    LPS_MODE_Set(LPM_DLPS_MODE);
#endif
                    if (!gSwitchIntoOtaPending)
                    {
                        peripheral_StartAdvertising();
                    }
                    else
                    {
                        gSwitchIntoOtaPending = false;
                        SwitchIntoOTAmode();
                    }
                }
            }
            break;
        //device is advertising
        case GAPSTATE_ADVERTISING:
            {

            }
            break;

        //device is connected
        case GAPSTATE_CONNECTED:
            {
#if CONFIG_DLPS_EN
                /*keep active when in connection*/
                LPS_MODE_Set(LPM_ACTIVE_MODE);
#endif
            }
            break;

        //device is connected and advertising
        case GAPSTATE_CONNECTED_ADV:
            {

            }
            break;

        //error comes here
        default:
            {

            }
            break;

    }

    gapProfileState = (gaprole_States_t)newState;
}

/******************************************************************
 * @fn       peripheral_HandleBtGapBondStateChangeEvt
 * @brief    All the bonding state change  events are pre-handled in this function.
 *           Then the event handling function shall be called according to the newState.
 *
 * @param    newState  - new bonding state
 * @return   void
 */
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
                if (status == 0)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE pair success", 0);
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "BT_MSG_TYPE_BOND_STATE_CHANGE pair failed", 0);
                }
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

/******************************************************************
 * @fn       peripheral_HandleBtGapConnParaChangeEvt
 * @brief    All the connection parameter update change  events are pre-handled in this function.
 *           Then the event handling function shall be called according to the status.
 *
 * @param    status  - connection parameter result, 0 - success, otherwise fail.
 * @return   void
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


/**@brief modify UUID in advertising data*/
void Beacon_SetUUID(uint8_t *pUUID)
{
    for (uint8_t i = 0; i < UUID_LENGTH; i++)
    {
        advertData[i + 9] = pUUID[i];
    }
}

/**@brief modify minor id in advertising data*/
void Beason_SetMinorId(uint8_t *pMinorId)
{
    for (uint8_t i = 0; i < MINORID_LENGTH; i++)
    {
        advertData[i + 27] = pMinorId[i];
    }
}

/**@brief modify major id in advertising data*/
void Beacon_SetMajorId(uint8_t *pMajorId)
{
    for (uint8_t i = 0; i < MAJORID_LENGTH; i++)
    {
        advertData[i + 25] = pMajorId[i];
    }
}

/**@brief modify measured power in advertising data*/
void Beacon_SetMeasuredPower(uint8_t *pMeasured)
{
    for (uint8_t i = 0; i < MEASURED_POWER_LENGTH; i++)
    {
        advertData[i + 29] = pMeasured[i];
    }
}





void BeaconStartAdvertising(void)
{
    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );
    peripheral_StartAdvertising();
}

void BeaconStopAdvertising(void)
{
    peripheral_StopAdvertising();
}

void BeaconUpdateBatteryLevel(void)
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
TAppResult AppHandleProfileCallback(uint8_t serviceID, void *pData)
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
                    peripheral_Init_StartAdvertising();
                }
                break;

            default:
                break;
        }
    }
    else  if (serviceID == gBeaconServiceId)
    {
        TBEACON_CALLBACK_DATA *pBeaconCallbackData = (TBEACON_CALLBACK_DATA *)pData;
        switch (pBeaconCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Read Beacon value:%d", 1, pBeaconCallbackData->msg_data.read_value_index);
                if ( BEACON_READ_UUID_CHAR_VAL == pBeaconCallbackData->msg_data.read_value_index)
                {
                    /*indicate beacon uuid, locate in advertData[9]~[24]*/
                    BeaconBleService_SetParameter(BEACON_READ_UUID_CHAR_VAL, UUID_LENGTH, &advertData[9]);
                }
                else if ( BEACON_READ_MAJORID_CHAR_VAL == pBeaconCallbackData->msg_data.read_value_index)
                {
                    /*indicate beacon major id, locate in advertData[25]~[26]*/
                    BeaconBleService_SetParameter(BEACON_READ_MAJORID_CHAR_VAL, MAJORID_LENGTH, &advertData[25]);
                }
                else if ( BEACON_READ_MINORID_CHAR_VAL == pBeaconCallbackData->msg_data.read_value_index)
                {
                    /*indicate beacon minor id, locate in advertData[27]~[28]*/
                    BeaconBleService_SetParameter(BEACON_READ_MINORID_CHAR_VAL, MINORID_LENGTH, &advertData[27]);
                }
                else if ( BEACON_READ_MEASURED_POWER_CHAR_VAL == pBeaconCallbackData->msg_data.read_value_index)
                {
                    /*indicate beacon measured power, locate in advertData[29]*/
                    BeaconBleService_SetParameter(BEACON_READ_MEASURED_POWER_CHAR_VAL, MEASURED_POWER_LENGTH, &advertData[29]);
                }
                else if ( BEACON_READ_ADV_INTERVAL_CHAR_VAL == pBeaconCallbackData->msg_data.read_value_index)
                {
                    /*indicate beacon advertising interval*/
                    peripheralGetGapParameter( GAPPRRA_ADV_INTERVAL_MIN, &adv_interval);
                    BeaconBleService_SetParameter(BEACON_READ_ADV_INTERVAL_CHAR_VAL, sizeof(adv_interval), &adv_interval);
                }
                break;
            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Write Beacon value:%d", 1, pBeaconCallbackData->msg_data.write.opcode);
                if ( BEACON_WRITE_UUID_CHAR_VAL == pBeaconCallbackData->msg_data.write.opcode)
                {
                    /*modify beacon UUID*/
                    Beacon_SetUUID(pBeaconCallbackData->msg_data.write.value);

                    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );
                    /*restart advertising*/
                    BeaconStopAdvertising();
                    BeaconStartAdvertising();
                }
                else if ( BEACON_WRITE_MAJORID_CHAR_VAL == pBeaconCallbackData->msg_data.write.opcode)
                {
                    /*modify beacon major id*/
                    Beacon_SetMajorId(pBeaconCallbackData->msg_data.write.value);
                    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );

                    /*restart advertising*/
                    BeaconStopAdvertising();
                    BeaconStartAdvertising();
                }
                else if ( BEACON_WRITE_MINORID_CHAR_VAL == pBeaconCallbackData->msg_data.write.opcode)
                {
                    /*modify beacon minor id*/
                    Beason_SetMinorId(pBeaconCallbackData->msg_data.write.value);
                    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );

                    /*restart advertising*/
                    BeaconStopAdvertising();
                    BeaconStartAdvertising();
                }
                else if ( BEACON_WRITE_MEASURED_POWER_CHAR_VAL == pBeaconCallbackData->msg_data.write.opcode)
                {
                    /*modify beacon measured power*/
                    Beacon_SetMeasuredPower(pBeaconCallbackData->msg_data.write.value);
                    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );

                    /*restart advertising*/
                    BeaconStopAdvertising();
                    BeaconStartAdvertising();
                }

                else if (BEACON_WRITE_ADV_INTERVAL_CHAR_VAL == pBeaconCallbackData->msg_data.write.opcode)
                {
                    uint16_t interval;
                    /*modify advertising interval value*/
                    adv_interval = (pBeaconCallbackData->msg_data.write.value[0] << 8) | pBeaconCallbackData->msg_data.write.value[1];
                    interval = (adv_interval << 4) / 10;
                    /* set interval fixed value */
                    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(interval), &interval);
                    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(interval), &interval);
                }

                break;
            default:

                break;
        }
    }
    else if (gOTAServiceId == serviceID)
    {
        TOTA_CALLBACK_DATA *pOTACallbackData = (TOTA_CALLBACK_DATA *)pData;
        switch (pOTACallbackData->msg_type)
        {

            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
                {
                    if (OTA_WRITE_CHAR_VAL == pOTACallbackData->msg_data.write.opcode &&
                            OTA_VALUE_ENTER == pOTACallbackData->msg_data.write.value)
                    {
                        /*check battery level firstly*/
                        BeaconUpdateBatteryLevel();
                        if (gBASBatteryLevel > 60)
                        {
                            /*battery level is above 60 percent*/
                            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Preparing switch into OTA mode\n", 0);
                            peripheral_Disconnect();
                            gSwitchIntoOtaPending = true;
                        }
                        else
                        {
                            /*battery level is below 60 percent*/
                            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Battery level is not enough to support OTA\n", 0);
                        }
                    }
                }
                break;
            default:

                break;
        }
    }

    else if (gBASServiceId == serviceID)
    {
        TBAS_CALLBACK_DATA *pBasCallbackData = (TBAS_CALLBACK_DATA *)pData;
        switch (pBasCallbackData->msg_type)
        {   //
            case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
                {
                    if (pBasCallbackData->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_ENABLE)
                    {
                        is_battery_notification_enabled = true;
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback BAS_NOTIFY_BATTERY_LEVEL_ENABLE\n", 0);
                    }
                    else if (pBasCallbackData->msg_data.notification_indification_index == BAS_NOTIFY_BATTERY_LEVEL_DISABLE)
                    {
                        is_battery_notification_enabled = false;
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback BAS_NOTIFY_BATTERY_LEVEL_DISABLE\n", 0);
                    }
                }
                break;

            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE\n", 0);

                    /*update Battery level*/
                    BeaconUpdateBatteryLevel();
                }

                break;

            default:
                break;
        }
    }

    else if (gDISServiceId == serviceID)
    {
        TDIS_CALLBACK_DATA *pDisCallbackData = (TDIS_CALLBACK_DATA *)pData;
        switch (pDisCallbackData->msg_type)
        {   //
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
                        const uint8_t DISIEEEDataList[] = "RTKBeeIEEEDatalist";
                        DIS_SetParameter(DIS_PARAM_IEEE_DATA_LIST,
                                         sizeof(DISIEEEDataList),
                                         (void *)DISIEEEDataList);
                    }
                    else if (pDisCallbackData->msg_data.read_value_index == DIS_READ_PNP_ID_INDEX)
                    {
                        uint16_t version = VERSION_BUILD;
                        uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = {0x01, 0x5D, 0x00, 0x01, 0xFF, (uint8_t)version, (uint8_t)(version >> 8)};
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


