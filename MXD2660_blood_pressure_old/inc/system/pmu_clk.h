/**
****************************************************************************
* @file pmu_clk.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2017/2/13
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
#ifndef __PMU_CLK_H__
#define __PMU_CLK_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/** 枚举常量 */
typedef enum HIGH_CLK_SRC
{
    RC_16M = 1,
    PLL_32M,
    PLL_48M,
    XO_16M_DCXO,
    XO_32M_DCXO,    /* reserved*/
    XO_16M_EXT,     /* reserved*/
    XO_32M_EXT,     /* reserved*/
}HIGH_CLK_SRC_ENUM;
/** 定义类型 HIGH_CLK_SRC_ENUM */

/** 枚举常量 */
typedef enum LOW_CLK_SRC
{
    RC_32K = 0,
    XO_32K = 1,
}LOW_CLK_SRC_ENUM;
/** 定义类型 LOW_CLK_SRC_ENUM */

/** 枚举常量 */
typedef enum XO_SOURCE
{
    XO_FROM_16M_RADIO = 0,
    XO_FROM_32M_RADIO = 1,  /* reserved*/
    XO_FROM_16M_EXT = 2,    /* reserved*/
    XO_FROM_32M_EXT = 3,    /* reserved*/
}XO_SOURCE_ENUM;
/** 定义类型 XO_SOURCE_ENUM */

/**
* @brief set system 32k clock source
* @param clk_32k_src: 1 select XO, 0 select 32kRc
* @return  success return 0
*/
extern uint8 system_32k_clk_select(LOW_CLK_SRC_ENUM clk_32k_src);

/**
* @brief	system high clock source select
* @param  HIGH_CLK_SRC_ENUM
* @return  success return 0
*/
extern uint8 system_high_clk_select(HIGH_CLK_SRC_ENUM high_clk_src);

/**
* @brief:	Used to control the clock. Advice is opened entirely.Finally release closed without the use of the clock.
* @param Please see the API documentation
* @return N/A
*/
extern void system_clk_gate(uint32 bitmap);

extern void system_dcxo_clk_check_for_ltc(void);

/**
* @brief	According to the hardware configuration, choosing the right parameters
* @param XO_SOURCE_ENUM
* @return  success return 0
*/
extern uint8 system_high_clk_set_xo_source(XO_SOURCE_ENUM xo_src);

/**
* @brief Select the system clock 
* @param 
* @return success return 0
*/
extern uint8 system_set_cpu_clk (uint8 clk_freq);

/**
* @brief set 32k xo clock ppm
* @param  ppm value
* @return N/A
*/
extern void system_set_32kxo_ppm(uint16 xo_32k_ppm);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PMU_CLK_H__ */
/** @} */ // 模块结尾
