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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include <string.h>
#include "Lts1303m.h"
#include "sample.h"


#define UART_TX_BUF_SIZE 256 /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1   /**< UART RX buffer size. */

#ifndef NRF_APP_PRIORITY_HIGH
#define NRF_APP_PRIORITY_HIGH 1
#endif



#define TEST_PORT 27
#define SAMPLES_IN_BUFFER 1
volatile uint8_t state = 1;

static const nrf_drv_timer_t   m_timer = NRF_DRV_TIMER_INSTANCE(0);
static nrf_saadc_value_t       m_buffer_pool[2][SAMPLES_IN_BUFFER];
static nrf_ppi_channel_t       m_ppi_channel;
static uint32_t                m_adc_evt_counter;
//static uint16_t                adc_datas[4000];
//static uint32_t  j=0;

const HEART_RATE_PARAM_T HeartRateParamInit = 
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
        UART_BAUDRATE_BAUDRATE_Baud460800
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_events_handler,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}

void timer_handler(nrf_timer_event_t event_type, void* p_context)
{

}

void saadc_sampling_event_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_timer_init(&m_timer, NULL, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event every 400ms */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 2);
	//  printf("ticks = %d\r\n",ticks);
	//  ticks = 6400;
    nrf_drv_timer_extended_compare(&m_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_event_addr = nrf_drv_saadc_task_address_get(NRF_SAADC_TASK_SAMPLE);

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);
    
    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel, timer_compare_event_addr, saadc_sample_event_addr);
    APP_ERROR_CHECK(err_code);
}

void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);
}

void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
 HRState bpmState;
		int sample;
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
     
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
	//	nrf_gpio_pin_toggle(27);
        int i;
     //   printf("ADC event number: %d\r\n",(int)m_adc_evt_counter);
				sample = 0;
        for (i = 0; i < SAMPLES_IN_BUFFER; i++)
        {
        //  printf("%d\r\n", p_event->data.done.p_buffer[i]);
				//	adc_datas[i] = p_event->data.done.p_buffer[i];
					sample +=p_event->data.done.p_buffer[i];
        }
		sample = sample/SAMPLES_IN_BUFFER;
		if(sample<0){
		  	sample = 0;
		  }
		
		// printf("s[%d] = %d ",pointCount,sample);		  
		 bpmState = 	 getHeartRateWaves(sample);  
		
		if(bpmState==HRFinish){
			
		heartRate =	getHeartRateFilter();
		 printf("heartRate = %d\r\n",heartRate);		
		heartRateInit();
		
		//
		}
		if(bpmState==HRPointMax){
			 printf("HRPointMax = %d\r\n",heartRate);		
		heartRateInit();
			}
		if(bpmState==HRLineOut){
			if(smoothValue<50){
				 printf("leave low !\r\n");	
				}else if(smoothValue>1000){
				 printf("leave high !\r\n");	
					}else {
					 printf("leave error !\r\n");	
						}
		
		heartRateInit();
			}
        m_adc_evt_counter++;
			
    }
}

void saadc_init(void)
{
    ret_code_t err_code;
	nrf_drv_saadc_config_t saadc_config ;
	
    nrf_saadc_channel_config_t channel_config =
            NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);

	channel_config.gain = NRF_SAADC_GAIN1_4;
	channel_config.acq_time = NRF_SAADC_ACQTIME_40US;

	saadc_config.interrupt_priority =SAADC_CONFIG_IRQ_PRIORITY;
	saadc_config.oversample =SAADC_CONFIG_OVERSAMPLE;
	saadc_config.resolution= 	NRF_SAADC_RESOLUTION_10BIT;
		
   // err_code = nrf_drv_saadc_init(NULL, saadc_callback);
	err_code = nrf_drv_saadc_init(&saadc_config, saadc_callback);
	
    APP_ERROR_CHECK(err_code);



    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0],SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
    
  //  err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1],SAMPLES_IN_BUFFER);
  //  APP_ERROR_CHECK(err_code);
	nrf_gpio_cfg_output(TEST_PORT);
  	nrf_gpio_pin_clear(TEST_PORT);
}




/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uart_config();

    printf("\n\rlts1303m Heart Rate  example.\r\n");

	heartRateParamSetup(HeartRateParamInit);
	clrHeartRateStack();
	heartRateInit();
    saadc_sampling_event_init();
    saadc_init();
    saadc_sampling_event_enable();
	
	
//	heartRateSubroutine();
	
    while(1)
    {
        __WFE();
    }
}


/** @} */
