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
#define PLUSE_SAMPLE_MAX					8

#define AD_SMOOTH_MAX					8

#define ADSMOOTH	1


#define MAXLEVEL	820
#define MINLEVEL	40
#define MAXTOPLEVEL	1000
#define MAXBOTTOMLEVEL	1000




/* ram define ------------------------------------------------------------------*/
uint16_t heartRate;   

uint16_t lastHeartRate = 60;

uint8_t  heartRateIndex  =0;


uint16_t adSmoothStack[AD_SMOOTH_MAX];

uint16_t heartRateStack[RATE_NUM_MAX];
uint16_t heartRateTemp[RATE_NUM_MAX];



int plusePeakStack[PLUSE_SAMPLE_MAX*2];


uint8_t wavePulseState;
int peakCount ;
int pulseIndex;
int peakStart;
int peakStop;	
int bottomStart;
int bottomStop;
int pointCount;	
int peakAve;
int pulsePeakLocal; 
int bottomValue;
uint16_t maxValue;
uint16_t lastAdData;

uint16_t minValue;
int firstWave;
int bottomIndex;
int  waveType;
int isTypeRect;
int isTypeSine;
int isDetectStart;
int slope;

int top;
int bottom;
int middle;
int totolPoint = 0;
int lastPulseLocal;

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
	pulsePeakLocal = 0;
	maxValue = 0;
	minValue = 1024;
	firstWave = 0;
	bottomValue = 0;
	waveType = 0;
	slope = 0;
	top = 0;
	bottom = 0;
	middle = 0;
	isTypeRect = 0;
	isTypeSine = 0;
	memset(plusePeakStack,0x00,PLUSE_SAMPLE_MAX);
	memset(adSmoothStack,0x00,AD_SMOOTH_MAX*2);

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

int pushPulsePeakStack(int pulsePeakLocation){
	
	int posiLength =0;
	isTypeRect = 0;
	if(firstWave==0){
		lastPulseLocal = pulsePeakLocation;
		firstWave=1;
		}else {
		posiLength =  pulsePeakLocation-lastPulseLocal;
		lastPulseLocal = pulsePeakLocation;
		if(posiLength<1000&&posiLength>100){
			plusePeakStack[pulseIndex]= posiLength;
	    	pulseIndex++;
			}
		
		}
	
	return posiLength;
}

