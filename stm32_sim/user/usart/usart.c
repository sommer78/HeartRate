#include "usart.h"	  
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "bsp_SysTick.h"
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB



#define TXD_high()		GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define TXD_low()		GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SendingDelay	104

//#define SIM_USART








#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
	
} 


#if 1
/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  



/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
//	extern void SendOneByte(char datatoSend);
#ifdef SIM_USART
int fputc(int ch, FILE *f)
{

	/* 写一个字节到USART1 */
	//USART_SendData(USART1, (uint8_t) ch);
	SendOneByte((uint8_t) ch);
	/* 等待发送结束 */
	//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
}
#endif
/*
*********************************************************************************************************
*	函 数 名: GPIO_Configuration
*	功能说明: 配置PA9为TXD，PA10做RXD
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void Sim_Usart_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIOA clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

	/* 防止上电后的误判 */
	GPIO_SetBits(GPIOA, GPIO_Pin_7);

	/* 配置PB1为推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



}

/*
*********************************************************************************************************
*	函 数 名: SendOneByte
*	功能说明: 模拟串口发送一字节数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SendOneByte(u8 datatoSend)
{
	u8 i, tmp;

	/* Start bit */
	TXD_low();
	Delay_us(SendingDelay);	

	for(i = 0; i < 8; i++)
	{
		tmp	= (datatoSend >> i) & 0x01;

		if(tmp == 0)
		{
			TXD_low();
			Delay_us(SendingDelay);	//0		
		}
		else
		{
			TXD_high();
			Delay_us(SendingDelay);	//HIGH   
			}
	}
	
	/* Stop bit */
	TXD_high();
	Delay_us(SendingDelay);	
}



//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置
/*
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  */
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
 //   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}
//初始化IO 串2
//bound:波特率
void uart2_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//USART2
 	USART_DeInit(USART2);  //复位串口2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口
		
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_Cmd(USART2, ENABLE);                    //使能串口 

}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
#if 1
static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */
#endif
/***************************************************************************************************
 * 函 数 名: debugInfo
 * 功能描述: 调试信息
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: 字符串 info 显示哪一行column
 * 返　　回: 
***************************************************************************************************/

void debugInfo(char* info, int column){
	MC_INFO("debug[%s]\r\n",info);
	//deleteWord(info, "\r\n");
	
	

}


/***************************************************************************************************
 * 函数名：USART2_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );
***************************************************************************************************/
void USART2_printf( char *Data, ... )
{
	const char *s;
  int d;   
  char buf[256];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USART2, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USART2, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USART2,*s);
						while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USART2,*s);
						while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USART2, *Data++);
		while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
	}
}


void USART2_printfByte( uint8_t *Data, int len )
{
	uint8_t temp;
	if(len>256)
		return;
	
	while ( len--)     // 判断是否到达字符串结束符
	{				                          
		temp =*Data++;
		USART_SendData(USART2,temp);
		//MC_INFO("%d,",temp);
		while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
	}
}



#if 0
void USART1_IRQHandler(void)                	//串口1中断服务程序
	{

if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断，可以扩展来控制
		{
	RxBuffer[RxCounter++] =USART_ReceiveData(USART1);//接收模块的数据
    } 
} 


void USART2_IRQHandler(void)                	//串口2中断服务程序
	{
if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收模块返回的数据
		{
			RxBuffer[RxCounter++] =USART_ReceiveData(USART2);//接收模块的数据
    
   } 

} 
#endif
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles USART2 Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler( void )
{	


	char ch;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		ch  = USART_ReceiveData( USART2 );
	
		
		
	}
	 	 
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) != RESET )                                         //数据帧接收完毕
	{
	
                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	
  }	

 
}


void USART1_IRQHandler(void)      //串口1 中断服务程序
{


  	char ch;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ch  = USART_ReceiveData( USART1 );
		
		
	
	}

	 	 
	if ( USART_GetITStatus( USART1, USART_IT_IDLE ) != RESET )                                         //数据帧接收完毕
	{
	
   	
	}

}


