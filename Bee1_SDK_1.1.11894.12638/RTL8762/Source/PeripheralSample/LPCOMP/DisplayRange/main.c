/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    Adc demo(one shot mode)
* @details
* @author   howie
* @date     2015-10-28
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_adc.h"
#include "rtl876x_rcc.h"
#include "string.h"
#include "trace.h"
#include "board.h"
#include "rtl876x_flash_storage.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bee_message.h"
#include "semphr.h"
#include "blueapi_types.h"
#include "rtl_types.h"
#include "legacyperiphapi.h"
#include "rtl876x_rtc.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void DataUartIntrHandler(void);
void LPCOMP_BatteryDetection_Enable(void);
float cal_refv(uint8_t value);


/* queue size & stak size & task priority */
#define IO_DEMO_EVENT_QUEUE_SIZE        0x10
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoADCMessageQueue;
xQueueHandle DemoIOEventQueue;

xQueueHandle DemoLPCMessageQueue;

/*Global*/
uint16_t global_count_for_lpc =0;





extern IRQ_FUN UserIrqFunTable[32+17];

/* functions declaration */
void IO_DemoTask(void* param);



int main(void) 
{

    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(char));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(uint16_t));
		DemoADCMessageQueue = xQueueCreate(0x10, sizeof(uint16_t));
		DemoLPCMessageQueue = xQueueCreate(0x01, sizeof(uint16_t));//only one message
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
	
	/* uart pinmux */
  Pinmux_Config(P3_0, DATA_UART_TX);
  Pinmux_Config(P3_1, DATA_UART_RX);
	/* lcp pin */
	Pinmux_Config(Monitoring_voltage_Pin, IDLE_MODE);
}

void PAD_Configuration(void)
{
	
	/* uart pad config */
  Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
  Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
	/* lpc pad config */
	Pad_Config(Monitoring_voltage_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
}

void RCC_Configuration(void)
{
	/* turn on UART clock */
  RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
}

void NVIC_Configuration(void)
{
	  /* Config LPCOMP interrupt */
  NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = ANALOG_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
		/* UART IRQ */
	NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
}

void LPCOMP_BatteryDetection_Enable(void)
{
    UINT8 aon_reg_value = 0x0;
    
    /* Set RTC_BTAON_POWERCUT_BIT */
    *((uint32_t *)0x40000014) |= BIT(9);
    
    /* Release ISO_BT_CORE_RTC */
    bee_aon_gp_write(0x1D, bee_aon_gp_read(0x1D)&(~BIT(4)));

    /* Enable lpcomp out sync signal to CPU interrupt */
    //RTC->CR1 |= BIT(21);//disable lpc interrpt
    
    /*---------------set threshold voltage of low power comparator(user can set threshold voltage here)-----------*/
    aon_reg_value = bee_aon_gp_read(0x8E);
    /* threshold voltage = (0b10*0.05 + 0.05)*4V = 0.6V */
    aon_reg_value |= 0x2;
		bee_aon_gp_write(0x8E, aon_reg_value);
    
    /* Enable signal detector from BTAON */
    aon_reg_value = bee_aon_gp_read(0x9E);
    aon_reg_value |= BIT(0); 
    bee_aon_gp_write(0x9E, aon_reg_value);
}

void IO_DemoTask(void* param)
{
		uint8_t event = 0;
		uint8_t rxByte = 0;
		 /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
	    /* NVIC configuration */
    NVIC_Configuration();
		UserIrqFunTable[UART_IRQ]               = (IRQ_FUN)DataUartIntrHandler;
    
    /******************* uart init *****************************/
    UART_InitTypeDef uartInitStruct;
    
    UART_StructInit(&uartInitStruct);
    
    /* default 115200 baudrate */
    UART_Init(UART, &uartInitStruct);
    
		UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
		
		LPCOMP_BatteryDetection_Enable();
		
		serial_puts("Begin Now!\n");
    
    /************************** start task while loop **********************/
    while(1)
    {		
        if (xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)//portMAX_DELAY
        {
						if(event == IO_DEMO_EVENT_BATTERY_LEVEL_LOW)
            {
							serial_puts("IO_DEMO_EVENT_BATTERY_LEVEL_LOW\n");
            }
						if (event == IO_DEMO_EVENT_UART_RX) 
            {
                /* uart message */
                while (xQueueReceive(DemoIOMessageQueue, &rxByte, 0) == pdPASS)
                {
                    /* start */
                    if(rxByte == 0x11)//enable or start
                    {
												loop_threshold();
												serial_puts("Power no lower than ");
												unsigned char arr_lpc[20];
												Float2Char((float)global_count_for_lpc*(float)0.2,arr_lpc);
												serial_puts((const void *)arr_lpc);
												serial_puts("V\n");
                    }
                    /* stop */
                    else if(rxByte == 0x22)
                    {		
												loop_threshold();
												serial_puts("Power no lower than ");
												unsigned char arr_lpc[20];
												Float2Char((float)global_count_for_lpc*(float)0.2,arr_lpc);
												serial_puts((const void *)arr_lpc);
												serial_puts("V\n");
                    }
						
                }
            }
        }
				
		}
}

void io_assert_failed(uint8_t* file, uint32_t line)
{
    DBG_DIRECT("io driver parameters error! file_name: %s, line: %d", file, line);
    
    for(;;);
}
