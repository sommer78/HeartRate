/*
* Oryginal by:
* Author : Aaron Lee
* Version: 1.00
* Date   : 2014.3.24
* Email  : hello14blog@gmail.com
* Modification: none
* Mod by reaper7
* found at http://bbs.espressif.com/viewtopic.php?f=15&t=31
*/

#include "stm32f10x_i2c.h"
#include "i2c_oled.h"
#include "fonts.h"
#include <string.h>
#include "bsp_SysTick.h"
//#include "drv_delay.h"

//#include "osapi.h"

#define MAX_SMALL_FONTS 21 //SMALL FONTS
#define MAX_BIG_FONTS 16 //BIG FONTS



#define I2C_SSD1306_SLAVE_ADDRESS8 0x78                      // 8 bit slave address (write)
#define I2C_TIMEOUT                 100000
#define COMMAND_BYTE 0x00
 #define DATA_BYTE 0x40


 #define I2C_SPEED                   50000                     /*!< I2C Speed */



void init_I2C1_peripheral(I2C_TypeDef *I2Cx){
// Initializes the I2C1 Peripheral on PB6 & PB7
// 1) Enables the GPIOB Peripheral Clock
// 2) Enable the I2C1 Peripheral Clock
// 3) Configure the GPIO peripheral with GPIO_InitStructure and GPIO_Init()
// 4) Configure the I2C1 peripheral with I2C_InitStructure and I2C_Init() 

    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    /* GPIOB Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO   , ENABLE);

	
	

    /* I2C1 and I2C1 Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* Configure I2C1 pins: SCL and SDA ----------------------------------------*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // Open Drain, I2C bus pulled high externally
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);


    /* Enable I2C1 -------------------------------------------------------------*/
    I2C_DeInit(I2C1);

	
    I2C_Cmd(I2C1, ENABLE);

    /* I2C1 configuration ------------------------------------------------------*/
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
   // I2C_InitStructure.I2C_OwnAddress1 = 0x039;                                  // This is important for some reason!
    I2C_InitStructure.I2C_OwnAddress1 = 0x000;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    I2C_Init(I2C1, &I2C_InitStructure);
}


u8 i2c_send_command(  u8 slave_data){
// Sends I2C data over I2Cx:
//  1) Sends Start Condition. Checks for I2C EV5 
//  2) Sends 7 bit address & checks for EV6
//  3) Sends 8 bit command byte (0x00) & checks for EV8
//  4) Sends 8 bits (1 byte) of data & checks for EV8
//  5) Sends Stop Condition
    int TimeOut;

    

    /* Send I2C1 START condition */
    I2C_GenerateSTART(I2C1, ENABLE);

    /* Test on I2C1 EV5 and clear it */
    TimeOut = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        TimeOut--;
        if (TimeOut == 0){
         //   turn_on_error_led_pin();       // Error LED
            return 1;
        }
    }

    /* Send SSD1306 7 bit slave Address for write. Check to make sure ACK received */
    I2C_Send7bitAddress(I2C1, I2C_SSD1306_SLAVE_ADDRESS8, I2C_Direction_Transmitter);

    //Test on I2C1 EV6 and clear it
    TimeOut = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2C1, ENABLE);
         //   turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }
    I2C_SendData(I2C1, COMMAND_BYTE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2C1, ENABLE);
        //    turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }
    I2C_SendData(I2C1, slave_data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2C1, ENABLE);
         //   turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
		return 0;
}
u8 i2c_send_data(  u8 slave_data){
// Sends I2C data over I2Cx:
//  1) Sends Start Condition. Checks for I2C EV5 
//  2) Sends 7 bit address & checks for EV6
//  3) Sends 8 bit command byte (0x00) & checks for EV8
//  4) Sends 8 bits (1 byte) of data & checks for EV8
//  5) Sends Stop Condition
    int TimeOut;

   

    /* Send I2C1 START condition */
    I2C_GenerateSTART(I2C1, ENABLE);

    /* Test on I2C1 EV5 and clear it */
    TimeOut = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        TimeOut--;
        if (TimeOut == 0){
         //   turn_on_error_led_pin();       // Error LED
            return 1;
        }
    }

    /* Send SSD1306 7 bit slave Address for write. Check to make sure ACK received */
    I2C_Send7bitAddress(I2C1, I2C_SSD1306_SLAVE_ADDRESS8, I2C_Direction_Transmitter);

    //Test on I2C1 EV6 and clear it
    TimeOut = I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2C1, ENABLE);
         //   turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }
    I2C_SendData(I2C1, DATA_BYTE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2C1, ENABLE);
        //    turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }
    I2C_SendData(I2C1, slave_data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){ // Wait for EV8
        TimeOut--;
        if (TimeOut == 0){
            // Send I2C1 STOP Condition
            I2C_GenerateSTOP(I2C1, ENABLE);
         //   turn_on_error_led_pin();        // Error LED

            return 2;
        }
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
		return 0;
}
void 
OLED_writeCmd(u8 I2C_Command)
{
  i2c_send_command(I2C_Command);
}

