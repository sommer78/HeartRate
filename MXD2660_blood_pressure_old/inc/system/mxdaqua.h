/**
  ******************************************************************************
  * @file    mxdaqua.h
  * @author  wenjie.li
  * @version V1.0
  * @date    14-Jan-2016
  * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer Header File.
  *          This file contains all the peripheral register's definitions, bits
  *          definitions and memory mapping for Aqua.
  *
  *          The file is the unique include file that the application programmer
  *          is using in the C source code, usually in main.c.
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *           - Macros to access peripheral’s registers hardware
  *
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
  */


#ifndef __MXDAQUA_H_
#define __MXDAQUA_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Library_configuration_section
  * @{
  */


/**
 * @brief In the following line adjust the value of External High Speed oscillator (HSE)
   used in your application

   Tip: To avoid modifying this file each time you need to use different HSE, you
        can define the HSE value in your toolchain compiler preprocessor.
  */
#if !defined  HSE_VALUE
#define HSE_VALUE    ((uint32_t)16000000) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */


/**
 * @brief In the following line adjust the External High Speed oscillator (HSE) Startup
   Timeout value
   */
#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x0500) /*!< Time out for HSE start up */

#define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/

/**
 * @brief mxdaqua Standard Peripheral Library version number
   */
#define __MXDAQUA_STDPERIPH_VERSION_MAIN   (0x03) /*!< [31:24] main version */
#define __MXDAQUA_STDPERIPH_VERSION_SUB1   (0x05) /*!< [23:16] sub1 version */
#define __MXDAQUA_STDPERIPH_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __MXDAQUA_STDPERIPH_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __MXDAQUA_STDPERIPH_VERSION       ( (__MXDAQUA_STDPERIPH_VERSION_MAIN << 24)\
                                             |(__MXDAQUA_STDPERIPH_VERSION_SUB1 << 16)\
                                             |(__MXDAQUA_STDPERIPH_VERSION_SUB2 << 8)\
                                             |(__MXDAQUA_STDPERIPH_VERSION_RC))

/**
  * @}
  */

/** @addtogroup Configuration_section_for_CMSIS
  * @{
  */

/**
 * @brief Configuration of the Cortex-M0+ Processor and Core Peripherals
 */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used */

/**
 * @brief STM32F10x Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
    /* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
    NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt          */
    HardFault_IRQn                = -13,      /*!<  3 HardFault Interrupt             */



    SVCall_IRQn                   =  -5,      /*!< 11 SV Call Interrupt               */

    PendSV_IRQn                   =  -2,      /*!< 14 Pend SV Interrupt               */
    SysTick_IRQn                  =  -1,      /*!< 15 System Tick Interrupt           */

    /* ----------------------  ARMCM0 Specific Interrupt Numbers  --------------------- */
    PMU_IRQn                    =   0,      /**< PMU interrupt        */
    BLE_LL_IRQn                 =   1,      /*!< LLC Interrupt           */
    UART0_IRQn                  =   2,      /*!< UART0 Interrupt          */
    UART1_IRQn                  =   3,      /*!< UART1 Interrupt          */
    SPI0_IRQn                   =   4,      /*!< SPI0 Interrupt          */
    SPI1_IRQn                   =   5,      /*!< SPI1 Interrupt                     */
    DMA_IRQ0n                   =   6,      /*!< DMA0 Interrupt                     */
    DMA_IRQ1n                   =   7,      /*!< DMA1 Interrupt                    */
    I2S_IRQn                    =   8,      /*!< I2S Interrupt                    */
    I2C0_IRQn                   =   9,      /*!< I2C0 Interrupt                    */
    I2C1_IRQn                   =   10,      /*!< I2C1 Interrupt                    */
    _7816_IRQn                  =   11,      /*!< 7816 Interrupt              */
    RESERVED0_IRQn              =   12,      /*!< reserved Interrupt            */
    RTC_IRQn                    =   13,      /*!< RTC  Interrupt                 */
    TMR0_IRQn                   =   14,      /*!< TMR0  Interrupt               */
    TMR1_IRQn                   =   15,      /*!< TMR1 Interrupt      */
    TMR2_IRQn                   =   16,      /*!< TMR2 Interrupt            */
    GPIO_0_IRQn                 =   17,      /*!< GPIO0 Interrupt                  */
    GPIO_1_IRQn                 =   18,      /*!< GPIO1 Interrupt                      */
    GPIO_2_IRQn                 =   19,      /*!< GPIO2 Interrupt                      */
    KB_IRQn                     =   20,      /*!< GPIO3 Interrupt              */
    CRYPT_IRQn                  =   21,      /*!< crypt  Interrupt        */
    RESERVED1_IRQn              =   22,      /*!< reserved  Interrupt                    */
    RESERVED2_IRQn              =   23,      /*!< reserved  Interrupt          */
    RESERVED3_IRQn              =   24,      /*!< reserved  Interrupt          */
    SWI0_IRQn                   =   25,      /*!< SWI0_IRQn   Interrupt           */
    SWI1_IRQn                   =   26,      /*!< SWI1_IRQn   Interrupt           */
    SWI2_IRQn                   =   27,      /*!< SWI2_IRQn   Interrupt           */
    SWI3_IRQn                   =   28,      /*!< SWI3_IRQn   Interrupt           */
    RESERVED4_IRQn              =   29,     /*!< reserved  Interrupt        */
    RESERVED5_IRQn              =   30,     /*!< reserved  Interrupt        */
    RESERVED6_IRQn              =   31,     /*!< reserved  Interrupt        */
