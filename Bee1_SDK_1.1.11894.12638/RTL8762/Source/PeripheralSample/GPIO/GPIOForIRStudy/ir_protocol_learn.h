/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ir_protocol_learn.h
* @brief     
* @details   
* @author    elliot chen
* @date      2016-3-18
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __IR_PROTOCOL_LEARN_H
#define __IR_PROTOCOL_LEARN_H

#ifdef __cpluspuls
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "gap.h"
#include "blueapi.h"
#include "trace.h"

/* Defines ------------------------------------------------------------------*/

/*---------------Board configuration by user------------------------*/
#define IR_LEARN_PIN				P0_0
#define GPIO_IR_LEARN				GPIO_GetPin(IR_LEARN_PIN)

/*! 
* @brief  Configure GPIO interrupt polarity.
* @ note This parameter configuration into rising edge trigger when first GPIO interrupt occur. 
*/
#define GPIO_IR_INT_POLARITY		GPIO_INT_POLARITY_ACTIVE_HIGH
#define GPIO_IR_IRQ					GPIO0_IRQ
#define GPIO_IR_IntrHandler			Gpio0IntrHandler

/*! 
* @brief Enable dynamic switching GPIO edge trigger parameters or not. 
* @note If TRUE, both rising edge and falling edge will cause GPIO interrupt. 
* If FALSE, GPIO interrupt occur depending on GPIO_IR_INT_POLARITY parameter. 
*/
#define SWITCH_EDGE_POLARITY		TRUE

/* Enable print log or not */
#define PRINT_LOG					TRUE

/* Timer for learning IR wave which record every time of GPIO interrupt */
#define TIM_LEARN_HIGH  			TIM4
/* Timer for time out for quit IR learn mode */
#define TIM_TIME_OUT				TIM5
/* IR learn time out is 1s */
#define IR_LEARN_TIME_OUT			(10000*1000UL)
#define IR_TIME_OUT_IRQ				TIMER5_IRQ
#define TIME_OUT_IntrHandler		Timer5IntrHandler

/* Configure IR loop queue parameters */
#define LOOP_QUEUE_MAX_SIZE			(1024)
#define QUEUE_CAPABILITY			(LOOP_QUEUE_MAX_SIZE-1)
#define DETECT_WAVE_MAX_SIZE		(200)
#define ABS_FREQUENCY(a,b) 			((a>b) ? (a-b):(b-a))

/*--------------Configure by user to confirm IR learn parameter --------*/
/* Filter threshold value. If 100< time interval< 2000, treat it as a part of a carrier time */
#define TIME_LOWEST_VALUE			100
#define TIME_HIGHEST_VALUE 			2000
/* If low voltage time>0.25ms,we treat it as a time which have no carrier */
#define TIME_LOW_VOLTAGE_VALUE 		2500	

/* Enable IR send IR learn data */
#define ENABLE_TX_TEST				FALSE
#if ENABLE_TX_TEST
#define IR_TX_PIN					P2_0
#include "rtl876x_ir.h"
#endif

/* Enable calculate frequency when decode packet or not */
#define CALCULATE_FREQUENCY			FALSE
/* Enable correcting frequency by software */
#define CORRECT_FREQ_BY_SOFTWARE	TRUE

/* Data struct for IR study */
typedef struct _IR_PacketTypeDef
{
	/* Manage loop queue */
	volatile uint16_t 	ReadIndex;			/* index of read queue */
	volatile uint16_t 	WriteIndex; 		/* index of write queue */

	/* Record IR learned data */
	uint32_t			DetectTime[DETECT_WAVE_MAX_SIZE];
	uint32_t 			SquareWaveNum;
	uint16_t			DetectTimeNum;
	bool				IsHighLevel;
	volatile bool 		OverFLow;
	volatile uint8_t 	rxEndFlag;			/* Flag for detecting IR end wave */

#if CALCULATE_FREQUENCY
	/* Record learned IR frequency */
	float				wave_frequency[DETECT_WAVE_MAX_SIZE/2 +1];
#else
	/* Record IR high waveform time parameters */
	uint32_t 			high_wave_time[DETECT_WAVE_MAX_SIZE/2 +1];
	uint16_t 			high_wave_count[DETECT_WAVE_MAX_SIZE/2 +1];
#endif	
	uint16_t 			frequencyNum;		/* Record number of calculated frequency */
	volatile BOOL 		rising_status;		/* Record waveform edge state */
	volatile bool 		learn_time_out;		/* Record IR time out flag */
}IR_PacketTypeDef;

void LoopQueueInit(IR_PacketTypeDef *pIR_Packet);
__STATIC_INLINE BOOL LoopQueueIsFull(IR_PacketTypeDef *pIR_Packet);
BOOL LoopQueueIsEmpty(IR_PacketTypeDef *pIR_Packet);
uint8_t IR_DecodePacket(IR_PacketTypeDef *pIR_Packet);
float IR_DecodeFrequency(IR_PacketTypeDef *pIR_Packet);
void IR_Protocol_Learn_DeInit(void);
void IR_Protocol_Learn_Init(void);
BOOL IR_ReturnLearnWaveForm(uint16_t *pstore_address, uint8_t len);
BOOL IR_SendLearnWaveForm(IR_PacketTypeDef *pIR_Packet);
void IR_SendInit(uint16_t frequency);
void IR_User_Sample_Code(void);
void IR_Waveform_Learn_Test_Code(void);

#ifdef __cplusplus
}
#endif

#endif /*__IR_PROTOCOL_LEARN_H*/

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

