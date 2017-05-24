/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    AMIC demo(ADC/CODEC/GDMA)
* @details
* @author   tifnan
* @date     2015-05-27
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_codec.h"
#include "rtl876x_uart.h"
#include "rtl876x_rcc.h"
#include "rtl876x_adc.h"
#include "rtl876x_gdma.h"
#include "string.h"
#include "trace.h"
#include "board.h"

void PINMUX_Configuration(void);
void PAD_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);

/* uart command */
#define START_AMIC          0x01
#define STOP_AMIC           0x02

/* queue size & stak size & task priority */
#define IO_DEMO_EVENT_QUEUE_SIZE        0x10
#define IO_DEMO_MESSAGE_QUEUE_SIZE      0x10
#define IO_DEMO_TASK_STACK_SIZE         1024
#define IO_DEMO_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

/* task handle & queue handle */
xTaskHandle IODemoTaskHandle;
xQueueHandle DemoIOMessageQueue;
xQueueHandle DemoIOEventQueue;

SRAM_OFF_BD_DATA_SECTION
uint8_t TxBuf[100];
uint8_t VoiceRxEndFlag = 0;
uint8_t VoiceGdmaEndFlag = 0;
SRAM_OFF_BD_DATA_SECTION
uint8_t VoiceBuffer[200];

extern IRQ_FUN UserIrqFunTable[32+17];
void Gdma0Ch1IntrHandler(void);
void DataUartIntrHandler(void);
void IO_DemoTask(void* param);

int main(void) 
{
    /* create io test task */
    xTaskCreate(IO_DemoTask, "IO_Demo", IO_DEMO_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, IO_DEMO_TASK_PRIORITY, &IODemoTaskHandle);
    
    /* create event queue and message queue */
    DemoIOEventQueue = xQueueCreate(IO_DEMO_EVENT_QUEUE_SIZE, sizeof(char));
    DemoIOMessageQueue = xQueueCreate(IO_DEMO_MESSAGE_QUEUE_SIZE, sizeof(char));
    
    /* start task schedule */
    vTaskStartScheduler();
    
    return 0;
}

void PINMUX_Configuration(void)
{
    /* uart pinmux, Amic not needed */
    Pinmux_Config(P4_3, DATA_UART_TX);
    Pinmux_Config(P4_2, DATA_UART_RX);
    
    return;
}

void PAD_Configuration(void)
{
    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    //for amic input, must pull none
    Pad_Config(P2_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P2_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    
    return;
}

void RCC_Configuration(void)
{
    /* turn on UART/ADC/CODEC clock */
    RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_CODEC, APBPeriph_CODEC_CLOCK, ENABLE);
    
    return;
}

void NVIC_Configuration(void)
{

    /* GDMA IRQ */
    NVIC_InitTypeDef NVIC_InitStruct;
      
    NVIC_InitStruct.NVIC_IRQChannel = GDMA0_CHANNEL1_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);
    
    /* UART IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);
    
    return;
}

void Gdma0Ch1IntrHandler(void)
{
    uint8_t event = 0;
    portBASE_TYPE TaskWoken = pdFALSE;
    
    DBG_BUFFER(MODULE_LTP, LEVEL_ERROR, "Gdma0Ch1IntrHandler", 0);

    GDMA_ClearAllTypeINT(1);
    
    GDMA_Cmd(1, DISABLE);

    /* send voice data in task */
    event = IO_DEMO_EVENT_GDMA_RX;
    
    /* send event to DemoIO task */
    xQueueSendFromISR(DemoIOEventQueue, &event, &TaskWoken);
    
    portEND_SWITCHING_ISR(TaskWoken);
    
    return;
}

