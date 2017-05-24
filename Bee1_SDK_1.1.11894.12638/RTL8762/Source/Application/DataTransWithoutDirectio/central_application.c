/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      centralFullTestApp_application.c
* @brief     Central full test application implementation
* @details   Central full test application implementation
* @author    ranhui
* @date      2015-03-27
* @version   v0.2
* *********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include "rtl876x.h"
#include "bee_message.h"
#include "trace.h"
#include "bee_message.h"
#include "central.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "application.h"
#include "central_application.h"
#include "dataTrans_application.h"
#include "board.h"
#include "module_param_config.h"
#include "at_cmd.h"
#include "profile_client.h"
#include "datatrans_client.h"
#include "profile.h"

extern TClientID   DtClientID;

gaprole_States_t GAPCentralRoleState = GAP_CENTRAL_STATE_INIT;

void central_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg);




void central_HandleBtGapStateChangeEvt(uint8_t newState)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Enter central_HandleBtGapStateChangeEvt", 0);
#endif

    switch ( newState )
    {
        case GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN:
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "GAP_CENTRAL_STATE_IDLE_NO_SCAN_NO_CONN", 0);
#endif

                if (g_AppCB->gapProfileState == GAP_CENTRAL_STATE_CONNECTED)            /*disconnected*/
                {
                    AtCmdSendResponseDisConnED();
                }
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
                isConnecting = FALSE;
                xTimerStop(TimersConnTimeOut, 0);
                g_AppCB->gapProfileState = (gaprole_States_t)newState;
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "GAP_CENTRAL_STATE_CONNECTED, g_AppCB->gapProfileState = %d", 1, g_AppCB->gapProfileState);
#endif
                gProfileData.wMTUSize = 244;
                AtCmdSendResponseConnEd(connindex);
                DtClient_StartDiscovery();
            }
            break;

        default:
            {

            }
            break;

    }

    g_AppCB->gapProfileState = (gaprole_States_t)newState;
    GAPCentralRoleState = (gaprole_States_t)newState;
}






/**
* @brief
*
*
* @param   pBtStackMsg
* @return  void
*/
void central_HandleBtGapMessage(BEE_IO_MSG  *pBeeIoMsg)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "central_HandleBtGapMessage subType = %d", 1, pBeeIoMsg->subType);
#endif
    BT_STACK_MSG BtStackMsg;
    memcpy(&BtStackMsg, &pBeeIoMsg->parm, sizeof(pBeeIoMsg->parm));

    switch (pBeeIoMsg->subType)
    {
        case BT_MSG_TYPE_CONN_STATE_CHANGE:
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_CONN_STATE_CHANGE:(%d->%d)",
                           2, GAPCentralRoleState, BtStackMsg.msgData.gapConnStateChange.newState);
#endif

                central_HandleBtGapStateChangeEvt(BtStackMsg.msgData.gapConnStateChange.newState);
            }
            break;


        case BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE:
            {

            }
            break;

        case BT_MSG_TYPE_PARAM_SET_RESULT:
            {
                if (BtStackMsg.msgData.gapBTParamSetResult.result == TRUE)
                {
                    if (BtStackMsg.msgData.gapBTParamSetResult.bleParamType == BLE_PARAM_TYPE_SET_CHANNEL_CLASS)
                    {
#ifndef CODE_SIZE_32K_DEBUG
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Central BLE_PARAM_TYPE_SET_CHANNEL_CLASS: success", 0);
#endif
                    }
                }
            }
            break;

        default:
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "central_HandleBtGapMessage unknown subtype", 1, pBeeIoMsg->subType);
#endif

            break;

    }

}

