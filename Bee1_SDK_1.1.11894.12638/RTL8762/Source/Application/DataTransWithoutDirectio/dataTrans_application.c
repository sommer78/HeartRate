enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dataTrans_application.c
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include "blueapi.h"
#include "dlps_platform.h"
#include <string.h>

#include "profile.h"
#include "profileAPI.h"

#include "app_queue.h"
#include "timers.h"
#include "bee_message.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"

#include "module_param_config.h"
#include "dataTrans_uart.h"
#include "dataTrans_application.h"
#include "dataTrans_uart.h"
#include "dataTrans_profile.h"
#include "dev_info_profile.h"


#include "rtl876x_uart.h"

#include "central.h"
#include "central_application.h"
/****************************************************************************/
/* Test Profile task macros                                                       */
/****************************************************************************/



gaprole_States_t gapProfileState = GAPSTATE_INIT;


#define TX_HANDLE_PRIORITY          (tskIDLE_PRIORITY + 1)   /* Task priorities. */
#define TX_HANDLE_STACK_SIZE            256
#define MAX_NUMBER_OF_TX_MESSAGE        TX_PACKET_COUNT

P_APP_TCB g_AppCB ;
TAPP_TCB  g_TappCB ;
P_APP_BUF_TCB g_AppBufCB SRAM_OFF_BD_BSS_SECTION;
TAPP_BUF_TCB g_TappbufCB SRAM_OFF_BD_BSS_SECTION;



TimerHandle_t TimersUartDataExtra;
TimerHandle_t TimersUartDataRx;
TimerHandle_t TimersUartBaudrateChange;
TimerHandle_t TimersUartStopBitChange;
TimerHandle_t TimersUartPariBitChange;
TimerHandle_t TimersUartFcEnChange;
TimerHandle_t TimersReset;
TimerHandle_t TimersDefault;
TimerHandle_t TimersWakeUp;
TimerHandle_t TimersEnterLowPower;
TimerHandle_t TimersConnTimeOut;
TimerHandle_t TimersEnterDLPSAfterConnected;


BOOL allowedDataTransEnterDlps = FALSE;



bool dataTrans_getSendBufferFromRxBuffer(uint16_t len);
bool dataTrans_getCmdBufferFromRxBuffer(uint16_t len);



void TimerResetCallback(TimerHandle_t pxTimer )
{
    moduleParam_SetSystemReset();
}




void TimersUartConfigChangeCallback(TimerHandle_t pxTimer )
{
    UARTChange();
}



void TimerDefaultCallback(TimerHandle_t pxTimer )
{

    /*  name    */
    memcpy(gConfigParam->localName.Local_name, defaultName , strlen(defaultName) + 1);
    fs_save_Local_name_struct(&gConfigParam->localName);
    peripheralSetGapParameter(GAPPRRA_DEVICE_NAME, strlen((char *)gConfigParam->localName.Local_name) + 1, gConfigParam->localName.Local_name);

    /*  PIN */
    gConfigParam->pin.pin = defaultPin;
    uint32_t passkey = gConfigParam->pin.pin;
    fs_save_pin_struct(&gConfigParam->pin);
    GAPBondMgr_SetParameter( GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey);

    /*  baudrate, stopbit and paribit   */
    gConfigParam->baudrate.baudrate = defaultbaudrate;
    fs_save_baudrate_struct(&gConfigParam->baudrate);

    gConfigParam->stopbit.stopbit = defaultstopbit;
    fs_save_stopbit_struct(&gConfigParam->stopbit);

    gConfigParam->paribit.paribit = defaultparibit;
    fs_save_paribit_struct(&gConfigParam->paribit);

    /*  is_enter_sleep  */
    gConfigParam->is_enter_sleep.is_enter_sleep = default_is_enter_sleep;
    fs_save_is_enter_sleep_struct(&gConfigParam->is_enter_sleep);

    /* gap role */
    gConfigParam-> gaprole.gaprole = defaultgaprole;
    fs_save_gaprole_struct(&gConfigParam->gaprole);

    /* flow control */
    gConfigParam-> fc_enable.fc_enable = defaultfc;
    fs_save_fc_struct(&gConfigParam->fc_enable);



    UARTChange();

}



