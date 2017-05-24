
/****************************************************************************************************//**
 * @file     rtl876x.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           RTL876X from Realtek Semiconductor.
 *
 * @version  V0.1
 * @date     3. March 2015
 *
 * @note     Generated with SVDConv Vx.xxp 
 *           from CMSIS SVD File 'RTL876X.xml' Version x.xC,
 *
 * @par      Copyright (c) 2015 Realtek Semiconductor. All Rights Reserved. 
 *           
 *           The information contained herein is property of Realtek Semiconductor. 
 *           Terms and conditions of usage are described in detail in Realtek 
 *           SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *           
 *           Licensees are granted free, non-transferable use of the information. NO 
 *           WARRANTY of ANY KIND is provided. This heading must NOT be removed from 
 *           the file. 
 *           
 *
 *******************************************************************************************************/


/** @addtogroup Realtek Semiconductor
  * @{
  */

/** @addtogroup RTL876X
  * @{
  */

#ifndef RTL876X_H
#define RTL876X_H

#include "rtl_types.h"
#include "common_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */
typedef enum {
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
    Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
    NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
    HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
    SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
    PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
    SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* ----------------------  RTL876X Specific Interrupt Numbers  ---------------------- */
    SYSTEM_ON_IRQ                 =  0,               /*!<  0                                                               */
    WDG_IRQ                       =  1,               /*!<  1                                                               */
    TIMER0_IRQ                    =  2,               /*!<  2                                                               */
    TIMER1_IRQ                    =  3,               /*!<  3                                                               */
    TIMER2_IRQ                    =  4,               /*!<  4                                                               */
    BTMAC_BLUEWIZ_IRQ             =  5,               /*!<  5                                                               */
    BTMAC_HCIDMA_IRQ              =  6,               /*!<  6                                                               */
    BTMAC_BZDMA_IRQ               =  7,               /*!<  7                                                               */
    BTMAC_CROSSBAR_IRQ            =  8,               /*!<  8                                                               */
    GPIO0_IRQ                     =  9,               /*!<  9                                                               */
    GPIO1_IRQ                     =  10,              /*!<  10                                                              */
    LOG_UART_IRQ                  =  11,              /*!<  11                                                              */
    UART_IRQ                      =  12,              /*!<  12                                                              */
    RTC_IRQ                       =  13,              /*!<  13                                                              */
    SPI0_IRQ                      =  14,              /*!<  14                                                              */
    SPI1_IRQ                      =  15,              /*!<  15                                                              */
    I2C0_IRQ                      =  16,              /*!<  16                                                              */
    I2C1_IRQ                      =  17,              /*!<  17                                                              */
    ADC_IRQ                       =  18,              /*!<  18                                                              */
    PERIPHERAL_IRQ                =  19,              /*!<  19                                                              */
    GDMA0_CHANNEL0_IRQ            =  20,              /*!<  20                                                              */
    GDMA0_CHANNEL1_IRQ            =  21,              /*!<  21                                                              */
    GDMA0_CHANNEL2_IRQ            =  22,              /*!<  22                                                              */
    GDMA0_CHANNEL3_IRQ            =  23,              /*!<  23                                                              */
    GDMA0_CHANNEL4_IRQ            =  24,              /*!<  24                                                              */
    GDMA0_CHANNEL5_IRQ            =  25,              /*!<  25                                                              */
    KEYSCAN_IRQ                   =  26,              /*!<  26                                                              */
    QDECODE_IRQ                   =  27,              /*!<  27                                                              */
    IR_IRQ                        =  28,              /*!<  28                                                              */
    EFLASH_IRQ                    =  29,              /*!<  29                                                              */
    SPI2WIRE_IRQ                  =  30,              /*!<  30                                                              */
    ANALOG_IRQ                    =  31,               /*!<  31                                                              */

/******  RTL8762 Peripheral Interrupt Numbers ************/
    SPIFLASH_IRQ                    =  64,// 0 + 64,
    GPIO2_IRQ                       =  65,// 1 + 64,
    GPIO3_IRQ                       =  66,// 2 + 64,
    GPIO4_IRQ                       =  67,// 3 + 64,
    GPIO5_IRQ                       =  68,// 4 + 64,
    TIMER3_IRQ                      =  69,// 5 + 64,
    TIMER4_IRQ                      =  70,// 6 + 64,
    TIMER5_IRQ                      =  71,// 7 + 64,
    TIMER6_IRQ                      =  72,// 8 + 64,
    TIMER7_IRQ                      =  73,// 9 + 64,
    GPIO6To31_IRQ                   =  80// 16 + 64,

} IRQn_Type;

