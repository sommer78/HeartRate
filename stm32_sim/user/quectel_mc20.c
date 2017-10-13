/***************************************************************************************************
	* Copyright (c) 2017, ETEK
	* All rights reserved.
	* 文件名称： QuectelM26.c
	* 摘 要：    M20模块
	* 当前版本： V1.0, sommer jiang, 2017-01-15
	* 更改记录： 无
***************************************************************************************************/
#include "quectel_mc20.h"
#include "stm32f10x_conf.h"	
#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "bsp_SysTick.h"
#include "ctype.h"
#include "i2c_oled.h"
#include "wdg.h"

/***************************************************************************************************
//#define   
***************************************************************************************************/


#define		CME_SHOW

#define 	MC20_POWER_ON 			GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define     MC20_POWER_OFF			GPIO_ResetBits(GPIOB,GPIO_Pin_12);


#define 	HTTP_TEST

/***************************************************************************************************
//#CONST
***************************************************************************************************/

//const char REPORT_SERVER[] = "http:\57\57api.efxnow.com\57DEMOWebServices2.8\57Service.asmx\57Echo";
//const char REPORT_SERVER[] = "http://222.191.229.234\57appuser\57report.do";
//const char REPORT_SERVER[] = "http://etek.f3322.net:8081/lekon/getbrand/6";
const char REPORT_SERVER[] = "http://etek.f3322.net:8081/batteryReport/report.do";

const char REPORT_SOCKET[] = "AT+QIOPEN=\042TCP\042,\042222.191.229.234\042,\0421883\042\r\n";


const char AT_AT[]="AT\r\n";
const char AT_GSN[]="AT+GSN\r\n";				//IMEI
const char AT_ATE_CLR[]="ATE0&w\r\n";				//关闭回显
const char AT_ATV_CLR[]="ATV0\r\n";				//TA 回显格式
const char AT_ATE_SET[]="ATE1&w\r\n";				//打开回显
const char AT_CMEE_ON[]="AT+CMEE=1\r\n";
const char AT_CMEE_OFF[]="AT+CMEE=0\r\n";
const char AT_QISTAT[]="AT+QISTAT\r\n";			//网络状态显示
const char AT_QHTTPGET[]="AT+QHTTPGET=60\r\n";
const char AT_QHTTPREAD[]="AT+QHTTPREAD=30\r\n";
const char AT_QIDEACT[]="AT+QIDEACT\r\n";			//关闭移动场景，意思就是关闭相关的数据连接
const char AT_QISEND[]="AT+QISEND\r\n";
const char AT_QISACK[]="AT+QISACK\r\n";
const char AT_QICLOSE[]="AT+QICLOSE\r\n";			//关闭连接
const char AT_CGATT_ZERO[]="AT+CGATT=0\r\n";
const char AT_QCELLLOC_REQ[]="AT+QCELLLOC=?\r\n";
const char AT_QCELLLOC_SET[]="AT+QCELLLOC=1\r\n";
const char AT_QGNSSRD_NMEARMC[]="AT+QGNSSRD=\042NMEA/RMC\042\r\n"; //接入APN，
const char AT_QGNSSC_REQ[]="AT+QGNSSC?\r\n";
const char AT_QGNSSC_SET[]="AT+QGNSSC=1\r\n";
const char AT_QGNSSC_CLR[]="AT+QGNSSC=0\r\n";
const char AT_CSQ[]="AT+CSQ\r\n";					//检查CSQ，可以利用手机APP查看CSQ值
const char AT_CPIN_REQ[]="AT+CPIN?\r\n";			//检查SIM卡是否在位
const char AT_CREG[]="AT+CREG?\r\n";				//查看是否注册GSM网络
const char AT_CREG_SET[]="AT+CREG=0\r\n";				//注册GSM网络
const char AT_CGREG[]="AT+CGREG?\r\n";				//查看是否注册GPRS网络
const char AT_CGREG_SET[]="AT+CGREG=0\r\n";				//查看是否注册GPRS网络
const char AT_QIFGCNT_ZERO[]="AT+QIFGCNT=0\r\n";  	//配置场景，为后面的数据连接做基础
const char AT_QIREGAPP[]="AT+QIREGAPP\r\n";			//开启移动场景
const char AT_QICSGP_SET_CMNET[]="AT+QICSGP=1,\042CMNET\042\r\n"; 
const char AT_QIACT[]="AT+QIACT\r\n";  				//PDP上下文激活]
const char AT_QUIT_SEND[]="+++";  						//退出透传模式，避免模块还处于透传模式中
const char AT_QIDNSIP_IP[]="AT+QIDNSIP=0\r\n"; 			//使用IP连接还是域名连接，0是IP 1是域名
const char AT_QIDNSIP_DOMAIN[]="AT+QIDNSIP=1\r\n"; 		//使用IP连接还是域名连接，0是IP 1是域名
const char AT_QIMODE_SET[]="AT+QIMODE=1\r\n";			//配置为透传模式,需要在配置场景前设定
const char AT_QIMODE_ZERO[]="AT+QIMODE=0\r\n";			//配置为透传模式,需要在配置场景前设定
const char AT_QITCFG[]="AT+QITCFG=3,2,512,1\r\n";		//重传次数3,200ms间隔,每次发送512个字节
const  char AT_QRST[]     = "AT+QRST=1,0\r\n";                    //模块复位
const  char AT_QPOWD[]    = "AT+QPOWD=0\r\n";                     //关闭模块

