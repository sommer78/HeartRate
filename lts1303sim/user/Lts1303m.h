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
#include "stm32f10x.h"
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
typedef struct HEART_RATE_WAVE
{
	uint8_t		waveIndex;			 /**<  第几个波形  > */
  uint8_t    waveType;        /**< wave type . Like sine, squre sawtooth... */
  uint16_t  	topIndex;    /**< top index */
	uint16_t  	topValue;    /**< top value */
	uint16_t  	topLength;    /**< top length */
	uint16_t  	bottomIndex;    /**< bottom index */
	uint16_t  	bottomValue;    /**< bottom value */
	uint16_t  	bottomLength;    /**< bottom length */
  uint16_t    risingEdge; 		/*   rising edge time   */
  uint16_t    fallingEdge;      /*   falling edge time   */
	uint16_t	firstPointIndex;	 /**<    起始点  > */
	uint16_t	endPointIndex;		 /**<   结束点   > */		
	uint8_t		peakBottomValue;			 /**<  峰值间隔   > */
	
} HEART_RATE_WAVE_T, *HEART_RATE_WAVE_INDEX;


typedef struct HEART_RATE_PARAM
{
	uint16_t WaveSlopeRange;		  /**< 锯齿波判断标准初始为200 > */
	uint8_t  HeartRateMAX;				 /**<   心率最大值 初始为160    > */
	uint8_t	HeartRateMIN;				 /**<   心率最小值 初始为40    > */
	uint16_t SampleRate;				 /**<   采样率 默认为2ms    > */	
	uint16_t SamplePointTotal;			 /**<   所有的采样点 60*1000/SampleRate > */	
	uint8_t TriggerPeriodPoint;				 /**<  波形窗口采样 10> */	
 	uint16_t SamplePointMax;				 /**<  心率采样最大值默认4000点> */	
	uint16_t EvennessMax;				 /**<  平整度最大值 默认为200> */	
	uint16_t WaveSampleMax;				 /**<  取多少点进行算法> */	
	uint16_t WaveArrayMax;				 /**<  取多少个波形进行计算> */	
	uint16_t RecDetectData;				 /**<  平整度超过多少判断为方波> */	
	uint16_t PeriodMax;					/**< 最大周期数> */	
	uint16_t PeriodMin;					/**< 最小周期数> */	
	uint16_t PeakBottomStand;			/**<   峰值间值          > */	
} HEART_RATE_PARAM_T, *HEART_RATE_PARAM_INDEX;


typedef struct SLOPE
{
	uint16_t value;		
	SlopeDirection  direction;			
	uint8_t smooth;
} SLOPE_T, *SLOPE_INDEX;



/* ram define ------------------------------------------------------------------*/


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


HRState heartRateWaveDetect(uint16_t adData) ;







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

