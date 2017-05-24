/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of the hardware of trigonometric function calculation.
* @details   
* @author   elliot chen
* @date     2016-03-1
* @version  v1.0
*********************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "hardware_calculate.h"
#include "os.h"

/* Please select uint of input parameters before use some APIs which involve function of sin/cos/tan or RotateTransformByAngle */
/* User can select in hardware_calculate.h by configure UNIT_IS_PI and OPTIMIZE_PERFORMANCE */
/* Please refer to the document notes specific information in hardware_calculate.h */

/**
  * @brief  calculate performance of hardware function,such as sin, cos and so on.
  * @param  none.
  * @retval none.
  */
void Performance_Test(void)
{
	float xdata = 0;
	float angle = 0;
	COORDINATE x;
	int i = 0;

	vPortEnterCritical();
    
	DBG_DIRECT("Performance_Test begin!");
	for(i = 0; i < 10000; i++)
	{
		/* function of sin(x) */
		xdata = sina(-0.166*3.14159265898);
		//xdata = sina(-0.166);

		/* function of cos(x) */
		xdata = cosa(-0.166*3.14159265898);
		//xdata = cosa(-0.166);

		/* function of tan(x) */
		xdata = tana(-0.166*3.14159265898);
		//xdata = tana(-0.166);

		/* function of  Rotation transformation */
		RotateTransformByAngle(0.70710678, 0.70710678, -0.5*3.14159265898, &x);
		//RotateTransformByAngle(0.70710678, 0.70710678, -0.5, &x);

		/* function of arctan(x) */
		angle = arctana(1.73205)*3.14159265898;
		angle = arctana(0.57735)*3.14159265898;
		angle = arctana(-1.73205)*3.14159265898;
		angle = arctana(-0.57735)*3.14159265898;

		/* function of sqrt */
		xdata = sqrta(-0.70710678, -0.70710678);

		/* function of transfer value of rotate to angle */
		angle = RotateToAngle(-0.866025,-0.5);
	}
	DBG_DIRECT("Performance_Test end!");

	vPortExitCritical();
}

void test_code(void)
{ 
	/* Enable hardware calculate function before you use it */
	ALU_Cmd(ENABLE);

	/* Performance test */
	Performance_Test();
	while(1);
}

/**
* @brief  main() is the entry of user code.
* @param   No parameter.
* @return  void
*/
int main(void)
{ 
	test_code();  
}

