/**
************************************************************************************************************
*               Copyright(c) 2014-2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     rtl_assert.h
* @brief    Assert definition
* @author   lory_xu
* @date     2015-03
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __RTL_ASSERT_H__
#define __RTL_ASSERT_H__

#include "rtl_types.h"

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Basic Basic Definition
  * @{
  */

/** @addtogroup Platform_Basic_Assert Assert
  * @details Assert definition
  * @{
  */

#if defined(DEBUG_BEE)

VOID AssertCallback(UINT16 line_num, const UINT8 *file_name);

#define RTL_ASSERT(x) \
    if (x)                                                                     \
    {                                                                             \
    }                                                                             \
    else                                                                          \
    {                                                                             \
        AssertCallback((UINT16)__LINE__, (UINT8 *)__FILE__);               \
    }
#else

#define RTL_ASSERT(x)

#endif

/** End of Platform_Basic_Assert
  * @}
  */

/** End of Platform_Basic
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* __RTL_ASSERT_H__ */
