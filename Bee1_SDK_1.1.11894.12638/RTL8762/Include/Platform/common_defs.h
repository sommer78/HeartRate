/**
************************************************************************************************************
*               Copyright(c) 2014-2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     common_defs.h
* @brief    bit and other normal used common definition
* @author   lory_xu
* @date     2015-03
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#include "rtl_types.h"

/** @addtogroup Platform RTK Plaftorm
  * @{
  */

/** @addtogroup Platform_Basic Basic Definition
  * @{
  */

/** @addtogroup Platform_Basic_Bit_And_Common_Macro Bit & Common Macro
  * @details Bit constants and common used macro
  * @{
  */

#define BIT0        0x00000001
#define BIT1        0x00000002
#define BIT2        0x00000004
#define BIT3        0x00000008
#define BIT4        0x00000010
#define BIT5        0x00000020
#define BIT6        0x00000040
#define BIT7        0x00000080
#define BIT8        0x00000100
#define BIT9        0x00000200
#define BIT10       0x00000400
#define BIT11       0x00000800
#define BIT12       0x00001000
#define BIT13       0x00002000
#define BIT14       0x00004000
#define BIT15       0x00008000
#define BIT16       0x00010000
#define BIT17       0x00020000
#define BIT18       0x00040000
#define BIT19       0x00080000
#define BIT20       0x00100000
#define BIT21       0x00200000
#define BIT22       0x00400000
#define BIT23       0x00800000
#define BIT24       0x01000000
#define BIT25       0x02000000
#define BIT26       0x04000000
#define BIT27       0x08000000
#define BIT28       0x10000000
#define BIT29       0x20000000
#define BIT30       0x40000000
#define BIT31       0x80000000

#define BIT(_n)   (UINT32)(1U<<(_n))


#ifndef MSB
/** @brief The upper 8 bits of a 16 bit value */
#define MSB(a) (((a) & 0xFF00) >> 8)
#endif

#ifndef LSB
/** @brief The lower 8 bits of a 16 bit value */
#define LSB(a) ((a) & 0xFF)
#endif


#ifndef MIN
/** @brief Computes the minimum of \a n and \a m. */
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
/** @brief Computes the maximum of \a n and \a m. */
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

/** @brief Mark paramter \a X as unused */
#define UNUSED_PARAMETER(X) ((VOID)(X))

/** End of Platform_Basic_Bit_And_Common_Macro
  * @}
  */

/** End of Platform_Basic
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* __COMMON_DEFS_H__ */