//  irq_pad3n                    =  31    /*!< irq_pad3  Interrupt        */

} IRQn_Type;

/**
  * @}
  */

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the Cortex-M4 Processor and Core Peripherals  ------- */
#define __CM0PLUS_REV             0x0000    /*!< Core revision r0p0                              */
#define __MPU_PRESENT             0         /*!< MPU present or not                              */
#define __VTOR_PRESENT            0         /*!< VTOR present or not                             */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels         */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used    */

#include <core_cm0plus.h>                   /* Processor and core peripherals                    */
#include "system_ARMCM0plus.h"              /* System Header                                     */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language=extended
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning 586
#else
#warning Not supported compiler type
#endif





/* ================================================================================ */
/* ================                   Timer (TIM)                  ================ */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t Timer1Load;       /* Offset: 0x000 (R/W)  Timer 1 Load */
    __I  uint32_t Timer1Value;      /* Offset: 0x004 (R/ )  Timer 1 Counter Current Value */
    __IO uint32_t Timer1Control;    /* Offset: 0x008 (R/W)  Timer 1 Control */
    __O  uint32_t Timer1IntClr;     /* Offset: 0x00C ( /W)  Timer 1 Interrupt Clear */
    __I  uint32_t Timer1RIS;        /* Offset: 0x010 (R/ )  Timer 1 Raw Interrupt Status */
    __I  uint32_t Timer1MIS;        /* Offset: 0x014 (R/ )  Timer 1 Masked Interrupt Status */
    __IO uint32_t Timer1BGLoad;     /* Offset: 0x018 (R/W)  Background Load Register */
    uint32_t RESERVED0[1];
} TIM_TypeDef;


/* ================================================================================ */
/* ============== Universal Asyncronous Receiver / Transmitter (UART) ============= */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t UART_FIFO_WR;               /* Offset: 0x000 (W)  Tx FIFO write */
    __IO uint32_t UART_FIFO_RD;               /* offset: 0x4 (R) Rx FIFO read*/
    __IO uint32_t UART_INT_STATE;             /* offset: 0x8 (R) uart interruption status*/
    __IO uint32_t UART_INT_MASK;              /* offset: 0xc (R) uart interruption mask*/
    __IO uint32_t UART_INT_CLR;               /* offset: 0x10 (W) uart bit by bit clear*/
    __IO uint32_t UART_CONFIG;                /* offset: 0x14 (R/W) uart param config*/
    __IO uint32_t UART_DIVISOR;               /* offset: 0x18 (R/W) uart trig_intv/cap_intv*/
    __IO uint32_t UART_FIFO_CNT;              /* offset: 0x1c (R) uart fifo count*/
    __IO uint32_t UART_FIFO_CLR;              /* offset: 0x20 fifo clean*/
    __IO uint32_t UART_RFIFO_VALID;

} UART_TypeDef;


