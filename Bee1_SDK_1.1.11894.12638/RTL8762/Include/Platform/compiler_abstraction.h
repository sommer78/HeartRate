/**
************************************************************************************************************
*               Copyright(c) 2014-2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     compiler_abstraction.h
* @brief    Compiler specific intrinsics
* @author   lory_xu
* @date     2015-03
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __COMPILER_ABSTRACTION_H__
#define __COMPILER_ABSTRACTION_H__

/** @addtogroup Platform
  * @brief Platform Manual
  * @{
  */

/** @addtogroup Platform_Toolchain Toolchain
  * @{
  */

/** @addtogroup Platform_Toolchain_Compiler Compiler Support
  * @{
  */

/** @brief Define compilor specific symbol */

#if defined ( __CC_ARM )
#define __ASM                   __asm                                      /*!< asm keyword for ARM Compiler          */
#define __INLINE                __inline                                   /*!< inline keyword for ARM Compiler       */
#define __STATIC_INLINE         static __inline
#define _PACKED_                __attribute__ ((packed))
#define PACKED(structOrUnion)   __packed structOrUnion
#define SECTION(_name)          __attribute__ ((__section__(_name)))
#define SECTION_ZI(_name)       __attribute__ ((section(_name),zero_init))
#define DEPRECATED              __attribute__ ((deprecated))
#elif defined ( __ICCARM__ )
#define __ASM                   __asm                                      /*!< asm keyword for IAR Compiler          */
#define __INLINE                inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
#define __STATIC_INLINE         static inline
#define _PACKED_                __packed
#define PACKED(structOrUnion)   __packed structOrUnion
#define SECTION(_name)          @_name
#define SECTION_ZI(_name)       @_name
#elif defined ( __GNUC__ )
#define __ASM                   __asm                                      /*!< asm keyword for GNU Compiler          */
#define __INLINE                inline                                     /*!< inline keyword for GNU Compiler       */
#define __STATIC_INLINE         static inline
#define _PACKED_                __attribute__ ((packed))
#define  PACKED(structOrUnion)  structOrUnion __attribute__ ((packed))
#define SECTION(_name)          __attribute__ ((__section__(_name)))
#define SECTION_ZI(_name)       __attribute__ ((section(_name),zero_init))
#define DEPRECATED              __attribute__ ((deprecated))
#endif

/** End of Platform_Toolchain_Compiler
  * @}
  */

/** End of Platform_Toolchain
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* __COMPILER_ABSTRACTION_H__ */
