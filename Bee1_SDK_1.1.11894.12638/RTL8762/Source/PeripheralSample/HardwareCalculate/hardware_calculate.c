/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hardware_calculate.c
* @brief    This file provides APIs for hardware calculate.
* @details
* @author   elliot chen
* @date     2016-02-29
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "hardware_calculate.h"

/* Internal Defines ------------------------------------------------------------*/
#define HARDWARE_ALU_REG_BASE		(VENDOR_REG_BASE + 0x2c)
#define HW_ALU                      ((HW_ALU_TypeDef*) HARDWARE_ALU_REG_BASE)
#define PI							(3.14159265898)
#define PHASE_RESOLUTION			(0x2000)
#define OUT_RESOLUTION				(32768)
#define	NARROW_RATIO				(0.5)
#define	ARCTAN_GAIN					(OUT_RESOLUTION*NARROW_RATIO)

/* ANGLE_TRANSFORM = PHASE_RESOLUTION/PI */
const ALU_IN_TYPE ANGLE_TRANSFORM = 2607.594583144;

/**
  * @brief  Enables or disables the ALU peripheral.
  * @param  NewState: new state of the ALU peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ALU_Cmd(FunctionalState NewState)
{

	/* Check the parameters */
	 assert_param(IS_FUNCTIONAL_STATE(NewState));

	 if (NewState != DISABLE)
	 {
	 	/* Enable clock of ALU */
		*((volatile uint32_t *)0x40000230) |= BIT(1);
	 	
		/* Enable clock of vendor register */
		*((volatile uint32_t *)0x40000230) |= BIT(6);

		/* Enable ALU module */
		HW_ALU->CTL = BIT(0);
	 }
	 else
	 {
		/* Disable clock of ALU */
		*((volatile uint32_t *)0x40000230) &= ~(BIT(1));

		/* Disable ALU module */
		HW_ALU->CTL &= ~(BIT(0));
	 }
}

/**
  * @brief  calculate function of sin(x).
  * @param  x: input data which can be -PI to PI.
  * @retval value of function of sin(x)
  */
ALU_OUT_TYPE sina(ALU_IN_TYPE x)
{
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 0 which as sin/cos calculate function*/
	HW_ALU->CTL = BIT(0);

	/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
	HW_ALU->IN0 = 32767;

	/* Convert input data to value of register */
	if(x > 0)
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = (uint32_t)(x*PHASE_RESOLUTION);//%PHASE_RESOLUTION
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = (uint32_t)(x*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = (uint32_t)((x/PI)*PHASE_RESOLUTION);
#endif
#endif
	}
	else
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = 65536 + (int16_t)(x*PHASE_RESOLUTION);
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = 65536 + (int16_t)(x*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = 65536 + (int16_t)((x/PI)*PHASE_RESOLUTION);
#endif
#endif	
	}

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}
	return ((ALU_OUT_TYPE)((int16_t)(((HW_ALU->OUT0)>>16)&0xffff)))/OUT_RESOLUTION;
}

/**
  * @brief  calculate function of cos(x).
  * @param  x: input data which can be -PI to PI.
  * @retval value of function of cos(x)
  */

ALU_OUT_TYPE cosa(ALU_IN_TYPE x)
{
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 0 which as sin/cos calculate function*/
	HW_ALU->CTL = BIT(0);

	/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
	HW_ALU->IN0 = 32767;

	/* Convert input data to value of register */
	if(x > 0)
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = (uint32_t)(x*PHASE_RESOLUTION);//%PHASE_RESOLUTION
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = (uint32_t)(x*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = (uint32_t)((x/PI)*PHASE_RESOLUTION);
#endif
#endif
	}
	else
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = 65536 + (int16_t)(x*PHASE_RESOLUTION);
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = 65536 + (int16_t)(x*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = 65536 + (int16_t)((x/PI)*PHASE_RESOLUTION);
#endif
#endif	
	}

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}
	
	return ((ALU_OUT_TYPE)((int16_t)((HW_ALU->OUT0)&0xffff)))/OUT_RESOLUTION;
}

