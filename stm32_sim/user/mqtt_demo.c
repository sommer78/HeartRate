#include <stdio.h>
#include <string.h>
#include "mqtt_demo.h"
#include "MQTTPacket.h"
#include "quectel_mc20.h"
#include "bsp_SysTick.h"
#include "usart.h"
#include "bsp_gpio.h"
#include "wdg.h"
void Heartbeat(uint16_t sec);
	


char payload[] = "10s!";
u8 shanshuo = 0;

char data_w_s_t[10];

//接收段所需变量
int len1;
//uint8_t buf_sub[200];
//uint8_t buf_send[200];
//uint8_t buf_rec[200];
uint8_t len_send = 4;

uint8_t heartbeat_time = 0;
char num[11] = {'0','1','2','3','4','5','6','7','8','9'};
int time_send;

char respTemp[] ={0x30,0x1b,0x0,0xa,0x74,0x65,0x73,0x74,0x2f,0x74,0x6f,0x70,0x69,0x63,0x72,0x65,0x73,0x70,0x3a,0x32,0x30,0x31,0x37,0x2d,0x30,0x36,0x2d,0x32,0x39};

static int recLen;
u8 transport_sendPacketBuffer(uint8_t* buf, int32_t buflen)
{

	sendSocketData(buf,buflen);	
	return 0;
}

int16_t waitRespData(uint8_t* buf){
	int32_t overtime;
	overtime = 200;
	while (overtime--)
    {
     MC_INFO("overtime %d\n\r", overtime);
     Delay_ms(100);
	 IWDG_Feed();	
		if(MC20_Frame_Rec.FramFinishFlag>0){
			memcpy(buf,MC20_Frame_Rec.FrameBuffer,MC20_Frame_Rec.FramLength)	;
			printArray(MC20_Frame_Rec.FrameBuffer, MC20_Frame_Rec.FramLength);
			return MC20_Frame_Rec.FramLength;
		
		}
	
	
    }
	return 0;

}




int transport_getdata(unsigned char * buf, int count)
{
	int rc=0;
	while(count--){
		buf[rc] = MC20_Frame_Rec.FrameBuffer[recLen++];
		rc++;
		}
	
	return rc;
}


void testRecvalue(){
	uint8_t buf[256];
//	unsigned char sessionPresent, connack_rc;
	int buflen = 256;
	unsigned char dup;
	int qos;
	unsigned char retained;
	unsigned short msgid;
	int payloadlen_in;
	unsigned char* payload_in;
//	int rc;
	MQTTString receivedTopic;
	recLen = 0;

	MC_INFO("DEBUG START \r\n");
	memcpy(MC20_Frame_Rec.FrameBuffer,respTemp,sizeof(respTemp));
	MC20_Frame_Rec.FramFinishFlag=1;
	MC20_Frame_Rec.FramLength = sizeof(respTemp);
	if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
		
			MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			
	MC_INFO("message arrived %.*s\r\n", payloadlen_in, payload_in);
		}

		
	MC_INFO("publishing reading\r\n");
		
}

#if 0
int32_t transport_getdata(uint8_t* buf, int32_t count)
{
	int32_t n, rc = 0;
	
			
	
	do{
		n = recv(SOCK_TCPC,&buf[rc], count - rc);
		rc += n;
	}while(rc < count);

	return rc;
}
#endif