/***************************************************************************************************
//#RAM
***************************************************************************************************/
USARTX_Frame_REC MC20_Frame_Rec = { 0 };


//char *strx=0; 	//返回值指针判断

unsigned char LongLatistr[50];

char IMEI[16];






/***************************************************************************************************
//#PROGRAM define
***************************************************************************************************/
u8 deleteWord(char *orig,char *word);

void regularWord(char *respBuf);

/***************************************************************************************************
//#PROGRAM
***************************************************************************************************/

/***************************************************************************************************
 * 函 数 名:comma_split
 * 功能描述:根据, 把字符串进行分割
 * 函数说明: 
 * 调用函数:属于pstr根据','将字符串分割
 * 全局变量:
 * 输　　入: 字符串pstr ，最大buflen长度 开始s 结束e
 * 返　　回: 得到buf字符串
***************************************************************************************************/


int comma_split(unsigned char* pstr,char* buf,int buflen,int s,int e )
{
	int i , k ;
	int commaCount = 0;
	int rc = -1;
	i=k=0;
	if( e <= s )
	{
		return -1;
	}
	while(pstr[i]!='\0')
	{
		if( pstr[i]==',')
		{
			commaCount++;
		}
		if( commaCount == s )
		{
			k = ++i;
			break;
		}
		i++; 
	}

	while(pstr[i]!='\0')
	{
	if( pstr[i]==',')
	{
		commaCount++;
	}
	if( commaCount == e )
	{
		if( i - k + 1 > buflen)
		{
			return -1;
		}
		memset(buf,0,sizeof(buf));
		memcpy(buf,pstr+k,i-k);

		rc = i - k;
		buf[rc] ='\0';
		MC_INFO("%s\r\n",buf);
		break;
	}
	i++; 
	}
	return rc;
}

 /***************************************************************************************************
 * 函 数 名:getStringCharNums
 * 功能描述:得到pstr字符串中特殊字符c的个数
 * 函数说明: 
 * 调用函数:
 * 全局变量:
 * 输　　入: 字符串pstr 特定字符c
 * 返　　回: 得到pstr 中的c的个数
***************************************************************************************************/
int getStringCharNums(char* pstr,char c){
	int len = 0;
	int commaCount=0;


	// 计算字符串长度
	while(pstr[len] != '\0' )
	{
		if( pstr[len] == c)
		{
			commaCount++;
		}
	len++;
	}
	return commaCount;
 }
 /***************************************************************************************************
 * 函 数 名:GNRMC_Parse
 * 功能描述:解析GNRMC字符串
 * 函数说明: 
 * 调用函数:
 * 全局变量:
 * 输　　入: 字符串pstr 
 * 返　　回: 得到GNRMC_Info
***************************************************************************************************/

int GNRMC_Parse(unsigned char* pstr, GNRMC_Info* pGNRMCInfo )
{
	int len = 0;
	int commaCount=0;
	//double temp = 0;
	char buf[32]={0};
	// 计算字符串长度
	while(pstr[len] != '\0' )
	{
		if( pstr[len] == ',')
		{
			commaCount++;
		}
	len++;
	}
	// 逗号个数是12个
	if( commaCount != 12 )
	{
		return -1;
	}
	// 判断开始字符是:$GNRMC
	if( ! ((pstr[0]== '$') && (pstr[1]== 'G') && (pstr[2]== 'N') && (pstr[3]== 'R') && (pstr[4]== 'M') && (pstr[5]== 'C')))
	{
		return -1;
	}
	// 判断结束字符是:\r\n
	if( (pstr[len-2]!= '\r') && (pstr[len-1]!= '\n') )
	{
	//return -1;
	}
	// UTC时间:第1个逗号与第2个逗号之前 (eg:092846.400 hhmmss.sss )
	len = comma_split(pstr,buf,sizeof(buf),1,2);
	if( len < 0 )
	{
	return -1;
	}
	pGNRMCInfo->hour = (buf[0]-'0')*10+(buf[1]-'0');
	pGNRMCInfo->Minute = (buf[2]-'0')*10+(buf[3]-'0');
	pGNRMCInfo->second = (buf[4]-'0')*10+(buf[5]-'0');

	 


	// 定位状态:第2个逗号与第3个逗号之前
	len = comma_split(pstr,buf,sizeof(buf),2,3);
	if( len != 1 )
	{
	return -1;
	}
	pGNRMCInfo->position_valid = ((buf[0]=='A' ) ?1:0) ;
	if(pGNRMCInfo->position_valid ==0){
		return 1;
		}
	// 经度
	len = comma_split(pstr,buf,sizeof(buf),3,4);
	if( len < 0 )
	{
	return -1;
	}
	//temp = atof(buf);
	//temp = temp*10000;
	pGNRMCInfo->longitude = atof(buf)/100;
	// 经度半球E(东经)或W(西经)
	len = comma_split(pstr,buf,sizeof(buf),4,5);
	if( len != 1 )
	{
	return -1;
	}
	pGNRMCInfo->longitude_suffix = buf[0];
	// 纬度
	len = comma_split(pstr,buf,sizeof(buf),5,6);
	if( len < 0 )
	{
		return -1;
	}
	//temp = atof(buf);
	//temp = temp*10000;
	pGNRMCInfo->latitude = atof(buf)/100;

	 

	// 纬度半球N(北半球)或S(南半球)
	len = comma_split(pstr,buf,sizeof(buf),6,7);
	if( len != 1 )
	{
		return -1;
	}
	pGNRMCInfo->latitude_suffix = buf[0];


	// UTC日期:ddmmyy(日月年)格式 eg:070417
	len = comma_split(pstr,buf,sizeof(buf),9,10);
	if( len < 0 )
	{
		return -1;
	}
	pGNRMCInfo->day = (buf[0]-'0')*10+(buf[1]-'0');
	pGNRMCInfo->month = (buf[2]-'0')*10+(buf[3]-'0');
	pGNRMCInfo->year = (buf[4]-'0')*10+(buf[5]-'0') + 2000;

	return 0;
}

