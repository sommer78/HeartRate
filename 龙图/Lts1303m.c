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
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

//#include "system_config.h"
//#include "mxd2660_adc.h"
//#include "mxd2660_gpio.h"
//#include "mxd2660_timer.h"
//#include "utility.h"
//#include "sample.h"

#include "ap_value.h"


/* const define ------------------------------------------------------------------*/



#define RATE_NUM_MAX                        16
#define PLUSE_SAMPLE_MAX					16 //32

#define AD_SMOOTH_MAX					8     //32

//#define ADSMOOTH	1
#define AD_SAMPLE_MAX	200//1000


#define MAXLEVEL	3200//820
#define MINLEVEL	100//40
#define MAXTOPLEVEL	25//100
#define MAXBOTTOMLEVEL	25//100

#define  SamplePointTotal 1200		



/* ram define ------------------------------------------------------------------*/
u16 heartRate;

u16 lastHeartRate = 75;

u8  heartRateIndex  =0;


u16 adSmoothStack[AD_SMOOTH_MAX];

u16 heartRateStack[RATE_NUM_MAX];


u16 adValues[AD_SAMPLE_MAX];


int plusePeakLocStack[PLUSE_SAMPLE_MAX];



int topStart;
int slopeUp;
int slopeDown;
u16 maxValue;
u16 minValue;
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
#if 0
#ifdef debug_gpio
		if(gpio_read_output_bit(GPIO_19)){
				gpio_clear_bit(GPIO_19);
			}else {
				gpio_set_bit(GPIO_19);
			}
	
#endif
#endif
}



/* subroutine define ------------------------------------------------------------------*/

