
/* Includes ------------------------------------------------------------------*/
#include "LCM_00930.h"

#include "mxd2660_gpio.h"
//#include "nrf_delay.h"
#include "mxd2660_iic.h"
#include "mem.h"
//#include "ble_app.h"
#include "mxd_trx_service.h"
#include "app_message.h"
#include "app_task.h"
#include "wristband_service.h"

#define	Slave_Address 0x3c//0x7c//0xF1//0xF4//0x7B		// 器件地址
#define	OP_Command 0x00
#define	OP_Data 0x40

static void display_black(void);
static void display_white(void);
uint8 screen_on = 0;

uint8_t i2c_buff[2] = {0, 0};
static void Write_Command (uint8_t cmd)
{
	i2c_buff[0] = cmd;
	i2c0_write(i2c_buff,1,OP_Command,Slave_Address);
}

static void Write_Data (uint8_t data)
{
	i2c_buff[0] = data;
	i2c0_write(i2c_buff,1,OP_Data,Slave_Address);
}

void LCM_init_oled(void)
{
	gpio_clear_bit(GPIO_19);
	delay_us(3000);
	gpio_set_bit(GPIO_19);

    Write_Command(0xAE);	  //Set Display Off

    Write_Command(0xd5);	  //display divide ratio/osc. freq. mode
    Write_Command(0x80);	  //105Hz	 内部晶振470 KHz		 d1

    Write_Command(0xA8);	  //multiplex ration mode:31
    Write_Command(0x1F);	   //32

    Write_Command(0xD3);	  //Set Display Offset
    Write_Command(0x00);

    Write_Command(0x40);	  //Set Display Start Line

    Write_Command(0x8D);	  //Set Display Offset
    Write_Command(0x14);  //for enabling charge pump

    Write_Command(0xA1);	  //Segment Remap

    Write_Command(0xC8);	  //Sst COM Output Scan Direction

    Write_Command(0xDA);	  //common pads hardware: alternative
    Write_Command(0x02);

    Write_Command(0x81);	  //contrast control 
    Write_Command(0x68);   //Write_Command(0x00);

    Write_Command(0xD9);	   //t pre-charge period
    Write_Command(0xF1);

    Write_Command(0xDB);	  //VCOM deselect level mode
    Write_Command(0x40);	   //t Vvcomh=0.83*Vcc

    Write_Command(0xA4);	  //Set Entire Display On/Off

    Write_Command(0xA6);	  //Set Normal Display

    //display_white();
	Write_Command(0xAF);	  //Set Display On
}

void LCM_display_off(void)
{
	Write_Command(0xAE);     //Set Display Off
}

void LCM_display_on(void)
{
	Write_Command(0xAF);     //Set Display On
}

static void display_black(void)
{
    unsigned short j,i;
    for (i=0; i<4; i++) {
        Write_Command(0xB0+i);
        Write_Command(0x00);
        Write_Command(0x10);
        for (j=0; j<128; j++) {
            Write_Data(0x00);
        }    
    }
}

static void display_white(void)
{
    unsigned short j,i;
    for (i=0; i<4; i++) {
        Write_Command(0xB0+i);
        Write_Command(0x00);
        Write_Command(0x10);
        for (j=0; j<128; j++) {
            Write_Data(0x55);
        }    
    }
}

/**
* @brief 
* @param 
* @return 
*/
uint8 lcm_on = 0;
uint32 lcm_cmd_proc(void *buffer)
{
	uint8 *ptr = ((APP_PERIPHERAL_MSG *)buffer)->p;
	//01 00
	switch(ptr[0])
	{
		case 0:
		{
			lcm_on = 0;
			display_black();
			break;
		}
		case 1:
		{
			lcm_on = 1;
			display_white();
			break;
		}
		case 2:
		{
			if(0 == lcm_on)
			{
				lcm_on = 1;
				display_white();
			}
			else
			{
				lcm_on = 0;
				display_black();
			}
			break;
		}
		default:
		{
			return 0;
		}
	}
	//wristband_notify_AppSendToBle(&(ptr->cmd));
	return 0;
}

