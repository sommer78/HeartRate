/*************************************************************************
*
* Wearable device team
* This file charging detect & charging interaction 
*
**************************************************************************/
#include <stdint.h>
#include "board_config_pins.h"
/**************************************************************************
* Charging state defination
***************************************************************************/
typedef enum {
    InCharging = 0,       /* Device connect to the plugs*/
    ChargingComplete,      /* Show charging complete & still connect to plugs*/
    NoCharge           /* Show not connect to plugs*/
}Charging_State;

void charger_init(void);
void charger_framework_init(void);
Charging_State charger_status(void);
uint8_t charger_connected(void);
void function_charger_event_handler(uint8_t pin_no);
void function_chargingcomplete_event_handler(uint8_t pin_no);