/***************************************************************************************************
 * 函 数 名: getRespCmdData
 * 功能描述: 得到resp的返回值中的数据字符串
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: respBuf
 * 返　　回: status: 有数据len  无数据0 data 字符串
***************************************************************************************************/
  int getRespCmdData(char* respBuf, char* data ){


	int len;
	char *strx=0; 	//返回值指针判断
	
	strx =  strstr(respBuf,":") ;
	if(strx){
	
		deleteWord(strx,":");	
		regularWord(strx);
		len = strlen(strx); 
		memcpy(data,strx,len);
		return len;
		}else {
		return 0;
			}

	

}



/***************************************************************************************************
 * 函 数 名: clrRecBuffer
 * 功能描述: 清空接收buf的值
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: 无
 * 返　　回: 
***************************************************************************************************/

void clrRecBuffer(void){ 
	memset( MC20_Frame_Rec.FrameBuffer,0,RX_BUF_MAX_LEN);
	MC20_Frame_Rec.FramFinishFlag = 0;
	MC20_Frame_Rec.FramLength = 0;
	IWDG_Feed();
	
}

/*
u16 getATCommand(char * cmdStr, char *cmdAT){
	u16 len = 0;
	len = sprintf(cmdStr,"%s\r\n",cmdAT);
	return len;
}
*/
u16 getATDataPackage(char * data, char *cmdAT){
	u16 len = 0;
	len = sprintf(data,"%s\x1a",cmdAT);

	return len;
}

u16 getATDataBytePackage(char * data, char *cmdAT){
	u16 len = 0;
	len = sprintf(data,"%s\x1a",cmdAT);

	return len;
}

void printArray(char * data,int len){
	int i=0;
	MC_INFO("array:[");
	for (i=0;i<len;i++)
	//MC_INFO("[%d]=0x%x,",i,data[i]);
	MC_INFO("0x%x,",data[i]);
	MC_INFO("] \r\n");
}





/***************************************************************************************************
 * 函 数 名: deleteWord
 * 功能描述: 删除字符串里面的字符
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入:字符串orig 字符串word
 * 返　　回: 删除后的orig
***************************************************************************************************/

u8 deleteWord(char *orig,char *word)
{
	int data[256]={0};
	int i=0;
	int j= 0;
	int len1=strlen(orig);
	int len2=strlen(word);
	char* strx=0;
	strx=strstr((const char*)orig,(const char*)word);
	if(strx){
		for(i=0;i<len2;i++)

		data[(unsigned char)word[i]]++;

		for(j=0;j<len1;j++)
		{
			if(data[(unsigned char)orig[j]]>0)
			{
			i=j;
			for(;i<len1-1;i++)
				{
				orig[i]=orig[i+1];
				}
			orig[i]=0;
			j--;
			}
		}
		return i;
	}else {
	return 0;
		}

}

int deleteChar(char *s,char c){

    int j, k;
	  for(j=k=0; s[j]!='\0'; j++)
        if(s[j]!=c)
            s[k++]=s[j];
    s[k]= '\0';
	return k+1;
}



/***************************************************************************************************
 * 函 数 名: regularWord
 * 功能描述: 删除字符串里面的无用字符
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入:字符串orig 字符串word
 * 返　　回: 删除后的orig
***************************************************************************************************/
void regularWord(char *respBuf)
{
	deleteWord(respBuf,"OK");
	deleteWord(respBuf,"\r\n");
	deleteWord(respBuf," ");
	deleteWord(respBuf,"+");	

}
/***************************************************************************************************
 * 函 数 名: split
 * 功能描述: text
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入:字符串text  字符串parts
 * 返　　回: 删除后的字符串parts[]
***************************************************************************************************/
int split(char text[], char* parts[])
{
	int count = 0; // 分段的个数
	int start = 0; // 每一分段的首地址
	u8 flag = 0; // 遍历text，标识当前是否处于有效字符
	int i;

	for ( i = 0; i<256; i++)
	{
	char ch = text[i];


	if (ch == ',' || ch == '\0' || ch == ' ' || ch == '\t')//遇到分隔符，换行符不是分隔符
	{
		if (flag) // 如果当前flag==true
		{
			flag = 0;//如果遇着连续的分隔符不进入这里


			text[i] = '\0'; // 修改为结束符,完成分段
			parts[count] = text + start; // 记录这一分段的首地址
			count++;
		}
	}
	else//遇到有效字符
	{
		if (flag==0) // 如果当前flag==false
		{
			flag = 1;//遇到连续的有效数字不进入这里
			start = i;
		}
	}
	if (ch == '\0')//进一步判断 如果遇到的分隔符是\0则终止循环，不读取\0后面的字符
	break; // 结束循环
	}
	return count; // 返回分段个数,表示parts含有的字符串个数
}






