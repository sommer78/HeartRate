/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    pxp_io_new.c
  * @brief   .
    * @details
  * @author  Chuanguo Xue
  * @date    6-April-2015
  * @version v0.1
  ******************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl_endian.h"
#include "pxp_io_new.h"
#include "board.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_adc.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rtc.h"
#include "rtl876x_nvic.h"
#include "bee_message.h"
#include "rtl876x_tim.h"
#include "dlps_platform.h"
#include "rtl876x_io_dlps.h"

#include "os.h"
#include "lls.h"
#include "tps.h"
#include "bas.h"
#include "pxp_ext.h"

#define KEY_PRESS_DEBOUNCE_TIME     8*2
#define KEY_RELEASE_DEBOUNCE_TIME   16*2
#define RTC_COMPARATOR_INDEX  0

uint8_t keystatus = 1;
uint8_t isPress = FALSE;
uint8_t keycancel = FALSE;

uint8_t gBASBatteryLevel = 0;
uint8_t gPxp_linkloss_alert_level = 2;
uint8_t gPxp_immediate_alert_level = 0;
uint8_t gPxp_tx_power = 0;

UINT32 ImmActCnt = 0;
UINT32 LssActCnt = 0;
UINT32 TimeImmAct = 20;
extern UINT32 gTimeParaValue;
extern uint8_t gBASServiceId;
extern BOOL allowedProximityEnterDlps;

TimerHandle_t xTimersImmAlert = NULL;
TimerHandle_t xTimersLssAlert = NULL;

void vTimerAlertCallback(TimerHandle_t pxTimer);
void ADC_CollectTimeout(xTimerHandle handle);
void Key0TimerIntrHandler(void);
void Key0GpioIntrHandler(void);

/**
  * @brief  Key button initialization function.
  * @param  GPIO_INT_PARAM.
  * @retval None
  */
void KEY_Init(GPIO_InitTypeDef *GPIO_INT_PARAM)
{
    /* Enable Interrupt (Peripheral, CPU NVIC) */
    {
        GPIO_INTConfig(GPIO_GetPin(KEY_0), ENABLE);
        //GPIO_INTConfig(GPIO_GetPin(KEY_1), ENABLE);

        /*  GPIO2~GPIO31 IRQ    */
        NVIC_ClearPendingIRQ(PERIPHERAL_IRQ);
        NVIC_SetPriority(PERIPHERAL_IRQ, 0);
        NVIC_EnableIRQ(PERIPHERAL_IRQ);
    }
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_InitStruct.TIM_Period = KEY_PRESS_DEBOUNCE_TIME * 10000;
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
    TIM_TimeBaseInit(KEY0_Timer, &TIM_InitStruct);
    TIM_INTConfig(KEY0_Timer, ENABLE);

    xTimersImmAlert = xTimerCreate("xTimersImmAlert",       // Just a text name, not used by the kernel.
                                   20,   // The timer period in ticks.
                                   pdFALSE,        // The timers will auto-reload themselves when they expire.
                                   ( void *) 1,   // Assign each timer a unique id equal to its array index.
                                   vTimerAlertCallback // Each timer calls the same callback when it expires.
                                  );

    if ( xTimersImmAlert == NULL )
    {
        // The timer was not created.
    }
    else
    {
        //The timer was created.
    }

    xTimersLssAlert = xTimerCreate("xTimersLssAlert",       // Just a text name, not used by the kernel.
                                   20,   // The timer period in ticks.
                                   pdFALSE,        // The timers will auto-reload themselves when they expire.
                                   ( void *) 2,   // Assign each timer a unique id equal to its array index.
                                   vTimerAlertCallback // Each timer calls the same callback when it expires.
                                  );

    if ( xTimersLssAlert == NULL )
    {
        // The timer was not created.
    }
    else
    {
        //The timer was created.
    }
}

/******************************************************************
 * @fn       ADC_Init
 * @brief    Initialize ADC
 * @param    void
 * @return   void
 */
void ADC_Battery_Init()
{
    /* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

    ADC_InitTypeDef adcInitStruct;
    ADC_StructInit(&adcInitStruct);

    /* use battery channel & auto sensor mode */
    adcInitStruct.channelMap = ADC_CH_BAT;
    adcInitStruct.adcMode = ADC_One_Shot_Mode;
    ADC_Init(ADC, &adcInitStruct);

    /* disable auto sensor mode */
    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);


}

/******************************************************************
 * @fn       RTC_Init
 * @brief    Initialize RTC
 * @param    void
 * @return   void
 */
