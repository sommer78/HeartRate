/**
 *****************************************************************************
 * @file     func_table.c
 * @author   Richard
 * @version  V1.0.0
 * @date     29-June-2017
 * @brief    func table register here
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */
 
#include "system_config.h"
#include "pmu_api.h"
#include "hci_api.h"
#include "call_back.h"
#include "interrupt.h"
#include "utility.h"
#include "device_cfg.h"
#include "sys.h"

extern void system_assert_debug(uint16 adr, uint32 info);
extern void os_stack_overflow(void);
extern void main_lib_init(void);
extern void NMI_IRQ_Handler(void);
extern void HardFault_IRQ_Handler(void);
extern void main_process(void);
extern void gpio_wakeup_disable_gpios(uint32 gpio_s);
extern void gpio_wakeup_enable_gpios(uint32 pins, GPIO_WAKEUP_LEVEL edge);

extern void _7816_int_IRQHandler(void);
extern void DMA_int_IRQ0Handler(void);
extern void DMA_int_IRQ1Handler(void);
extern void GPIO_0_int_IRQHandler(void);
extern void GPIO_1_int_IRQHandler(void);
extern void GPIO_2_int_IRQHandler(void);
extern void I2C0_int_IRQHandler(void);
extern void I2C1_int_IRQHandler(void);
extern void I2S_int_IRQHandler(void);
extern void SPI0_int_IRQHandler(void);
extern void SPI1_int_IRQHandler(void);
extern void TMR0_int_IRQHandler(void);
extern void TMR1_int_IRQHandler(void);
extern void TMR2_int_IRQHandler(void);
extern void KEYBOARD_int_IRQHandler(void);
extern void UART0_int_IRQHandler(void);
extern void UART1_int_IRQHandler(void);
extern void I2S_int_IRQHandler(void);

//extern void CRYPT_int_IRQHandler(void);
//extern void RESERVED0_int_IRQHandler(void);
//extern void RESERVED1_int_IRQHandler(void);
//extern void RESERVED2_int_IRQHandler(void);
//extern void RESERVED3_int_IRQHandler(void);
//extern void RESERVED4_int_IRQHandler(void);
//extern void RESERVED5_int_IRQHandler(void);
//extern void RESERVED6_int_IRQHandler(void);
//extern void SWI0_int_IRQHandler(void);
//extern void SWI1_int_IRQHandler(void);
//extern void SWI2_int_IRQHandler(void);
//extern void SWI3_int_IRQHandler(void);

extern bool save_info_before_sleep_cb(void);
extern bool recover_info_after_wakeup_cb(void);

void system_pins_wakeup_enable(uint32 gpio_pins, GPIO_WAKEUP_LEVEL Polarity)
{
    gpio_wakeup_enable_gpios(gpio_pins, Polarity);
    dly_us(100);
}

void system_pins_wakeup_disable(uint32 gpio_pins)
{
    gpio_wakeup_disable_gpios(gpio_pins);
    dly_us(100);
}


void user_call_back_register(void)
{
    system_assert_cb[0] = (SYSTEM_ASSERT_FUNC_CB)system_assert_debug;

    call_back_func_table[0] = (CALL_BACK_FUNC)main_process;
    call_back_func_table[1] = (CALL_BACK_FUNC)os_stack_overflow;
#ifdef HCI_FUNC_EN
    call_back_check_table[0] = (CALL_BACK_CHECK_FUNC)is_hci_sleep;
#endif
    call_back_check_table[1] = (CALL_BACK_CHECK_FUNC)save_info_before_sleep_cb;
    call_back_check_table[2] = (CALL_BACK_CHECK_FUNC)recover_info_after_wakeup_cb;
#ifdef _TRUNK_DEBUG__
    call_back_check_table[3] = (CALL_BACK_CHECK_FUNC)is_log_canbe_sleep;
    system_assert_cb[1] = (SYSTEM_ASSERT_FUNC_CB)uart1_Log_output;
#endif
}