/* This callback will be called when advertising or scan response data received. */
uint8_t ScanDestAddr[10][B_ADDR_LEN] = {0};
uint8_t ScanAddrNum = 0;
TAppResult App_CentralRoleCallback(TCentralAppCB_MsgType msgType, TCentralAppCB_MsgData centralCbData)
{
    TAppResult appResult = AppResult_Success;
    char scanRsp[40];
    char remote_name[10][31] = {0};

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "App_CentralRoleCallback: msgType = %d", 1, msgType);
#endif
    switch (msgType)
    {
        case ADV_SCAN_RSP_DATA_MSGTYPE:
            {

                if ((centralCbData.pAdvScanRspData->data[4] == 0x07) && (centralCbData.pAdvScanRspData->data[5] == 0x12) && (centralCbData.pAdvScanRspData->data[6] == 0xA2))
                {
                    if (ScanAddrNum <= 9)
                    {
                        memcpy(remote_name[ScanAddrNum], centralCbData.pAdvScanRspData->data + 23,  centralCbData.pAdvScanRspData->data[21] - 1) ;

                        sprintf(scanRsp, "%d:%s 0x%02X%02X%02X%02X%02X%02X\r\n", ScanAddrNum, remote_name[ScanAddrNum],
                                centralCbData.pAdvScanRspData->remote_BD[5],
                                centralCbData.pAdvScanRspData->remote_BD[4],
                                centralCbData.pAdvScanRspData->remote_BD[3],
                                centralCbData.pAdvScanRspData->remote_BD[2],
                                centralCbData.pAdvScanRspData->remote_BD[1],
                                centralCbData.pAdvScanRspData->remote_BD[0]);
                        AtCmdSendResponse(scanRsp, strlen(scanRsp));


                        for (uint8_t k = 0; k < B_ADDR_LEN; k++)
                        {
                            ScanDestAddr[ScanAddrNum][k] = centralCbData.pAdvScanRspData->remote_BD[k];
                        }
                        ScanAddrNum++;
                    }
                }

            }
            break;
        default:
            break;
    }
    return appResult;
}

TAppResult App_ClientCallback(TClientID clientID, void *pData)
{
    TAppResult  appResult = AppResult_Success;


    if (clientID == DtClientID)
    {
        TDtClientCB_Data *pDtCallbackData = (TDtClientCB_Data *)pData;
        TDtHandleType handle_type;
        uint16_t handle_value;
        TDtClientNotifIndReceiveType dt_receive_type;
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DT Client CB Type: %d.", 1, pDtCallbackData->cb_type);
#endif
        switch (pDtCallbackData->cb_type)
        {
            case DT_CLIENT_CB_TYPE_DISC_STATE:
                switch (pDtCallbackData->cb_content.disc_state)
                {
                    case DT_DISC_DONE:
                        /* Discovery RSC service procedure successfully done. */
#ifndef CODE_SIZE_32K_DEBUG
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DT Client CB: discover procedure done.", 0);
#endif
                        DtClient_V1NotifyCmd(1);

                        if (gConfigParam->isChanMapSet == TRUE)
                        {
                            blueAPI_LESetHostChanClass(gConfigParam->chanMap);
#ifndef CODE_SIZE_32K_DEBUG
                            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "excute channel map set!!", 0);
#endif
                            gConfigParam->isChanMapSet = FALSE;
                        }


                        break;
                    case DT_DISC_PENDING:
                        /* Discovery Request failed. */
#ifndef CODE_SIZE_32K_DEBUG
                        DBG_BUFFER(MODULE_APP, LEVEL_WARN, "DT Client CB: discover request failed.", 0);
#endif
                        break;
                    default:
                        break;
                }
                break;
            case DT_CLIENT_CB_TYPE_DISC_HDL:
                /* Tiis branch executed once handle is found. */
                handle_type = pDtCallbackData->cb_content.disc_hdl_info.handle_type;
                handle_value = pDtCallbackData->cb_content.disc_hdl_info.handle_value;
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DT Client CB: discover handle, idx--value %d--0x%4.4x.", 2, handle_type, handle_value);
#endif
                /* Here means app handle changed, we must set them to rsc client module. */
                DtClient_SetHandle(handle_type, handle_value);
                break;
            case DT_CLIENT_CB_TYPE_WRITE_RESULT:
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DT Client CB: write result = %d.",
                           1, pDtCallbackData->cb_content.write_result);
#endif
                break;
            case DT_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
                dt_receive_type = pDtCallbackData->cb_content.notif_ind_data.receive_type;
                switch (dt_receive_type)
                {
                    case DT_NOTIF_RECEIVE_V1_WRITE_NOTIFY:
#ifndef CODE_SIZE_32K_DEBUG
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DT Client CB: notify received.", 0);
#endif
                        break;
                    case DT_NOTIF_IND_RECEIVE_FAIL:
#ifndef CODE_SIZE_32K_DEBUG
                        DBG_BUFFER(MODULE_APP, LEVEL_WARN, "DT Client CB: notif/ind receive failed.", 0);
#endif
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

