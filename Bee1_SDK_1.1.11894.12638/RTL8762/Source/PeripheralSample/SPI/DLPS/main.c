/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief	This file provides demo code of SPI comunication in DLPS situation. Please add file of board.h
* @details
* @author	elliot chen
* @date 	2015-06-04
* @version	v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "rtl876x_rcc.h"
#include "rtl876x_spi.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_io_dlps.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "dlps_platform.h"

/* Global variables------------------------------------------------------------*/
xQueueHandle DemoIOEventQueue;
uint8_t IO_DEMO_EVENT_SYSTEM_WAKE = 0x01;

/* Demo code-------------------------------------------------------------------*/
void RCC_Configuration(void)
{
	/* turn on SPI clock */
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
}

void SPI_DeInitPin(void)
{
#if 0
	/*add for Deinit all pin*/
	PeripherialType periType = PeripherialType_HCI_UART;
	
	/*add for Deinit all pin*/
	for (periType = PeripherialType_HCI_UART; periType < PeripherialType_MAX; periType++)
	{
		if ((periType != PeripherialType_DATA_UART)&&(periType!= PeripherialType_LOG_UART)&&(periType!= PeripherialType_SWD))
		
		{
			HalDumpOrDeinitPinByPeripheralType(periType, FALSE, TRUE);
		}
	}
#endif
}

void SPI_PINMUXConfiguration(void)
{
    Pinmux_Config(SPI0_SCK, SPI0_CLK_Master);
	Pinmux_Config(SPI0_MOSI, SPI0_MO_Master);
	Pinmux_Config(SPI0_MISO, SPI0_MI_Master);
	Pinmux_Config(SPI0_CS, SPI0_SS_N_0_Master);
}

void SPI_PADConfiguration(void)
{
    Pad_Config(SPI0_SCK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI0_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI0_MISO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(SPI0_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

void SPI_Configuration(void)    
{
    SPI_InitTypeDef  SPI_InitStructure;

	SPI_DeInit(SPI0);
    RCC_Configuration();
    SPI_DeInitPin();
    SPI_PINMUXConfiguration();
    SPI_PADConfiguration();
    
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_ClockSpeed  = 400000;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola; 
    
    SPI_Init(SPI0, &SPI_InitStructure);
 	SPI_Cmd(SPI0, ENABLE);
}

/*--------------------------SPI demo code----------------------------------*/
void SPI_DemoCode(void)
{
	uint8_t SPI_WriteBuf[16] = {0x01,0x02,0x00};
    uint8_t SPI_ReadBuf[16] = {0, 0 , 0, 0};
    uint8_t idx = 0;
    uint8_t len = 0;
    
    /*RCC_Configuration();
    SPI_DeInitPin();
    SPI_PINMUXConfiguration();
    SPI_PADConfiguration();
    SPI_Configuration();*/
	
    /*---------------read flash ID--------------*/
    SPI_WriteBuf[0] = 0x9f;
    SPI_SendBuffer(SPI0, SPI_WriteBuf, 4);
    
    /*Waiting for SPI data transfer to end*/
    while(SPI_GetFlagState(SPI0, SPI_FLAG_BUSY));
    
    /*read ID number of flash GD25Q20*/
    len = SPI_GetRxFIFOLen(SPI0);
    for (idx=0; idx<len; idx++)
	{
        SPI_ReadBuf[idx] = SPI_ReceiveData(SPI0);
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SPI_ReadBuf[%d] = 0x%x", 2, idx, SPI_ReadBuf[idx]);
    }
}
/*--------------------- Io test task-------------------------*/
void IOTestTask(void *pParameters)
{
	uint8_t event;
	uint8_t idx = 0;
    
	while(true)
	{
        if(xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {   
            /* system wakeup event */
            if (event == IO_DEMO_EVENT_SYSTEM_WAKE)
            {
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "IO test-->Task is running!", 0);
                /*-------print SPI register-----------------*/
                for(idx = 0; idx<13;idx++)
                {
                    DBG_BUFFER(MODULE_APP, LEVEL_INFO,"SPI0_StoreReg[%d] = 0x%2x!",2, idx, *((uint32_t *)(0x40042000UL + idx*4)) );
                }
                SPI_DemoCode();
            }
            
            //vTaskDelay(200 / portTICK_RATE_MS);
        }
	}
}

void DlpsExitCallback(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DlpsExitCallback", 0);
    
    uint8_t event = IO_DEMO_EVENT_SYSTEM_WAKE;
    
    xQueueSend(DemoIOEventQueue, &event, 0);
}

/**
* @brief  PwrMgr_Init() contains the setting about power mode.
*
* @param   No parameter.
* @return  void
*/
void PwrMgr_Init(void)
{
    DLPS_IO_Register();
    DLPS_IO_RegUserDlpsExitCb(DlpsExitCallback);
	LPS_MODE_Set(LPM_DLPS_MODE);
    ActiveTime_Restart(10);
}

int main(void)
{
    BaseType_t xReturn = pdFAIL;

    /*enable DLPS*/
    PwrMgr_Init();
    /*SPI init and read GD25Q20 ID*/
    SPI_Configuration();
    SPI_DemoCode();
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(uint8_t));
    xReturn = xTaskCreate( IOTestTask, "IO driver test", IOTest_STACK_SIZE/sizeof(portSTACK_TYPE), NULL, IOTest_PRIORITY, NULL );
    
    vTaskStartScheduler();
    return xReturn;
}
