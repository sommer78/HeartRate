/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      SimpleBLEPeripheral_application.c
* @brief     Simple BLE Peripheral demo application implementation
* @details   Simple BLE Peripheral demo application implementation
* @author    ranhui
* @date      2015-03-27
* @version   v0.2
* *********************************************************************************************************
*/
#include "rtl876x.h"
#include "application.h"
#include "SimpleBLEPeripheral_application.h"
#include "bee_message.h"
#include "trace.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include <string.h>
#include "profileApi.h"
#include "simple_ble_service.h"

// gap state
gaprole_States_t gapProfileState = GAPSTATE_INIT;

//extern uint8_t gSimpleProfileServiceId;

void peripheral_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);

extern void traceString(uint8_t Mid, uint32_t level, uint8_t DataLength, char *pString);
extern void traceBinary(uint8_t Mid, uint8_t pkt_type, uint16_t DataLength, uint8_t *pBinaryData);
/******************************************************************
 * @fn          AppHandleIODriverMessage
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
        default:
            break;
    }
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
                    /* Reset OpCode of simple ble service's control point. */
                    SimpBleService_SetParameter(SIMPLE_BLE_SERVICE_PARAM_CTL_PNT_PROG_CLR, 0, NULL);
                    peripheral_StartAdvertising();
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
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_STARTED)", 0);
            }
            break;

        case GAPBOND_PAIRING_STATE_COMPLETE:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_COMPLETE)", 0);
                if (status == 0)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE pair success", 0);
#ifdef ANCS_SUPPORT
                    peripheral_AncsDiscover();
#endif
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "BT_MSG_TYPE_BOND_STATE_CHANGE pair failed", 0);
#ifdef ANCS_SUPPORT
                    GAPBondMgr_Pair();
#endif
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

#ifdef BT_GAP_VENDOR_CMD_ADD
void peripheral_HandleBtGapBTParamSetResult(TGAPBT_PARAM_SET_RESULT btParamSetResult)
{
    int8_t txPower;
    int8_t rssi;

    if (btParamSetResult.result == TRUE)
    {
        switch (btParamSetResult.bleParamType)
        {
            case BLE_PARAM_TYPE_TX_POWER:
                peripheralGetGapParameter(GAPPARA_BLE_TX_POWER, &txPower);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BLE_PARAM_TYPE_TX_POWER: %d", 1, txPower);
                break;
            case BLE_PARAM_TYPE_RSSI:
                peripheralGetGapParameter(GAPPARA_BLE_RSSI, &rssi);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BLE_PARAM_TYPE_RSSI: %d", 1, rssi);
                break;
            default:
                break;
        }
    }
    else
    {
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "BT_MSG_TYPE_PARAM_SET_RESULT failed!!!", 0);
    }
}
#endif

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
                GAPBondMgr_GetParameter(GAPBOND_PASSKEY, &displayValue);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_DISPLAY: %d", 1, displayValue);
            }
            break;

        case BT_MSG_TYPE_BOND_PASSKEY_INPUT:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_INPUT", 0);

                uint32_t passKey = 888888;
                GAPBondMgr_SetParameter(GAPBOND_PASSKEY, sizeof(passKey), &passKey);
                GAPBondMgr_InputPassKey();
            }
            break;

        case BT_MSG_TYPE_BOND_OOB_INPUT:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_OOB_INPUT", 0);
                uint8_t ooBData[KEYLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                GAPBondMgr_SetParameter(GAPBOND_OOB_DATA, KEYLEN, ooBData);
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

#ifdef BT_GAP_VENDOR_CMD_ADD
        case BT_MSG_TYPE_PARAM_SET_RESULT:
            {
                peripheral_HandleBtGapBTParamSetResult(BtStackMsg.msgData.gapBTParamSetResult);
            }
            break;
#endif

        default:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "peripheral_HandleBtGapMessage unknown subtype", 1, pBeeIoMsg->subType);

            break;

    }

}

/******************************************************************
 * @fn          AppProfileCallback
 * @brief      All the bt profile callbacks are handled in this function.
 *                Then the event handling function shall be called according to the serviceID
 *                of BEE_IO_MSG.
 *
 * @param    serviceID  -  service id of profile
 * @param    pData  - pointer to callback data
 * @return     void
 */
