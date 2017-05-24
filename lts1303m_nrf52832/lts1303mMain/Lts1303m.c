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
#include<stdio.h>
#include<string.h>
#include "nrf_gpio.h"
/* const define ------------------------------------------------------------------*/



#define RATE_NUM_MAX                        16
#define WAVE_SAMPLE_MAX					16
#define WAVE_INDEX_MAX					16


const HEART_RATE_WAVE_T HeartRateWaveInit = 
{
	0,      /**< wave type . Like sine, squre sawtooth... */
    0,	    /**< peak index */
	0,	   /**< bottom index */
    0,		/*  上升沿时间   */
    0,     /*  下降沿时间  */
	0,	 /**<    起始点  > */
	0,	 /**<   结束点   > */		
	0,		 /**<  方向 0为上升 1 为下降    > */
	0,		 /**<  第几个波形    > */
};

/* ram define ------------------------------------------------------------------*/
uint16_t heartRate;   

uint16_t lastHeartRate;





uint16_t heartRateStack[RATE_NUM_MAX];
uint16_t heartRateTemp[RATE_NUM_MAX];
uint8_t  heartRateIndex ;




uint16_t pointValueCurrent;
uint16_t pointValueLast;

uint16_t pointCount;
uint16_t smooth;
uint16_t smoothValue;

SlopeDirection  direction;
uint8_t  upDirect,downDirect;
uint16_t topValue,bottomValue;
uint16_t topIndex,bottomIndex;
uint16_t topTempValue,bottomTempValue;
uint16_t topTempIndex,bottomTempIndex;
uint16_t upTrendCount,bottomTrendCount;


uint16_t topLength,bottomLength;

uint8_t	 waveDirection = 0;
uint8_t  waveStart;

uint8_t trigPeriodStart;
uint8_t trigPeriodPoint;

SlopeDirection lastSlopeDirection;

HEART_RATE_WAVE_T waves[16];
HEART_RATE_WAVE_T currentWave;
uint16_t waveIndex;
uint8_t trig;


static HEART_RATE_PARAM_T HeartRateParam;

/**
  *
 **/





/* subroutine define ------------------------------------------------------------------*/


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

uint16_t getArrayAverage(uint16_t *array,int length){
	int i = 0;
	int total = 0;
	for(i=0;i<length;i++){
		total +=array[i];
		}
	return total/length;
}


