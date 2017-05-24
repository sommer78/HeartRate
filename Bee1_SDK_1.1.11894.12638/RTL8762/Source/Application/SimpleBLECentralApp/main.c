/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    main.c
  * @brief   This is the entry of user code which the main function resides in.
  * @details
  * @author  ranhui
  * @date    2015-03-29
  * @version v1.0
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include <string.h>
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "dlps_platform.h"
#include "central.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "profileApi.h"
#include "simple_ble_central_application.h"
#include "simple_ble_central.h"
#include "profile_client.h"
#include "simple_ble_client.h"
#include "user_cmd.h"
#include "data_uart.h"

/** Default parameters for BT stack **/
// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80
// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800
// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0
// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000
// Connect request scan timeout value (units of 10ms, 1000=10s)
#define DEFAULT_CONN_SCAN_TIMEOUT             1000

/**<  Client ID for Simple BLE Client module, get from lower layer when add Simple BLE Client module. */
TClientID   SimBleClientID;

/**
  * @brief  Initiate GAP parameters, these params will affect the local device's behavior.
  * @retval none.
  */
void BtStack_Init_Gap()
{
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "Bee_central";
    uint16_t Appearance = GAP_GATT_APPEARANCE_UNKNOWN;

    uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm = GAPBOND_AUTH_NO_MITM_YES_BOND;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oobEnable = FALSE;
    uint32_t passkey = 0; // passkey "000000"
    uint8_t ScanFilterDuplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;
    uint16_t Conn_ScanTimeout = DEFAULT_CONN_SCAN_TIMEOUT;
    
    centralSetGapParameter( GAPPARA_FILTERDUPLICATES, sizeof ( ScanFilterDuplicate ), &ScanFilterDuplicate );
    centralSetGapParameter(GAPPRRA_CONN_SCAN_TIMEOUT, sizeof(Conn_ScanTimeout), &Conn_ScanTimeout);

    centralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
    centralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
    centralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
    centralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );

    centralSetGapParameter( GAPPRRA_DEVICE_NAME, GAP_DEVICE_NAME_LEN, DeviceName);
    centralSetGapParameter( GAPPRRA_APPEARANCE, sizeof(Appearance), &Appearance);

    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );

    GAPBondMgr_SetParameter( GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey );
}

/**
  * @brief  Initiate Profile Client.
  * @retval none.
  */
void BtProfile_Init(void)
{
    /* Add Simple BLE Client Module, App_ClientCallback used to handle events from this Module. */
    SimBleClientID = SimpBle_AddClient( App_ClientCallback );
    /* Register app callback--App_CentralRoleCallback to handle events from GAP Central Role. */
    central_RegisterAppDirectCB( App_CentralRoleCallback );
    /* Register this callcack--App_ClientCallback to handle events from Profile Client layer. */
    clientAPI_RegisterAppCB( App_ClientCallback );
}

/**
  * @brief  Board_Init() contains the initialization of pinmux settings and pad settings.
  *
  * NOTE: All the pinmux settings and pad settings shall be initiated in this function.
  * But if legacy driver is used, the initialization of pinmux setting and pad setting
  * should be peformed with the IO initializing. 
  *
  * @retval  none.
  */
void Board_Init()
{
    
}

/**
  * @brief  Driver_Init() contains the initialization of peripherals.
  *         NOTE: Both new architecture driver and legacy driver initialization method can be used.
  * @retval  none.
  */
void Driver_Init()
{
    /* We use Data UART to handle user commands. */
    dataUART_Init();
    User_CmdInit(&gUserCmdIF);
}

/**
  * @brief  PwrMgr_Init() contains the setting about power mode.
  * @retval  none.
  */
void PwrMgr_Init()
{
	LPS_MODE_Pause();
}

/**
  * @brief  Task_Init() contains the initialization of all the tasks.
  *
  * NOTE: There are four tasks are initiated.
  * Lowerstack task and upperstack task are used by bluetooth stack.
  * Application task is task which user application code resides in.
  * Emergency task is reserved.
  * 
  * @retval  none.
  */
void Task_Init()
{
	void lowerstack_task_init();
	void upperstack_task_init();
	void emergency_task_init();
	application_task_init();
}

/**
  * @brief  main() is the entry of user code.
  * @retval  none.
  */
int main(void)
{
    Board_Init();
    Driver_Init();
    BtStack_Init_Central();
    BtStack_Init_Gap();
    BtProfile_Init();
    PwrMgr_Init();
    Task_Init();
    vTaskStartScheduler();

    return 0;
}

