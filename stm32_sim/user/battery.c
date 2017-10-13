/***************************************************************************************************
	* Copyright (c) 2017, ETEK
	* All rights reserved.
	* ???????? battery.c
	* ? ???   
	* ????汾?? V1.0, sommer jiang, 2017-06-15
	* ???????? ??
***************************************************************************************************/
#include "quectel_mc20.h"
#include "battery.h"
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

/***************************************************************************************************
//#CONST
***************************************************************************************************/

//const char REPORT_SERVER[] = "http:\57\57api.efxnow.com\57DEMOWebServices2.8\57Service.asmx\57Echo";
const char REPORT_SERVERS[] = "http://222.191.229.234\57appuser\57report.do";
/***************************************************************************************************
//#RAM
***************************************************************************************************/





/***************************************************************************************************
//#PROGRAM
***************************************************************************************************/
int addMsgContent(char *str, int len, char *key,char *content) {
	len += sprintf(str + len, "%s=%s", key,content);
	return len;
}

int addSplite(char *str, int len) {
	len += sprintf(str + len, "&");
	return len;
}

int getBatteryMsgData(char *str,BATTERY_REPORT battery){
	int len =0;
	char buff[16];
	battery.type = battery.gnrmc_info.position_valid;
	len = addMsgContent(str, len,"name",battery.name);
	len = addSplite(str, len);
	sprintf(buff, "%f", battery.level);
	len = addMsgContent(str, len,"level",buff);
	len = addSplite(str, len);
	sprintf(buff, "%c,%f", battery.gnrmc_info.latitude_suffix,battery.gnrmc_info.latitude);
	len = addMsgContent(str, len,"latitude",buff);
	len = addSplite(str, len);
	sprintf(buff, "%c,%f", battery.gnrmc_info.longitude_suffix,battery.gnrmc_info.longitude);
	len = addMsgContent(str, len,"longitude",buff);
	len = addSplite(str, len);
	sprintf(buff, "%d", battery.type);
	len = addMsgContent(str, len,"type",buff);
	len = addSplite(str, len);
	len = addMsgContent(str, len,"time",battery.time.orig);
	
	return len;
}



