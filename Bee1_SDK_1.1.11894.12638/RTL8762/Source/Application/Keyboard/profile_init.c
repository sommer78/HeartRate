/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     profile_init.c
* @brief    .
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v1.0
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "bee_message.h"
#include "trace.h"
#include <blueapi.h>


#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"

BOOL Adv_Enable_Default = FALSE;
uint8_t is_bluetooth_active = FALSE;
uint8_t bluetooth_stack_state = blueAPI_EventIdle;


//extern uint8_t advertising_interval;
//extern BOOL IsBeenPaired;
extern uint8_t RemoteBd[6];
#define PAIRING_MODE 3 // JUST_WORKS: 3 (blueAPI_IOCapNoIO)


/**
* @brief
*
*
* @param   void
* @return  void
*/
void KeyboardStartAdv()
{
    peripheral_StartAdvertising();
}


/**
* @brief
*
*
* @param   void
* @return  void
*/
void KeyboardStopAdv()
{
    peripheral_StopAdvertising();
}

/**
* @brief
*
*
* @param   void
* @return  void
*/
void KeyboardDisconnect()
{
    peripheral_Disconnect();
}

/**
* @brief
*
*
* @param
* @return  void
*/
void KeyboardEnablePairing()
{
    //configure parameter first
    uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm = GAPBOND_AUTH_NO_MITM_YES_BOND;
    uint8_t ioCap = GAPBOND_IO_CAP_KEYBOARD_ONLY;
    uint8_t oobEnable = FALSE;

    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );

    GAPBondMgr_SetPairable();
}

