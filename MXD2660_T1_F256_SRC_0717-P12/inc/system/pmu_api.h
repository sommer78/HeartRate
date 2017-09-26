/**
****************************************************************************
* @file pmu_api.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2017/1/22
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
/** @defgroup 模块名 模块的说明文字
* @{
*/
#ifndef __PMU_CTRL_H__
#define __PMU_CTRL_H__

#include "mxd2660_gpio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum SLEEP_LEVEL
{
    ACTIVE = 0,
    
    IDLE = 1,    

    SLEEP = 2,

    DEEP_SLEEP = 3,

    SHUTDOWN = 4,

    
}SLEEP_LEVEL_ENUM;



/** 枚举常量 */
typedef enum CLK_16M_SEL
{
    CLK_16M = 0,
    CLK_16M_DIVI2 = 1,
    CLK_16M_DIVI3 = 2,
}CLK_16M_SEL_ENUM;
/** 定义类型 CLK_16M_SEL_ENUM */

/** 枚举常量 */
typedef enum FLASH_CLK_SET
{
    FLASH_1M = 0,
    FLASH_2M = 1,
    FLASH_4M = 2,
    FLASH_8M = 3,
    FLASH_16M = 4,
    FLASH_32M = 8,
    FLASH_48M = 0xc,
}FLASH_CLK_SET_ENUM;
/** 定义类型 FLASH_CLK_SET_ENUM */

/** 枚举常量 */
typedef enum POWER_MODE
{
    POWER_MODE_DCDC = 1,
    POWER_MODE_LDO = 2,
}POWER_MODE_ENUM;
/** 定义类型 POWER_MODE_ENUM */

/** 枚举常量 */
typedef enum RADIO_RX_SENSITIVITY
{
    Rx_neg95_dbm = 0, 
    Rx_neg92_dbm = 1, 
}RADIO_RX_SENSITIVITY_ENUM;
/** 定义类型 RADIO_RX_SENSITIVITY_ENUM */

/** 枚举常量 */
typedef enum RADIO_TX_POWER
{
    TX_neg15_dbm = 0,   //≈-15dbm
    TX_neg12_dbm = 1,   //≈-12dbm
    TX_neg10_dbm = 2,   // -10dbm
    TX_neg5_dbm = 3,    //-5dbm
    TX_neg2_dbm = 4,    //-3dbm
    TX_0_dbm = 5,       //0 dbm 
        
}RADIO_TX_POWER_ENUM;
/** 定义类型 RADIO_TX_POWER_ENUM */

/**<若用户选择大于0DB,则需要重新编译并且下载代码，重新运行*/
typedef enum __RF_DB_SEL
{
    RF_0DB_CFG = 0,
    RF_4DB_CFG = 1,
    RF_7DB_CFG = 2,
}RF_DB_SEL;

/** 枚举常量 */
typedef enum RESET_SOURCE
{
    RESET_FOR_POR_OR_WDT = 0,
	RESET_FOR_SHUTDOWN = 1,
}RESET_SOURCE_ENUM;
/** 定义类型 RESET_SOURCE_ENUM */


//function list

/**
* @brief  disable gpio wakeup cpu.
* @param gpio_pins:  bits map
* @return N/A
*/
extern void system_pins_wakeup_disable(uint32 gpio_pins);
/**
* @brief  enable gpios wakeup cpu.
* @param pins:  gpios bit map
* @param Polarity:   0 ,gpio low wakeup ，1, gpio high wakeup
* @return N/A
*/
extern void system_pins_wakeup_enable(uint32 gpio_pins, GPIO_WAKEUP_LEVEL Polarity);

/**
* @brief set mcu low power mode
* @param low_pwr_mode: 0,alway on ,1 idle,  2or 3 sleep, 4, shutdown;
* @return N/A
*/
extern void system_set_low_power_mode(SLEEP_LEVEL_ENUM low_pwr_mode);

/**
* @brief set radio power level 
* @param radio_pwr_mode:  reserved
* @return N/A
*/
extern void system_set_radio_mode(RADIO_RX_SENSITIVITY_ENUM radio_pwr_mode);

/**
* @brief dcdc and ldo mmode sel
* @param mode:  1:POWER_MODE_DCDC 2:POWER_MODE_LDO
* @return N/A
*/
extern void system_power_dcdc_mode_set(POWER_MODE_ENUM mode);

/**
* @brief set radio power level 
* @param radio_pwr_mode:  reserved
* @return N/A
*/
extern void system_set_radio_tx_power(RADIO_TX_POWER_ENUM radio_tx_power);

/**
* @brief get System reset reason 
* @param N/A
* @return  RESET_SOURCE_ENUM
*/
extern RESET_SOURCE_ENUM system_get_reset_source(void);

/**
* @brief user to adjust system frequency deviation
* @param uint8 cap tune value
* @return N/A
*/
extern void system_set_freq_accuracy(uint8 cap);

/**<若用户选择大于0DB,则需要重新编译并且下载代码，重新运行*/
extern void system_set_rf_tx_power(RF_DB_SEL rf_db);

/**
* @brief system reset
* @param NONE
* @return NONE
*/
extern void system_reset(void);

/**
* @brief software reset
* @param NONE
* @return NONE
*/
extern void software_reset(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PMU_CTRL_H__ */
/** @} */ // 模块结尾