TAppResult AppProfileCallback(uint8_t serviceID, void *pData)
{
    TAppResult appResult = AppResult_Success;
    if (serviceID == ProfileAPI_ServiceUndefined)
    {
        TEventInfoCBs_t *pPara = (TEventInfoCBs_t *)pData;
        switch (pPara->eventId)
        {
            case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback PROFILE_EVT_SRV_REG_COMPLETE\n", 0);
                {
                    peripheral_Init_StartAdvertising();
                }
                break;

            case PROFILE_EVT_SEND_DATA_COMPLETE:
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "profile callback PROFILE_EVT_SEND_DATA_COMPLETE, wCredits = %d\n",
                           1, pPara->sParas[0]);
                {
                    /* Reset OpCode of simple ble service's control point. */
                    SimpBleService_SetParameter(SIMPLE_BLE_SERVICE_PARAM_CTL_PNT_PROG_CLR, 0, NULL);
                }
                break;

            default:
                break;
        }
    }
  // else  if (serviceID == gSimpleProfileServiceId)
			  else  if (serviceID == 0x00)
    {
        TSIMP_CALLBACK_DATA *pSimpCallbackData = (TSIMP_CALLBACK_DATA *)pData;
        switch (pSimpCallbackData->msg_type)
        {
            case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION\n", 0);
                    switch (pSimpCallbackData->msg_data.notification_indification_index)
                    {
                        case SIMP_NOTIFY_INDICATE_V3_ENABLE:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_NOTIFY_INDICATE_V3_ENABLE\n", 0);
                            }
                            break;

                        case SIMP_NOTIFY_INDICATE_V3_DISABLE:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_NOTIFY_INDICATE_V3_DISABLE\n", 0);
                            }
                            break;
                        case SIMP_NOTIFY_INDICATE_V4_ENABLE:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_NOTIFY_INDICATE_V4_ENABLE\n", 0);
                            }
                            break;
                        case SIMP_NOTIFY_INDICATE_V4_DISABLE:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_NOTIFY_INDICATE_V4_DISABLE\n", 0);
                            }
                            break;
                        case SIMP_NOTIFY_INDICATE_V5_ENABLE:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_NOTIFY_INDICATE_V5_ENABLE\n", 0);
                            }
                            break;
                        case SIMP_NOTIFY_INDICATE_V5_DISABLE:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_NOTIFY_INDICATE_V5_DISABLE\n", 0);
                            }
                            break;
                    }
                }
                break;


            case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE\n", 0);
                    if (pSimpCallbackData->msg_data.read_value_index == SIMP_READ_V1)
                    {
                        uint8_t value1 = 0x88;
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_READ_V1: 0x%x\n", 1, value1);
                        SimpBleService_SetParameter(SIMPLE_BLE_SERVICE_PARAM_V1_READ_CHAR_VAL, 1, &value1);
                    }
                }
                break;
            case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE\n", 0);
                    switch (pSimpCallbackData->msg_data.write.opcode)
                    {
                        case SIMP_WRITE_V2:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_WRITE_V2: value = 0x%x\n", 1, pSimpCallbackData->msg_data.write.value);
                            }
                            break;
                        case SIMP_WRITE_V5_OPCODE_SET_CHAR_INDICATE_VAL:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_WRITE_V5_OPCODE_SET_CHAR_INDICATE_VAL: value = 0x%x\n", 1, pSimpCallbackData->msg_data.write.value);
                            }
                            break;
                        case SIMP_WRITE_V5_OPCODE_SET_CHAR_NOTIFY_VAL:
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SIMP_WRITE_V5_OPCODE_SET_CHAR_NOTIFY_VAL: value = 0x%x\n", 1, pSimpCallbackData->msg_data.write.value);
                            }
                            break;
                        default:
                            break;
                    }
                }
                break;

            default:
                break;
        }
    }
    return appResult;
}

#ifdef ANCS_SUPPORT
void AppParseDataSoucreNotifications(uint8_t *pData, uint8_t len)
{
#ifdef ANCS_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "AppParseDataSoucreNotifications: ANCS_FROM_DATA_SOURCE, len =%d", 1, len);
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "data = ", 0);
    traceBinary(0, 0xf0, len, pData);
