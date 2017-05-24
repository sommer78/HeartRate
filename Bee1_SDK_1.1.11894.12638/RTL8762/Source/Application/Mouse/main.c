/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This is the entry of user code which the main function resides in.
* @details
* @author   Chuanguo Xue
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "rtl876x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "profile_init.h"
#include "dlps_platform.h"
#include "keyscan.h"
#include "mousebutton.h"
#include "qdecoder.h"
#include "adns3000.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "mouse_application.h"
#include "simpleBLEPeripheral.h"
#include "hids_mouse.h"
#include "bas.h"
#include "dis.h"
#include "sps.h"
#include "profileApi.h"
#include "legacyperiphapi.h"
#include "rtl876x_io_dlps.h"
#include "os.h"
#include "rtl876x_spi.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_tim.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_qdec.h"
#include "rtl876x_nvic.h"

#if AUTOTEST
#include "mouse_autotest.h"
#endif

/* Reference varibales statement ----------------------------------------------*/
extern BOOL IsAbleToSendAction;
extern BOOL IsMouseEnabled;

/* Function declaration -------------------------------------------------------*/
void DLPS_TimerInit(void);

/* Global variables defines ---------------------------------------------------*/
/* control of allowing enter DLPS mode or not */
BOOL allowedLeftButton = true;
BOOL allowedRightButton = true;
BOOL allowedQDecoder = true;
BOOL allowedSensor = true;
BOOL allowedAutotest = true;

uint16_t current_interval = 0x10;
uint8_t gHIDServiceId;
uint8_t gBASServiceId;
uint8_t gDISServiceId;
uint8_t gSPSServiceId;

TimerHandle_t xTimersEnableSensor;
void vTimerEnableSensorCallback(xTimerHandle handle);
extern BOOL SensorState;

/* Defines---------------------------------------------------------------------*/
#define MOUSE_KEYPAD_ROW_SIZE             4
#define MOUSE_KEYPAD_COLUMN_SIZE          4
/* DEBOUNCE_TIME_8ms */
#define MOUSE_SWDEBOUNCE_TIMES            8*32
/* SCAN_INTERVAL: 0 - 12.5ms, 1 - 25ms, 2 - 50ms, 3 - 100ms */
#define MOUSE_KEYSCAN_INTERVAL            2

#define MOUSE_QDECODE_DEBOUNCE_INTERVAL 5500    //us
#define MOUSE_QDECODE_SAMPLE_RATE       10000   //Hz

/*
********************************************************
* parameter for btstack
*
*
*********************************************************
*/
// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL_MIN            0x83        /* 81.875ms */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            0x83        /* 81.875ms */
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
    0xc2, 0x03,     /* Mouse */
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
    0xc2, 0x03,     /* Mouse */
    0x0A,           /* length     */
    0x09,           /* type="Complete local name" */
    'M', 'o', 'u', 's', 'e', '_', 'X', 'X', 'X' /* Mouse_Name */
};



void BtStack_Init_Gap()
{
    //device name and device appearance
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "Mouse_XXX";
    uint16_t Appearance = GAP_GATT_APPEARANCE_MOUSE;

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
    uint8_t mitm = GAPBOND_AUTH_NO_MITM_YES_BOND;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oobEnable = FALSE;
    uint32_t passkey = 0; // passkey "000000"

    uint8_t secReqEnable = FALSE;
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
    //GAPBondMgr_SetParameter(GAPBOND_FIXED_PASSKEY_ENABLE, sizeof ( uint8_t ), &bUseFixedPasskey);

    GAPBondMgr_SetParameter(GAPBOND_SEC_REQ_ENABLE, sizeof ( uint8_t ), &secReqEnable);
    GAPBondMgr_SetParameter(GAPBOND_SEC_REQ_REQUIREMENT, sizeof ( uint8_t ), &secReqRequirement);

    // check bonded device exist or not
    remote_BD_struct BondedDevice;
    if (!GAPBondMgr_GetBondedDevice(&BondedDevice))
    {
        IsAbleToSendAction = false;
    }
}