void TimerWakeUpCallback(TimerHandle_t pxTimer )
{
    if (g_AppCB->gapProfileState == GAPSTATE_CONNECTED)
    {
        if (gConfigParam->isTimerDLPSActive == FALSE)
        {
            xTimerReset(TimersEnterDLPSAfterConnected, 0);
            //#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Already WAKE UP from DLPS,  Reset the TimersEnterDLPSAfterConnected again !!", 0);
            //#endif
            gConfigParam->isTimerDLPSActive = TRUE;
        }
    }
    else
    {
        AtCmdSendResponseWake();
    }
}


void TimerEnterLowPowerCallback(TimerHandle_t pxTimer )
{
    allowedDataTransEnterDlps = TRUE;
}



void TimerConnTimeOutCallback(TimerHandle_t pxTimer )
{
    isConnecting = FALSE;
    AtCmdSendResponse(AtCmdResponseCONNTIMEOUT, strlen(AtCmdResponseCONNTIMEOUT));
}




void TimersEnterDLPSAfterConnectedCallback(TimerHandle_t pxTimer)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Time is Out , Enter TimersEnterDLPSAfterConnectedCallback, DLPS IS Enabled !!", 0);
#endif

    gConfigParam->isTimerDLPSActive = FALSE;
    /**1. update the connection interval to:  Connection interval * (1+ Slave latency)*1.25ms> 80ms**/
    moduleParam_SetConnInterval(200);  //update connection interval to 200ms
    /**2. enable DLPS**/
    allowedDataTransEnterDlps = TRUE;
    gConfigParam->isSleeping = TRUE;
}






void dataTrans_handleUartRX(bool isRxTimeout)
{

    if (g_AppCB->rxBufferDataLength == 0)
    {
        return;
    }

    if (g_AppCB->gapProfileState == GAPSTATE_CONNECTED)
    {

        uint16_t len = 0;


        if (isRxTimeout)
        {
            for (; g_AppCB->rxBufferDataLength != 0;)
            {
                if (g_AppCB->rxBufferDataLength >= gProfileData.wMTUSize - 3)
                    len = gProfileData.wMTUSize - 3;
                else
                    len = g_AppCB->rxBufferDataLength;

                if (dataTrans_getSendBufferFromRxBuffer(len))
                {

                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            for (; g_AppCB->rxBufferDataLength >= gProfileData.wMTUSize - 3;)
            {
                len = gProfileData.wMTUSize - 3;

                if (dataTrans_getSendBufferFromRxBuffer(len))
                {

                }
                else
                {
                    return;
                }
            }
        }


        Profile_DataTransSendData();
    }
    else
    {

        taskENTER_CRITICAL();
        if (!g_AppCB->isReceivingAtCmd)
        {
            g_AppCB->rxBufferDataLength = 0;
            g_AppCB->rxBufferReadOffset = g_AppCB->rxBufferWriteOffsetOld;
        }
        taskEXIT_CRITICAL();
        if (g_AppCB->isReceivingAtCmd)
        {


            uint16_t len = g_AppCB->rxBufferDataLength;
            if (len > AT_CMD_MAX_LENGTH)
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "dataTrans_handleUartRX: AT cmd too long (%d)", 1, len);
#endif
                g_AppCB->isReceivingAtCmd = false;
                g_AppCB->rxBufferDataLength = 0;
                g_AppCB->rxBufferReadOffset = g_AppCB->rxBufferWriteOffsetOld;
            }
            else
            {
                if (dataTrans_getCmdBufferFromRxBuffer(len))
                {
                    g_AppCB->isReceivingAtCmd = false;
                    AtCmdParse();
                }
            }


        }
        else
        {
            return;
        }
    }
}