void 
OLED_writeDat(u8 I2C_Data)
{
	i2c_send_data(I2C_Data);
}

u8 
OLED_Init(void)
{
u8 state;
	   /*----- Configure I2C Phase -----*/
    init_I2C1_peripheral(I2C1);   // To Do - Move All this into init_I2C1_peripheral()
	Delay_ms(10);
  state = i2c_send_command(0xae);
  if(state!=0){
  	return 0;
  	}

  OLED_writeCmd(0xae); // turn off oled panel
  OLED_writeCmd(0x00); // set low column address 
  OLED_writeCmd(0x10); // set high column address 
  OLED_writeCmd(0x40); // set start line address 
  OLED_writeCmd(0x81); // set contrast control register 
  
  OLED_writeCmd(0xa0);
  OLED_writeCmd(0xc0);
  
  OLED_writeCmd(0xa6); // set normal display 
  OLED_writeCmd(0xa8); // set multiplex ratio(1 to 64) 
  OLED_writeCmd(0x3f); // 1/64 duty 
  OLED_writeCmd(0xd3); // set display offset 
  OLED_writeCmd(0x00); // not offset 
  OLED_writeCmd(0xd5); // set display clock divide ratio/oscillator frequency 
  OLED_writeCmd(0x80); // set divide ratio 
  OLED_writeCmd(0xd9); // set pre-charge period 
  OLED_writeCmd(0xf1); 
  OLED_writeCmd(0xda); // set com pins hardware configuration 
  OLED_writeCmd(0x12); 
  OLED_writeCmd(0xdb); // set vcomh 
  OLED_writeCmd(0x40); 
  OLED_writeCmd(0x8d); // set Charge Pump enable/disable 
  OLED_writeCmd(0x14); // set(0x10) disable
  OLED_writeCmd(0xaf); // turn on oled panel	
    
  OLED_Fill(0x00);  //OLED CLS
  
  return 1;
}

void 
OLED_SetPos(u8 x, u8 y)
{ 
	OLED_writeCmd(0xb0+y);
	OLED_writeCmd(((x&0xf0)>>4)|0x10);
	OLED_writeCmd((x&0x0f)|0x01);
}

void 
OLED_Fill(u8 fill_Data)
{
	u8 m,n;
	for(m=0;m<8;m++)
	{
		OLED_writeCmd(0xb0+m);		//page0-page1
		OLED_writeCmd(0x00);		//low column start address
		OLED_writeCmd(0x10);		//high column start address
		for(n=0;n<132;n++)
			{
				OLED_writeDat(fill_Data);
			}
	}
}

void 
OLED_CLS(void)
{
	OLED_Fill(0x00);
}

void 
OLED_ON(void)
{
	OLED_writeCmd(0X8D);
	OLED_writeCmd(0X14);
	OLED_writeCmd(0XAF);
}

void 
OLED_OFF(void)
{
	OLED_writeCmd(0X8D);
	OLED_writeCmd(0X10);
	OLED_writeCmd(0XAE);
}

void 
OLED_ShowStr(u8 x, u8 y, char ch[], u8 TextSize)
{
	u8 c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					OLED_writeDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					OLED_writeDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					OLED_writeDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

void 
OLED_Print(u8 x, u8 y, u8 ch[], u8 TextSize)
{
  u8 step;
  if (TextSize==1)
    step = x*6; 
  else if (TextSize==2)
    step = x*8; 
  else 
    step = x;
  OLED_ShowStr(step,y,ch,TextSize);
}

void 
OLED_Line(u8 y, u8 ch[], u8 TextSize)
{
  u8 step;
  u8 len = strlen(ch);
  u8 maxf = 0;
  
  y--;
  
  if (TextSize==1) { 
    maxf = MAX_SMALL_FONTS;
  }
  else if (TextSize==2) {
    y *= 2;
    maxf = MAX_BIG_FONTS;
  }

  if ((TextSize>=1) && (TextSize<=2)) {
    memset(ch+len+1,' ',maxf-len);
    memset(ch+maxf,'\0',1);

    OLED_ShowStr(0,y,ch,TextSize);
  }
}

void 
OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	unsigned int j=0;
	u8 x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_writeDat(BMP[j++]);
		}
	}
}
