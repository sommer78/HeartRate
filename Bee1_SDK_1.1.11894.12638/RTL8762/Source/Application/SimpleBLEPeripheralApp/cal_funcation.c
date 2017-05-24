/*
*********************************************************************************************************
*               Copyright(c) 2017, ETEK Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      cal_funcation.C
* @brief      file of cal_funcation
* @details   
* @author    jiangs
* @date      2017-05-12
* @version   v1.0
* *********************************************************************************************************
*/



/* Includes ------------------------------------------------------------------*/
#include "cal_funcation.h"
#include<stdio.h>
#include<string.h>

/* const define ------------------------------------------------------------------*/


/* ram define ------------------------------------------------------------------*/





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
  * @brief  比较前后两值大小，当前面大于后面 返回1 否则返回0
  * @param uint16_t before,uint16_t after
  * @retval type
  */
uint8_t getSlopeType(uint16_t before,uint16_t after){
	
	if(before>after){
		return 1;
		}else {
		return 0;
		}	
}


/* hex to Ascii*/
uint8_t HexToAscii(uint8_t hex)
{
    if(hex <= 9)
    {
        return (hex - 0x00 + 0x30);
    }
    else if(hex >= 0x0A && hex <= 0x0F)
    {
        return (hex - 0x0A + 0x41);
    }
    else
    {
        /* wrong */
        return 0xFF;
    }
}



#endif
