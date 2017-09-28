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
#define WAVE_INDEX_MAX					16

#define AD_SMOOTH_MAX					8

#define ADSMOOTH	1


#define MAXLEVEL	840
#define MINLEVEL	40




/* ram define ------------------------------------------------------------------*/
uint16_t heartRate;   

uint16_t lastHeartRate = 60;

uint8_t  heartRateIndex  =0;


uint16_t adSmoothStack[AD_SMOOTH_MAX];



uint16_t heartRateStack[RATE_NUM_MAX];
uint16_t heartRateTemp[RATE_NUM_MAX];



uint16_t peakIndexs[PLUSE_SAMPLE_MAX];


uint8_t wavePulseState;
int peakCount ;
int pulseIndex;
int peakStart;
int peakStop;	
int bottomStart;
int bottomStop;
int pointCount;	
int peakAve;
int pulsePeakIndex; 
int bottomValue;
uint16_t maxValue;
uint16_t lastAdData;

uint16_t minValue;
int firstWave;
int bottomIndex;
int  waveType;
int slope;

int top;
int bottom;
int middle;


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
	maxValue = 0;
	minValue = 1024;
	firstWave = 0;
	bottomValue = 0;
	waveType = 0;
	slope = 0;
	top = 0;
	bottom = 0;
	middle = 0;
	memset(adSmoothStack,0x00,AD_SMOOTH_MAX);

}



/**
  *	1: bottom 2 peak 0 other
 **/
int isPeakBottom(uint16_t adData){
	
	uint16_t tmp;
	
	tmp = adData;
	if(tmp<40){
		return 1;
		}
	//tmp= adData-40;
	
	if(tmp>840){
		return 2;
		}else {
		return 0;
		}	
}

HRState getHeartRateWaveState(uint16_t adData){
	HRState state;
	int peak;
	state = HRInit;
		// APP_DBG(("adData = %d\r\n",adData));
		
	peak = isPeakBottom( adData);
	adData =	adValueFilter(adData);
	APP_DBG(("%d,",adData));
		pointCount++;

		
	if(peak==1){			// bottom value
	
	//	APP_DBG(("peak = %d\r\n",peak));

		if(bottomIndex==0){
		bottomValue=1;
		bottomIndex=1;
		}else {
		bottomValue++;
		if(bottomValue>10){
			waveType =1;
			}
		}
	bottom++;	
	}else if(peak ==2){							// peak value
	top++;
	//	APP_DBG(("peak = %d\r\n",peak));
		
		
	}else if(peak ==0){					// middle value
	middle++;
		if(adData<minValue){
			minValue = adData;
			if(firstWave==0){
			//	APP_DBG(("count = %d\r\n",pointCount));
			//	pointCount = 0;
				}
			
			
			}
		
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
	int total = 0;
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
		//nrf_delay_ms(100);
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
	 int size ;
	 heartRateInit();
	 size =  sizeof(wave_2660_1)/sizeof(uint16_t);
	  APP_DBG(("size = %d \r\n ",size));		  
	  for (i = 0; i < size; i++){
       
		
		// APP_DBG(("s[%d] = %d \r\n ",i,wave_2660[i]));		  
		 bpmState = 	 getHeartRateWaveState(wave_2660_1[i]);  
		
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
	  	}
	//  APP_DBG(("top  = %d bottom=%d middle=%d  \r\n",top,bottom,middle));		
}
//#define debug_gpio
void adc_read_callback(void)
{
//	HRState 	bpmState;
	uint16		adc=0;
 
	adc=adc_io_read(ADC_CH1);
	heartDealLoop(adc);
	//APP_DBG(("%d,",adc));
#ifdef debug_gpio	
	if(gpio_read_output_bit(GPIO_19)){
			gpio_clear_bit(GPIO_19);
		}else {
			gpio_set_bit(GPIO_19);
		}

#endif
	

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
}