#if 1
/*  Temp added - Chuanguo Xue 2015/4/6 */
typedef unsigned int (*IRQ_FUN)();

typedef struct _IRQ_HANDLE_ {
    IRQ_FUN     IrqFun;
    unsigned int      Data;
    IRQn_Type   IrqNum;
    unsigned int      Priority;
    unsigned int      GPIO_Irq_Num;
}IRQ_HANDLE, *PIRQ_HANDLE;

#endif

/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the cm0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0301            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         1            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include <core_cm0.h>                               /*!< Cortex-M0 processor and core peripherals                              */
#include "system_RTL876X.h"                           /*!< RTL876X System                                                          */
#include <stdint.h>

/** @addtogroup Exported_types
  * @{
  */  

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

//typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
/** @} */ /* End of group Exported_types */


/* ================================================================================ */
/* ================    Peripheral Registers Structures Section     ================ */
/* ================================================================================ */


/** @addtogroup Peripheral_Registers_Structures
  * @{
  */

/* ================================================================================ */
/* ================                      UART                      ================ */
/* ================================================================================ */


/**
  * @brief Universal Asynchronous Receiver/Transmitter, version 1.0. (UART)
  */

typedef struct {                                    /*!< UART Structure                                                        */
__IO  uint32_t DLL;
__IO  uint32_t DLH_INTCR;
__IO  uint32_t INTID_FCR;
__IO  uint32_t LCR;
__IO  uint32_t MCR;     //0x10
__I   uint32_t LSR;
__I   uint32_t MSR;
__IO  uint32_t SPR;
__IO  uint32_t STSR;    //0x20
__IO  uint32_t RB_THR;
__IO  uint32_t MISCR;   //may not be seen for client
} UART_TypeDef;

/* ================================================================================ */
/* ================                 2WIRE_SPI                      ================ */
/* ================================================================================ */


/**
  * @brief 2wire spi, mostly used with mouse sensor. (2WIRE_SPI)
  */

typedef struct {                                    /*!< 3WIRE_SPI Structure                                             */
__IO  uint32_t RSVD0[12];                           /*!< 0x00 -- 0x2C for Q-decode                                       */
__IO  uint32_t CFGR;                                 /*!< 0x30                                           */    
__IO  uint32_t CR;
__IO  uint32_t INTCR;
__I   uint32_t SR;
__IO  uint32_t RD0;                                 /*!< 0x40                                           */    
__IO  uint32_t RD1;
__IO  uint32_t RD2;
__IO  uint32_t RD3;
} SPI2WIRE_TypeDef;

/* ================================================================================ */
/* ================                 IR                             ================ */
/* ================================================================================ */


/**
  * @brief IR.
  */

typedef struct {                                    /*!< IR Structure                                             */
__I   uint32_t SR;                                  
__IO  uint32_t MFCR;
__IO  uint32_t INTCR;
__IO  uint32_t CSR1;                                
__IO  uint32_t TFR;                                /*!< 0x10                                           */ 
__IO  uint32_t CSR2;
__IO  uint32_t CR;                                  
__IO  uint32_t RFR;
} IR_TypeDef;

/* ================================================================================ */
/* ================                       SPI                                                      ================ */
/* ================================================================================ */


/**
  * @brief SPI master 0. (SPI)
  */
typedef struct {                                        /*!< SPI Structure                                */
    __IO  uint32_t CTRLR0;                              /*!< 0x00                                           */
    __IO  uint32_t CTRLR1;                              /*!< 0x04                                           */
    __IO  uint32_t SSIENR;                              /*!< 0x08                                           */
    __IO  uint32_t RSVD_0C;                             /*!< 0x0C                                           */
    __IO  uint32_t SER;                                 /*!< 0x10                                           */
    __IO  uint32_t BAUDR;                               /*!< 0x14                                           */
    __IO  uint32_t TXFTLR;                              /*!< 0x18                                           */
    __IO  uint32_t RXFTLR;                              /*!< 0x1C                                           */
    __I  uint32_t TXFLR;                                /*!< 0x20                                           */
    __I  uint32_t RXFLR;                                /*!< 0x24                                           */
    __I  uint32_t SR;                                   /*!< 0x28                                           */
    __IO  uint32_t IMR;                                 /*!< 0x2C                                           */
    __I  uint32_t ISR;                                  /*!< 0x30                                           */
    __I  uint32_t RISR;                                 /*!< 0x34                                           */
    __I  uint32_t TXOICR;                               /*!< 0x38                                           */
    __I  uint32_t RXOICR;                               /*!< 0x3C                                           */
    __I  uint32_t RXUICR;                               /*!< 0x40                                           */
    __I  uint32_t RSVD_44;                              /*!< 0x44                                           */
    __I  uint32_t ICR;                                  /*!< 0x48                                           */
    __IO  uint32_t DMACR;                               /*!< 0x4C                                           */
    __IO  uint32_t DMATDLR;                             /*!< 0x50                                           */
    __IO  uint32_t DMARDLR;                             /*!< 0x54                                           */
    __I  uint32_t IDR;                                  /*!< 0x58                                           */
    __I  uint32_t SSI_COMP_VERSION;                     /*!< 0x5C                                           */
    __IO  uint32_t DR[36];                              /*!< 0x60 - 0xEC                                 */
    __IO  uint32_t RX_SAMPLE_DLY;                       /*!< 0xF0                                           */
}SPI_TypeDef;

