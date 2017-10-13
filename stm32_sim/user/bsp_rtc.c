/***************************************************************************************************
	* Copyright (c) 2017, ETEK
	* All rights reserved.
	* 文件名称： bsp_rtc.c
	* 摘 要：    
	* 当前版本： V1.0, sommer jiang, 2017-01-15
	* 更改记录： 无
***************************************************************************************************/

#include "bsp_rtc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_bkp.h"
#include "usart.h"
#include <string.h>
#include "battery.h"
/***************************************************************************************************
//#define   
***************************************************************************************************/

/***************************************************************************************************
//#CONST
***************************************************************************************************/


/***************************************************************************************************
//#RAM
***************************************************************************************************/

extern BATTERY_REPORT batteryReporter;

/***************************************************************************************************
//#PROGRAM define
***************************************************************************************************/




/***************************************************************************************************
//#PROGRAM
***************************************************************************************************/
void getUtcTimeStr(){
	sprintf(batteryReporter.time.orig,"%02d-%02d-%02d  %02d:%02d:%02d",batteryReporter.time.year,batteryReporter.time.month,batteryReporter.time.day,batteryReporter.time.hour,batteryReporter.time.minute,batteryReporter.time.second);
	
}
void RCC_Configuration(void)
{

ErrorStatus HSEStartUpStatus;
  //复位RCC外部设备寄存器到默认值
  RCC_DeInit();
 
  //打开外部高速晶振
  RCC_HSEConfig(RCC_HSE_ON);
  
 
   //等待外部高速时钟准备好
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
 
  if(HSEStartUpStatus == SUCCESS)   //外部高速时钟已经准别好
  {     
   
 
  //配置AHB(HCLK)时钟=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
  
   //配置APB2(PCLK2)钟=AHB时钟
    RCC_PCLK2Config(RCC_HCLK_Div1); 
 
    //配置APB1(PCLK1)钟=AHB 1/2时钟
    RCC_PCLK1Config(RCC_HCLK_Div2);
 
     //配置PLL时钟 == 外部高速晶体时钟*9  PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
 
    //使能PLL时钟
    RCC_PLLCmd(ENABLE);
 
   //等待PLL时钟就绪
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
 
  //配置系统时钟 = PLL时钟
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
 
   //检查PLL时钟是否作为系统时钟
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
 
 
                         
}
 
void RTC_Configuration(void)
{

 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR| RCC_APB1Periph_BKP, ENABLE);
  
  PWR_BackupAccessCmd(ENABLE);

#if 0
  RCC_LSEConfig(RCC_LSE_OFF);

 while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

  RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
  #endif
 RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
  RCC_RTCCLKCmd(ENABLE);

  RTC_WaitForSynchro();

  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  RTC_WaitForLastTask();

  RTC_SetPrescaler(62498); 

  RTC_WaitForLastTask();

 

}
/*************************************************************
Function : Calendar_Time_Regulate
Description: 校准RTC时间
Input : none
return : none
*************************************************************/
 u32 Calendar_Time_Regulate(u8 hour,u8 minute,u8 second)
{
	u32 Tmp_HH = 0xff, Tmp_MM = 0xff, Tmp_SS = 0xff;

	MC_INFO("\r\n==================Time Settings================\r\n");

	while(Tmp_HH == 0xff)
	{
	Tmp_HH = hour;//输入小时数
	}

	while(Tmp_MM == 0xff)
	{
	Tmp_MM = minute;//输入分钟数
	}

	while(Tmp_SS == 0xff)
	{
	Tmp_SS = second;//输入秒数
	}


	return (Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS);//返回秒数
}
//上面代码来调整准确的当前时间。它会在串口上提示你输入小时数、分钟数、秒数。在这个函数中，定义了3个局部变量，分别用来保存要设置的小时、分钟、秒钟，它们的默认值都设0xff。最后在将他们全部转换成秒数值返回。输入的媒介也是串口，通过串口来输出这些要设定的时间，在这里调用Calendar_Adjust_Scanf()函数来作为输入的接口。下面就讲讲这个函数。
//Calendar_Adjust_Scanf()函数的代码如下：

