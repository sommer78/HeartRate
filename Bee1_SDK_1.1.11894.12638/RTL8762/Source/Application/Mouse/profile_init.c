/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     profile_init.c
* @brief    .
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v0.2
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "bee_message.h"
#include "trace.h"
#include <blueapi.h>


#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
//#include "bcomdef.h"      /* This file has been removed by Ranhui.*/

BOOL Adv_Enable_Default = FALSE;
uint8_t is_bluetooth_active = FALSE;
uint8_t bluetooth_stack_state = blueAPI_EventIdle;


extern uint8_t advertising_interval;
extern uint8_t RemoteBd[6];
#define PAIRING_MODE 3 // JUST_WORKS: 3 (blueAPI_IOCapNoIO)


/**
* @brief
*
*
* @param   void
* @return  void
*/
void MouseStartAdv()
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
void MouseStopAdv()
{
    peripheral_StopAdvertising();
}


/**
* @brief
*
*
* @param
* @return  void
*/
void MouseEnablePairing()
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


/**
* @brief
*  Start undirected advertising
*
* @param   void
* @return  void
*/
void Mouse_StartAdv_ind()
{

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;
    uint16_t advIntMin = 0x83;//0x80
    uint16_t advIntMax = 0x83;//0x80
    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);



    peripheral_StartAdvertising();
}


/**
* @brief
*  Start directed low duty advertising
*
* @param   void
* @return  void
*/
void Mouse_StartAdv_direct_ind_hd()
{
    uint8_t  advEventType = GAP_ADTYPE_ADV_LDC_DIRECT_IND;
    uint8_t  advDirectAddrType = PEER_ADDRTYPE_PUBLIC;
    uint8_t  advDirectAddr[B_ADDR_LEN] = {0};

    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint16_t advIntMin = 0x80;
    uint16_t advIntMax = 0x80;

    //get connected device address and address type
    peripheralGetGapParameter(GAPPRRA_CONN_BD_ADDR_TYPE, &advDirectAddrType);
    peripheralGetGapParameter(GAPPRRA_CONN_BD_ADDR, &advDirectAddr);


    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR_TYPE, sizeof ( advDirectAddrType ), &advDirectAddrType );
    peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR, sizeof ( advDirectAddr ), advDirectAddr );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);

    peripheral_StartAdvertising();
}