/***************************************************************************************************
 * 函 数 名: getRespErr
 * 功能描述: 得到resp的返回值
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: respBuf返回值原始数据
 * 返　　回: GSM_RESP
***************************************************************************************************/
 static GSM_ERROR getRespErr(char* respBuf ){
 	char buff[32];

	GSM_ERROR gError ;
   
	char *strx=0; 	//返回值指针判断
	
	strx =  strstr(respBuf,":") ;
	if(strx){
		deleteWord(strx,":");	
		deleteWord(strx," ");	
		strcpy(gError.detail,strx);
		gError.type = 1;
		}else {
		gError.type =0;
			}
	if(gError.type){
	sprintf(buff,"Error[1]: %s",gError.detail);
			}else {
			sprintf(buff,"Error[0]");
				}
	debugInfo(buff, 7);
//	MC_INFO("gError type = %s code = %d error = %s \r\n",gError.type,gError.code,gError.detail);
	
	return gError;
	

}

/***************************************************************************************************
 * 函 数 名: getGSMAckData
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: buff 需要比较的数组， *string给定字符串指针 overtime 100ms
 * 返　　回: 状态
***************************************************************************************************/
 u16 getGSMRecData(char *str, u16 overtime)
{


    while (overtime--)
    {
   
     Delay_ms(100);
	 IWDG_Feed();
	if(MC20_Frame_Rec.FramFinishFlag>0){

		
	 printArray(MC20_Frame_Rec.FrameBuffer,MC20_Frame_Rec.FramLength);
	// MC_INFO("Data:[ %s ] \r\n",MC20_Frame_Rec.FrameBuffer);
	 
	return MC20_Frame_Rec.FramLength;
    }

}
	//	MC_INFO("getGSMAckData OVER_TIME \r\n");
	// gsmResp.resp =OVER_TIME;
	//	  	return gsmResp;
 return 0;
}



/***************************************************************************************************
 * 函 数 名: getGSMAckData
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: buff 需要比较的数组， *string给定字符串指针 overtime 100ms
 * 返　　回: 状态
***************************************************************************************************/
static u8 getGSMAckData(char *str, u16 overtime)
{

	char *retStr=0;

    while (overtime--)
    {
   
     Delay_ms(100);
	 IWDG_Feed();
	if(MC20_Frame_Rec.FramFinishFlag>0){

		
	  MC_INFO("Rec:[ %s ] \r\n",MC20_Frame_Rec.FrameBuffer);
	  retStr = strstr ((const char*) MC20_Frame_Rec .FrameBuffer,"ERROR") ;
	  if( retStr!=NULL){
	  	//  MC_INFO("Error %s ",MC20_Frame_Rec.FrameBuffer);
		//  gsmResp.resp =RESP_ERR;
		getRespErr(MC20_Frame_Rec.FrameBuffer);
	  	return RESP_ERR;
	  	}
	  if(str!=NULL){
		 
		  retStr = strstr ((const char*) MC20_Frame_Rec .FrameBuffer,(const char*) str ) ;
		  if( retStr!=NULL){
		
			 return MC20_OK;
			
			}else{
			
			 return STRING_ERR;
			}
		}else {
		
		return MC20_OK;
		}
	
		
    }

}
	//	MC_INFO("getGSMAckData OVER_TIME \r\n");
	// gsmResp.resp =OVER_TIME;
	//	  	return gsmResp;
 return OVER_TIME;
}





/***************************************************************************************************
 * 函 数 名: getGSMAckGNSSData
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: buff 需要比较的数组， *string给定字符串指针 overtime 100ms
 * 返　　回: 状态
***************************************************************************************************/
//char* gsm ="$GPRMC,015606.000,A,3150.7584,N,11712.0491,E,0.00,231.36,280715,,,A*67\r\n";
//  char* gnss ="\r\n+QGNSSRD: $GNRMC,133051.000,A,3133.0821,N,12020.5020,E,1.83,203.99,310517,,,A*70\r\nOK\r\n";

static u8 getGSMAckGNSSData( u16 overtime,   GNRMC_Info *GNRMC)
{
 u8 ret=0;
 char pstr[128]={0};


 ret =  getGSMAckData(NULL,overtime);
  if(ret==MC20_OK){
  //	memcpy(MC20_Frame_Rec.FrameBuffer,gnss,strlen(gnss));
		getRespCmdData(MC20_Frame_Rec.FrameBuffer,pstr);
		ret = GNRMC_Parse((unsigned char *)pstr,GNRMC);
		if(ret==0){
			sprintf(pstr,"%c %f",GNRMC->longitude_suffix,GNRMC->longitude);
			debugInfo(pstr, 4);
			sprintf(pstr,"%c %f",GNRMC->latitude_suffix,GNRMC->latitude);
			debugInfo(pstr, 5);
		//MC_INFO("time:%d-%d-%d %d:%d:%d\n\r",GNRMC.year,GNRMC.month,GNRMC.day,GNRMC.hour,GNRMC.Minute,GNRMC.second);
		//MC_INFO("%f %c ---- %f %c local:%d\n\r",GNRMC.longitude,GNRMC.longitude_suffix,GNRMC.latitude,GNRMC.latitude_suffix,GNRMC.position_valid);
			}else if(ret == 1){
				
			sprintf(pstr,"GNSSData invalid");
			debugInfo(pstr, 4);
			}else {
			
			sprintf(pstr,"GNSSData error");
			debugInfo(pstr, 4);
			}
			return MC20_OK;
  	}
	return ret;
   
}

