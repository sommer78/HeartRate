/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    profile_init.c
  * @brief   .
    * @details
  * @author  Chuanguo Xue
  * @date    27-March-2015
  * @version v0.2
  ******************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

#include "rtl876x.h"

#include "bee_message.h"
#include "trace.h"
#include <blueapi.h>
#include "profileApi.h"

#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"

BOOL Adv_Enable_Default = FALSE;
uint8_t is_bluetooth_active = FALSE;
uint8_t bluetooth_stack_state = blueAPI_EventIdle;


extern BOOL IsBeenPaired;
extern uint8_t RemoteBd[6];
#define PAIRING_MODE 3 // JUST_WORKS: 3 (blueAPI_IOCapNoIO)

void PxpStartAdv()
{
    peripheral_StartAdvertising();
}

void PxpStopAdv()
{
    peripheral_StopAdvertising();
}

void PxpEnablePairing()
{
    //configure parameter first
    uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm = GAPBOND_AUTH_NO_MITM_YES_BOND;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oobEnable = FALSE;

    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );


    GAPBondMgr_SetPairable();
}
