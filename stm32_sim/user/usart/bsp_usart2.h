#ifndef __USART2_H
#define	__USART2_H


#include "stm32f10x.h"
#include <stdio.h>


void USART2_Config( void );
#define     PC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

//extern void bsp_USART2_IRQHandler(void);
//extern char *get_rebuff(uint8_t *len);
//extern void clean_rebuff(void);


#endif /* __USART2_H */
