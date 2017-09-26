#ifndef __SYS_H__
#define __SYS_H__

#include "mxd2660_gpio.h"

    
#define NVR2_TRIM_INFO_STORE            (0x100)       
#define LDO_TRIM                        (0)
#define DCDC_TRIM                       (1)
#define RC32K_TRIM                      (2)
#define ADC_TRIM                        (3)
#define FREQ_TRIM                       (5)       /**<for Frequency deviation calibration*/

/**
 * @brief  some general call back function register
 * @param  none
 * @return none
 */
void user_call_back_register(void);

/**
 * @brief  peripheral int register 
 * @param  none
 * @return none
 */
void user_irq_register(void);

/**
* @brief  disable gpio wakeup cpu.
* @param gpio_pins:  bits map
* @return N/A
*/
extern void system_pins_wakeup_disable(uint32 gpio_pins);
/**
* @brief  enable gpios wakeup cpu.
* @param pins:  gpios bit map
* @param Polarity:   0 ,gpio low wakeup £¬1, gpio high wakeup
* @return N/A
*/
extern void system_pins_wakeup_enable(uint32 gpio_pins, GPIO_WAKEUP_LEVEL Polarity);

#endif