typedef struct
{
	__IO uint32_t SPI_FIFO_WR; 			  /* Offset: 0x000 (W)	Tx FIFO write */ 
	__IO uint32_t SPI_FIFO_RD;				/* offset: 0x4 (R) Rx FIFO read*/
	__IO uint32_t SPI_INT_STATE; 			/* offset: 0x8 (R) spi  interruption status*/
	__IO uint32_t SPI_INT_MASK;				/* offset: 0xc (R) spi  interruption mask*/
	__IO uint32_t SPI_INT_CLR;				/* offset: 0x10 (W) spi  bit by bit clear*/
	__IO uint32_t SPI_CONFIG;				/* offset: 0x14 (R/W) spi  param config*/
	__IO uint32_t SPI_DIVISOR;				/* offset: 0x18 (R/W) spi  trig_intv/cap_intv*/
	__IO uint32_t SPI_FIFO_CNT;				/* offset: 0x1c (R) spi  fifo count*/
	__IO uint32_t SPI_IDLE_DATA;			/* offset: 0x20 (W) spi tx idle data*/
	__IO uint32_t SPI_FIFO_CLR;				/* offset: 0x24 (R) spi  fifo count clear*/
}SPI_TypeDef;

typedef struct
{
	__IO uint32_t tfifo_wr_data;
	__IO uint32_t rfifo_rd_data;
	__IO uint32_t int_status;
	__IO uint32_t int_mask;
	__IO uint32_t int_clr;
	__IO uint32_t i2cm_cr;
	__IO uint32_t i2c_config;	
	__IO uint32_t i2cm_cfg;
	__IO uint32_t i2cm_tar;
	__IO uint32_t i2cs_sar;
	__IO uint32_t fifo_cnt;
	__IO uint32_t transfer_len;
	__IO uint32_t tfifo_idle_data;
	__IO uint32_t i2c_bus_status;
	__IO uint32_t fifo_clr;
}IIC_TypeDef;
typedef struct
{
	__IO uint32_t int_status;
	__IO uint32_t int_mask;
	__IO uint32_t int_clr;
	__IO uint32_t trig_cfg;
	__IO uint32_t prescale;
	__IO uint32_t comp0_hi;
	__IO uint32_t comp0_low;
	__IO uint32_t comp1_hi;
	__IO uint32_t comp1_low;
	__IO uint32_t comp2_hi;
	__IO uint32_t comp2_low;
	__IO uint32_t comp3_hi;
	__IO uint32_t comp3_low;
	__IO uint32_t trig_comp;
	__IO uint32_t cnt_hi;
	__IO uint32_t cnt_low;
}RTC_TypeDef;
typedef struct 
{
	__IO uint32_t int_status;
	__IO uint32_t int_mask;
	__IO uint32_t int_clear;
	__IO uint32_t trig_cfg;
	__IO uint32_t wen_ctrl;
	__IO uint32_t compare;
	__IO uint32_t cnt_value;
}WDT_TypeDef;

typedef struct
{
	__IO uint32_t gpio_int_status;
	__IO uint32_t gpio_int_mask;
	__IO uint32_t gpio_int_clear;
	__IO uint32_t kbrd_int_status;
	__IO uint32_t kbrd_int_mask;
	__IO uint32_t kbrd_int_clear;
	__IO uint32_t gpio_out_set;
	__IO uint32_t gpio_out_clear;
	__IO uint32_t interrupt_pol;
	__IO uint32_t interrupt_edge_level;
	__IO uint32_t interrupt_both_edge;
	__IO uint32_t gpio_int_en[3];
	__IO uint32_t gpio_input_status;
	__IO uint32_t gpio_pin_config[31];
	__IO uint32_t pwt_config_load;
	__IO uint32_t pwt_debounce_config;
	__IO uint32_t pwt_pol;
	__IO uint32_t pwt_en;
	__IO uint32_t pwt_wakeup_clr;
	__IO uint32_t kbrd_config_load;
	__IO uint32_t kbrd_config;
	__IO uint32_t kbrd_pin_sel;
}GPIO_TypeDef;

