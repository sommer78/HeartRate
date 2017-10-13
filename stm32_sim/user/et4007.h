/***************************************************************************************************
	
***************************************************************************************************/

#ifndef _ET4007_H_
#define _ET4007_H_
#include "stm32f10x.h"
#include "bsp_gpio.h"   


/***************************************************************************************************
//#define   
***************************************************************************************************/

#define PIN_SCL   GPIO_Pin_7  
#define PIN_SDA   GPIO_Pin_6
#define PIN_BUSY   GPIO_Pin_5

#define ET4007_SET_SDA_HIGH GPIO_SetBits(GPIOA,PIN_SDA)
#define ET4007_SET_SDA_LOW GPIO_ResetBits(GPIOA,PIN_SDA)
#define ET4007_SET_SCL_HIGH GPIO_SetBits(GPIOA,PIN_SCL)
#define ET4007_SET_SCL_LOW GPIO_ResetBits(GPIOA,PIN_SCL)
#define ET4007_GET_SDA_STATE  GPIOA->IDR  & PIN_SDA
#define ET4007_GET_BUSY_STATE  GPIOA->IDR  & PIN_BUSY
/***************************************************************************************************
//#STRUCT
***************************************************************************************************/  


/***************************************************************************************************
//#PROGRAM define
***************************************************************************************************/
void send_normal_remote(void);
void ET4007_GPIO_Config( void );
void ET_IR_learn_start(void);
 int ET_IR_read_device_info(char ir_version[]);
 
int  ET_IR_read_learn_data(char *buf_ir_test);

int et4007_Learndata_UnCompress(char *datas, int *code_buffer, int *freq);
#endif