bool dataTrans_getSendBufferFromRxBuffer(uint16_t len)
{
    PTxAppData pTxData = NULL;

    if ((len == 0) || (len > g_AppCB->rxBufferDataLength))
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "dataTrans_getSendBufferFromRxBuffer: length is invalid", 0);
#endif
        return FALSE;
    }


    pTxData = AppQueueOut(&g_AppCB->txAppDataQueueFree);
    if (pTxData != NULL)
    {
        pTxData->length = len;
        if (g_AppCB->rxBufferReadOffset + len <= UART_RX_BUFFER_LENGTH)
        {
            memcpy(pTxData->send_buffer, g_AppCB->rxBuffer + g_AppCB->rxBufferReadOffset, len);
            g_AppCB->rxBufferReadOffset += len;
        }
        else
        {
            uint16_t len1 = UART_RX_BUFFER_LENGTH - g_AppCB->rxBufferReadOffset;
            memcpy(pTxData->send_buffer, g_AppCB->rxBuffer + g_AppCB->rxBufferReadOffset, len1);
            g_AppCB->rxBufferReadOffset = 0;
            memcpy(pTxData->send_buffer + len1, g_AppCB->rxBuffer + g_AppCB->rxBufferReadOffset, len - len1);
            g_AppCB->rxBufferReadOffset += len - len1;
        }
        taskENTER_CRITICAL();
        g_AppCB->rxBufferDataLength -= len;
        taskEXIT_CRITICAL();
        AppQueueIn(&g_AppCB->txAppDataQueue, pTxData);
        return true;
    }
    else
    {

        Profile_DataTransSendData();
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_TRACE, "dataTrans_getSendBufferFromRxBuffer: txAppDataQueueFree is empty", 0);
#endif
        return false;
    }


}




