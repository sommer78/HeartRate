/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    this is the main file of beacon project
* @details
* @author   hunter_shuai
* @date     23-June-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#include "rtl876x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "dlps_platform.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "profileApi.h"
#include "simpleBLEPeripheral.h"

#include "beacon_application.h"
#include "beacon_service.h"
#include "dis.h"
#include "bas.h"
#include "battery_monitor.h"
#include "rtl876x_io_dlps.h"
#include "board.h"
#include "rtl876x_pinmux.h"
#include "ota_service.h"

/*
********************************************************
* parameter for btstack
*
*
*********************************************************
*/
// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
/* set Beacon advtising interval fixed value*/
#define DEFAULT_ADVERTISING_INTERVAL_MIN            0x320 //0xA0 /* 100ms */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            0x320 // /* 100ms */


// service id for simple profile service
uint8_t gBeaconServiceId;
uint8_t gOTAServiceId;
uint8_t gBASServiceId;
uint8_t gDISServiceId;

#define  GATT_UUID128_BEACON_SERVICE_ADV           0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0x02, 0x00, 0x00
// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[31] =
{
    /* Service */
    17,             /* length     */
    GAP_ADTYPE_128BIT_COMPLETE,            /* type="Complete 128-bit UUIDs available" */
    GATT_UUID128_BEACON_SERVICE_ADV,
    /* BT stack appends Local Name.  */
    0x0b,           /* length     */
    0x09,           /* type="Complete local name" */
    'B', 'e', 'e', '_', 'B', 'e', 'a', 'c', 'o', 'n', /* Bee_Beacon */
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
uint8_t advertData[30] =
{
    /* IBeacon format data */
    0x02,               /* 1:length*/
    0x01,               /* 2:type Flag  */
    0x06,               /* 3:dicoverable mode & not support BR/EDR */
    0x1A,               /* 4:length     */
    0xFF,               /* 5:type="GAP_ADTYPE_MANUFACTURER_SPECIFIC" */
    0x4C, 0x00,         /* 6-7: Apple company Id*/
    0x02, 0x15,         /* 8-9:For all proximity beacon,specify data type & remaining data length*/

    //Wechat test uuid:FDA50693-A4E2-4FB1-AFCF-C6EB07647825, Major: 10, Minor: 7
    0xFD, 0xA5, 0x06, 0x93,
    0xA4, 0xE2, 0x4F, 0xB1,
    0xAF, 0xCF, 0xC6, 0xEB,
    0x07, 0x64, 0x78, 0x25,
    0x00, 0x0A,         /* 26-27:major id*/
    0x00, 0x07,         /* 28-29:minor id*/
    0xC6                /* 30:mesured power*/
};


/******************************************************************
 * @fn         Initial gap parameters
 * @brief      Initialize peripheral and gap bond manager related parameters
 *
 * @return     void
 */
void BtStack_Init_Gap(void)
{
    //device name and device appearance
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "Bee_Beacon";
    uint16_t Appearance = GAP_GATT_APPEARANCE_UNKNOWN;

    //default start adv when bt stack initialized
    uint8_t  advEnableDefault = TRUE;

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;//GAP_ADTYPE_ADV_IND;//GAP_ADTYPE_ADV_NONCONN_IND;
    uint8_t  advDirectType = PEER_ADDRTYPE_PUBLIC;
    uint8_t  advDirectAddr[B_ADDR_LEN] = {0};
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;
    uint16_t advIntMin = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t advIntMax = DEFAULT_ADVERTISING_INTERVAL_MIN;

    //GAP Bond Manager parameters
    uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm = GAPBOND_AUTH_YES_MITM_YES_BOND;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oobEnable = FALSE;
    uint32_t passkey = 0; // passkey "000000"

    uint8_t bUseFixedPasskey = TRUE;

    //Set device name and device appearance
    peripheralSetGapParameter(GAPPRRA_DEVICE_NAME, GAP_DEVICE_NAME_LEN, DeviceName);
    peripheralSetGapParameter(GAPPRRA_APPEARANCE, sizeof(Appearance), &Appearance);

    peripheralSetGapParameter( GAPPRRA_ADV_ENABLE_DEFAULT, sizeof ( advEnableDefault ), &advEnableDefault );

    //Set advertising parameters
    peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR_TYPE, sizeof ( advDirectType ), &advDirectType );
    peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR, sizeof ( advDirectAddr ), advDirectAddr );
    peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );

    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);

    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );
    peripheralSetGapParameter( GAPPRRA_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );

    // Setup the GAP Bond Manager
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );

    GAPBondMgr_SetParameter( GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_FIXED_PASSKEY_ENABLE, sizeof ( uint8_t ), &bUseFixedPasskey);
}

/******************************************************************
 * @fn         Initial profile
 * @brief      Add simple profile service and register callbacks
 *
 * @return     void
 */
void BtProfile_Init(void)
{
    gBeaconServiceId = BeaconService_AddService(AppHandleProfileCallback);
    gOTAServiceId = OTAService_AddService(AppHandleProfileCallback);
    gBASServiceId = BAS_AddService(AppHandleProfileCallback);
    gDISServiceId = DIS_AddService(AppHandleProfileCallback);
    ProfileAPI_RegisterCB(AppHandleProfileCallback);
}

/**
* @brief    Board_Init() contains the initialization of pinmux settings and pad settings.
*
*           All the pinmux settings and pad settings shall be initiated in this function.
*           But if legacy driver is used, the initialization of pinmux setting and pad setting
*           should be peformed with the IO initializing.
*
* @return  void
*/
void Board_Init(void)
{
    All_Pad_Config_Default();
}

/**
* @brief   Driver_Init() contains the initialization of peripherals.
*
*          Both new architecture driver and legacy driver initialization method can be used.
*
* @return  void
*/
void Driver_Init(void)
{
    BatteryMonitor_Init();
}



/**
* @brief    PwrMgr_Init() contains the setting about power mode.
*
* @return  void
*/
void PwrMgr_Init(void)
{
#if CONFIG_DLPS_EN
    DLPS_IO_Register();
    LPS_MODE_Set(LPM_DLPS_MODE);
#endif
}


/**
* @brief  Task_Init() contains the initialization of all the tasks.
*
*           There are four tasks are initiated.
*           Lowerstack task and upperstack task are used by bluetooth stack.
*           Application task is task which user application code resides in.
*           Emergency task is reserved.
*
* @return  void
*/
void Task_Init(void)
{
    void lowerstack_task_init();
    void upperstack_task_init();
    void emergency_task_init();
    application_task_init();
}


/**
* @brief  main() is the entry of user code.
*
*
* @return  void
*/
int main(void)
{
    Board_Init();
    BtStack_Init_Peripheral();
    BtStack_Init_Gap();
    BtProfile_Init();
    PwrMgr_Init();
    Task_Init();
    vTaskStartScheduler();

    return 0;
}