void user_irq_register(void)
{
    sys_irq_func_table[3] = (SYS_IRQ_FUNC)HardFault_IRQ_Handler;
    
    user_irq_func_table[UART0_IRQn] = (IRQ_FUNC)UART0_int_IRQHandler;
    user_irq_func_table[UART1_IRQn] = (IRQ_FUNC)UART1_int_IRQHandler;
    user_irq_func_table[SPI0_IRQn] = (IRQ_FUNC)SPI0_int_IRQHandler;
    user_irq_func_table[SPI1_IRQn] = (IRQ_FUNC)SPI1_int_IRQHandler;
    user_irq_func_table[DMA_IRQ0n] = (IRQ_FUNC)DMA_int_IRQ0Handler;
    user_irq_func_table[DMA_IRQ1n] = (IRQ_FUNC)DMA_int_IRQ1Handler;
    user_irq_func_table[I2S_IRQn] = (IRQ_FUNC)I2S_int_IRQHandler;
    user_irq_func_table[I2C0_IRQn] = (IRQ_FUNC)I2C0_int_IRQHandler;
    user_irq_func_table[I2C1_IRQn] = (IRQ_FUNC)I2C1_int_IRQHandler;
    user_irq_func_table[_7816_IRQn] = (IRQ_FUNC)_7816_int_IRQHandler;
    user_irq_func_table[TMR0_IRQn] = (IRQ_FUNC)TMR0_int_IRQHandler;
    user_irq_func_table[TMR1_IRQn] = (IRQ_FUNC)TMR1_int_IRQHandler;
    user_irq_func_table[TMR2_IRQn] = (IRQ_FUNC)TMR2_int_IRQHandler;
    user_irq_func_table[GPIO_0_IRQn] = (IRQ_FUNC)GPIO_0_int_IRQHandler;
    user_irq_func_table[GPIO_1_IRQn] = (IRQ_FUNC)GPIO_1_int_IRQHandler;
    user_irq_func_table[GPIO_2_IRQn] = (IRQ_FUNC)GPIO_2_int_IRQHandler;
    user_irq_func_table[KB_IRQn] = (IRQ_FUNC)KEYBOARD_int_IRQHandler;

//    user_irq_func_table[CRYPT_IRQn] = (IRQ_FUNC)CRYPT_int_IRQHandler;
//    user_irq_func_table[RESERVED0_IRQn] = (IRQ_FUNC)RESERVED0_int_IRQHandler;
//    user_irq_func_table[RESERVED1_IRQn] = (IRQ_FUNC)RESERVED1_int_IRQHandler;
//    user_irq_func_table[RESERVED2_IRQn] = (IRQ_FUNC)RESERVED2_int_IRQHandler;
//    user_irq_func_table[RESERVED3_IRQn] = (IRQ_FUNC)RESERVED3_int_IRQHandler;
//    user_irq_func_table[RESERVED4_IRQn] = (IRQ_FUNC)RESERVED4_int_IRQHandler;
//    user_irq_func_table[RESERVED5_IRQn] = (IRQ_FUNC)RESERVED5_int_IRQHandler;
//    user_irq_func_table[RESERVED6_IRQn] = (IRQ_FUNC)RESERVED6_int_IRQHandler;
//    user_irq_func_table[SWI0_IRQn] = (IRQ_FUNC)SWI0_int_IRQHandler;
//    user_irq_func_table[SWI1_IRQn] = (IRQ_FUNC)SWI1_int_IRQHandler;
//    user_irq_func_table[SWI2_IRQn] = (IRQ_FUNC)SWI2_int_IRQHandler;
//    user_irq_func_table[SWI3_IRQn] = (IRQ_FUNC)SWI3_int_IRQHandler;

}

int main(void)
{
    system_set_rf_tx_power(RF_0DB_CFG);
    system_set_radio_mode(Rx_neg95_dbm);  //this API will be called when used new symbol lib,at that time user can be set this value
    system_power_dcdc_mode_set(POWER_MODE_DCDC);//wyp	Aug 07, 2017  POWER_MODE_DCDC POWER_MODE_LDO
    main_lib_init();
}