bool dataTrans_getCmdBufferFromRxBuffer(uint16_t len)
{
    uint16_t  rxBufferReadOffset_temp;

    if ((len == 0) || (len > g_AppCB->rxBufferDataLength))
    {
        return FALSE;
    }

    rxBufferReadOffset_temp = g_AppCB->rxBufferReadOffset;


    g_AppCB->atCmdLength = len;
    if (g_AppCB->rxBufferReadOffset + len <= UART_RX_BUFFER_LENGTH)
    {
        memcpy(g_AppCB->atCmdBuffer, g_AppCB->rxBuffer + g_AppCB->rxBufferReadOffset, len);
        g_AppCB->rxBufferReadOffset += len;
    }
    else
    {
        uint16_t len1 = UART_RX_BUFFER_LENGTH - g_AppCB->rxBufferReadOffset;
        memcpy(g_AppCB->atCmdBuffer, g_AppCB->rxBuffer + g_AppCB->rxBufferReadOffset, len1);
        g_AppCB->rxBufferReadOffset = 0;
        memcpy(g_AppCB->atCmdBuffer + len1, g_AppCB->rxBuffer + g_AppCB->rxBufferReadOffset, len - len1);
        g_AppCB->rxBufferReadOffset += len - len1;
    }


    if ((len % UartRxTriggerLevel == 0 ) && ((g_AppCB->atCmdBuffer[len - 1] != '\n') || (g_AppCB->atCmdBuffer[len - 2] != '\r')))
    {
        g_AppCB->rxBufferReadOffset = rxBufferReadOffset_temp;
        return FALSE;
    }


    taskENTER_CRITICAL();
    g_AppCB->rxBufferDataLength -= len;
    taskEXIT_CRITICAL();
    return true;
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
                g_AppCB->tx_service_cccd_enable = FALSE;
                if (gapProfileState == GAPSTATE_CONNECTED)
                {
                    uint8_t disc_reason;
                    peripheralGetGapParameter(GAPPRRA_DISCONNECTED_REASON, &disc_reason);
#ifndef CODE_SIZE_32K_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "peripheral_HandleBtGapStateChangeEvt: disc_reason = %d", 1, disc_reason);
#endif
                    gConfigParam->isConnectionFirstEstablished = FALSE;
                    peripheral_StartAdvertising();
                }
                else if (gapProfileState == GAPSTATE_ADVERTISING)
                {
                    peripheral_StartAdvertising();
                }
				g_AppCB->gapProfileState = (gaprole_States_t)newState;
            }
            break;
        //device is advertising
        case GAPSTATE_ADVERTISING:
            {
				g_AppCB->gapProfileState = (gaprole_States_t)newState;
            }
            break;

        //device is connected
        case GAPSTATE_CONNECTED:
            {
                g_AppCB->gapProfileState = (gaprole_States_t)newState;
                peripheralGetGapParameter(GAPPRRA_MAXTPDUDSCREDITS, &g_AppCB->wDsCredits);
                gConfigParam->isConnectionFirstEstablished = TRUE;
                moduleParam_SetConnInterval(20);

                if (gConfigParam->isTimerDLPSActive == FALSE)
                {
#ifndef CODE_SIZE_32K_DEBUG
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Reset the TimersEnterDLPSAfterConnected when connection is first established !!!", 0);
#endif
                    xTimerReset(TimersEnterDLPSAfterConnected, 0);
                    gConfigParam->isTimerDLPSActive = TRUE;
                }
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



void peripheral_HandleBtGapBondStateChangeEvt(uint8_t newState, uint8_t status)
{
    switch (newState)
    {
        case GAPBOND_PAIRING_STATE_STARTED:
            {
                //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_STARTED)", 0);
            }
            break;

        case GAPBOND_PAIRING_STATE_COMPLETE:
            {
                //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_COMPLETE)", 0);
                /* mark being paired & able to send action*/

            }
            break;

        case GAPBOND_PAIRING_STATE_BONDED:
            {
                //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(GAPBOND_PAIRING_STATE_BONDED)", 0);
            }
            break;

        default:
            {
                //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_STATE_CHANGE:(unknown newstate: %d)", 1, newState);
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
                uint16_t conn_interval = 0;

                if (gConfigParam->is_conn_update)
                {
                    AtCmdSendResponseOK();
                    gConfigParam->is_conn_update = FALSE;
                }
                else
                {
                }
                peripheralGetGapParameter(GAPPRRA_CONN_INTERVAL, &conn_interval);
//#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "BT_MSG_TYPE_CONN_PARA_UPDATE_CHANGE %d", 1, conn_interval);
//#endif

                if ((g_AppCB->gapProfileState == GAPSTATE_CONNECTED) && (gConfigParam->isTimerDLPSActive == TRUE))
                {
                    if (gConfigParam->isConnectionFirstEstablished == TRUE)
                    {
                        gConfigParam->isConnectionFirstEstablished = FALSE;
                    }
                    else
                    {
                        AtCmdSendResponseWake();
                    }
                }

                gConfigParam->conn_interval = conn_interval * 5 / 4;
            }
            break;
        case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
            {
                if (gConfigParam->is_conn_update)
                {

                    gConfigParam->is_conn_update = FALSE;
                }
                else
                {
                }
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




/**
* @brief
*
*
* @param   pBtStackMsg
* @return  void
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
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_DISPLAY: %d", 1, displayValue);
#endif
            }
            break;

        case BT_MSG_TYPE_BOND_PASSKEY_INPUT:
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_PASSKEY_INPUT", 0);
#endif
                GAPBondMgr_InputPassKey();
            }
            break;

        case BT_MSG_TYPE_BOND_OOB_INPUT:
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BT_MSG_TYPE_BOND_OOB_INPUT", 0);
#endif
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

            }
            break;
#endif

        default:
            DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "peripheral_HandleBtGapMessage unknown subtype", 1, pBeeIoMsg->subType);

            break;
    }
}



extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hIoQueueHandle;


void AppSendMsgFromGATTCallback(BEE_IO_MSG *pBeeMsgBlk)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;
    TickType_t  xTicksToWait = 0;

    SendQueueResult = xQueueSend(hIoQueueHandle, pBeeMsgBlk,  xTicksToWait);
    if (SendQueueResult != pdPASS)
    {
//#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "AppSendMsgFromGATTCallback fail 1", 1, SendQueueResult);
//#endif

    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event,  xTicksToWait);
    if (SendQueueResult != pdPASS)
    {
//#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "AppSendMsgFromGATTCallback fail 2", 1, SendQueueResult);
//#endif

    }
}



void TimerUartDataRxCallback(TimerHandle_t pxTimer )
{
    dataTrans_handleUartRX(true);
}





uint8_t  rx_overflow_handle_flag = 0;
TAppResult AppHandleGATTCallback(uint8_t serviceID, void *pData)
{
    TAppResult appResult = AppResult_Success;

    if (serviceID == ProfileAPI_ServiceUndefined)
    {
        TEventInfoCBs_t *pPara = (TEventInfoCBs_t *)pData;
        switch (pPara->eventId)
        {
            case PROFILE_EVT_SRV_REG_COMPLETE:  /*  srv register result event   */
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "profile callback PROFILE_EVT_SRV_REG_COMPLETE\n", 0);
#endif
                {
                    //DeviceInformationUpdate();
                    uint8_t     LocalBd[BLUE_API_BD_SIZE];
                    char SystemID[8]        = {0, 1, 2, 0, 0, 3, 4, 5};
                    peripheralGetGapParameter(GAPPARA_BD_ADDR, &LocalBd);
                    SystemID[0] = LocalBd[5];
                    SystemID[1] = LocalBd[4];
                    SystemID[2] = LocalBd[3];
                    SystemID[5] = LocalBd[2];
                    SystemID[6] = LocalBd[1];
                    SystemID[7] = LocalBd[0];
                    DIS_SetParameter(DIS_PARAM_SYSTEM_ID, 8, SystemID);
                    peripheral_Init_StartAdvertising();
                }
                break;
            case PROFILE_EVT_SEND_DATA_COMPLETE:
                g_AppCB->wDsCredits = pPara->sParas[0];
                Profile_DataTransSendData();
                if ((g_AppCB->txAppDataQueueFree.ElementCount >= 1) && ( gConfigParam->fc_enable.fc_enable ) && (rx_overflow_handle_flag == 1))
                {
                    BEE_IO_MSG app_msg;
                    app_msg.IoType = IO_UART_MSG_TYPE;
                    app_msg.subType = MSG_UART_RX_EMPTY;
                    AppSendMsgFromGATTCallback(&app_msg);
                }
                xTimerReset(TimersUartDataExtra, 0);

                break;
        }
    }

    return appResult;
}

