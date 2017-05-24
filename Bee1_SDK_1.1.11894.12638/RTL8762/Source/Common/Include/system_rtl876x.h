/* Copyright (c) 2015 Realtek Semiconductor. All Rights Reserved. 
 *           
 *  The information contained herein is property of Realtek Semiconductor. 
 *  Terms and conditions of usage are described in detail in Realtek 
 *  SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *           
 *   Licensees are granted free, non-transferable use of the information. NO 
 *   WARRANTY of ANY KIND is provided. This heading must NOT be removed from 
 *   the file. 
 *
 */

#ifndef __SYSTEM_RTL876X_H
#define __SYSTEM_RTL876X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define __XTAL              (40000000UL)    /* Oscillator frequency */

//extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock)  */
extern uint32_t SystemClock;    /*!< System Clock Frequency (Core Clock)  */

extern void System_Init (void);

extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif

