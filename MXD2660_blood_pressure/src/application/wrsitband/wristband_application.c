/**
 *****************************************************************************
 * @file     wristband_application.c
 * @author   nemohan
 * @version  V1.0.0
 * @date     20-Sep-2016
 * @brief    all application message definition
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

#include "system_config.h"
#include "wristband_service.h"
#include "app_message.h"
#include "app_ble.h"
#include "wristband_application.h"
#include "calendar.h"
#include "timer.h"
#include "mxd2660_gpio.h"
#include "mxd2660_spi.h"
#include "mxd2660_spi_master.h"
#include "LCM_00930.h"
#include "lis3dh_driver.h"
#include "zx_7816.h"

uint8 readCnt = 0;
uint16 wband_current_step = 0;
uint8 wband_current_power = 100;
timer_node_t wb_motor_timer;

uint32 wristband_set_data(void *buffer)
{
	//msg:type len yyyymmddhhmmss
	//msg byte:type len YYMDHMS
	uint8 *ptr = ((BLE_TYPE_MSG *)buffer)->p;

	CALENDAR_DATE date;
	uint8 return_data[2+sizeof(CALENDAR_DATE)];
	return_data[0] = WRISTBAND_CMD_SET_DATA;
	return_data[1] = sizeof(CALENDAR_DATE);

	date.year = (ptr[0]<<8) + ptr[1];
	date.mon = ptr[2];
	date.day = ptr[3];
	date.hour = ptr[4];
	date.min = ptr[5];
	date.sec = ptr[6];
	date.ms = 0x5A;

	memcpy(&return_data[2],&date,sizeof(CALENDAR_DATE));

	update_system_date(date);
	wristband_notify_AppSendToBle((WRISTBAND_CMD_HEAD *)return_data);
	return return_data[1]+2;
}
uint32 wristband_battay_check(void *buffer)
{
	WRISTBAND_CMD_HEAD msg;
	msg.cmd = WRISTBAND_CMD_GET_POWER;
	msg.length =1;
	msg.buf[0] = 0x23;
	wristband_notify_AppSendToBle(&msg);
	return 0;
}

void motor_timer_callback(void *parm)
{
	gpio_clear_bit(GPIO_4);
	timer_stop(&wb_motor_timer);
	
}
uint32 wristband_motor_proc(void *buffer)
{
	uint8 *ptr = ((BLE_TYPE_MSG *)buffer)->p;
	if(TIMER_NOT_INIT != wb_motor_timer.state)
	{
		timer_delete(&wb_motor_timer);
	}
	//05 01 XX
	switch(ptr[0])
	{
		case 0:
		{
			gpio_clear_bit(GPIO_4);
			timer_delete(&wb_motor_timer);
			break;
		}
		case 1:
		case 2:
		{
			gpio_set_bit(GPIO_4);
			timer_create(&wb_motor_timer,APP_TIMER_MODE_ONCE,motor_timer_callback);
			timer_start(&wb_motor_timer,1000,NULL);
			break;
		}
		default:
		{
			break;
		}
	}
	return 0;
}
uint32 sky1321_msg_proc(void *buffer)
{
	WRISTBAND_CMD_HEAD msg;
	uint8 *ptr = ((BLE_TYPE_MSG *)buffer)->p;
	uint8 tx_data[2] = {0x00,0x55};
	uint8 rx_data[2] = {0};

	spi1_master_init(TRX_DUPLEX, CPOL_CPHA_1_1, SPI_RATE_2M);
	
	//spi_set_master(SPI1);
	tx_data[0] = 0x02;
	tx_data[1] = 0xFF;
	spi1_master_trx(2, tx_data, rx_data);
	tx_data[0] = ptr[0] | 0x40;
	tx_data[1] = 0xFF;
	spi1_master_trx(0x0202, tx_data, rx_data);
	msg.cmd = WRISTBAND_CMD_1321;
	msg.length = 1;
	msg.buf[0] = rx_data[1];
	wristband_notify_AppSendToBle(&msg);
	return 0;
}
uint32 wristband_main_msg_proc(void *buffer)
{
	uint8 *ptr = ((BLE_TYPE_MSG *)buffer)->p;
	switch(ptr[0])
	{
		case WRISTBAND_CMD_GET_STEP:
		{
			uint8 step_data[5];
			step_data[0] = WRISTBAND_CMD_MAIN;
			step_data[1] = 3;
			step_data[2] = WRISTBAND_CMD_GET_STEP;
			step_data[3] = wband_current_step>>8;
			step_data[4] = wband_current_step&0xFF;
			wristband_notify_AppSendToBle((WRISTBAND_CMD_HEAD *)step_data);
			wband_current_step++;
			return 2;
		}
		case WRISTBAND_CMD_GET_POWER:
		{
			uint8 step_data[4];
			step_data[0] = WRISTBAND_CMD_MAIN;
			step_data[1] = 2;
			step_data[2] = WRISTBAND_CMD_GET_POWER;
			step_data[3] = wband_current_power;
			wristband_notify_AppSendToBle((WRISTBAND_CMD_HEAD *)step_data);
			wband_current_power--;
			if(0 == wband_current_power)
			{
				wband_current_power = 100;
			}
			return 1;
		}
		case WRISTBAND_CMD_GET_DATE:
		{
			CALENDAR_DATE can_date;
			uint8 step_data[10];
			get_system_date(&can_date);
			step_data[0] = WRISTBAND_CMD_MAIN;
			step_data[1]= 8;
			step_data[2] = WRISTBAND_CMD_GET_DATE;
			step_data[3] = can_date.year>>8;
			step_data[4] = can_date.year&0xFF;
			step_data[5] = can_date.mon;
			step_data[6] = can_date.day;
			step_data[7] = can_date.hour;
			step_data[8] = can_date.min;
			step_data[9] = can_date.sec;
			wristband_notify_AppSendToBle((WRISTBAND_CMD_HEAD *)step_data);
			break;
		}
		case WRISTBAND_CMD_SET_DATE:
		{
			CALENDAR_DATE can_date;
			can_date.year = ptr[1]*1000+ptr[2]*100+ptr[3]*10+ptr[4];
			can_date.mon = ptr[5]*10+ptr[6];
			can_date.day = ptr[7]*10+ptr[8];
			can_date.hour = ptr[9]*10+ptr[10];
			can_date.min = ptr[11]*10+ptr[12];
			can_date.sec = ptr[13]*10+ptr[14];
			update_system_date(can_date);
			break;
		}
		default:
		{
			return 0;
		}
	}
	return 0;
}


void wristband_msg_handler(APP_PERIPHERAL_MSG *msg)
{
    switch(msg->st_peri.sub_type)
    {
        case MSG_WB_TOUCH:
            break;
        
        case MSG_WB_PROFILE:
            APP_DBG(("WRISTBAND receive msg type:%d\n",msg->st_peri.command));
            switch(msg->st_peri.command)
            {
                case WRISTBAND_CMD_MAIN: //0
                {
                    wristband_main_msg_proc(msg);
                    break;
                }
                case WRISTBAND_CMD_LCM: //1
                {
                    lcm_cmd_proc(msg);
                    break;
                }
                case WRISTBAND_CMD_3DH: //2
                {
                    LIS3DH_cmd_proc(msg);
                    break;
                }
                case WRISTBAND_CMD_SET_DATA: //3
                {
                    wristband_set_data(msg);
                    break;
                }
                case WRISTBAND_CMD_GET_BAT: //4
                {
                    wristband_battay_check(msg);
                    break;
                }
                case WRISTBAND_CMD_MOTOR_CMD: //5
                {
                    wristband_motor_proc(msg);
                    break;
                }
                case WRISTBAND_CMD_7816_DATA: //6
                {
                    wristband_7816_cmd_proc(msg);
                    break;
                }
                case WRISTBAND_CMD_1321: //7
                {
                    sky1321_msg_proc(msg);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        
        default:
            break;
    }
}

void wristband_msg_to_ble_handler(uint8 command)
{

}

