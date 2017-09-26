/**
****************************************************************************
* @file    mxd2660_gpio.h
* @brief   
* @author  wanli.han
* @version Initial Draft
* @date    2016/11/14
* @history 
* @note    
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
#ifndef __MXD2660_GPIO_H__
#define __MXD2660_GPIO_H__

#ifdef __cplusplus
	extern "C" {
#endif 

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "system_config.h"
#include "mxdaqua.h"

#define GPIO_MAX  (31)

typedef enum
{
	GPIO_PIN = 0,
	UART_0_TXD = 1,
	UART_0_RXD = 2,
	UART_0_CTS = 3,
	UART_0_RTS = 4,
	UART_1_TXD = 5,
	UART_1_RXD= 6,
	UART_1_CTS= 7,
	UART_1_RTS = 8,
	SPI_0_CLK = 9,
	SPI_0_CSN= 10,
	SPI_0_MOSI= 11,
	SPI_0_MISO = 12,
	SPI_1_CLK = 13,
	SPI_1_CSN= 14,
	SPI_1_MOSI = 15,
	SPI_1_MISO = 16,
	I2C_0_SCL = 17,
	I2C_0_SDA = 18,
	I2C_1_SCL = 19,
	I2C_1_SDA = 20,
	PWM_0 = 21,
	PWM_1 = 22,
	PWM_2 = 23,
	PWM_3 = 24,
	PWM_4 = 25,
	PWM_5 = 26,
	_7816_CLK = 27,
	_7816_DATA = 28,
	_7816_RST_N = 29,
	I2S_MCLK = 30,
	I2S_BCLK = 31,
	I2S_WCLK = 32,
	I2S_DATA = 33,
	IR = 34,
	MUTI_FUNC_MAX = 35
}GPIO_FUNC_DEFINE;


typedef enum
{
	GPIO_0 = (0x01<<0),
	GPIO_1 = (0x01<<1),
	GPIO_2 = (0x01<<2),
	GPIO_3 = (0x01<<3),
	GPIO_4 = (0x01<<4),
	GPIO_5 = (0x01<<5),
	GPIO_6 = (0x01<<6),
	GPIO_7 = (0x01<<7),
	GPIO_8 = (0x01<<8),
	GPIO_9 = (0x01<<9),
	GPIO_10 = (0x01<<10),
	GPIO_11 = (0x01<<11),
	GPIO_12 = (0x01<<12),
	GPIO_13 = (0x01<<13),
	GPIO_14 = (0x01<<14),
	GPIO_15 = (0x01<<15),
	GPIO_16 = (0x01<<16),
	GPIO_17 = (0x01<<17),
	GPIO_18 = (0x01<<18),
	GPIO_19 = (0x01<<19),
	GPIO_20 = (0x01<<20),
	GPIO_21 = (0x01<<21),
	GPIO_22 = (0x01<<22),
	GPIO_23 = (0x01<<23),
	GPIO_24 = (0x01<<24),
	GPIO_25 = (0x01<<25),
	GPIO_26 = (0x01<<26),
	GPIO_27 = (0x01<<27),
	GPIO_28 = (0x01<<28),
	GPIO_29 = (0x01<<29),
	GPIO_30 = (0x01<<30)
}GPIO_DEFINE;


typedef enum
{
	GPIO_INT_HIGH_LEVEL = 0x00,        /**< interrupt at high leavel */
	GPIO_INT_LOW_LEVEL = 0x01,         /**< interrupt at low leavel */
	GPIO_INT_RISING_EDGE = 0x02,       /**< interrupt at rising edge */
	GPIO_INT_FALLING_EDGE = 0x03,      /**< interrupt at falling edge */
	GPIO_INT_BOTH_EDGE = 0x04          /**< interrupt at both edge */   
}GPIO_INT_DEFINE;

typedef enum
{
	GPIO_WAKEUP_LOW_LEVEL = 0,     /**< weakup mcu when falling edge happaned */
	GPIO_WAKEUP_HIGH_LEVEL = 1,      /**< weakup mcu when rising edge happaned */
}GPIO_WAKEUP_LEVEL;