/* ================================================================================ */
/* ================                      I2C                      ================= */
/* ================================================================================ */

/**
  * @brief I2C
  */
typedef struct
{
  __IO uint32_t IC_CON;
  __IO uint32_t IC_TAR;
  __IO uint32_t IC_SAR; 
  __IO uint32_t IC_HS_MADDR;
  __IO uint32_t IC_DATA_CMD;
  __IO uint32_t IC_SS_SCL_HCNT;
  __IO uint32_t IC_SS_SCL_LCNT;
  __IO uint32_t IC_FS_SCL_HCNT;
  __IO uint32_t IC_FS_SCL_LCNT;
  __IO uint32_t IC_HS_SCL_HCNT;
  __IO uint32_t IC_HS_SCL_LCNT; 
  __I uint32_t IC_INTR_STAT;
  __IO uint32_t IC_INTR_MASK;
  __I uint32_t IC_RAW_INTR_STAT;   
  __IO uint32_t IC_RX_TL;
  __IO uint32_t IC_TX_TL;
  __I uint32_t IC_CLR_INTR;
  __I uint32_t IC_CLR_RX_UNDER;
  __I uint32_t IC_CLR_RX_OVER;
  __I uint32_t IC_CLR_TX_OVER;
  __I uint32_t IC_CLR_RD_REQ;
  __I uint32_t IC_CLR_TX_ABRT;
  __I uint32_t IC_CLR_RX_DONE;   
  __I uint32_t IC_CLR_ACTIVITY;
  __I uint32_t IC_CLR_STOP_DET;
  __I uint32_t IC_CLR_START_DET;
  __I uint32_t IC_CLR_GEN_CALL;
  __IO uint32_t IC_ENABLE;
  __I uint32_t IC_STATUS;
  __I uint32_t IC_TXFLR;
  __I uint32_t IC_RXFLR;
  __IO uint32_t IC_SDA_HOLD;
  __I uint32_t IC_TX_ABRT_SOURCE;
  __IO uint32_t IC_SLV_DATA_NACK_ONLY;
  __IO uint32_t IC_DMA_CR;  
  __IO uint32_t IC_DMA_TDLR;
  __IO uint32_t IC_DMA_RDLR;
  __IO uint32_t IC_SDA_SETUP; 
  __IO uint32_t IC_ACK_GENERAL_CALL;
  __IO uint32_t IC_ENABLE_STATUS;
} I2C_TypeDef;

/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */


/**
  * @brief Analog to digital converter. (ADC)
  */

typedef struct {                                    /*!< ADC Structure                                                         */
__O  uint32_t FIFO;
__IO uint32_t CR;
__IO uint32_t INTCR;
__O  uint32_t INTSR;
__IO uint32_t CPT01;
__IO uint32_t CPT23;
__IO uint32_t CPCFG;
__IO uint32_t PWR;
__IO uint32_t ANP0;     //0x20
__IO uint32_t ANP1;
__IO uint32_t ANP2;
__IO uint32_t ANP3;
__IO uint32_t ANP4;     //0x30
__IO uint32_t ANP5;
__IO uint32_t ANP6;     
__IO uint32_t LDO;
__IO uint32_t CLBD;
__IO uint32_t CPT45;
__IO uint32_t CPT67;
__IO uint32_t RSVD0[7];
__IO uint32_t CPTTB;    //compare threshould temperature and battery
__IO uint32_t RSVD1[3];
__IO uint32_t TMPD;
__IO uint32_t BATD;
__IO uint32_t CH0D;
__IO uint32_t CH1D;
__IO uint32_t CH2D;
__IO uint32_t CH3D;
__IO uint32_t CH4D;
__IO uint32_t CH5D;
__IO uint32_t CH6D;
__IO uint32_t CH7D;     /*!< 0xA8                                   */
} ADC_TypeDef;

