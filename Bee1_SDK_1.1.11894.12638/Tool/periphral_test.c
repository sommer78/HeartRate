/* Defines ------------------------------------------------------------------*/
#define Output_Pin            P2_7
#define GPIO_Output_Pin       GPIO_GetPin(Output_Pin)

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
uint32_t testadcbuf[100];

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void Board_GPIO_Init(void)
{
	Pad_Config(Output_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pinmux_Config(Output_Pin, GPIO_FUN);
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



#define HR_ADC							P2_6

#include "rtl876x_adc.h"

void test_hr_Init(void)
{
    Pad_Config(HR_ADC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
//    Pad_Config(BATTERY_CHECK, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

	/* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

    /* ADC init */
    ADC_InitTypeDef adcInitStruct;

    ADC_StructInit(&adcInitStruct);

    /* use channel battery */
    adcInitStruct.channelMap = ADC_CH6;
    ADC_Init(ADC, &adcInitStruct);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE);

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
	Board_GPIO_Init();
	test_hr_Init();
	Driver_GPIO_Init();
	hrTimerInit();
	GPIO_DemoCode();
	int i;
	i = 0;
	while(1)
	{
	//		test_hr_scan();	
//			if(i >= 100)
//				i= 0;
//			testadcbuf[i++] = adcTestValue;
//			adcTestValue = 0;
			for(;adcTestValue < 0x4000;adcTestValue++)
			;
	}
}


#if 1
#include "rtl876x_tim.h"
void hrTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_InitStruct;

  TIM_StructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
	TIM_InitStruct.TIM_Period = 10000*100; //1ms
	TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_TimeBaseInit(TIM4, &TIM_InitStruct);
    
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIMER4_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	TIM_ClearINT(TIM4);
  TIM_INTConfig(TIM4,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

#define hrTimerHandle									Timer4IntrHandler
uint8_t i;
void hrTimerHandle()
{
	test_hr_scan();
			if(i >= 100)
				i= 0;
			testadcbuf[i++] = adcTestValue;
			adcTestValue = 0;
}

#endif
	