/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     peripheral_stack_api.c
* @brief    This file provides some demo code for app use
* @details
* @author   ranhui
* @date     2015-03-27
* @version  v0.2
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "bee_message.h"
#include "trace.h"
#include <blueapi.h>
#include "profileApi.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"


/**
* @brief
*  Start undirected advertising
*
* @param   void
* @return  void
*/
void peripheral_StartAdv_ind()
{

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;
    uint16_t advIntMin = 0x80;
    uint16_t advIntMax = 0x80;
    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);



    peripheral_StartAdvertising();
}

/**
* @brief
*  Start undirected advertising with white list
*
* @param   void
* @return  void
*/
void peripheral_StartAdv_ind_with_whitelist()
{

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_WHITE;
    uint16_t advIntMin = 0x80;
    uint16_t advIntMax = 0x80;


    uint8_t  whiteListAddrType = PEER_ADDRTYPE_PUBLIC;
    uint8_t  whiteListAddr[B_ADDR_LEN] = {0};

    //get connected device address and address type
    peripheralGetGapParameter(GAPPRRA_CONN_BD_ADDR_TYPE, &whiteListAddrType);
    peripheralGetGapParameter(GAPPRRA_CONN_BD_ADDR, &whiteListAddr);

    peripheralSetGapParameter( GAPPRRA_ADV_WL_BD_ADDR_TYPE, sizeof ( whiteListAddrType ), &whiteListAddrType );
    peripheralSetGapParameter( GAPPRRA_ADV_WL_BD_ADDR, sizeof ( whiteListAddr ), &whiteListAddr );

    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);



    peripheral_StartAdvertising();
}



/**
* @brief
*  Start scan advertising
*
* @param   void
* @return  void
*/
void peripheral_StartAdv_scan_ind()
{
    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_SCAN_IND;
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;
    uint16_t advIntMin = 0x80;
    uint16_t advIntMax = 0x80;
    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);

    peripheral_StartAdvertising();
}

/**
* @brief
*  Start non connectable undirected advertising
*
* @param   void
* @return  void
*/
void peripheral_StartAdv_noconn_ind()
{
    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_NONCONN_IND;
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;
    uint16_t advIntMin = 0x80;
    uint16_t advIntMax = 0x80;
    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);

    peripheral_StartAdvertising();
}


/**
* @brief
*  Start directed high duty advertising
*
* @param   void
* @return  void
*/
void peripheral_StartAdv_direct_ind_hd()
{
    uint8_t  advEventType = GAP_ADTYPE_ADV_HDC_DIRECT_IND;
    uint8_t  advDirectAddrType = PEER_ADDRTYPE_PUBLIC;
    uint8_t  advDirectAddr[B_ADDR_LEN] = {0};
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;

    //get connected device address and address type
    peripheralGetGapParameter(GAPPRRA_CONN_BD_ADDR_TYPE, &advDirectAddrType);
    peripheralGetGapParameter(GAPPRRA_CONN_BD_ADDR, &advDirectAddr);

    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR_TYPE, sizeof ( advDirectAddrType ), &advDirectAddrType );
    peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR, sizeof ( advDirectAddr ), advDirectAddr );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );


    peripheral_StartAdvertising();
}

/**
* @brief
*  Start directed low duty advertising
*
* @param   void
* @return  void
*/
void peripheral_StartAdv_direct_ind_ld()
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

/**
* @brief
* Stop advertising
*
* @param   void
* @return  void
*/
void peripheral_StopAdv()
{
    peripheral_StopAdvertising();
}


/**
* @brief
* Set device pairable mode
*
* @param
* @return  void
*/
void peripheral_EnablePairing()
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

