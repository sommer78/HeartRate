/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_it.h
* @brief     
* @details   
* @author    Chuanguo Xue
* @date      2015-3-27
* @version   v0.1
* *********************************************************************************************************
*/
#ifndef _RTL876X_INT_H
#define _RTL876X_INT_H

#ifdef __cplusplus
 extern "C" {
#endif

extern void SysOnIntrHandler(void);
extern void WdgIntrHandler(void);
extern void Timer2IntrHandler(void);
extern void Gpio0IntrHandler(void);
extern void Gpio1IntrHandler(void);
extern void LogUartIntrHandler(void);
extern void DataUartIntrHandler(void);
extern void RTCIntrHandler(void);
extern void Spi0IntrHandler(void);
extern void Spi1IntrHandler(void);
extern void I2C0IntrHandler(void);
extern void I2C1IntrHandler(void);
extern void ADCIntrHandler(void);
extern void PeripheralIntrHandler(void);
extern void Gdma0Ch0IntrHandler(void);
extern void Gdma0Ch1IntrHandler(void);
extern void Gdma0Ch2IntrHandler(void);
extern void Gdma0Ch3IntrHandler(void);
extern void Gdma0Ch4IntrHandler(void);
extern void Gdma0Ch5IntrHandler(void);
extern void KeyscanIntrHandler(void);
extern void QdecodeIntrHandler(void);
extern void IRIntrHandler(void);
extern void SPI2wireIntrHandler(void);
extern void AnalogComparatorIntrHandler(void);

extern void SPIFLASHIntrHandler(void);
extern void Timer3IntrHandler(void);
extern void Timer4IntrHandler(void);
extern void Timer5IntrHandler(void);
extern void Timer6IntrHandler(void);
extern void Timer7IntrHandler(void);

extern void Gpio2IntrHandler(void);
extern void Gpio3IntrHandler(void);
extern void Gpio4IntrHandler(void);
extern void Gpio5IntrHandler(void);
extern void Gpio6IntrHandler(void);
extern void Gpio7IntrHandler(void);
extern void Gpio8IntrHandler(void);
extern void Gpio9IntrHandler(void);
extern void Gpio10IntrHandler(void);
extern void Gpio11IntrHandler(void);
extern void Gpio12IntrHandler(void);
extern void Gpio13IntrHandler(void);
extern void Gpio14IntrHandler(void);
extern void Gpio15IntrHandler(void);
extern void Gpio16IntrHandler(void);
extern void Gpio17IntrHandler(void);
extern void Gpio18IntrHandler(void);
extern void Gpio19IntrHandler(void);
extern void Gpio20IntrHandler(void);
extern void Gpio21IntrHandler(void);
extern void Gpio22IntrHandler(void);
extern void Gpio23IntrHandler(void);
extern void Gpio24IntrHandler(void);
extern void Gpio25IntrHandler(void);
extern void Gpio26IntrHandler(void);
extern void Gpio27IntrHandler(void);
extern void Gpio28IntrHandler(void);
extern void Gpio29IntrHandler(void);
extern void Gpio30IntrHandler(void);
extern void Gpio31IntrHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_INT_H */

