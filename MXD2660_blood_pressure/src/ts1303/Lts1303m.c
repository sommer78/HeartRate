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
#include "sample.h"


/* const define ------------------------------------------------------------------*/



#define RATE_NUM_MAX                        16
#define PLUSE_SAMPLE_MAX					32

#define AD_SMOOTH_MAX					32

//#define ADSMOOTH	1
#define AD_SAMPLE_MAX	1000


#define MAXLEVEL	820
#define MINLEVEL	40
#define MAXTOPLEVEL	100
#define MAXBOTTOMLEVEL	100

#define  SamplePointTotal 6000		



/* ram define ------------------------------------------------------------------*/
uint16_t heartRate;   

uint16_t lastHeartRate = 75;

uint8_t  heartRateIndex  =0;

uint8_t  heartRateOddIndex=0;
uint16_t heartRateOddStack[RATE_NUM_MAX];



uint16_t adSmoothStack[AD_SMOOTH_MAX];

uint16_t heartRateStack[RATE_NUM_MAX];


uint16_t adValues[AD_SAMPLE_MAX];


int plusePeakLocStack[PLUSE_SAMPLE_MAX];



int topStart;
int slopeUp;
int slopeDown;
uint16_t maxValue;
uint16_t minValue;
int totalPoint = 0;
int top;

int pointCount;	
int isTypeRect;
int currentPosition; 
int bottomStart;
int bottom;

WaveSLope slope;
int pulseIndex;







//static HEART_RATE_PARAM_T HeartRateParam;

/**
  *
 **/

#define debug_gpio


void debubGpioChange(){
#ifdef debug_gpio	
		if(gpio_read_output_bit(GPIO_19)){
				gpio_clear_bit(GPIO_19);
			}else {
				gpio_set_bit(GPIO_19);
			}
	
#endif

}



/* subroutine define ------------------------------------------------------------------*/

void heartRateInit(void){
	APP_DBG((" init \r\n"));

	pointCount = 0;
//	wavePulseState = 0;
//	peakCount= 0;
	pulseIndex = 0;
	currentPosition = 0;
	maxValue = 0;
	minValue = 1024;
	//firstWave = 0;
	//bottomValue = 0;
//	waveType = 0;
	slope = SlopeBottom;
	top = 0;
	bottom = 0;
//	middle = 0;
	isTypeRect = 0;
//	topCount = 0;
//	bottomCount = 0;
	topStart = 0;
	slopeUp=0;
	slopeDown=0;
	memset(plusePeakLocStack,0x00,PLUSE_SAMPLE_MAX);
	
}


int isSamliarValue(uint16_t first ,uint16_t second){
	
	int Similar = 10;
	if((first-second)>Similar){
		return 0;
		}
	if((second-first)>Similar){
		return 0;
		}
	return 1;
}


void heartRateStackInit(void){

	memset(heartRateStack,75,RATE_NUM_MAX);
	
}


int isHeartRateValid(uint16_t heartRate){
	if(heartRate>HeartRateMAX){
		APP_DBG(("HeartRateMAX = %d\r\n",heartRate));
		return 1;
		}
	if(heartRate<HeartRateMIN){
		APP_DBG(("HeartRateMIN = %d\r\n",heartRate));
		return 2;
		}
	return 0;
}

/**
  *	1: bottom 2 peak 0 other
 **/
int isPeakBottom(uint16_t adData){
	
	uint16_t tmp;
	
	tmp = adData;
	if(tmp<MINLEVEL){
		return 1;
		}
	
	if(tmp>MAXLEVEL){
		return 2;
		}else {
		return 0;
	}	
}

