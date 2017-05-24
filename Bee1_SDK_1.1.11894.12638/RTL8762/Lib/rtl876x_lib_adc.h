/**
************************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     rtl876x_lib_adc.h
* @brief
* @details
* @author
* @date     2016-03-22
* @version
*************************************************************************************************************
*/

#ifndef __RTL876X_LIB_ADC_H
#define __RTL876X_LIB_ADC_H

#include "rtl_types.h"

/**
  * @brief  Convert data from temperature data regiser to KALVIM or CELSIUS set CR bit 27 '1'
  * @param  Temp_data: The 12-bit converted ADC data.
  * @param  TEMP_TYPE: the result is KALVIN or CELSIUS
        This parameter can be one of the following values:
  *     @arg CELSIUS: result in CELSIUS mode.
  *     @arg KELVIN: result in KALVIN mode.
  * @retval The converted temperature result in KALVIN or CELSIUS mode
  */
float ADC_Temperature_Conver(uint16_t Temp_data, bool TEMP_TYPE);

/**
  * @brief  Get different mode calibration result.IF you want use this API, you must be sure that your chip have been calibration.
  * @param[in] cin, converted ADC data.
  * @diff_voltage = k.cin + b, cin is the hex read from ADC register.
  * @retval None
  */
float ADC_GetDiffCalRes(uint16_t cin);

/**
  * @brief  Get calibration result.IF you want use this API, you must be sure that your chip have been calibration.
  * @param  cin, converted ADC data.
  * @retval voltage in float mode
  */
float ADC_GetCalRes(uint16_t cin);

/**
  * @brief  Get calibration result.ADC_GetDirectIN
  * @param  cin, converted ADC data.
  * @param  VbatMode.
  *     @arg VBAT_28V_33V: the voltage of Vbat between 2.8v~3.3v
  *     @arg VBAT_20V_28V: the voltage of Vbat between 2.0v~2.799v
  * @retval voltage in float mode
  */
float ADC_GetDirectIN(uint16_t cin, uint16_t VbatMode);

/**
  * @brief  Get calibration result.ADC_GetPullUPIN
  * @param  cin, converted ADC data.
  * @param  VbatMode.
  *     @arg VBAT_28V_33V: the voltage of Vbat between 2.8v~3.3v
  *     @arg VBAT_20V_28V: the voltage of Vbat between 2.0v~2.799v
  * @retval voltage in float mode
  */
float ADC_GetPullUPIN(uint16_t cin, uint16_t VbatMode);

/**
  * @brief  Get calibration result.ADC_GetRin
  * @param  cin, converted ADC data.
  * @param  VbatMode.
  *     @arg VBAT_28V_33V: the voltage of Vbat between 2.8v~3.3v
  *     @arg VBAT_20V_28V: the voltage of Vbat between 2.0v~2.799v
  * @retval The value of Rin
  */
float ADC_GetRin(uint16_t cin, uint16_t VbatMode);

/**
  * @brief  Get calibration result.ADC_GetVcc
  * @param  cin, converted ADC data.
  * @param  VbatMode.
  * This parameter can be one of the following values:
  *     @arg VBAT_28V_33V: the voltage of Vbat between 2.8v~3.3v
  *     @arg VBAT_20V_28V: the voltage of Vbat between 2.0v~2.799v
  * @retval voltage in float mode
  */
float ADC_GetVcc(uint16_t cin, uint16_t VbatMode);

/**
  * new FT version
  */
float ADC_GetDirectIN_CHx(uint32_t channel, uint16_t cin);
float ADC_GetPullUPIN_CHx(uint32_t channel, uint16_t cin);
float ADC_GetRin_CHx(uint32_t channel, uint16_t cin);
/**
  * @brief  Get calibration result.ADC_GetVcc_CHx
  * @param  channel ,channel num
  * @param  cin, converted ADC data.
  * @param  R1.
  * @param  R2.
  * This parameter can be one of the following values:
  * @retval voltage in float mode
  */
float ADC_GetVcc_CHx(uint32_t channel, uint16_t cin, float R1, float R2);
#endif /*__RTL876X_LIB_ADC_H*/
