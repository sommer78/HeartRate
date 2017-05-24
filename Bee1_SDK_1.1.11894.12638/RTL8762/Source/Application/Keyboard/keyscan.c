/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyscan.c
* @brief
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "board.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_keyscan.h"
#include "bee_message.h"
#include <string.h>
#include "os.h"
#include "keyscan.h"

static KEYSCAN_DATA_STRUCT *pKey_Data = NULL;
static KEYSCAN_DATA_STRUCT  Key_Data;
static KEYSCAN_DATA_STRUCT  Key_Data_Pre;

uint32_t KeyTimeoutPeriod;
uint8_t RepeatReport = FALSE;       /* FALSE - No repeat; TRUE - Repeat. */
/* control of entering DLPS */
extern BOOL allowedKeyboardEnterDlps;

xTimerHandle hKeyscan_Timer_0 = NULL;
void KeyScan_TimerCallback_0(xTimerHandle xTimer);


void KeyPad_Init()//KEYSCAN_CONFIG_STRUCT * Keyscan_Param)
{

    memset(&Key_Data, 0, sizeof(KEYSCAN_DATA_STRUCT));
    memset(&Key_Data_Pre, 0, sizeof(KEYSCAN_DATA_STRUCT));
    pKey_Data = &Key_Data;

    KEYSCAN_InitTypeDef keyScanInitStruct;

    KeyScan_StructInit(KEYSCAN, &keyScanInitStruct);

    keyScanInitStruct.colSize         = KEYBOARD_KEYPAD_COLUMN_SIZE;
    keyScanInitStruct.rowSize         = KEYBOARD_KEYPAD_ROW_SIZE;

    keyScanInitStruct.detectPeriod    = KeyScan_Detect_Period_20ms;
    keyScanInitStruct.debounceEn      = KeyScan_Debounce_Enable;
    keyScanInitStruct.timeout         = KeyScan_Timeout_2s;
    keyScanInitStruct.scanInterval    = KeyScan_Scan_Interval_50ms;

    keyScanInitStruct.debounceTime    = (20 * 32); //16ms
    keyScanInitStruct.detectMode      = KeyScan_Detect_Mode_Edge;
    keyScanInitStruct.fifoOvrCtrl     = KeyScan_FIFO_OVR_CTRL_DIS_LAST;
    keyScanInitStruct.maxScanData     = 0x1A;


    if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_12_5ms)
    {
        KeyTimeoutPeriod += 12;
    }
    else if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_25ms)
    {
        KeyTimeoutPeriod += 25;
    }
    else if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_50ms)
    {
        KeyTimeoutPeriod += 50;
    }
    else if (keyScanInitStruct.scanInterval == KeyScan_Scan_Interval_100ms)
    {
        KeyTimeoutPeriod += 100;
    }
    else
    {
        //wrong!!
    }

    KeyTimeoutPeriod += (keyScanInitStruct.debounceTime / 32 + 20);

    RepeatReport = FALSE;

    if (hKeyscan_Timer_0 == NULL)
    {
        hKeyscan_Timer_0 = xTimerCreate("Keyscan_Timer",                    /* Just a text name, not used by the kernel.    */
                                        (KeyTimeoutPeriod / portTICK_RATE_MS), /* The timer period in ticks. KeyTimeoutPeriod = Interval_time +Debounce_time +2ms  */
                                        pdFAIL,                               /* The timers will auto-reload themselves when they expire. */
                                        (void *)1,                            /* Assign each timer a unique id equal to its array index.  */
                                        KeyScan_TimerCallback_0               /* Each timer calls the same callback when it expires.  */
                                       );
        if (hKeyscan_Timer_0 == NULL)
        {
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, " ***hKeyscan_Timer fail", 0);
        }
    }

    KeyScan_Init(KEYSCAN, &keyScanInitStruct);
    KeyScan_INTConfig(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
    KeyScan_Cmd(KEYSCAN, ENABLE);

    /*  Enable KEYSCAN IRQ  */
    NVIC_ClearPendingIRQ(KEYSCAN_IRQ);
    NVIC_SetPriority(KEYSCAN_IRQ, 0);
    NVIC_EnableIRQ(KEYSCAN_IRQ);
}

