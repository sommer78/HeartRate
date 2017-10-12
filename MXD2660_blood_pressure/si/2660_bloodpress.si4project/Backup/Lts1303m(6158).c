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

#define AD_SMOOTH_MAX					64

//#define ADSMOOTH	1
#define AD_SAMPLE_MAX	2000


#define MAXLEVEL	820
#define MINLEVEL	40
#define MAXTOPLEVEL	400
#define MAXBOTTOMLEVEL	400

#define  SamplePointTotal 30000		



/* ram define ------------------------------------------------------------------*/
uint16_t heartRate;   

uint16_t lastHeartRate = 60;

uint8_t  heartRateIndex  =0;


uint16_t adSmoothStack[AD_SMOOTH_MAX];

uint16_t heartRateStack[RATE_NUM_MAX];
uint16_t heartRateTemp[RATE_NUM_MAX];

uint16_t adValues[AD_SAMPLE_MAX];





int plusePeakLocStack[PLUSE_SAMPLE_MAX];
int plusePeakValStack[PLUSE_SAMPLE_MAX];


uint8_t wavePulseState;
int peakCount ;
int pulseIndex;
int peakStart;
int peakStop;	
int bottomStart;
int bottomStop;
int pointCount;	
int peakAve;
int currentPosition; 
int bottomValue;
uint16_t maxValue;
uint16_t lastAdData;

uint16_t minValue;
int firstWave;
int bottomIndex;
int  waveType;
int isTypeRect;
int isDetectStart;
WaveSLope slope;

int top;
int bottom;
int middle;
int totalPoint = 0;
int lastPulseLocal;
int topCount;
int bottomCount;
int topStart;
int slopeUp;
int slopeDown;



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
	wavePulseState = 0;
	peakCount= 0;
	pulseIndex = 0;
	currentPosition = 0;
	maxValue = 0;
	minValue = 1024;
	firstWave = 0;
	bottomValue = 0;
	waveType = 0;
	slope = SlopeBottom;
	top = 0;
	bottom = 0;
	middle = 0;
	isTypeRect = 0;
	topCount = 0;
	bottomCount = 0;
	topStart = 0;
	slopeUp=0;
	slopeDown=0;
	memset(plusePeakLocStack,0x00,PLUSE_SAMPLE_MAX);
	memset(plusePeakValStack,0x00,PLUSE_SAMPLE_MAX);
	
}

void heartRateStackInit(void){

	memset(heartRateStack,75,RATE_NUM_MAX);
	
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
	//tmp= adData-40;
	
	if(tmp>MAXLEVEL){
		return 2;
		}else {
		return 0;
	}	
}
#if 0
int pushPulsePeakStack(int pulsePeakLocation,int minValue){
	
	int posiLength =0;
	isTypeRect = 0;
	if(firstWave==0){
		APP_DBG((" first lastPulseLocal: %d \r\n",lastPulseLocal));
		lastPulseLocal = pulsePeakLocation;
		firstWave=1;
		}else {
		posiLength =  pulsePeakLocation-lastPulseLocal;
		lastPulseLocal = pulsePeakLocation;
		if(posiLength<1000&&posiLength>100){
			plusePeakLocStack[pulseIndex]= posiLength;
	    	pulseIndex++;
			}else {
				return -1;
				}
		APP_DBG(("next lastPulseLocal: %d \r\n",lastPulseLocal));
		}
	
	return posiLength;
}
#endif
int pushPulsePeakStack(int pulsePeakLocation,int minValue){
	
	
	if(pulseIndex<PLUSE_SAMPLE_MAX){
	plusePeakLocStack[pulseIndex]= pulsePeakLocation;
	plusePeakValStack[pulseIndex]= pulsePeakLocation;
	}
	pulseIndex++;
	if(pulseIndex>=PLUSE_SAMPLE_MAX){
		pulseIndex = PLUSE_SAMPLE_MAX-1;
		}
	
	
	return pulseIndex;
}