uint16_t getArrayAverageWithoutPeak(uint16_t *array,int length){
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



void pushArrayData(uint16_t array[],int length,uint16_t data){
	
	int i = 0;

	for(i=length-1;i>0;i--){
		array[i] =array[i-1];
		}
	array[0] = data;
		
}


void clrArrayData(uint16_t array[],int length){
	
	int i = 0;

	for(i=0;i<length;i++){
		array[i] =0;
		}
	
		
}

uint16_t getSlope(uint16_t max,uint16_t min,uint16_t time){
	
	uint16_t slope = 0;
	slope = (max-min)/time;
	return slope;
		
}


uint16_t getSlopeValue(uint16_t before,uint16_t after){
	uint16_t slope = 0;
	if(before>after){
		slope = before-after;
		}else {
			slope = after-before;
			}
	return slope;
		
}

/**
  * @brief  比较前后两值大小，当前面大于后面 返回0 否则返回1
  * @param uint16_t before,uint16_t after
  * @retval type
  */
uint8_t getSlopeType(uint16_t before,uint16_t after){
	
	if(before>after){
		return 0;
		}else {
		return 1;
		}	
}


/**
  * @brief  比较前后两值大小，当前面大于后面 返回0 否则返回1
  * @param uint16_t before,uint16_t after
  * @retval SLOPE_T
  */
SLOPE_T getSlopeObj(uint16_t before,uint16_t after){

	SLOPE_T slopeObj;
	
	if(before>after){
		
		slopeObj.value= before-after;
		slopeObj.direction =SlopeUp;
		}else {

		slopeObj.value= after-before;
		slopeObj.direction = SlopeDown;	
		}
		
	if(slopeObj.value<HeartRateParam.waveSlopeRange){
		slopeObj.smooth =1;
		}else {
		slopeObj.smooth =0;
		}
		
	return slopeObj;
}



void pushWaveArray(){
	currentWave.waveIndex=waveIndex;
	memcpy(&waves[waveIndex],&currentWave,sizeof(currentWave));
	waveIndex++;
	
}


void clrCurrentWave(){

	memcpy(&currentWave,&HeartRateWaveInit,sizeof(currentWave));

}



void heartRateInit(void){
	pointCount = 0;
	heartRateClrRam();
	
}


void heartRateClrRam(void){
	pointValueLast = 0;

	trigPeriodStart = 0;
    trigPeriodPoint = 0;
	smooth = 0;
	direction = SlopeDown;
	waveDirection = 0;
	waveStart = 0;
	waveIndex = 0;
	upDirect =0;
	downDirect = 0;
	trig = 0;
	bottomLength = 0;
	topLength = 0;
	
}


void clrHeartRateStack(){

	lastHeartRate = 0;
	heartRateIndex = 0;

	clrArrayData(heartRateStack,RATE_NUM_MAX);
	
}


void heartRateParamSetup(HEART_RATE_PARAM_T _heartRateParam){

	memcpy(&HeartRateParam,&_heartRateParam,sizeof(_heartRateParam));
	
}


uint16_t getHeartRateFilter(){
	//lastHeartRate 
	
	// heartRate = SamplePointFull/(waveUp+waveDown);
	int i = 0;
	int peaks[WAVE_INDEX_MAX];
	uint16_t hearts[WAVE_INDEX_MAX];
	SLOPE_T slopeObj;
	int count =0;
	
	
	for(;i<waveIndex;i++){
		currentWave = waves[i];
		
		if(waves[i].waveType==1){
			if((waves[i].topIndex-(waves[i].topLength/2))>0){
				
				peaks[i] = waves[i].topIndex-(waves[i].topLength/2);
			}else {
				peaks[i] = waves[i].topIndex;
			}
		
		}else {
				peaks[i] = waves[i].topIndex;
		}
		// printf(" peaks[%d]= %d\r\n",i,peaks[i] );
	}
	
	for(i=0;i<(waveIndex-1);i++){
		heartRate = HeartRateParam.samplePointTotal/ (peaks[i+1]-peaks[i]);
		if(heartRate<=HeartRateParam.heartRateMAX&&heartRate>=HeartRateParam.heartRateMIN){
			hearts[count++]=heartRate;
		
			}
			 printf(" heart [%d]= %d\r\n",i,heartRate );
		
		
	}
	heartRate =	getArrayAverage(hearts,count);
	 printf(" heart = %d\r\n",heartRate );
	//for(;i<waveIndex;i++){
	//	pushArrayData(heartRateTemp,waveIndex,waves[i].heartRate);
	//	}
//	heartRate = getArrayAverage(heartRateTemp,waveIndex);
	
	if(heartRate>HeartRateParam.heartRateMAX){
		
		if(lastHeartRate>0){
			heartRate = lastHeartRate;
			return lastHeartRate;
			}else {
			heartRate= HeartRateParam.heartRateMAX;
			return lastHeartRate;
			}
		}
	
	if(heartRate<HeartRateParam.heartRateMIN){
		if(lastHeartRate>0){
			heartRate = lastHeartRate;
			return lastHeartRate;
			}else {
			heartRate= HeartRateParam.heartRateMIN;
			return lastHeartRate;
				}
		}
	if(lastHeartRate!=0){
		
		slopeObj = getSlopeObj(pointValueCurrent,pointValueLast);
		if(slopeObj.smooth<10){
			heartRate = (heartRate+lastHeartRate)/2;
		}else {
			if(slopeObj.direction>0){
				heartRate = lastHeartRate-1;
				
			}else {
				heartRate = lastHeartRate+1;
			}
		}
	}
		lastHeartRate = heartRate;
		return heartRate;
	
}



HRState getHeartRateWaves(uint16_t adData)   {   
 	
	HRState state;
	SLOPE_T slopeObj;

	
	
	
	state = HRInit;
	
	pointValueCurrent  = adData;
	
	slopeObj = getSlopeObj(pointValueCurrent,pointValueLast);
	
	if(slopeObj.smooth!=0) {				// 数据波动较小
		smooth++;
		smoothValue = pointValueCurrent;
		if(lastSlopeDirection==1){
			bottomLength++;
			downDirect++;
			}else {
			upDirect++;
			topLength++;
			}
		if(smooth>HeartRateParam.recDetectData){
			currentWave.waveType = 1;
			if(smooth>HeartRateParam.evennessMax){
				state = HRLineOut;
			
				}
			}
		}else {
		
		
		lastSlopeDirection = slopeObj.direction;
		smooth=0;
		
		}
	
	
	if(lastSlopeDirection==SlopeDown){					//当前点的斜率方向向下
	
		nrf_gpio_pin_clear(27);

		if(direction==SlopeUp){						//前面向上 --|__
	
			direction = SlopeDown;
			downDirect = 0;
				topTempIndex = pointCount;
			}else {								
				downDirect++;	
				}
		
		
	
		
		
		}else {	
		
		nrf_gpio_pin_set(27);
			
		if(direction==SlopeDown){			//前面向下		 __|--
			printf(" dir[%d] u=%d d=%d\r\n",pointCount,upDirect,downDirect);
			upDirect =0;
			direction = SlopeUp;
			
			bottomTempIndex = pointCount;
			
		
		}else {
		upDirect++;		
			}
		
		
		
		
		
		
		}
	
	
	pointValueLast = pointValueCurrent;
	pointCount++;
	if(pointCount>=HeartRateParam.samplePointMax){
		state = HRPointMax;
		}
	return state;
	
} 


