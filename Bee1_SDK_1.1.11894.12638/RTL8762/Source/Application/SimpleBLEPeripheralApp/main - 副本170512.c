/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This is the entry of user code which the main function resides in.
* @details
* @author   ranhui
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
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
#include "SimpleBLEPeripheral_application.h"
#include "simpleBLEPeripheral.h"
#include "simple_ble_service.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_adc.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include <string.h>
#include  "heart_rate_sample.h"

#include "Lts1303m.h"



/*
********************************************************
* 
*
*
*********************************************************
*/
void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);


#define TEST_Pin            P2_4
#define GPIO_TEST_Pin       GPIO_GetPin(TEST_Pin)


/* queue size & stak size & task priority */
#define IO_DEMO_EVENT_QUEUE_SIZE        0x10
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoIOEventQueue;

/* ram define ------------------------------------------------------------------*/
uint8_t DemoStrBuffer[20];

const HEART_RATE_PARAM_T HeartRateParam = 
{
		
	200,		  /**< 锯齿波判断标准初始为200 > */
	160,			 /**<   心率最大值 初始为160    > */
	40,				 /**<   心率最小值 初始为40    > */
	20,				 /**<   采样率 默认为20ms    > */	
	24000,			 /**<   所有的采样点 60*1000/30 > */	
	4,			 /**<  波形窗口采样 4> */	
	4000,		 /**<  心率采样最大值默认4000点> */	
	200,			 /**<  平整度最大值 默认为200> */	
	4,			 /**<  取多少点进行算法> */	
	2,		 /**<  取多少个波形进行计算    默认2个> */	
};


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


// service id for simple profile service
uint8_t gSimpleProfileServiceId;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
#ifdef SIMP_USE_UUID128
#else
    0x03,
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(GATT_UUID_SIMPLE_PROFILE),
    HI_WORD(GATT_UUID_SIMPLE_PROFILE),
#endif

    0x03,
    GAP_ADTYPE_APPEARANCE,
    LO_WORD(GATT_APPEARANCE_UNKNOWN),
    HI_WORD(GATT_APPEARANCE_UNKNOWN),
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
//#define ADV_WITH_MANUFACTURE_DATA

#ifdef ADV_WITH_MANUFACTURE_DATA
static uint8_t advertData[] =
{
    0x02,
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    0x10,
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
    0x5d, 0x00,//Company Identifier for Realtek
    'i', 't', ' ', 'i', 's', 'm', 'a', 'n', 'u', 'd', 'a', 't', 'a'
};
#else
static uint8_t advertData[] =
{
    0x02,
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

#ifdef SIMP_USE_UUID128
#else
    0x03,
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_WORD(GATT_UUID_SIMPLE_PROFILE),
    HI_WORD(GATT_UUID_SIMPLE_PROFILE),
#endif

    0x03,
    GAP_ADTYPE_APPEARANCE,
    LO_WORD(GATT_APPEARANCE_UNKNOWN),
    HI_WORD(GATT_APPEARANCE_UNKNOWN),

    0x0A,
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'B', 'e', 'e', '_', 'p', 'e', 'r', 'i', 'p'
};
#endif


/******************************************************************
 * @fn          Initial gap parameters
 * @brief      Initialize peripheral and gap bond manager related parameters
 *
 * @return     void
 */
void BtStack_Init_Gap(void)
{
    //device name and device appearance
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "Bee_perip";
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

    uint8_t secReqEnable = FALSE;
    uint8_t secReqRequirement = GAPBOND_SEC_REQ_NO_MITM;

    uint8_t bUseFixedPasskey = TRUE;

#ifdef ANCS_SUPPORT
    secReqEnable = TRUE;
#endif

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

    GAPBondMgr_SetParameter(GAPBOND_SEC_REQ_ENABLE, sizeof ( uint8_t ), &secReqEnable);
    GAPBondMgr_SetParameter(GAPBOND_SEC_REQ_REQUIREMENT, sizeof ( uint8_t ), &secReqRequirement);

}

/******************************************************************
 * @fn          Initial profile
 * @brief      Add simple profile service and register callbacks
 *
 * @return     void
 */
void BtProfile_Init(void)
{
    gSimpleProfileServiceId = SimpBleService_AddService(AppProfileCallback);

    ProfileAPI_RegisterCB(AppProfileCallback);
#ifdef ANCS_SUPPORT
    ProfileAPI_RegisterAncsNotification(AppAncsNotificationCallback);
#endif
}

