/*************************************************************************
*
* Wearable device team
* This file provides the total pin config for WT1000
*
**************************************************************************/

#ifndef _BD_PUBLIC_BOARD_CONFIG_PINS_H__
#define _BD_PUBLIC_BOARD_CONFIG_PINS_H__

// Device Information

#define DEVICE_NAME       "DuLife"   /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME  "Baidu Inc"   /**< Manufacturer. Will be passed to Device Information Service. */
#define FW_REV_STR       "1.0.5"
#define MODULE_NUM       "B1000"     /**< Model Number String. */
#define SERIAL_NUM       "0123456789abcde"   /**< Serial Number String. */
#define HW_REV_STR       "1.1.0"     /**< Hardware Revision String. */

#define BATTERY_VOLTAGE_ADJUSTMENT           60     /**< Adjustment for charging */

//#define FEATURE_BUTTON
#define FEATURE_LED_PROGRESS
//#define FEATURE_LED_CLOCK
#define FEATURE_MOTOR
//#define FEATURE_MOTOR_LRA
//#define FEATURE_TWI
#define FEATURE_SENSOR_LIS3DH
#define FEATURE_PRE_INIT


//GPIO pin number for LEDs
#ifdef FEATURE_LED_PROGRESS
  #define BAIDU_LED_0           6   //7
  #define BAIDU_LED_1           5   //19
  #define BAIDU_LED_2           18
  #define BAIDU_LED_3           19  //5
  #define BAIDU_LED_4           7   //6
#endif

//GPIO pin number for MOTOR
#ifdef FEATURE_MOTOR
  #define BAIDU_MOTOR_0         29
#endif
//GPIO pin number for LRA
#ifdef FEATURE_MOTOR_LRA
  #define LRA_EN_PIN     13 /*!< GPIO pin number for LRA_EN           */
  #define LRA_PWM_PIN     14 /*!< GPIO pin number for LRA_PWM           */
  #define LRA_SCL_PIN     15 /*!< GPIO pin number for LRA_SCL           */
  #define LRA_SDA_PIN     16 /*!< GPIO pin number for LRA_SDA           */

//I2C define
#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER LRA_SCL_PIN
  #define TWI_MASTER_CONFIG_DATA_PIN_NUMBER LRA_SDA_PIN
#endif

//GPIO pin number for BUTTON
#ifdef FEATURE_BUTTON
  #define BAIDU_BUTTON_0           /*not use */
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

/* GPIO pin number for LIS3DH */
#ifdef FEATURE_SENSOR_LIS3DH
  #define LIS3DH_INT1     8 /*!< GPIO pin number for LIS3DH_INT1           */
  #define LIS3DH_INT2     28 /*!< GPIO pin number for LIS3DH_INT2          */
#endif


/*****************************************************************
* uart pin config
******************************************************************/
#if defined(DEBUG_LOG) || defined (DEBUG_ACC)

#define RX_PIN_NUMBER  25    // UART RX pin number.
#define TX_PIN_NUMBER  23    // UART TX pin number.
//the follow pin is useless
#define CTS_PIN_NUMBER 43    // UART Clear To Send pin number. Not used if HWFC is set to false
  #define RTS_PIN_NUMBER 43    // Not used if HWFC is set to false
  #define HWFC           false // UART hardware flow control

#endif  //DEBUG_LOG

#endif // _BD_PUBLIC_BOARD_CONFIG_PINS_H__

/** @} */