/****************************************************************************/
/* Message handle                                                           */
/****************************************************************************/


void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv)
{
    uint16_t msgtype = io_driver_msg_recv.IoType;

    switch (msgtype)
    {
        case IO_UART_MSG_TYPE:
            if (gConfigParam->isSleeping == TRUE)
            {
                allowedDataTransEnterDlps = FALSE;
                gConfigParam->isSleeping = FALSE;
                g_AppBufCB = &g_TappbufCB;
                memset(g_AppBufCB, 0, sizeof(TAPP_BUF_TCB));
                TxUartQueueInit();
                TxAppQueueInit();
                xTimerStart(TimersWakeUp, 0);
                if (g_AppCB->gapProfileState == GAPSTATE_CONNECTED)
                {
                    moduleParam_SetConnInterval(20);
                }
            }

            if (gConfigParam->isTimerDLPSActive == TRUE)
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Data to NOTIFY before time out, Stop TimersEnterDLPSAfterConnected and Reset con-interval to 18.75ms", 0);
#endif
                xTimerStop(TimersEnterDLPSAfterConnected, 0);
                gConfigParam->isTimerDLPSActive = FALSE;
                gConfigParam-> isDataAvailable = TRUE;
            }


            if (io_driver_msg_recv.subType == MSG_UART_RX)
            {
                xTimerStop(TimersUartDataExtra, 0);
                xTimerReset(TimersUartDataRx, 0);
                dataTrans_handleUartRX(false);

                break;
            }
            else if (io_driver_msg_recv.subType == MSG_UART_RX_TIMEOUT)
            {
                xTimerStop(TimersUartDataExtra, 0);
                xTimerStop(TimersUartDataRx, 0);
                dataTrans_handleUartRX(true);
                break;
            }
            else if ((io_driver_msg_recv.subType == MSG_UART_RX_OVERFLOW) && (gConfigParam->fc_enable.fc_enable))
            {
                rx_overflow_handle_flag = 1;                                                                         /* This flag is set every time overflow happen */
                UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, DISABLE);    /* prevent interrupt is triggered continuously by the data remain in FIFO   */
                break;
            }
            else if ((io_driver_msg_recv.subType == MSG_UART_RX_EMPTY) && (gConfigParam->fc_enable.fc_enable))
            {
                if (rx_overflow_handle_flag == 1)
                {
                    dataTrans_handleUartRX(false);

                    /*  copy data from FIIFO to buffer*/
                    CopyFromFIFO();

                    /* update rx data length */
                    RxDataStatusUpdate();

                    /* handle the data in g_AppCB->rxBuffer */
                    dataTrans_handleUartRX(false);

                    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);

                    /* reset the flag */
                    rx_overflow_handle_flag = 0;
                }

                break;

            }

        case BT_STATUS_UPDATE:
            {

                if (gConfigParam->gaprole.gaprole == CENTRAL_ROLE)
                {
#ifndef CODE_SIZE_32K_DEBUG
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "BT_STATUS_UPDATE: gConfigParam->gaprole.gaprole = Central", 0);
#endif
                    central_HandleBtGapMessage(&io_driver_msg_recv);
                }
                else if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
                {
#ifndef CODE_SIZE_32K_DEBUG
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "BT_STATUS_UPDATE: gConfigParam->gaprole.gaprole = Peripheral", 0);
#endif
                    peripheral_HandleBtGapMessage(&io_driver_msg_recv);
                }
            }
            break;
        default:
            break;
    }

}