void IO_DemoTask(void* param)
{
    uint8_t blkCount = 0;
    uint8_t remainder = 0;
    uint8_t demoStrLen = 0;
    uint8_t i = 0;
    uint8_t event = 0;
    uint8_t rxByte = 0;
    uint8_t* pBuf = VoiceBuffer;
    char* demoStr = "## Please send audio command ##\r\n"
                    "## 0x01: start AMIC record ##\r\n"
                    "## 0x02: stop AMIC record ##\r\n";
    
    DBG_BUFFER(MODULE_LTP, LEVEL_ERROR, "main", 0);
    
    /* System clock configuration */
    RCC_Configuration();
    
    /* pinmux configuration */
    PINMUX_Configuration();
    
    /* pad configuration */
    PAD_Configuration();
    
    /* NVIC configuration */
    NVIC_Configuration();
    
    /******************* uart init *****************************/
    UserIrqFunTable[UART_IRQ]               = (IRQ_FUN)DataUartIntrHandler;
    
    UART_InitTypeDef uartInitStruct;
    
    UART_StructInit(&uartInitStruct);
    
    /* change to 3M baudrate */
    uartInitStruct.div = 1;
    uartInitStruct.ovsr = 8;
    uartInitStruct.ovsr_adj = 0x492;
    
    UART_Init(UART, &uartInitStruct);
    
    UART_INTConfig(UART, UART_INT_RD_AVA | UART_INT_LINE_STS, ENABLE);
 
    /******************** ADC init ******************************/
    ADC_InitTypeDef adcInitStruct;
    
    ADC_StructInit(&adcInitStruct);
    
    /* change to audio mode */
    adcInitStruct.adcMode   = ADC_Audio_Mode;
    adcInitStruct.amicEn    = ADC_AMIC_Enable;
    adcInitStruct.diffModeEn = ANA_DIFF_MODE_ENABLE;    //enable diff mode
    adcInitStruct.adcBurstSize        = 0x10;
    adcInitStruct.adcFifoThd          = 0x20;
    
    ADC_Init(ADC, &adcInitStruct);
    
    ADC_SetFifoDataEndian(ADC, ADC_FIFO_LITTLE_ENDIAN);

    /******************** gdma init *****************************/
    UserIrqFunTable[GDMA0_CHANNEL1_IRQ]     = (IRQ_FUN)Gdma0Ch1IntrHandler;
    
    GDMA_InitTypeDef GDMA_InitStruct;
    
    GDMA_StructInit(&GDMA_InitStruct);
        
    GDMA_InitStruct.GDMA_ChannelNum             = 1;
    GDMA_InitStruct.GDMA_DIR                    = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize             = MSBC_2_FRAME_SIZE / 4;          /* MSBC_2_FRAME_SIZE */
    GDMA_InitStruct.GDMA_SourceInc              = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc         = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize         = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize    = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize            = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize       = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr             = (uint32_t)((uint32_t*)&(ADC->FIFO));
    GDMA_InitStruct.GDMA_DestinationAddr        = (uint32_t)((uint32_t*)VoiceBuffer);
    GDMA_InitStruct.GDMA_TransferType           = GDMA_TransferType_ADC;
    GDMA_InitStruct.GDMA_ChannelPriority        = 6;                              /*channel prority between 0 to 7*/
    
    GDMA_Init(GDMA_Channel1, &GDMA_InitStruct);
    
    /* enable gdma interrup */
    GDMA_INTConfig(1, GDMA_INT_Transfer, ENABLE);

    /*********************** codec init(not enabled now) *********************************/
    CODEC_InitTypeDef codecInitStruct;

    CODEC_StructInit(&codecInitStruct);
    
    /* 12dB((0x4f - 0x2f)*0.375)*/
    //codecInitStruct.micVolume =0x4f; 
    
    //CODEC_Init(CODEC, &codecInitStruct);

#if 0
    /* send tips through uart */
    demoStrLen = strlen(demoStr);       
    memcpy(TxBuf, demoStr, demoStrLen);
    blkCount = demoStrLen / UART_TX_FIFO_SIZE;
    remainder = demoStrLen % UART_TX_FIFO_SIZE;
    
    for(i = 0; i < blkCount; i++)
    {
        /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
        UART_SendData(UART, &TxBuf[16 * i], 16);
        /* wait tx fifo empty */
        while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
    }
    
    /* send left bytes */
    UART_SendData(UART, &TxBuf[16 * i], remainder);
    /* wait tx fifo empty */
    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
#endif
    
    while(1)
    {
        if (xQueueReceive(DemoIOEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            /* uart rx event */
            if (event == IO_DEMO_EVENT_UART_RX) 
            {
                /* uart message */
                while (xQueueReceive(DemoIOMessageQueue, &rxByte, 0) == pdPASS)
                {
                    /* start amic */
                    if(rxByte == START_AMIC)
                    {
                        /* start amic */
                        CODEC_Init(CODEC, &codecInitStruct);
                        
#if 0       /* print all registers for debug */
                        uint8_t regCount = 0;
                        for(regCount = 0; regCount < 0x70; regCount+= 4)
                        {
                            DBG_BUFFER(MODULE_APP, LEVEL_INFO, "register[+0x%x]: 0x%x", 2,\
                                regCount, HAL_READ32(0x40010000, regCount));
                        }
#endif                        
                         /* enable gdma */
                        GDMA_Cmd(1, ENABLE);
 
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Amic started!", 0);
                    }
                    /* stop amic */
                    else if(rxByte == STOP_AMIC)
                    {
                        /* stop amic */
                        
                        CODEC_DeInit(CODEC);
                        /* disable gdma */
                        GDMA_Cmd(1, DISABLE); 
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Amic stoped!", 0);
                    }
                }
            }
            
            /* gdma event */
            if (event == IO_DEMO_EVENT_GDMA_RX) 
            {
                DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Voice gdma rx", 0);
            
                blkCount = (MSBC_2_FRAME_SIZE) / UART_TX_FIFO_SIZE;
                remainder = (MSBC_2_FRAME_SIZE) % UART_TX_FIFO_SIZE;
                
                /* send voice data through uart */
                for(i = 0; i < blkCount; i++)
                {
                    /* 1. max send 16 bytes(Uart tx and rx fifo size is 16) */
                    UART_SendData(UART, pBuf, 16);
                    /* wait tx fifo empty */
                    while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                    pBuf+= 16;
                }
                
                /* send left bytes */
                UART_SendData(UART, pBuf, remainder);
                /* wait tx fifo empty */
                while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
                //pBuf+= remainder;
                
                pBuf = VoiceBuffer;
                
                /* reset destination address */
                GDMA_SetDestinationAddress(GDMA_Channel1, (uint32_t)(VoiceBuffer));
                /* enable gdma gagin */
                GDMA_Cmd(1, ENABLE);
            }
        }
    }
}