void RTC_Initconfig(void)
{
    /* initialize RTC*/
    RTC_Reset();
    RTC_Enable();
    RTC_SetPrescaler(0);
    RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, 10 * 32768);
    RTC_EnableCOMP(RTC_COMPARATOR_INDEX);
    RTC_EnableSystemWakeup();

    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

}

/******************************************************************
 * @fn       Key0GpioIntrHandler
 * @brief    GPIO interrupt trigger by button is handled in this function.
 * @param    void
 * @return   void
 */
void Key0GpioIntrHandler()
{
#if CONFIG_DLPS_EN
    allowedProximityEnterDlps = false;
#endif
    keystatus = GPIO_ReadInputDataBit(GPIO_GetPin(KEY_0));
    /*  Mask GPIO interrupt */
    GPIO_INTConfig(GPIO_GetPin(KEY_0), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_0), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(KEY_0));

    //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Enter Key0 intr\n", 0);
    if (isPress == FALSE) {
        /* delay 16ms to read the GPIO status */
        TIM_ChangePeriod(KEY0_Timer, KEY_PRESS_DEBOUNCE_TIME * 10000);
    } else {
        /* delay 32ms to read the GPIO status */
        TIM_ChangePeriod(KEY0_Timer, KEY_RELEASE_DEBOUNCE_TIME * 10000);
    }
    TIM_Cmd(KEY0_Timer, ENABLE);
}

/******************************************************************
 * @fn       Key0TimerIntrHandler
 * @brief    When debouncing timer of the button is timeoust, this function shall be called.
 * @param    void
 * @return   void
 */
void Key0TimerIntrHandler()
{
    BEE_IO_MSG button_msg;
    long xHigherPriorityTaskWoken = pdFALSE;
    TIM_Cmd(KEY0_Timer, DISABLE);
    TIM_ClearINT(KEY0_Timer);
    //DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " SEND MSG.", 0);

    if (keystatus != GPIO_ReadInputDataBit(GPIO_GetPin(KEY_0)))
    {
        GPIO_MaskINTConfig(GPIO_GetPin(KEY_0), DISABLE);
        GPIO_INTConfig(GPIO_GetPin(KEY_0), ENABLE);
        return;
    }

    /* Change GPIO Interrupt Polarity, Enable Interrupt */
    button_msg.IoType = IO_GPIO_MSG_TYPE;
    if (ImmActCnt || LssActCnt)
    {
        keycancel = TRUE;
    }
    GPIO->INTPOLARITY &= ~GPIO_GetPin(KEY_0);       //Polarity Low

    if (keystatus)                                  //If the button released
    {
        button_msg.parm = 0;
        if (keycancel)
        {
            button_msg.subType = KEY_ALERT_CANCEL;
        }
        else
        {
            button_msg.subType = KEY_FIND_ME;
        }
        keycancel = FALSE;
        isPress = FALSE;
    }
    else                                            //If the button pressed.
    {
        button_msg.parm = 1;
        if (keycancel)
        {
            button_msg.subType = KEY_ALERT_CANCEL;
        }
        else
        {
            button_msg.subType = KEY_FIND_ME;
        }
        GPIO->INTPOLARITY |= GPIO_GetPin(KEY_0);    //Polarity High
        isPress = TRUE;
    }
    GPIO_MaskINTConfig(GPIO_GetPin(KEY_0), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(KEY_0), ENABLE);


    /* Send MSG to APP task */
    if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
    {
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " ***Find me Button SendQueue fail.\n", 0);
    }
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/******************************************************************
 * @fn       RTCIntrHandler
 * @brief    Read and send the battery level in RTC interrupt.
 * @param    void
 * @return   void
 */
void RTCIntrHandler(void)
{
    uint32_t CompareValue = 0;
    uint16_t adcConvertRes = 0;

    /* start adc convert procedure */
    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
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
    /* update battery level to APP*/
    ProfileAPI_SendData(gBASServiceId, GATT_SVC_BAS_BATTERY_LEVEL_INDEX, &gBASBatteryLevel, sizeof(gBASBatteryLevel));

    /* selected comparator interrupt */
    if ((RTC->SR_OFF & BIT(16 + RTC_COMPARATOR_INDEX)) == BIT(16 + RTC_COMPARATOR_INDEX))
    {
        CompareValue = RTC_GetCOMPValue(RTC_COMPARATOR_INDEX) + 32768 * 10;
        RTC_SetCOMPValue(RTC_COMPARATOR_INDEX, CompareValue & 0xFFFFFF);

        RTC_ClearCOMPEvent(RTC_COMPARATOR_INDEX);
    }
}

