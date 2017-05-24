enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     at_cmd.c
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/


#include "trace.h"
#include "dataTrans_uart.h"

#include <stdio.h>
#include <string.h>
#include "module_param_config.h"
#include "at_cmd.h"
#include "dataTrans_application.h"
#include "central.h"
#include "central_application.h"

#include "dlps_platform.h"
#include "rtl876x_io_dlps.h"

#define AT_CMD_HEADER_LENGTH 2
#define AT_CMD_AT_LENGTH 4

#define AT_CMD_RESET_LENGTH 10
#define AT_CMD_ROLE_LENGTH 9
#define AT_CMD_VERSION_LENGTH 12
#define AT_CMD_LADDR_LENGTH 10
#define AT_CMD_BAUD_LENGTH 9
#define AT_CMD_NAME_LENGTH 9
#define AT_CMD_STOP_LENGTH 9
#define AT_CMD_PARI_LENGTH 9
#define AT_CMD_PIN_LENGTH 8
#define AT_CMD_DEFAULT_LENGTH 12
#define AT_CMD_SLEEP_LENGTH 10
#define AT_CMD_PWRM_LENGTH 9

#define AT_CMD_INQ_LENGTH 8
#define AT_CMD_SINQ_LENGTH 9
#define AT_CMD_SHOW_LENGTH 9
#define AT_CMD_CONN_LENGTH 10
#define AT_CMD_DISCONN_LENGTH 12
#define AT_CMD_POWE_LENGTH 9
#define AT_CMD_FC_LENGTH 7

#define AT_CMD_CCS_LENGTH 8


const char AtCmdHeader[AT_CMD_HEADER_LENGTH] = {'A', 'T'};
const char AtCmdResponseOK[] = "OK\r\n";
const char AtCmdRESET[] = "RESET\r\n";
const char AtCmdROLE[] = "ROLE\r\n";
const char AtCmdVERSION[] = "VERSION\r\n";
const char AtCmdLADDR[] = "LADDR\r\n";
const char AtCmdBAUD[] = "BAUD\r\n";
const char AtCmdResponseMAC[] = "+LADDR=";
const char AtCmdNAME[] = "NAME";
const char AtCmdResponseNAME[] = "+NAME=";
const char AtCmdResponseINQS[] = "+INQS\r\n";
const char AtCmdResponseINQE[] = "+INQE\r\n";
const char AtCmdResponseCONNS[] = "+CONNS\r\n";
const char AtCmdResponseBAUD[] = "+BAUD=";
const char AtCmdResponseSTOP[] = "+STOP=";
const char AtCmdResponseROLE[] = "+ROLE=";
const char AtCmdResponseFC[] = "+FC=";
const char AtCmdResponsePARI[] =  "+PARI=";
const char AtCmdResponsePIN[] =  "+PIN=";
const char AtCmdResponseSLEEP[] =  "+SLEEP";
const char AtCmdResponseWAKE[] =  "+WAKE";
const char AtCmdResponsePWRM[] =  "+PWRM=";
const char AtCmdResponseDISCONN[] = "\r\n+DISCONNECTED\r\n";
const char AtCmdResponseINVALIDCONN[] = "+INVALID CONNECTION COMMAND\r\n";
const char AtCmdResponseCONNTIMEOUT[] = "+CONNECTION TIME OUT\r\n";
const char AtCmdResponseCCS[] =  "+CCS=";





void AtCmdSendResponse(const char *p_resp, uint16_t len)
{
    PTxData pTxData = NULL;

    pTxData = AppQueueOut(&g_AppCB->txUartDataQueueFree);
    if (pTxData != NULL)
    {
        memcpy(pTxData->tx_buffer, p_resp, len);
        pTxData->length = len;
        if (xQueueSend(g_AppCB->QueueHandleTxData, &pTxData, 0) == errQUEUE_FULL)
        {
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "AtCmdSendResponse:send data failed\n", 0);
#endif
            AppQueueIn(&g_AppCB->txUartDataQueueFree, pTxData);
            return;
        }
    }
    else
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "AtCmdSendResponse: queue is full\n", 0);
#endif
        return;
    }

}

void AtCmdSendResponseOK()
{
    AtCmdSendResponse(AtCmdResponseOK, strlen(AtCmdResponseOK));
}


