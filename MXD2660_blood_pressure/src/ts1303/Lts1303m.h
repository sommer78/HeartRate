/**
*********************************************************************************************************
*               Copyright(c) 2017, ETEK Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      lts1303m.h
* @brief     header file of lts1303m
* @details   
* @author    jiangs
* @date      2017-05-04
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _LTS1303M_H_
#define _LTS1303M_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "system_config.h"


#define  WaveSlopeRange 100		 
#define  HeartRateMAX	160			
#define  HeartRateMIN   40			
#define  SampleRate     10				

#define  TriggerPeriodPoint 10				
#define  SamplePointMax     10000			
#define  SmoothMax 		800		
#define  WaveSampleMax 		4000		
#define  WaveArrayMax		3		
#define  RecDetectData		10		
#define  PeriodMax  		750			
#define  PeriodMin			188		
#define  PeakBottomStand	100		




/*
 * Defines the prototype to which the application bpm  function must conform.
 *  
 */
 /**<      > */
typedef enum
{
    HRInit = 0,   /*BPM init */
    HRFinish = 1,	 /*BPM finish */		
    HRError = 2,/*BPM error */	
    HRErrShortWave = 3,/*BPM error */	
    HRErrLongWave = 4,/*BPM error */	
    HRErrPointOut = 5,/*BPM error */	
    HRErrTopLong = 6,/*BPM error */	
	HRErrBottomLong = 7,/*BPM error */ 
	HRErrPulseLength = 8,/*BPM error */ 

	
} HRState;

typedef enum
{
    SlopeUP = 0,  
    SlopeDown = 1,	 	
    SlopeTop = 2,
    SlopeBottom = 3,

} WaveSLope;




/* ram define ------------------------------------------------------------------*/


extern uint16_t heartRate;   

/*
 * common funcation subroutine
 * 
 */




/*
 *
 * param:
 * return: heartRate
 */
uint16_t getHeartRateDetection(void);

/*
 * 
 * param:
 * return: 
 */
void heartRateInit(void);


uint16_t getHeartRateSmooth(uint16_t tmpHeartRate);

void hw_timer_config(void);
void bsp_ts1303_init(void);

uint16_t adValueFilter(uint16_t adValue);




#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_ADC_H_ */

