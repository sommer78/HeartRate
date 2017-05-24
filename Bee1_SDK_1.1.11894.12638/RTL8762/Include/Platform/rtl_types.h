/**
************************************************************************************************************
*               Copyright(c) 2014-2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     rtl_types.h
* @brief    Basic types definition.
* @author   lory_xu
* @date     2014-05
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __RTL_TYPES_H__
#define __RTL_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include "compiler_abstraction.h"

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Basic Basic Definition
  * @{
  */

/** @addtogroup Platform_Basic_Data_Type Data Type
  * @brief Basic Data Type Definition
  * @{
  */

#ifndef CONST
#define CONST const
#endif

#ifndef EXTERN
#define EXTERN extern
#endif

#ifndef INLINE
#define INLINE  inline
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef NULL
#define NULL    ((void*)0)
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAIL
#define FAIL    (-1)
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef _TRUE
#define _TRUE  TRUE
#endif

#ifndef _FALSE
#define _FALSE FALSE
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef ON
#define ON        1
#endif

#ifndef OFF
#define OFF       0
#endif

#if 0
#ifndef ENABLE
#define ENABLE    1
#endif

#ifndef DISABLE
#define DISABLE   0
#endif
#endif

typedef enum
{
    DISABLE = 0,
    ENABLE = 1
} FunctionalState;

typedef unsigned char                   BOOL;

typedef void                            VOID, *PVOID;

typedef unsigned char                   UCHAR;
typedef unsigned char                   UINT8;
typedef unsigned short                  UINT16;
typedef unsigned long                   UINT32;
typedef unsigned long long              UINT64;

typedef signed char                     CHAR;
typedef signed char                     INT8;
typedef signed short                    INT16;
typedef signed long                     INT32;
typedef signed long long                INT64;

/** End of Platform_Basic_Data_Type
  * @}
  */

/** End of Platform_Basic
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* __RTL_TYPES_H__ */