void mqtt_config(void)
{


	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	MQTTString topicString = MQTTString_initializer;
			
	int32_t msgid = 1;
	int32_t req_qos = 0;
	int32_t payloadlen, len;
	uint8_t buf[256];
	int loop = 0;
	int buflen = sizeof(buf);
	/* 初始化 */
	MC_INFO("MQTT Connect...\r\n");

	
	data.clientID.cstring = "client1";
	data.keepAliveInterval = 300;
	//data.will=1;
	data.cleansession = 1;
	data.username.cstring = "test";
	data.password.cstring = "test";     //密码（可以不用）
	
	
	
	len = MQTTSerialize_connect(buf, sizeof(buf), &data);	
	transport_sendPacketBuffer(buf, len); //发送数据包

	len = waitRespData(buf);
	#if 1
	if(len>0){
		recLen =0;
		/* wait for connack */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
		{
			unsigned char sessionPresent, connack_rc;

			if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
			{
				MC_INFO("Unable to connect, return code %d\n", connack_rc);
				goto exit;
			}
		}
		else
			goto exit;
   
		}
	#endif

	/* subscribe */                //配置接收状态  --订阅主题    “222”
	topicString.cstring = "test/topic";
	//len_send += strlen(topicString.cstring);
	len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, msgid, 1, &topicString, &req_qos);
	transport_sendPacketBuffer(buf, len);	//发送数据包	
    clrRecBuffer();
	#if 0
	topicString.cstring = "test/topic";       //发送的主题
	len = MQTTSerialize_publish(buf, sizeof(buf), 0, 0, 0, 0, topicString, (unsigned char*)payload, 4);

	transport_sendPacketBuffer(buf, len);	//发送数据包	
    clrRecBuffer();
	#endif
	
	while (1)
		{

		
		//	getGSMState();
			 
			MQTT_Sub();    				//处理接收到的数据
		//	payloadlen = strlen(payload);	
		//	len = MQTTSerialize_publish(buf, sizeof(buf), 0, 1, 1, 2, topicString, (unsigned char*)payload, payloadlen);
		//	transport_sendPacketBuffer(buf, len);	//发送数据包		
		if(loop>60){
			loop= 0;
			Heartbeat(5);                         //发送心跳包的函数;传的值以秒为单位;连续三次未收到回应重启系统
			}		
	loop++;
	Delay_ms(1000);

 	IWDG_Feed();
	}
	

	MC_INFO("Disconnecting\r\n");
	len = MQTTSerialize_disconnect(buf, sizeof(buf));
	transport_sendPacketBuffer(buf, len);
	
exit:	
	closeSocket();
}



void mqtt_client_loop(void)
{	

	switch(getGSMState())								  				         /*获取socket的状态*/
	{

		case IP_INITIAL:											        		         /*socket处于关闭状态*/
		
		
		 gsmNetRegistor();
		
		  break;
		case IP_START:											        		         /*socket处于准备状态*/
		
		
		 contextAction();
		
		  break;
		case PDP_DEACT:											        		         /*socket处于关闭状态*/
		
		 contextAction();
		
		  break;
		case IP_STATUS:											        		         /*socket 处于状态可查状态*/
			connectSocket();             							   /*socket连接服务器*/ 
			break;
		
		case IP_GPRSACT:
		case IP_CLOSE:		
			connectSocket();             							   /*socket连接服务器*/ 
			Delay_ms(500);
		  break;
													        	         /*socket处于初始化状态*/
			
		
		case CONNECT_OK: 												             /*socket处于连接建立状态*/
			
		  mqtt_config();                                           //MQTT配置
		  break;
			


	}
}

void MQTT_Sub(void)
{
	uint8_t buf[256];
	int buflen = 256;
	unsigned char dup;
	int qos;
	unsigned char retained;
	unsigned short msgid;
	int payloadlen_in;
	unsigned char* payload_in;
	int rc;
	MQTTString receivedTopic;

	if(MC20_Frame_Rec.FramFinishFlag>0){
		recLen =0;
		 if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
		
			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			
		MC_INFO("message arrived %.*s\r\n", payloadlen_in, payload_in);
		}

	 clrRecBuffer();	
	}

}


void Heartbeat(uint16_t sec)
{
uint8_t buffer0[10];
uint16_t len = 0;	
int len0;
unsigned char  buff0[100] = "Heartbeat_connect";
	sec = sec*1000;

	LED1_TRIG;
	
	len0 = MQTTSerialize_pingreq(buff0,20);
	sendSocketData(buff0,len0);	

	Delay_ms(10);		
	
	//判断心跳包响应
	#if 0
	if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
	{
		setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
	}
	Delay_ms(500);
	len0 = getSn_RX_RSR(SOCK_TCPC); 
	recv(SOCK_TCPC,buff0,len0);
	if(buff0[0] == 0xD0)
	{
		heartbeat_time = 0;
	}
	else
	{
		LED2_TRIG;
		heartbeat_time++;
	}
	#endif
	
	
	if(heartbeat_time > 3)
	{
	shanshuo= 6;
	while(shanshuo--)
	
		{
			LED1_TRIG;
			Delay_ms(250);
				
		}
	
	}
}
