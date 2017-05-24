/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_io_dlps.h
* @brief     
* @details   
* @author    tifnan_ge
* @date      2015-05-18
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_IO_DLPS_H_
#define _RTL876X_IO_DLPS_H_

#include "board.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup DLPS DLPS
  * @brief IO DLPS dirver module
  * @{
  */

/** @defgroup IO_DLPS_Exported_Types IO DLPS Exported Types
  * @{
  */

typedef void (*DLPS_IO_ExitDlpsCB)(void);
typedef void (*DLPS_IO_EnterDlpsCB)(void);

/**
  * @}
  */
     
/** @defgroup IO_DLPS_Exported_Constants IO DLPS Exported Constants
  * @{
  */

/**
  * @}
  */ 

/** @defgroup IO_DLPS_Exported_Functions IO DLPS Exported Functions
  * @{
  */

extern void DLPS_IO_Register(void);
extern uint32_t DLPS_ReadWakeupPinData(void);

#if USE_USER_DEFINE_DLPS_EXIT_CB

extern DLPS_IO_ExitDlpsCB User_IO_ExitDlpsCB;

/**
  * @brief  Rrgister user-defined exit dlps callback function 
  * @param  func: user-defined callback functon.
  * @retval None
  */
__STATIC_INLINE void DLPS_IO_RegUserDlpsExitCb(DLPS_IO_ExitDlpsCB func)
{
    User_IO_ExitDlpsCB = func;
}

#endif /* USE_USER_DEFINE_DLPS_EXIT_CB */

#if USE_USER_DEFINE_DLPS_ENTER_CB

extern DLPS_IO_EnterDlpsCB User_IO_EnterDlpsCB;

/**
  * @brief  Rrgister user-defined enter dlps callback function 
  * @param  func: user-defined callback functon.
  * @retval None
  */
__STATIC_INLINE void DLPS_IO_RegUserDlpsEnterCb(DLPS_IO_EnterDlpsCB func)
{
    User_IO_EnterDlpsCB = func;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_IO_DLPS_H_ */

/**
  * @}
  */ 


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor *****END OF FILE****/

