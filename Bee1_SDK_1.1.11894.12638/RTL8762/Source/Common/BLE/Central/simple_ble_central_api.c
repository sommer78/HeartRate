enum { __FILE_NUM__ = 0 };
/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_central_api.c
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
  * @brief  Start BT Stack when using GAP Central Role.
  * @retval result.
  */
bool central_StartBtStack()
{
    return GAP_StartBtStack();
}

