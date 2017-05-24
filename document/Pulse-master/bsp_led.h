#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

extern __IO int fadeRate;

void LED_Configuration(void);
void LED_On(void);
void LED_Off(void);
void Delay(__IO uint32_t);
int constrain(int, int, int);
void fadeLED(int);
void ledFadeToBeat(void);

#endif
