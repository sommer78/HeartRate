;/*****************************************************************************/
;/* arm_startup_rtl8762.s: Startup file for Realtek RTL8762 device series     */
;/*****************************************************************************/

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     0x55534552             ; FLAG - 'USER'
                DCD     Reset_Handler          ; Reset Handler
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved
                DCD     0                      ; Reserved

                ; External Interrupts
                DCD     SysOnIntrHandler
                DCD     WdgIntrHandler
                DCD     0
                DCD     0
                DCD     Timer2IntrHandler
                DCD     0
                DCD     0
                DCD     0
                DCD     0
                DCD     Gpio0IntrHandler
                DCD     Gpio1IntrHandler
                DCD     LogUartIntrHandler
                DCD     DataUartIntrHandler
                DCD     RTCIntrHandler
                DCD     Spi0IntrHandler
                DCD     Spi1IntrHandler
                DCD     I2C0IntrHandler
                DCD     I2C1IntrHandler
                DCD     ADCIntrHandler
                DCD     PeripheralIntrHandler
                DCD     Gdma0Ch0IntrHandler
                DCD     Gdma0Ch1IntrHandler
                DCD     Gdma0Ch2IntrHandler
                DCD     Gdma0Ch3IntrHandler
                DCD     Gdma0Ch4IntrHandler
                DCD     Gdma0Ch5IntrHandler
                DCD     KeyscanIntrHandler
                DCD     QdecodeIntrHandler
                DCD     IRIntrHandler
                DCD     0
                DCD     SPI2wireIntrHandler
                DCD     AnalogComparatorIntrHandler

                DCD     SPIFLASHIntrHandler         ;48
                DCD     Gpio2IntrHandler            ;49
                DCD     Gpio3IntrHandler            ;50
                DCD     Gpio4IntrHandler            ;51
                DCD     Gpio5IntrHandler            ;52
                DCD     Timer3IntrHandler           ;53
                DCD     Timer4IntrHandler           ;54
                DCD     Timer5IntrHandler           ;55
                DCD     Timer6IntrHandler           ;56
                DCD     Timer7IntrHandler           ;57
                DCD     Gpio6IntrHandler            ;58
                DCD     Gpio7IntrHandler
                DCD     Gpio8IntrHandler
                DCD     Gpio9IntrHandler
                DCD     Gpio10IntrHandler
                DCD     Gpio11IntrHandler
                DCD     Gpio12IntrHandler
                DCD     Gpio13IntrHandler
                DCD     Gpio14IntrHandler
                DCD     Gpio15IntrHandler
                DCD     Gpio16IntrHandler
                DCD     Gpio17IntrHandler
                DCD     Gpio18IntrHandler
                DCD     Gpio19IntrHandler
                DCD     Gpio20IntrHandler
                DCD     Gpio21IntrHandler
                DCD     Gpio22IntrHandler
                DCD     Gpio23IntrHandler
                DCD     Gpio24IntrHandler
                DCD     Gpio25IntrHandler
                DCD     Gpio26IntrHandler
                DCD     Gpio27IntrHandler
                DCD     Gpio28IntrHandler
                DCD     Gpio29IntrHandler
                DCD     Gpio30IntrHandler
                DCD     Gpio31IntrHandler
               
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler               [WEAK]
                IMPORT  System_Init
                IMPORT  __main

                LDR     R0, =System_Init
                BLX     R0
                LDR     R0, =__main
                BX      R0

                ENDP


