/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mousebutton.c
* @brief    .
* @details
* @author   Chuanguo Xue
* @date     2015-4-6
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl_endian.h"
#include "mousebutton.h"
#include "board.h"
#include "rtl876x_pinmux.h"
#include "bee_message.h"
#include "dlps_platform.h"

#include "os.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_tim.h"

#define KEY_PRESS_DEBOUNCE_TIME     8*2
#define KEY_RELEASE_DEBOUNCE_TIME   16*2

uint8_t leftkey_status = 1;
uint8_t rightkey_status = 1;
uint8_t isLeftPress = FALSE;
uint8_t isRightPress = FALSE;
extern BOOL allowedLeftButton;
extern BOOL allowedRightButton;

/**
* @brief  Mouse button initialization function.
*
*
* @param   GPIO_INT_PARAM.
* @return  void
*/
void MouseButton_Init()
{
    /* Enable Interrupt (Peripheral, CPU NVIC) */
    {
        GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
        GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);
        GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
        GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);

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
    TIM_InitStruct.TIM_Mode = 1;

    /*LeftButton_Timer Init*/
    TIM_TimeBaseInit(LeftButton_Timer, &TIM_InitStruct);
    /*RightButton_Timer Init*/
    TIM_TimeBaseInit(RightButton_Timer, &TIM_InitStruct);

    /*Enable TIM5 and TIM6 Interrupt*/
    TIM_INTConfig(LeftButton_Timer, ENABLE);
    TIM_INTConfig(RightButton_Timer, ENABLE);
}


/**
* @brief  GPIO interrupt trigger by left button is handled in this function.
*
*
* @param
* @return  void
*/
void MouseButton_LeftIntrHandler()
{
    /*  Mask GPIO interrupt */
    GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX));

    allowedLeftButton = false;

    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "MouseButton_LeftIntrHandler", 0);
    leftkey_status = GPIO_ReadInputDataBit(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX));

    if (isLeftPress == FALSE)
    {
        TIM_ChangePeriod(LeftButton_Timer, KEY_PRESS_DEBOUNCE_TIME * 10000);
    }
    else
    {
        TIM_ChangePeriod(LeftButton_Timer, KEY_RELEASE_DEBOUNCE_TIME * 10000);
    }

    TIM_Cmd(LeftButton_Timer, ENABLE);
}

/**
* @brief  GPIO interrupt trigger by right button is handled in this function.
*
*
* @param
* @return  void
*/
void MouseButton_RightIntrHandler()
{
    /*  Mask GPIO interrupt */
    GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX));

    allowedRightButton = false;

    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "MouseButton_RightIntrHandler", 0);
    rightkey_status = GPIO_ReadInputDataBit(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX));

    if (isRightPress == FALSE)
    {
        TIM_ChangePeriod(RightButton_Timer, KEY_PRESS_DEBOUNCE_TIME * 10000);
    }
    else
    {
        TIM_ChangePeriod(RightButton_Timer, KEY_RELEASE_DEBOUNCE_TIME * 10000);
    }

    TIM_Cmd(RightButton_Timer, ENABLE);
}


/**
* @brief  When debouncing timer of left button is timeoust, this function shall be called.
*
*
* @param
* @return  void
*/
void TimerLeftButtonIntrHandler()
{
    BEE_IO_MSG button_msg;
    long xHigherPriorityTaskWoken = pdFALSE;

    TIM_Cmd(LeftButton_Timer, DISABLE);
    TIM_ClearINT(LeftButton_Timer);

    if (leftkey_status != GPIO_ReadInputDataBit(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX)))
    {
        GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
        GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
        return;
    }

    /* Change GPIO Interrupt Polarity */
    GPIO->INTPOLARITY &= ~GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX);      //Polarity Low

    /* Set Button Message Type */
    button_msg.IoType = IO_MOUSE_BUTTON_MSG_TYPE;

    if (leftkey_status)                                              //If left button released
    {
        button_msg.subType = MOUSE_BTN_LEFT_RELEASE;
        isLeftPress = FALSE;
    }
    else                                                             //If left button pressed.
    {
        button_msg.subType   = MOUSE_BTN_LEFT_PRESS;                 //Left Press
        GPIO->INTPOLARITY |= GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX);   //Polarity High
        isLeftPress = TRUE;
    }

    /* Send MSG to APP Task */
    if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
    {
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " ***Mouse Button SendQueue fail.\n", 0);
    }

    /* Enable GPIO Interrupt */
    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/**
* @brief  When debouncing timer of right button is timeoust, this function shall be called.
*
*
* @param
* @return  void
*/
void TimerRightButtonIntrHandler()
{
    BEE_IO_MSG button_msg;
    long xHigherPriorityTaskWoken = pdFALSE;

    TIM_Cmd(RightButton_Timer, DISABLE);
    TIM_ClearINT(RightButton_Timer);

    if (rightkey_status != GPIO_ReadInputDataBit(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX)))
    {
        GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);
        GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);
        return;
    }

    /* Change GPIO Interrupt Polarity */
    GPIO->INTPOLARITY &= ~GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX);       //Polarity Low

    /* Set Button Message Type */
    button_msg.IoType = IO_MOUSE_BUTTON_MSG_TYPE;

    if (rightkey_status)                                               //If right button released
    {
        button_msg.subType = MOUSE_BTN_RIGHT_RELEASE;
        isRightPress = FALSE;
    }
    else                                                               //If right button pressed.
    {
        button_msg.subType   = MOUSE_BTN_RIGHT_PRESS;                  //Right Press
        GPIO->INTPOLARITY |= GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX);    //Polarity High
        isRightPress = TRUE;
    }

    /* Send MSG to APP task */
    if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
    {
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " ***Mouse Button SendQueue fail.\n", 0);
    }

    /* Enable GPIO Interrupt */
    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

