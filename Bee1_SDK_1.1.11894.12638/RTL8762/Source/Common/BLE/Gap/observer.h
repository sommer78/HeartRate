
/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      observer.h
* @brief    Head file for Gap Observer role
* @details
* @author    ranhui
* @date      2015-4-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef OBSERVER_H
#define OBSERVER_H

#include "rtl_types.h"
#include "blueapi_types.h"
#include "gap.h"


#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup TObserverAppCB_MsgType TObserverAppCB_MsgType
  * @{ Message data type, when data sent to app directly
  */
typedef enum
{
    ADV_SCAN_RSP_DATA_MSGTYPE                //!< MSG used to sent received advertise or scan response data. 
}TObserverAppCB_MsgType, *PObserverAppCB_MsgType;
/** End of TObserverAppCB_MsgType * @} */

/** @defgroup TObserverAppCB_MsgData TObserverAppCB_MsgData
  * @{ Message data sent to app directly from Observer role
  */
typedef union
{
    PBlueAPI_LEScanInfo   pAdvScanRspData;
}TObserverAppCB_MsgData, *PObserverAppCB_MsgData;
/** End of TObserverAppCB_MsgData * @} */

/** @brief function ponter Type used for app Call back, to send events to application. */
typedef void (*pfnObserverAppCB_t)(TObserverAppCB_MsgType msgType, TObserverAppCB_MsgData CbData);

/*********************************************************************
 * API FUNCTIONS
 */

/*-------------------------------------------------------------------
 * Observer Profile Public APIs
 */

/**
 * @defgroup OBSERVER_PROFILE_API Observer Profile API Functions
 *
 * @{
 */

/**
 * @brief   Get a parameter in the Observer Profile.
 *
 * @param   param - profile parameter ID: @ref GAPCENTRALROLE_PROFILE_PARAMETERS
 * @param   pValue - pointer to buffer to contain the read data
 *
 * @return  gapAPI_CauseSuccess: Operation successful.<BR>
 *          INVALIDPARAMETER: Invalid parameter ID.<BR>
 */
extern TGAP_STATUS observerSetGapParameter( uint16_t param, uint8_t len, void *pValue );

/**
 * @brief   Get a parameter in the Observer Profile.
 *
 * @param   param - profile parameter ID: @ref GAPCENTRALROLE_PROFILE_PARAMETERS
 * @param   pValue - pointer to buffer to contain the read data
 *
 * @return  gapAPI_CauseSuccess: Operation successful.<BR>
 *          INVALIDPARAMETER: Invalid parameter ID.<BR>
 */
extern TGAP_STATUS observerGetGapParameter( uint16_t param, void *pValue );

/**
 * @brief   Start a device discovery scan.
 *
 *
 * @return  gapAPI_CauseSuccess: Discovery scan started.<BR>
 *          gapAPI_IncorrectMode: Invalid profile role.<BR>
 *          gapAPI_AlreadyInRequestedMode: Not available.<BR>
 */
extern TGAP_STATUS observer_StartScan(void);

/**
 * @brief   Cancel a device discovery scan.
 *
 * @return  gapAPI_CauseSuccess: Cancel started.<BR>
 *          bleInvalidTaskID: Not the task that started discovery.<BR>
 *          gapAPI_IncorrectMode: Not in discovery mode.<BR>
 */
extern TGAP_STATUS observer_StopScan(void);

/**
 * @brief   Set TX Power of BLE Controller.
 *
 * @return  gapAPI_CauseSuccess: Set request ok.<BR>
 */
extern TGAP_STATUS observer_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index );

/**
 * @}
 */


/**
 * @internal
 *
 * @brief   Register device state change callbacks from profile.
 *
 * @param   pAppCallbacks - Callback app providedfor notify device state change
 *
 * @return  gapAPI_CauseSuccess
 */
extern TGAP_STATUS observer_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks );

extern void observer_RegisterAppDirectCB( pfnObserverAppCB_t pAppCallback );

/**
 * @internal
 *
 * @brief   Initialize default parameters for observer role.
 *
 * @param   none
 *
 * @return  none
 */
extern void observer_GapParaInit(void);


/**
 * @internal
 *
 * @brief   Observer Profile Task event processing function.
 *
 * @param   taskId - Task ID
 * @param   events - Events.
 *
 * @return  events not processed
 */
bool observer_HandleBlueAPIMessage( PBlueAPI_UsMessage pMsg );

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

extern pfnObserverAppCB_t pGapObserverAppDirectCb;

#ifdef __cplusplus
}
#endif

#endif /* CENTRAL_H */
