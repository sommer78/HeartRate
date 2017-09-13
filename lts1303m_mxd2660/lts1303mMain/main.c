/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @defgroup nrf_adc_example main.c
 * @{
 * @ingroup nrf_adc_example
 * @brief ADC Example Application main file.
 *
 * This file contains the source code for a sample application using ADC.
 *
 * @image html example_board_setup_a.jpg "Use board setup A for this example."
 */

//#include <stdbool.h>
//#include <stdint.h>
//#include <stdio.h>
#include "nrf.h"

#include "boards.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
//#include "app_util_platform.h"
#include <string.h>
#include "Lts1303m.h"
#include "sample.h"
#include <stdlib.h>

#define UART_TX_BUF_SIZE 256 /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1   /**< UART RX buffer size. */

#ifndef NRF_APP_PRIORITY_HIGH
#define NRF_APP_PRIORITY_HIGH 1
#endif







/**
 * @brief UART events handler.
 */
void uart_events_handler(app_uart_evt_t * p_event)
{
}

/**
 * @brief UART initialization.
 */
void uart_config(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_events_handler,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}




void heartDealLoop(void){
	 uint16_t i;
	 HRState bpmState;
	 int size ;
	 heartRateInit();
	 size =  sizeof(wave_2660)/sizeof(uint16_t);
	//  printf("size = %d \r\n ",size);		  
	  for (i = 0; i < size; i++){
       
		
		// printf("s[%d] = %d \r\n ",i,wave_2660[i]);		  
		 bpmState = 	 getHeartRateWaveInfo(wave_2660[i],i);  
		
		if(bpmState==HRFinish){
		
		heartRate =	getHeartRateDetection();
		heartRate =	getHeartRateSmooth(heartRate); //动态数据模拟 可以暂时不用
		 printf("heartRate = %d\r\n",heartRate);		
		heartRateInit();
		
		//
		}
		
		if(bpmState==HRError){
			 printf("HRError \r\n");		
			heartRateInit();
			}
		 nrf_delay_ms(1);
	  	}
	 
}


const uint16_t hearts[]={62,77,48,99,58,88,76,99,101,132,133,135,145,77,66,88,99,45,74,84};

void testSmoothValue(){
	uint16_t i;

	 int size ;
	 heartRateInit();
	 size =  sizeof(hearts)/sizeof(uint16_t);
	//  printf("size = %d \r\n ",size);		  
	  for (i = 0; i < size; i++){
       
		
	 printf("h[%d] = %d \r\n ",i,hearts[i]);	
		heartRate =	getHeartRateSmooth(hearts[i]); 
		
		 nrf_delay_ms(100);
	  	}
}


/**
 * @brief Function for main application entry.
 */

int main(void)
{
	

    uart_config();
	printf("********************************************************\r\n");
    printf("               lts1303m Heart Rate  example.            \r\n");
	printf("********************************************************\r\n");
	
	  heartDealLoop();
	//testSmoothValue();

    while(1)
    {
  
	

		
    }
}


/** @} */
