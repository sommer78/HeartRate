/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This is the entry of user code which the main function resides in.
* @details
* @author   Chuanguo Xue
* @date     2015-03-29
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "board.h"
#include "profile_init.h"
#include "dlps_platform.h"
#include "keyscan.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "keyboard_application.h"
#include "simpleBLEPeripheral.h"
#include "bas.h"
#include "dis.h"
#include "sps.h"
#include "hids_keyboard.h"
#include "profileApi.h"
#include "legacyperiphapi.h"
#include "rtl876x_io_dlps.h"
#include "os.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_tim.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "keyboardbutton.h"

/* Function declaration -------------------------------------------------*/
extern BOOL IsAbleToSendAction;
#ifdef MULTIMEDIA_KEYBOARD
extern BOOL IsMmKbToSendAction;
#endif
extern VOID (*SPI_Initialize_V2)(IN VOID *Data);

/* Global variables defines -----------------------------------------------------------*/
uint8_t gHIDServiceId;
uint8_t gBASServiceId;
uint8_t gDISServiceId;
uint8_t gSPSServiceId;
/* control of allowing enter DLPS mode or not */
BOOL allowedKeyboardEnterDlps = true;
TimerHandle_t xTimersPairing;
TimerHandle_t xTimersEnableSend;
extern void vTimerPairingCallback(xTimerHandle handle);
extern void vTimerEnableSendCallback(xTimerHandle handle);

/* parameter for btstack -----------------------------------------------------------*/

// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL_MIN            0x83        //0x20 /* 20ms */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            0x83        //0x30 /* 30ms */
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80
// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800
// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         10
// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
    0x03,           /* length     */
    0x03,           /* type="More 16-bit UUIDs available" */
    0x12,
    0x18,
    /* place holder for Local Name, filled by BT stack. if not present */
    /* BT stack appends Local Name.                                    */
    0x03,           /* length     */
    0x19,           /* type="Appearance" */
    0xc1, 0x03,     /* Keyboard */
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8_t advertData[] =
{
    /* Core spec. Vol. 3, Part C, Chapter 18 */
    /* Flags */
    0x02,            /* length     */
    //XXXXMJMJ 0x01, 0x06,      /* type="flags", data="bit 1: LE General Discoverable Mode", BR/EDR not supp. */
    0x01, 0x05,      /* type="flags", data="bit 1: LE General Discoverable Mode" */
    /* Service */
    0x03,           /* length     */
    0x03,           /* type="More 16-bit UUIDs available" */
    0x12,
    0x18,
    /* place holder for Local Name, filled by BT stack. if not present */
    /* BT stack appends Local Name.                                    */
    0x03,           /* length     */
    0x19,           /* type="Appearance" */
    0xc1, 0x03,     /* Keyboard */
    0x0D,           /* length     */
    0x09,           /* type="Complete local name" */
    'K', 'e', 'y', 'b', 'o', 'a', 'r', 'd', '_', 'Q', 'W', 'E' /* Keyboard Name */
};

void BtStack_Init_Gap()
{
    //device name and device appearance
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "Keyboard_QWE";
    uint16_t Appearance = GAP_GATT_APPEARANCE_KEYBOARD;

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

    //connection update parameters
    uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    //GAP Bond Manager parameters
    uint8_t pairMode = GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm = GAPBOND_AUTH_YES_MITM_YES_BOND;

    uint8_t ioCap = GAPBOND_IO_CAP_KEYBOARD_ONLY;
    uint8_t oobEnable = FALSE;
    uint32_t passkey = 0; // passkey "000000"

    uint8_t secReqEnable = TRUE;
    uint8_t secReqRequirement = GAPBOND_SEC_REQ_NO_MITM;

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

    //set connection update parameters
    peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
    peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
    peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
    peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );

    // Setup the GAP Bond Manager
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );

    GAPBondMgr_SetParameter( GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey );

    GAPBondMgr_SetParameter(GAPBOND_SEC_REQ_ENABLE, sizeof ( uint8_t ), &secReqEnable);
    GAPBondMgr_SetParameter(GAPBOND_SEC_REQ_REQUIREMENT, sizeof ( uint8_t ), &secReqRequirement);

    // check bonded device exist or not
    remote_BD_struct BondedDevice;
    if (!GAPBondMgr_GetBondedDevice(&BondedDevice))
    {
        IsAbleToSendAction = false;
#ifdef MULTIMEDIA_KEYBOARD
        IsMmKbToSendAction = false;
#endif
    }
}

