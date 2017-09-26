/**
 *****************************************************************************
 * @file     led.c
 * @author   Richard
 * @version  V1.0.0
 * @date     22-June-2016
 * @brief    LED module
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */

#include "system_config.h"
#include "mxd2660_gpio.h"
#include "led.h"

void led_init(void)
{
    gpio_init_output(LED_RED_PIN, GPIO_PULL_UP, 1);
    gpio_init_output(LED_BLUE_PIN, GPIO_PULL_UP, 1);
    gpio_init_output(LED_GREEN_PIN, GPIO_PULL_UP, 1);
}

void led_on(GPIO_DEFINE gpio_pin)
{
    gpio_clear_bit(gpio_pin);
}

void led_off(GPIO_DEFINE gpio_pin)
{
    gpio_set_bit(gpio_pin);
}

