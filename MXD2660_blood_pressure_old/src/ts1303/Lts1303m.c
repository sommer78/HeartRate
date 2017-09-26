/*
*********************************************************************************************************
*               Copyright(c) 2017, ETEK Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      lts1303m.C
* @brief      file of lts1303m
* @details   
* @author    jiangs
* @date      2017-05-04
* @version   v0.1
* *********************************************************************************************************
*/



/* Includes ------------------------------------------------------------------*/
#include "lts1303m.h"
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

#include "system_config.h"
#include "mxd2660_adc.h"
#include "mxd2660_gpio.h"
#include "mxd2660_timer.h"
#include "utility.h"


/* const define ------------------------------------------------------------------*/



#define RATE_NUM_MAX                        16
#define PLUSE_SAMPLE_MAX					32
#define WAVE_INDEX_MAX					16




#if 0
const HEART_RATE_PARAM_T HeartRateParamConst = 
{
		
	WaveSlopeRange,		  /**< 锯齿波判断标准初始为200 > */
	HeartRateMAX,			 /**<   心率最大值 初始为160    > */
	HeartRateMIN,				 /**<   心率最小值 初始为40    > */
	SampleRate,				 /**<   采样率 默认为2ms    > */	
	SamplePointTotal,			 /**<   所有的采样点 60*1000/2 > */	
	TriggerPeriodPoint,			 /**<  波形窗口采样 10> */	
	SamplePointMax,		 /**<  心率采样最大值默认4000点> */	
	SmoothMax,			 /**<  平整度最大值 默认为600> */	
	WaveArrayMax,		 /**<  取多少个波形进行计算    默认3个> */	
	RecDetectData,		 /**<  平整度超过多少判断为方波默认10个> */	
	PeriodMax,	 /**<最大周期数 (60000/HeartRateMAX)/SampleRate> */		
	PeriodMin,	 /**<最小周期数 (60000/HeartRateMIN)/SampleRate> */		
  PeakBottomStand,	 /**< 有效波峰直接的间值    > */		
};
#endif


/* ram define ------------------------------------------------------------------*/
uint16_t heartRate;   

uint16_t lastHeartRate = 60;

uint8_t  heartRateIndex  =0;





uint16_t heartRateStack[RATE_NUM_MAX];
uint16_t heartRateTemp[RATE_NUM_MAX];



uint16_t peakIndexs[PLUSE_SAMPLE_MAX];


uint8_t wavePulseState;
int peakCount ;
int pulseIndex;	//每帧指示
int peakStart;	//高峰开始指示
int peakStop;	//高峰结束指示
int bottomStart;
int bottomStop;
int pointCount;	//总时间指示
int peakAve;
int pulsePeakIndex;   //高电平 中间点位置



//static HEART_RATE_PARAM_T HeartRateParam;

/**
  *
 **/





/* subroutine define ------------------------------------------------------------------*/

void heartRateInit(void){
	pointCount = 0;
	wavePulseState = 0;
	peakCount= 0;
	pulseIndex = 0;
	pulsePeakIndex = 0;

}
/**
  *	1: bottom 2 peak 0 other
 **/


uint8_t getWavePeakBottom(uint16_t adData){
	
	uint16_t tmp;
	
	tmp = adData;
	if(tmp<100){
		return 1;
		}
	tmp= adData-40;
	
	if(tmp>900){
		return 2;
		}else {
		return 0;
		}	
}




HRState getHeartRateWaveInfo(uint16_t adData,int count) {
	HRState state;
	uint8_t peak;
	state = HRInit;
	// APP_DBG(("adData = %d\r\n",adData));
	
	peak = getWavePeakBottom( adData);
//	APP_DBG(("isWavePeak = %d\r\n",peak));
	pointCount++;
	

	if(wavePulseState==0){			//判断高峰
		if(peak==2){
			peakCount++;
			if(peakCount>10){
				peakCount = 0;
				wavePulseState =1;
				if(pulseIndex==0){	//首帧开始
			
				pointCount=10;			
				peakStart = 10;
				pulseIndex = 1;
				APP_DBG(("peakstart[%d] = %d \r\n ",count,adData));		  
				}else {						//下一帧处理
				APP_DBG(("peakstart2[%d] = %d \r\n ",count,adData));		  
				bottomStop = pointCount;
				peakStart = pointCount-10;
				pulseIndex++;
				if(pulseIndex>16){
					state=HRFinish;
					}
			
				
					}
				}

			
			}else {
			peakCount=0;
				}
			
		}else {					//	判断低谷
		if(peak!=2){			//不等于高峰值
			peakCount++;
			if(peakCount>10){
				wavePulseState =0;
				peakStop = pointCount-10;
				peakAve = peakStop-peakStart;
				pulsePeakIndex= peakStart+peakAve;
				peakIndexs[pulseIndex-1]=pulsePeakIndex; 
				APP_DBG(("peakstop[%d] = %d peakAve = %d pulsePeakIndex = %d \r\n ",count,adData,peakAve,pulsePeakIndex));		  
				}else {						
				bottomStart = pointCount;
				
				
	
				}

			
			}else {
			peakCount=0;
				}
		}
	

	if(pointCount>=SamplePointMax){
		state = HRError;
		}
	
	return state;
	
}



/**
  * @brief  This function getArrayMaxIndex
  * @param uint16_t  array[], int len
  * @retval array max index
  */
