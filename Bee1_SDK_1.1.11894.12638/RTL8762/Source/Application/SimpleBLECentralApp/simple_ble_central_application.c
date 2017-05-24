/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      simple_ble_central_application.c
* @brief     Simple BLE Central demo application implementation
* @details   Simple BLE Central demo application implementation
* @author    ranhui
* @date      2015-09-10
* @version   v1.0
* *********************************************************************************************************
*/
#include <string.h>
#include "rtl876x.h"
#include "bee_message.h"
#include "trace.h"
#include "bee_message.h"
#include "central.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "profileAPI.h"
#include "application.h"
#include "simple_ble_central_application.h"
#include "user_cmd.h"
#include "user_cmd_parse.h"
#include "simple_ble_client.h"

/* User command interface data, used to parse the commands from Data UART. */
TUserCmdIF    gUserCmdIF;
/* Client ID of Simple BLE Client Module, generated when add this module in main.c. Can't be modified by user. */
extern TClientID   SimBleClientID;
/* Central Role state maintained in application. */
gaprole_States_t GapCentralRoleState = GAP_CENTRAL_STATE_INIT;

void central_HandleBtGapMessage(BEE_IO_MSG *pBeeIoMsg);

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
	uint8_t RxChar;

    switch(msgtype)
    {
        case BT_STATUS_UPDATE:
            central_HandleBtGapMessage(&io_driver_msg_recv);
            break;
        case IO_UART_MSG_TYPE:
            /* We handle user command informations from Data UART in this branch. */
            RxChar = (uint8_t)io_driver_msg_recv.subType;
            User_CmdCollect(&gUserCmdIF, &RxChar, sizeof(RxChar), UserCmdTable);
            break;
        default:
            break;
    }
}

/**
  * @brief  handle messages indicate that GAP state has changed.
  * @param  newState: GAP state.
  * @retval none
  */
void central_HandleBtGapStateChangeEvt(uint8_t newState)
{
    switch ( newState )
    {
        case GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN:
            {

            }
            break;
        case GAP_CENTRAL_STATE_SCANNING:
            {
                
            }
            break;
        case GAP_CENTRAL_STATE_CONNECTING:
            {

            }
            break;
        case GAP_CENTRAL_STATE_CONNECTED:
            {
                
            }
            break;
        default:
            break;
    }

    GapCentralRoleState = (gaprole_States_t)newState;
}

/**
  * @brief  handle messages indicate that GAP bond state has changed.
  * @param  newState: GAP bond state.
  * @retval none
  */
void central_HandleBtGapBondStateChangeEvt(uint8_t newState, uint8_t status)
{
    switch(newState)
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

/**
  * @brief  handle messages indicate that encrypt state has changed.
  * @param  newState: encrypt state.
  * @retval none
  */
void central_HandleBtGapEncryptStateChangeEvt(uint8_t newState)
{
    switch(newState)
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

/**
  * @brief  handle messages indicate that connection parameters has changed.
  * @param  status: change status.
  * @retval none
  */
void central_HandleBtGapConnParaChangeEvt(uint8_t status)
{
    if(status == 0)
    {
        uint16_t con_interval;
        uint16_t conn_slave_latency;
        uint16_t conn_supervision_timeout;

        centralGetGapParameter(GAPPRRA_CONN_INTERVAL, &con_interval);
        centralGetGapParameter(GAPPRRA_CONN_LATENCY, &conn_slave_latency);
        centralGetGapParameter(GAPPRRA_CONN_TIMEOUT, &conn_supervision_timeout);

        DBG_BUFFER(MODULE_APP, LEVEL_INFO,
            "BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE success, con_interval = 0x%x, conn_slave_latency = 0x%x, conn_supervision_timeout = 0x%x",
            3, con_interval, conn_slave_latency, conn_supervision_timeout);
    }
    else
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE failed, status = %d",
            1, status);
    }
}

#ifdef BT_GAP_VENDOR_CMD_ADD
void central_HandleBtGapBTParamSetResult(TGAPBT_PARAM_SET_RESULT btParamSetResult)
{
    if(btParamSetResult.result == TRUE)
    {
        switch(btParamSetResult.bleParamType)
        {
            case BLE_PARAM_TYPE_TX_POWER:
            {
                int8_t txPower;
                centralGetGapParameter(GAPPARA_BLE_TX_POWER, &txPower);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BLE_PARAM_TYPE_TX_POWER: %d",1,txPower);
            }
                break;
            case BLE_PARAM_TYPE_RSSI:
            {
                int8_t rssi;
                centralGetGapParameter(GAPPARA_BLE_RSSI, &rssi);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BLE_PARAM_TYPE_RSSI: %d",1,rssi);
            }
                break;
            case BLE_PARAM_TYPE_SET_CHANNEL_CLASS:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BLE_PARAM_TYPE_SET_CHANNEL_CLASS: success",0);
            }
                break;
            default:
                break;
        }
    }
    else
    {
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "BT_MSG_TYPE_PARAM_SET_RESULT failed!!!",0);
    }
}
#endif

