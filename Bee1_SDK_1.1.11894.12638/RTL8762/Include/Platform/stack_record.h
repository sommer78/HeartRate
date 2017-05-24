/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     stack_record.h
* @brief    Stack and interrupt trace implementation head file.
* @note     The head file is reserved for debug.
* @author   lory_xu
* @date     2014-10-03
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __STACK_RECORD_H
#define __STACK_RECORD_H

#include "rtl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Debug Debug
  * @{
  */

/** @addtogroup Platform_Debug_Stack_Record Stack & Interrupt Trace
  * @{
  */

/** @addtogroup Platform_Debug_Stack_Record_Constants Exported Constants
  * @{
  */
#define MAX_RECORD_COUNT    16
#define RECORD_SIGNATURE    0x12345678
/** End of Platform_Debug_Stack_Record_Constants
  * @}
  */

/** @addtogroup Platform_Debug_Stack_Record_Types Record Types
  * @{
  */
/** @brief Record Type */
typedef enum
{
    RECORD_Invalid      = 0, /**< Invalid Record Type */
    RECORD_INTERRUPT    = 1, /**< Interrupt */
    RECORD_TASK         = 2  /**< FreeRTOS Task */
} RECORD_TYPE;

/*
HISTORY_RECORD
bit 0-5: Num
bit 6-7: Type
*/
typedef UINT8 RECORD;

/** @brief Record Table */
typedef struct _RECORD_TABLE
{
    UINT32 signature;                /*!< if signature is RECORD_SIGNATURE, then stop record */
    UINT16 consecutive_WDG_times;    /*!< Watchdog count */
    UINT16 record_index;             /*!< Current record index in record[MAX_RECORD_COUNT] array */
    RECORD record[MAX_RECORD_COUNT]; /*!< Array to record interrupt and task run history */
} RECORD_TABLE;

/** End of Platform_Debug_Stack_Record_Types
  * @}
  */

/** @addtogroup Platform_Debug_Stack_Record_Globatable Global Table
  * @{
  */
/** @brief Record table for recording task and interrupt call history */
extern RECORD_TABLE gRecordTable;
/** End of Platform_Debug_Stack_Record_Globaldata
  * @}
  */

/** @addtogroup Platform_Debug_Stack_Record_Export_Functions_and_Macros Function & Macro
  * @{
  */

/** @brief  Always add a task/interrupt call history into record table
  * @param  type: Task or Interrupt
 *    @arg  RECORD_INTERRUPT: Interrupt
  *   @arg  RECORD_TASK: Task
  * @param  number: Interrupt Number
  * @retval None
  */
extern VOID RECORD_PUSH(UINT32 type, UINT32 number);

/** @brief  Dump record table
  * @retval None
  */
extern VOID RECORD_DUMP(void);

/** @brief  Add a task/interrupt call history into record table while signature is not equal RECORD_SIGNATURE */
#define RECORD_ADD(type, number) do {\
        if(RECORD_SIGNATURE != gRecordTable.signature) {\
            RECORD_PUSH(type, number); \
        }\
    }while(0)
/** End of Platform_Debug_Stack_Record_Export_Functions_and_Macros
  * @}
  */

/** End of Platform_Debug_Stack_Record
  * @}
  */

/** End of Platform_Debug
  * @}
  */

/** End of Platform
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STACK_RECORD_H */