#endif
    static uint8_t *ptr;
    static uint16_t current_len;
    static DS_NOTIFICATION_ATTR notification_attr;
    static DS_APP_ATTR app_attr;
    static uint8_t m_parse_state = ATTRIBUTE_PARSE_STATE_NOT_START;
    int i;

    if (m_parse_state == ATTRIBUTE_PARSE_STATE_NOT_START)
    {
        if (len >= 1 && pData[0] == CP_CommandIDGetNotificationAttributes)
        {
            m_parse_state = COMMAND_ID_GET_NOTIFICATION_ATTR;
        }
        else if (len >= 1 && pData[0] == CP_CommandIDGetAppAttributes)
        {
            m_parse_state = COMMAND_ID_GET_APP_ATTR;
        }
    }


    for (i = 0; i < len; i++)
    {
        switch (m_parse_state)
        {
            case COMMAND_ID_GET_NOTIFICATION_ATTR:
                notification_attr.command_id = pData[i];
                m_parse_state = NOTIFICATION_UID1;
                break;

            case NOTIFICATION_UID1:
                notification_attr.notification_uid[0] = pData[i];
                m_parse_state = NOTIFICATION_UID2;
                break;

            case NOTIFICATION_UID2:
                notification_attr.notification_uid[1] = pData[i];
                m_parse_state = NOTIFICATION_UID3;
                break;

            case NOTIFICATION_UID3:
                notification_attr.notification_uid[2] = pData[i];
                m_parse_state = NOTIFICATION_UID4;
                break;

            case NOTIFICATION_UID4:
                notification_attr.notification_uid[3] = pData[i];
                m_parse_state = NOTIFICATION_ATTRIBUTE_ID;
                break;

            case NOTIFICATION_ATTRIBUTE_ID:
                notification_attr.attribute_id = pData[i];
                m_parse_state = NOTIFICATION_ATTRIBUTE_LEN1;
                break;

            case NOTIFICATION_ATTRIBUTE_LEN1:
                notification_attr.attribute_len = pData[i];
                m_parse_state = NOTIFICATION_ATTRIBUTE_LEN2;
                break;

            case NOTIFICATION_ATTRIBUTE_LEN2:
                notification_attr.attribute_len |= (pData[i] << 8);
                m_parse_state = NOTIFICATION_ATTRIBUTE_READY;
                ptr = notification_attr.data;
                current_len = 0;

                if (notification_attr.attribute_len == 0)
                {
                    m_parse_state = NOTIFICATION_ATTRIBUTE_ID;
#ifdef ANCS_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Notification attribute_id = %d, attribute_len = %d", 2,
                               notification_attr.attribute_id,
                               notification_attr.attribute_len
                              );
#endif
                }
                break;

            case NOTIFICATION_ATTRIBUTE_READY:
                *ptr++ = pData[i];
                current_len++;

                if (current_len == notification_attr.attribute_len)
                {
                    *ptr++ = 0;
#ifdef ANCS_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Notification attribute_id = %d, attribute_len = %d", 2,
                               notification_attr.attribute_id,
                               notification_attr.attribute_len
                              );
                    traceString(0, 0, notification_attr.attribute_len, (char *)notification_attr.data);
#endif
                    if (notification_attr.attribute_id != DS_NotificationAttributeIDNegativeActionLabel)
                    {
                        m_parse_state = NOTIFICATION_ATTRIBUTE_ID;

                        if (notification_attr.attribute_id == DS_NotificationAttributeIDAppIdentifier)
                        {
                            uint8_t AttributeIDList[1];
                            uint8_t iCurIndex = 0;
                            AttributeIDList[iCurIndex++] = DS_AppAttributeIDDisplayName;
                            peripheral_AncsGetAppAttributes(notification_attr.data, (char *)AttributeIDList, iCurIndex);
                        }

                    }
                    else//All attributes has been parased
                    {
                        m_parse_state = ATTRIBUTE_PARSE_STATE_NOT_START;
                    }
                }
                break;
        }
    }


    for (i = 0; i < len; i++)
    {
        switch (m_parse_state)
        {
            case COMMAND_ID_GET_APP_ATTR:
                app_attr.command_id = pData[i];
                m_parse_state = APP_IDENTIFIER_START;
                break;

            case APP_IDENTIFIER_START:
                if (pData[i] == 0x00)
                {
                    m_parse_state = APP_IDENTIFIER_END;

                    if (i + 1 == len)
                    {
                        m_parse_state = ATTRIBUTE_PARSE_STATE_NOT_START;

                        //remove notification if wanted
#if 0
                        uint32_t notificationUID = notification_attr.notification_uid[0];
                        notificationUID |= notification_attr.notification_uid[1] << 8;
                        notificationUID |= notification_attr.notification_uid[2] << 8;
                        notificationUID |= notification_attr.notification_uid[3] << 8;
                        peripheral_AncsPerformNotificationAction(notificationUID, CP_ActionIDNegative);
#endif
                    }
                }
                break;
            case APP_IDENTIFIER_END:
                i++;
                m_parse_state = APP_ATTRIBUTE_ID;
                break;

            case APP_ATTRIBUTE_ID:
                app_attr.attribute_id = pData[i];
                m_parse_state = APP_ATTRIBUTE_LEN1;
                break;

            case APP_ATTRIBUTE_LEN1:
                app_attr.attribute_len = pData[i];
                m_parse_state = APP_ATTRIBUTE_LEN2;
                break;

            case APP_ATTRIBUTE_LEN2:
                app_attr.attribute_len |= (pData[i] << 8);
                m_parse_state = APP_ATTRIBUTE_READY;
                ptr = app_attr.data;
                current_len = 0;

                if (app_attr.attribute_len == 0)
                {
                    m_parse_state = APP_ATTRIBUTE_ID;
#ifdef ANCS_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "App attribute_id = %d, attribute_len = %d", 2,
                               app_attr.attribute_id,
                               app_attr.attribute_len
                              );
#endif
                }
                break;

            case APP_ATTRIBUTE_READY:
                *ptr++ = pData[i];
                current_len++;

                if (current_len == app_attr.attribute_len)
                {
                    *ptr++ = 0;
#ifdef ANCS_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "App attribute_id = %d, attribute_len = %d", 2,
                               app_attr.attribute_id,
                               app_attr.attribute_len
                              );
                    traceString(0, 0, app_attr.attribute_len, (char *)app_attr.data);
#endif
                    m_parse_state = ATTRIBUTE_PARSE_STATE_NOT_START;

                }
                break;
        }
    }



}