/**
  * @brief  handle messages from GAP layer.
  * @param  pBeeIoMsg: message from GAP layer.
  * @retval none
  */
void central_HandleBtGapMessage(BEE_IO_MSG *pBeeIoMsg)
{
    BT_STACK_MSG BtStackMsg;
    
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "central_HandleBtGapMessage subType = %d", 1, pBeeIoMsg->subType);
    memcpy(&BtStackMsg, &pBeeIoMsg->parm, sizeof(pBeeIoMsg->parm));

    switch(pBeeIoMsg->subType)
    {
        case BT_MSG_TYPE_CONN_STATE_CHANGE:
        {
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_CONN_STATE_CHANGE:(%d->%d)", 
                       2, GapCentralRoleState, BtStackMsg.msgData.gapConnStateChange.newState);
            central_HandleBtGapStateChangeEvt(BtStackMsg.msgData.gapConnStateChange.newState);
        }
            break;
        case BT_MSG_TYPE_BOND_STATE_CHANGE:
        {
            central_HandleBtGapBondStateChangeEvt(BtStackMsg.msgData.gapBondStateChange.newState,
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
           central_HandleBtGapEncryptStateChangeEvt(BtStackMsg.msgData.gapEncryptStateChange.newState);
        }
            break;
        case BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE:
        {
            central_HandleBtGapConnParaChangeEvt(BtStackMsg.msgData.gapConnParaUpdateChange.status);
        }
            break;
#ifdef BT_GAP_VENDOR_CMD_ADD
        case BT_MSG_TYPE_PARAM_SET_RESULT:
        {
            central_HandleBtGapBTParamSetResult(BtStackMsg.msgData.gapBTParamSetResult);
        }
            break;
#endif
        default:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "central_HandleBtGapMessage unknown subtype", 1, pBeeIoMsg->subType);
            break;
    }

}

/**
 * @internal
 *
 * @brief   Split valid information from peer device's ADV data or SCAN RSP data.
 *          NOTE--this function just for demo, user can cutter the code according
 *                to requirement.
 *
 * @param   pLeScanInfo - message informed from upper stack
 *
 */
static void App_SplitScanInfo(PBlueAPI_LEScanInfo pLeScanInfo)
{
    uint8_t buffer[32];
    uint8_t pos = 0;

    while (pos < pLeScanInfo->dataLength)
    {
        /* Length of the AD structure. */
        uint8_t length = pLeScanInfo->data[pos++];
        uint8_t type;

        if (length < 0x02 || length > 0x1F || (pos + length) > 0x1F)
            continue;
        
        /* Copy the AD Data to buffer. */
        memcpy(buffer, pLeScanInfo->data + pos + 1, length -1);
        /* AD Type, one octet. */
        type = pLeScanInfo->data[pos];
        
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Structure Info: AD type = 0x%x, AD Data Length = %d", 2, type, length -1);

        switch (type)
        {
            case GAP_ADTYPE_FLAGS:
            {
                /* (flags & 0x01) -- LE Limited Discoverable Mode */
                /* (flags & 0x02) -- LE General Discoverable Mode */
                /* (flags & 0x04) -- BR/EDR Not Supported */
                /* (flags & 0x08) -- Simultaneous LE and BR/EDR to Same Device Capable (Controller) */
                /* (flags & 0x10) -- Simultaneous LE and BR/EDR to Same Device Capable (Host) */
                uint8_t flags = pLeScanInfo->data[pos +1];
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: Flags = 0x%x", 1, flags);
            }
                break;

            case GAP_ADTYPE_16BIT_MORE:
            case GAP_ADTYPE_16BIT_COMPLETE:
            case GAP_ADTYPE_SERVICES_LIST_16BIT:
            {
                uint16_t *pUUID = (uint16_t *)(buffer);
                uint8_t i = length - 1;

                while (i >= 2)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: UUID16 List Item %d = 0x%x", 2, i/2, *pUUID++);
                    i -= 2;
                }
            }
                break;

            case GAP_ADTYPE_32BIT_MORE:
            case GAP_ADTYPE_32BIT_COMPLETE:
            {
                uint32_t *pUUID = (uint32_t *)(buffer);
                uint8_t    i     = length - 1;

                while (i >= 4)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: UUID32 List Item %d = 0x%x", 2, i/4, *pUUID++);
                    i -= 4;
                }
            }
                break;

            case GAP_ADTYPE_128BIT_MORE:
            case GAP_ADTYPE_128BIT_COMPLETE:
            case GAP_ADTYPE_SERVICES_LIST_128BIT:
            {
                uint32_t *pUUID = (uint32_t *)(buffer);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: UUID128 value: 0x%8.8x%8.8x%8.8x%8.8x", 4,
                            pUUID[3], pUUID[2], pUUID[1], pUUID[0]);
            }
                break;

            case GAP_ADTYPE_LOCAL_NAME_SHORT:
            case GAP_ADTYPE_LOCAL_NAME_COMPLETE:
            {
                buffer[length -1] = '\0';
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: Local Name", 0);
            }
                break;

            case GAP_ADTYPE_POWER_LEVEL:
            {
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: TX power = 0x%x", 1, pLeScanInfo->data[pos + 1]);
            }
            break;

            case GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE:
            {
                uint16_t *pMin = (uint16_t *)(buffer);
                uint16_t *pMax = pMin +1;
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: Slave conn interval range, 0x%x - 0x%x", 2, *pMin, *pMax);
            }
                break;

            case GAP_ADTYPE_SERVICE_DATA:
            case GAP_ADTYPE_MANUFACTURER_SPECIFIC:
            {
                uint16_t *pUUID = (uint16_t *)(buffer);
                uint8_t    i;

                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: Service Data UUID = 0x%x, len = 0x%x", 2, *pUUID, length - 2);

                for (i = 2; i < length; i++)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: Service Data = 0x%x", 1, buffer[i]);
                }
            }
                break;
                
            default:
            {
                uint8_t i = 0;

                for (i = 0; i < (length-1); i++)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  AD Data: Unhandled Data = 0x%x", 1, pLeScanInfo->data[pos + i]);
                }
            }
                break;
        }

        pos += length;
    }
}

