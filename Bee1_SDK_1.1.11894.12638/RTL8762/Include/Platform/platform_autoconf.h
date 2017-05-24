/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     platform_autoconf.h
* @brief    Platform configuration
* @author   lory xu
* @date     2015-06-29
* @version  v0.1
*************************************************************************************************************
*/
#ifndef _PLATFORM_AUTOCONF_H_
#define _PLATFORM_AUTOCONF_H_

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Build_Configuration Build Configuration
  * @{
  */

/** @brief Enable DLPS or not */
//#define  CONFIG_DLPS_EN 1
#undef  CONFIG_FPGA

/** @brief Use eFlash on board or not. FPGA doesn't have eFlash */
#ifdef CONFIG_FPGA
#undef  CONFIG_EFLASH_BOARD_EXIST
#else
#define CONFIG_EFLASH_BOARD_EXIST 1
#endif

/** End of Platform_Build_Configuration
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* End of _PLATFORM_AUTOCONF_H_ */
