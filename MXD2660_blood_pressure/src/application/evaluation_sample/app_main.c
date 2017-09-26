#include "system_config.h"
#include "mxd2660_uart.h"
#include "mxd2660_flash.h"
#include "mxd2660_gpio.h"
#include "mxd2660_rtc.h"
#include "mxd2660_dma.h"
#include "mxd2660_adc.h"
#include "mxd2660_watch_dog.h"
#include "timer.h"
#include "scan_para_service.h"
#include "hid_service.h"
#include "mxd_trx_service.h"
#include "stack_task.h"
#include "app_task.h"
#include "app_ble.h"
#include "hci_api.h"
#include "ble_api.h"
#include "app_ota.h"
#include "pmu_api.h"
#include "pmu_clk.h"
#include "utility.h"
#include "version.h"
#include "device_cfg.h"
#include "app_uart_update.h"
#include "call_back.h"
#include "sys.h"


#define WAKEUP_PIN GPIO_10
#define UART_UPDATE_PIN GPIO_29
#define UART_UPDATE_PIN2 GPIO_30


const UPDATE_HEADER user_app_header __attribute__((section(".user.app.header"))) = 
{
    PROJECT_ID,
    0,
    0,
    APP_VERSION,
    0,
    0
};

#ifdef RELEASE_RAM_20KB_VERSION
void app_main(void) __attribute__((section(".ARM.__at_0x2B100"))) ;
#endif

#ifdef RELEASE_DEFAULT_VERSION
void app_main(void) __attribute__((section(".ARM.__at_0x2B100"))) ;
#endif


void main_process(void)
{
//    APP_DBG(("main_process\n"));
#ifdef HCI_FUNC_EN
    hci_soft_reset();
#endif
#ifdef _TRUNK_DEBUG__
    uart1_Log_print();
#endif    
    watch_dog_feed();
}

void app_irq_init(void)
{
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 0x3);
    NVIC_EnableIRQ(UART0_IRQn);
    
    NVIC_ClearPendingIRQ(I2C0_IRQn);
    NVIC_SetPriority(I2C0_IRQn, 0x3);
    NVIC_EnableIRQ(I2C0_IRQn);

    NVIC_ClearPendingIRQ(I2C1_IRQn);
    NVIC_SetPriority(I2C1_IRQn, 0x3);
    NVIC_EnableIRQ(I2C1_IRQn);

    NVIC_ClearPendingIRQ(DMA_IRQ0n);
    NVIC_SetPriority(DMA_IRQ0n, 0x0);
    NVIC_EnableIRQ(DMA_IRQ0n);

    NVIC_ClearPendingIRQ(DMA_IRQ1n);
    NVIC_SetPriority(DMA_IRQ1n, 0x0);
    NVIC_EnableIRQ(DMA_IRQ1n);
  
}


void peripheral_init(void)
{
    user_irq_register();
    user_call_back_register();
    timer_init();
    
#if defined(_TRUNK_DEBUG__) || defined(LOG_FUNC_EN)
    gpio_init_function(GPIO_11, UART_1_TXD); //uart1 tx
    uart1_init(BAUDRATE_921600);
#endif
    
#ifdef _TRUNK_DEBUG__
    sys_debug_init();
    sys_debug_en(1);
#endif
    
    APP_DBG(("\nMXD Evaluation sample  start to work\n"));   //user debug format
    
#ifdef HCI_FUNC_EN  
    gpio_config_pupd(GPIO_16,GPIO_PULL_UP);
	gpio_config_pupd(GPIO_15,GPIO_PULL_DOWN);
    gpio_init_function(GPIO_16, UART_0_RXD);//uart0 rx
    gpio_init_function(GPIO_15, UART_0_TXD);//uart0 rt.
    hci_app_init();
    uart0_set_baudrate(BAUDRATE_921600);
#endif

//    gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
//    adc_init(7);

 
}

void ble_profile_init(void)
{
    trx_init_service();
    ota_init();
}

bool save_info_before_sleep_cb(void)
{  
    if(gpio_read_input_bit(WAKEUP_PIN))
    {
        APP_DBG(("NO SLEEP\n"));
        return 1;
    }
#ifdef HCI_FUNC_EN
    hci_uart_rx_buf_clear_cnt();
#endif

#if defined(_TRUNK_DEBUG__) || defined(LOG_FUNC_EN)
    gpio_init_function(GPIO_11, GPIO_PIN);//uart0 rx
    gpio_init_input(GPIO_11, GPIO_PULL_DOWN); 
#endif
    return 0 ;
}

bool recover_info_after_wakeup_cb(void)
{
#ifdef HCI_FUNC_EN
    hci_dma_init(); 
#endif
    
#if defined(_TRUNK_DEBUG__) || defined(LOG_FUNC_EN)
    gpio_init_function(GPIO_11, UART_1_TXD); //uart1 tx
#endif
    return 0 ;
}

void power_manage_init(void)
{
    system_clk_gate(0x7fffff);  //0x7fffff
    system_32k_clk_select(RC_32K);
    system_set_radio_tx_power(TX_0_dbm);
    
    system_pins_wakeup_enable(GPIO_10, GPIO_WAKEUP_HIGH_LEVEL);

    system_set_low_power_mode(SLEEP);
}


void app_main(void)
{
    dly_us(2000000);
    power_manage_init();
    
    peripheral_init();
    
    app_irq_init();
    app_task_init();

    ble_profile_init();
    ble_app_init();

//    watch_dog_init(15000);
//	watch_dog_enable();
    osThreadTerminate (osThreadGetId());

}