/****************************************************************************/
/*                                  application Init                                                  */
/****************************************************************************/
static void TimerInit(void)
{


    uint16_t time = 2;
    if (gConfigParam->baudrate.baudrate == 4800)
    {
        time = 4;
    }
    else if (gConfigParam->baudrate.baudrate == 9600)
    {
        time = 3;
    }
    else
    {
        time = 2;
    }

    TimersUartDataExtra = xTimerCreate("TimersUartDataExtra", time, pdFALSE, ( void *) 2, TimerUartDataRxCallback);

    TimersUartDataRx = xTimerCreate("TimersUartDataRx", time, pdFALSE, ( void *) 3, TimerUartDataRxCallback);



    TimersUartBaudrateChange = xTimerCreate("TimersUartBaudrateChange", 50, pdFALSE, ( void *) 4, TimersUartConfigChangeCallback);

    if ( TimersUartBaudrateChange == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersUartBaudrateChange init failed", 0);
#endif
    }

    TimersUartStopBitChange = xTimerCreate("TimersUartStopBitChange", 50, pdFALSE, ( void *) 5, TimersUartConfigChangeCallback);

    if ( TimersUartStopBitChange == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersUartStopBitChange init failed", 0);
#endif
    }

    TimersUartPariBitChange = xTimerCreate("TimersUartPariBitChange", 50, pdFALSE, ( void *) 6, TimersUartConfigChangeCallback);

    if ( TimersUartPariBitChange == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersUartPariBitChange init failed", 0);
#endif
    }


    TimersUartFcEnChange = xTimerCreate("TimersUartFcEnChange", 50, pdFALSE, ( void *) 7, TimersUartConfigChangeCallback);

    if ( TimersUartFcEnChange == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersUartFcEnChange init failed", 0);
#endif
    }


    TimersReset = xTimerCreate("TimersATReset", 50, pdFALSE, ( void *) 8, TimerResetCallback);

    if ( TimersReset == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersReset init failed", 0);
#endif
    }


    TimersDefault = xTimerCreate("TimersATDefault", 50, pdFALSE, ( void *) 9, TimerDefaultCallback);

    if ( TimersDefault == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersDefault init failed", 0);
#endif
    }

    TimersWakeUp =  xTimerCreate("TimersWakeup", 50, pdFALSE, ( void *) 10, TimerWakeUpCallback);
    if ( TimersWakeUp == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersWakeUp init failed", 0);
#endif
    }

    TimersEnterLowPower =  xTimerCreate("TimersEnterLowPower", 50, pdFALSE, ( void *) 11, TimerEnterLowPowerCallback);
    if ( TimersEnterLowPower == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersEnterLowPower init failed", 0);
#endif
    }

    TimersConnTimeOut = xTimerCreate("TimersConnTimeOut", 1000, pdFALSE, ( void *) 12, TimerConnTimeOutCallback);
    if ( TimersConnTimeOut == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimerInit TimersConnTimeOut init failed", 0);
#endif
    }

    TimersEnterDLPSAfterConnected = xTimerCreate("TimersEnterDLPSAfterConnected", 3000, pdFALSE, ( void *) 13, TimersEnterDLPSAfterConnectedCallback);
    if ( TimersEnterDLPSAfterConnected == NULL )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "TimersEnterDLPSAfterConnected init failed", 0);