typedef union
{
	uint32 data;
	struct
	{
		uint8 func:6;            /**< gpio function select, see */
		uint8 output_en:1;       /**< input enable */
		uint8 input_en:1;        /**< output enable */
		uint8 pull_down_en:1;    /**< pull down enable */
		uint8 pull_up_en:1;      /**< pull up enable */
		uint8 slew_rate:1;       /**< 0-fast slew rate, 1-slow slew rate */
		uint8 drive_strength:1;  /**< 0-strong drive, 1-weak drive */
		uint8 cmos_schmit:1;     /**< 0-schmit trigger, 1-CMOS */
		uint8 nouse1:3;          /**< nouse */
		uint8 nouse2:8;          /**< nouse */
		uint8 nouse3:8;          /**< nouse */
	}bit;
}GPIO_CONFIG_STRU;

typedef enum
{
	GPIO_SCHMIT = 0x00,    /**< schmitt trigger input buffer */
	GPIO_CMOS = 0x01,      /**< cmos input buffer */
}GPIO_HW_TYPE;
typedef enum
{
	GPIO_DRIVE_STRENGTH_HIGH = 0x00,   /**< high driving strength */
	GPIO_DRIVE_STRENGTH_LOW  = 0x01    /**< low driving strength */
}GPIO_DRIVE_STRENGTH;
typedef enum
{
	GPIO_SLEW_RATE_FAST = 0x00, /**< slew rate fast */
	GPIO_SLEW_RATE_SLOW = 0x01  /**< slew rate slow */
}GPIO_SPEED_TYPE;
typedef enum
{
	GPIO_PULL_NONE = 0x00, /**< pull none */
	GPIO_PULL_DOWN = 0x01, /**< pull down */
	GPIO_PULL_UP   = 0x02  /**< pull up */
}GPIO_PULL_MODE;
typedef enum
{
	GPIO_MODE_HIGH_IMPEDANCE = 0x00, /**< high impedance io */
	GPIO_MODE_OUTPUT = 0x01,         /**< output io */
	GPIO_MODE_INPUT = 0x02           /**< input io */
}GPIO_MODE_TYPE;
typedef enum 
{
	GPIO_INT_CH0 = 0x00, /**< GPIO interrupt channel 0 */
	GPIO_INT_CH1 = 0x01, /**< GPIO interrupt channel 1 */
	GPIO_INT_CH2 = 0x02, /**< GPIO interrupt channel 2 */
	GPIO_INT_MAX = 0x03  /**< do not use */
}GPIO_INT_CHANNEL;