/* ================================================================================ */
/* ================                 CODEC                          ================ */
/* ================================================================================ */

/**
  * @brief IR.
  */

typedef struct {                                    /*!< CODEC Structure                            */
      uint32_t RSVD0[20];
__IO  uint32_t RESET;                               /*!< 0x50                                   */  
__IO  uint32_t CR0;
__IO  uint32_t CR1;
__IO  uint32_t CR2;
__IO  uint32_t CR3;                                 /*!< 0x60                                   */
__IO  uint32_t CR4;
      uint32_t RSVD1[1];
__IO  uint32_t CR5;                                 /*!< 0x6C                                  */
} CODEC_TypeDef;


/* ================================================================================ */
/* ================                      TIM                      ================ */
/* ================================================================================ */

/**
  * @brief TIM
  */
typedef struct {                                    
    __IO uint32_t LoadCount;                        /*!< 0x00*/
    __I  uint32_t CurrentValue;                     /*!< 0x04*/
    __IO uint32_t ControlReg;                       /*!< 0x08*/
    __I  uint32_t EOI;                              /*!< 0x0C*/
    __I  uint32_t IntStatus;                        /*!< 0x10*/
} TIM_TypeDef;

/* ================================================================================ */
/* ================                      GDMA                      ================ */
/* ================================================================================ */

/**
  * @brief GDMA
  */
typedef struct
{
    __I uint32_t RAW_TFR;
    uint32_t RSVD0;
    __I uint32_t RAW_BLOCK;
    uint32_t RSVD1;
    __I uint32_t RAW_SRC_TRAN;
    uint32_t RSVD2;
    __I uint32_t RAW_DST_TRAN;
    uint32_t RSVD3;
    __I uint32_t RAW_ERR;
    uint32_t RSVD4;
        
    __I uint32_t STATUS_TFR;
    uint32_t RSVD5;
    __I uint32_t STATUS_BLOCK;
    uint32_t RSVD6;
    __I uint32_t STATUS_SRC_TRAN;
    uint32_t RSVD7;
    __I uint32_t STATUS_DST_TRAN;
    uint32_t RSVD8;
    __I uint32_t STATUS_ERR;
    uint32_t RSVD9;

    __IO uint32_t MASK_TFR;
    uint32_t RSVD10;
    __IO uint32_t MASK_BLOCK;
    uint32_t RSVD11;
    __IO uint32_t MASK_SRC_TRAN;
    uint32_t RSVD12;
    __IO uint32_t MASK_DST_TRAN;
    uint32_t RSVD13;
    __IO uint32_t MASK_ERR;
    uint32_t RSVD14;

    __O uint32_t CLEAR_TFR;
    uint32_t RSVD15;
    __O uint32_t CLEAR_BLOCK;
    uint32_t RSVD16;
    __O uint32_t CLEAR_SRC_TRAN;
    uint32_t RSVD17;
    __O uint32_t CLEAR_DST_TRAN;
    uint32_t RSVD18;
    __O uint32_t CLEAR_ERR;
    uint32_t RSVD19;
    __O uint32_t StatusInt;
    uint32_t RSVD191;

    __IO uint32_t ReqSrcReg;
    uint32_t RSVD20;
    __IO uint32_t ReqDstReg;
    uint32_t RSVD21;
    __IO uint32_t SglReqSrcReg;
    uint32_t RSVD22;
    __IO uint32_t SglReqDstReg;
    uint32_t RSVD23;
    __IO uint32_t LstSrcReg;
    uint32_t RSVD24;
    __IO uint32_t LstDstReg;
    uint32_t RSVD25;
        
    __IO uint32_t DmaCfgReg;
    uint32_t RSVD26;
    __IO uint32_t ChEnReg;
    uint32_t RSVD27;
    __I uint32_t DmaIdReg;
    uint32_t RSVD28;
    __IO uint32_t DmaTestReg;
    uint32_t RSVD29;
} GDMA_TypeDef;

typedef struct
{
    __IO uint32_t SAR;
    uint32_t RSVD0;
    __IO uint32_t DAR;
    uint32_t RSVD1;
    __IO uint32_t LLP;
    uint32_t RSVD2;
    __IO uint32_t CTL_LOW;
    __IO uint32_t CTL_HIGH;
    __IO uint32_t SSTAT;
    uint32_t RSVD4;
    __IO uint32_t DSTAT;
    uint32_t RSVD5;
    __IO uint32_t SSTATAR;
    uint32_t RSVD6;
    __IO uint32_t DSTATAR;
    uint32_t RSVD7;
    __IO uint32_t CFG_LOW;
    __IO uint32_t CFG_HIGH;
    __IO uint32_t SGR;
    uint32_t RSVD9;
    __IO uint32_t DSR;
    uint32_t RSVD10;
} GDMA_ChannelTypeDef;