Default_Handler PROC
                EXPORT  SysOnIntrHandler            [WEAK]
                EXPORT  WdgIntrHandler              [WEAK]
                EXPORT  Timer2IntrHandler           [WEAK]
                EXPORT  Gpio0IntrHandler            [WEAK]
                EXPORT  Gpio1IntrHandler            [WEAK]
                EXPORT  LogUartIntrHandler          [WEAK]
                EXPORT  DataUartIntrHandler         [WEAK]
                EXPORT  RTCIntrHandler              [WEAK]
                EXPORT  Spi0IntrHandler             [WEAK]
                EXPORT  Spi1IntrHandler             [WEAK]
                EXPORT  I2C0IntrHandler             [WEAK]
                EXPORT  I2C1IntrHandler             [WEAK]
                EXPORT  ADCIntrHandler              [WEAK]
                EXPORT  PeripheralIntrHandler       [WEAK]
                EXPORT  Gdma0Ch0IntrHandler         [WEAK]
                EXPORT  Gdma0Ch1IntrHandler         [WEAK]
                EXPORT  Gdma0Ch2IntrHandler         [WEAK]
                EXPORT  Gdma0Ch3IntrHandler         [WEAK]
                EXPORT  Gdma0Ch4IntrHandler         [WEAK]
                EXPORT  Gdma0Ch5IntrHandler         [WEAK]
                EXPORT  KeyscanIntrHandler          [WEAK]
                EXPORT  QdecodeIntrHandler          [WEAK]
                EXPORT  IRIntrHandler               [WEAK]
                EXPORT  SPI2wireIntrHandler         [WEAK]
                EXPORT  AnalogComparatorIntrHandler [WEAK]

                EXPORT  SPIFLASHIntrHandler         [WEAK]
                EXPORT  Timer3IntrHandler           [WEAK]
                EXPORT  Timer4IntrHandler           [WEAK]
                EXPORT  Timer5IntrHandler           [WEAK]
                EXPORT  Timer6IntrHandler           [WEAK]
                EXPORT  Timer7IntrHandler           [WEAK]

                ;EXPORT  Gpio0IntrHandler           [WEAK]
                ;EXPORT  Gpio1IntrHandler           [WEAK]
                EXPORT  Gpio2IntrHandler            [WEAK]
                EXPORT  Gpio3IntrHandler            [WEAK]
                EXPORT  Gpio4IntrHandler            [WEAK]
                EXPORT  Gpio5IntrHandler            [WEAK]
                EXPORT  Gpio6IntrHandler            [WEAK]
                EXPORT  Gpio7IntrHandler            [WEAK]
                EXPORT  Gpio8IntrHandler            [WEAK]
                EXPORT  Gpio9IntrHandler            [WEAK]
                EXPORT  Gpio10IntrHandler           [WEAK]
                EXPORT  Gpio11IntrHandler           [WEAK]
                EXPORT  Gpio12IntrHandler           [WEAK]
                EXPORT  Gpio13IntrHandler           [WEAK]
                EXPORT  Gpio14IntrHandler           [WEAK]
                EXPORT  Gpio15IntrHandler           [WEAK]
                EXPORT  Gpio16IntrHandler           [WEAK]
                EXPORT  Gpio17IntrHandler           [WEAK]
                EXPORT  Gpio18IntrHandler           [WEAK]
                EXPORT  Gpio19IntrHandler           [WEAK]
                EXPORT  Gpio20IntrHandler           [WEAK]
                EXPORT  Gpio21IntrHandler           [WEAK]
                EXPORT  Gpio22IntrHandler           [WEAK]
                EXPORT  Gpio23IntrHandler           [WEAK]
                EXPORT  Gpio24IntrHandler           [WEAK]
                EXPORT  Gpio25IntrHandler           [WEAK]
                EXPORT  Gpio26IntrHandler           [WEAK]
                EXPORT  Gpio27IntrHandler           [WEAK]
                EXPORT  Gpio28IntrHandler           [WEAK]
                EXPORT  Gpio29IntrHandler           [WEAK]
                EXPORT  Gpio30IntrHandler           [WEAK]
                EXPORT  Gpio31IntrHandler           [WEAK]
SysOnIntrHandler
WdgIntrHandler
Timer2IntrHandler
Gpio0IntrHandler
Gpio1IntrHandler
LogUartIntrHandler
DataUartIntrHandler
RTCIntrHandler
Spi0IntrHandler
Spi1IntrHandler
I2C0IntrHandler
I2C1IntrHandler
ADCIntrHandler
PeripheralIntrHandler
Gdma0Ch0IntrHandler
Gdma0Ch1IntrHandler
Gdma0Ch2IntrHandler
Gdma0Ch3IntrHandler
Gdma0Ch4IntrHandler
Gdma0Ch5IntrHandler
KeyscanIntrHandler
QdecodeIntrHandler
IRIntrHandler
SPI2wireIntrHandler
AnalogComparatorIntrHandler

SPIFLASHIntrHandler
Timer3IntrHandler
Timer4IntrHandler
Timer5IntrHandler
Timer6IntrHandler
Timer7IntrHandler

;Gpio0IntrHandler
;Gpio1IntrHandler
Gpio2IntrHandler
Gpio3IntrHandler
Gpio4IntrHandler
Gpio5IntrHandler
Gpio6IntrHandler
Gpio7IntrHandler
Gpio8IntrHandler
Gpio9IntrHandler
Gpio10IntrHandler
Gpio11IntrHandler
Gpio12IntrHandler
Gpio13IntrHandler
Gpio14IntrHandler
Gpio15IntrHandler
Gpio16IntrHandler
Gpio17IntrHandler
Gpio18IntrHandler
Gpio19IntrHandler
Gpio20IntrHandler
Gpio21IntrHandler
Gpio22IntrHandler
Gpio23IntrHandler
Gpio24IntrHandler
Gpio25IntrHandler
Gpio26IntrHandler
Gpio27IntrHandler
Gpio28IntrHandler
Gpio29IntrHandler
Gpio30IntrHandler
Gpio31IntrHandler
                B       .
                ENDP


; User Initial Stack
                EXPORT  __user_initial_stackheap
__user_initial_stackheap PROC
                BX      LR
                ENDP

                ALIGN

                END
