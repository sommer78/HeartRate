/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief   this is the main file of DataTrans project
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "dlps_platform.h"
#include <string.h>

#include "peripheral.h"
#include "central.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "central_application.h"
#include "simple_ble_central.h"
#include "simpleBLEPeripheral.h"
#include "profileApi.h"

#include "dataTrans_application.h"
#include "module_param_config.h"
#include "dataTrans_uart.h"
#include "trace.h"
#include "rtl876x_pinmux.h"
#include "dataTrans_profile.h"
#include "dev_info_profile.h"

#include "rtl876x_io_dlps.h"

#include "profile_client.h"
#include "datatrans_client.h"

TClientID   DtClientID;


/*
********************************************************
* parameter for btstack
*
*
*********************************************************
*/


#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 16=20ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     16

// Maximum connection interval (units of 1.25ms, 16=20ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     16

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE




void BtStack_Init_Gap()
{
    //device name and device appearance
    uint16_t Appearance = GAP_GATT_APPEARANCE_UNKNOWN;


    //default start adv when bt stack initialized
    uint8_t  advEnableDefault = TRUE;

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;
    uint8_t  advDirectType = PEER_ADDRTYPE_PUBLIC;
    uint8_t  advDirectAddr[B_ADDR_LEN] = {0};
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;
    uint16_t advIntMin = 480;   /*  What is the advertising interval when the device is discoverable(uints of 625us, 480=300ms) */
    uint16_t advIntMax = 480;


    //connection update parameters
    uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    //GAP Bond Manager parameters
    uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm = GAPBOND_AUTH_YES_MITM_YES_BOND;
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8_t oobEnable = FALSE;
    uint32_t passkey = gConfigParam->pin.pin;

    uint8_t bUseFixedPasskey = TRUE;
    uint8_t ScanFilterDuplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;     /* only for central role */


    if (gConfigParam->gaprole.gaprole == CENTRAL_ROLE)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BtStack_Init_Gapl(): Central", 0);
#endif

        centralSetGapParameter( GAPPARA_FILTERDUPLICATES, sizeof ( ScanFilterDuplicate ), &ScanFilterDuplicate );

        centralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
        centralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
        centralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
        centralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );


        centralSetGapParameter(GAPPRRA_DEVICE_NAME, strlen((char *)gConfigParam->localName.Local_name) + 1, gConfigParam->localName.Local_name);
        centralSetGapParameter(GAPPRRA_APPEARANCE, sizeof(Appearance), &Appearance);
    }
    else if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BtStack_Init_Gapl(): peripheral", 0);
#endif

        moduleParam_InitAdvAndScanRspData();

        //Set device name and device appearance
        peripheralSetGapParameter(GAPPRRA_DEVICE_NAME, strlen((char *)gConfigParam->localName.Local_name) + 1, gConfigParam->localName.Local_name);
        peripheralSetGapParameter(GAPPRRA_APPEARANCE, sizeof(Appearance), &Appearance);

        //Set advertising parameters
        peripheralSetGapParameter( GAPPRRA_ADV_ENABLE_DEFAULT, sizeof ( advEnableDefault ), &advEnableDefault );
        peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
        peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR_TYPE, sizeof ( advDirectType ), &advDirectType );
        peripheralSetGapParameter( GAPPRRA_ADV_DIRECT_ADDR, sizeof ( advDirectAddr ), advDirectAddr );
        peripheralSetGapParameter( GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
        peripheralSetGapParameter( GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );
        peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
        peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);

        //set connection update parameters
        peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
        peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
        peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
        peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
    }


    // Setup the GAP Bond Manager
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );
    GAPBondMgr_SetParameter( GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_FIXED_PASSKEY_ENABLE, sizeof ( uint8_t ), &bUseFixedPasskey);

}





void BtProfile_Init(void)
{
    if (gConfigParam->gaprole.gaprole == CENTRAL_ROLE)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BtProfile_Init(): Central", 0);
#endif

        DtClientID = Dt_AddClient( App_ClientCallback );

        //SimBleClientID = SimpBle_AddClient();
        central_RegisterAppDirectCB( App_CentralRoleCallback );
        clientAPI_RegisterAppCB( App_ClientCallback );
    }
    else if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BtProfile_Init(): Peripheral", 0);
