#include "system_config.h"
#include "stack_task.h"
#include "mxd2660_uart.h"
#include "mxd2660_gpio.h"
#include "mxd2660_flash.h"
#include "mxd2660_rtc.h"
#include "mxd2660_dma.h"
#include "mxd2660_adc.h"
#include "mxd2660_watch_dog.h"
#include "app_kbd.h"
#include "hci_api.h"
#include "app_ir.h"
#include "scan_para_service.h"
#include "hid_service.h"
#include "mxd_trx_service.h"
#include "dev_info_service.h"
#include "battery_service.h"
#include "app_task.h"
#include "app_ble.h"
#include "hci_api.h"
#include "timer.h"
#include "app_hid_kbd.h"
#include "pmu_api.h"
#include "app_ota.h"
#include "pmu_clk.h"
#include "utility.h"
#include "version.h"
#include "device_cfg.h"
#include "app_uart_update.h"
#include "sys.h"

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
    
    APP_DBG(("\nMXD hid keyboard start to work\r\n"));
       
    app_kbd_init();   //init keyboard

    //creat a timer
    timer_create(&kbd_adv_timer, APP_TIMER_MODE_ONCE, kbd_timeout_handler);
    //buzzer io configuration
	gpio_init_output(GPIO_4,GPIO_PULL_NONE,0);
    
	gpio_init_high_impedance(GPIO_3, GPIO_PULL_NONE);  //must set the selected ADC ch as high impedence
    adc_init(7);

}

void ble_profile_init(void)
{

    dev_info_init_service();
    batt_init_service();
    scan_para_init_service();
    // Set up HID keyboard service
    hid_init_service( );

    ota_init();
    
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
	
	NVIC_ClearPendingIRQ(KB_IRQn);
    NVIC_SetPriority(KB_IRQn, 0x3);
    NVIC_EnableIRQ(KB_IRQn);

	NVIC_ClearPendingIRQ(TMR0_IRQn);
    NVIC_SetPriority(TMR0_IRQn, 0x3);
    NVIC_EnableIRQ(TMR0_IRQn);
}

bool save_info_before_sleep_cb(void)
{
	if(KEYBOARD_WAKEUP_PIN & gpio_read_input())
	{
		system_pins_wakeup_enable(KEYBOARD_WAKEUP_PIN, GPIO_WAKEUP_LOW_LEVEL);
	}
	else
	{
		return 1;
	}
	if(gpio_read_input_bit(GPIO_10))
	{
		return 1;
	}
//	APP_DBG(("SLEEP\n"));
#if defined(_TRUNK_DEBUG__) || defined(LOG_FUNC_EN)
    gpio_init_function(GPIO_11, GPIO_PIN);//uart0 rx
    gpio_init_input(GPIO_11, GPIO_PULL_DOWN); 
#endif
    return 0 ;
}

bool recover_info_after_wakeup_cb(void)
{
	system_pins_wakeup_disable(KEYBOARD_WAKEUP_PIN);
//    APP_DBG(("Wakeup\n"));
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
//    system_set_low_power_mode(ACTIVE);
}


void app_main(void)
{
    dly_us(2000000); //仅仅为了调试，保证时钟切换带来的风险
    power_manage_init();
    
    peripheral_init();
    
    ble_profile_init();
    ble_app_init();
    
    app_irq_init();
    app_task_init();
    
//	watch_dog_init(15000);
//	watch_dog_enable();
    osThreadTerminate (osThreadGetId());

}