/***************************************************************************************************
 * 函 数 名: getGSMIMEI
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: buff 需要比较的数组， *string给定字符串指针 overtime 100ms
 * 返　　回: 状态
***************************************************************************************************/
static u8 getGSMIMEI(char *numStr, u16 overtime)
{
  u8 ret=0;

  ret =  getGSMAckData(NULL,overtime);
  if(ret==MC20_OK){
  	regularWord(MC20_Frame_Rec.FrameBuffer);

	memcpy(numStr,MC20_Frame_Rec.FrameBuffer,strlen(MC20_Frame_Rec.FrameBuffer));
  	debugInfo(numStr,2);
	return MC20_OK;
	
  	}
//	MC_INFO("getGSMAckData OVER_TIME \r\n");
	 return GSM_IMEI_ERR;
}


/***************************************************************************************************
 * 函 数 名: getCREG
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: buff 需要比较的数组， *string给定字符串指针 overtime 100ms
 * 返　　回: 状态
***************************************************************************************************/
static u8 getGSMNum(char *numStr, u16 overtime)
{
  u8 ret=0;

  char pstr[64]={0};
  char* data[4];
  int len;
  int i;
  ret =  getGSMAckData(NULL,overtime);
  if(ret==MC20_OK){

	getRespCmdData(MC20_Frame_Rec.FrameBuffer,pstr);
	len = getStringCharNums(pstr,',');
	MC_INFO("%s len: %d \r\n",pstr,len);
	if(len>0){
		len = split(pstr, data);
	for ( i = 0; i < len; i++){
		numStr[i]=atol(data[i]);
		}
	sprintf(pstr," num1[%d],num2[%d] ",numStr[0],numStr[1]);
		debugInfo(pstr,3);
		return MC20_OK;
	}else {
		numStr[0]=atol(pstr);
		sprintf(pstr,"num:[%d]",numStr[0]);
		debugInfo(pstr,3);
		return MC20_OK;
		}

	
  	}
//	MC_INFO("getGSMAckData OVER_TIME \r\n");
	 return GSM_NUM_ERR;
}


/***************************************************************************************************
 * 函 数 名: getCREG
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: buff 需要比较的数组， *string给定字符串指针 overtime 100ms
 * 返　　回: 状态
***************************************************************************************************/
static u8 getGSMNumFloat(double *numStr, u16 overtime)
{
  u8 ret=0;

  char pstr[64]={0};
  char* data[4];
  int len;
  int i;
  ret =  getGSMAckData(NULL,overtime);
  if(ret==MC20_OK){

	getRespCmdData(MC20_Frame_Rec.FrameBuffer,pstr);
	len = getStringCharNums(pstr,',');
	MC_INFO("%s len: %d \r\n",pstr,len);
	if(len>0){
		len = split(pstr, data);
	for ( i = 0; i < len; i++){
		numStr[i]=atof(data[i]);
		}
	//sprintf(pstr," Float1[%f],Float2[%f] ",numStr[0],numStr[1]);
	//	debugInfo(pstr,3);
		return MC20_OK;
	}else {
		numStr[0]=atof(pstr);
	//	sprintf(pstr,"Float:[%f]",numStr[0]);
	//	debugInfo(pstr,3);
		return MC20_OK;
		}

	
  	}
//	MC_INFO("getGSMAckData OVER_TIME \r\n");
	 return GSM_NUM_FLOAT_ERR;
}
/***************************************************************************************************
 * 函 数 名: noGSMAckResp
 * 功能描述: 比较接收的数据和给定字符串是否相同
 * 函数说明: 无
 * 调用函数: 无
 * 全局变量: 无
 * 输　　入: 
 * 返　　回: 状态
***************************************************************************************************/
static u8 noGSMAckResp(u16 overtime)
{

  u8 ret=0;
  ret =  getGSMAckData(NULL,overtime);
  return ret;
}



/***************************************************************************************************
 * 函 数 名: getGSMStateInfo
 * 功能描述: GSM模组 状态值解析
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  GSM_STATE
  ***************************************************************************************************/

GSM_STATE getGSMStateInfo(char *str){


	
    if(strstr ((const char*) str,"IP GPRSACT")){
		return IP_GPRSACT;
    	}

	if(strstr ((const char*) str,"IP INITIAL")){
		return IP_INITIAL;
		}
	if(strstr ((const char*) str,"CONNECT OK")){
		return CONNECT_OK;
		}
	if(strstr ((const char*) str,"IP CLOSE")){
		return IP_CLOSE;
		}
	if(strstr ((const char*) str,"TCP CONNECTING")){
		return CONNECTING;
		}
	if(strstr ((const char*) str,"IP START")){
		return IP_START;
		}
	if(strstr ((const char*) str,"PDP DEACT")){
		return PDP_DEACT;
		}
	if(strstr ((const char*) str,"IP STATUS")){
		return IP_STATUS;
		}
	if(strstr ((const char*) str,"IP IND")){
		return IP_IND;
		}
   return UNKONW_STATE;

}