#endif
    }

}




SemaphoreHandle_t xSemaphore = NULL;
static void SemaphoreInit(void)
{
    vSemaphoreCreateBinary(xSemaphore);
    if ( xSemaphore != NULL )
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
    }
}






void TxUartQueueInit(void)
{
    uint8_t i = 0;
    uint8_t tx_queue_size = TX_PACKET_COUNT;
    PTxData pTxData = g_AppBufCB->txUartDataBuffer;
    g_AppCB->txUartDataQueueFree.ElementCount = 0;
    g_AppCB->txUartDataQueueFree.First = NULL;
    g_AppCB->txUartDataQueueFree.Last = NULL;

    for ( i = 0; i < tx_queue_size; i++ )
    {
        AppQueueIn(&g_AppCB->txUartDataQueueFree, pTxData);
        pTxData++;
    }
}

void TxAppQueueInit(void)
{
    uint8_t i = 0;
    uint8_t send_queue_size = TX_APP_PACKET_COUNT;
    PTxAppData pTxData = g_AppBufCB->txAppDataBuffer;
    g_AppCB->txAppDataQueueFree.ElementCount = 0;
    g_AppCB->txAppDataQueueFree.First = NULL;
    g_AppCB->txAppDataQueueFree.Last = NULL;
    g_AppCB->txAppDataQueue.ElementCount = 0;
    g_AppCB->txAppDataQueue.First = NULL;
    g_AppCB->txAppDataQueue.Last = NULL;
    for ( i = 0; i < send_queue_size; i++ )
    {
        AppQueueIn(&g_AppCB->txAppDataQueueFree, pTxData);
        pTxData++;
    }
}

void ApplicationInit( void )
{

    g_AppCB->QueueHandleTxData  = xQueueCreate(MAX_NUMBER_OF_TX_MESSAGE, sizeof(PTxData));
    TxUartQueueInit();
    TxAppQueueInit();
    TimerInit();
    SemaphoreInit();
    xTaskCreate(TxHandleTask, "TxHandle",  TX_HANDLE_STACK_SIZE / sizeof(portSTACK_TYPE),  NULL, TX_HANDLE_PRIORITY, &g_AppCB->txAssistHandle);
}


