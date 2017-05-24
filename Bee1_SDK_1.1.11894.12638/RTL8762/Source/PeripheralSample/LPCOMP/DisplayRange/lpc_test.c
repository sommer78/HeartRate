/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     lcp_test.c
* @brief    LPC demo
* @details
* @author   howie
* @date     2015-10-28
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x_uart.h"
#include "board.h"
#include "legacyperiphapi.h"
#include "freeRTOS.h"
#include "task.h"
#include "queue.h"

/**
  * @brief  Hex To Ascii,static function
  * @param  uint8_t number
  * @retval the number of array
  */
static uint8_t HexToAscii(uint8_t hex)
{
    if(hex <= 9)
    {
        return (hex - 0x00 + 0x30);
    }
    else if(hex >= 0x0A && hex <= 0x0F)
    {
        return (hex - 0x0A + 0x41);
    }
    else
    {
        /* wrong */
        return 0xFF;
    }  
}

/**
  * @brief  Output a single byte to the serial port.
  * @param  data.
  * @retval None
  */
static void putc (unsigned char data)
{
	/* wait for room in the transmit FIFO */
	while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);

	if (data=='\n'){
		UART->RB_THR='\r';
		UART->RB_THR='\n';
	}
	else
	UART->RB_THR = data;
}

/**
  * @brief  Output String to the serial port.
  * @param  point to string.
  * @retval None
  */
void serial_puts (const char *s)
{
	while (*s) {
		putc (*s++);
	}
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc (void)
{
	int temp;
	return temp;
}

/**
  * @brief  print_uint16 use serial port.
  * @param  uint16_t num.
  * @retval None
  */
void print_uint16(uint16_t num)
{
	uint8_t StringBuf[4];
	
	StringBuf[0] = HexToAscii((num & 0xf000) >> 12);
	StringBuf[1] = HexToAscii((num & 0x0f00) >> 8);
	StringBuf[2] = HexToAscii((num & 0x00f0) >> 4);
	StringBuf[3] = HexToAscii((num & 0x000f) >> 0);
	UART_SendData(UART, StringBuf, 4);
	/* wait tx fifo empty */
  while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
	serial_puts("\n");
}
/**
  * @brief  print_uint32 use serial port.
  * @param  uint32_t num.
  * @retval None
  */
void print_uint32(uint32_t num)
{
	uint8_t StringBuf[8];
	
	StringBuf[0] = HexToAscii((num & 0xf0000000) >> 28);
	StringBuf[1] = HexToAscii((num & 0x0f000000) >> 24);
	StringBuf[2] = HexToAscii((num & 0x00f00000) >> 20);
	StringBuf[3] = HexToAscii((num & 0x000f0000) >> 16);
	StringBuf[4] = HexToAscii((num & 0x0000f000) >> 12);
	StringBuf[5] = HexToAscii((num & 0x00000f00) >> 8);
	StringBuf[6] = HexToAscii((num & 0x000000f0) >> 4);
	StringBuf[7] = HexToAscii((num & 0x0000000f) >> 0);
	UART_SendData(UART, StringBuf, 8);
	/* wait tx fifo empty */
  while(UART_GetFlagState(UART, UART_FLAG_THR_EMPTY) != SET);
	serial_puts("\n");
}
/**
  * @brief  float to char*
  * @param  float value
	* @param  unsigned char *
  * @retval the number of array
  */
unsigned char Float2Char(float value,unsigned char *array)
{
    unsigned int  IntegerPart;
    float DecimalPart;
    unsigned char i=0;
    unsigned char j=0;
   char temp;
    if(value>=1){
      IntegerPart=(unsigned int)value;
        DecimalPart=value-IntegerPart;
    }
    else{
       IntegerPart=0;
       DecimalPart=value-IntegerPart;
    }
    if(IntegerPart == 0) {   
        array[0] = '0';   
        array[1] = '.';  
        i = 1;
    } else {   
        while(IntegerPart > 0) {  
            array[i] = IntegerPart % 10 + '0';  
                //printf("=%c\n",array[i]);
            IntegerPart /= 10;   
            i++;  
        }  
        i--;   //fix the result
        for(j=0; j<i; j++) {    
            temp = array[j];   
            array[j] = array[i - j];  
            array[i - j] = temp;
        }
        i++;
        array[i]='.';
    }
    //converttheDecimalpart
    i++;
    array[i++]=(unsigned int)(DecimalPart*10)%10+'0';
    array[i++]=(unsigned int)(DecimalPart*100)%10+'0';
    array[i++]=(unsigned int)(DecimalPart*1000)%10+'0';
    array[i++]=(unsigned int)(DecimalPart*10000)%10+'0';
    array[i]='\0';
    return i;
}

/**
  * @brief  Loop threshold.
  * @param  None.
  * @retval None
  */
void loop_threshold(void)
{
	uint8_t aon_reg_value = 0x0;
	uint8_t temp_data = 0x50;
	
	for(int i=0; i<16; i++)
	{
		aon_reg_value = bee_aon_gp_read(0x8E);
    /* threshold voltage = (0b10*0.05 + 0.05)*4V = 0.6V */
    aon_reg_value = temp_data;
		temp_data++;
		bee_aon_gp_write(0x8E, aon_reg_value);	
		RTC->CR1 |= BIT(21);
		vTaskDelay( 100/portTICK_RATE_MS );		
	}
}