/**
* @brief  .
*
*
* @param
* @return  void
*/
void KeyPad_DeInit()
{
    /*  Disable KEYSCAN IRQ */
    NVIC_ClearPendingIRQ(KEYSCAN_IRQ);
    NVIC_SetPriority(KEYSCAN_IRQ, 0);
    NVIC_DisableIRQ(KEYSCAN_IRQ);

    /*  Delete Keyscan Timer    */
    if (hKeyscan_Timer_0 != NULL)
    {
        xTimerDelete(hKeyscan_Timer_0, 0);
        hKeyscan_Timer_0 = NULL;
    }

    //Reset Keyscan Setting
    PERIPH->PERI_FUNC0_EN &= ~BIT_PERI_KEYSCAN_EN;
    PERIPH->PERI_CLK_CTRL1 &= ~BIT_SOC_ACTCK_KEYSCAN_EN;
}

/**
* @brief  .
*
*
* @param
* @return  void
*/
void KeyscanIntrHandler(void)
{
    uint32_t reg_value __attribute__((unused)) = 0x0;
    uint32_t FIFO_Length;
    long xHigherPriorityTaskWoken = pdFALSE;

    /* Read current keyscan interrupt status and mask interrupt */
    KeyScan_INTMask(KEYSCAN, ENABLE);
    KeyScan_ClearINTPendingBit(KEYSCAN, KEYSCAN_INT_SCAN_END);

    /*control of entering DLPS */
    allowedKeyboardEnterDlps = false;


    /* Check keyscan timer is created or not.
       If not, it means something wrong and we require to handle this error. */
    if (hKeyscan_Timer_0 == NULL)
    {
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, "hKeyscan_Timer is NULL!", 0);
        goto UNMASK_INT;
    }

    /* Reset timer before use it. */
    xTimerResetFromISR(hKeyscan_Timer_0, &xHigherPriorityTaskWoken);


    memset(pKey_Data, 0, sizeof(KEYSCAN_DATA_STRUCT));

    /* Read keyscan FIFO status */
    reg_value = (uint32_t)KeyScan_GetCurState(KEYSCAN);
    FIFO_Length = (uint32_t)KeyScan_GetFifoDataNum(KEYSCAN);
    pKey_Data->Length = FIFO_Length;

    /* If more than one data in FIFO */
    if (FIFO_Length > 0)
    {
        if (FIFO_Length > (sizeof(pKey_Data->key) / sizeof(uint8_t)))
        {
            FIFO_Length = sizeof(pKey_Data->key) / sizeof(uint8_t);
        }

        /* Read out all FIFO data */
        KeyScan_Read(KEYSCAN, (uint8_t *)(&pKey_Data->key[0]), FIFO_Length);

    }
    RepeatReport = false;
    if (!RepeatReport)
    {
        if (!memcmp(pKey_Data, &Key_Data_Pre, sizeof(KEYSCAN_DATA_STRUCT)))
        {
            goto UNMASK_INT;
        }
        else
        {
            memcpy(&Key_Data_Pre, pKey_Data, sizeof(KEYSCAN_DATA_STRUCT));
        }
    }

    /* Send event to app task */
    BEE_IO_MSG keyscan_msg;
    keyscan_msg.IoType = IO_KEYSCAN_MSG_TYPE;
    keyscan_msg.subType = MSG_KEYSCAN_RX_PKT;
    keyscan_msg.pBuf = (void *)pKey_Data;


    if (!SendMessageFromISR(&keyscan_msg, &xHigherPriorityTaskWoken))
    {
        xTimerStopFromISR(hKeyscan_Timer_0, &xHigherPriorityTaskWoken);
        pKey_Data = NULL;
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " ***Key Scan SendQueue fail.", 0);
        goto UNMASK_INT;
    }

UNMASK_INT:
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

    /* Clear and unmask interrupt */
    KeyScan_INTMask(KEYSCAN, DISABLE);   //unmask interrupt
}

/**
* @brief  .
*
*
* @param
* @return  void
*/
void KeyScan_TimerCallback_0(xTimerHandle xTimer)
{
    memset(&Key_Data, 0, sizeof(KEYSCAN_DATA_STRUCT));
    memset(&Key_Data_Pre, 0, sizeof(KEYSCAN_DATA_STRUCT));

    /*control of entering DLPS */
    allowedKeyboardEnterDlps = false;

    BEE_IO_MSG keyscan_msg;
    keyscan_msg.IoType = IO_KEYSCAN_MSG_TYPE;
    keyscan_msg.subType = MSG_KEYSCAN_ALLKEYRELEASE;
    keyscan_msg.pBuf = NULL;

    if (!SendMessage(&keyscan_msg))
    {
        /* Stop timer then release buffer */
        xTimerStop(hKeyscan_Timer_0, 0xFFFF);
        pKey_Data = NULL;
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " ***Key Scan xTimer SendQueue fail.", 0);
        goto ERROR;
    }
ERROR:
    ;
}
