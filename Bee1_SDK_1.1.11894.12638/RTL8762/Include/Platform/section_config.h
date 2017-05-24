/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     section_config.h
* @brief    Memory section.
* @author   lory_xu
* @date     2014-05
* @version  v0.1
*************************************************************************************************************
*/

#ifndef _SECTION_CONFIG_H_
#define _SECTION_CONFIG_H_

#include "rtl_types.h"

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Toolchain Toolchain
  * @{
  */

/** @addtogroup Platform_Toolchain_Linker Linker Support
  * @{
  */

/** @cond private */
/** @brief  Patch Header Struct */
typedef struct
{
    UINT16 Offset;          /* real offset = offset >> 2 */
    UINT16 Signature;
    UINT16 BuildVersion;
    UINT16 CheckSum;
    UINT16 Length;          /* real Length = Length >> 2 */
    UINT8 ota_flag;         /* default: 0xff */
    UINT8 UserDataPageIndex;         /* default 0xff: invalid; others: valid page index */
} PATCH_HEADER;

/** @brief Patch Version Header Struct */
typedef struct
{
    const char UTS_Version[28];
    uint8_t MajorVersion;
    uint8_t MinorVersion;
    uint16_t RevisionVersion;
} TIMESTAMP_VER_HEADER;

/********** User  Flash   **********/
/** @brief Put the .user.flash.header symbol in Flash. */
#define USER_FLASH_HEADER    SECTION(".user.flash.header")
/** @brief Put the .user.flash.timestamp symbol in Flash. */
#define USER_FLASH_TIMESTAMP SECTION(".user.flash.timestamp")
/** @brief Put the .user.flash.TestMode.flag symbol in Flash. */
#define USER_FLASH_TEST_MODE_FLAG SECTION(".user.flash.TestMode.flag")
/** @endcond */

/***************************************  Ram   *****************************************/
/** @brief  Put the .data symbol in data off memory. */
#define SRAM_OFF_BD_DATA_SECTION SECTION(".bdsram.off.data")
/** @brief  Put the .bss symbol in data off memory. */
#define SRAM_OFF_BD_BSS_SECTION SECTION_ZI(".bdsram.off.bss")

/***************************************  TRACE  *****************************************/
/** @brief Put .TRACE symbol in .TRACE section to reduce code size. */
#define TRACE_DATA SECTION(".TRACE") __attribute__((aligned(4)))

/** End of Platform_Toolchain_Linker
  * @}
  */

/** End of Platform_Toolchain
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* _SECTION_CONFIG_H_ */