void BtProfile_Init(void)
{
    gHIDServiceId = HIDS_AddService(AppProfileCallback);
    if (gHIDServiceId == 0xff)
    {
        //error
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
#if AUTOTEST

#else
    /* Setting all pins to the default state */
    All_Pad_Config_Default();

    /*--------------Mouse left button and right button setting-------------*/
    Pinmux_Config(MOUSE_LEFT_BUTTON_INDEX, GPIO_FUN);
    Pinmux_Config(MOUSE_RIGHT_BUTTON_INDEX, GPIO_FUN);
    Pad_Config(MOUSE_LEFT_BUTTON_INDEX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(MOUSE_RIGHT_BUTTON_INDEX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    /*--------------------------Mouse wheel setting-----------------------*/
    Pinmux_Config(QDEC_A_Y, qdec_phase_a_y);
    Pinmux_Config(QDEC_B_Y, qdec_phase_b_y);
    Pad_Config(QDEC_A_Y, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(QDEC_B_Y, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);


    /*--------------------------Mouse sensor setting--------------------- */
    Pinmux_Config(SENSOR_SPI_CS, GPIO_FUN);
    Pinmux_Config(SENSOR_SPI_CLK, SPI0_CLK_Master);
    Pinmux_Config(SENSOR_SPI_MOSI, SPI0_MO_Master);
    Pinmux_Config(SENSOR_SPI_MISO, SPI0_MI_Master);
    Pinmux_Config(SENSOR_MOTION_INT, GPIO_FUN);

    Pad_Config(SENSOR_SPI_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_MOTION_INT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

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
#if AUTOTEST
    NVIC_ClearPendingIRQ(PERIPHERAL_IRQ);
    NVIC_SetPriority(PERIPHERAL_IRQ, 0);
    NVIC_EnableIRQ(PERIPHERAL_IRQ);
#else
    /*-----------------Mouse button initialization-----------------------*/
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    /* Define Mouse Button parameter structure. Mouse button is configed as GPIO */
    GPIO_InitTypeDef MouseButton_Param;
    MouseButton_Param.GPIO_Pin  = GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX) | GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX);
    MouseButton_Param.GPIO_Mode = GPIO_Mode_IN;
    MouseButton_Param.GPIO_ITCmd = ENABLE;
    MouseButton_Param.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
    MouseButton_Param.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    MouseButton_Param.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
    GPIO_Init(&MouseButton_Param);
    MouseButton_Init();

    /*-----------------Mouse wheel initialization-----------------------*/
    RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);
    QDEC_InitTypeDef qdecInitStruct;
    QDEC_StructInit(&qdecInitStruct);
    qdecInitStruct.sampleRate = MOUSE_QDECODE_SAMPLE_RATE;
    QDEC_Init(QDEC, &qdecInitStruct);
    QDEC_INTConfig(QDEC, QDEC_INT_NEW_DATA, ENABLE);

    /*  Enable QDECODE IRQ  */
    NVIC_ClearPendingIRQ(QDECODE_IRQ);
    NVIC_SetPriority(QDECODE_IRQ, 0);
    NVIC_EnableIRQ(QDECODE_IRQ);

    /*-----------------Mouse sensor initialization-----------------------*/
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_DeInit(SPI0);
    /* turn on SPI clock */
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 500000;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_Init(SPI0, &SPI_InitStructure);
    SPI_Cmd(SPI0, ENABLE);

    /* CSN GPIO Init */
    GPIO_InitTypeDef Sensor_CS_Setting;

    Sensor_CS_Setting.GPIO_Pin  = GPIO_GetPin(SENSOR_SPI_CS);
    Sensor_CS_Setting.GPIO_Mode = GPIO_Mode_OUT;
    Sensor_CS_Setting.GPIO_ITCmd = DISABLE;
    Sensor_CS_Setting.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
    Sensor_CS_Setting.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    Sensor_CS_Setting.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
    GPIO_Init(&Sensor_CS_Setting);

    /* MOTION INT GPIO Init */
    GPIO_InitTypeDef Sensor_MOTION_Setting;
    Sensor_MOTION_Setting.GPIO_Pin = GPIO_GetPin(SENSOR_MOTION_INT);
    Sensor_MOTION_Setting.GPIO_Mode = GPIO_Mode_IN;
    Sensor_MOTION_Setting.GPIO_ITCmd = ENABLE;
    Sensor_MOTION_Setting.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    Sensor_MOTION_Setting.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    Sensor_MOTION_Setting.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
    GPIO_Init(&Sensor_MOTION_Setting);
    GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);//mask sensor motion interrupt

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = SENSOR_MOTION_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    Mouse_Sensor_Init();

    xTimersEnableSensor = xTimerCreate((const char *)"EnableSensor_Timer", \
                                       10000 / portTICK_RATE_MS, pdFALSE, NULL, vTimerEnableSensorCallback);
#endif

}

/**
* @brief  vTimerEnableSensorCallback(xTimerHandle handle) enables the sensor and button.
*
*
* @param   No parameter.
* @return  void
*/
void vTimerEnableSensorCallback(xTimerHandle handle)
{
    Mouse_Sensor_Init();

    GPIO_MaskINTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), DISABLE);

    GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);
    GPIO_INTConfig(GPIO_GetPin(MOUSE_LEFT_BUTTON_INDEX), ENABLE);
    GPIO_INTConfig(GPIO_GetPin(MOUSE_RIGHT_BUTTON_INDEX), ENABLE);

    System_WakeUp_Pin_Enable(SENSOR_MOTION_INT, 0);
    System_WakeUp_Pin_Enable(MOUSE_LEFT_BUTTON_INDEX, 0);
    System_WakeUp_Pin_Enable(MOUSE_RIGHT_BUTTON_INDEX, 0);
    SensorState = true;

    QDEC->REG_CR2 = 0xFFFFFFFF;
    QDEC->REG_CR1 |= (0x07 << 16);
    QDEC->REG_CR1 &= (~(0x07 << 12));
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "xxxxx", 0);

}