void AtCmdSendResponseRename(char *name,  uint16_t len)
{
    const char *rename = (const char *)(name);
    AtCmdSendResponse(AtCmdResponseNAME, strlen(AtCmdResponseNAME));
    AtCmdSendResponse(rename, len);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponseName()
{
    AtCmdSendResponse(AtCmdResponseNAME, strlen(AtCmdResponseNAME));
    AtCmdSendResponse((const char *)(gConfigParam->localName.Local_name), strlen((const char *)(gConfigParam->localName.Local_name)));
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}

void AtCmdSendResponseInqStart()
{
    AtCmdSendResponseOK();
    AtCmdSendResponse(AtCmdResponseINQS, strlen(AtCmdResponseINQS));
}

void AtCmdSendResponseConnStart()
{
    AtCmdSendResponseOK();
    AtCmdSendResponse(AtCmdResponseCONNS, strlen(AtCmdResponseCONNS));
}



void AtCmdSendResponseInqEnd()
{
    AtCmdSendResponse(AtCmdResponseINQE, strlen(AtCmdResponseINQE));
}


void AtCmdSendResponseConn(uint8_t connindex)
{
    char connRsp[40];

    sprintf(connRsp, "+CONNECTING>>0x%02X%02X%02X%02X%02X%02X\r\n",
            ScanDestAddr[connindex][5],
            ScanDestAddr[connindex][4],
            ScanDestAddr[connindex][3],
            ScanDestAddr[connindex][2],
            ScanDestAddr[connindex][1],
            ScanDestAddr[connindex][0]);
    AtCmdSendResponse(connRsp, strlen(connRsp));
}



void AtCmdSendResponseConnEd(uint8_t connindex)
{
    char connRsp[40];

    sprintf(connRsp, "+CONNECTED>>0x%02X%02X%02X%02X%02X%02X\r\n",
            ScanDestAddr[connindex][5],
            ScanDestAddr[connindex][4],
            ScanDestAddr[connindex][3],
            ScanDestAddr[connindex][2],
            ScanDestAddr[connindex][1],
            ScanDestAddr[connindex][0]);
    AtCmdSendResponse(connRsp, strlen(connRsp));
}


void AtCmdSendResponseDisConnED()
{
    AtCmdSendResponse(AtCmdResponseDISCONN, strlen(AtCmdResponseDISCONN));
}





void AtCmdSendResponseBaudConfig()
{
    AtCmdSendResponse(AtCmdResponseBAUD, strlen(AtCmdResponseBAUD));
    AtCmdSendResponse((const char *)(g_AppCB->atCmdBuffer + 7), 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponseBaud()
{
    char index = (char)(moduleParam_GetBaudrateIndex() + '0');
    const char *baudindex = (const char *)(&index);
    AtCmdSendResponse(AtCmdResponseBAUD, strlen(AtCmdResponseBAUD));
    AtCmdSendResponse(baudindex, 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}


void AtCmdSendResponseStopConfig()
{
    AtCmdSendResponse(AtCmdResponseSTOP, strlen(AtCmdResponseSTOP));
    AtCmdSendResponse((const char *)(g_AppCB->atCmdBuffer + 7), 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponseStop()
{
    char index = (char)(gConfigParam-> stopbit.stopbit + '0');
    const char *stopindex = (const char *)(&index);
    AtCmdSendResponse(AtCmdResponseSTOP, strlen(AtCmdResponseSTOP));
    AtCmdSendResponse(stopindex, 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}





void AtCmdSendResponseRoleConfig()
{
    AtCmdSendResponse(AtCmdResponseROLE, strlen(AtCmdResponseROLE));
    AtCmdSendResponse((const char *)(g_AppCB->atCmdBuffer + 7), 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponseRole()
{
    char index = (char)(gConfigParam-> gaprole.gaprole + '0');
    const char *gaproleindex = (const char *)(&index);
    AtCmdSendResponse(AtCmdResponseROLE, strlen(AtCmdResponseROLE));
    AtCmdSendResponse(gaproleindex, 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}




void AtCmdSendResponseFcConfig()
{
    AtCmdSendResponse(AtCmdResponseFC, strlen(AtCmdResponseFC));
    AtCmdSendResponse((const char *)(g_AppCB->atCmdBuffer + 5), 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponseFc()
{
    char index = (char)(gConfigParam-> fc_enable.fc_enable + '0');
    const char *fcindex = (const char *)(&index);
    AtCmdSendResponse(AtCmdResponseFC, strlen(AtCmdResponseFC));
    AtCmdSendResponse(fcindex, 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}





void AtCmdSendResponsePariConfig()
{
    AtCmdSendResponse(AtCmdResponsePARI, strlen(AtCmdResponsePARI));
    AtCmdSendResponse((const char *)(g_AppCB->atCmdBuffer + 7), 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponsePari()
{
    char index = (char)(gConfigParam-> paribit.paribit + '0');
    const char *pariindex = (const char *)(&index);
    AtCmdSendResponse(AtCmdResponsePARI, strlen(AtCmdResponsePARI));
    AtCmdSendResponse(pariindex, 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}



void AtCmdSendResponsePinConfig(char *pin)
{
    const char *newpin = (const char *)(pin);
    AtCmdSendResponse(AtCmdResponsePIN, strlen(AtCmdResponsePIN));
    AtCmdSendResponse(newpin, 6);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}


void AtCmdSendResponseSleep()
{
    AtCmdSendResponse(AtCmdResponseSLEEP, strlen(AtCmdResponseSLEEP));
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}


void AtCmdSendResponseWake()
{
    AtCmdSendResponse(AtCmdResponseWAKE, strlen(AtCmdResponseWAKE));
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}



void AtCmdSendResponsePwrmConfig()
{
    AtCmdSendResponse(AtCmdResponsePWRM, strlen(AtCmdResponsePWRM));
    AtCmdSendResponse((const char *)(g_AppCB->atCmdBuffer + 7), 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}

void AtCmdSendResponsePwrm()
{
    char index = (char)(gConfigParam->is_enter_sleep.is_enter_sleep + '0');
    const char *is_enter_sleep_index = (const char *)(&index);
    AtCmdSendResponse(AtCmdResponsePWRM, strlen(AtCmdResponsePWRM));
    AtCmdSendResponse(is_enter_sleep_index, 1);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}


void Itoa(uint32_t num, char *str)
{
    uint32_t i = 0, j, k;
    do {
        str[i++] = (num % 10) + '0';
        num = num / 10;
    } while (num);

    str[i] = '\0';
    k = 0;
    char temp;
    for ( j = k; j <= (i - 1) / 2; j++)
    {
        temp = str[j];
        str[j] = str[i - 1 + k - j];
        str[i - 1 + k - j] = temp;
    }
}



void AtCmdSendResponsePin()
{
    char pin_buffer[10] = {0};
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "gConfigParam->pin.pin= %d", 1, gConfigParam->pin.pin);
#endif
    Itoa(gConfigParam->pin.pin,  pin_buffer);
    AtCmdSendResponse(AtCmdResponsePIN, strlen(AtCmdResponsePIN));
    AtCmdSendResponse((const char *)( pin_buffer ) , strlen(pin_buffer));
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
}



void AtCmdSendResponseCcsConfig(char *chan_map)
{
    const char *chanMap = (const char *)(chan_map);
    AtCmdSendResponse(AtCmdResponseCCS, strlen(AtCmdResponseCCS));
    AtCmdSendResponse(chanMap, 10);
    AtCmdSendResponse((const char *)("\r\n"), strlen("\r\n"));
    AtCmdSendResponseOK();
}





static void AtCmdHandleReset(void)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_Reset", 0);
#endif
    xTimerStart(TimersReset, 0);
    AtCmdSendResponseOK();
}



static void AtCmdHandleBaud(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_BAUD_LENGTH )               /*  AT CMD: AT+BAUD */
    {
        AtCmdSendResponseBaud();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_BAUD_LENGTH + 1 )      /*  AT CMD: AT+BAUD<Param>  */
    {
        int baudindex = g_AppCB->atCmdBuffer[7] - '0' ;

        if ( moduleParam_SetBaudrate(baudindex) )
        {
            AtCmdSendResponseBaudConfig();
        }
    }
}


static void AtCmdHandleName(void)
{
    uint16_t len;

    if ( g_AppCB->atCmdLength == AT_CMD_NAME_LENGTH )   /*  AT CMD: AT+NAME */
    {
        AtCmdSendResponseName();
    }
    else                                                    /*  AT CMD: AT+NAME<Param>  */
    {
        len =  g_AppCB->atCmdLength - AT_CMD_NAME_LENGTH;
        if ( moduleParam_SetModuleName(g_AppCB->atCmdBuffer + 7, len) )
        {
            g_AppCB->atCmdBuffer[g_AppCB->atCmdLength - 2] = '\0';
            AtCmdSendResponseRename(g_AppCB->atCmdBuffer + 7, len);
        }
    }
}




static void AtCmdHandleInq(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_INQ_LENGTH) /*  AT CMD: AT+INQ*/
    {
        memset( ScanDestAddr, 0, sizeof(ScanDestAddr));
        ScanAddrNum = 0;
        AtCmdSendResponseInqStart();
        central_StartScan();
    }
}



static void AtCmdHandleSInq(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_SINQ_LENGTH) /*  AT CMD: AT+SINQ */
    {
        central_StopScan();
        AtCmdSendResponseInqEnd();
    }
}


uint8_t  connindex;
bool isConnecting = FALSE;
static void AtCmdHandleConn(void)
{
    if (isConnecting)
    {
        AtCmdSendResponse(AtCmdResponseINVALIDCONN, strlen(AtCmdResponseINVALIDCONN));
    }
    else
    {
        AtCmdSendResponseConnStart();
        connindex = g_AppCB->atCmdBuffer[7] - '0' ;
        centralSetGapParameter(GAPPRRA_CONN_DEST_ADDR, B_ADDR_LEN, ScanDestAddr[connindex]);

        uint8_t DestAddrType = blueAPI_RemoteBDTypeLEPublic;
        uint16_t Conn_ScanTimeout = 1000;
        uint16_t conIntervalMin = 0x10;
        uint16_t conIntervalMax = 0x10;
        uint16_t slaveLantency = 0;
        uint16_t supervisionTimeout = 1000;
        uint16_t CE_Length = 2 * conIntervalMax - 2;

        centralSetGapParameter(GAPPRRA_CONN_DEST_ADDR_TYPE, sizeof(DestAddrType), &DestAddrType);
        centralSetGapParameter(GAPPRRA_CONN_SCAN_TIMEOUT, sizeof(Conn_ScanTimeout), &Conn_ScanTimeout);
        centralSetGapParameter(GAPPRRA_MIN_CONN_INTERVAL, sizeof(conIntervalMin), &conIntervalMin);
        centralSetGapParameter(GAPPRRA_MAX_CONN_INTERVAL, sizeof(conIntervalMax), &conIntervalMax);
        centralSetGapParameter(GAPPRRA_SLAVE_LATENCY, sizeof(slaveLantency), &slaveLantency);
        centralSetGapParameter(GAPPRRA_TIMEOUT_MULTIPLIER, sizeof(supervisionTimeout), &supervisionTimeout);
        centralSetGapParameter(GAPPRRA_CE_LENGTH, sizeof(CE_Length), &CE_Length);

        central_Connect();
        isConnecting = TRUE;
        xTimerReset(TimersConnTimeOut, 0);
    }
}




static void AtCmdHandleStop(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_STOP_LENGTH )               /*  AT CMD: AT+STOP */
    {
        AtCmdSendResponseStop();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_STOP_LENGTH + 1 )      /*  AT CMD: AT+STOP<Param>  */
    {
        uint16_t stopindex = g_AppCB->atCmdBuffer[7] - '0' ;

        if ( moduleParam_SetStopBit(stopindex) )
        {
            AtCmdSendResponseStopConfig();
        }

    }
}


static void AtCmdHandleRole(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_ROLE_LENGTH )               /*  AT CMD: AT+ROLE */
    {
        AtCmdSendResponseRole();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_ROLE_LENGTH + 1 )      /*  AT CMD: AT+ROLE<Param>  */
    {
        uint16_t gaproleindex = g_AppCB->atCmdBuffer[7] - '0' ;

        if ( moduleParam_SetGapRole(gaproleindex) )
        {
            AtCmdSendResponseRoleConfig();
            xTimerStart(TimersReset, 0);
        }

    }
}




static void AtCmdHandleSleep(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_SLEEP_LENGTH )  /*  AT CMD: AT+SLEEP    */
    {
        AtCmdSendResponseSleep();
        xTimerStart(TimersEnterLowPower, 0);
        gConfigParam->isSleeping = TRUE;
    }
}



static void AtCmdHandlePari(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_PARI_LENGTH )   /*  AT CMD: AT+PARI */
    {
        AtCmdSendResponsePari();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_PARI_LENGTH + 1 )      /*  AT CMD: AT+PARI<Param>  */
    {
        uint16_t pariindex = g_AppCB->atCmdBuffer[7] - '0' ;

        if ( moduleParam_SetPariBit(pariindex) )
        {
            AtCmdSendResponsePariConfig();
        }
    }
}


static void AtCmdHandlePin(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_PIN_LENGTH ) /*  AT CMD: AT+PIN  */
    {
        AtCmdSendResponsePin();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_PIN_LENGTH + 6 )       /*  AT CMD: AT+PIN<Param>   */
    {
        if ( moduleParam_SetModulePin(g_AppCB->atCmdBuffer + 6) )
        {
            AtCmdSendResponsePinConfig(g_AppCB->atCmdBuffer + 6);
        }
    }
}



static void AtCmdHandlePwrm(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_PWRM_LENGTH )   /*  AT CMD: AT+PWRM */
    {
        AtCmdSendResponsePwrm();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_PWRM_LENGTH + 1 )      /*  AT CMD: AT+PWRM<Param>  */
    {
        uint32_t is_enters_sleep_index = g_AppCB->atCmdBuffer[7] - '0' ;

        if ( moduleParam_SetIsEnterSleep(is_enters_sleep_index) )
        {
            AtCmdSendResponsePwrmConfig();
        }
    }
}




static void AtCmdHandleDefault(void)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_Default", 0);
#endif
    xTimerStart(TimersDefault, 0);
    AtCmdSendResponseOK();
}


static void AtCmdHandleMAC(void)
{
    PTxData pTxData = NULL;
    uint16_t pos = 0;
    uint8_t     LocalBd[BLUE_API_BD_SIZE];
    char bdStr[20];
    peripheralGetGapParameter(GAPPARA_BD_ADDR, &LocalBd);
    sprintf(bdStr, "%02X%02X%02X%02X%02X%02X", LocalBd[5], LocalBd[4], LocalBd[3], LocalBd[2], LocalBd[1], LocalBd[0]);
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "AtCmdHandleMAC", 0);
#endif

    pTxData = AppQueueOut(&g_AppCB->txUartDataQueueFree);
    if (pTxData != NULL)
    {
        memcpy(pTxData->tx_buffer, AtCmdResponseMAC, strlen(AtCmdResponseMAC));
        pos += strlen(AtCmdResponseMAC);
        memcpy(pTxData->tx_buffer + pos, bdStr, strlen(bdStr));
        pos += strlen(bdStr);
        pTxData->tx_buffer[pos] = '\r';
        pos++;
        pTxData->tx_buffer[pos] = '\n';
        pos++;
        pTxData->length = pos;
        if (xQueueSend(g_AppCB->QueueHandleTxData, &pTxData, 0) == errQUEUE_FULL)
        {
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "AtCmdHandleMAC:send data failed\n", 0);
#endif
            AppQueueIn(&g_AppCB->txUartDataQueueFree, pTxData);
            return;
        }
    }
    else
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "AtCmdHandleMAC: queue is full\n", 0);
#endif
        return;
    }
}







static void AtCmdHandleFc(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_FC_LENGTH )             /*  AT CMD: AT+ROLE */
    {
        AtCmdSendResponseFc();
    }
    else if ( g_AppCB->atCmdLength == AT_CMD_FC_LENGTH + 1 )    /*  AT CMD: AT+ROLE<Param>  */
    {
        uint16_t fcindex = g_AppCB->atCmdBuffer[5] - '0' ;

        if ( moduleParam_SetFc(fcindex) )
        {
            AtCmdSendResponseFcConfig();
        }

    }
}


