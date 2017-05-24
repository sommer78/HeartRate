#include "interrupt_config.h"
#include "stm32f10x_tim.h"
#include "misc.h"


__IO int rate[10];                    // array to hold last ten IBI values
__IO unsigned long sampleCounter = 0; // used to determine pulse timing
__IO unsigned long lastBeatTime = 0;  // used to find IBI
__IO int P = 512;                     // used to find peak in pulse wave, seeded
__IO int T = 512;                     // used to find trough in pulse wave, seeded
__IO int thresh = 512;                // used to find instant moment of heart beat, seeded
__IO int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
__IO int firstBeat = TRUE;            // used to seed rate array so we startup with reasonable BPM
__IO int secondBeat = FALSE;          // used to seed rate array so we startup with reasonable BPM
__IO int BPM;                         // used to hold the pulse rate
__IO int IBI = 600;                   // holds the time between beats, must be seeded! 
__IO int Pulse = FALSE;               // true when pulse wave is high, false when it's low
__IO int QS = FALSE;                  // becomes true when Arduoino finds a beat.

__IO int      counter = 9;            // count timer interrupt to ten to send data
__IO uint32_t preSignal = 500;        // store the last signal value
__IO int      Status_Flag = ABNORMAL; // status flag stands for the state of current heart wave

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Configurtion(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/*设置TIM2分配时钟,采用72MHz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	/*设置2ms中断一次*/
	TIM_TimeBaseStructure.TIM_Period=1999;
	TIM_TimeBaseStructure.TIM_Prescaler=71;

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2,TIM_FLAG_Update);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

	TIM_Cmd(TIM2, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}