/**
  * @brief  calculate function of tan(x).
  * @param  x: input data which can be -PI<= x< -PI/2, - PI/2<x<PI/2, PI/2<x<=PI.
  * @retval value of function of tan(x)
  */
ALU_OUT_TYPE tana(ALU_IN_TYPE x)
{
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 0 which as sin/cos calculate function*/
	HW_ALU->CTL = BIT(0);

	/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
	HW_ALU->IN0 = 32767;

	/* Convert input data to value of register */
	if(x > 0)
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = (uint32_t)(x*PHASE_RESOLUTION);//%PHASE_RESOLUTION
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = (uint32_t)(x*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = (uint32_t)((x/PI)*PHASE_RESOLUTION);
#endif
#endif
	}
	else
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = 65536 + (int16_t)(x*PHASE_RESOLUTION);
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = 65536 + (int16_t)(x*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = 65536 + (int16_t)((x/PI)*PHASE_RESOLUTION);
#endif
#endif	
	}

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}
	if(((HW_ALU->OUT0)& 0xffff) != 0)
	{
		return ((ALU_OUT_TYPE)((int16_t)(((HW_ALU->OUT0)>>16)&0xfffff)))/((int16_t)((HW_ALU->OUT0)&0xffff));
	}
	else
	{
		return 0xffffffff;
	}
}

/**
  * @brief  Rotation transformation.
  * @param  x: the original coordinates x.
  * @param  y: the original coordinates y.Must be meet the following: x^2 +y^2 <=1!!!
  * @param  phase: rotation angle which can be -PI to PI or -1 to 1 which can be configure!!!
  * @retval value of new coordinates.
  */
void RotateTransformByAngle(ALU_IN_TYPE x, ALU_IN_TYPE y, ALU_IN_TYPE phase, COORDINATE *coordinate)
{
	
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 0 */
	HW_ALU->CTL = BIT(0);

	/* Convert r_hw_alu_in_a to value of register */
	if(x > 0)
	{
		/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
		HW_ALU->IN0 = (uint16_t)(x*32768);
	}
	else
	{
		/* Configure r_hw_alu_in_a */
		HW_ALU->IN0 = 65536 + (int16_t)(x*32768);
	}
	
	/* Convert r_hw_alu_in_b to value of register */
	if(y > 0)
	{
		/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
		HW_ALU->IN0 |= (uint16_t)(y*32768) << 16;
	}
	else
	{
		/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
		HW_ALU->IN0 |= (65536 + (int16_t)(y*32768)) << 16;
	}
	
	/* Configure r_hw_alu_in_c and r_hw_alu_in_d */
	if(phase > 0)
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = (uint16_t)(phase*PHASE_RESOLUTION);
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = (uint16_t)(phase*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = (uint16_t)((phase/PI)*PHASE_RESOLUTION);
#endif
#endif
	}
	else
	{
#if UNIT_IS_PI
		HW_ALU->IN1 = 65536 + (int16_t)(phase*PHASE_RESOLUTION);
#else
#if OPTIMIZE_PERFORMANCE
		HW_ALU->IN1 = 65536 + (int16_t)(phase*ANGLE_TRANSFORM);
#else
		HW_ALU->IN1 = 65536 + (int16_t)((phase/PI)*PHASE_RESOLUTION);
#endif
#endif
	}

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}

	/* Store value */	
	coordinate->x = ((ALU_OUT_TYPE)((int16_t)((HW_ALU->OUT0)&0xffff)))/OUT_RESOLUTION;
	coordinate->y = ((ALU_OUT_TYPE)((int16_t)(((HW_ALU->OUT0)>>16)&0xffff)))/OUT_RESOLUTION;
}


/**
  * @brief  calculate function of arctan(x).
  * @param  x: input data .
  * @retval value of function of arctan(x) which can be -1 to 1.
  */
