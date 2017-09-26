#ifndef _LOG_DESCRIPTOR_H_
#define _LOG_DESCRIPTOR_H_


#define LOG_MAIN_INITIALIZATION         0xD000
#define LOG_MAIN_TASK_ID                0xD001
#define LOG_MAIN_MSG_ID                 0xD002
#define LOG_HARDFAULT_RESET             0xD003


#define LOG_PROFILE_DEBUG_BASE          0xD400

//iic
#define LOG_I2C_DEBUG_BASE              0xD500
#define LOG_I2C_NOT_FINE_ID             LOG_I2C_DEBUG_BASE+0          //i2c regesiter fail and not fount id

//RTC
#define LOG_RTC_DEBUG_BASE              0xD600
#define LOG_RTC_TIMER_SET_COMP          LOG_RTC_DEBUG_BASE+0          //rtc softerware timer set compare value

//GPIO
#define LOG_GPIO_DEBUG_BASE             0xD680
#define LOG_GPIO_SYSTEM_ASSERT          LOG_GPIO_DEBUG_BASE+0        //GPIO system assert

//ir
#define LOG_IR_DEBUG_BASE               0xD880
#define LOG_IR_MSG_KEY                  LOG_IR_DEBUG_BASE+0           //ir module receive key msg value
#define LOG_IR_LOAD_COMPARE_ERROR       LOG_IR_DEBUG_BASE+1           //ir load compare error
#define LOG_IR_SEND_BIT                 LOG_IR_DEBUG_BASE+2           //ir sedn bit value
#define LOG_IR_SEND_CNT                 LOG_IR_DEBUG_BASE+3           //ir send bit counter
#define LOG_IR_SEND_END                 LOG_IR_DEBUG_BASE+4           //ir send data end,disable ir timer
#define LOG_IR_TIMER_CALLBACK           LOG_IR_DEBUG_BASE+5           //ir timer callback function,value is cnt

//UART
#define LOG_UART_DEBUG_BASE             0xD900

//SPI
#define LOG_SPI_DEBUG_BASE              0xD980
#define LOG_SPI_READ_TEMPER             LOG_SPI_DEBUG_BASE+0
#define LOG_SPI_READ_IC_ID              LOG_SPI_DEBUG_BASE+1
#define LOG_SPI_READ_DATA               LOG_SPI_DEBUG_BASE+2

#define LOG_PMU_DEBUG_BASE              0xDA00

//keyboard
#define LOG_KEYBOARD_DEBUG_BASE         0xDA80
#define LOG_KEYBOARD_SEND_MSG           LOG_KEYBOARD_DEBUG_BASE+0     //keyboard send msg to app task 
#define LOG_KEYBOARD_SEND_QID           LOG_KEYBOARD_DEBUG_BASE+1     //keyboard send msg resault
#define LOG_KEYBOARD_KEY_INFO_ADDR      LOG_KEYBOARD_DEBUG_BASE+2     //keyboard info struct address
#define LOG_KEYBOARD_TIMER_ID           LOG_KEYBOARD_DEBUG_BASE+3     //keyboard timer id
#define LOG_KEYBOARD_TIMER_START        LOG_KEYBOARD_DEBUG_BASE+4     //keyboard timer start resault
#define LOG_KEYBOARD_PINSTATES          LOG_KEYBOARD_DEBUG_BASE+5     //keyboard pin status
#define LOG_KEYBOARD_TIMER_STATES       LOG_KEYBOARD_DEBUG_BASE+6     //keyboard timer status
#define LOG_KEYBOARD_KEY_SCAN           LOG_KEYBOARD_DEBUG_BASE+7     //keyboard scanned key value
#define LOG_KEYBOARD_RUN_STATES         LOG_KEYBOARD_DEBUG_BASE+8     //keyboard run status
#define LOG_KEYBOARD_ASSERT             LOG_KEYBOARD_DEBUG_BASE+9     //keyboard assert
#define LOG_KEYBOARD_INT_GPIO_STATES    LOG_KEYBOARD_DEBUG_BASE+10    //when keyboard interrupt happended,gpio states
#define LOG_KEYBAORD_AFTER_CLR_INT      LOG_KEYBOARD_DEBUG_BASE+11    //after clear interrupt, interrupt status


#define LOG_ASSERT_ID                   0xDFFF

#endif