typedef struct
{
	__IO uint32_t int_status;
	__IO uint32_t int_mask;
	__IO uint32_t int_clear;
	__IO uint32_t timer0_prescale;
	__IO uint32_t timer0_trig_conf;
	__IO uint32_t timer0_compare;
	__IO uint32_t timer0_cnt;
	__IO uint32_t gtimer_ctrl;
	__IO uint32_t gtimer_trig_conf;
	__IO uint32_t gtimer_prescale_ctrl;
	__IO uint32_t gtimer_cnt_read_sel;
	__IO uint32_t gtimer_cnt;
	__IO uint32_t gtimer_compare[6];
	__IO uint32_t gpt_int_en[3];
}GPT_TypeDef;
typedef struct
{
	__IO uint32_t fifo_read;
	__IO uint32_t int_status;
	__IO uint32_t int_set;
	__IO uint32_t int_clr;
	__IO uint32_t int_mask;
	__IO uint32_t config;
}I2S_TypeDef;

typedef struct 
{
    __IO uint32_t flash_act_access_bgn;
    __IO uint32_t flash_act_cmd_adrs;
    __IO uint32_t flash_act_write_data;
    __IO uint32_t flash_act_debug_status;
    __IO uint32_t flash_act_ind_dout;
    __IO uint32_t flash_act_prog_err_adr;
	__IO uint32_t flash_act_nvr_key;
    __IO uint32_t no_use[249];
    __IO uint32_t flash_cpu_set;
    __IO uint32_t flash_cpu_time_set2;
    __IO uint32_t flash_cpu_time_set3;
}FLASH_Typedef;

typedef struct
{
    __IO uint32_t misc_cpu_cfg;
    __IO uint32_t misc_cpu_decrypt_key;
    __IO uint32_t rf_cpu_gp_adc;
    __IO uint32_t rf_cpu_gp_adc_data;
    __IO uint32_t adc_cpu_detect_threshold;
    __IO uint32_t adc_cpu_clr_overflow;
    __IO uint32_t adc_cpu_ctrl;
    __IO uint32_t misc_cpu_int_en;
    __IO uint32_t misc_cpu_int_clr;
    __IO uint32_t misc_cpu_int_trig;
    
    
}MSIC_Typedef;

typedef struct
{
	__IO uint32_t tfifo_wr;
	__IO uint32_t rfifo_rd;
	__IO uint32_t int_status;
	__IO uint32_t int_mask;
	__IO uint32_t int_clr;
	__IO uint32_t basic_config;
	__IO uint32_t mode_trig;
	__IO uint32_t config;
	__IO uint32_t divisor;
	__IO uint32_t fifo_cnt;
	__IO uint32_t fifo_clr;
}_7816_TypeDef;



typedef struct
{
    __IO uint32_t pmu_aon_32k_calibration;
    __IO uint32_t pmu_aon_int_en;
    __IO uint32_t pmu_aon_int_state;
    __IO uint32_t pmu_aon_int_clear;
    __IO uint32_t pmu_aon_cpu_ctrl;
    __IO uint32_t pmu_aon_clk_high;
    __IO uint32_t pmu_aon_clk_gate;
    __IO uint32_t pmu_aon_soft_reset;
    __IO uint32_t pmu_aon_wdt_ctrl;
    __IO uint32_t pmu_aon_sleep_ctrl;
    __IO uint32_t pmu_aon_pwr_ctrl;
    __IO uint32_t pmu_aon_pwr_to_retention;
    __IO uint32_t pmu_aon_pwr_to_active;
    __IO uint32_t pmu_aon_rad_ctrl;
	__IO uint32_t reserved0[2];
    __IO uint32_t pmu_aon_pmu_state;
    __IO uint32_t pmu_aon_power_iso_state;
    __IO uint32_t rf_aon_rc32k_16m;
    __IO uint32_t rf_aon_pll_xo16m;
    __IO uint32_t rf_aon_ldo;
    __IO uint32_t rf_aon_pll_clk;
    __IO uint32_t pmu_aon_cpu_vectrst_trg;
    __IO uint32_t pmu_aon_cpu_remap_en;
    __IO uint32_t pmu_aon_rbus_ext_clk_en;
    __IO uint32_t pmu_aon_func_ext_clk_en;
	__IO uint32_t reserved1[4];
    __IO uint32_t pmu_aon_code;
    
}PMU_Typedef;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
/* leave anonymous unions enabled */
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning restore
#else
#warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CPU FPGA memory map  ------------------------------- */
#define FLASH_BASE            (0x00000000UL)
#define RAM_BASE              (0x20000000UL)
#define RAM_FPGA_BASE         (0x1EFF0000UL)
#define CPU_CFG_BASE          (0xDFFF0000UL)