int regularPulseWave(){
	int i=0;
	int distance;
	int plusePeakTemp[PLUSE_SAMPLE_MAX];
	int plusePeakTempIndex = 0;
	//for(i=0;i<pulseIndex-1;i++){
		while (i<pulseIndex){
			
		distance = plusePeakLocStack[i+1]-plusePeakLocStack[i];
		APP_DBG(("distantce [%d] = %d\r\n",i,distance));
		if(distance<200){
			if((plusePeakValStack[i+1]-plusePeakValStack[i])>0){
				plusePeakTemp[plusePeakTempIndex++]=plusePeakLocStack[i+1];
				}else {
				plusePeakTemp[plusePeakTempIndex++]=plusePeakLocStack[i];
					}
			i+=2;
			}else {
			plusePeakTemp[plusePeakTempIndex++]=plusePeakLocStack[i];
			i++;
				}
			
		}
	for(i=0;i<plusePeakTempIndex;i++){
		APP_DBG(("plusePeakTemp[%d] = %d\r\n",i,plusePeakTemp[i]));
		}
	
	return pulseIndex;
}




#define DEBUG_MODE
HRState getHeartRateWave(){
	int peak;
	int i;
	uint16_t adData;
	uint16_t lastAdData=0;
	int lastAdPeak=0;
	int hasSlopeDown=0;
	int outBottom =0;
	int lastPosition = 0;
	int distance=0;
	int lastMinValue=0;
	
	WaveSLope currentDirect;
		// APP_DBG(("adData = %d\r\n",adData));
	pointCount=0;
	isTypeRect =0;
	currentPosition = 0;
	bottomStart = 0;
	bottom = 0;
	isDetectStart = 0;
	slope = SlopeBottom;
	pulseIndex = 0;
	
	 for (i = 0; i < AD_SMOOTH_MAX; i++){
  	adSmoothStack[i] = wave_2660_2[i];
  	}
    for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
	 adValues[i]=adValueFilter (wave_2660_2[i]);
	 }

	for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
	//	adData =	wave_2660_2[i];

