/*************************************************************************
*
* Wearable device team
* This file is used to implement a wall clock timer
*
**************************************************************************/

#include <stdint.h>
#include <string.h>
#include "app_util.h"
#include "config.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "board_config_pins.h"
#include "hal_acc.h"
#include "bd_charging.h"

void power_wfi()
{
#ifdef CHARGER_CONNECTED_PIN
    GPIO_WAKEUP_BUTTON_CONFIG(CHARGER_CONNECTED_PIN);//charger wakeup
#else

    GPIO_WAKEUP_BUTTON_CONFIG(CHARGER_CHARGING_PIN);//charger wakeup
#endif

    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    NVIC_EnableIRQ(GPIOTE_IRQn);
    while(true) {
        __WFI();
        if(charger_connected()) {
            break;
        }
    }

    //wakeup, disable irq
    NVIC_DisableIRQ(GPIOTE_IRQn);
}

void power_system_off()
{
#ifdef CHARGER_CONNECTED_PIN
    GPIO_WAKEUP_BUTTON_CONFIG(CHARGER_CONNECTED_PIN);//charger wakeup
#else

    GPIO_WAKEUP_BUTTON_CONFIG(CHARGER_CHARGING_PIN);//charger wakeup
#endif
    // close sensor
    hal_acc_PowerDown();
    //enter system off
    NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
}

void sd_system_off()
{
#ifdef CHARGER_CONNECTED_PIN
    GPIO_WAKEUP_BUTTON_CONFIG(CHARGER_CONNECTED_PIN);//charger wakeup
#else

    GPIO_WAKEUP_BUTTON_CONFIG(CHARGER_CHARGING_PIN);//charger wakeup
#endif
    // close sensor
    hal_acc_PowerDown();
    //enter system off
    sd_power_system_off();
}