void BtProfile_Init(void)
{
    gHIDServiceId = HIDS_AddService(AppProfileCallback);

    if (gHIDServiceId == 0xff)
    {
        //error.
        return;
    }
    gBASServiceId = BAS_AddService(AppProfileCallback);
    gDISServiceId = DIS_AddService(AppProfileCallback);
    gSPSServiceId = SPS_AddService(AppProfileCallback);
    ProfileAPI_RegisterCB(AppProfileCallback);
}

/**
* @brief  Board_Init() contains the initialization of pinmux settings and pad settings.
*
* All the pinmux settings and pad settings shall be initiated in this function.
* But if legacy driver is used, the initialization of pinmux setting and pad setting
* should be peformed with the IO initializing.
*
* @param   No parameter.
* @return  void
*/
void Board_Init()
{
    All_Pad_Config_Default();

#if ENABLE_SWITCH_BUTTON
    /* Keyboard switch button  setting */
    Pinmux_Config(KEYBOARD_SWITCH_BUTTON_INDEX, GPIO_FUN);
    Pad_Config(KEYBOARD_SWITCH_BUTTON_INDEX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
#endif

    /* Keypad pin setting */
    Pinmux_Config(KEYPAD_ROW0, KEY_ROW_0);
    Pinmux_Config(KEYPAD_ROW1, KEY_ROW_1);
    Pinmux_Config(KEYPAD_ROW2, KEY_ROW_2);
    Pinmux_Config(KEYPAD_ROW3, KEY_ROW_3);
    Pinmux_Config(KEYPAD_ROW4, KEY_ROW_4);
    Pinmux_Config(KEYPAD_ROW5, KEY_ROW_5);
    Pinmux_Config(KEYPAD_ROW6, KEY_ROW_6);
    Pinmux_Config(KEYPAD_ROW7, KEY_ROW_7);
    Pinmux_Config(KEYPAD_COLUMN0, KEY_COL_0);
    Pinmux_Config(KEYPAD_COLUMN1, KEY_COL_1);
    Pinmux_Config(KEYPAD_COLUMN2, KEY_COL_2);
    Pinmux_Config(KEYPAD_COLUMN3, KEY_COL_3);
    Pinmux_Config(KEYPAD_COLUMN4, KEY_COL_4);
    Pinmux_Config(KEYPAD_COLUMN5, KEY_COL_5);
    Pinmux_Config(KEYPAD_COLUMN6, KEY_COL_6);
    Pinmux_Config(KEYPAD_COLUMN7, KEY_COL_7);
    Pinmux_Config(KEYPAD_COLUMN8, KEY_COL_8);
    Pinmux_Config(KEYPAD_COLUMN9, KEY_COL_9);
    Pinmux_Config(KEYPAD_COLUMN10, KEY_COL_10);
    Pinmux_Config(KEYPAD_COLUMN11, KEY_COL_11);
    Pinmux_Config(KEYPAD_COLUMN12, KEY_COL_12);
    Pinmux_Config(KEYPAD_COLUMN13, KEY_COL_13);
    Pinmux_Config(KEYPAD_COLUMN14, KEY_COL_14);
    Pinmux_Config(KEYPAD_COLUMN15, KEY_COL_15);
    Pinmux_Config(KEYPAD_COLUMN16, KEY_COL_16);
    Pinmux_Config(KEYPAD_COLUMN17, KEY_COL_17);
#ifndef ATANS_KEYBOARD
    Pinmux_Config(KEYPAD_COLUMN18, KEY_COL_18);
    Pinmux_Config(KEYPAD_COLUMN19, KEY_COL_19);
#endif

    Pad_Config(KEYPAD_ROW0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN8, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN9, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN10, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN11, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN12, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN13, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN14, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN15, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN16, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN17, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#ifndef ATANS_KEYBOARD
    Pad_Config(KEYPAD_COLUMN18, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN19, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif

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
    RCC_PeriphClockCmd(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, ENABLE);
    KeyPad_Init();

#if ENABLE_SWITCH_BUTTON
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    KeyboardButton_Init();
#endif

    xTimersPairing = xTimerCreate((const char *)"Pairing_Timer", \
                                  10000 / portTICK_RATE_MS, pdFALSE, NULL, vTimerPairingCallback);
    xTimersEnableSend = xTimerCreate((const char *)"EnableSend_Timer", \
                                     5000 / portTICK_RATE_MS, pdFALSE, NULL, vTimerEnableSendCallback);

}
void KeyboardEnterDlpsSet(void)
{
    Pad_Config(KEYPAD_ROW0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW4, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW6, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW7, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pad_Config(KEYPAD_COLUMN0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN4, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN6, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN7, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN8, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN9, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN10, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN11, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN12, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN13, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN14, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN15, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN16, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN17, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#ifndef ATANS_KEYBOARD
    Pad_Config(KEYPAD_COLUMN18, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN19, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif

    /* Config wakeup pins which can wake up system from dlps mode */
#if ENABLE_SWITCH_BUTTON
    Pad_Config(KEYBOARD_SWITCH_BUTTON_INDEX, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    System_WakeUp_Pin_Enable(KEYBOARD_SWITCH_BUTTON_INDEX, 0);
#endif
    System_WakeUp_Pin_Enable(KEYPAD_ROW0, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW1, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW2, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW3, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW4, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW5, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW6, 0);
    System_WakeUp_Pin_Enable(KEYPAD_ROW7, 0);
}

void KeyboardExitDlpsInit(void)
{

#if ENABLE_SWITCH_BUTTON
    /* Keyboard switch button  setting */
    Pad_Config(KEYBOARD_SWITCH_BUTTON_INDEX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
#endif

    Pad_Config(KEYPAD_ROW0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_ROW7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pad_Config(KEYPAD_COLUMN0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN8, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN9, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN10, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN11, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN12, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN13, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN14, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN15, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN16, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN17, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#ifndef ATANS_KEYBOARD
    Pad_Config(KEYPAD_COLUMN18, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(KEYPAD_COLUMN19, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
#endif

}

BOOL DLPS_KeyboardCheck()
{
    return allowedKeyboardEnterDlps;
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

    /* register of call back function */
    if (FALSE == DLPS_ENTER_CHECK_CB_REG(DLPS_KeyboardCheck))
    {
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Error: DLPS_ENTER_CHECK_CB_REG(DLPS_KeyboardCheck) failed!\n", 0);
    }
    DLPS_IO_RegUserDlpsEnterCb(KeyboardEnterDlpsSet);
    DLPS_IO_RegUserDlpsExitCb(KeyboardExitDlpsInit);
    LPS_MODE_Set(LPM_DLPS_MODE);
}


/**
* @brief  Task_Init() contains the initialization of all the tasks.
*
* There are four tasks are initiated.
* Lowerstack task and upperstack task are used by bluetooth stack.
* Application task is task which user application code resides in.
* Emergency task is reserved.
*
* @param   No parameter.
* @return  void
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
*
*
* @param   No parameter.
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


void io_assert_failed(uint8_t *file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);

    for (;;);
}