//	for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
			adData =	adValues[i];



		//
	//	APP_DBG(("V:%d P:%d ",adData,pointCount));

	//APP_DBG(("%d,",adData));

		
		peak = isPeakBottom( adData);


		if(peak==2){					//  波峰平
			
			bottom=0;
			top++;
			if(top>MAXTOPLEVEL){
				return HRErrTopLong;
				}
			if(isTypeRect ){
				outBottom++;
				if(outBottom>10){
					currentPosition = bottomStart+bottom/2;
				APP_DBG(("TOP  lastPosition: %d \r\n",lastPosition));
					if(lastPosition>0){
					distance=	currentPosition-lastPosition;
					APP_DBG(("TOP  distance: %d heart: %d \r\n",distance,SamplePointTotal/distance));
						if(distance>100&&distance<750){
							heartRate = SamplePointTotal/distance;
							return HRFinish;
						}
					}
					lastPosition= currentPosition;
					isDetectStart =0;
					isTypeRect = 0;
					bottom = 0;
					}
				}
		hasSlopeDown = 1;


		slope = SlopeTop;
		currentDirect = SlopeTop;
		
		}else if(peak==1){				//波谷平
			bottom++;
			top =0;
			
			if(bottom>MAXBOTTOMLEVEL){
				return HRErrBottomLong;
				}
			if(hasSlopeDown==1){
				
				if(bottom>30){					// 方波判断
			//	APP_DBG(("start rec bottomStart: %d \r\n",pointCount));
					isTypeRect =1;					
					if(isDetectStart==0){
						isDetectStart =1;
						bottomStart = pointCount-30;
					    outBottom=0;
						APP_DBG(("\r\n start rec detect: %d \r\n",bottomStart));

					
					}
				}
			}

		slope = SlopeBottom;
	currentDirect = SlopeBottom;

			
		}else {							//中间值
	//	APP_DBG(("middle  : %d type: %d \r\n",pointCount,isTypeRect));	
		lastAdPeak = isPeakBottom( lastAdData);
		if(lastAdPeak==1){
			slopeUp++;
			slopeDown=0;
		}else if(lastAdPeak==2) {
			slopeDown++;
			slopeUp=0;
		}else {
			if(lastAdData<adData){
			slopeUp++;
			slopeDown=0;
			}else {
			slopeDown++;
			slopeUp=0;
			}	
		}
		if(slopeUp>15){					//上升沿	
			slope= SlopeUP;
			maxValue =0;
			if(currentDirect!=SlopeUP){
					currentDirect = SlopeUP;
					currentPosition = bottomStart;
				
					
					APP_DBG(("  bottom: %d  minValue: %d current : %d distance:%d \r\n",bottomStart,minValue,pointCount,distance));
					pushPulsePeakStack(currentPosition,minValue);
					if(pulseIndex>7){
						regularPulseWave();
						}
					lastPosition = currentPosition;
					lastMinValue = minValue;
				}
			
			}

		if(slopeDown>15){			//下降沿
			slope = SlopeDown;
			hasSlopeDown = 1;
	    	minValue =1024;
			if(currentDirect!=SlopeDown){
					currentDirect = SlopeDown;
				
			
					//APP_DBG(("sloop down  top: %d  maxValue: %d current : %d\r\n",topStart,maxValue,pointCount));
				}
		
			
			
		//	APP_DBG(("sloop down  point: %d \r\n",pointCount));
			}
			
		if(isTypeRect ){
				
			outBottom++;
			if(outBottom>10){
				currentPosition = bottomStart+bottom/2;
		
				if(lastPosition>0){
				distance=	currentPosition-lastPosition;
				APP_DBG(("distance: %d heart: %d \r\n",distance,SamplePointTotal/distance));
					if(distance>100&&distance<750){
						heartRate = SamplePointTotal/distance;
						return HRFinish;
					}
				}
				APP_DBG(("lastPosition: %d cur: %d \r\n",lastPosition,currentPosition));
				lastPosition= currentPosition;
				isDetectStart=0;
				isTypeRect = 0;
				bottom = 0;
				}
			}else {			
			// sine wave detection
		//	APP_DBG(("NORMAL DETECT: %d Posi: %d\r\n",adData,pointCount));
			
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
				topStart = pointCount;
				}
			}
			
		}





	//	top =0;
	//	bottom=0;
		
			
		}
		
	lastAdData = adData;
		
	pointCount++;
	
  }
  //APP_DBG(("size = %d \r\n ",sizeof(wave_2660_2)/sizeof(uint16_t)));	
	//APP_DBG(("T:%d B:%d M:%d \r\n",top,bottom,middle));

	


	return HRErrPointOut;

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
	for(i=0;i<length;i++){
		total +=array[i];
		}
	return total/length;
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



uint16_t getHeartAverageWithoutPeak(uint16_t *array,int length){
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
	
	//heartRate =	getHeartRateDetection();
//	heartRate =	getHeartRateSmooth(heartRate); 
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
	adValues[totalPoint++]=adc;
	//heartDealLoop(adc);
	
//	debubGpioChange();

	

}

void hw_timer1_handler(void)
{
	if(totalPoint<AD_SAMPLE_MAX){
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
	hw_timer_init(TIME1, TIMER_CLK_8M, 2, TIMER_CHANNEL_0, hw_timer1_handler);

	APP_DBG(("hw_timer_config done!\r\n"));
}

void bsp_ts1303_init(void)
{
	gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
    adc_init(7);
	
	gpio_init_output(GPIO_21,GPIO_PULL_NONE,1);
	//hw_timer_start(TIME1);
	heartRateStackInit();
	heartDealLoop();
//	heartDealLoop();
}