/***************************************************************************************************
 * 函 数 名: getGSMState
 * 功能描述:得到GSM模组的状态值
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  GSM_STATE
  ***************************************************************************************************/
GSM_STATE getGSMState(void){
	int ret;
	GSM_STATE state;
	QuectelMC20_SendCmd(AT_QISTAT);
 	ret = getGSMAckData(NULL,100);
	if(ret== MC20_OK){
	state =	getGSMStateInfo(MC20_Frame_Rec.FrameBuffer);
	regularWord(MC20_Frame_Rec.FrameBuffer);
	debugInfo(MC20_Frame_Rec.FrameBuffer, 3);
	//MC_INFO("state: %d\r\n",state);
	return state;
		}
	else if(ret == OVER_TIME) {
		return NO_RESP;
		}else {
		return UNKONW_STATE;
			}

}



/***************************************************************************************************
 * 函 数 名: QuectelMC20_SendCmd
 * 功能描述: M26底层发送命令
 * 函数说明: 
 * 调用函数: 无
 * 全局变量:
 * 输　　入: *string 需要发送的字符串指针
 * 返　　回: 无
***************************************************************************************************/
void QuectelMC20_SendCmd( const char *str)
{
	
	char send_buff[RX_BUF_MAX_LEN];
	int slen = strlen(str);
	memcpy(send_buff,str,slen);
	MC20_Usart(send_buff);
	
	clrRecBuffer();
	
	MC_INFO("CMD: %s",send_buff);
	//TIM3_Int_Init(499,7199);//10Khz?????,???5000?500ms  

}



void QuectelMC20_SendData( char *str)
{
	
	u16 len = 0;
	char send_buff[RX_BUF_MAX_LEN];
	len = getATDataPackage(send_buff,str);
	if(len <= 0){
		return;
		}
	MC20_Usart(send_buff);
	
	clrRecBuffer();

	
	MC_INFO("DATA:[ %s ]\r\n",send_buff);

}
void QuectelMC20_SendDataByte( uint8_t *str,int len)
{
	

	uint8_t send_buff[RX_BUF_MAX_LEN];
	memcpy(send_buff,str,len);
	send_buff[len] = '\x1a';
	len++;
	
	if(len <= 0){
		return;
		}
	printArray(send_buff,  len);
	USART2_printfByte(send_buff,len);
	
	clrRecBuffer();

	
	//MC_INFO("DATA:[ %s ]\r\n",str);

}


void printError(int error){
	switch(error){
		case 3801:
			MC_INFO("HTTP time out");
			break;
		default:
			MC_INFO("Unknown error ");
			break;
		}


}



void sendHttpGetData(char *data){
	int len,size;
	int ret;
	char sendBuf[256]={0};
	size = strlen(REPORT_SERVER);
	getGSMState();
	Delay_ms(1000);
	getGSMState();
	len = sprintf(sendBuf,"AT+QHTTPURL=%d,%d",size,30);
    printf("len = %d",len);
	if(len<=0)
		return;
	QuectelMC20_SendCmd(sendBuf);
 	ret = noGSMAckResp( 100);

	memcpy(sendBuf,REPORT_SERVER,size);
	QuectelMC20_SendCmd(sendBuf);
	ret = noGSMAckResp( 100);
	Delay_ms(1000);

	QuectelMC20_SendCmd("AT+QHTTPGET=60");
 	ret = noGSMAckResp( 100);

	Delay_ms(1000);
	//QuectelMC20_SendCmd(data);
	//ret = noGSMAckResp( 100);

	QuectelMC20_SendCmd("AT+QHTTPREAD=30");
 	ret = noGSMAckResp( 100);
	getGSMState();
	if(ret==MC20_OK){
		debugInfo(MC20_Frame_Rec.FrameBuffer, 0);
		}



	QuectelMC20_SendCmd("AT+QIDEACT ");
 	ret = noGSMAckResp( 5); 
}