void AppAncsNotificationCallback(uint8_t ancs_notification_type, uint8_t *pData, uint8_t len)
{
    if (ANCS_DISCOVERY_COMPLETE == ancs_notification_type)
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "AppAncsNotificationCallback: ANCS_DISCOVERY_COMPLETE", 0);
#endif
        peripheral_AncsSetDataSourceNotify(true);
    }
    else if (ANCS_FROM_NOTIFICATION_SOURCE == ancs_notification_type)
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "AppAncsNotificationCallback: ANCS_FROM_NOTIFICATION_SOURCE, len =%d", 1, len);
#endif
        if (8 == len)
        {
            TANCS_NOTIFICATION_SOURCE_DATA NsData;
            memcpy(&NsData, pData, len);
#ifdef ANCS_DEBUG
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "EventID = %d, EventFlags = 0x%02x, CategoryID = %d, CategoryCount = %d, NotificationID = 0x%08x, ", 5,
                       NsData.EventID,
                       NsData.EventFlags,
                       NsData.CategoryID,
                       NsData.CategoryCount,
                       NsData.NotificationID
                      );
#endif
            //you can filter by CategoryID here, for demo purpose, we didn't filter any CategoryID here.

            uint8_t AttributeIDList[14];
            uint8_t iCurIndex = 0;
            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDAppIdentifier;
            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDTitle;
            AttributeIDList[iCurIndex++] = 0xFF;
            AttributeIDList[iCurIndex++] = 0xFF;

            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDSubtitle;
            AttributeIDList[iCurIndex++] = 0xFF;
            AttributeIDList[iCurIndex++] = 0xFF;

            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDMessage;
            AttributeIDList[iCurIndex++] = 0xFF;
            AttributeIDList[iCurIndex++] = 0xFF;

            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDMessageSize;
            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDDate;
            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDPositiveActionLabel;
            AttributeIDList[iCurIndex++] = DS_NotificationAttributeIDNegativeActionLabel;

            peripheral_AncsGetNotificationAttributes(NsData.NotificationID, AttributeIDList, iCurIndex);

        }
    }
    else if (ANCS_FROM_DATA_SOURCE == ancs_notification_type)
    {
        AppParseDataSoucreNotifications(pData, len);
    }
    else if (ANCS_NOTIFICATION_SOURCE_NOTIFY_ENABLE == ancs_notification_type)
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "AppAncsNotificationCallback: ANCS_NOTIFICATION_SOURCE_NOTIFY_ENABLE", 0);
#endif
    }
    else if (ANCS_NOTIFICATION_SOURCE_NOTIFY_DISABLE == ancs_notification_type)
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "AppAncsNotificationCallback: ANCS_NOTIFICATION_SOURCE_NOTIFY_DISABLE", 0);
#endif
    }
    else if (ANCS_DATA_SOURCE_NOTIFY_ENABLE == ancs_notification_type)
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "AppAncsNotificationCallback: ANCS_DATA_SOURCE_NOTIFY_ENABLE", 0);
#endif
        peripheral_AncsSetNotificationSourceNotify(true);
    }
    else if (ANCS_DATA_SOURCE_NOTIFY_DISABLE == ancs_notification_type)
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "AppAncsNotificationCallback: ANCS_DATA_SOURCE_NOTIFY_DISABLE", 0);
#endif
    }

}

#endif

