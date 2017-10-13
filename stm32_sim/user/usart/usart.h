#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"



#define MC_DEBUG_ON
#define 	OLED
#ifdef MC_DEBUG_ON
#define MC_INFO( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#define MC_INFO( format, ... )
#endif

	
void uart1_init(u32 bound);
void uart2_init(u32 bound);
void USART2_printf( char *Data, ... );
void USART2_printfByte( uint8_t *Data, int len );
void debugInfo(char* info, int column);
 void Sim_Usart_GPIO_Configuration(void);
 void SendOneByte(u8 datatoSend);	
#endif


