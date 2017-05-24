enum { __FILE_NUM__ = 0 };
/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simpleBLECentral_api.c
  * @brief   This file provides the BT stack start interface.
  * @details
  * @author  ranhui
  * @date    11-September-2015
  * @version v1.0
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include "gap.h"

/**
  * @brief  Start BT Stack when using GAP Peripheral Role.
  * @retval result.
  */
bool peripheral_StartBtStack()
{
    return GAP_StartBtStack();
}