void PINMUX_Configuration(void)
{
    /* uart pinmux */
    Pinmux_Config(P3_0, DATA_UART_TX);
    Pinmux_Config(P3_1, DATA_UART_RX);
    Pinmux_Config(TEST_Pin, GPIO_FUN);
    return;
}

void PAD_Configuration(void)
{
    /* uart pad config */
    Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(TEST_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	
    /* ADC pad config */
   // Pad_Config(P2_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

    return;
}

void RCC_Configuration(void)
{
    /* turn on UART clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    /* turn on ADC clock */
  //  RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
    /* Enable GPIO clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	/* Initialize GPIO */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin  = GPIO_TEST_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
    return; 
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* ADC IRQ */  
  //  NVIC_InitStruct.NVIC_IRQChannel = ADC_IRQ;
//    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
//    NVIC_Init(&NVIC_InitStruct);
    
    /* UART IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);
    
    return;
}



/**
* @brief    Board_Init() contains the initialization of pinmux settings and pad settings.
*
*               All the pinmux settings and pad settings shall be initiated in this function.
*               But if legacy driver is used, the initialization of pinmux setting and pad setting
*               should be peformed with the IO initializing.
*
* @return  void
*/
void Board_Init(void)
{
  /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /******************* uart init *****************************/
    UART_InitTypeDef uartInitStruct;
    
    UART_StructInit(&uartInitStruct);
    
    /* default 115200 baudrate */
    UART_Init(UART, &uartInitStruct);
    
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
 
    /******************** ADC init ******************************/
  //  ADC_InitTypeDef adcInitStruct;
    
  //  ADC_StructInit(&adcInitStruct);
    
    /* use channel 4 */
 //   adcInitStruct.channelMap = ADC_CH4;
 //   ADC_Init(ADC, &adcInitStruct);
 //   ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
 //   ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);


}

/**
* @brief    Driver_Init() contains the initialization of peripherals.
*
*               Both new architecture driver and legacy driver initialization method can be used.
*
* @return  void
*/
void Driver_Init(void)
{

}


/**
* @brief    PwrMgr_Init() contains the setting about power mode.
*
* @return  void
*/
void PwrMgr_Init(void)
{
    LPS_MODE_Set(LPM_DLPS_MODE);
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
#if 0
void printfShow(){

	 char* demoStr = "\r\n";
   strLen = strlen(demoStr);
   memcpy(DemoStrBuffer, demoStr, strLen);
    
    /* send demo tips */
    blkcount = strLen / UART_TX_FIFO_SIZE;
  
                     
    /* send block bytes(16 bytes) */
    for(i = 0; i < blkcount; i++)
    {
        UART_SendData(UART, &DemoStrBuffer[16 * i], 16);
        /* wait tx fifo empty */
        while(UART_GetFlagState(UART, UART_FLAG_THR_TSR_EMPTY) != SET);
    }
    
}
#endif

void heartRateSubroutine(){
		int i = 0;	
	    HRState bpmState;
		uint16_t heartRate = 40;

		heartRateInit();
	
		
		for(i=1000;i<4000;i++){
		bpmState = 	 heartRateWaveDetect(testWave_30MS[i]);  
		if(bpmState==HRFinish){
		heartRate =	getHeartRateFilter();
			 sprintf( DemoStrBuffer, "HeartRate=%d\r\n",heartRate);
				UART_SendData(UART, DemoStrBuffer, 16);
		heartRateInit();
		
		//
		}
		
	}

}

/**
* @brief  main() is the entry of user code.
*
*
* @return  void
*/
uint8_t s;
int main(void)
{
//uint16_t value[16];
//int i;
	
	Board_Init();
#if 0
    Board_Init();
    Driver_Init();
    BtStack_Init_Peripheral();
    BtStack_Init_Gap();
    BtProfile_Init();
    PwrMgr_Init();
    Task_Init();
    vTaskStartScheduler();
#endif
		heartRateParamSetup(HeartRateParam);
		clrHeartRateStack();
		heartRateInit();
		
	//	GpioTest();
while(1){
	if(s%2==0){
		GPIO_SetBits(GPIO_TEST_Pin);
		}else {
		GPIO_ResetBits(GPIO_TEST_Pin);
		}
		s++;
//	heartRateSubroutine();
}


   // return 0;
}