void heartRateInit(void){
	//APP_DBG((" init \r\n"));

	pointCount = 0;
//	wavePulseState = 0;
//	peakCount= 0;
	pulseIndex = 0;
	currentPosition = 0;
	maxValue = MINLEVEL;
	minValue = MAXLEVEL;
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


int isSamliarValue(u16 first ,u16 second){
	
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


int isHeartRateValid(u16 heartRate){
	if(heartRate>HeartRateMAX){
		//APP_DBG(("HeartRateMAX = %d\r\n",heartRate));
		return 1;
		}
	if(heartRate<HeartRateMIN){
		//APP_DBG(("HeartRateMIN = %d\r\n",heartRate));
		return 2;
		}
	return 0;
}

/**
  *	1: bottom 2 peak 0 other
 **/
int isPeakBottom(u16 adData){
	
	u16 tmp;
	
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
int getArrayMaxIndex(u16 *array,int len){
	int i,iMax;

    for(i=iMax=0;i<len;i++){
        if(array[iMax]<array[i]){
			  iMax=i;
        	}
          
    	}
    return iMax;
}

int getArrayMinIndex(u16 *array,int len){
	int i,iMin;

    for(i=iMin=0;i<len;i++)
        if(array[iMin]>array[i])
            iMin=i;
  
    return iMin;
}

u16 getArrayAverage(u16 *array,int length){
	int i = 0;
	u32 total = 0;
	u16 aveH;

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


u16 getHeartRateDetection(void){
	int i = 0;
	u16 tmpHeartRate;
	int count = 0;
	u16 heartRateTemp[RATE_NUM_MAX];
	for(i=0;i<pulseIndex-1;i++){
	//	APP_DBG(("getHeartRateDetection[%d] = %d  \r\n ",i,plusePeakLocStack[i+1]-plusePeakLocStack[i]));	
		tmpHeartRate = SamplePointTotal/ (plusePeakLocStack[i+1]-plusePeakLocStack[i]);
		// APP_DBG((" heart [%d]= %d\r\n",i,tmpHeartRate ));
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

	u16 adData;
	u16 lastAdData=0;

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

		if(slopeUp>5){					//上升沿
			slope= SlopeUP;
			maxValue =0;
		   
			if(currentDirect!=SlopeUP){
					currentDirect = SlopeUP;
					if(isTypeRect==1){
						currentPosition = bottomStart+bottom/2;
						//APP_DBG((" [rect]  start:%d  len:%d position:%d \r\n",bottomStart,bottom/2,currentPosition));
						isTypeRect = 0;
						}else {
						//APP_DBG((" [normal] position:%d \r\n",currentPosition));
						currentPosition = bottomStart;
							}
					distance = currentPosition-lastPosition;
					
					//APP_DBG(("  bottom: %d  minValue: %d current : %d distance:%d \r\n",bottomStart,minValue,pointCount,distance));
					
					if(pulseIndex==0){
						//APP_DBG((" push first %d \r\n" ,currentPosition));
						pushPulsePeakStack(currentPosition);
						}else {
							if(distance>8){							//AD_SAMPLE_MAX /HeartRateMAX
								//APP_DBG((" push  %d \r\n" ,currentPosition));
								
							pushPulsePeakStack(currentPosition);
							}else {
								if((lastMinValue-minValue)>0){
								//APP_DBG((" update  %d \r\n" ,currentPosition));
									updatePulsePeakStack(currentPosition);
									}else {
										}
								}
						}
					
					
					
					if(pulseIndex>10){
					//	regularPulseWave();
						}
				
					//lastPosition = currentPosition;
						//APP_DBG(("F  lastPosition :%d \r\n",lastPosition));
					lastMinValue = minValue;
				}
			
			}

		if(slopeDown>5){			//下降沿
			slope = SlopeDown;
		//	hasSlopeDown = 1;
	    	minValue = MAXLEVEL;
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

u16 adValueFilter(u16 adValue){
	int i = 0;
	u16 adAvergerValue;
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



u16 getArrayAverageWithoutPeak(u16 *array,int length){
	int i = 0;
	int total = 0;
	int maxIndex,minIndex;
	u16 ave;
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

u16 getHeartRateSmooth(u16 tmpHeartRate){
	int i = 0;
	//APP_DBG(("original heart = %d\r\n",tmpHeartRate));

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
	//APP_DBG((" smooth heart  = %d\r\n",heartRate ));
	
	return heartRate;
}

#if 0
void heartDealSimLoop(void){

	 u16 i;
	 HRState bpmState;

	 
	 //APP_DBG(("\r\n***********************heartDealSimLoop*********************** \r\n "));

   for(i=0;i<AD_SAMPLE_MAX;i++){
   	adValues[i] = wave_2660_0[i];
   	}

	/* 婊ゆ尝鍑芥暟 寰楀埌鍧囧�兼护娉㈢殑鍊�*/
  for (i = 0; i < AD_SMOOTH_MAX; i++){
  	adSmoothStack[i] = adValues[i];
  	}
  for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
	 adValues[i]=adValueFilter (adValues[i]);
	 }


  	/* 蹇冭烦澶勭悊 寰楀埌蹇冭烦闂撮殧鍛ㄦ湡  */
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
#endif

void heartDealLoop(void){
	 u16 i;
	 HRState bpmState;
	 	
//	 size =  sizeof(wave_2660_2)/sizeof(uint16_t);
//	  APP_DBG(("size = %d \r\n ",size));	
 // APP_DBG(("\r\n***********************heartdealloop*********************** \r\n "));

	 /* 滤波函数 得到均值滤波的值*/
  for (i = 0; i < AD_SMOOTH_MAX; i++){
  	adSmoothStack[i] = adValues[i];
  	}
  for (i = AD_SMOOTH_MAX; i < AD_SAMPLE_MAX; i++){
	 adValues[i]=adValueFilter (adValues[i]);
	 }


	/* 心跳处理 得到心跳间隔周期  */
	bpmState = 	 getHeartRateWave();  

	//wang_test32_2=bpmState;
	if(bpmState==HRFinish){
	

	
	heartRate =	getHeartRateSmooth(heartRate); 
	// APP_DBG(("heartRate = %d\r\n",heartRate));
	//wang_test32_2=heartRate;
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
		// APP_DBG(("HRError:%d \r\n", bpmState));
		heartRateInit();
		}

   totalPoint= 0;
	// APP_DBG(("\r\n***********************finish*********************** \r\n "));
	//  APP_DBG(("top  = %d bottom=%d middle=%d  \r\n",top,bottom,middle));		
}

void adc_read_callback(void)
{

	u16		adc=0;
 
	 portest_adc=bat_adc_cal();
	 adc=portest_adc*49/10;

	 wang_test32_1=adc;

//	need adc=adc_io_read(ADC_CH1);
	//APP_DBG(("%d,",adc));
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
#if 0
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
#endif
}

void bsp_ts1303_init(void)
{
//	gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
//    adc_init(7);
	
//	gpio_init_output(GPIO_21,GPIO_PULL_NONE,1);
//	hw_timer_start(TIME1);
	heartRateStackInit();


//	heartDealSimLoop();

}


