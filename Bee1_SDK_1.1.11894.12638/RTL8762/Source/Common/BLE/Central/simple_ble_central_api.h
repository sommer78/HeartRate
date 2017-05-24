/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    simple_ble_central_api.h
  * @brief   This file contains all the functions prototypes for the [peripheral] driver 
  *          library. 
  * @details
  * @author  author
  * @date    11-March-2011
  * @version v3.5.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _SIMPLE_BLE_CENTRAL_API_H_
#define _SIMPLE_BLE_CENTRAL_API_H_

#ifdef __cplusplus
 extern "C" {
#endif

///@cond
bool central_StartBtStack(void);
bool central_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg );
void centralBlueAPIMessageCB(PBlueAPI_UsMessage pMsg);
///@endcond

#ifdef __cplusplus
}
#endif

#endif

