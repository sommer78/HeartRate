/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     dlps_platform.h
* @brief    DLPS platform implementation head file.
* @author   lory_xu
* @date     2014-08-05
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __DLPS_PLATFORM_H
#define __DLPS_PLATFORM_H

#include "rtl_types.h"
#include "FreeRTOS.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Platform
  * @{
  */

/** @addtogroup Platform_Function Hardware Function
  * @{
  */

/** @addtogroup Platform_Function_DLPS DLPS
  * @{
  */

/** @addtogroup Platform_Function_DLPS_Exported_Types DLPS Types
  * @{
  */

/** @brief DLPS_BUFFER struct */
typedef struct _DLPS_BUFFER
{
    struct DLPS_BUFFER *pNext;
    UINT32  Length;
    UINT8   *pBufferStart;
    UINT8   *pBuffer_ON;
    BOOL    ShouldFree;
} DLPS_BUFFER;

/** @brief This CB is used for any module which needs to be checked before entering DLPS */
typedef BOOL (*DLPSEnterCheckFunc) ();

/** @brief DLPS_CHECKFUNC struct */
typedef struct _DLPS_CHECKFUNC
{
    struct DLPS_CHECKFUNC *pNext;
    DLPSEnterCheckFunc CheckFunc;
} DLPS_CHECKFUNC ;

/** @brief This CB is used for any module which needs to control (Enable or Disable) the interrupts before entering DLPS */
typedef void (*DLPSInterruptControlFunc) ();

/** @brief DLPS_STATE struct */
typedef enum _DLPS_STATE
{
    DLPS_ENTER      = 0, /**< Enter DLPS */
    DLPS_EXIT       = 1, /**< Exit DLPS For System */
    DLPS_EXIT_ALL   = 2  /**< Exit DLPS For APP */
} DLPS_STATE;

/** @brief LPS_MODE struct */
typedef enum _LPS_MODE
{
    LPM_PowerDown_MODE  = 0, /**< Power down */
    LPM_Hibernare_MODE  = 1, /**< Hibernate  */
    LPM_DLPS_MODE       = 2, /**< DLPS       */
    LPM_LPS_MODE        = 3, /**< LPS        */
    LPM_ACTIVE_MODE     = 4  /**< Active     */
} LPS_MODE;

/** @brief DLPS_InterruptFUNC struct */
typedef struct _DLPS_InterruptFUNC
{
    struct DLPS_InterruptFUNC *pNext;              /**< Pointer to next entry           */
    DLPSInterruptControlFunc InterruptControlFunc; /**< DLPS Interrupt Control Function */
    DLPS_STATE DLPSState; /**< Indicate the time of CB should be called                 */
} DLPS_InterruptFUNC ;

/** End of Platform_Function_DLPS_Exported_Types
  * @}
  */

/** @addtogroup Platform_Function_DLPS_Function DLPS Function
  * @{
  */

/**
 * @brief Register Check CB to DlpsPlatform which will call it before enter Dlps.
 * @param  func -- DLPSEnterCheckFunc.
 * @return  Status of Operation.
 * @retval TURE if success, FALSE if fail.
*/
extern BOOL DLPS_ENTER_CHECK_CB_REG(DLPSEnterCheckFunc func);

/**
 * @brief Register Interrupt_control CB to DlpsPlatform which will call it before enter Dlps or after resume from Dlps (according to DLPS_STATE) .
 * @param  func -- DLPSInterruptControlFunc.
 * @param  DLPSState -- tell the DlpsPlatform the CB should be called when DLPS_ENTER or DLPS_EXIT.
 * @return  Status of Operation.
 * @retval TURE if success, FALSE if fail.
*/
extern BOOL DLPS_INTERRUPT_CONTROL_CB_REG(DLPSInterruptControlFunc func, DLPS_STATE DLPSState);

/**
 * @brief Unregister Interrupt_control CB to DlpsPlatform.
 * @param  func -- DLPSInterruptControlFunc.
 * @return  none.
*/
extern void DLPS_INTERRUPT_CONTROL_CB_UNREG(DLPSInterruptControlFunc func);

/**
 * @brief Register buffer information to DlpsPlatform.
 * @param  pBufferStart -- Start address of buffer.
 * @param  Length -- Length of buffer.
 * @param    ShouldFree -- Should the registered buffer information be freed when exit dlps.
 * @return  status of operation.
 * @retval TURE if success, FALSE if fail.
*/
extern BOOL DLPS_BUFFER_REG(UINT8   *pBufferStart, UINT32   Length, BOOL ShouldFree);

/**
 * @brief Unregister buffer information to DlpsPlatform.
 * @param  pBufferStart -- Start address of buffer.
 * @return  none.
*/
extern void DLPS_BUFFER_UNREG(UINT8 *pBufferStart);

/**
 * @brief Special Restore flow.
 * @note This interface is reserved for internal use.
*/
extern void DLPS_RESTORE_SPECIAL(void);

/**
 * @brief Keep platform in LPM_ACTIVE_MODE.
 * @return  none.
*/
extern void LPS_MODE_Pause(void);

/**
 * @brief Restore to original LPS_MODE.
 * @return  none.
*/
extern void LPS_MODE_Resume(void);

/**
 * @brief  LPS_MODE Set .
 * @param  mode -- LPS_MODE.
 * @return  none.
*/
extern void LPS_MODE_Set(LPS_MODE mode);

/**
 * @brief  LPS_MODE Get .
 * @return  LPS_MODE.
*/
extern LPS_MODE LPS_MODE_Get(void);

/**
 * @brief  Return Pause LPS_MODE stack, only 0 can enter any low power mode.
 * @return  stack num.
*/
extern INT8 LPS_MODE_Stack_Get(void);

/**
 * Deprecated Functions
 * Please use DLPS_KeepActiveTicks instead.
 * @note  Keep in LPM_ACTIVE_MODE for 50 * restart_val ms.
*/
extern void ActiveTime_Restart(UINT16 restart_val);

/**
 * @note  Keep in LPM_ACTIVE_MODE for 10*xTimerPeriodInTicks ms.
*/
extern void DLPS_KeepActiveTicks(const TickType_t xTimerPeriodInTicks);

/** End of Platform_Function_DLPS_Function
  * @}
  */

/** End of Platform_Function_DLPS
  * @}
  */

/** End of Platform_Function
  * @}
  */

/** End of Platform
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif  /* __DLPS_PLATFORM_H */