/******************************************************************
 * @fn       vTimerAlertCallback
 * @brief    Timer callback for immediate alert and linkloss alert.
 * @param    TimerHandle_t pxTimer
 * @return   void
 */
void vTimerAlertCallback(TimerHandle_t pxTimer )  //ImmediateAlertLevel   //LinkLossAlertLevel
{
    int32_t lArrayIndex;
    UINT8 value;
    TickType_t xNewPeriod;

    lArrayIndex = ( int32_t ) pvTimerGetTimerID( pxTimer );
    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "1. lArrayIndex = 0x%x", 1, lArrayIndex);

    if ( xTimerIsTimerActive( pxTimer ) != pdFALSE ) // or more simply and equivalently "if( xTimerIsTimerActive( xTimer ) )"
    {
        // xTimer is already active - delete it.
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "xTimer is already active - delete it", 0);
        // xTimerDelete( pxTimer,0);
    }
    else
    {
        if (lArrayIndex == 1)
        {
            if (gPxp_immediate_alert_level == 0)
            {
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "gPxp_immediate_alert_level = 0x%x", 1, gPxp_immediate_alert_level);
#if CONFIG_DLPS_EN
                allowedProximityEnterDlps = true;
#endif
            }
            else
            {
                value = GPIO_ReadOutputDataBit(GPIO_GetPin(LED_0));
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "GPIOB 5 Value is %x", 1, value);
                if (value & 0x1)
                {
                    xNewPeriod = 90;
                    GPIO_ResetBits(GPIO_GetPin(LED_0));

                    if (gPxp_immediate_alert_level == 2)
                    {
                        GPIO_ResetBits(GPIO_GetPin(BEEP));
                    }
                    ImmActCnt++;
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "RESET is %x", 4, GPIO->DATAOUT);
                }
                else
                {
                    xNewPeriod = 10;
                    GPIO_SetBits(GPIO_GetPin(LED_0));

                    if (gPxp_immediate_alert_level == 2)
                    {
                        GPIO_SetBits(GPIO_GetPin(BEEP));
                    }
                    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "SET is %x", 4, GPIO->DATAOUT);
                }
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "ImmActCnt = %d", 4, ImmActCnt);

                if (ImmActCnt < TimeImmAct)
                {
                    if ( xTimerChangePeriod( pxTimer, xNewPeriod, 0 ) == pdPASS ) //800 / portTICK_PERIOD_MS
                    {
                        // The command was successfully sent.
                    }
                    else
                    {
                        // The command could not be sent, even after waiting for 100 ticks
                        // to pass.  Take appropriate action here.
                    }
                }
                else
                {
                    ImmActCnt = 0;
                    GPIO_ResetBits(GPIO_GetPin(BEEP));
                    GPIO_ResetBits(GPIO_GetPin(LED_0));

#if CONFIG_DLPS_EN
                    allowedProximityEnterDlps = true;
#endif
                }
            }//END ELSE gPxp_immediate_alert_level == 0
        }
        else if (lArrayIndex == 2)
        {
            value = GPIO_ReadOutputDataBit(GPIO_GetPin(LED_0));
            DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "GPIOB 5 Value is %x", 1, value);
            if (value & 0x1)
            {
                xNewPeriod = 90;
                GPIO_ResetBits(GPIO_GetPin(LED_0));
                if (gPxp_linkloss_alert_level == 2)
                {
                    GPIO_ResetBits(GPIO_GetPin(BEEP));
                }
                LssActCnt++;
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "LssActCnt Value is %d", 1, LssActCnt);
            }
            else
            {
                xNewPeriod = 10;
                GPIO_SetBits(GPIO_GetPin(LED_0));
                if (gPxp_linkloss_alert_level == 2)
                {
                    GPIO_SetBits(GPIO_GetPin(BEEP));
                }
            }
            if (LssActCnt < gTimeParaValue)
            {
                if ( xTimerChangePeriod( pxTimer, xNewPeriod, 0 ) == pdPASS ) //800 / portTICK_PERIOD_MS
                {
                    // The command was successfully sent.
                }
                else
                {
                    // The command could not be sent, even after waiting for 100 ticks
                    // to pass.  Take appropriate action here.
                }
            }
            else
            {
                LssActCnt = 0;
#if CONFIG_DLPS_EN
                allowedProximityEnterDlps = true;
#endif

            }
        }
        // xTimer is not active, change its period to 500ms.  This will also
        // cause the timer to start.  Block for a maximum of 100 ticks if the
        // change period command cannot immediately be sent to the timer
        // command queue.

    }
}
