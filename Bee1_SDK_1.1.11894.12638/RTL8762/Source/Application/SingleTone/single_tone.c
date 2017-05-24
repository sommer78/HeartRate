enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      single_tone.c
* @brief     single tone test implementation.
* @details   none.
* @author    Lory
* @date      2015-07-28
* @version   v1.0
* *********************************************************************************************************
*/

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "single_tone.h"
#include "trace.h"

#define CMD_PKT      0x01

//Set tx power 3dbm
#define Config_TX_POWER_3DBM 0

bool gHasSentStartCmd = false;


TComResult SingleTone_HandleHciApiCallBack(void *pContext,
        TComEvent  Event,
        uint32_t   EventResult,
        void       *pBuffer,
        uint32_t   Length,
        uint8_t pType)
{
    TComResult Result = comResultSuccess;
    VEND_CMD_SINGLE_TONE *pCmd_singleTone_reset = NULL;
    VEND_CMD_SINGLE_TONE *pCmd_singleTone_start = NULL;

    switch (Event)
    {
        case comeDataIndication:
            comResponse(pBuffer);
            if (!gHasSentStartCmd)
            {
#if Config_TX_POWER_3DBM
                VEND_CMD_TX_POWER *pCmd_tx_power_setting = (VEND_CMD_TX_POWER *)pvPortMalloc(sizeof(VEND_CMD_TX_POWER) , RAM_TYPE_DATA_OFF);
                if (pCmd_tx_power_setting)
                {
                    pCmd_tx_power_setting->opcode = 0xfca0;
                    pCmd_tx_power_setting->length = 1;
                    pCmd_tx_power_setting->txPower = 7;
                    comWrite(CMD_PKT, (uint8_t *)pCmd_tx_power_setting, 4);
                }
#endif

                pCmd_singleTone_start = (VEND_CMD_SINGLE_TONE *)pvPortMalloc(sizeof(VEND_CMD_SINGLE_TONE) , RAM_TYPE_DATA_OFF);
                if (pCmd_singleTone_start)
                {
                    pCmd_singleTone_start->opcode = 0xfc78;
                    pCmd_singleTone_start->length = 4;
                    pCmd_singleTone_start->start = 1;
                    pCmd_singleTone_start->channle = 20; //2442		
#if Config_TX_POWER_3DBM
                    pCmd_singleTone_start->txPower = 7;  //6: 0dbm, 7: 3dbm
#else
                    pCmd_singleTone_start->txPower = 6;  //6: 0dbm, 7: 3dbm
#endif
										pCmd_singleTone_reset->isLE = 1;     //LE device
                    comWrite(CMD_PKT, (uint8_t *)pCmd_singleTone_start, 7);
                    gHasSentStartCmd = true;
                }
            }
            break;

        case comeDataTransmitted:
            vPortFree(pBuffer, RAM_TYPE_DATA_OFF);
            break;

        case comeOpenCompleted:
            pCmd_singleTone_reset = (VEND_CMD_SINGLE_TONE *)pvPortMalloc(sizeof(VEND_CMD_SINGLE_TONE) , RAM_TYPE_DATA_OFF);
            if (pCmd_singleTone_reset)
            {
                pCmd_singleTone_reset->opcode = 0xfc78;
                pCmd_singleTone_reset->length = 4;
                pCmd_singleTone_reset->start = 0;
                pCmd_singleTone_reset->channle = 20;	 //2442
                pCmd_singleTone_reset->txPower = 6;    //0dbm
								pCmd_singleTone_reset->isLE = 1;			 //LE device
                comWrite(CMD_PKT, (uint8_t *)pCmd_singleTone_reset, 7);
            }
            break;

        case comeCloseCompleted:
            break;

        case comeNone:
            break;

        default:
            break;
    }
    return (Result);
}


void SingleTone_Task(void *pParameters)
{
    comClose();
    comOpen(SingleTone_HandleHciApiCallBack, NULL);

    while (1);
}

void SingleTone_Init(void)
{
    /* Create SingleTone Task. */
    xTaskCreate(SingleTone_Task,             /* Pointer to the function that implements the task. */
                "SingleTone",                /* Text name for the task.  This is to facilitate debugging only. */
                256,                      /* Stack depth in words. 1KB*/
                NULL,                     /* We are not using the task parameter. */
                1,                        /* This task will run at priority 1. */
                NULL );                     /* the task handle. */
}


