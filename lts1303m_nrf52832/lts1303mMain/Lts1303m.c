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
#define PLUSE_SAMPLE_MAX					16
#define WAVE_INDEX_MAX					16


const HEART_RATE_PULSE_T HeartRatePulseInit = 
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

uint8_t	 pluseDirection = 0;
uint8_t  waveStart;

uint16_t period;
uint8_t firstPeriod;

SlopeDirection lastSlopeDirection;

HEART_RATE_PULSE_T pulseStacks[PLUSE_SAMPLE_MAX];
HEART_RATE_PULSE_T currentPulse;
uint8_t pulseIndex;



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



void pushPulseStack(){
	currentPulse.index=pulseIndex;
	memcpy(&pulseStacks[pulseIndex],&currentPulse,sizeof(currentPulse));
	pulseIndex++;
	
}
void  clrPulseStack(){
	int i = 0;
	for(;i<PLUSE_SAMPLE_MAX;i++)
	memcpy(&pulseStacks[i],&HeartRatePulseInit,sizeof(currentPulse));

}

void clrCurrentPulse(){

	memcpy(&currentPulse,&HeartRatePulseInit,sizeof(currentPulse));

}



void heartRateInit(void){
	printf(" heartRateInit \r\n" );
	pointCount = 0;
	heartRateClrRam();
}
void printCurrentPluse(){
	printf(" cp[%d] t=%d s=%d e=%d p=%d u=%d d=%d tl=%d bl=%d ti=%d\r\n",currentPulse.index,currentPulse.type,currentPulse.startPointIndex,currentPulse.endPointIndex,currentPulse.period,
		currentPulse.upTime,currentPulse.downTime,currentPulse.topLength,currentPulse.bottomLength,currentPulse.topIndex);
}

void heartRateClrRam(void){
	
	pointValueLast = 0;	
	smooth = 0;
	pluseDirection = 0;
	waveStart = 0;
	pulseIndex = 0;
	upDirect =0;
	downDirect = 0;
	bottomLength = 0;
	topLength = 0;
	firstPeriod = 0;
	
}


void clrHeartRateStack(){

	lastHeartRate = 0;
	heartRateIndex = 0;

	clrArrayData(heartRateStack,RATE_NUM_MAX);
	
}


void heartRateParamSetup(HEART_RATE_PARAM_T _heartRateParam){

	memcpy(&HeartRateParam,&_heartRateParam,sizeof(_heartRateParam));
	HeartRateParam.oneMinutePoint = 60000/HeartRateParam.sampleRate;
	HeartRateParam.periodMax = HeartRateParam.oneMinutePoint/HeartRateParam.heartRateMIN;
	HeartRateParam.periodMin = HeartRateParam.oneMinutePoint/HeartRateParam.heartRateMAX;
}


