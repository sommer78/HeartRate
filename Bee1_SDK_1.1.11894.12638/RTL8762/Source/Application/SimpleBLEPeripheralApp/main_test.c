

/* Includes -----------------------------------------------------------------*/
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "portmacro.h"

#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_adc.h"
#include "freeRTOS.h"
#include "timers.h"
#include "string.h"
#include "trace.h"
#include "board.h"
#include <string.h>
#include "stdio.h"


#include "Lts1303m.h"
#include "rtl_delay.h"
#include "rtl876x_flash_storage.h"

/* Defines ------------------------------------------------------------------*/
#define Output_Pin            P2_7
#define HR_ADC							P2_6
#define GPIO_Output_Pin       GPIO_GetPin(Output_Pin)
uint16_t testadcbuf[256];
uint8_t StrBuffer[16];
int count;

#define hrTimerHandle									Timer4IntrHandler
uint16_t i;
uint8_t len;
uint8_t j;

uint8_t adFinish;




#include "rtl876x_adc.h"



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




/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void Board_GPIO_Init(void)
{
	Pad_Config(Output_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pinmux_Config(Output_Pin, GPIO_FUN);
	 /* ADC pad config */
  //  Pad_Config(HR_ADC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

/**
  * @brief  Initialize GPIO peripheral.
  * @param   No parameter.
  * @return  void
  */
void Driver_GPIO_Init(void)
{
	/* Enable GPIO clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	/* Initialize GPIO */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Output_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
}

/**
  * @brief  common operation of GPIO output mode.
  * @param   No parameter.
  * @return  void
  */
void GPIO_DemoCode(void)
{
	/* Output high */
	GPIO_SetBits(GPIO_Output_Pin);
	
	/* Output low */
	GPIO_ResetBits(GPIO_Output_Pin);

	/* Level inversion */
//	GPIO_WriteBit(GPIO_Output_Pin, (BitAction)(1-GPIO_ReadOutputDataBit(GPIO_Output_Pin)));
	GPIO_WriteBit(GPIO_Output_Pin, (BitAction)(1));
}



void test_hr_Init(void)
{
  

//    Pad_Config(BATTERY_CHECK, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

	/* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

    Pad_Config(HR_ADC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

 	NVIC_InitTypeDef NVIC_InitStruct;
    
    /* ADC IRQ */  
    NVIC_InitStruct.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);


    ADC_InitTypeDef adcInitStruct;
    
    ADC_StructInit(&adcInitStruct);
    
    adcInitStruct.channelMap = ADC_CH6;
    ADC_Init(ADC, &adcInitStruct);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);
    ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);

	

    return;
}

static uint16_t adcTestValue;
void test_hr_scan(void)
{
    portENTER_CRITICAL();
    Pad_Config(HR_ADC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    portEXIT_CRITICAL();
    //enable ADC read and wait for ADC data ready interrupt
    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);   //must disable first
    ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
    /* read ADC convert result*/
    while (ADC_GetIntFlagStatus(ADC, ADC_INT_ONE_SHOT_DONE) != SET);
    ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);   //disable for manual power mode
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
    /*read sample result*/
    adcTestValue = ADC_Read(ADC, ADC_CH6);
    portENTER_CRITICAL();
    Pad_Config(HR_ADC, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    portEXIT_CRITICAL();
}

void hrTimerInit(void);

/**
* @brief  main() is the entry of user code.
* @param   No parameter.
* @return  void
*/
void GpioTest(void)
{

	j=0;
	Board_GPIO_Init();
	test_hr_Init();
	Driver_GPIO_Init();

	GPIO_DemoCode();
//	delayMS(2000);
	hrTimerInit();
	
	
	count = 0;
	
	while(1)
	{
	if(adFinish==1){
		#if 0
			if(j%2==0){
		GPIO_SetBits(GPIO_Output_Pin);
		}else {
		GPIO_ResetBits(GPIO_Output_Pin);
			}
		j++;
			#endif
		 /* start adc convert again */
          ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
           ADC_Cmd(ADC, ADC_One_Shot_Mode, ENABLE);
		adFinish =0;
		}
	//		test_hr_scan();	
//			if(i >= 100)
//				i= 0;
//			testadcbuf[i++] = adcTestValue;
//			adcTestValue = 0;
		//	for(;adcTestValue < 0x4000;adcTestValue++)
		//	;
	}
}


#if 1
#include "rtl876x_tim.h"
void hrTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_InitStruct;

  TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_32KHZ;
	TIM_InitStruct.TIM_Period =  16*5; //2.5ms
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM4, &TIM_InitStruct);
    
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIMER4_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_ClearINT(TIM4);
    TIM_INTConfig(TIM4,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}



void hrTimerHandle()
{
	HRState bpmState;
	uint16_t heartRate ;
	uint32_t ret ;
	TIM_ClearINT(TIM4);
	#if 0
	//test_hr_scan();
	if(i%2==0){
		GPIO_SetBits(GPIO_Output_Pin);
		}else {
		GPIO_ResetBits(GPIO_Output_Pin);
			}
	//GPIO_WriteBit(GPIO_Output_Pin, (BitAction)(1));
if(i > 256){
	i= 0;
	//}
}

			if(count<4000){
				if(count%16 == 0){
				len = sprintf( StrBuffer, "\r\n");
				UART_SendData(UART, StrBuffer, len);
				}
			len = sprintf( StrBuffer, "%d,",adcTestValue);
			UART_SendData(UART, StrBuffer, len);
				
			}
			//	count++;
#endif
	
		
		//bpmState =	 heartRateWaveDetect(adcTestValue);  
		//if(bpmState==HRFinish){
	//		heartRate = getHeartRateFilter();
		//	sprintf( StrBuffer, "Count[%d]=%d\r\n",count,heartRate);
		//	UART_SendData(UART, StrBuffer, 16);
		//	count++;
		//	heartRateInit();
					
		//	}
	testadcbuf[i++] = adcTestValue;
	adcTestValue = 0;
			
}


#if 1
void ADCIntrHandler(void)
{
   HRState bpmState;
 	uint16_t heartRate ;
	
	ADC_Cmd(ADC, ADC_One_Shot_Mode, DISABLE);
    
    ADC_ClearINTPendingBit(ADC, ADC_INT_ONE_SHOT_DONE);
    
    /* read ADC convert result*/
    adcTestValue = ADC_Read(ADC, ADC_CH6);
		bpmState =	 heartRateWaveDetect(adcTestValue);  
		if(bpmState==HRFinish){
			heartRate = getHeartRateFilter();
			StrBuffer[0] = HexToAscii(heartRate/100%10);
      StrBuffer[1] = HexToAscii(heartRate/10%10);
      StrBuffer[2] = HexToAscii(heartRate%10);
			StrBuffer[3] = '\r';
			StrBuffer[4] = '\n';
		//	sprintf( StrBuffer, "Count[%d]=%d\r\n",count,heartRate);
			UART_SendData(UART, StrBuffer, 6);
			count++;
			heartRateInit();
			
					
			}
		
 //   DBG_BUFFER(MODULE_APP, LEVEL_INFO, "ADC convert result = 0x%x", 1, adcConvertRes);
   adFinish = 1;
    return;
}
#endif

#endif

