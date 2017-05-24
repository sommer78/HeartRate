/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dataTrans_application.h
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/


#ifndef _DATA_TRANSMIT_APPLICATION_H_
#define  _DATA_TRANSMIT_APPLICATION_H_
#include "rtl_types.h"
#include "blueapi_types.h"
#include "app_queue.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "at_cmd.h"
#include "timers.h"
#include "bee_message.h"
#include "peripheral.h"
#include "profileApi.h"

#define UART_RX_BUFFER_LENGTH       3500
#define UART_RX_BUFFER_OVERFLOW_LENGTH   1000
#define UART_TX_BUFFER_LENGTH       200
#define TX_PACKET_MAX_LENGTH 250
#define TX_PACKET_COUNT      25
#define TX_APP_PACKET_MAX_LENGTH  245
#define TX_APP_PACKET_COUNT      10
#define DATA_TRANSMIT_DATA_PACKET_MAX_LENGTH 155

typedef struct _TXData
{
    struct _TXData *pNext;
    uint8_t   tx_buffer[TX_PACKET_MAX_LENGTH];
    uint16_t    length;
} TTxData, *PTxData;

typedef struct _TxAppData
{
    struct _TxAppData *pNext;
    uint8_t   send_buffer[TX_APP_PACKET_MAX_LENGTH];
    uint16_t  length;
} TTxAppData, *PTxAppData;

/** @brief uart control struct */
typedef struct
{
    xTaskHandle   txAssistHandle;
    xQueueHandle  QueueHandleTxData;   /* Tx queue */

    char          atCmdBuffer[AT_CMD_MAX_LENGTH];
    bool          isReceivingAtCmd;
    uint8_t       atCmdLength;

    //TTxAppData     txAppDataBuffer[TX_APP_PACKET_COUNT];
    QUEUE_T       txAppDataQueueFree;
    QUEUE_T       txAppDataQueue;

    uint8_t         rxBuffer[UART_RX_BUFFER_LENGTH];
    uint16_t        rxBufferWriteOffset;
    uint16_t        rxBufferWriteOffsetOld;
    uint16_t        rxBufferReadOffset;
    uint16_t        rxBufferDataLength;
    uint16_t        rxBufferCmdOffset;

    //TTxData       txUartDataBuffer[TX_PACKET_COUNT];
    QUEUE_T       txUartDataQueueFree;

    gaprole_States_t gapProfileState;
    bool          tx_service_cccd_enable;
    uint8_t      wDsCredits;


} TAPP_TCB, *P_APP_TCB;


typedef struct
{
    TTxAppData     txAppDataBuffer[TX_APP_PACKET_COUNT];
    //uint8_t         rxBuffer[UART_RX_BUFFER_LENGTH];
    TTxData       txUartDataBuffer[TX_PACKET_COUNT];
} TAPP_BUF_TCB, *P_APP_BUF_TCB;








extern P_APP_TCB g_AppCB;
extern TAPP_TCB  g_TappCB;
extern P_APP_BUF_TCB g_AppBufCB;
extern TAPP_BUF_TCB g_TappbufCB;


extern TimerHandle_t TimersUartDataExtra;
extern TimerHandle_t TimersUartDataRx;
extern TimerHandle_t TimersUartBaudrateChange;
extern TimerHandle_t TimersUartStopBitChange;
extern TimerHandle_t TimersUartPariBitChange;
extern TimerHandle_t TimersUartFcEnChange;
extern TimerHandle_t TimersReset;
extern TimerHandle_t TimersDefault;
extern TimerHandle_t TimersWakeUp;
extern TimerHandle_t TimersEnterLowPower;
extern TimerHandle_t TimersConnTimeOut;
extern TimerHandle_t TimersEnterDLPSAfterConnected;

extern BOOL allowedDataTransEnterDlps;


void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
TAppResult AppHandleGATTCallback(uint8_t serviceID, void *pData);
void ApplicationInit( void );
void dataTrans_handleUartRX(bool isRxTimeout);
void AppSendMsgFromGATTCallback(BEE_IO_MSG *pBeeMsgBlk);
void TxUartQueueInit(void);
void TxAppQueueInit(void);

#endif
