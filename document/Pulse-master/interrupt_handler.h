#ifndef __IT_HANDLER_H
#define __IT_HANDLER_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

int detect_trigger(uint32_t tempSignal);
int detect_state();
void calculate_process(uint32_t);
void normal_process(uint32_t tempSignal);
void abnormal_process(uint32_t tempSignal);

#endif