uint16_t getHeartRateFilter(){
	//lastHeartRate 
	
	// heartRate = SamplePointFull/(waveUp+waveDown);
	int i = 0;
	int peaks[WAVE_INDEX_MAX];
	uint16_t hearts[WAVE_INDEX_MAX];
	SLOPE_T slopeObj;
	int count =0;
	
	
	for(;i<pulseIndex;i++){
		currentPulse = pulseStacks[i];
	//	printCurrentPluse();
		if(pulseStacks[i].type==1){
			if((pulseStacks[i].topIndex-(pulseStacks[i].topLength/2))>0){
				
				peaks[i] = pulseStacks[i].topIndex-(pulseStacks[i].topLength/2);
			}else {
				peaks[i] = pulseStacks[i].topIndex;
			}
		
		}else {
				peaks[i] = pulseStacks[i].topIndex;
		}
		// printf(" peaks[%d]= %d\r\n",i,peaks[i] );
	}
	
	for(i=0;i<(pulseIndex-1);i++){
		heartRate = HeartRateParam.oneMinutePoint/ (peaks[i+1]-peaks[i]);
		if(heartRate<=HeartRateParam.heartRateMAX&&heartRate>=HeartRateParam.heartRateMIN){
			hearts[count++]=heartRate;
		
			}
		//	 printf(" heart [%d]= %d\r\n",i,heartRate );
		
		
	}
	if(count!=0){
	heartRate =	getArrayAverage(hearts,count);
	 printf(" heart first = %d\r\n",heartRate );
	}else {
	heartRate = lastHeartRate;
		}
	
	//for(;i<pulseIndex;i++){
	//	pushArrayData(heartRateTemp,pulseIndex,pulseStacks[i].heartRate);
	//	}
//	heartRate = getArrayAverage(heartRateTemp,pulseIndex);
	
	if(heartRate>HeartRateParam.heartRateMAX){
		
		if(lastHeartRate>0){
			heartRate = lastHeartRate;
			return heartRate;
			}else {
			heartRate= HeartRateParam.heartRateMAX;
			lastHeartRate = heartRate;
			return heartRate;
			}
		}
	
	if(heartRate<HeartRateParam.heartRateMIN){
		if(lastHeartRate>0){
			heartRate = lastHeartRate;
			return heartRate;
			}else {
			heartRate= HeartRateParam.heartRateMIN;
			lastHeartRate = heartRate;
			return heartRate;
				}
		}
	if(lastHeartRate!=0){
		
		slopeObj = getSlopeObj(heartRate,lastHeartRate);
		if(slopeObj.smooth<10){
				
			heartRate = (heartRate+lastHeartRate)/2;
			
			 printf(" little change heart = %d\r\n",heartRate );
		}else {
			if(slopeObj.direction>0){
				heartRate = lastHeartRate-1;
				heartRateIndex = 0;
				
			}else {
				heartRate = lastHeartRate+1;
			}
			heartRateStack[heartRateIndex]=heartRate;
			heartRateIndex++;
			if(heartRateIndex>8){
				heartRate = getArrayAverageWithoutPeak(heartRateStack,8);
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
		if(lastSlopeDirection==SlopeDown){
			
			downDirect++;
			bottomLength++;
			}else {
			upDirect++;
			topLength++;
			}
		if(smooth>HeartRateParam.recDetectData){
			currentPulse.type= 1;
			
			}
		if(smooth>HeartRateParam.evennessMax){
				state = HRLineOut;
				if(smoothValue<50){
				direction = SlopeDown;
				}else if(smoothValue>1000){
				 	direction = SlopeUp;
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
			topTempValue = pointValueCurrent;
			}							
			downDirect++;	
			
		
		
	
		
		
		}else {	
		
		nrf_gpio_pin_set(27);
			
		if(direction==SlopeDown){			//前面向下		 __|--
			
		
			bottomTempIndex = pointCount;
			period = upDirect+downDirect;
		//	printf(" p[%d] u=%d d=%d p=%d\r\n",pointCount,upDirect,downDirect,period);
		if(period<HeartRateParam.evennessMax&&period>HeartRateParam.periodMin){
				
				if(firstPeriod==0){
					currentPulse.startPointIndex=pointCount;
					firstPeriod = 1;
					}else {
					currentPulse.endPointIndex=pointCount;
					currentPulse.upTime = upDirect;
				    currentPulse.downTime = downDirect;
					currentPulse.bottomLength =bottomLength;
					currentPulse.topLength=topLength;
					currentPulse.topIndex = topTempIndex;
					currentPulse.period = period;
					pushPulseStack();
					printCurrentPluse();
					clrCurrentPulse();
					currentPulse.startPointIndex=pointCount;
					if(pulseIndex>HeartRateParam.waveArrayMax){
					//	clrPulseStack();
						state = HRFinish;
						}
					}
				upDirect = 0;
				downDirect = 0;
				bottomLength = 0;
				topLength = 0;
					
				
				}
				else if(upDirect<10) {

				}
			upDirect = 0;
			direction = SlopeUp;
		}
		upDirect++;		
			
		
		
		
		
		
		
		}
	
	
	pointValueLast = pointValueCurrent;
	pointCount++;
	if(pointCount>=HeartRateParam.samplePointMax){
		state = HRPointMax;
		}
	return state;
	
} 