//这个函数带有一个参数：value，用这个参数来限定输入数值的上限，以便防止错误输入。因为是串口输入的，它输入的是字符，需要将字符转换成十进制数字，然后在检测它是否有效，如果转换后的不是数字0~9，则需要提示输入有效的值，否则保存，等待输入第2个字符。如果输入的两个字符都是有效的，然后将他们分别作为个位与十位转换一个2位的整数，如果这个整数超过输入的上限值value，则提示重新输入。如果都正确，则返回这个歌值。
//接下去就要将上面输入并转换好的描述写进RTC了，代码如下：
/*************************************************************
Function : Calendar_Time_Adjust
Description: 设置当前时间：时、分、秒
Input : none
return : none
*************************************************************/
static void Calendar_Time_Adjust(u8 hour,u8 minute,u8 second)
{
	RTC_WaitForLastTask(); //等待操作完成
	RTC_SetCounter(Calendar_Time_Regulate(hour,minute,second));//修改当前时间
	RTC_WaitForLastTask(); //等待操作完成
}
//RTC通过RTC_SetCounter()这个函数将要设定的秒数写进RTC的计数寄存器中去。
//为了方便调试，可以打开RTC的时钟中断。上面的代码中，已经RTC的节拍设置为1s，所以如果打开RTC的中断，就会1s进入一次这个中断，我们则可以在这个中断函数中做些文章方便调试。所以这里还要配置下RTC的中断，代码如下：
/*************************************************************
Function : Calendar_Int_Config
Description: 设置RTC中断配置
Input : none
return : none
*************************************************************/
static void Calendar_Int_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; //设置RTC中断
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


}
void setUtcTime(UTC_TIME _utcTime)
{
//	memcpy(batteryReporter.time,_utcTime,sizeof(batteryReporter.time));
}
//还要编写一个总函数Calendar_init()，将上面的配置函数全部包含进来，代码如下：
/*************************************************************
Function : Calendar_init
Description: 日历初始化
Input : none
return : none
*************************************************************/
void Calendar_init(void)
{


	Calendar_Int_Config();//配置RTC秒中断
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)//查看BKP，判断RTC是否配置
	{
	MC_INFO("RTC not yet configured!\r\n");
		RCC_Configuration();
		RTC_Configuration();//RTC初始化
	MC_INFO("RTC configured!\r\n");
	Calendar_Time_Adjust(15,47,30);
	BKP_WriteBackupRegister(BKP_DR1, 0xA5A6);
	}
	else
	{
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)//上电复位标志
	{
	MC_INFO("Power on reset!\r\n");
	}
	else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)//引脚复位标志
	{
	MC_INFO("External reset occured!\r\n");
	}
	MC_INFO("No need to configure RTC\r\n");
	RTC_WaitForSynchro(); //等待RTC寄存器同步
	RTC_ITConfig(RTC_IT_SEC, ENABLE);//打开秒中断
	RTC_WaitForLastTask(); //等待对吼一个寄存器操作完成
	}
	RCC_ClearFlag();//清除复位标志
}
//这段代码，先是打开秒中断，然后再读取备份寄存器的数据寄存器1，看看是否等于0xA5A5。这里用备份寄存器的数据寄存器1来判断否需要配置的重新设置RTC。如果它的值不为0xA5A5，则重新配置RTC，输入初始时间，同时将值0xA5A5写入备份寄存器的数据寄存器1。如果数据寄存器的值为0xA5A5，则只需要打开中断就可以了。一般情况下，STM32有一个后备电源，单独为RTC模块提供电源，那么即使开发板上电或者案件复位，都不会影响RTC的正常工作。
//最后，当然要给出一个显示当前时间的函数，代码如下：
/*************************************************************
Function : Calendar_Time_Display
Description: 显示当前时间
Input : none
return : none
*************************************************************/
void Calendar_Time_Display(void)
{
	u32 THH = 0, TMM = 0, TSS = 0;
	u32 timeValue = 0;

	timeValue = RTC_GetCounter();
	if(timeValue == 0x0001517F) //0x0001515F对应的时间是23:59:59
	{
	RTC_SetCounter(0x0);//RTC时钟定时时间清零
	RTC_WaitForLastTask();//等待操作完成
	}

	THH = timeValue / 3600;//计算小时
	TMM = (timeValue % 3600) / 60;//计算分钟
	TSS = (timeValue % 3600) % 60;//计算秒
	batteryReporter.time.minute = TMM;
	batteryReporter.time.second = TSS;
	batteryReporter.time.hour = THH;
	getUtcTimeStr();
//	MC_INFO("Time: %0.2d:%0.2d:%0.2d\r\n", THH, TMM, TSS);//输出当前时间
}


