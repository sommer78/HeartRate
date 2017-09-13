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

//#include "rtl876x.h"
#include "nrf.h"



#define  WaveSlopeRange 30		  /**< 锯齿波判断标准初始为25 > */
#define  HeartRateMAX	160			 /**<   心率最大值 初始为160    > */
#define  HeartRateMIN   40			 /**<   心率最小值 初始为40    > */
#define  SampleRate     2				 /**<   采样率 默认为2ms    > */	
#define  SamplePointTotal 30000		 /**<   所有的采样点 60*1000/SampleRate > */	
#define  TriggerPeriodPoint 10				 /**<  波形窗口采样 10> */	
#define  SamplePointMax     8000				 /**<  心率采样最大值默认4000点> */	
#define  SmoothMax 		800		 /**<  平整度最大值 默认为600> */	
#define  WaveSampleMax 		4000			 /**<  取多少点进行算法> */	
#define  WaveArrayMax		3		 /**<  取多少个波形进行计算> */	
#define  RecDetectData		10		 /**<  平整度超过多少判断为方波> */	
#define  PeriodMax  		750			/**< 最大周期数> */	
#define  PeriodMin			188			/**< 最小周期数> */	
#define  PeakBottomStand	100		/**<   峰值间值          > */	




/*
 * Defines the prototype to which the application bpm  function must conform.
 *  
 */
 /**<      > */
typedef enum
{
    HRInit = 0,   /*BPM init */
    HRFinish = 1,	 /*BPM finish */		
    HRPointMax = 2,	 /**<   心率采样最大值出错   > */
    HRLineOut = 3    /**<  采样数据太平整退出  > */
} HRState;


typedef enum
{
    SlopeUp= 0,   
    SlopeDown = 1,	
    SlopeSmooth = 2,	 
} SlopeDirection;


/**
 * @brief BPM wave define key definition.
 *
 * Each key definition will contains BPM wave type/ period / rising time / falling time 
 * key address/ key value.
 */
typedef struct HEART_RATE_PULSE
{
	uint8_t		index;			 /**<  第几个波形  > */
    uint8_t     type;        /**< wave type . Like sine, squre sawtooth... */
    uint16_t  	topIndex;    /**< top index */
	uint16_t  	topValue;    /**< top value */
	uint16_t  	topLength;    /**< top length */
	uint16_t  	bottomIndex;    /**< bottom index */
	uint16_t  	bottomValue;    /**< bottom value */
	uint16_t  	bottomLength;    /**< bottom length */
    uint16_t    upTime; 		/*   rising edge time   */
    uint16_t    downTime;      /*   falling edge time   */
	uint16_t	startPointIndex;	 /**<    起始点  > */
	uint16_t	endPointIndex;		 /**<   结束点   > */		
	uint8_t		peakBottomValue;			 /**<  峰值间隔   > */
	uint16_t	period;
} HEART_RATE_PULSE_T, *HEART_RATE_PULSE_INDEX;


typedef struct HEART_RATE_PARAM
{
	uint16_t waveSlopeRange;		  /**< 锯齿波判断标准初始为200 > */
	uint8_t  heartRateMAX;				 /**<   心率最大值 初始为160    > */
	uint8_t	heartRateMIN;				 /**<   心率最小值 初始为40    > */
	uint16_t sampleRate;				 /**<   采样率 默认为2ms    > */	
	uint16_t oneMinutePoint;			 /**<   所有的采样点 60*1000/SampleRate > */	
	uint8_t triggerPeriodPoint;				 /**<  波形窗口采样 10> */	
 	uint16_t samplePointMax;				 /**<  心率采样最大值默认4000点> */	
	uint16_t evennessMax;				 /**<  平整度最大值 默认为200> */	
	uint16_t waveArrayMax;				 /**<  取多少个波形进行计算> */	
	uint16_t recDetectData;				 /**<  平整度超过多少判断为方波> */	
	uint16_t periodMax;					/**< 最大周期数> */	
	uint16_t periodMin;					/**< 最小周期数> */	
	uint16_t peakBottomStand;			/**<   峰值间值          > */	
} HEART_RATE_PARAM_T, *HEART_RATE_PARAM_INDEX;


typedef struct SLOPE
{
	uint16_t value;		
	SlopeDirection  direction;			
	uint8_t smooth;
} SLOPE_T, *SLOPE_INDEX;






/* ram define ------------------------------------------------------------------*/

extern uint16_t smoothValue;
extern uint16_t heartRate;   
extern uint16_t pointCount;
/*
 * common funcation subroutine
 * 
 */
void pushArrayData(uint16_t *array,int length,uint16_t data);




void clrHeartRateStack(void);

void heartRateClrRam(void);


uint16_t getArrayAverageWithoutPeak(uint16_t * array,int length);


/*
 * heart rate calculation main subroutine
 * 
 */


HRState getHeartRateWaves(uint16_t adData) ;







/*
 * heartRateParamSetup
 * param: _waveSlopeStandard  
 * _sampleRate 1 = 1ms 30 = 30ms
 */
void heartRateParamSetup(HEART_RATE_PARAM_T heartRateParam);


/*
 * 得到滤波后的心率值
 * param:
 * return: heartRate
 */
uint16_t getHeartRateFilter(void);

/*
 * 每次开始测试必须要调用此初始化
 * param:
 * return: 
 */
void heartRateInit(void);



#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_ADC_H_ */

