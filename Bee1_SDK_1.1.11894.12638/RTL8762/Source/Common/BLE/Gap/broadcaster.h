/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      broadcaster.h
* @brief    Head file for Gap Broadcaster role
* @details
* @author    ranhui
* @date      2015-4-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef BROADCASTER_H
#define BROADCASTER_H

#include "rtl_types.h"
#include "blueapi_types.h"
#include "gap.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Callback when the device has been started.  Callback event to
 * the Notify of a state change.
 */
//typedef void (*gapDeviceStateChangeCb)( gaprole_States_t newState );

/*-------------------------------------------------------------------
 * API FUNCTIONS
 */

/**
 * @defgroup GAPROLES_BROADCASTER_API GAP Broadcaster Role API Functions
 *
 * @{
 */

/**
 * @brief       Set a GAP Broadcaster parameter.
 *
 *  NOTE: You can call this function with a GAP Broadcaster Parameter ID and it will set the
 *        GAP Parameter.  GAP Parameters are defined in (broadcaster.h).  Also,
 *        the "len" field must be set to the size of a "uint16" and the
 *        "pValue" field must point to a "uint16".
 *
 * @param       param - Profile parameter ID: @ref GAPROLE_BROADCASTER_PARAMETERS
 * @param       len - length of data to write
 * @param       pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return      gapAPI_CauseSuccess or INVALIDPARAMETER (invalid paramID)
 */
extern TGAP_STATUS broadcasterSetGapParameter( uint16_t param, uint8_t len, void *pValue );
/**
 * @brief       Get a GAP Broadcaster parameter.
 *
 *  NOTE: You can call this function with a GAP Broadcaster Parameter ID and it will get a
 *        GAP Parameter.  GAP Broadcaster Parameters are defined in (broadcaster.h).  Also, the
 *        "pValue" field must point to a "uint16".
 *
 * @param       param - Profile parameter ID: @ref GAPROLE_BROADCASTER_PARAMETERS
 * @param       pValue - pointer to location to get the value.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return      gapAPI_CauseSuccess or INVALIDPARAMETER (invalid paramID)
 */
extern TGAP_STATUS broadcasterGetGapParameter( uint16_t param, void *pValue );

/**
 * @brief       Setup or change advertising.  Also starts advertising.
 *
 * @param       pParams - advertising parameters
 *
 * @return      gapAPI_CauseSuccess: advertising started,<BR>
 *              gapAPI_IncorrectMode: invalid profile role,<BR>
 *              gapAPI_AlreadyInRequestedMode: not available at this time,<BR>
 *              bleNotReady: advertising data isn't set up yet.<BR>
*/
extern TGAP_STATUS broadcaster_StartAdvertising(void);

/**
 * @brief       Cancel an existing device discovery request.
 *
 *
 * @return      gapAPI_CauseSuccess: cancel started,<BR>
 *              bleInvalidTaskID: Not the task that started discovery,<BR>
 *              gapAPI_IncorrectMode: not in discovery mode<BR>
*/
extern TGAP_STATUS broadcaster_StopAdvertising(void);
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
extern TGAP_STATUS broadcaster_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks );

/**
 * @internal
 *
 * @brief   Initialize default parameters for peripheral role.
 *
 *
 */
extern void broadcaster_GapParaInit(void);
/**
 * @internal
 *
 * @brief   Start advertising in initialize phase, used by profile.
 *
 *
 * @return  gapAPI_CauseSuccess
 */
extern TGAP_STATUS broadcaster_Init_StartAdvertising(void);
/**
 * @internal
 *
 * @brief   Set TX Power of BLE Controller.
 *
 *
 * @return  gapAPI_CauseSuccess
 */
extern TGAP_STATUS broadcaster_SetTxPower( TBLE_TX_POWER_INDEX tx_power_index );

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BROADCASTER_H */
