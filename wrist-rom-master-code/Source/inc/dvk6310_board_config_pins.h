/*************************************************************************
*
* Wearable device team
* This file provides the total pin config for dev6310
*
**************************************************************************/

#ifndef DVK6310_BOARD_CONFIG_PINS_H__
#define DVK6310_BOARD_CONFIG_PINS_H__

#define DEVICE_NAME                       "DuLife"        /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                 "Baidu Inc"         /**< Manufacturer. Will be passed to Device Information Service. */
#define FW_REV_STR            "0.9.0"
#define MODULE_NUM       "DVK6310"     /**< Model Number String. */
#define SERIAL_NUM       "0123456789abcde"   /**< Serial Number String. */
#define HW_REV_STR       "1.0"     /**< Hardware Revision String. *

#define BATTERY_VOLTAGE_ADJUSTMENT           60               /**< Adjustment for charging */

#define FEATURE_BUTTON
#define FEATURE_LED_PROGRESS
//#define FEATURE_LED_CLOCK
//#define FEATURE_MOTOR
//#define FEATURE_MOTOR_LRA
//#define FEATURE_TWI
//#define FEATURE_SENSOR_LIS3DH

//GPIO pin number for LEDs
#ifdef FEATURE_LED_PROGRESS
    #define BAIDU_LED_0           8
    #define BAIDU_LED_1           9
    #define BAIDU_LED_2           10
    #define BAIDU_LED_3           11
    #define BAIDU_LED_4           12
    #define BAIDU_MOTOR_0           13      //use led
#endif


//GPIO pin number for BUTTON
#ifdef FEATURE_BUTTON
  #define BAIDU_BUTTON_0        0
#endif

//XXX
#define ADVERTISING_LED_PIN_NO      BAIDU_LED_0
#define CONNECTED_LED_PIN_NO        BAIDU_LED_1
#define ASSERT_LED_PIN_NO           BAIDU_LED_2

//GPIO pin number for Charging
#define CHARGER_CONNECTED_PIN 1
#define CHARGER_CHARGING_PIN 17  /*!< GPIO pin number for Charging State           */


/* GPIO pin number for SPI */
#define SPI_PSELSCK0              4   /*!< GPIO pin number for SPI clock (note that setting this to 31 will only work for loopback purposes as it not connected to a pin) */
#define SPI_PSELMOSI0             3   /*!< GPIO pin number for Master Out Slave In    */
#define SPI_PSELMISO0             21   /*!< GPIO pin number for Master In Slave Out    */
#define SPI_PSELSS0               22   /*!< GPIO pin number for Slave Select           */

/*****************************************************************
* uart pin config
******************************************************************/
#ifdef DEBUG_LOG

#define RX_PIN_NUMBER  16    // UART RX pin number.
    #define TX_PIN_NUMBER  17    // UART TX pin number.

#define CTS_PIN_NUMBER 18    // UART Clear To Send pin number. Not used if HWFC is set to false
    #define RTS_PIN_NUMBER 19    // Not used if HWFC is set to false
    #define HWFC           false // UART hardware flow control

#endif  //DEBUG_LOG

#endif //DVK6310_BOARD_CONFIG_PINS_H__