/**
  * @brief  This callback will be called when advertising or scan response data received.
  * @param  msgType: type of the message sent from GAP Central Role layer.
  * @param  centralCbData: message sent from GAP Central Role layer.
  * @retval None
  */
TAppResult App_CentralRoleCallback(TCentralAppCB_MsgType msgType, TCentralAppCB_MsgData centralCbData)
{
    TAppResult appResult = AppResult_Success;
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "App_CentralRoleCallback: msgType = %d", 1, msgType);
    switch(msgType)
    {
        case ADV_SCAN_RSP_DATA_MSGTYPE:
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  ADV_SCAN_RSP_DATA_MSGTYPE: bd = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x, bdtype=%d, event=0x%x, rssi=%d, len=%d",
                10, centralCbData.pAdvScanRspData->remote_BD[5],
                   centralCbData.pAdvScanRspData->remote_BD[4],
                   centralCbData.pAdvScanRspData->remote_BD[3],
                   centralCbData.pAdvScanRspData->remote_BD[2],
                   centralCbData.pAdvScanRspData->remote_BD[1],
                   centralCbData.pAdvScanRspData->remote_BD[0],
                   centralCbData.pAdvScanRspData->remote_BD_type,
                   centralCbData.pAdvScanRspData->advType,
                   centralCbData.pAdvScanRspData->rssi,
                   centralCbData.pAdvScanRspData->dataLength);
            /* User can split interested information by using the function as follow. */
            App_SplitScanInfo(centralCbData.pAdvScanRspData);
            break;
        case CONN_PARAM_CONFIRM_MSGTYPE:
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "  CONN_PARAM_CONFIRM_MSGTYPE: maxInterval=0x%x, minInterval=0x%x, Latency=0x%x,timeOut=0x%x",
                4, centralCbData.pConnUpdateIndData->connIntervalMax,
                   centralCbData.pConnUpdateIndData->connIntervalMin,
                   centralCbData.pConnUpdateIndData->connLatency,
                   centralCbData.pConnUpdateIndData->supervisionTimeout);
            /* if reject the proposed connection parameter from peer device, use AppResult_Reject. */
            appResult = AppResult_Accept;
            break;
        default:
            break;
    }
    return appResult;
}

/**
  * @brief  Callback will be called when data sent from specific client module or profile client layer.
  * @param  clientID: the ID distinguish which module sent the data.
  * @param  pData: pointer to data.
  * @retval TProfileResult
  */
