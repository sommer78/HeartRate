/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    main.c
  * @brief   This is the entry of user code which the main function resides in.
    * @details
  * @author  Chuanguo Xue
  * @date    29-March-2015
  * @version v0.2
  ******************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

#include "rtl876x.h"
#include "rtl876x_rcc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "board.h"
#include "profile_init.h"
#include "dlps_platform.h"
#include "rtl876x_pinmux.h"
#include "pxp_io_new.h"
#include <string.h>
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"

#include "profileApi.h"
#include "ias.h"
#include "lls.h"
#include "tps.h"
#include "pxp_ext.h"
#include "bas.h"
#include "dis.h"
#include "rtl876x_io_dlps.h"
#include "pxp_application.h"
#include "simpleBLEPeripheral.h"

#include "legacyperiphapi.h"
#include "timers.h"


/* parameter for btstack*/
// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL_MIN            0x120          /* 180ms */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            0x130          /* 190ms */
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     120
// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800
// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0
// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

/* pxp application related services' ID. */
uint8_t gIASServiceId;
uint8_t gLLSServiceId;
uint8_t gTPPServiceId;
uint8_t gPXPEXTServiceId;
uint8_t gBASServiceId;
uint8_t gDISServiceId;
extern TimerHandle_t xTimersConParaUpdt;
extern void vTimerConParaCallback(xTimerHandle handle);

#if ADVERTISING_LED
extern bool IsAdvLED_ON;
extern TimerHandle_t xTimersAdvLED;
extern void vTimerAdvLEDCallback(xTimerHandle handle);
#endif

BOOL allowedProximityEnterDlps = true;
#if 0
// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
    0x03,           /* length     */
    0x03,           /* type="More 16-bit UUIDs available" */
    0x02,
    0x18,
    /* place holder for Local Name, filled by BT stack. if not present */
    /* BT stack appends Local Name.                                    */
    0x03,           /* length     */
    0x19,           /* type="Appearance" */
    0x40, 0x02,     /* Keyring */
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
    /* Core spec. Vol. 3, Part C, Chapter 18 */
    /* Flags */
    0x02,            /* length     */
    //XXXXMJMJ 0x01, 0x06,      /* type="flags", data="bit 1: LE General Discoverable Mode", BR/EDR not supp. */
    0x01, 0x06,      /* type="flags", data="bit 1: LE General Discoverable Mode" */
    /* Service */
    0x03,           /* length     */
    0x03,           /* type="More 16-bit UUIDs available" */
    0x02,
    0x18,
    /* place holder for Local Name, filled by BT stack. if not present */
    /* BT stack appends Local Name.                                    */
    0x03,           /* length     */
    0x19,           /* type="Appearance" */
    0x40, 0x02,     /* Keyring */
    0x09,           /* length     */
    0x09,           /* type="Complete local name" */
    'B', 'L', 'B', '_', 'P', 'R', 'O', 'X' /* BLB_PROX */
};
#else
// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
    /* BT stack appends Local Name.                                    */
    0x03,           /* length     */
    0x19,           /* type="Appearance" */
    0x40, 0x02,     /* Keyring */

    /* place holder for Local Name, filled by BT stack. if not present */
    0x09,           /* length     */
    0x09,           /* type="Complete local name" */
    'B', 'L', 'B', '_', 'P', 'R', 'O', 'X' /* BLB_PROX */
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
    /* Core spec. Vol. 3, Part C, Chapter 18 */
    /* Flags */
    0x02,       /* length     */
    0x01, 0x06, /* type="flags", data="bit 1: LE General Discoverable Mode" */

//    /* Adv data encrypt sample*/
//    0x07,       /* length     */
//    GAP_ADTYPE_MANUFACTURER_SPECIFIC, /* AD Type */
//    0x5d, 0x00, /*Company ID  */
//    'D', 'A', 'T', 'A', /* data */

    /* Service */
    0x11,       /* length     */
    0x07,       /* type="Complete 128-bit UUIDs available" */
    0xA6, 0xF6, 0xF6, 0x07,
    0x4D, 0xC4, 0x9D, 0x98,
    0x6D, 0x45, 0x29, 0xBB,
    0xD0, 0xFF, 0x00, 0x00
};
#endif


void BtStack_Init_Gap()
{

    {
        uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
        uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
        uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
        uint16_t desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;


        peripheralSetGapParameter( GAPPRRA_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
        peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );

        peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
        peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
        peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
        peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
    }

    {
        uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "BLB_PROX";
        uint16_t Appearance = GAP_GATT_APPEARANCE_GENERIC_KEYRING;

        peripheralSetGapParameter(GAPPRRA_DEVICE_NAME, GAP_DEVICE_NAME_LEN, DeviceName);
        peripheralSetGapParameter(GAPPRRA_APPEARANCE, sizeof(Appearance), &Appearance);
    }

    // Set advertising interval
    {
        uint16_t advIntMin = DEFAULT_ADVERTISING_INTERVAL_MIN;
        uint16_t advIntMax = DEFAULT_ADVERTISING_INTERVAL_MIN;
        //uint8_t  advEventType = GAP_ADTYPE_ADV_NONCONN_IND;
        peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin);
        peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax);
        //peripheralSetGapParameter( GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    }

    // Setup the GAP Bond Manager
    {
        uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
        uint8_t mitm = GAPBOND_AUTH_NO_MITM_YES_BOND;
        uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
        //uint8_t oobEnable = FALSE;
        //uint32_t passkey = 0; // passkey "000000"

        GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
        GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
        GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
        //GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );
        //GAPBondMgr_SetParameter( GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey );
    }
}