HRState getHeartRateWaveState(uint16_t adData){
	HRState state= HRInit;
	int peak;
	int posiLength;
		// APP_DBG(("adData = %d\r\n",adData));
	adData =	adValueFilter(adData);	
	peak = isPeakBottom( adData);
	
	//APP_DBG((" P:%d ,",adData));
	pointCount++;
	
		
	if(peak==1){			// bottom value
	//APP_DBG(("bottom: %d \r\n",bottom));
	//	APP_DBG(("peak = %d\r\n",peak));
		top=0;
		bottom++;

		
		if(bottom>10){
			isTypeRect =1;
			if(isDetectStart==0){
				isDetectStart =1;
				APP_DBG(("start rec detect: %d \r\n",totolPoint));
				if(pulseIndex==0){
					pointCount=0;
					bottomStart = 0;
	
					}else {
						bottomStart = pointCount;
						}
				}
			
			}
	if(bottom>MAXBOTTOMLEVEL){
	//	APP_DBG(("HRErrBottomLong : %d \r\n",totolPoint));
		bottom=0;
		state = HRErrBottomLong;
		}	
	}else if(peak ==2){							// peak value
	//APP_DBG(("top: %d \r\n",top));
	top++;
		
	if(isTypeRect){
		if(isDetectStart){
			isDetectStart=0;
			pulsePeakLocal= bottomStart+bottom/2;
		    isTypeRect = 0;
			posiLength = pushPulsePeakStack(pulsePeakLocal);
			APP_DBG(("pulse   pulsePeakLocal: %d posiLength : %d \r\n",pulsePeakLocal,posiLength));
			if(pulseIndex>PLUSE_SAMPLE_MAX){
			state = HRFinish;	
				}
			
			//APP_DBG(("finish rec detect: %d  \r\n",totolPoint));
			}
		
		}
	//	APP_DBG(("peak = %d\r\n",peak));
	bottom =0;
	if(top>MAXTOPLEVEL){
		top = 0;
		//APP_DBG(("HRErrTopLong : %d\r\n",totolPoint));
		state = HRErrTopLong;
		}
		
	}else if(peak ==0){					// middle value
	
		if(adData<minValue){
			minValue = adData;
			if(firstWave==0){
			//	APP_DBG(("count = %d\r\n",pointCount));
			//	pointCount = 0;
				}
			
			
			}
		
		}

	if(pointCount>60000){
		pointCount = 0;
		state = HRErrPointOut;
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

uint16_t getHeartRateDetection(void){
	int i = 0;
	uint16_t tmpHeartRate;
	int count = 0;
	for(i=0;i<PLUSE_SAMPLE_MAX;i++){
	
		tmpHeartRate = SamplePointTotal/ plusePeakStack[i];
		 APP_DBG((" heart [%d]= %d\r\n",i,tmpHeartRate ));

		if(tmpHeartRate<=HeartRateMAX&&tmpHeartRate>=HeartRateMIN){
			heartRateTemp[count++]=tmpHeartRate;
		
			}
	}
	if(count!=0){
	tmpHeartRate =	getHeartAverageWithoutPeak(heartRateTemp,count);
	if(tmpHeartRate>HeartRateMAX||tmpHeartRate<HeartRateMIN){
		tmpHeartRate = lastHeartRate;
		lastHeartRate=60;
		}
	}else {
	tmpHeartRate = lastHeartRate;
	}
	
	APP_DBG(("  before heart = %d\r\n",tmpHeartRate ));
	heartRate = tmpHeartRate;
	
	
	 

	return heartRate;
}
#if 0
uint16_t getHeartRateDetection(void){
	int i = 0;
	uint16_t tmpHeartRate;
	int count = 0;
	for(i=0;i<PLUSE_SAMPLE_MAX-1;i++){
		APP_DBG(("getHeartRateDetection[%d] = %d  \r\n ",i,plusePeakStack[i+1]-plusePeakStack[i]));	
		tmpHeartRate = SamplePointTotal/ (plusePeakStack[i+1]-plusePeakStack[i]);
		 APP_DBG((" heart [%d]= %d\r\n",i,tmpHeartRate ));

		if(tmpHeartRate<=HeartRateMAX&&tmpHeartRate>=HeartRateMIN){
			heartRateTemp[count++]=tmpHeartRate;
		
			}
	}
	if(count!=0){
	tmpHeartRate =	getHeartAverageWithoutPeak(heartRateTemp,count);
	if(tmpHeartRate>HeartRateMAX||tmpHeartRate<HeartRateMIN){
		tmpHeartRate = lastHeartRate;
		lastHeartRate=60;
		}
	}else {
	tmpHeartRate = lastHeartRate;
	}
	
	APP_DBG(("  before heart = %d\r\n",tmpHeartRate ));
	heartRate = tmpHeartRate;
	
	
	 

	return heartRate;
}
#endif 


void heartDealLoop(uint16_t adc){
	 uint16_t i;
	 HRState bpmState;
	 int size ;
	
//	 size =  sizeof(wave_2660_2)/sizeof(uint16_t);
//	  APP_DBG(("size = %d \r\n ",size));		  
//	  for (i = 0; i < size; i++){
       
		totolPoint++;
		// APP_DBG(("s[%d] = %d \r\n ",i,wave_2660[i]));		  
		// bpmState = 	 getHeartRateWaveState(wave_2660_2[i]);  
		 bpmState = 	 getHeartRateWaveState(adc);  
	//	 APP_DBG(("%d,",wave_2660_2[i]));		  
		if(bpmState==HRFinish){
		
		heartRate =	getHeartRateDetection();
	//	heartRate =	getHeartRateSmooth(heartRate); 
		 APP_DBG(("heartRate = %d\r\n",heartRate));		
		heartRateInit();
		
		//
		}
		
		if(bpmState==HRError||bpmState==HRErrShortWave||bpmState==HRErrLongWave){
			 APP_DBG(("HRError:%d \r\n", bpmState));		
			heartRateInit();
			}
		if(bpmState==HRErrBottomLong||bpmState==HRErrTopLong||bpmState==HRErrPointOut){
			 APP_DBG(("HRError:%d \r\n", bpmState));		
			heartRateInit();
			}
//	  	}
	//  APP_DBG(("top  = %d bottom=%d middle=%d  \r\n",top,bottom,middle));		
}

void adc_read_callback(void)
{

	uint16		adc=0;
 
	adc=adc_io_read(ADC_CH1);
//	APP_DBG(("%d,",adc));
	heartDealLoop(adc);
	
	debubGpioChange();

	

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
	hw_timer_init(TIME1, TIMER_CLK_8M, 2, TIMER_CHANNEL_0, hw_timer1_handler);

	APP_DBG(("hw_timer_config done!\r\n"));
}

void bsp_ts1303_init(void)
{
	gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
    adc_init(7);
	
	gpio_init_output(GPIO_21,GPIO_PULL_NONE,1);
	hw_timer_start(TIME1);
	heartRateInit();
	//heartDealLoop(0);
}