#define CPU_SYS_BASE          (ARM_CPU_CFG_BASE  + 0x00000)

/* --------------------------  DUT FPGA memory map  ------------------------------- */
#define APB_BASE              (0x40000000UL)
#define AHB_BASE              (0x4FF00000UL)
#define DMC_BASE              (0x60000000UL)
#define SMC_BASE              (0xA0000000UL)

#define EFLASH_BASE           (FLASH_BASE      + 0x10000)
#define GPIO_BASE             (APB_BASE      + 0x01000)
#define DMA_BASE              (APB_BASE      + 0x02000)
#define CPATCH_BASE           (APB_BASE      + 0x03000)
#define EFLASH_REG_BASE       (APB_BASE      + 0x04000)


#define LLC_BASE              (APB_BASE      + 0x10000)	
#define IIC0_BASE             (APB_BASE      + 0x11000)	
#define IIC1_BASE             (APB_BASE      + 0x12000)	
#define UART0_BASE            (APB_BASE      + 0x13000)
#define UART1_BASE            (APB_BASE      + 0x14000)
#define GPT_BASE              (APB_BASE      + 0x15000)
#define RTC_BASE              (APB_BASE      + 0x16000)
#define MISC_BASE             (APB_BASE      + 0x17000)
#define WDT_BASE              (APB_BASE      + 0x18000)
#define SPI0_BASE			  (APB_BASE      + 0x19000)
#define SPI1_BASE             (APB_BASE      + 0x1a000)
#define PMU_BASE              (APB_BASE      + 0x1b000)

#define I2S_BASE              (APB_BASE      + 0x20000)
#define _7816_BASE          (APB_BASE      + 0x21000)

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
/* --------------------------  CPU FPGA Peripherals  ------------------------------ */
#define CPU_SYS               ((CPU_SYS_TypeDef *)  CPU_SYS_BASE)


/* --------------------------  DUT FPGA Peripherals  ------------------------------ */

#define UART0                 ((   UART_TypeDef *)    UART0_BASE)
#define UART1                 ((   UART_TypeDef *)    UART1_BASE)
#define GPT                   ((    GPT_TypeDef *)      GPT_BASE)
#define SPI0                  ((    SPI_TypeDef *)     SPI0_BASE)
#define SPI1                  ((    SPI_TypeDef *)     SPI1_BASE)
#define IIC0                  ((    IIC_TypeDef *)     IIC0_BASE)
#define IIC1                  ((    IIC_TypeDef *)     IIC1_BASE)
#define RTC                   ((    RTC_TypeDef *)      RTC_BASE)            
#define WDT                   ((    WDT_TypeDef *)      WDT_BASE) 
#define GPIO                  ((   GPIO_TypeDef *)     GPIO_BASE)
#define I2S                   ((    I2S_TypeDef *)      I2S_BASE)
#define _7816                    (( _7816_TypeDef*)   _7816_BASE)
#define FLASH                 ((  FLASH_Typedef *) EFLASH_REG_BASE)  
#define MISC                  ((   MSIC_Typedef *)       MISC_BASE)
#define PMU                   ((    PMU_Typedef *)        PMU_BASE)  
/*------------------------  for test read/write reg ------------------------------*/


#ifdef __cplusplus
}
#endif
 
#endif  /* __MXDAQUA_H */