void BtProfile_Init(void)
{

    gIASServiceId = IAS_AddService(AppHandleGATTCallback);
    gLLSServiceId = LLS_AddService(AppHandleGATTCallback);
    gTPPServiceId = TPS_AddService(AppHandleGATTCallback);
    gPXPEXTServiceId = PXPExtended_AddService(AppHandleGATTCallback);
    gBASServiceId = BAS_AddService(AppHandleGATTCallback);
    gDISServiceId = DIS_AddService(AppHandleGATTCallback);
    ProfileAPI_RegisterCB(AppHandleGATTCallback);
}

/**
  * @brief  Board_Init() contains the initialization of pinmux settings and pad settings.
  *         All the pinmux settings and pad settings shall be initiated in this function.
  *         But if legacy driver is used, the initialization of pinmux setting and pad setting
  *         should be peformed with the IO initializing.
  * @param  None
  * @retval None
  */
void Board_Init()
{
    /* Setting all pins to the default state */
    All_Pad_Config_Default();

    /* PXP pin setting */
    {
        Pinmux_Config(KEY_0, GPIO_FUN);
        Pinmux_Config(BEEP, GPIO_FUN);
        Pinmux_Config(LED_0, GPIO_FUN);

        Pad_Config(KEY_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        Pad_Config(BEEP, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(LED_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

#if ADVERTISING_LED
        Pinmux_Config(ADV_LED, GPIO_FUN);
        Pad_Config(ADV_LED, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif
    }

}

/**
  * @brief  Driver_Init() contains the initialization of peripherals.
  *         Both new architecture driver and legacy driver initialization method can be used.
  * @param  None
  * @retval None
  */
void Driver_Init()
{
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    /* Define GPIO parameter structure. KEY Beep LED is configed as GPIO. */
    GPIO_InitTypeDef GPIO_Param;
    GPIO_Param.GPIO_Pin = GPIO_GetPin(LED_0) | GPIO_GetPin(BEEP);
    GPIO_Param.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Param.GPIO_ITCmd = DISABLE;
    GPIO_Init(&GPIO_Param);

#if ADVERTISING_LED
    GPIO_Param.GPIO_Pin = GPIO_GetPin(ADV_LED);
    GPIO_Param.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Param.GPIO_ITCmd = DISABLE;
    GPIO_Init(&GPIO_Param);
    GPIO_ResetBits(GPIO_GetPin(ADV_LED));
    xTimersAdvLED = xTimerCreate((const char *)"AdvLED_Timer", \
                                 1000 / portTICK_RATE_MS, pdFALSE, NULL, vTimerAdvLEDCallback);
#endif

    GPIO_StructInit(&GPIO_Param);
    GPIO_Param.GPIO_Pin = GPIO_GetPin(KEY_0);
    GPIO_Param.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Param.GPIO_ITCmd = ENABLE;
    GPIO_Param.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
    GPIO_Param.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_Init(&GPIO_Param);
    KEY_Init(&GPIO_Param);
    ADC_Battery_Init();
    RTC_Initconfig();
    xTimersConParaUpdt = xTimerCreate((const char *)"ConPara_Timer", \
                                      1000 / portTICK_RATE_MS, pdFALSE, NULL, vTimerConParaCallback);

}

/**
  * @brief  Proximity_DLPS_Enter() configs the pad before the system enter DLPS.
  *         Pad should be set PAD_SW_MODE and PAD_PULL_UP/PAD_PULL_DOWN.
  * @param  None
  * @retval None
  */
void Proximity_DLPS_Enter(void)
{

    Pad_Config(KEY_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(BEEP, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LED_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);

#if ADVERTISING_LED

    if (IsAdvLED_ON)
    {
        Pad_Config(ADV_LED, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(ADV_LED, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }

#endif

    System_WakeUp_Pin_Enable(KEY_0, 0);

}

/**
  * @brief  Proximity_DLPS_Exit() config the pad after the system exit DLPS.
  *         Pad should be set PAD_PINMUX_MODE.
  * @param  None
  * @retval None
  */
void Proximity_DLPS_Exit(void)
{

    Pad_Config(KEY_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(BEEP, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LED_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

#if ADVERTISING_LED
    Pad_Config(ADV_LED, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif

}


BOOL DLPS_ProximityCheck()
{
    return allowedProximityEnterDlps;
}

/**
  * @brief  PwrMgr_Init() contains the setting about power mode.
  * @param  None
  * @retval None
  */
void PwrMgr_Init()
{
    System_WakeUp_Pin_Enable(KEY_0, 0);

#if CONFIG_DLPS_EN
    DLPS_IO_Register();

    if (FALSE == DLPS_ENTER_CHECK_CB_REG(DLPS_ProximityCheck))
    {
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Error: DLPS_ENTER_CHECK_CB_REG(DLPS_ProximityCheck) failed!\n", 0);
    }
    DLPS_IO_RegUserDlpsEnterCb(Proximity_DLPS_Enter);
    DLPS_IO_RegUserDlpsExitCb(Proximity_DLPS_Exit);
    LPS_MODE_Set(LPM_DLPS_MODE);
#else
    LPS_MODE_Pause();
#endif
}

/**
  * @brief  Task_Init() contains the initialization of all the tasks.
  *         There are four tasks are initiated.
  *         Lowerstack task and upperstack task are used by bluetooth stack.
  *         Application task is task which user application code resides in.
  *         Emergency task is reserved.
  * @param  None
  * @retval None
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
  * @param  None
  * @retval int
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