/**
* @brief  GPIO interrupt callback function define
* @param  interrupt states
* @return none
*/
typedef void (*GPIO_INT_CALLBACK)(uint32);
/**
* @brief  config gpio pins to input, in this function, gpio will be config with GPIO_PULL_NONE, GPIO_CMOS
* @param  gpio_pins [30:0]set some bits to 1 means config this pin to input
* @param  pupd internal pull up or pull down function, and also can be set not use
* @return none
*/
extern void gpio_init_input(uint32 gpio_pins, GPIO_PULL_MODE pupd);
/**
* @brief  config gpio pins to output, in this function, gpio will be config with GPIO_PULL_NONE, GPIO_SLEW_RATE_FAST, GPIO_DRIVE_LOW
* @param  gpio_pins [30:0]set some bits to 1 means config this pin to output
* @param  pupd internal pull up or pull down function, and also can be set not use
* @param  level set gpio default level
* @return none
*/
extern void gpio_init_output(uint32 gpio_pins, GPIO_PULL_MODE pupd, uint8 default_level);
/**
* @brief  config gpio pins to high impedance, in this function, gpio will be pull down
* @param  gpio_pins [30:0]set some bits to 1 means config this pin to high impedance
* @param  pupd internal pull up or pull down function, and also can be set not use
* @return none
*/
extern void gpio_init_high_impedance(uint32 gpio_pins, GPIO_PULL_MODE pupd);
/**
* @brief  initialize gpio interrupt
* @param  gpio_pins [30:0]set some bits to 1 means config this pin to interrupt pin
* @param  ch witch channel to handle interrupt,see enum GPIO_INT_CHANNEL
* @param  int_type interrupt trigger mode, see enum GPIO_INT_DEFINE
* @param  callback interrupt callback function
* @return none
*/
extern void gpio_init_interrupt(uint32 gpio_pins, GPIO_INT_CHANNEL ch, GPIO_INT_DEFINE int_type, GPIO_INT_CALLBACK callback);
/**
* @brief  enable gpio interrupt
* @param  gpio_pins [30:0]set some bits to 1 means enable the pin interrupt
* @return none
*/
extern void gpio_enable_interrupt(uint32 gpio_pins);
/**
* @brief  disable gpio interrupt
* @param  gpio_pins [30:0]set some bits to 1 means disable the pin interrupt
* @return none
*/
extern void gpio_disable_interrupt(uint32 gpio_pins);
/**
* @brief  clear old config and config gpio use user defined
* @param  cfg config struct
* @param  gpio_pins [30:0]set some bits to 1 means initilaze it
* @return none
*/
extern void gpio_init(uint32 gpio_pins, GPIO_CONFIG_STRU cfg);
/**
* @brief  config gpio hardware type, CMOS or SCHMIT
* @param  gpio_pins [30:0]set some bits to 1 means config this pin
* @param  type hardware type GPIO_SCHMIT or GPIO_CMOS
* @return none
*/
extern void gpio_config_hw_type(uint32 gpio_pins, GPIO_HW_TYPE type);
/**
* @brief  config gpio drive strength
* @param  gpio_pins [30:0]set some bits to 1 means config this pin
* @param  strength drive strength
* @return none
*/
extern void gpio_config_drive_strength(uint32 gpio_pins, GPIO_DRIVE_STRENGTH strength);
/**
* @brief  config gpio slew rate
* @param  gpio_pins [30:0]set some bits to 1 means config this pin
* @param  speed slew rate
* @return none
*/
extern void gpio_config_slew_rate(uint32 gpio_pins, GPIO_SPEED_TYPE speed);
/**
* @brief  config gpio pull up or pull down, this function will not clear old config, just modify
* @param  gpio_pins [30:0]set some bits to 1 means config this pin to high impedance
* @param  pupd pull mode select
* @return none
*/
extern void gpio_config_pupd(uint32 gpio_pins, GPIO_PULL_MODE pupd);
/**
* @brief  get all gpio input level
* @param  none
* @return gpio level,[30:0]used
*/
extern uint32 gpio_read_input(void);
/**
* @brief  get one gpio input level
* @param  GPIO_n gpio select
* @return 0-low level, 1-high level
*/
extern bool gpio_read_input_bit(GPIO_DEFINE gpio_n);
/**
* @brief  get gpio output level
* @param  none
* @return gpio level,[30:0]used
*/
extern uint32 gpio_read_output(void);
/**
* @brief  get one gpio output level
* @param  gpio_n gpio select
* @return 0-low level, 1-high level
*/
extern bool gpio_read_output_bit(GPIO_DEFINE gpio_n);
/**
* @brief  set gpio_n to high level
* @param  gpio_n gpio select
* @return none
*/
extern void gpio_set_bit(GPIO_DEFINE gpio_n);
/**
* @brief  set gpios to high level
* @param  gpio_pins [30:0]if the bits value is 1, set the gpio_n to high level
* @return none
*/
extern void gpio_set_data(uint32 gpio_pins);
/**
* @brief  reset the gpio_n to output low level
* @param  gpio_n gpio select
* @return none
*/
extern void gpio_clear_bit(GPIO_DEFINE gpio_n);
/**
* @brief  reset the gpios to output low level
* @param  gpio_pins gpios select, [30:0]one bit for one gpio pin
* @return none
*/
extern void gpio_clear_data(uint32 gpio_pins);
/**
* @brief  gpio pin function select
* @param  gpio_n gpios select
* @param  func function define
* @return none
*/
extern void gpio_init_function(GPIO_DEFINE gpio_n, GPIO_FUNC_DEFINE func);

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_H_ */

