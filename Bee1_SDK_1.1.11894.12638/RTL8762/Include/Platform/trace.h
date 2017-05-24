/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     trace.h
* @brief    Log print and setting interface head file.
* @author   lory_xu
* @date     2014-05
* @version  v0.1
*************************************************************************************************************
*/

#ifndef _TRACE_H_
#define _TRACE_H_

#include "rtl_types.h"
#include "section_config.h"
#include "common_defs.h"

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Debug Debug
  * @{
  */

/** @addtogroup Platform_Debug_Log_Trace Log Support
  * @{
  */

/** @addtogroup Platform_Debug_Log_Trace_Exported_Types Log Trace Types
  * @{
  */

/** @brief Log Module Definition */
typedef enum _MODULE_DEFINE_
{
    MODULE_OS           = 0,  /**< FreeRTOS */
    /* BEE hardware modules*/
    MODULE_KEYSCAN      = 1,  /**< Keyscan */
    MODULE_QDECODE      = 2,  /**< Qdecode */
    MODULE_IR           = 3,  /**< IR */
    MODULE_3DG          = 4,  /**< 3D Glass */
    MODULE_ADC          = 5,  /**< ADC */
    MODULE_GDMA         = 6,  /**< GDMA */
    MODULE_I2C          = 7,  /**< I2C */
    MODULE_RTC          = 8,  /**< RTC */
    MODULE_SPI          = 9,  /**< SPI */
    MODULE_TIMER        = 10, /**< Timer */
    MODULE_UART         = 11, /**< UART */
    MODULE_EFLASH       = 12, /**< eFlash */
    MODULE_GPIO         = 13, /**< GPIO */
    MODULE_PINMUX       = 14, /**< Pinmux */
    MODULE_PWM          = 15, /**< PWM */

    /* BEE software modules*/
    MODULE_DFU          = 23, /**< DFU */
    MODULE_DTM          = 24, /**< DTM */
    MODULE_LTP          = 25, /**< LTP */
    MODULE_APP          = 26, /**< Application */
    MODULE_PROFILE      = 27, /**< BT Profile */
    MODULE_FRAMEWORK    = 28, /**< BT Framework */
    MODULE_LOWERSTACK   = 29, /**< BT Lowerstack */
    MODULE_UPPERSTACK   = 30, /**< BT Upperstack */
    MODULE_DRIVERTASK   = 31, /**< Driver Task(deprecated) */
    MODULE_NUMs         = 32  /**< Module Number */
} MODULE_DEFINE;

/** @brief Log Level Definition */
typedef enum _LEVEL_DEFINE_
{
    LEVEL_ERROR         = 0, /**< Error */
    LEVEL_WARN          = 1, /**< Warning */
    LEVEL_INFO          = 2, /**< Information */
    LEVEL_TRACE         = 3, /**< Trace Data */
    LEVEL_NUMs          = 4  /**< Level Number */
} LEVEL_DEFINE;

/** End of Platform_Debug_Log_Trace_Exported_Types
  * @}
  */

/** @addtogroup Platform_Debug_Log_Trace_Exported_Constants Log Trace Constants
  * @{
  */
/** DBG_LEVEL is used to control the log printed by DBG_BUFFER(), and there are 5 levels from 0 to 4.
  * 0: No Log
  * 1: Print ERROR
  * 2: Print ERROR, WARN
  * 3: Print ERROR, WARN, INFO
  * 4: Print ERROR, WARN, INFO, TRACE
  */
#ifdef RELEASE_VERSION
#define DBG_LEVEL            0
#else
#define DBG_LEVEL            4
#endif
#define MODULE_START_INDEX 220
/** End of Platform_Debug_Log_Trace_Exported_Constants
  * @}
  */

/** @addtogroup Platform_Debug_Log_Trace_Functions_And_Macro Function & Macro
  * @{
  */

/** @cond private */
/** @brief  Debug Log mask */
extern UINT32 ConfigDebug[];

/** @brief LOG_RAW is reserved, use DBG_DIRECT macro instead */
UINT32 LOG_RAW(IN  char *fmt, ...);

/** @brief LOG_BUFFER_APP is reserved, use DBG_BUFFER macro instead */
UCHAR LOG_BUFFER_APP(UINT32 log_str_index, UINT8 para_num, ...);
/** @endcond */

/**
 * @brief  Debug Log Mask Set.
 * @param  config[4] --  Print log if bit MODULE_DEFINE of config[LEVEL_DEFINE] is 1;
 * @return void.
 * @note   Here is a IR module sample to demostrate the using of set_debu_mask().
 *         We want to print log under ERROR level and INFO level.
 * @code{.c}
    UINT32 debug[4];
    debug[LEVEL_ERROR] = 0x1 << MODULE_IR;
    debug[LEVEL_WARN]  = 0x0;
    debug[LEVEL_INFO]  = 0x1 << MODULE_IR;
    debug[LEVEL_TRACE] = 0x0;
    set_debug_mask(debug);

    DBG_BUFFER(MODULE_IR, LEVEL_INFO, "IR Info.\n", 0);
    DBG_BUFFER(MODULE_IR, LEVEL_ERROR, "IR Error!\n", 0);
 * @endcode
 */
void set_debug_mask(UINT32 config[]);

/**
  * @brief  DBG_DIRECT is used to print log without buffer or PrintTask.
  * DBG_DIRECT is used when:
  * 1. Before PrintTask starts;
  * 2. In Hard Fault Error ISR;
  * 3. Warning when PrintTask's buffer is full.
  * ...
  */
#ifdef RELEASE_VERSION
#define DBG_DIRECT(...)
#else
#define DBG_DIRECT(...)     do {\
        LOG_RAW(__VA_ARGS__);\
    }while(0)
