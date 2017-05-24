/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     observer_broadcaster.h
* @brief    Head file for Gap Observer and Broadcaster roles.
* @details
* @author    ranhui
* @date      2015-4-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef OBSERVER_BROADCASTER_H
#define OBSERVER_BROADCASTER_H

#include "rtl_types.h"
#include "blueapi_types.h"
#include "gap.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define OB_STATE_BITS           2
#define OB_STATE_MASK           0x03 // (0xff >> (8*sizeof(uint8_t) - OB_STATE_BITS))
#define OB_STATE_IDLE           0
#define OB_STATE_START          1
#define OB_STATE_STOP           2
#define OB_STATE_ACTIVE         3

#define OB_SCAN_STATE_OFFSET    1
#define OB_ADV_STATE_OFFSET     (OB_SCAN_STATE_OFFSET + OB_STATE_BITS)

#define ob_ScanStateSet(state, value) do                        \
{                                                               \
    state &= ~(OB_STATE_MASK << OB_SCAN_STATE_OFFSET);          \
    state |= (value & OB_STATE_MASK) << OB_SCAN_STATE_OFFSET;   \
} while (0)

#define ob_ScanStateGet(state) ((state >> OB_SCAN_STATE_OFFSET) & OB_STATE_MASK)

#define ob_AdvStateSet(state, value) do                         \
{                                                               \
    state &= ~(OB_STATE_MASK << OB_ADV_STATE_OFFSET);           \
    state |= (value & OB_STATE_MASK) << OB_ADV_STATE_OFFSET;    \
} while (0)

#define ob_AdvStateGet(state) ((state >> OB_ADV_STATE_OFFSET) & OB_STATE_MASK)


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
extern TGAP_STATUS obSetGapParameter( uint16_t param, uint8_t len, void *pValue );

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
extern TGAP_STATUS obGetGapParameter( uint16_t param, void *pValue );

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
extern TGAP_STATUS ob_StartAdvertising(void);

/**
 * @brief       Cancel an existing device discovery request.
 *
 *
 * @return      gapAPI_CauseSuccess: cancel started,<BR>
 *              bleInvalidTaskID: Not the task that started discovery,<BR>
 *              gapAPI_IncorrectMode: not in discovery mode<BR>
*/
extern TGAP_STATUS ob_StopAdvertising(void);

/**
 * @brief   Start a device discovery scan.
 *
 *
 * @return  gapAPI_CauseSuccess: Discovery scan started.<BR>
 *          gapAPI_IncorrectMode: Invalid profile role.<BR>
 *          gapAPI_AlreadyInRequestedMode: Not available.<BR>
 */
extern TGAP_STATUS ob_StartScan(void);

/**
 * @brief   Cancel a device discovery scan.
 *
 * @return  gapAPI_CauseSuccess: Cancel started.<BR>
 *          bleInvalidTaskID: Not the task that started discovery.<BR>
 *          gapAPI_IncorrectMode: Not in discovery mode.<BR>
 */
extern TGAP_STATUS ob_StopScan(void);

/**
 * @internal
 *
 * @brief   Register device state change callbacks from profile.
 *
 * @param   pAppCallbacks - Callback app providedfor notify device state change
 *
 * @return  gapAPI_CauseSuccess
 */
extern TGAP_STATUS ob_RegisterDeviceStateChangeCB( gapDeviceStateChangeCb pAppCallbacks );

/**
 * @internal
 *
 * @brief   Initialize default parameters for peripheral role.
 *
 *
 */
extern void ob_GapParaInit(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* OBSERVER_BROADCASTER_H */