int pushPulsePeakStack(int pulsePeakLocation){
	
	
	if(pulseIndex<PLUSE_SAMPLE_MAX){
	plusePeakLocStack[pulseIndex]= pulsePeakLocation;
	//plusePeakValStack[pulseIndex]= minValue;
	}
	pulseIndex++;
	if(pulseIndex>=PLUSE_SAMPLE_MAX){
		pulseIndex = PLUSE_SAMPLE_MAX-1;
		}
	
	
	return pulseIndex;
}
int updatePulsePeakStack(int pulsePeakLocation){
	
	
	
	plusePeakLocStack[pulseIndex-1]= pulsePeakLocation;
	
	
	
	return pulseIndex;
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

uint16_t getArrayAverage(uint16_t *array,int length){
	int i = 0;
	uint32_t total = 0;
	uint16_t aveH;

	if(length==0){
		return 0;
		}
	for(i=0;i<length;i++){
		total +=array[i];
		}
	aveH = total/length;
	if((total%length)>(length/2)){
		aveH +=1;
		}

	return aveH;
}

int regularPulseWave(){
	int i=0;

	
	
	for(i=0;i<pulseIndex;i++){
	//	APP_DBG(("plusePeakTemp[%d] = %d\r\n",i,plusePeakLocStack[i]));
		}
	
	return pulseIndex;
}


uint16_t getHeartRateDetection(void){
	int i = 0;
	uint16_t tmpHeartRate;
	int count = 0;
	uint16_t heartRateTemp[RATE_NUM_MAX];
	for(i=0;i<pulseIndex-1;i++){
	//	APP_DBG(("getHeartRateDetection[%d] = %d  \r\n ",i,plusePeakLocStack[i+1]-plusePeakLocStack[i]));	
		tmpHeartRate = SamplePointTotal/ (plusePeakLocStack[i+1]-plusePeakLocStack[i]);
		 APP_DBG((" heart [%d]= %d\r\n",i,tmpHeartRate ));
	if(isHeartRateValid(tmpHeartRate)==0){
			heartRateTemp[count++]=tmpHeartRate;
		}

	}
	if(count>0){
		
	tmpHeartRate =	getArrayAverage(heartRateTemp,count);
	if(isHeartRateValid(tmpHeartRate)==0){
		heartRate = tmpHeartRate;
			return 0;
		}
	
	}else {
	return 1;
	}
	
	 

	return 2;
}





HRState getHeartRateWave(){
	int peak;

	uint16_t adData;
	uint16_t lastAdData=0;

//	int hasSlopeDown=0;

	int lastPosition = 0;
	int distance=0;
	int lastMinValue=0;
	int ret;
	int isBottomStart=0;
	
	WaveSLope currentDirect;
	//	 APP_DBG(("adData = %d\r\n",adData));
	pointCount=0;
	isTypeRect =0;
	currentPosition = 0;
	bottomStart = 0;
	bottom = 0;
	slope = SlopeBottom;
	pulseIndex = 0;

	for (pointCount = AD_SMOOTH_MAX; pointCount < AD_SAMPLE_MAX; pointCount++){
		
		adData =	adValues[pointCount];

		peak = isPeakBottom( adData);


		if(peak==2){					//  波峰    平
			
			bottom=0;
			top++;
			if(top>MAXTOPLEVEL){
				return HRErrTopLong;
				}
			
//		hasSlopeDown = 1;

		slope = SlopeTop;
		currentDirect = SlopeTop;
		slopeDown=0;
		slopeUp = 0;
		maxValue = adData;
		continue;
		
		}else if(peak==1){				//波谷     平
			bottom++;
			top =0;
			
			if(bottom>MAXBOTTOMLEVEL){
				return HRErrBottomLong;
				}
			if(isBottomStart==0){
				bottomStart = pointCount;
				isTypeRect =1;			
				}
		


		slope = SlopeBottom;
		currentDirect = SlopeBottom;
		slopeUp = 0;
		minValue = adData; 
		continue;

			
		}
	//	 APP_DBG(("normal = %d\r\n",lastAdData));

		if(lastAdData<adData){
		slopeUp++;
		slopeDown=0;
		}else {
		slopeDown++;
		slopeUp=0;
		}	

		top = 0;

		if(slopeUp>10){					//上升沿	
			slope= SlopeUP;
			maxValue =0;
		   
			if(currentDirect!=SlopeUP){
					currentDirect = SlopeUP;
					if(isTypeRect==1){
						currentPosition = bottomStart+bottom/2;
						APP_DBG((" [rect]  start:%d  len:%d position:%d \r\n",bottomStart,bottom/2,currentPosition));
						isTypeRect = 0;
						}else {
						APP_DBG((" [normal] position:%d \r\n",currentPosition));
						currentPosition = bottomStart;
							}
					distance = currentPosition-lastPosition;
					
					APP_DBG(("  bottom: %d  minValue: %d current : %d distance:%d \r\n",bottomStart,minValue,pointCount,distance));
					
					if(pulseIndex==0){
						APP_DBG((" push first %d \r\n" ,currentPosition));
						pushPulsePeakStack(currentPosition);
						}else {
							if(distance>40){							//AD_SAMPLE_MAX /HeartRateMAX
								APP_DBG((" push  %d \r\n" ,currentPosition));
								
							pushPulsePeakStack(currentPosition);
							}else {
								if((lastMinValue-minValue)>0){
								APP_DBG((" update  %d \r\n" ,currentPosition));
									updatePulsePeakStack(currentPosition);
									}else {
										}
								}
						}
					
					
					
					if(pulseIndex>10){
						regularPulseWave();
						}
				
					//lastPosition = currentPosition;
						APP_DBG(("F  lastPosition :%d \r\n",lastPosition));
					lastMinValue = minValue;
				}
			
			}

		if(slopeDown>10){			//下降沿
			slope = SlopeDown;
		//	hasSlopeDown = 1;
	    	minValue =1024;
			if(currentDirect!=SlopeDown){
					currentDirect = SlopeDown;
				}
		
			}
			
				
			// sine wave detection
	
			
			if(slope==SlopeDown){				
				if(adData<minValue){
				minValue = adData;
		//		APP_DBG(("MIN[]: %d Posi: %d\r\n",adData,pointCount));
				
				bottomStart = pointCount;
				
				}
			}else if(slope==SlopeUP) {

			if(adData>maxValue){
				maxValue = adData;
		//	APP_DBG(("MAX**: %d Posi: %d\r\n",adData,pointCount));
	
				}
			}
			
		

		

	lastAdData = adData;

	
  }
	
	if(pulseIndex>2){
	ret  = getHeartRateDetection();
	if(ret == 0){
		if(isHeartRateValid(heartRate)==0){
				return HRFinish;
			}else {
			return HRErrRate;
				}
	
		}else {
		return HRError;
		}
	}


	return HRErrPointOut;

}





/**
  * @brief  This function get ad value avager
  * @param uint16_t adValue
  * @retval adAvergerValue
  */

uint16_t adValueFilter(uint16_t adValue){
	int i = 0;
	uint16_t adAvergerValue;
	for(i=0;i<AD_SMOOTH_MAX-1;i++){
	//	APP_DBG(("b heart = %d\r\n",heartRateStack[i] ));
		adSmoothStack[i]=adSmoothStack[i+1];
		//APP_DBG(("a heart = %d\r\n",heartRateStack[i] ));
		}
	adSmoothStack[AD_SMOOTH_MAX-1] = adValue;
	adAvergerValue = getArrayAverage(adSmoothStack,AD_SMOOTH_MAX);
	
	//APP_DBG((" smooth ad  = %d\r\n",adAvergerValue ));
	
	return adAvergerValue;
}



uint16_t getArrayAverageWithoutPeak(uint16_t *array,int length){
	int i = 0;
	int total = 0;
	int maxIndex,minIndex;
	uint16_t ave;
	minIndex = getArrayMinIndex(array,length);
	maxIndex = getArrayMaxIndex(array,length);
	if(length<4){
		for(i=0;i<length;i++){
		
			total +=array[i];
		
			}
		ave = total/(length);
    	return ave;
		}else {
		for(i=0;i<length;i++){
			if(i!=maxIndex&&i!=minIndex){
				total +=array[i];
				}	
		}
		ave = total/(length-2);
		return ave;
		}
}

uint16_t getHeartRateSmooth(uint16_t tmpHeartRate){
	int i = 0;
	APP_DBG(("original heart = %d\r\n",tmpHeartRate));	

	if(heartRateIndex<9){
			heartRateStack[heartRateIndex++]=tmpHeartRate;
			heartRate = getArrayAverage(heartRateStack,heartRateIndex);
	}else {
	for(i=0;i<7;i++){
	//	APP_DBG(("b heart = %d\r\n",heartRateStack[i] ));
		heartRateStack[i]=heartRateStack[i+1];
		//APP_DBG(("a heart = %d\r\n",heartRateStack[i] ));
		//nrf_delay_ms(100);
		}
	heartRateStack[7] = tmpHeartRate;
	heartRate = getArrayAverage(heartRateStack,8);
	}
	APP_DBG((" smooth heart  = %d\r\n",heartRate ));
	
	return heartRate;
}


int compareTwoDatas(uint16_t first ,uint16_t second,int value){
	

	if((first-second)>value){
		return 0;
		}
	if((second-first)>value){
		return 0;
		}
	return 1;
}



uint16_t getHeartRateSmoothData(uint16_t tmpHeartRate){
	int i = 0;
	APP_DBG(("original heart = %d\r\n",tmpHeartRate));	
	if(heartRateIndex==0){
		heartRateOddIndex = 0;
		heartRate = tmpHeartRate;
		lastHeartRate = heartRate;
		heartRateStack[heartRateIndex++]=tmpHeartRate;
		return heartRate;
		}
	if(compareTwoDatas(lastHeartRate,tmpHeartRate,10)==1){
		heartRateOddIndex = 0;
		if(heartRateIndex<9){
			heartRateStack[heartRateIndex++]=tmpHeartRate;
			heartRate = getArrayAverage(heartRateStack,heartRateIndex);
			lastHeartRate = heartRate;
			return heartRate;
			}else {
				for(i=0;i<7;i++){
				heartRateStack[i]=heartRateStack[i+1];
				}
			    heartRateStack[7] = tmpHeartRate;
				heartRate = getArrayAverage(heartRateStack,8);
				lastHeartRate = heartRate;
				return heartRate;
			}
		}else {
			if(heartRateOddIndex<4){
					heartRateOddStack[heartRateOddIndex++]=tmpHeartRate;
					heartRate = lastHeartRate;
					return heartRate;
				}else{
					for(i=0;i<2;i++){
					heartRateStack[i]=heartRateOddStack[i];
					}
					heartRate = getArrayAverage(heartRateStack,3);
					lastHeartRate = heartRate;
					return heartRate;
				}
		}
	
	APP_DBG((" smooth heart  = %d\r\n",heartRate ));
	
	return heartRate;
}



void heartDealSimLoop(void){
	 uint16_t i;
	 HRState bpmState;

	 
	 APP_DBG(("\r\n***********************heartDealSimLoop*********************** \r\n ")); 

   for(i=0;i<AD_SAMPLE_MAX;i++){
   	adValues[i] = wave_2660_0[i];
   	}

	/* 滤波函数 得到均值滤波的值*/
  for (i = 0; i < AD_SMOOTH_MAX; i++){
  	adSmoothStack[i] = adValues[i];
  	}
  for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
	 adValues[i]=adValueFilter (adValues[i]);
	 }


  	/* 心跳处理 得到心跳间隔周期  */
	bpmState = 	 getHeartRateWave();  
  
	if(bpmState==HRFinish){
	if(isHeartRateValid(heartRate)==0){
		heartRate =	getHeartRateSmooth(heartRate); 
	 	APP_DBG(("heartRate = %d\r\n",heartRate));		
		}
	
	heartRateInit();
	
	//
	}
	
	
	if(bpmState==HRErrBottomLong
		||bpmState==HRErrTopLong
		||bpmState==HRErrPointOut
		||bpmState==HRErrPulseLength
		||bpmState==HRError
		||bpmState==HRErrShortWave
		||bpmState==HRErrLongWave){
		 APP_DBG(("HRError:%d \r\n", bpmState));		
		heartRateInit();
		}

   totalPoint= 0;
	 APP_DBG(("\r\n***********************finish*********************** \r\n ")); 
	//  APP_DBG(("top  = %d bottom=%d middle=%d  \r\n",top,bottom,middle));		
}


