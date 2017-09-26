
                PRESERVE8
                THUMB
                    
;Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     reset_handler             ; reset Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved 
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved 
                DCD     0                         ; Reserved 

                ; External Interrupts
                DCD 0
                DCD 0
                DCD UART0_int_IRQHandler
                DCD UART1_int_IRQHandler
                DCD SPI0_int_IRQHandler  
                DCD SPI1_int_IRQHandler  
                DCD DMA_int_IRQ0Handler  
                DCD DMA_int_IRQ1Handler  
                DCD I2S_int_IRQHandler   
                DCD I2C0_int_IRQHandler  
                DCD I2C1_int_IRQHandler  
                DCD _7816_int_IRQHandler 
                DCD 0   
                DCD 0   
                DCD TMR0_int_IRQHandler  
                DCD TMR1_int_IRQHandler  
                DCD TMR2_int_IRQHandler  
                DCD GPIO_0_int_IRQHandler 
                DCD GPIO_1_int_IRQHandler  
                DCD GPIO_2_int_IRQHandler                
                DCD KEYBOARD_int_IRQHandler                 
                DCD 0                  
                DCD 0                     
                DCD 0                    
                DCD 0                 
                DCD 0                   
                DCD 0                   
                DCD 0                   
                DCD 0                   
                DCD 0                 
                DCD 0                    
                DCD 0            


__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors                    


                AREA    |.text|, CODE, READONLY

; Reset Handler
                
reset_handler   PROC
                EXPORT  reset_handler             [WEAK]
                IMPORT  __main
                LDR     R0, =__main
                BLX     R0
                ENDP   

Default_Handler PROC

    EXPORT UART0_int_IRQHandler [WEAK]
    EXPORT UART1_int_IRQHandler [WEAK]
    EXPORT SPI0_int_IRQHandler [WEAK]
    EXPORT SPI1_int_IRQHandler [WEAK]
    EXPORT DMA_int_IRQ0Handler [WEAK]
    EXPORT DMA_int_IRQ1Handler [WEAK]
    EXPORT I2S_int_IRQHandler [WEAK]
    EXPORT I2C0_int_IRQHandler [WEAK]
    EXPORT I2C1_int_IRQHandler [WEAK]
    EXPORT _7816_int_IRQHandler [WEAK]
    EXPORT TMR0_int_IRQHandler [WEAK]
    EXPORT TMR1_int_IRQHandler [WEAK]
    EXPORT TMR2_int_IRQHandler [WEAK]
    EXPORT GPIO_0_int_IRQHandler [WEAK]
    EXPORT GPIO_1_int_IRQHandler [WEAK]
    EXPORT GPIO_2_int_IRQHandler [WEAK]
    EXPORT KEYBOARD_int_IRQHandler [WEAK]


UART0_int_IRQHandler
UART1_int_IRQHandler
SPI0_int_IRQHandler
SPI1_int_IRQHandler
DMA_int_IRQ0Handler
DMA_int_IRQ1Handler
I2S_int_IRQHandler
I2C0_int_IRQHandler
I2C1_int_IRQHandler
_7816_int_IRQHandler
TMR0_int_IRQHandler
TMR1_int_IRQHandler
TMR2_int_IRQHandler
GPIO_0_int_IRQHandler
GPIO_1_int_IRQHandler
GPIO_2_int_IRQHandler
KEYBOARD_int_IRQHandler



                B .
                ENDP

 ;User Initial Stack
                EXPORT  __user_initial_stackheap
__user_initial_stackheap
				BX      LR

                ALIGN

                END               
