/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    this is the main file of OTA_demo project
* @details
* @author   hunter_shuai
* @date     14-July-2015
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

#include "ota_application.h"
#include "ota_service.h"
#include "battery_monitor.h"
#include "bas.h"
#include "dis.h"

/*
********************************************************
* parameter for btstack
*
*
*********************************************************
*/
// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL_MIN            0x20 /* 20ms */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            0x30 /* 30ms */


// service id
uint8_t gOTAServiceId;
uint8_t gBASServiceId;
uint8_t gDISServiceId;

#define  GATT_UUID128_OTA_SERVICE_ADV             0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0xD0, 0x00, 0x00


// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
    /* place holder for Local Name, filled by BT stack. if not present */
    /* BT stack appends Local Name.                                    */
    0x03,           /* length     */
    0x19,           /* type="Appearance" */
    0x00, 0x00,     /* Unknown */
    /* place holder for Local Name, filled by BT stack. if not present */
    /* BT stack appends Local Name.                                    */
    0x08,           /* length     */
    0x09,           /* type="Complete local name" */
    'B', 'e', 'e', '_', 'O', 'T', 'A' /* Bee_OTA*/
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
uint8_t advertData[] =
{
    /* Core spec. Vol. 3, Part C, Chapter 18 */
    /* Flags */
    0x02,            /* length     */
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    /* Service */
    17,             /* length     */
    GAP_ADTYPE_128BIT_MORE,            /* type="Complete 128-bit UUIDs available" */
    GATT_UUID128_OTA_SERVICE_ADV
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
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "Bee_OTA";
    uint16_t Appearance = GAP_GATT_APPEARANCE_UNKNOWN;

    //default start adv when bt stack initialized
    uint8_t  advEnableDefault = TRUE;

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;
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
    GAPBondMgr_SetParameter(GAPBOND_FIXED_PASSKEY_ENABLE, sizeof ( uint8_t ), &bUseFixedPasskey);
}

/******************************************************************
 * @fn         Initial profile
 * @brief      Add simple profile service and register callbacks
 *
 * @return     void
 */
void BtProfile_Init(void)
{
    gOTAServiceId = OTAService_AddService(AppProfileCallback);
    gBASServiceId = BAS_AddService(AppProfileCallback);
    gDISServiceId = DIS_AddService(AppProfileCallback);
    ProfileAPI_RegisterCB(AppProfileCallback);
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

    //LPS_MODE_Set(LPM_DLPS_MODE);
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