/***************************************************************************************************
 * 函 数 名: sendPostData
 * 功能描述: 打开mc20
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/
u8 sendHttpPostData(char *data,int datalen){
	int len,size;
	int ret;
	char sendBuf[256]={0};
	size = strlen(REPORT_SERVER);
	len = sprintf(sendBuf,"AT+QHTTPURL=%d,%d",size,30);
	getGSMState();
	Delay_ms(1000);
	getGSMState();
	QuectelMC20_SendCmd(sendBuf);
 	ret = noGSMAckResp( 20);

	memcpy(sendBuf,REPORT_SERVER,size);
	QuectelMC20_SendCmd(sendBuf);
	ret = noGSMAckResp( 20);


//	QuectelMC20_SendCmd("AT+QHTTPGET=60");
// 	ret = noGSMAckResp( 100);
	
	
	size = strlen(data);
	len = sprintf(sendBuf,"AT+QHTTPPOST=%d,%d,%d",size,50,10);
	printf("len = %d",len);
	QuectelMC20_SendCmd(sendBuf);
	ret = noGSMAckResp( 20);
//	len = sprintf(sendBuf,"%s\r\n",data);
	QuectelMC20_SendCmd(data);
	ret = noGSMAckResp( 20);

	QuectelMC20_SendCmd("AT+QHTTPREAD=30");
 	ret = noGSMAckResp( 20);
//	getGSMState();
	if(ret==MC20_OK){
		debugInfo(MC20_Frame_Rec.FrameBuffer, 0);
		}
	return ret;
	//QuectelMC20_SendCmd("AT+QIDEACT ");
 //	ret = noGSMAckResp( 5); 

}
u8 sendTestHttpData(char*data)
{

	u8 ret;
	//GSM_STATE gsmState;
	#ifdef HTTP_TEST
	//gsmState = getGSMState();


	
	//gsmState = getGSMState();
//	QuectelMC20_SendCmd("AT+QIDNSIP=1"); //使用IP连接还是域名连接，0是IP 1是域名
 //	ret = noGSMAckResp( 10); 
	
//	Delay_ms(1000);
//	getQuecLocatorAGPS();
//	sendHttpPostData("Message=3.14",18);
//	sendHttpGetData("test");
	//sendPostData("level=3.14",18);
//	Delay_ms(1000);
//	QuectelMC20_SocketDataSend("mc20 test");
	

	#endif 

	return ret;
}  

/***************************************************************************************************
 * 函 数 名: sendTestSocketData
 * 功能描述: send test data throuth socket
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/

u8 sendTestSocketData(char*data)
{

	u8 ret;
	int size;
	GSM_STATE state;
	char sendBuf[256]={0};
	
	size = strlen(REPORT_SOCKET);
	memcpy(sendBuf,REPORT_SOCKET,size);
	QuectelMC20_SendCmd	(sendBuf);
	ret = getGSMAckData("CONNECT OK", 50);     
	state = getGSMState();
	printf("state = %d",state);
	QuectelMC20_SendCmd("AT+QISEND");//????
	ret = getGSMAckData(">", 500);
	QuectelMC20_SendData(data);
	ret = getGSMAckData("SEND OK", 100);
	state = getGSMState();
	QuectelMC20_SendCmd("AT+QICLOSE");//close
	ret = getGSMAckData(NULL, 500);
	state = getGSMState();
	QuectelMC20_SendCmd("AT+CGATT=0");
 	ret = noGSMAckResp( 5); 
	getGSMState();
	QuectelMC20_SendCmd("AT+QIDEACT");
 	ret = noGSMAckResp( 5); 
	getGSMState();
	return ret;
}  

/***************************************************************************************************
 * 函 数 名: connectSocket
 * 功能描述:connect socket
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/

u8 connectSocket(void)
{

	u8 ret;
	int size;
	char sendBuf[64]={0};
	size = strlen(REPORT_SOCKET);
	memcpy(sendBuf,REPORT_SOCKET,size);
	QuectelMC20_SendCmd	(sendBuf);
	ret = getGSMAckData("CONNECT OK", 50);    
	if(ret==MC20_OK){
		}
	return ret;
}  

/***************************************************************************************************
 * 函 数 名: closeSocket
 * 功能描述: close socket
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/

u8 closeSocket(void)
{

	u8 ret;

	
	QuectelMC20_SendCmd(AT_QICLOSE);//close
	ret = getGSMAckData(NULL, 500);
	
	return ret;
}  

/***************************************************************************************************
 * 函 数 名: contextAction
 * 功能描述: contextAction 
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/
uint8_t contextAction(){
	uint8_t ret;
	
	
	
	QuectelMC20_SendCmd(AT_QIACT);
 	ret = noGSMAckResp( 10); 
	
	return ret;

}

/***************************************************************************************************
 * 函 数 名: gsmNetRegistor
 * 功能描述: gsmNetRegistor 
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/
uint8_t gsmNetRegistor(){
	uint8_t ret;
	QuectelMC20_SendCmd(AT_QIFGCNT_ZERO);
 	ret = noGSMAckResp( 5);     
	
	QuectelMC20_SendCmd(AT_QICSGP_SET_CMNET);
 	ret = noGSMAckResp( 5); 
	
	
	QuectelMC20_SendCmd(AT_QIREGAPP);
 	ret = noGSMAckResp( 10); 
	
	
	
	return ret;

}



/***************************************************************************************************
 * 函 数 名: contextDeAction
 * 功能描述: contextDeAction 
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/
uint8_t contextDeAction(){
	uint8_t ret;
	
	
	
	QuectelMC20_SendCmd(AT_QIDEACT);
 	ret = noGSMAckResp( 10); 
	
	return ret;

}


/***************************************************************************************************
 * 函 数 名: sendSocketData
 * 功能描述: send data throuth socket
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/

u8 sendSocketData(uint8_t *data,int len)
{

	u8 ret;
	//printArray(data,len);
	QuectelMC20_SendCmd(AT_QISEND);
	ret = getGSMAckData(">", 500);
	if(ret!=MC20_OK)
		return ret;
	QuectelMC20_SendDataByte(data,len);
	ret = getGSMAckData("SEND OK", 100);
	QuectelMC20_SendCmd(AT_QISACK);
	ret = getGSMAckData(NULL, 500);
	clrRecBuffer();
	return ret;
}  

/***************************************************************************************************
 * 函 数 名: getQuecLocatorAGPS
 * 功能描述: 得到模块agps的值
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  是否得到值
  ***************************************************************************************************/
