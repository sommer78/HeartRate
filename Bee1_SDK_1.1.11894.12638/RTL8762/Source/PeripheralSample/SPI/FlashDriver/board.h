/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     board.h
* @brief        Pin definitions
* @details
* @author   Chuanguo Xue
* @date     2015-4-7
* @version  v0.1
* *********************************************************************************************************
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif
#define CONFIG_DLPS_EN  0

/* User defines---------------------------------------------*/
#define FLASH_SCK				P4_0
#define FLASH_MOSI				P4_1
#define FLASH_MISO				P4_2
#define FLASH_CS				P4_3
#define FLASH_SPI				SPI0

/** @defgroup IO Driver Config
  * @note user must config it firstly!! Do not change macro names!!
  * @{
  */

/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_EXIT_CB      0
#define USE_USER_DEFINE_DLPS_ENTER_CB     0

/* if use any peripherals below, #define it 1 */
#define USE_I2C0_DLPS        0
#define USE_I2C1_DLPS        0
#define USE_TIM_DLPS         0
#define USE_QDECODER_DLPS    0
#define USE_IR_DLPS          0
#define USE_RTC_DLPS         0
#define USE_UART_DLPS        0
#define USE_ADC_DLPS         0
#define USE_SPI0_DLPS        0
#define USE_SPI1_DLPS        0
#define USE_SPI2W_DLPS       0
#define USE_KEYSCAN_DLPS     0
#define USE_DMIC_DLPS        0
#define USE_GPIO_DLPS        0
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

#define USE_GDMA_DLPS               (USE_GDMACHANNEL0_DLPS | USE_GDMACHANNEL1_DLPS | USE_GDMACHANNEL2_DLPS\
                                    | USE_GDMACHANNEL3_DLPS | USE_GDMACHANNEL4_DLPS | USE_GDMACHANNEL5_DLPS\
                                        | USE_GDMACHANNEL6_DLPS)

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