TAppResult App_ClientCallback(TClientID clientID, void *pData)
{
    TAppResult  appResult = AppResult_Success;

    if(clientID == AppProcessGeneralClientMsgID)
    {
        TClientAppCB_Data *pClientAppCbData = (TClientAppCB_Data *)pData;
        switch(pClientAppCbData->cb_type)
        {
            case CLIENT_APP_CB_TYPE_DISC_STATE:
                if(pClientAppCbData->cb_content.discStateCB_Data.discState == DISC_STATE_SRV_DONE)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Discovery Service Procedure Done.", 0);
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Discovery state send to application directly.", 0);
                }
                break;
            case CLIENT_APP_CB_TYPE_DISC_RESULT:
                if(pClientAppCbData->cb_content.discResultCB_Data.resultType == DISC_RESULT_ALL_SRV_UUID16)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Discovery All Primary Service: UUID16=0x%x,StartHdl=0x%x,EndHdl=0x%x.", 3,
                            pClientAppCbData->cb_content.discResultCB_Data.resultData.pSrvUUID16DiscData->UUID16,
                            pClientAppCbData->cb_content.discResultCB_Data.resultData.pSrvUUID16DiscData->attHandle,
                            pClientAppCbData->cb_content.discResultCB_Data.resultData.pSrvUUID16DiscData->endGroupHandle);
                }
                else
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Discovery result send to application directly.", 0);
                }
                break;
            case CLIENT_APP_CB_TYPE_READ_RESULT:
                break;
            case CLIENT_APP_CB_TYPE_WRITE_RESULT:
                break;
            case CLIENT_APP_CB_TYPE_NOTIF_IND_RESULT:
                break;
            default:
                break;
        }
    }
    else if(clientID == SimBleClientID)
    {
        TSimpClientCB_Data *pSimpCallbackData = (TSimpClientCB_Data *)pData;
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB Type: %d.", 1, pSimpCallbackData->cb_type);
        switch (pSimpCallbackData->cb_type)
        {
            case SIMP_CLIENT_CB_TYPE_DISC_STATE:
                switch(pSimpCallbackData->cb_content.disc_state)
                {
                    case SIMP_DISC_DONE:
                        /* Discovery Simple BLE service procedure successfully done. */
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: discover procedure done.", 0);
                        break;
                    case SIMP_DISC_PENDING:
                        /* Discovery Request failed. */
                        DBG_BUFFER(MODULE_APP, LEVEL_WARN, "Simple BLE Client CB: discover request failed.", 0);
                        break;
                    default:
                        break;
                }
                break;
            case SIMP_CLIENT_CB_TYPE_DISC_HDL:
                /* Tiis branch executed once handle is found. */
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: discover handle, idx--value %d--0x%4.4x.", 2,
                            pSimpCallbackData->cb_content.disc_hdl_info.handle_type,
                            pSimpCallbackData->cb_content.disc_hdl_info.handle_value);
                /* Here means app handle changed, we must set them to rsc client module. */
                SimpBleClient_SetHandle(pSimpCallbackData->cb_content.disc_hdl_info.handle_type, pSimpCallbackData->cb_content.disc_hdl_info.handle_value);
                break;
            case SIMP_CLIENT_CB_TYPE_READ_RESULT:
                switch(pSimpCallbackData->cb_content.read_data.read_result)
                {

                    case SIMP_READ_RESULT_V1_READ:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: read v1_read = %d.",
                                    1,pSimpCallbackData->cb_content.read_data.read_content.v1_read);
                        break;
                    case SIMP_READ_RESULT_V3_NOTIFY_NOTIF_BIT:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: read v3_notify_notif_flag = %d.",
                                    1,pSimpCallbackData->cb_content.read_data.read_content.v3_notify_notif_flag);
                        break;
                    case SIMP_READ_RESULT_V4_INDICATE_IND_BIT:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: read v4_indicate_ind_flag = %d.",
                                    1,pSimpCallbackData->cb_content.read_data.read_content.v4_indicate_ind_flag);
                        break;
                    case SIMP_READ_RESULT_V5_CTL_PNT_IND_BIT:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: read v5_ctl_pnt_ind_flag = %d.",
                                    1,pSimpCallbackData->cb_content.read_data.read_content.v5_ctl_pnt_ind_flag);
                        break;
                    case SIMP_READ_RESULT_FAIL:
                        DBG_BUFFER(MODULE_APP, LEVEL_WARN, "Simple BLE Client CB: read request failed.", 0);
                        break;
                    default:
                        break;
                }
                break;
            case SIMP_CLIENT_CB_TYPE_WRITE_RESULT:
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: write result = %d.",
                                    1,pSimpCallbackData->cb_content.write_result);
                break;
            case SIMP_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
                switch(pSimpCallbackData->cb_content.notif_ind_data.receive_type)
                {
                    case SIMP_NOTIF_RECEIVE_V3_NOTIFY:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: v3_notify received.",0);
                        break;
                    case SIMP_IND_RECEIVE_V4_INDICATE:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: v4_indicate received.",0);
                        break;
                    case SIMP_IND_RECEIVE_V5_CTL_PNT:
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Simple BLE Client CB: v5_control_point received.",0);
                        break;
                    case SIMP_NOTIF_IND_RECEIVE_FAIL:
                        DBG_BUFFER(MODULE_APP, LEVEL_WARN, "Simple BLE Client CB: notif/ind receive failed.",0);
                        break;
                    default:
                        break;
                }
                break;
                
            default:
                break;
        }
    }

    return appResult;
}


