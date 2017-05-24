/**
************************************************************************************************************
*               Copyright(c) 2014-2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     rtl_endian.h
* @brief    Endian related definition and function.
* @author   lory_xu
* @date     2014-05
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __RTL_ENDIAN_H__
#define __RTL_ENDIAN_H__

#include "rtl_types.h"

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Basic Basic Definition
  * @{
  */

/** @addtogroup Platform_Basic_Endian Endian
  * @details Endian related macro
  * @{
  */

#define LITTLE_ENDIAN                           0
#define BIG_ENDIAN                              1

#define SYSTEM_ENDIAN                           LITTLE_ENDIAN

#define SWAP32(x) ((UINT32)(                         \
                   (((UINT32)(x) & (UINT32)0x000000ff) << 24) |            \
                   (((UINT32)(x) & (UINT32)0x0000ff00) <<  8) |            \
                   (((UINT32)(x) & (UINT32)0x00ff0000) >>  8) |            \
                   (((UINT32)(x) & (UINT32)0xff000000) >> 24)))

#define WAP16(x) ((UINT16)(                         \
                  (((UINT16)(x) & (UINT16)0x00ff) <<  8) |            \
                  (((UINT16)(x) & (UINT16)0xff00) >>  8)))

#if SYSTEM_ENDIAN == LITTLE_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)      ((UINT32)(x))
#define rtk_le32_to_cpu(x)      ((UINT32)(x))
#define rtk_cpu_to_le16(x)      ((UINT16)(x))
#define rtk_le16_to_cpu(x)      ((UINT16)(x))
#define rtk_cpu_to_be32(x)      SWAP32((x))
#define rtk_be32_to_cpu(x)      SWAP32((x))
#define rtk_cpu_to_be16(x)      WAP16((x))
#define rtk_be16_to_cpu(x)      WAP16((x))
#endif

#elif SYSTEM_ENDIAN == BIG_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)      SWAP32((x))
#define rtk_le32_to_cpu(x)      SWAP32((x))
#define rtk_cpu_to_le16(x)      WAP16((x))
#define rtk_le16_to_cpu(x)      WAP16((x))
#define rtk_cpu_to_be32(x)      ((UINT32)(x))
#define rtk_be32_to_cpu(x)      ((UINT32)(x))
#define rtk_cpu_to_be16(x)      ((UINT16)(x))
#define rtk_be16_to_cpu(x)      ((UINT16)(x))
#endif
#endif

/** End of Platform_Basic_Endian
  * @}
  */

/** End of Platform_Basic
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* __RTL_ENDIAN_H__ */
