/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file		main.c
* @brief		This file provides demo code of meomory to memory transfer by GDMA.
* @details
* @author	elliot chen
* @date 		2015-06-04
* @version	v1.0
*********************************************************************************************************
*/

/*! 
* @brief This file provides demo code of meomory to memory transfer by GDMA. 
* @note Transfer 100 bytes and is divided into 10 tramsmission.
* @ note This just provide a demo for how to use GDMA because the number each GDMA can tranfer is 4095 which unit is GDMA_SourceDataSize. 
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_gdma.h"
#include "rtl876x_rcc.h"

/* Globals -------------------------------------------------------------------*/

/* Record total number of GDMA interrupt */
uint8_t GDMA_intr_num = 0;
uint8_t GDMA_TestBuffer[100];
uint8_t GDMA_TestReceiveBuffer[100];

/* GDMA memory to memory transfer demo code */
void RCC_Configuration(void)
{
	/* Turn on GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
}
void GDMA_MemoryToMemory(void)    
{
	GDMA_InitTypeDef GDMA_InitStruct;
    uint8_t i = 0;
        
     /* Initialize send and receive buffer */
    for(i=0; i<100; i++)
    {   
        GDMA_TestBuffer[i] = i + 1;
    }
    for(i=0; i<100; i++)
    {   
        GDMA_TestReceiveBuffer[i] = 0;
    }

	/* Initialize variable for number of data to transmission */
	GDMA_intr_num = 0;

	/* Initialize GDMA peripheral */
    GDMA_InitStruct.GDMA_ChannelNum          = 0;
  	GDMA_InitStruct.GDMA_DIR 			     = GDMA_DIR_MemoryToMemory;
  	GDMA_InitStruct.GDMA_BufferSize 	     = 10;
  	GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
	GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;

	GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
	GDMA_InitStruct.GDMA_SourceMsize	     = GDMA_Msize_8;
	GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
	
	GDMA_InitStruct.GDMA_SourceAddr 	     = (uint32_t)GDMA_TestBuffer;
	GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)GDMA_TestReceiveBuffer;
	GDMA_InitStruct.GDMA_TransferType        = GDMA_TransferType_Other;
	/* This parameter can be Configure from 0 to 5 which 5 is highest priority */
	GDMA_InitStruct.GDMA_ChannelPriority     = 0;  
     
    GDMA_Init(GDMA_Channel0, &GDMA_InitStruct);
    
    GDMA_INTConfig(0, GDMA_INT_Transfer, ENABLE);
   
    /* GDMA IRQ */
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL0_IRQ);
    NVIC_SetPriority(GDMA0_CHANNEL0_IRQ, 1);
    NVIC_EnableIRQ(GDMA0_CHANNEL0_IRQ);

   	/* Enable GDMA to transmit data */
    GDMA_Cmd(0, ENABLE);
}

int main(void)
{
    RCC_Configuration();
    GDMA_MemoryToMemory();
    while(1);

}

void Gdma0Ch0IntrHandler(void)
{
	GDMA_intr_num++;
	
    GDMA_ClearAllTypeINT(0);

	/* Transfer control */
	if(GDMA_intr_num < 10)
	{
		/* Enable GDMA to transmit data */
    	GDMA_Cmd(0, ENABLE);
	}
}
