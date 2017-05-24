/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyboardbutton.c
* @brief
* @details
* @author   Elliot Chen
* @date     2015-7-20
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl_endian.h"
#include "keyboardbutton.h"
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

uint8_t key_status = 1;
uint8_t isPress = FALSE;
extern BOOL allowedKeyboardEnterDlps;

/**
* @brief  keyboard button initialization function.
*
*
* @param   GPIO_INT_PARAM.
* @return  void
*/
void KeyboardButton_Init()
{
    GPIO_InitTypeDef KeyboardButton_Param;     /* Define Mouse Button parameter structure. Mouse button is configed as GPIO. */
    KeyboardButton_Param.GPIO_Pin  = GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX);
    KeyboardButton_Param.GPIO_Mode = GPIO_Mode_IN;
    KeyboardButton_Param.GPIO_ITCmd = ENABLE;
    KeyboardButton_Param.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
    KeyboardButton_Param.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    KeyboardButton_Param.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
    GPIO_Init(&KeyboardButton_Param);

    GPIO_INTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), ENABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), DISABLE);

    NVIC_ClearPendingIRQ(PERIPHERAL_IRQ);
    NVIC_SetPriority(PERIPHERAL_IRQ, 0);
    NVIC_EnableIRQ(PERIPHERAL_IRQ);

    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_InitStruct.TIM_Period = KEY_PRESS_DEBOUNCE_TIME * 10000;
    TIM_InitStruct.TIM_Mode = 1;

    TIM_TimeBaseInit(Switch_Button_Timer, &TIM_InitStruct);

    /* enable  interrupt */
    TIM_INTConfig(Switch_Button_Timer, ENABLE);
}


/**
* @brief  GPIO interrupt trigger by button is handled in this function.
*
*
* @param
* @return  void
*/
/**
* @brief  GPIO Interrupt handler
*
*
* @return  void
*/
void SwitchButtonIntrHandler()
{
    /*  Mask GPIO interrupt */
    GPIO_INTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX));

    allowedKeyboardEnterDlps = false;

    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SwitchButtonIntrHandler", 0);
    key_status = GPIO_ReadInputDataBit(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX));

    if (isPress == FALSE)
    {
        TIM_ChangePeriod(Switch_Button_Timer, KEY_PRESS_DEBOUNCE_TIME * 10000);
    }
    else
    {
        TIM_ChangePeriod(Switch_Button_Timer, KEY_RELEASE_DEBOUNCE_TIME * 10000);
    }

    TIM_Cmd(Switch_Button_Timer, ENABLE);
}

/**
* @brief  When debouncing timer of left button is timeoust, this function shall be called.
*
*
* @param
* @return  void
*/
void TimerSwitchButtonIntrHandler()
{
    BEE_IO_MSG button_msg;
    long xHigherPriorityTaskWoken = pdFALSE;

    TIM_Cmd(Switch_Button_Timer, DISABLE);
    TIM_ClearINT(Switch_Button_Timer);
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, " ***TimerSwitchButtonIntrHandler\n", 0);

    if (key_status != GPIO_ReadInputDataBit(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX)))
    {
        GPIO_MaskINTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), DISABLE);
        GPIO_INTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), ENABLE);
        return;
    }

    /* Change GPIO Interrupt Polarity, Enable Interrupt */
    button_msg.IoType = IO_KEYBOARD_BUTTON_MSG_TYPE;

    GPIO->INTPOLARITY &= ~(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX));

    if (key_status)
    {
        button_msg.subType = KEYBOARD_SWITCH_BTN_RELEASE;
        isPress = FALSE;
    }
    else
    {
        button_msg.subType   = KEYBOARD_SWITCH_BTN_PRESS;
        GPIO->INTPOLARITY |= GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX);
        isPress = TRUE;
    }

    /* Send MSG to APP task */
    if (!SendMessageFromISR(&button_msg, &xHigherPriorityTaskWoken))
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, " ***Keyboard Button SendQueue fail.\n", 0);
    }

    GPIO_MaskINTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(KEYBOARD_SWITCH_BUTTON_INDEX), ENABLE);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