u8 getQuecLocatorAGPS(GNRMC_Info *GNRMC){
	int ret;
	double dData[4];

	//QuectelMC20_SendCmd("AT+QCELLLOC=?");
 	//ret = noGSMAckResp(10);

	QuectelMC20_SendCmd("AT+QCELLLOC=1");
 	ret = getGSMNumFloat(dData,100);
	if(ret==MC20_OK){
		GNRMC->longitude= dData[0];
		GNRMC->latitude= dData[1];
		GNRMC->longitude_suffix ='E';
		GNRMC->latitude_suffix ='N';
		GNRMC->position_valid =2;
		}
	return ret;

}

/***************************************************************************************************
 * 函 数 名: getGnssData
 * 功能描述: 得到模块的gnss模块
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
  ***************************************************************************************************/
u8 getGnssData(GNRMC_Info *GNRMC){
	int ret;
	QuectelMC20_SendCmd("AT+QGNSSRD=\042NMEA/RMC\042");
 	ret = getGSMAckGNSSData(100, GNRMC);
	if(ret==MC20_OK){
		return ret;
		//OLED_Fill(0);
		//debugInfo(MC20_Frame_Rec.FrameBuffer, 0);
		}else {
		return GET_GNSS_ERR;
		//debugInfo(MC20_Frame_Rec.FrameBuffer, 0);
			}
}



/***************************************************************************************************
 * 函 数 名: OPEN_MC20
 * 功能描述: 打开mc20
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
 ***************************************************************************************************/
uint8_t OPEN_MC20(void)
{
	u8 ret=0 ,i=3;

    while (i--)                                            //连续发送5次，判断是否能连通
    {
    QuectelMC20_SendCmd(AT_AT);
    ret = getGSMAckData("OK", 10);                      
    if (ret== MC20_OK)
		
        return ret;
    }
	
	
	MC20_POWER_OFF;
	Delay_ms(500);
	
	MC20_POWER_ON;
	Delay_ms(500);
	Delay_ms(500);
	Delay_ms(500);
	i= 10;
	 while (i--)                                            //连续发送5次，判断是否能连通
    {
     QuectelMC20_SendCmd(AT_AT);
     ret = getGSMAckData("OK", 10);                      
     if (ret == MC20_OK)
        return ret;
    }
	return ret;
		
}
/***************************************************************************************************
 * 函 数 名: getIMEI
 * 功能描述: 打开mc20
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  1 ok 0 failed
 ***************************************************************************************************/
void getIMEI(char *imei)
{
  unsigned char i=5;
	int ret;
	
	 while (i--)                                            //连续发送5次，判断是否能连通
    {
  	QuectelMC20_SendCmd(AT_GSN); //IMEI
    ret = getGSMIMEI(imei, 5);                      
    if (ret == MC20_OK)
        break;
    }
	//debugInfo(nums,2);
   
   
}

/***************************************************************************************************
 * 函 数 名: MC20_Init
 * 功能描述:mc20 init
 * 函数说明:
 * 调用函数: 无
 * 全局变量:
 * 输　　入:
 * 返　　回:  MC20 error_info
 ***************************************************************************************************/

u8  MC20_Init(void)
{
	int i,ret;
	
	char nums[4];
	QuectelMC20_SendCmd(AT_ATE_CLR); //关闭回显
    ret = noGSMAckResp( 5);
#ifdef CME_SHOW
	QuectelMC20_SendCmd(AT_CMEE_OFF);//查看错误命令
    ret = getGSMAckData("OK", 5);  	
#endif 

    getIMEI(IMEI);//获取IMEI值

#if 1
	QuectelMC20_SendCmd(AT_QGNSSC_REQ);//检查上电情况
    ret = getGSMNum(nums, 5);     

	if(nums[0]==0){
		QuectelMC20_SendCmd(AT_QGNSSC_SET); //对GPS模块上电
    ret = noGSMAckResp( 5);  
		}
#endif 
	i= 20;
	while(i--){

	QuectelMC20_SendCmd(AT_CSQ);		//检查CSQ，
    ret = getGSMNum(nums, 50);   
	if(ret!=MC20_OK)
		return CSQ_ERR;
	if(nums[1]==0&&(nums[0]>10&&nums[0]<50)){
		break;
		}
	}
    if(i==0){
		return CSQ_ERR;
    	}
	QuectelMC20_SendCmd(AT_CPIN_REQ);	//检查SIM卡是否在位
    ret = getGSMAckData("READY" ,5);  
	if(ret!=MC20_OK){
		return SIM_NOT_READY;
		}

	QuectelMC20_SendCmd(AT_CREG);	//查看是否注册GSM网络
    ret = getGSMNum(nums, 5);  
   if(ret!=MC20_OK)
		return CREG_ERR;
	if(nums[1]!=1&&nums[1]!=5)
		return CREG_ERR;
	QuectelMC20_SendCmd(AT_CGREG);	//查看是否注册GPRS网络
    ret = getGSMNum(nums, 5); 
	   if(ret!=MC20_OK)
		return CGREG_ERR;
		if(nums[1]!=1&&nums[1]!=5)
		return CGREG_ERR;
//	debugInfo("CGREG OK",4);
			
//	QuectelMC20_SendCmd("AT+QGNSSRD=\042NMEA/RMC\042");
  
//  ret = getGNSSData( 5);


		return 0;
}		