ALU_OUT_TYPE arctana(ALU_IN_TYPE x)
{
	
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 1 */
	HW_ALU->CTL = BIT(0)|BIT(1);

	/* Convert input data to value of register */
	if(x > 0)
	{
		/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
		HW_ALU->IN0 = ((uint16_t)(ARCTAN_GAIN/x))|(((uint16_t)(ARCTAN_GAIN))<<16);
	}
	else
	{
		/* Configure r_hw_alu_in_a and r_hw_alu_in_b */
		HW_ALU->IN0 = ((65536+(int16_t)(ARCTAN_GAIN/x)))| (((uint16_t)(ARCTAN_GAIN))<<16);
	}
	/* Configure r_hw_alu_in_c and r_hw_alu_in_d */
	HW_ALU->IN1 = 0;

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}

	return ((ALU_OUT_TYPE)((int16_t)((HW_ALU->OUT1)&0xffff)))/PHASE_RESOLUTION;
}

/**
  * @brief  calculate function of sqrt(x, y).
  * @param  x: input data.
  * @param  y: input data. Must be meet the following: x^2 +y^2 <=1!!!
  * @retval value of function of atan(x) which can be 0 to 1.
  */
ALU_OUT_TYPE sqrta(ALU_IN_TYPE x, ALU_IN_TYPE y)
{
	
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 1 */
	HW_ALU->CTL = BIT(0)|BIT(1);

	/* Convert ir_hw_alu_in_a to value of register */
	if(x > 0)
	{
		HW_ALU->IN0 = ((int16_t)(x*32768));
	}
	else
	{
		HW_ALU->IN0 = (65536 + (int16_t)(x*32768));
	}

	/* Convert r_hw_alu_in_b to value of register */
	if(y > 0)
	{
		HW_ALU->IN0 |= ((int16_t)(y*32768)) << 16;
	}
	else
	{
		HW_ALU->IN0 |= (65536 + (int16_t)(y*32768)) << 16;
	}
	/* Configure r_hw_alu_in_c and r_hw_alu_in_d */
	HW_ALU->IN1 = 0;

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}

	return ((ALU_OUT_TYPE)((int16_t)((HW_ALU->OUT0)&0xffff)))/OUT_RESOLUTION;
}

/**
  * @brief  transfer value of rotate to angle.
  * @param  x: x rotate value.
  * @param  y: y totate value. Must be meet the following: x^2 +y^2 <=1!!!
  * @retval value of function of atan(x) which can be -1 to 1.
  */
ALU_OUT_TYPE RotateToAngle(ALU_IN_TYPE x, ALU_IN_TYPE y)
{
	uint32_t time_out = 0xfffff;
	
	/* Enable ALU module and configure to mode 1 */
	HW_ALU->CTL = BIT(0)|BIT(1);

	/* Convert x to value of register */
	if(x > 0)
	{
		HW_ALU->IN0 = (uint16_t)(x*32768);
	}
	else
	{
		HW_ALU->IN0 = (65536 + (int16_t)(x*32768));
	}

	/* Convert y to value of register */
	if(y > 0)
	{
		HW_ALU->IN0 |= ((uint16_t)(y*32768)) << 16;
	}
	else
	{
		HW_ALU->IN0 |= (65536 + (int16_t)(y*32768)) << 16;
	}
	
	/* Configure r_hw_alu_in_c and r_hw_alu_in_d */
	HW_ALU->IN1 = 0;

	/* Configure trig to 0 and then configure it to 1 */
	HW_ALU->CTL &= ~(BIT(4));
	HW_ALU->CTL |= BIT(4);

	/* Check transmission finish or not */
	while(((HW_ALU->CTL & BIT(31)) == 0) && (time_out>0))
	{
		time_out--;
	}

	return ((ALU_OUT_TYPE)((int16_t)((HW_ALU->OUT1)&0xffff)))/PHASE_RESOLUTION;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor Corporation *****END OF FILE****/