static void AtCmdHandleCcs(void)
{
    if ( g_AppCB->atCmdLength == AT_CMD_CCS_LENGTH + 10 )       /*  AT CMD: AT+CCS<Param>   */
    {
        if ( moduleParam_SetChanClass(g_AppCB->atCmdBuffer + 6) )
        {
            AtCmdSendResponseCcsConfig(g_AppCB->atCmdBuffer + 6);
        }
    }
}








uint32_t Atoi(const char *str)
{
    uint32_t res = 0, begin = 0;

    while (*str != '\0')
    {
        if (begin == 0 && (('0' <= *str && *str <= '9') || *str == '-') )
        {
            begin = 1;
        }
        else if ( begin == 1 && (*str < '0' || *str > '9') )
            break;
        if (begin == 1)
            res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}




bool IsAtCmd(char *at_cmd, uint16_t pos)
{
    bool res = TRUE;

    while (*at_cmd != '\0')
    {
        if ((g_AppCB->atCmdBuffer[pos] != *at_cmd) && (g_AppCB->atCmdBuffer[pos] != (*at_cmd) + 32))
        {
            res = FALSE;
            break;
        }
        pos++;
        at_cmd++;
    }

    return res;
}





void AtCmdParse(void)
{
    uint16_t pos = 0;

    if (g_AppCB->atCmdLength < AT_CMD_HEADER_LENGTH)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "AtCmdParse cmd too short %d", 1, g_AppCB->atCmdLength);
#endif
        return;
    }


    if ( ((g_AppCB->atCmdBuffer[pos] != 'A') && (g_AppCB->atCmdBuffer[pos] != 'a')) || ((g_AppCB->atCmdBuffer[pos + 1] != 'T') && (g_AppCB->atCmdBuffer[pos + 1] != 't')))
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "AtCmdParse: not at cmd", 0);
#endif
        return;
    }

    if ( (g_AppCB->atCmdBuffer[g_AppCB->atCmdLength - 2] != '\r') || (g_AppCB->atCmdBuffer[g_AppCB->atCmdLength - 1] != '\n') )
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "AtCmdParse: not at cmd", 0);
#endif
        return;
    }


    pos += AT_CMD_HEADER_LENGTH;


    if ((g_AppCB->atCmdBuffer[pos] == '\r') && (g_AppCB->atCmdBuffer[pos + 1] == '\n') && (g_AppCB->atCmdLength == AT_CMD_AT_LENGTH))
    {
        AtCmdSendResponseOK();  /*  AT cmd: AT  */
    }
    else if ( g_AppCB->atCmdBuffer[pos] == '+' )
    {
        pos++;
        if (IsAtCmd("RESET", pos) && ( g_AppCB->atCmdLength == AT_CMD_RESET_LENGTH ))
        {
            AtCmdHandleReset();
        }
        else if (IsAtCmd("ROLE", pos))
        {
            AtCmdHandleRole();
        }
        else if (IsAtCmd("LADDR", pos) && (g_AppCB->atCmdLength == AT_CMD_LADDR_LENGTH ))
        {
            AtCmdHandleMAC();
        }
        else if (IsAtCmd("BAUD", pos))
        {
            AtCmdHandleBaud();
        }
        else if (IsAtCmd("NAME", pos))
        {
            AtCmdHandleName();
        }
        else if (IsAtCmd("CONN", pos) && (g_AppCB->atCmdLength == AT_CMD_CONN_LENGTH ))
        {
            AtCmdHandleConn();
        }
        else if (IsAtCmd("INQ", pos) && (g_AppCB->atCmdLength == AT_CMD_INQ_LENGTH ))
        {
            AtCmdHandleInq();
        }
        else if (IsAtCmd("SINQ", pos) && (g_AppCB->atCmdLength == AT_CMD_SINQ_LENGTH ))
        {
            AtCmdHandleSInq();
        }
        else if (IsAtCmd("STOP", pos))
        {
            AtCmdHandleStop();
        }
        else if (IsAtCmd("SLEEP", pos) && (g_AppCB->atCmdLength == AT_CMD_SLEEP_LENGTH ))
        {
            AtCmdHandleSleep();
        }
        else if (IsAtCmd("PARI", pos))
        {
            AtCmdHandlePari();
        }
        else if (IsAtCmd("PIN", pos))
        {
            AtCmdHandlePin();
        }
        else if (IsAtCmd("PWRM", pos))
        {
            AtCmdHandlePwrm();
        }
        else if (IsAtCmd("DEFAULT", pos) && (g_AppCB->atCmdLength == AT_CMD_DEFAULT_LENGTH ))
        {
            AtCmdHandleDefault();
        }
        else if (IsAtCmd("FC", pos))
        {
            AtCmdHandleFc();
        }
        else if (IsAtCmd("CCS", pos))
        {
            AtCmdHandleCcs();
        }
    }
}