/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real time counter 0. (RTC)
  */

typedef struct {                                    /*!< RTC Structure                                                         */
__IO uint32_t CR0;                                                          /*!< 0x00                                           */
__IO uint32_t CR1;                                                          /*!< 0x04                                           */
__IO uint32_t SR;                                                           /*!< 0x08                                           */
__IO uint32_t RTC_PRESCALER;                                                /*!< 0x0C                                           */
__IO uint32_t COMP0;                                                        /*!< 0x10                                           */
__IO uint32_t COMP1;                                                        /*!< 0x14                                           */
__IO uint32_t COMP2;                                                        /*!< 0x18                                           */
__IO uint32_t COMP3;                                                        /*!< 0x1C                                           */
__IO uint32_t LPCOMP_CMP;                                                   /*!< 0x20                                           */
__I  uint32_t CNT;                                                          /*!< 0x24                                           */
__I  uint32_t LPCOMP_CNT;                                                   /*!< 0x28                                           */
__IO uint32_t SR_OFF;                                                       /*!< 0x2C                                           */
} RTC_TypeDef;


/* ================================================================================ */
/* ================                      QDEC                      ================ */
/* ================================================================================ */

/**
  * @brief Rotary decoder. (QDEC)
  */

typedef struct {                                    /*!< QDEC Structure           */
__I  uint32_t REG_SR_X;                                                         /*!< 0x00                                           */
__I  uint32_t REG_SR_Y;                                                         /*!< 0x04                                           */
__I  uint32_t REG_SR_Z;                                                         /*!< 0x08                                           */
__IO uint32_t REG_CR0;                                                          /*!< 0x0C                                           */
__IO uint32_t REG_CR1;                                                          /*!< 0x10                                           */
__IO uint32_t REG_CR2;                                                          /*!< 0x14                                           */
     uint32_t RSVD0[4];                                                         /*!< Reserved                                   */
__IO uint32_t REG_DIVNUM;                                                       /*!< 0x28                                           */
} QDEC_TypeDef;


/* ================================================================================ */
/* ================            System Block Control            ================ */
/* ================================================================================ */


/**
  * @brief System Block Control. (SYS_BLKCTRL)
  */

typedef struct {                                    /*!< SYS_BLKCTRL Structure      */
__IO uint32_t SYS_CLK_SEL;                                          /*!< 0x200                                          */
     uint32_t RSVD0[3];                                                         /*!< Reserved                                       */
__IO uint32_t SOC_FUNC_EN;                                          /*!< 0x210                                          */
     uint32_t RSVD1;                                                                /*!< Reserved                                       */
__IO uint32_t PERI_FUNC0_EN;                                /*!< 0x218                                          */
__IO uint32_t PERI_FUNC1_EN;                                /*!< 0x21C                                          */
     uint32_t RSVD2[4];                                                         /*!< Reserved                                       */
__IO uint32_t CLK_CTRL;                                     /*!< 0x230                                          */
__IO uint32_t PERI_CLK_CTRL0;                           /*!< 0x234                                          */
__IO uint32_t PERI_CLK_CTRL1;                           /*!< 0x238                                          */
     uint32_t RSVD3[2];                                                         /*!< Reserved                                       */
__IO uint32_t COM_CLK_CTRL1;                            /*!< 0x244                                          */
} SYS_BLKCTRL_TypeDef;


/* ================================================================================ */
/* ================                     Pinmux                     ================ */
/* ================================================================================ */


/**
  * @brief Pinmux. (Pinmux)
  */

typedef struct {                                    /*!< Peripheral Structure                       */
__IO uint32_t CFG[10];                              /*!<                                            */
} PINMUX_TypeDef;


/* ================================================================================ */
/* ================                   Peripheral                   ================ */
/* ================================================================================ */


/**
  * @brief Peripheral. (Peripheral)
  */

typedef struct {                                    /*!< Peripheral Structure                                                        */
uint32_t RSVD0[134];                                /*!< Reserved                                       */
__IO uint32_t PERI_FUNC0_EN;                        /*!< 0x218                                          */
__IO uint32_t PERI_FUNC1_EN;                        /*!< 0x21C                                          */
__IO uint32_t PERI_BD_FUNC0_EN;                     /*!< 0x220                                          */
uint32_t RSVD1[3];                                  /*!< Reserved                                       */
__IO uint32_t PERI_CLK_CTRL;                        /*!< 0x230                                          */
__IO uint32_t PERI_CLK_CTRL0;                       /*!< 0x234                                          */
__IO uint32_t PERI_CLK_CTRL1;                       /*!< 0x238                                          */
} PHERIPHERIAL_TypeDef;