#endif

/**
  * @brief  DBG_BUFFER is used to print log with buffer and PrintTask.
  *  PrintTask's priority is the lowest, so Print log will not block any normal task.
  */
#define DBG_BUFFER(MODULE, LEVEL, pFormat, para_num,...)     DBG_BUFFER_##LEVEL(MODULE, pFormat, para_num, ##__VA_ARGS__)
		
#if (DBG_LEVEL == 0)
#define DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ...)     
#define DBG_BUFFER_LEVEL_WARN(MODULE, pFormat, para_num, ...)    
#define DBG_BUFFER_LEVEL_INFO(MODULE, pFormat, para_num, ...)    
#define DBG_BUFFER_LEVEL_TRACE(MODULE, pFormat, para_num, ...)   
#elif (DBG_LEVEL == 1) 
#define DBG_BUFFER_LEVEL_ERROR(MODULE,pFormat, para_num, ...)     do {\
				static const char traceFormat[] TRACE_DATA = pFormat;\
				LOG_BUFFER_APP((UINT32)traceFormat, para_num, ##__VA_ARGS__); \
}while(0)
#define DBG_BUFFER_LEVEL_WARN(MODULE, pFormat, para_num, ...)     
#define DBG_BUFFER_LEVEL_INFO(MODULE, pFormat, para_num, ...)    
#define DBG_BUFFER_LEVEL_TRACE(MODULE, pFormat, para_num, ...)   
#elif (DBG_LEVEL == 2)
#define DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ...)     do {\
				static const char traceFormat[] TRACE_DATA = pFormat;\
				LOG_BUFFER_APP((UINT32)traceFormat, para_num, ##__VA_ARGS__); \
}while(0)
#define DBG_BUFFER_LEVEL_WARN(MODULE, pFormat, para_num, ...)			DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ##__VA_ARGS__)
#define DBG_BUFFER_LEVEL_INFO(MODULE, pFormat, para_num, ...)     
#define DBG_BUFFER_LEVEL_TRACE(MODULE, pFormat, para_num, ...)    
#elif (DBG_LEVEL == 3)
#define DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ...)     do {\
				static const char traceFormat[] TRACE_DATA = pFormat;\
				LOG_BUFFER_APP((UINT32)traceFormat, para_num, ##__VA_ARGS__); \
}while(0)
#define DBG_BUFFER_LEVEL_WARN(MODULE, pFormat, para_num, ...)     DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ##__VA_ARGS__)
#define DBG_BUFFER_LEVEL_INFO(MODULE, pFormat, para_num, ...)     DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ##__VA_ARGS__)
#define DBG_BUFFER_LEVEL_TRACE(MODULE, pFormat, para_num, ...)     
#elif (DBG_LEVEL == 4)
#define DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ...)     do {\
				static const char traceFormat[] TRACE_DATA = pFormat;\
				LOG_BUFFER_APP((UINT32)traceFormat, para_num, ##__VA_ARGS__); \
}while(0)
#define DBG_BUFFER_LEVEL_WARN(MODULE, pFormat, para_num, ...)     DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ##__VA_ARGS__)
#define DBG_BUFFER_LEVEL_INFO(MODULE, pFormat, para_num, ...)     DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ##__VA_ARGS__)
#define DBG_BUFFER_LEVEL_TRACE(MODULE, pFormat, para_num, ...)    DBG_BUFFER_LEVEL_ERROR(MODULE, pFormat, para_num, ##__VA_ARGS__)
#endif
/** End of Platform_Debug_Log_Trace_Functions_And_Macro
  * @}
  */

/** End of Platform_Debug_Log_Trace
  * @}
  */

/** End of Platform_Debug
  * @}
  */

/** End of Platform
  * @}
  */

#endif /* _TRACE_H_ */
