/**
******************************************************************************
* @file    main.c
* @author  fire
* @version V1.0
* @date    2015-01-xx
* @brief   WF-ESP8266 WiFi模块测试
******************************************************************************
* @attention
*
*
******************************************************************************
*/ 

#include "stm32f10x.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lts1303m.h"
#include  "heart_rate_sample.h"


const HEART_RATE_PARAM_T HeartRateParam = 
{
		
	25,		  /**< 锯齿波判断标准初始为200 > */
	160,			 /**<   心率最大值 初始为160    > */
	40,				 /**<   心率最小值 初始为40    > */
	2,				 /**<   采样率 默认为2ms    > */	
	30000,			 /**<   所有的采样点 60*1000/2 > */	
	10,			 /**<  波形窗口采样 10> */	
	3500,		 /**<  心率采样最大值默认4000点> */	
	200,			 /**<  平整度最大值 默认为200> */	
	4000,			 /**<  取多少点进行算法> */	
	3,		 /**<  取多少个波形进行计算    默认3个> */	
	10,		 /**<  平整度超过多少判断为方波默认10个> */	
	188,	 /**<最大周期数 (60000/HeartRateMAX)/SampleRate> */		
	750,	 /**<最小周期数 (60000/HeartRateMIN)/SampleRate> */		
  100,	 /**< 有效波峰直接的间值    > */		
};

#if 0
typedef struct HEART_RATE_PARAM
{
	uint16_t WaveSlopeRange;		  /**< 锯齿波判断标准初始为200 > */
	uint8_t  HeartRateMAX;				 /**<   心率最大值 初始为160    > */
	uint8_t	HeartRateMIN;				 /**<   心率最小值 初始为40    > */
	uint16_t SampleRate;				 /**<   采样率 默认为20ms    > */	
	uint16_t SamplePointTotal;			 /**<   所有的采样点 60*1000/SampleRate > */	
	uint8_t TriggerPeriodPoint;				 /**<  波形窗口采样 10> */	
 	uint16_t SamplePointMax;				 /**<  心率采样最大值默认4000点> */	
	uint16_t EvennessMax;				 /**<  平整度最大值 默认为200> */	
	uint16_t WaveSampleMax;				 /**<  取多少点进行算法> */	
	uint16_t WaveArrayMax;				 /**<  取多少个波形进行计算> */	
	
} HEART_RATE_PARAM_T, *HEART_RATE_PARAM_INDEX;

#endif

void heartRateSubroutine(){
		int i = 0;	
	    HRState bpmState;
		uint16_t heartRate = 40;

		heartRateInit();
	
		
		for(i=0;i<4000;i++){
		
		bpmState = 	 heartRateWaveDetect(testWave_TYPE3[i]);  
			
		if(bpmState==HRFinish){
		heartRate =	getHeartRateFilter();
			
		heartRateInit();
		
		//
		}
		if(bpmState==HRPointMax){
		heartRateInit();
			}
		if(bpmState==HRLineOut){
		heartRateClrRam();
			}
		
	}

}

/**
* @brief  主函数
* @param  无
* @retval 无
*/
int main(void)
{
	
	heartRateParamSetup(HeartRateParam);
	clrHeartRateStack();
	
	while(1){

	heartRateSubroutine();	
	
	}
		
		
		

	
}


/*********************************************END OF FILE**********************/