/* ================================================================================ */
/* ================              Peripheral Interrupt              ================ */
/* ================================================================================ */


/**
  * @brief Peripheral Interrupt. (Peripheral Interrupt)
  */

typedef struct {                                    /*!< Peripheral Structure                          */
__IO uint32_t RESERVED;                             /*!< 0x00                                          */
__IO uint32_t STATUS;                               /*!< 0x04                                          */
__IO uint32_t MODE;                                 /*!< 0x08                                          */
__IO uint32_t EN;                                   /*!< 0x0C                                          */
} PERI_INT_TypeDef;

/* ================================================================================ */
/* ================                    Key Scan                    ================ */
/* ================================================================================ */


/**
  * @brief Key Scan. (KeyScan)
  */

typedef struct {                                    /*!< Key Scan Structur                                  */
__IO uint32_t CR0;                                  /*!< Control register 0                                 */
__IO uint32_t CR1;                                  /*!< Control register 1                                 */
__IO uint32_t CR2;                                  /*!< Control register 2                                 */
__IO uint32_t INTCR;                                /*!< Control register 0                                 */
__I  uint32_t FIFODATA;                             /*!< FIFO data register                                 */
     uint32_t Reserved0[2];                         /*!< Reserved                                           */
__I  uint32_t STATUS;                               /*!< FIFO status register                               */
__I  uint32_t RTLVER;                               /*!< RTL version register                               */
__IO uint32_t DEG;                                  /*!< Debug Mode register                                */
} KEYSCAN_TypeDef;

/* ======================================================== */
/* ================                      GPIO                       ================ */
/* ======================================================== */

/**
  * @brief General purpose input and output. (GPIO)
  */

typedef struct 
{                                
    __IO uint32_t DATAOUT;                              /*!< Data register: data output             */
    __IO uint32_t DATADIR;                              /*!< Data direction register                    */
    __IO uint32_t DATASRC;                              /*!< Data source register                   */
        uint32_t RSVD0[9];                              /*!< Reserved                           */
    __IO uint32_t INTEN;                                /*!< Interrupt enable register              */
    __IO uint32_t INTMASK;                              /*!< Interrupt mask register                */
    __IO uint32_t INTTYPE;                              /*!< Interrupt level register                   */
    __IO uint32_t INTPOLARITY;                          /*!< Interrupt polarity register                */
    __IO uint32_t INTSTATUS;                            /*!< Interrupt status of Port A             */
    __IO uint32_t RAWINTSTATUS;                         /*!< Raw interrupt status of Port A (premasking) */
    __IO uint32_t DEBOUNCE;                             /*!< Debounce enable register               */
    __O  uint32_t INTCLR;                               /*!< clear interrupt register                   */
    __I  uint32_t DATAIN;                               /*!< external port register                 */
         uint32_t RSVD1[3];                             /*!< Reserved                           */
    __IO uint32_t LSSYNC;                               /*!< Level-sensitive synchronization enable register*/
    __I  uint32_t IDCODE;                               /*!< ID code register                       */
         uint32_t RSVD2;                                /*!< Reserved                           */
    __IO uint32_t VERIDCODE;                            /*!< Component Version register             */
    __I  uint32_t CONFIG2;                              /*!< Configuration Register 2               */
    __I  uint32_t CONFIG1;                              /*!< Configuration Register 1               */
} GPIO_TypeDef;

/* ======================================================= */
/* ================                      PWM                     ================ */
/* ======================================================= */

/**
  * @brief PWM
  */