#endif
        DataTrans_AddService(NULL);
        DIS_AddService(NULL);
        ProfileAPI_RegisterCB(AppHandleGATTCallback);
    }
}




/**
* @brief  Board_Init() contains the initialization of pinmux settings and pad settings.
*
* All the pinmux settings and pad settings shall be initiated in this function.
* But if legacy driver is used, the initialization of pinmux setting and pad setting
* should be peformed with the IO initializing.
*
* @param  No parameter.
* @return  void
*/
void Board_Init()
{
    All_Pad_Config_Default();
    g_AppCB = &g_TappCB;
    memset(g_AppCB, 0, sizeof(TAPP_TCB));
    g_AppBufCB = &g_TappbufCB;
    memset(g_AppBufCB, 0, sizeof(TAPP_BUF_TCB));
    moduleParam_LoadFromFlash();

}




/**
* @brief  Driver_Init() contains the initialization of peripherals.
*
* Both new architecture driver and legacy driver initialization method can be used.
*
* @param   No parameter.
* @return  void
*/
void Driver_Init()
{
    UARTInit();
}

void DataTrans_DLPS_Enter(void)
{
    Pad_Config(P3_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

}

/**
* @brief   DataTrans Exit DLPS callback function
* @param   No parameter.
* @return  void
*/
void DataTransExitDlpsInit(void)
{
    //#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DataTransExitDlpsInit", 0);
    //#endif

    /*  reconfig UART register  */
    Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    //Pad_Config(P3_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    //Pad_Config(P2_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
}



BOOL DLPS_DataTransCheck()
{
    return allowedDataTransEnterDlps;
}




/**
* @brief  PwrMgr_Init() contains the setting about power mode.
*
* @param   No parameter.
* @return  void
*/
void PwrMgr_Init()
{
    DLPS_IO_Register();

    if (FALSE == DLPS_ENTER_CHECK_CB_REG(DLPS_DataTransCheck))
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Error: DLPS_ENTER_CHECK_CB_REG(DLPS_DataTransCheck) failed!\n", 0);
#endif
    }

    DLPS_IO_RegUserDlpsEnterCb(DataTrans_DLPS_Enter);
    DLPS_IO_RegUserDlpsExitCb(DataTransExitDlpsInit);
    LPS_MODE_Set(LPM_DLPS_MODE);

    /*  whether enter DLPS or not   when power on   */
    if ( gConfigParam->is_enter_sleep.is_enter_sleep)
    {
        allowedDataTransEnterDlps = FALSE;  /*  not enter DLPS when power on    */
    }
    else
    {
        allowedDataTransEnterDlps = TRUE;   /*  enter DLPS when power on    */
        gConfigParam->isSleeping = TRUE;
    }
}


/**
* @brief  Task_Init() contains the initialization of all the tasks.
*
* There are five tasks are initiated.
* Lowerstack task and upperstack task are used by bluetooth stack.
* Emergency task is reserved
* Another two are tasks which user application code resides in.
*
* @param   No parameter.
* @return  void
*/
void Task_Init()
{
    void lowerstack_task_init();
    void upperstack_task_init();
    void emergency_task_init();
    ApplicationInit();
    application_task_init();
}



/**
* @brief  main() is the entry of user code.
*
*
* @param   No parameter.
* @return  void
*/
int main(void)
{
    Board_Init();
    if (gConfigParam->gaprole.gaprole == CENTRAL_ROLE)
    {
        BtStack_Init_Central();
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BtStack_Init_Central()", 0);
#endif
    }
    else if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
    {
        BtStack_Init_Peripheral();
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "BtStack_Init_Peripheral()", 0);
#endif
    }

    BtStack_Init_Gap();
    BtProfile_Init();
    Task_Init();
    vTaskStartScheduler();

    return 0;
}