/**
  * @brief  MouseEnterDlpsSet() configs the pad before the system enter DLPS.
  *         Pad should be set PAD_SW_MODE and PAD_PULL_UP/PAD_PULL_DOWN.
  * @param  None
  * @retval None
  */
void MouseEnterDlpsSet(void)
{
#if AUTOTEST

#else
    /* set PAD of wake up pin to software mode */
    /* left and right button switch to software mode */
    Pad_Config(MOUSE_LEFT_BUTTON_INDEX, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(MOUSE_RIGHT_BUTTON_INDEX, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    /*sensor interrupt pin switch to software mode */
    Pad_Config(SENSOR_MOTION_INT, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_CS, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_CLK, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_MOSI, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_MISO, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    /* Qdeocer pin switch to software mode */
    Pad_Config(QDEC_A_Y, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(QDEC_B_Y, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /*-------Config wakeup pins which can wake up system from dlps mode-----*/
//    System_WakeUp_Pin_Enable(MOUSE_LEFT_BUTTON_INDEX, 0);
//    System_WakeUp_Pin_Enable(MOUSE_RIGHT_BUTTON_INDEX, 0);
//    System_WakeUp_Pin_Enable(QDEC_A_Y, 0);
//    System_WakeUp_Pin_Enable(QDEC_B_Y, 0);
//    System_WakeUp_Pin_Enable(SENSOR_MOTION_INT, 0);
#endif
}

/**
  * @brief  MouseExitDlpsInit() config the pad after the system exit DLPS.
  *         Pad should be set PAD_PINMUX_MODE.
  * @param  None
  * @retval None
  */
void MouseExitDlpsInit(void)
{

#if AUTOTEST

#else
    /* left and right button switch to pinmux mode */
    Pad_Config(MOUSE_LEFT_BUTTON_INDEX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(MOUSE_RIGHT_BUTTON_INDEX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);


    /* Qdeocer pin switch to pinmux mode */
    Pad_Config(QDEC_A_Y, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(QDEC_B_Y, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /*sensor interrupt pin switch to pinmux mode */
    Pad_Config(SENSOR_SPI_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SENSOR_SPI_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    Pad_Config(SENSOR_MOTION_INT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Mouse_Sensor_Init();

    qdec_ctx_clear();
    if (IsAbleToSendAction && IsMouseEnabled)
    {
        //unmask interrupt and clear interrupt
        QDEC->REG_CR2 = 0xFFFFFFFF;
        QDEC->REG_CR1 |= (0x07 << 16);
        QDEC->REG_CR1 &= (~(0x07 << 12));
    }
#endif

    ActiveTime_Restart(1);

}

BOOL DLPS_MouseCheck()
{
    if (allowedLeftButton && allowedRightButton && allowedQDecoder && allowedSensor && allowedAutotest)
        return true;
    else
        return false;
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
    if (FALSE == DLPS_ENTER_CHECK_CB_REG(DLPS_MouseCheck))
    {
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Error: DLPS_ENTER_CHECK_CB_REG(DLPS_MouseCheck) failed!\n", 0);
    }

    DLPS_IO_RegUserDlpsEnterCb(MouseEnterDlpsSet);
    DLPS_IO_RegUserDlpsExitCb(MouseExitDlpsInit);

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


void DLPS_TimerInit()
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_InitStruct.TIM_Period = 5000 * 10000;
    TIM_InitStruct.TIM_Mode = 1;

    /* DLPS_Timer init */
    TIM_TimeBaseInit(DLPS_Timer, &TIM_InitStruct);

    /* enable DLPS_Timer */
    TIM_Cmd(DLPS_Timer, ENABLE);
    TIM_INTConfig(DLPS_Timer, ENABLE);
}


void DLPS_TimerIntrHandler()
{
    BEE_IO_MSG dlps_msg;
    long xHigherPriorityTaskWoken = pdFALSE;

    TIM_Cmd(DLPS_Timer, DISABLE);
    TIM_ClearINT(DLPS_Timer);

    /* Change GPIO Interrupt Polarity, Enable Interrupt */
    dlps_msg.IoType = APP_TIMER_MSG;
    dlps_msg.subType = TIMER_DLPS;

    /* Send MSG to APP task */
    if (!SendMessageFromISR(&dlps_msg, &xHigherPriorityTaskWoken))
    {
        DBG_BUFFER(MODULE_KEYSCAN, LEVEL_INFO, " ***DLPS SendQueue fail.\n", 0);
    }
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void io_assert_failed(uint8_t *file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);

    for (;;);
}