typedef struct {                                    
    __IO uint32_t CR0;                  /*!< 0x00*/
    __IO  uint32_t CR1;                 /*!< 0x04*/
}PWM_TypeDef;

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
#define SYSTEM_REG_BASE             0x40000000UL
#define PERIPH_REG_BASE             0x40000000UL
#define VENDOR_REG_BASE             0x40008000UL
#define PERI_INT_REG_BASE           0x40008004UL
#define SYSBLKCTRL_REG_BASE         0x40000200UL
#define PINMUX_REG_BASE             0x40000280UL
#define GPIO_REG_BASE               0x40001000UL
#define RTC_REG_BASE                0x40000100UL
#define TIM0_REG_BASE               0x40002000UL
#define TIM1_REG_BASE               0x40002014UL
#define TIM2_REG_BASE               0x40002028UL
#define TIM3_REG_BASE               0x4000203CUL
#define TIM4_REG_BASE               0x40002050UL
#define TIM5_REG_BASE               0x40002064UL
#define TIM6_REG_BASE               0x40002078UL
#define TIM7_REG_BASE               0x4000208CUL
#define PWM0_REG_BASE               0x40000364UL
#define PWM1_REG_BASE               0x4000036CUL
#define PWM2_REG_BASE               0x40000374UL
#define PWM3_REG_BASE               0x4000037CUL
#define GDMA_CHANNEL_REG_BASE       0x40060000UL
#define GDMA_REG_BASE               (GDMA_CHANNEL_REG_BASE + 0x2c0)
#define GDMA_Channel0_BASE          (GDMA_CHANNEL_REG_BASE + 0x0000)
#define GDMA_Channel1_BASE          (GDMA_CHANNEL_REG_BASE + 0x0058)
#define GDMA_Channel2_BASE          (GDMA_CHANNEL_REG_BASE + 0x00b0)
#define GDMA_Channel3_BASE          (GDMA_CHANNEL_REG_BASE + 0x0108)
#define GDMA_Channel4_BASE          (GDMA_CHANNEL_REG_BASE + 0x0160)
#define GDMA_Channel5_BASE          (GDMA_CHANNEL_REG_BASE + 0x01b8)
#define GDMA_Channel6_BASE          (GDMA_CHANNEL_REG_BASE + 0x0210)
#define QDEC_REG_BASE               0x40004000UL
#define SPI2WIRE_REG_BASE           0x40004000UL
#define CODEC_REG_BASE              0x40010000UL
#define KEYSCAN_REG_BASE            0x40006000UL
#define ADC_REG_BASE                0x40010000UL
#define UART_REG_BASE               0x40040000UL
#define IR_REG_BASE                 0x40041000UL
#define RTL_SPI0_BASE               0x40042000UL
#define RTL_SPI1_BASE               0x40042400UL
#define I2C0_REG_BASE               0x40044000UL
#define I2C1_REG_BASE               0x40044400UL
#define SPI0_REG_BASE               0x40042000UL
#define SPI1_REG_BASE               0x40042400UL
#define BT_BB_REG_BASE              0x40050000UL
#define BT_VENDOR_REG_BASE          0x40058000UL
#define GDMA0_REG_BASE              0x40060000UL
#define HCI_DMA_REG_BASE            0x40064000UL
#define HCI_UART_REG_BASE           0x40068000UL


#define REG_SOC_PERI_FUNC1_EN       0x21C
#define REG_PEON_SYS_CLK_SEL        0x0200
#define REG_PESOC_CLK_CTRL          0x0230
#define REG_SOC_PERI_FUNC1_EN       0x21C

#define SPI0_MASTER_MODE_REG *((volatile uint32_t *)0x40000308UL)
#define SPI0_MASTER_MODE_BIT BIT(8)


#define REG_PEON_SYS_CLK_SEL        0x0200
#define REG_SOC_FUNC_EN             0x0210
#define REG_SOC_HCI_COM_FUNC_EN     0x0214
#define REG_SOC_PERI_FUNC0_EN       0x0218
#define REG_PESOC_PERI_CLK_CTRL0    0x0234
#define REG_PESOC_PERI_CLK_CTRL1    0x0238

#define GPIO_OUTPUT_OFFSET          0x00
#define GPIO_DIRECTION_OFFSET       0x04

//AON PAD AREA
#define REG_PAD_WKEN_ADDRESS        0x20
#define REG_PAD_WK_CTRL_ADDRESS     0x24
#define REG_PAD_WKPOL_ADDRESS       0x25
#define REG_PAD_O_ADDRESS           0x2A
#define REG_AON_PAD_E_ADDRESS       0x2F
#define REG_AON_PAD_S_ADDRESS       0x34
#define REG_AON_PAD_PU_ADDRESS      0x39
#define REG_AON_PAD_PD_ADDRESS      0x3E
#define REG_AON_PAD_PWRON_ADDRESS   0x4C

#define BIT_WK_INTEN                BIT(7)
#define BIT_WK_FLAG_GPIO            BIT(6)

#define BIT_SOC_ACTCK_GPIO_EN       BIT(24)
#define BIT_PERI_GPIO_EN            BIT(8) 

#define BIT_PERI_KEYSCAN_EN         BIT(19)
#define BIT_SOC_ACTCK_KEYSCAN_EN    BIT(6)

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
/* System */
#define PERIPH                          ((PHERIPHERIAL_TypeDef  *) PERIPH_REG_BASE)
#define SYSBLKCTRL                      ((SYS_BLKCTRL_TypeDef   *) SYSBLKCTRL_REG_BASE)
#define PERIPHINT                       ((PERI_INT_TypeDef      *) VENDOR_REG_BASE)

