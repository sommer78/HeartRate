/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simpleBLECentral.h
  * @brief   This file contains function prototype of GAP Central Role. 
  * @details
  * @author  ranhui
  * @date    11-March-2015
  * @version v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef SIMPLEBLEPERIPHERAL_H
#define SIMPLEBLEPERIPHERAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Add Includes here **/
#include "gap.h"
#include "bee_message.h"

/** Add all public functions here **/
extern void BtStack_Init_Peripheral(void);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBLEPERIPHERAL_H */
