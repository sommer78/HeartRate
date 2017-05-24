/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     board.h
* @brief        Pin definitions
* @details
* @author   howie
* @date     2015-10-28
* @version  v0.1
* *********************************************************************************************************
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif
#define CONFIG_DLPS_EN 1

	
#define Monitoring_voltage_Pin  P4_3

	
/* event */
#define IO_DEMO_EVENT_ADC_CONVERT_END          0x01
#define IO_DEMO_EVENT_BATTERY_LEVEL_LOW         0x02
#define IO_DEMO_EVENT_UART_RX           0x11
/* globals */
extern xTaskHandle IODemoTaskHandle;
extern xQueueHandle DemoIOMessageQueue;
extern xQueueHandle DemoLPCMessageQueue;
extern xQueueHandle DemoADCMessageQueue;
extern xQueueHandle DemoIOEventQueue;	
	
	
extern unsigned char Float2Char(float value,unsigned char *array);
extern void serial_puts(const char *s);
extern void print_uint16(uint16_t num);
extern void print_uint32(uint32_t num);
extern void loop_threshold(void);



extern uint16_t global_count_for_lpc;//better not use global
	

/** @defgroup IO Driver Config
  * @note user must config it firstly!! Do not change macro names!!
  * @{
  */

/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_EXIT_CB      1
#define USE_USER_DEFINE_DLPS_ENTER_CB     1

/* if use any peripherals below, #define it 1 */
#define USE_I2C0_DLPS        0
#define USE_I2C1_DLPS        0
#define USE_TIM_DLPS         1
#define USE_QDECODER_DLPS    0
#define USE_IR_DLPS          0
#define USE_RTC_DLPS         1
#define USE_UART_DLPS        1
#define USE_ADC_DLPS         1
#define USE_SPI0_DLPS        0
#define USE_SPI1_DLPS        0
#define USE_SPI2W_DLPS       0
#define USE_KEYSCAN_DLPS     0
#define USE_DMIC_DLPS        0
#define USE_GPIO_DLPS        1
#define USE_PWM0_DLPS        0
#define USE_PWM1_DLPS        0
#define USE_PWM2_DLPS        0
#define USE_PWM3_DLPS        0

#define USE_GDMACHANNEL0_DLPS       0
#define USE_GDMACHANNEL1_DLPS       0
#define USE_GDMACHANNEL2_DLPS       0
#define USE_GDMACHANNEL3_DLPS       0
#define USE_GDMACHANNEL4_DLPS       0
#define USE_GDMACHANNEL5_DLPS       0
#define USE_GDMACHANNEL6_DLPS       0

/* do not modify USE_IO_DRIVER_DLPS macro */
#define USE_IO_DRIVER_DLPS   (USE_I2C0_DLPS | USE_I2C1_DLPS | USE_TIM_DLPS | USE_QDECODER_DLPS\
                              | USE_IR_DLPS | USE_RTC_DLPS | USE_UART_DLPS | USE_SPI0_DLPS\
                              | USE_SPI1_DLPS | USE_SPI2W_DLPS | USE_KEYSCAN_DLPS | USE_DMIC_DLPS\
                              | USE_GPIO_DLPS | USE_USER_DEFINE_DLPS_EXIT_CB | USE_GDMACHANNEL0_DLPS\
                              | USE_GDMACHANNEL1_DLPS | USE_GDMACHANNEL2_DLPS | USE_GDMACHANNEL3_DLPS \
                              | USE_GDMACHANNEL4_DLPS | USE_GDMACHANNEL5_DLPS | USE_GDMACHANNEL6_DLPS\
                              | USE_RTC_DLPS | USE_PWM0_DLPS | USE_PWM1_DLPS | USE_PWM2_DLPS\
                              | USE_PWM3_DLPS | USE_USER_DEFINE_DLPS_ENTER_CB)

/**
  * @}
  */
#ifdef __cplusplus
}
#endif


#endif /* _BOARD_H_ */