int getArrayMaxIndex(uint16_t *array,int len){
	int i,iMax;

    for(i=iMax=0;i<len;i++){
        if(array[iMax]<array[i]){
			  iMax=i;
        	}
          
    	}
    return iMax;
}

int getArrayMinIndex(uint16_t *array,int len){
	int i,iMin;

    for(i=iMin=0;i<len;i++)
        if(array[iMin]>array[i])
            iMin=i;
  
    return iMin;
}

uint16_t getHeartAverage(uint16_t *array,int length){
	int i = 0;
	int total = 0;
	for(i=0;i<length;i++){
		total +=array[i];
		}
	return total/length;
}


uint16_t getHeartAverageWithoutPeak(uint16_t *array,int length){
	int i = 0;
	int total = 0;
	int maxIndex,minIndex;
	uint16_t ave;
	minIndex = getArrayMinIndex(array,length);
	maxIndex = getArrayMaxIndex(array,length);
	
	for(i=0;i<length;i++){
		if(i!=maxIndex&&i!=minIndex){
			total +=array[i];
			}	
		}
	ave = total/(length-2);
	return ave;
}

uint16_t getHeartRateSmooth(uint16_t tmpHeartRate){
	int i = 0;
	
	if(heartRateIndex<9){
			heartRateStack[heartRateIndex++]=tmpHeartRate;
			heartRate = getHeartAverage(heartRateStack,heartRateIndex);
	}else {
	for(i=0;i<7;i++){
	//	APP_DBG(("b heart = %d\r\n",heartRateStack[i] ));
		heartRateStack[i]=heartRateStack[i+1];
		//APP_DBG(("a heart = %d\r\n",heartRateStack[i] ));
		//nrf_delay_ms(100);
		}
	heartRateStack[7] = tmpHeartRate;
	heartRate = getHeartAverage(heartRateStack,8);
	}
	APP_DBG((" smooth heart  = %d\r\n",heartRate ));
	
	return heartRate;
}


uint16_t getHeartRateDetection(void){
	int i = 0;
	uint16_t tmpHeartRate;
	int count = 0;
	for(i=0;i<(pulseIndex-2);i++){
		//APP_DBG(("getHeartRateDetection[%d] = %d  \r\n ",i,peakIndexs[i+1]-peakIndexs[i]));	
		tmpHeartRate = SamplePointTotal/ (peakIndexs[i+1]-peakIndexs[i]);
		// APP_DBG((" heart [%d]= %d\r\n",i,tmpHeartRate ));
		// nrf_delay_ms(100);
		if(tmpHeartRate<=HeartRateMAX&&tmpHeartRate>=HeartRateMIN){
			heartRateTemp[count++]=tmpHeartRate;
		
			}
	}
	if(count!=0){
	tmpHeartRate =	getHeartAverageWithoutPeak(heartRateTemp,count);
	}else {
	tmpHeartRate = lastHeartRate;
	}
	
	APP_DBG(("  before heart = %d\r\n",tmpHeartRate ));
	heartRate = tmpHeartRate;
	lastHeartRate = tmpHeartRate;
	
	 

	return heartRate;
}

void heartDealLoop(uint16_t adc){
	 uint16_t i;
	 HRState bpmState;
//	 int size ;
	 heartRateInit();
//	 size =  sizeof(wave_2660)/sizeof(uint16_t);
	//  APP_DBG(("size = %d \r\n ",size));		  
//	  for (i = 0; i < size; i++){
       
		
		// APP_DBG(("s[%d] = %d \r\n ",i,wave_2660[i]));		  
		 bpmState = 	 getHeartRateWaveInfo(adc,0);  
		
		if(bpmState==HRFinish){
		
		heartRate =	getHeartRateDetection();
		heartRate =	getHeartRateSmooth(heartRate); 
		 APP_DBG(("heartRate = %d\r\n",heartRate));		
		heartRateInit();
		
		//
		}
		
		if(bpmState==HRError){
			 APP_DBG(("HRError \r\n"));		
			heartRateInit();
			}
	 
}

void adc_read_callback(void)
{
//	HRState 	bpmState;
	uint16		adc=0;
 
	adc=adc_io_read(ADC_CH1);
//	heartDealLoop(adc);
	APP_DBG(("adc = %d\r\n",adc));
}

void hw_timer1_handler(void)
{
	adc_read_callback();
}

void hw_timer_config(void)
{
	NVIC_ClearPendingIRQ(TMR0_IRQn);
    NVIC_SetPriority(TMR0_IRQn, 0x3);
    NVIC_EnableIRQ(TMR0_IRQn);
	NVIC_ClearPendingIRQ(TMR1_IRQn);
    NVIC_SetPriority(TMR1_IRQn, 0x3);
    NVIC_EnableIRQ(TMR1_IRQn);
	NVIC_ClearPendingIRQ(TMR2_IRQn);
    NVIC_SetPriority(TMR2_IRQn, 0x3);
    NVIC_EnableIRQ(TMR2_IRQn);
	hw_timer_init(TIME1, TIMER_CLK_8M, 10, TIMER_CHANNEL_0, hw_timer1_handler);

	APP_DBG(("hw_timer_config done!\r\n"));
}

void bsp_ts1303_init(void)
{
	gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
    adc_init(7);
	
	gpio_init_output(GPIO_21,GPIO_PULL_NONE,1);
	hw_timer_start(TIME1);
}


