/*************************************************************************
*
* Wearable device team
* This file control press event
*
**************************************************************************/
#ifndef _BD_SWITCH_SLEEP_H_
#define _BD_SWITCH_SLEEP_H_
#include <stdbool.h>
#include <stdint.h>
#define SETTING_SLEEP_MODE 0
#define SETTING_STEP_MODE  1
/**
 * handle 2 Double-click event, switch between normal mode and sleeping mode
 *
 * 
 * @note 
 *
 * @param spi_base_address  register base address of the selected SPI master module
 * @param transfer_size  number of bytes to transmit/receive over SPI master
 * @param tx_data pointer to the data that needs to be transmitted
 * @param rx_data pointer to the data that needs to be received
 * @return
 * @retval true if transmit/reveive of transfer_size were completed.
 * @retval false if transmit/reveive of transfer_size were not complete and tx_data/rx_data points to invalid data.
 */
void two_Double_tap_handle(void);

void restore_SleepSettingsFromFlash(void);
bool save_sleep_settings_to_flash(void);
void update_sleep_setting_head(void);
void reset_sleep_setting_head(void);
void set_curr_sleep_flag(bool sleeping);
bool get_curr_sleep_flag(void);
uint16_t sleep_setting_count(void);
bool save_sleep_settings_to_flash(void);
bool send_sleep_setting_data(void);
void send_sleep_setting_data_cb(void);
#endif
