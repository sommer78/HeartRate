/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     cycle_queue.h
* @brief    cycle buffer implementation head file.
* @author   lory_xu
* @date     2014-05-19
* @version  v0.1
*************************************************************************************************************
*/

#ifndef _CYCLE_QUEUE_H_
#define _CYCLE_QUEUE_H_

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

/** @addtogroup Platform_Debug_Cycle_Queue Cycle Queue
  * @{
  */

/** @cond private */
/** @brief  Log Buffer */
extern UINT8 cyc_buffer[];

/** @brief  Read pointer of Log Buffer */
extern volatile UINT16 pRead;

/** @brief  Write pointer of Log Buffer */
extern volatile UINT16 pWrite;
/** @endcond */

/**
 * @brief Check log buffer is empty or not.
 * @return  Status of log buffer.
 * @retval TURE if empty, FALSE if not empty.
*/
BOOL IsCycQueueEmpty(void);

/**
 * @brief Check log buffer is full or not.
 * @return  Status of log buffer.
 * @retval TURE if full, FALSE if not full.
*/
BOOL IsCycQueueFull(void);

/**
 * @brief Return the number of data in log buffer.
 * @return  the number of data in log buffer.
*/
UINT16 CycQueueSize(void);

/**
 * @brief  Return the available buffer size in log buffer.
 * @return  the available buffer size in log buffer.
*/
UINT16 CycQueueRemainSize(void);

/**
 * @brief Put data in log buffer.
 * @param  pWriteBuf -- Start address of write buffer.
 * @param  length -- Length of write buffer.
 * @return  the status of operation.
 * @retval TURE if success, FALSE if fail.
*/
BOOL CycQueueWrite(UINT8 *pWriteBuf, UINT16 length);

/**
 * @brief Retrieve data in log buffer and then print them.
 * @param  AvailableSize -- The max number of data can be print. (The value is determined by Log uart buffer)
 * @return  The actual number of print data.
*/
UINT16 CycQueueReadandPrint(UINT8 AvailableSize);

/**
 * @brief Retrieve all data in log buffer and print them when exception happens like hard fault.
             This funcion is usually used for debugging.
 * @return  void.
*/
void CycQueueDumpWhenException(void);

/** End of Platform_Debug_Cycle_Queue
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

#endif /* _CYCLE_QUEUE_H_ */
