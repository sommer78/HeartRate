/**
****************************************************************************
* @file    mxd2660_adc.h
* @brief   
* @author  xiaoyu
* @version V0.1.1
* @date    2017/21/01
* @history 
* @note    
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/

#ifndef __ADC_H__
#define __ADC_H__

typedef enum _ADC_CH
{
    ADC_CH0 = (1 << 14),   //P2
    ADC_CH1 = (1 << 13),   //P3
    ADC_CH2 = (1 << 12),   //P4
    ADC_CH3 = (1 << 11),   //P5
    ADC_CH4 = (1 << 10),   //P6
    ADC_CH5 = (1 << 9),     //P7
    ADC_CH6 = (1 << 8)      //P8
}ADC_CH;


/**
* @brief  init adc,default work in 8MHz CLK
* @param  clk_sel 0:8M 1:4M ... N:8M/2^n
* @return none
*/
void adc_init(uint8 clk_sel);

/**
* @brief  disable ADC module
* @param  none
* @return none
*/
void adc_dis(void);

/**
* @brief  read Vbat channel voltage
* @param  none
* @return Vbat voltage
*/
uint16 adc_vbat_read(void);

/**
* @brief  read Vbat channel voltage
* @param  ch channel sel,
* @return io input voltage 
*/
uint16 adc_io_read(ADC_CH ch);

/**
* @brief    read chip tempreture
* @param None
* @return  tempreture(bit8 is symbol bit)
*/
uint16 mxd_read_tempreture(void);


#endif