void heartDealLoop(void){
	 uint16_t i;
	 HRState bpmState;
	 	
//	 size =  sizeof(wave_2660_2)/sizeof(uint16_t);
//	  APP_DBG(("size = %d \r\n ",size));	
  APP_DBG(("\r\n***********************heartdealloop*********************** \r\n ")); 

	/* 滤波函数 得到均值滤波的值*/
  for (i = 0; i < AD_SMOOTH_MAX; i++){
  	adSmoothStack[i] = adValues[i];
  	}
  for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
	 adValues[i]=adValueFilter (adValues[i]);
	 }


  	/* 心跳处理 得到心跳间隔周期  */
	bpmState = 	 getHeartRateWave();  
  
	if(bpmState==HRFinish){
	

	
	heartRate =	getHeartRateSmooth(heartRate); 
	 APP_DBG(("heartRate = %d\r\n",heartRate));		
	heartRateInit();
	
	//
	}
	
	
	if(bpmState==HRErrBottomLong
		||bpmState==HRErrTopLong
		||bpmState==HRErrPointOut
		||bpmState==HRErrPulseLength
		||bpmState==HRError
		||bpmState==HRErrShortWave
		||bpmState==HRErrLongWave){
		 APP_DBG(("HRError:%d \r\n", bpmState));		
		heartRateInit();
		}

   totalPoint= 0;
	 APP_DBG(("\r\n***********************finish*********************** \r\n ")); 
	//  APP_DBG(("top  = %d bottom=%d middle=%d  \r\n",top,bottom,middle));		
}

void adc_read_callback(void)
{

	uint16		adc=0;
 
	adc=adc_io_read(ADC_CH1);
	APP_DBG(("%d,",adc));
	//totalPoint++;
	adValues[totalPoint++]=adc;
	//heartDealLoop(adc);
	
//	debubGpioChange();

	

}

void hw_timer1_handler(void)
{
	if(totalPoint<(AD_SAMPLE_MAX)){
	adc_read_callback();
	}else {
	//hw_timer_stop(TIME1);
	heartDealLoop();
		}
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
	hw_timer_init(TIME1, TIMER_CLK_1M, 10, TIMER_CHANNEL_0, hw_timer1_handler);

	APP_DBG(("hw_timer_config done!\r\n"));
}

void bsp_ts1303_init(void)
{
	gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
    adc_init(7);
	
	gpio_init_output(GPIO_21,GPIO_PULL_NONE,1);
	hw_timer_start(TIME1);
	heartRateStackInit();
//	heartDealSimLoop();

}


