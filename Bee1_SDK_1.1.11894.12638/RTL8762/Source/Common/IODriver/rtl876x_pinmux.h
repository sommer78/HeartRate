/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_pinmux.h
* @brief     
* @details   
* @author    Chuanguo Xue
* @date      2015-3-27
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_PINMUX_H_
#define _RTL876X_PINMUX_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "legacyperiphapi.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup PINMUX PINMUX
  * @brief Pinmux driver module
  * @{
  */

/** @defgroup PINMUX_Exported_Types PINMUX Exported Types
  * @{
  */


/**
  * @}
  */

/** @defgroup PINMUX_Exported_Constants PINMUX Exported Constants
  * @{
  */

/** @defgroup Pin_Function_Number Pin Function Number
  * @{
  */
  
#define IDLE_MODE       0
#define HCI_UART_TX     1
#define HCI_UART_RX     2
#define HCI_UART_CTS    3
#define HCI_UART_RTS    4
#define I2C0_CLK        5
#define I2C0_DAT        6
#define PIN_NOUSE0      7
#define PIN_NOUSE1      8
#define I2C1_CLK        9
#define I2C1_DAT        10
#define PIN_NOUSE2      11
#define PIN_NOUSE3      12
#define timer_pwm0      13
#define timer_pwm1      14
#define timer_pwm2      15
#define timer_pwm3      16
#define timer_ete0      17
#define timer_ete1      18
#define timer_ete2      19
#define timer_ete3      20
#define qdec_phase_a_x  21
#define qdec_phase_b_x  22
#define qdec_phase_a_y  23
#define qdec_phase_b_y  24
#define qdec_phase_a_z  25
#define qdec_phase_b_z  26
#define LOG_UART_TX     27
#define LOG_UART_RX     28
#define IRDA_TX         29
#define IRDA_RX         30
#define RTC_CLK_OUT     31
#define DATA_UART_TX    32
#define DATA_UART_RX    33
#define DATA_UART_CTS   34
#define DATA_UART_RTS   35
#define SPI1_SS_N_0_Master  36
#define SPI1_SS_N_1_Master  37
#define SPI1_SS_N_2_Master  38
#define SPI1_CLK_Master     39
#define SPI1_MO_Master      40
#define SPI1_MI_Master      41
#define SPI0_SS_N_0_Slave   42
#define SPI0_CLK_0_Slave    43
#define SPI0_SO_0_Slave     44
#define SPI0_SI_0_Slave     45
#define SPI0_SS_N_0_Master  46
#define SPI0_CLK_Master     47
#define SPI0_MO_Master      48
#define SPI0_MI_Master      49
#define SPI2W_DATA_Master   50
#define SPI2W_CLK_Master    51
#define SPI2W_CS_Master     52
#define SWD_CLK         53
#define SWD_DIO         54
#define KEY_COL_0       55
#define KEY_COL_1       56
#define KEY_COL_2       57
#define KEY_COL_3       58
#define KEY_COL_4       59
#define KEY_COL_5       60
#define KEY_COL_6       61
#define KEY_COL_7       62
#define KEY_COL_8       63
#define KEY_COL_9       64
#define KEY_COL_10      65
#define KEY_COL_11      66
#define KEY_COL_12      67
#define KEY_COL_13      68
#define KEY_COL_14      69
#define KEY_COL_15      70
#define KEY_COL_16      71
#define KEY_COL_17      72
#define KEY_COL_18      73
#define KEY_COL_19      74
#define KEY_ROW_0       75
#define KEY_ROW_1       76
#define KEY_ROW_2       77
#define KEY_ROW_3       78
#define KEY_ROW_4       79
#define KEY_ROW_5       80
#define KEY_ROW_6       81
#define KEY_ROW_7       82
#define DMIC_DATA       83
#define DMIC_CLK        84
#define GPIO_FUN        85
#define BT_3DG_L1       86
#define BT_3DG_L2       87
#define BT_3DG_R1       88
#define BT_3DG_R2       89

/**
  * @}
  */

/** @defgroup Pin_Number Pin Number
  * @{
  */
  
#define P0_0    0
#define P0_1    1
#define P0_2    2
#define P0_3    3
#define P0_4    4
#define P0_5    5
#define P0_6    6
#define P0_7    7
#define P1_0    8
#define P1_1    9
#define P1_2    10
#define P1_3    11
#define P1_4    12
#define P1_5    13
#define P1_6    14
#define P1_7    15
#define P2_0    16
#define P2_1    17
#define P2_2    18
#define P2_3    19
#define P2_4    20
#define P2_5    21
#define P2_6    22
#define P2_7    23
#define P3_0    24
#define P3_1    25
#define P3_2    26
#define P3_3    27
#define P3_4    28
#define P3_5    29
#define P3_6    30
#define P3_7    31
#define P4_0    32
#define P4_1    33
#define P4_2    34
#define P4_3    35
#define P4_4    36
#define P4_5    37

/**
  * @}
  */

/** @defgroup GPIO_Number GPIO Number
  * @{
  */
  
#define GPIO0   P0_0
#define GPIO1   P0_1
#define GPIO2   P0_2
#define GPIO3   P0_3
#define GPIO4   P0_4
#define GPIO5   P0_5
#define GPIO6   P0_6
#define GPIO7   P0_7
#define GPIO8   P1_0
#define GPIO9   P1_1
#define GPIO10  P1_2
#define GPIO11  P1_3
#define GPIO12  P1_4
#define GPIO13  P1_5
#define GPIO14  P1_6
#define GPIO15  P1_7
#define GPIO16  P2_0
#define GPIO17  P2_1
#define GPIO18  P2_2
#define GPIO19  P2_3
#define GPIO20  P2_4
#define GPIO21  P2_5
#define GPIO22  P2_6
#define GPIO23  P2_7
#define GPIO24  P3_0
#define GPIO25  P3_1
#define GPIO26  P3_2
#define GPIO27  P3_3
#define GPIO28  P4_0
#define GPIO29  P4_1
#define GPIO30  P4_2
#define GPIO31  P4_3

/**
  * @}
  */

/** @defgroup PAD_Pull_Mode PAD Pull Mode
  * @{
  */
  
typedef enum _PAD_Pull_Mode
{
    PAD_PULL_NONE,
    PAD_PULL_UP,
    PAD_PULL_DOWN
} PAD_Pull_Mode;


/**
  * @}
  */

/** @defgroup PAD_Mode PAD Mode
  * @{
  */

typedef enum _PAD_Mode
{
    PAD_SW_MODE,
    PAD_PINMUX_MODE
} PAD_Mode;

/**
  * @}
  */

/** @defgroup PAD_Power_Mode PAD Power Mode
  * @{
  */
  
typedef enum _PAD_PWR_Mode
{
    /* PAD_NOT_PWRON, */  /* Reserved, not available in APP */
    PAD_IS_PWRON = 1
} PAD_PWR_Mode;

/**
  * @}
  */

/** @defgroup PAD_Outout_Config PAD Outout Config
  * @{
  */
  
typedef enum _PAD_OUTPUT_ENABLE_Mode
{
    PAD_OUT_DISABLE,
    PAD_OUT_ENABLE
} PAD_OUTPUT_ENABLE_Mode;

/**
  * @}
  */

/** @defgroup PAD_Outout_Value PAD Outout Value
  * @{
  */
  
typedef enum _PAD_OUTPUT_VAL
{
    PAD_OUT_LOW,
    PAD_OUT_HIGH
} PAD_OUTPUT_VAL;

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
  * @{
  */
  
extern void Pinmux_Reset(void);
extern void Pinmux_Deinit(uint8_t Pin_Num);
extern void Pinmux_Config(uint8_t Pin_Num, uint8_t Pin_Func);
extern void Pad_Config( uint8_t Pin_Num,
                        BOOL AON_PAD_Mode,
                        BOOL AON_PAD_PwrOn,
                        BOOL AON_PAD_Pull, 
                        BOOL AON_PAD_E, 
                        BOOL AON_PAD_O);
extern void All_Pad_Config_Default(void);
extern void Log_SWD_DLPS_Enter(void);
extern void Log_SWD_DLPS_Exit(void);
extern void System_WakeUp_Pin_Enable(uint8_t Pin_Num, uint8_t Polarity);
extern void System_WakeUp_Pin_Disable(uint8_t Pin_Num);

__STATIC_INLINE void System_WakeUp_Intr_Enable(void)
{
    uint8_t value8 = bee_aon_gp_read(REG_PAD_WK_CTRL_ADDRESS);
    value8 |= BIT_WK_INTEN;
    bee_aon_gp_write(REG_PAD_WK_CTRL_ADDRESS, value8);
}

__STATIC_INLINE void System_WakeUp_Intr_Disable(void)
{
    uint8_t value8 = bee_aon_gp_read(REG_PAD_WK_CTRL_ADDRESS);
    value8 &= ~BIT_WK_INTEN;
    bee_aon_gp_write(REG_PAD_WK_CTRL_ADDRESS, value8);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_PINMUX_H_ */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor *****END OF FILE****/

