/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mouse_autotest.c
* @brief
* @details
* @author   Elliot Chen
* @date     2015-7-31
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "FreeRTOS.h"
#include "os.h"
#include "board.h"
#include "bee_message.h"
#include "dlps_platform.h"
#include "rtl876x_io_dlps.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_nvic.h"
#include "mouse_autotest.h"
#include "rtl_delay.h"

/* Reference varibales statement -------------------------------*/
extern BOOL allowedAutotest;

/* Global variables defines ------------------------------------*/
/* Store test data*/
uint8_t mosue_AutoTest_xy[2] = {0, 0};
/* Count nuber of transmit data in connection state */
uint8_t mouse_xy_count = 0;
/*Count number of connection */
uint16_t test_count = 0;

/* Function declaration -------------------------------------------------*/

/**
  * @brief  Initializes the timer for send test data to app task in auto test mode.
  * @param  none.
  * @retval None
  */
void AutoTest_TimerInit(void)
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_InitStruct.TIM_Period = 15000 * 10000;
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
    /* Timer init*/
    TIM_TimeBaseInit(AutoTest_Timer, &TIM_InitStruct);

    /* Enable timer interrupt */
    TIM_INTConfig(AutoTest_Timer, ENABLE);
    //delayMS(10000);
}

/**
  * @brief  packing the test data which will be sent to app task in auto test mode.
  * @param  p_mouse_xy_msg: point to a BEE_IO_MSG structure that contains the test data.
  * @retval   return exection state of function.
  */
BOOL AutoTest_PacketEncode(BEE_IO_MSG *p_mouse_xy_msg)
{
    BEE_IO_MSG button_msg;
    long xHigherPriorityTaskWoken = pdFALSE;
    button_msg.IoType = IO_MOUSE_BUTTON_MSG_TYPE;

    /* Count number of transmit data */
    mouse_xy_count++;

    if (mouse_xy_count <= 2)
    {
        /* Disable AutoTest_Timer */
        AutoTest_Timer->ControlReg &= ~(BIT(0));
        /* Set 4ms */
        AutoTest_Timer->LoadCount = 4 * 10000;
        /* Enable AutoTest_Timer */
        AutoTest_Timer->ControlReg |= BIT(0);

        mosue_AutoTest_xy[0] = test_count % 5 + 1;
        mosue_AutoTest_xy[1] = test_count % 5 + 1;
    }
    else if (mouse_xy_count <= 20)
    {
        mosue_AutoTest_xy[0] = test_count % 5 + 1;
        mosue_AutoTest_xy[1] = test_count % 5 + 1;

        /* Delay 5s to send data */
        if (mouse_xy_count == 20)
        {
            /* Disable AutoTest_Timer */
            AutoTest_Timer->ControlReg &= ~(BIT(0));
            /* Set 5s */
            AutoTest_Timer->LoadCount = 30000000;
            /* Enable AutoTest_Timer */
            AutoTest_Timer->ControlReg |= BIT(0);
        }

        /*--------------------button message--------------*/
        if (mouse_xy_count % 2 == 1)
        {
            /* Send left button press message */
            button_msg.subType   = MOUSE_BTN_LEFT_PRESS;
            /* Send MSG to APP task */
            if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
            {
                DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " AutoTest-->Mouse Button SendQueue fail.\n", 0);
            }

            /* Send left button release message */
            button_msg.subType   = MOUSE_BTN_LEFT_RELEASE;
            /* Send MSG to APP task */
            if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
            {
                DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " AutoTest-->Mouse Button SendQueue fail.\n", 0);
            }
        }
    }
    else
    {
        /*--------------------button message--------------*/
        if (mouse_xy_count == 21)
        {
            /* Send left button press message */
            button_msg.subType   = MOUSE_BTN_LEFT_PRESS;
            /* Send MSG to APP task */
            if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
            {
                DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " AutoTest-->Mouse Button SendQueue fail.\n", 0);
            }

            /* Send left button release message */
            button_msg.subType   = MOUSE_BTN_LEFT_RELEASE;
            /* Send MSG to APP task */
            if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
            {
                DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " AutoTest-->Mouse Button SendQueue fail.\n", 0);
            }
        }

        /* Disable AutoTest_Timer */
        AutoTest_Timer->ControlReg &= ~(BIT(0));
        /* Set 4ms */
        AutoTest_Timer->LoadCount = 4 * 10000;
        /* Enable AutoTest_Timer */
        AutoTest_Timer->ControlReg |= BIT(0);

        mosue_AutoTest_xy[0] = 0x9c;
        mosue_AutoTest_xy[1] = 0x9c;

        if (mouse_xy_count >= 60)
        {
            mouse_xy_count = 0;

            /* stop to send data */
            TIM_Cmd(AutoTest_Timer, DISABLE);
        }
    }

    p_mouse_xy_msg->IoType = MOUSE_SENSOR_MSG;
    p_mouse_xy_msg->parm = (mosue_AutoTest_xy[0] << 8) | (mosue_AutoTest_xy[1]);

    DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "x: 0x%x,  Y:0x%x", 2, mosue_AutoTest_xy[0], mosue_AutoTest_xy[1]);

    return true;
}

void AutoTest_TimerIntrHandler()
{
    BEE_IO_MSG mouse_xy_msg;

    long xHigherPriorityTaskWoken = pdFALSE;

    /*--------------DLPS timer handle---------------*/
    allowedAutotest = false;

    /*--------------Auto test timer handle-----------*/
    /* Disable AutoTest_Timer */
    AutoTest_Timer->ControlReg &= ~(BIT(0));
    /* Set 4ms */
    AutoTest_Timer->LoadCount = 4 * 10000;
    /* Enable AutoTest_Timer */
    AutoTest_Timer->ControlReg |= BIT(0);
    /* Clear interrupt */
    TIM_ClearINT(AutoTest_Timer);

    AutoTest_PacketEncode(&mouse_xy_msg);

#if 0
    //DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "x: 0x%x,  Y:0x%x", 2, mosue_AutoTest_xy[0], mosue_AutoTest_xy[1]);
    mouse_xy_msg.IoType = MOUSE_SENSOR_MSG;
    mouse_xy_msg.parm = (mosue_AutoTest_xy[0] << 8) | (mosue_AutoTest_xy[1]);
    mouse_xy_count++;
    if (mouse_xy_count >= 2)
    {
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "x: 0x%x,  Y:0x%x", 2, mosue_AutoTest_xy[0], mosue_AutoTest_xy[1]);
        mouse_xy_count = 0;
        mosue_AutoTest_xy[1]++;
    }
    if (mosue_AutoTest_xy[1] > 20)
    {
        mosue_AutoTest_xy[1] = 1;
        TIM_Cmd(AutoTest_Timer, DISABLE);
    }
#endif

    /* -------------Send MSG to APP task------------*/
    if (!SendMessageFromISR(&mouse_xy_msg, &xHigherPriorityTaskWoken))
    {
        DBG_BUFFER(MODULE_TIMER, LEVEL_INFO, "AutoTest-->send mouse test data fail.\n", 0);
    }

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
