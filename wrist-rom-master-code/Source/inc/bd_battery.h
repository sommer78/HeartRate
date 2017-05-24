/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @cond To make doxygen skip this file */

/** @file
 *
 * @defgroup ble_sdk_app_hrs_eval_battery Battery Level Hardware Handling
 * @{
 * @ingroup ble_sdk_app_hrs_eval
 * @brief Battery Level Hardware Handling prototypes
 *
 */

#ifndef BATTERY_H__
#define BATTERY_H__
#include "app_scheduler.h"

#define ADC_INPUT_CHANNEL  ADC_CONFIG_PSEL_AnalogInput3
#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                     /**< Reference voltage (in  milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         1                                         /**< The ADC is configured to use VDD with no prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define ADC_HW_PRE_SCALING_COMPENSATION      4

#define BATTERY_ADC_RESOLUTION  (ADC_CONFIG_RES_10bit)
#define BATTERY_ADC_DIV         (1023)

#define MIN_BAT_MV (3600)
#define CHARGER_CHECK_INTERVAL_MS (APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER))

/**@brief Macro to convert the result of ADC conversion in millivolts.
 *
 * @param[in]  ADC_VALUE   ADC result.
 * @retval     Result converted to 0.1 millivolts.
 */
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS)  * ADC_PRE_SCALING_COMPENSATION * ADC_HW_PRE_SCALING_COMPENSATION)/ BATTERY_ADC_DIV)

/**@brief Function to make the ADC start a battery level conversion
 */
void battery_start(void);
void battery_measure_init(void);
void battery_measure_timer_start(uint32_t interval);
void register_battery_up_callback(app_sched_event_handler_t handle);
void battery_adc_dev_init(void);

uint8_t get_battery_power_percentage(void);
void battery_adc_read_once(uint16_t* batt_lvl_in_milli_volts);
void battery_adc_read_once_for_charging_detect(uint16_t * batt_lvl_in_milli_volts);
uint8_t voltage_detect_n_precharging(void);
#endif // BATTERY_H__

/** @} */
/** @endcond */
