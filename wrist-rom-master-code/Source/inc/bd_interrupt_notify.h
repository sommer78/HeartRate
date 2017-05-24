/*************************************************************************
*
* Wearable device team
* register an io interrupt and event callback
*
**************************************************************************/
#ifndef INTERRUPT_NOTIFY_H__
#define INTERRUPT_NOTIFY_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "nrf_gpio.h"

typedef void (* io_int_event_callback_t)(uint8_t pin_no); // 带一个参数，可以用一个callback来处理多个引脚的中断回调

typedef struct
{
    uint8_t pin_no;
    bool    active_high;
    nrf_gpio_pin_pull_t pull_cfg;
    io_int_event_callback_t callback;

}
io_int_event_cfg_t;



uint32_t interrupt_notify_register( io_int_event_cfg_t *      p_io_pins,
                                    uint8_t                 pin_count);

uint32_t app_io_evt_register_enable(void);

uint32_t app_io_evt_register_disable(void);



#endif //INTERRUPT_NOTIFY_H__