/* IO */
#define PINMUX                          ((PINMUX_TypeDef        *) PINMUX_REG_BASE)
#define KEYSCAN                         ((KEYSCAN_TypeDef       *) KEYSCAN_REG_BASE)
#define GPIO                            ((GPIO_TypeDef          *) GPIO_REG_BASE)
#define QDEC                            ((QDEC_TypeDef          *) QDEC_REG_BASE)
#define I2C0                            ((I2C_TypeDef           *) I2C0_REG_BASE)
#define I2C1                            ((I2C_TypeDef           *) I2C1_REG_BASE)
#define SPI0                            ((SPI_TypeDef           *) SPI0_REG_BASE)
#define SPI1                            ((SPI_TypeDef           *) SPI1_REG_BASE)
#define TIM0                            ((TIM_TypeDef           *) TIM0_REG_BASE)
#define TIM1                            ((TIM_TypeDef           *) TIM1_REG_BASE)
#define TIM2                            ((TIM_TypeDef           *) TIM2_REG_BASE)
#define TIM3                            ((TIM_TypeDef           *) TIM3_REG_BASE)
#define TIM4                            ((TIM_TypeDef           *) TIM4_REG_BASE)
#define TIM5                            ((TIM_TypeDef           *) TIM5_REG_BASE)
#define TIM6                            ((TIM_TypeDef           *) TIM6_REG_BASE)
#define TIM7                            ((TIM_TypeDef           *) TIM7_REG_BASE)
#define PWM0                            ((PWM_TypeDef           *) PWM0_REG_BASE)
#define PWM1                            ((PWM_TypeDef           *) PWM1_REG_BASE)
#define PWM2                            ((PWM_TypeDef           *) PWM2_REG_BASE)
#define PWM3                            ((PWM_TypeDef           *) PWM3_REG_BASE)
#define GDMA_BASE                       ((GDMA_TypeDef *) GDMA_REG_BASE)
#define GDMA_Channel0                   ((GDMA_ChannelTypeDef *) GDMA_Channel0_BASE)
#define GDMA_Channel1                   ((GDMA_ChannelTypeDef *) GDMA_Channel1_BASE)
#define GDMA_Channel2                   ((GDMA_ChannelTypeDef *) GDMA_Channel2_BASE)
#define GDMA_Channel3                   ((GDMA_ChannelTypeDef *) GDMA_Channel3_BASE)
#define GDMA_Channel4                   ((GDMA_ChannelTypeDef *) GDMA_Channel4_BASE)
#define GDMA_Channel5                   ((GDMA_ChannelTypeDef *) GDMA_Channel5_BASE)
#define GDMA_Channel6                   ((GDMA_ChannelTypeDef *) GDMA_Channel6_BASE)
#define ADC                             ((ADC_TypeDef             *) ADC_REG_BASE)
#define UART                            ((UART_TypeDef            *) UART_REG_BASE)
#define SPI2WIRE                        ((SPI2WIRE_TypeDef        *) SPI2WIRE_REG_BASE)
#define IR                              ((IR_TypeDef              *) IR_REG_BASE)
#define CODEC                           ((CODEC_TypeDef           *) CODEC_REG_BASE)
#define RTC                             ((RTC_TypeDef             *) RTC_REG_BASE)


/** @} */ /* End of group Peripheral_Registers_Structures */
/** @} */ /* End of group RTL876X */
/** @} */ /* End of group Realtek Semiconductor */


//Add by Vendor
#define HAL_READ32(base, addr)            \
        rtk_le32_to_cpu(*((volatile UINT32*)(base + addr)))
    
#define HAL_WRITE32(base, addr, value32)  \
        ((*((volatile UINT32*)(base + addr))) = rtk_cpu_to_le32(value32))

#define HAL_READ16(base, addr)            \
        rtk_le16_to_cpu(*((volatile UINT16*)(base + addr)))
        
#define HAL_WRITE16(base, addr, value)  \
        ((*((volatile UINT16*)(base + addr))) = rtk_cpu_to_le16(value))
    

#define HAL_READ8(base, addr)            \
        (*((volatile UINT8*)(base + addr)))
            
#define HAL_WRITE8(base, addr, value)  \
        ((*((volatile UINT8*)(base + addr))) = value)


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
//#define USE_FULL_ASSERT   1

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void io_assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif


#endif  /* RTL876X_H */

