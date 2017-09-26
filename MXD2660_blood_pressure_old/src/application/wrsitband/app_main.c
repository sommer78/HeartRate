#include "system_config.h"
#include "stack_task.h"
#include "mxd2660_uart.h"
#include "mxd2660_gpio.h"
#include "mxd2660_flash.h"
#include "mxd2660_rtc.h"
#include "mxd2660_dma.h"
#include "mxd2660_adc.h"
#include "mxd2660_watch_dog.h"
#include "timer.h"
#include "wristband_service.h"
#include "mxd_trx_service.h"
#include "calendar.h"
#include "LCM_00930.h"
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
#include "lts1303m.h"

#ifdef ANCS_FUNC_EN
#include "led.h"
#endif

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
    gpio_init_function(GPIO_24, UART_1_TXD); //uart1 tx
    uart1_init(BAUDRATE_921600);
	APP_DBG(("\nMOVNOW wristband start to work>>\n"));   //user debug format
#endif
    
#ifdef _TRUNK_DEBUG__
    sys_debug_init();
    sys_debug_en(0);
#endif
    
//    APP_DBG(("\nMXD wristband  start to work\n"));   //user debug format

#ifdef ANCS_FUNC_EN
    led_init();
#endif
  
    calendar_init();
	//iic0 pin select for LCM
	gpio_init_function(GPIO_20,I2C_0_SCL);
	gpio_init_function(GPIO_21,I2C_0_SDA);
	//LCM reset pin
	gpio_init_output(GPIO_19,GPIO_PULL_NONE,1);
	//moto pin
	gpio_init_output(GPIO_4,GPIO_PULL_NONE,0);
    LCM_init_oled();
    //LIS3DH_InitSPIDrive();
    
    gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
    adc_init(7);

}

void ble_profile_init(void)
{
    wristband_init_service();
    ota_init();
}

bool save_info_before_sleep_cb(void)
{
    if(gpio_read_input_bit(GPIO_10))
    {
        return 1;
    }
//    APP_DBG(("Go to Sleep\n"));
#if defined(_TRUNK_DEBUG__) || defined(LOG_FUNC_EN)
    gpio_init_function(GPIO_11, GPIO_PIN);//uart0 rx
    gpio_init_input(GPIO_11, GPIO_PULL_DOWN); 
#endif
    return 0 ;
}

bool recover_info_after_wakeup_cb(void)
{
#if defined(_TRUNK_DEBUG__) || defined(LOG_FUNC_EN)
    gpio_init_function(GPIO_24, UART_1_TXD); //uart1 tx
#endif
//    APP_DBG(("Wakeup\n"));
    return 0 ;
}

void power_manage_init(void)
{
    system_clk_gate(0x7fffff);  //0x7fffff
    system_32k_clk_select(RC_32K);
    system_set_radio_tx_power(TX_0_dbm);
    
    system_pins_wakeup_enable(GPIO_10, GPIO_WAKEUP_HIGH_LEVEL);

    system_set_low_power_mode(ACTIVE);
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
	
	hw_timer_config();
	bsp_ts1303_init();

//    watch_dog_init(15000);
//	watch_dog_enable();
    osThreadTerminate (osThreadGetId());

}





