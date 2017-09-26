/**
 *****************************************************************************
 * @file     led.h
 * @author   Richard
 * @version  V1.0.0
 * @date     22-June-2016
 * @brief    LED module
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */

#ifndef __LED_H__
#define __LED_H__

#define LED_RED_PIN        GPIO_17
#define LED_BLUE_PIN       GPIO_19
#define LED_GREEN_PIN      GPIO_18

extern void led_init(void);
extern void led_on(GPIO_DEFINE gpio_pin);
extern void led_off(GPIO_DEFINE gpio_pin);

#endif


