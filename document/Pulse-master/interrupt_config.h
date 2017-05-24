#ifndef __IT_CONFIG_H
#define __IT_CONFIG_H

#include "stm32f10x.h"

#define TRUE 1
#define FALSE 0

/* NORMAL: being used state */
/* ABNORMAL: not being used state */
#define NORMAL 1
#define ABNORMAL 0

/* The Threshold used to determine whether the device is being used or not */
/* NORMAL: more than LOWER_TRIGGER and less than UPPER_TRIGGER */
/* ABNORMWL: less than LOWER_TRIGGER and more than UPPER_TRIGGER */
/* TRIGGER_DELTA: the max delta between two normal heart wave */
#define UPPER_TRIGGER 800
#define LOWER_TRIGGER 400
#define TRIGGER_DELTA 150

extern __IO int rate[];                    // array to hold last ten IBI values
extern __IO unsigned long sampleCounter;   // used to determine pulse timing
extern __IO unsigned long lastBeatTime;    // used to find IBI
extern __IO int P;                         // used to find peak in pulse wave, seeded
extern __IO int T;                         // used to find trough in pulse wave, seeded
extern __IO int thresh;                    // used to find instant moment of heart beat, seeded
extern __IO int amp;                       // used to hold amplitude of pulse waveform, seeded
extern __IO int firstBeat;                 // used to seed rate array so we startup with reasonable BPM
extern __IO int secondBeat;                // used to seed rate array so we startup with reasonable BPM
extern __IO int BPM;                       // used to hold the pulse rate
extern __IO int IBI;                       // holds the time between beats, must be seeded! 
extern __IO int Pulse;                     // true when pulse wave is high, False when it's low
extern __IO int QS;                        // becomes True when Arduoino finds a beat.

extern __IO int      counter;              // count timer interrupt to ten to send data
extern __IO uint32_t preSignal;            // store the last signal value
extern __IO int      Status_Flag;          // status flag stands for the state of current heart wave


void TIM2_Configurtion(void);
void NVIC_Configuration(void);

#endif
