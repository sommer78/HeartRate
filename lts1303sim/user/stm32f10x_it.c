/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <stdio.h>
#include "bsp_SysTick.h"
#include "bsp_usart2.h"
#include "stm32f10x_exti.h"
#include "bsp_gpio.h"   

//extern void TimingDelay_Decrement(void);
//extern void USART2_printf(USART_TypeDef* USARTx, char *Data,...);


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();	
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles USART2 Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler( void )
{	

 
}


void USART1_IRQHandler(void)      //串口1 中断服务程序
{
#if 0
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	    
		     RxDataBuff[g_u8RxIndexW] = USART_ReceiveData(USART1);
				USART_SendData(USART2, RxDataBuff[g_u8RxIndexW]);
				g_u8RxIndexW++;
				if (g_u8RxIndexW > RX_BUFF_LEN-1)
        g_u8RxIndexW = 0;
		
				//RxBuffer1[RxCounter1]=USART_ReceiveData(USART1);
				//RxCounter1++;
		
				
	}  
 


  	char ch;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ch  = USART_ReceiveData( USART1 );
		
		
		if( M26_Fram_Record  .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
		{
				M26_Fram_Record .Data_RX_BUF [ M26_Fram_Record  .FramLength ++ ]  = ch;

		}
		if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8)){
			LED2_ON;
			}else {
			LED2_OFF;
			}
	}

	 	 
	if ( USART_GetITStatus( USART1, USART_IT_IDLE ) != RESET )                                         //数据帧接收完毕
	{
	
    M26_Fram_Record  .FramFinishFlag = 1;
		
		ch = USART_ReceiveData( USART1 );   
		if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_6)){
			LED3_ON;
			}else {
			LED3_OFF;
			}
	}
	 #endif

}
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
//char os_str[512];
 // sprintf( os_str,    "RTC irq \0");
		
	//OLED_ShowStr(0,0,os_str,1);
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/



void TIM3_IRQHandler(void)  
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  ); 
	//	  M26_Fram_Record  .FramFinishFlag = 1;
		TIM_Cmd(TIM3,DISABLE);
		/*
		if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_5)){
			LED4_ON;
			}else {
			LED4_OFF;
			}
		*/
		}
		
}





/**
  * @}
  */ 
void EXTI1_IRQHandler(void)
{	   

	if(EXTI_GetITStatus(EXTI_Line1)!= RESET)
	{

		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
