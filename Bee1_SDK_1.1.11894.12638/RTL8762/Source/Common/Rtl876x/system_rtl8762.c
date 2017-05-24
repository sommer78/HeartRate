/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     system_rtl8762.c
* @brief    Routines to initialize system.
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x.h"
#include "section_config.h"
#include "symboltable_uuid.h"
#include "flash_ota.h"
#include "version.h"
#include "trace.h"
#include "legacyperiphapi.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "hal_wdg.h"
#include "dlps_platform.h"
#include "cycle_queue.h"            /* Used by LOG_BUFFER_APP() */
#include <stdarg.h>                 /* Used by LOG_BUFFER_APP() */
#include "rtl876x_pinmux.h"
#include "board.h"

#define OTA_FLAG_DEFAULT 0x7f
#define MAX_LOG_MESSAGE_LEN  128    /* Used by LOG_BUFFER_APP() */
#define MAX_ARGUMENT_SIZE 20        /* Used by LOG_BUFFER_APP() */

// the header of user image
const PATCH_HEADER user_flash_header USER_FLASH_HEADER = {
    sizeof(PATCH_HEADER)/4, 
    Signature_USER, 
    VERSION_BUILD, 
    0, 
    0, 
    OTA_FLAG_DEFAULT,
#if (defined USER_DATA_EXIST) && (USER_DATA_EXIST)
    USER_DATA_PAGE_INDEX
#else
    0xff
#endif
};
const TIMESTAMP_VER_HEADER user_flash_timestamp USER_FLASH_TIMESTAMP = {
    DEFINE_symboltable_uuid,
    VERSION_MAJOR,
    VERSION_MINOR,
    VERSION_REVISION
};

//avoid empty trace file
const int trace_version TRACE_DATA = VERSION_REVISION;

#if defined (RTL8762AX_VB)
extern IRQ_FUN UserIrqFunTable[32];
extern uint32_t __Vectors;
#endif
extern UINT8 gSequenceNumber;       /* Used by LOG_BUFFER_APP() */

volatile uint32_t gKeepActiveCounter = 0;
volatile BOOL allowedSystemEnterDlps = TRUE;

void RecordAddEmptyFunc(void)
{
    return;
}

void WatchDogConfigAndEnable(void)
{
    //HalWatchDogConfig(8,15,1);  //~1s
    HalWatchDogConfig(16,15,1);  //~2s
    //HalWatchDogConfig(25,15,1);  //~3s
    //HalWatchDogConfig(34,15,1);  //~4s
    //HalWatchDogConfig(42,15,1);  //~5s
    //HalWatchDogConfig(51,15,1);  //~6s
    //HalWatchDogConfig(59,15,1);  //~7s
    //HalWatchDogConfig(68,15,1);  //~8s
    //HalWatchDogConfig(137,15,1);  //~16s
    HalWatchDogEnable();
}

typedef enum
{
    INDEX_LOG_UART  = 0,
    INDEX_DATA_UART = 1
}UartIndex;

BOOL DLPS_SystemCheck()
{
    //If the function (Print all log before entering DLPS) is enable
    if(*(uint8_t *)0x20000132 & BIT5)
    {
        extern uint32_t (*UART_GetLineStatus)(UartIndex index);
        if((UART_GetLineStatus(INDEX_LOG_UART) & 0x40) != 0x40)
        {
            //Don't allow to enter dlps if log uart tx fifo is not empty
            return false;
        }
    }
    return allowedSystemEnterDlps;
}

#if defined (RTL8762AX_VB)
void System_Init(void)
{
    //SDK version
    DBG_DIRECT("RTL8762AX_VB SDK Version: %s", FILE_VERSION_STR);
    DBG_DIRECT("Build Time:%s", BUILDING_TIME);

#ifndef RUN_APP_IN_HCIMODE
    if( ((*(uint32_t*)0x200000F0) & BIT(0)) == 1) //upper_stack_en
    {
        DBG_DIRECT("In SoC Mode");       
    }
    else
    {
        DBG_DIRECT("WARNING: In HCI Mode, will not run APP Task");
        vTaskStartScheduler();
    }      
#endif

    uint32_t i;
    for(i = 0; i < 32; ++i)
    {
        if( i == TIMER0_IRQ ||
            i == TIMER1_IRQ ||
            i == BTMAC_BLUEWIZ_IRQ ||
            i == BTMAC_HCIDMA_IRQ ||
            i == BTMAC_BZDMA_IRQ ||
            i == BTMAC_CROSSBAR_IRQ ||
            i == LOG_UART_IRQ
        )
            ; //Should not be remapped
        else
            UserIrqFunTable[i]  = (IRQ_FUN)*(&__Vectors+(16+i));
    }
    
    //NVIC_SetPriority(SYSTEM_ON_IRQ, 0);
    //NVIC_EnableIRQ(SYSTEM_ON_IRQ);  //Enable SYSTEM_ON Interrupt

    /* Please use SWDTool or MPTool to configure BT address. */
#if 0
    //Set BT address
    uint8_t addr[6] = {0x11, 0x11, 0x22, 0x22, 0x33, 0x33};
    memcpy((uint8_t *)0x20000076, addr, 6);
#endif
    
    /* Accelerate the interrupt response time */
    typedef VOID (*VoidPatchFun) ();
    extern VoidPatchFun pPatch_RECORD_ADD;
    pPatch_RECORD_ADD = RecordAddEmptyFunc;

    /* Config in Debug Version:
       BIT1: Check Stack Overflow 
       BIT5: Print all log in log buffer before entering DLPS 
    */
#ifndef RELEASE_VERSION
   uint8_t *pOSCconfig =  (uint8_t *)0x20000132;
   *pOSCconfig = (*pOSCconfig) | BIT1 | BIT5;
#endif
    
    /* Enable WDG in Release Version */
#ifdef RELEASE_VERSION
    extern void WatchDogEnable_encapsed();
    DLPS_INTERRUPT_CONTROL_CB_UNREG(WatchDogEnable_encapsed);
    DLPS_INTERRUPT_CONTROL_CB_REG(WatchDogConfigAndEnable, DLPS_EXIT_ALL);
    WatchDogConfigAndEnable(); 
#endif
    
    /* Enable CPU Dynamic Sleep in Release Version */
#ifdef RELEASE_VERSION
    //*(uint8_t*)(0x2000006A) |= BIT0;
#endif

    if (FALSE == DLPS_ENTER_CHECK_CB_REG(DLPS_SystemCheck))
    {
        DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "Error: DLPS_ENTER_CHECK_CB_REG(DLPS_SystemCheck) failed!\n", 0);
    }

    return;
}

void PeripheralIntrHandler(void)
{
    uint32_t PeriIrqStatus, ExactIrqStatus;
    IRQ_FUN pFun = NULL;
    
    PeriIrqStatus = PERIPHINT->STATUS;
    //Save exact IRQ status
    ExactIrqStatus = PeriIrqStatus & (PERIPHINT->EN);
    
    //Search from SPIFLASH_IRQ to TIMER7_IRQ
    if(ExactIrqStatus & BIT0)  //SPIFLASH_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(0 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT1)  //GPIO2_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(1 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT2)  //GPIO3_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(2 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT3)  //GPIO4_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(3 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT4)  //GPIO5_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(4 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT5)  //TIMER3_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(5 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT6)  //TIMER4_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(6 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT7)  //TIMER5_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(7 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT8)  //TIMER6_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(8 + 48));
        pFun();
    }
    if(ExactIrqStatus & BIT9)  //TIMER7_IRQ
    {
        pFun = (IRQ_FUN)*(&__Vectors+(9 + 48));
        pFun();
    }
    
    //GPIO6To31_IRQ
    if(ExactIrqStatus & BIT(GPIO6To31_IRQ - 64))    
    {
        uint32_t gpio_int = GPIO->INTSTATUS;
        if(gpio_int & BIT6)  //GPIO6_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 6));
            pFun();
        }
        if(gpio_int & BIT7)  //GPIO7_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 7));
            pFun();
        }
        if(gpio_int & BIT8)  //GPIO8_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 8));
            pFun();
        }
        if(gpio_int & BIT9)  //GPIO9_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 9));
            pFun();
        }
        if(gpio_int & BIT10)  //GPIO10_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 10));
            pFun();
        }
        if(gpio_int & BIT11)  //GPIO11_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 11));
            pFun();
        }
        if(gpio_int & BIT12)  //GPIO12_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 12));
            pFun();
        }
        if(gpio_int & BIT13)  //GPIO13_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 13));
            pFun();
        }
        if(gpio_int & BIT14)  //GPIO14_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 14));
            pFun();
        }
        if(gpio_int & BIT15)  //GPIO15_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 15));
            pFun();
        }
        if(gpio_int & BIT16)  //GPIO16_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 16));
            pFun();
        }
        if(gpio_int & BIT17)  //GPIO17_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 17));
            pFun();
        }
        if(gpio_int & BIT18)  //GPIO18_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 18));
            pFun();
        }
        if(gpio_int & BIT19)  //GPIO19_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 19));
            pFun();
        }
        if(gpio_int & BIT20)  //GPIO20_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 20));
            pFun();
        }
        if(gpio_int & BIT21)  //GPIO21_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 21));
            pFun();
        }
        if(gpio_int & BIT22)  //GPIO22_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 22));
            pFun();
        }
        if(gpio_int & BIT23)  //GPIO23_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 23));
            pFun();
        }
        if(gpio_int & BIT24)  //GPIO24_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 24));
            pFun();
        }
        if(gpio_int & BIT25)  //GPIO25_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 25));
            pFun();
        }
        if(gpio_int & BIT26)  //GPIO26_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 26));
            pFun();
        }
        if(gpio_int & BIT27)  //GPIO27_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 27));
            pFun();
        }
        if(gpio_int & BIT28)  //GPIO28_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 28));
            pFun();
        }
        if(gpio_int & BIT29)  //GPIO29_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 29));
            pFun();
        }
        if(gpio_int & BIT30)  //GPIO30_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 30));
            pFun();
        }
        if(gpio_int & BIT31)  //GPIO31_IRQ
        {
            pFun = (IRQ_FUN)*(&__Vectors+(52 + 31));
            pFun();
        }
    }
}
#endif

static void vTimerDLPSKeepActiveCallback( TimerHandle_t pxTimer )
{
    portENTER_CRITICAL();
    
    gKeepActiveCounter --;
    if(0 == gKeepActiveCounter)  //Allow to enter DLPS
    {
        allowedSystemEnterDlps = TRUE;
    }
    
    portEXIT_CRITICAL();
	xTimerDelete(pxTimer,0);
	
}

void DLPS_KeepActiveTicks(const TickType_t xTimerPeriodInTicks)
{
    /* xTimersDLPSKeepActive is a local variable, and the timer should not be reset/period-change/stop */
    TimerHandle_t xTimersDLPSKeepActive = NULL;
    
    /* Different timer will share the same timer CB */
    xTimersDLPSKeepActive = xTimerCreate(  "xTimersDLPSKeepActive",     // Just a text name, not used by the kernel.
                                       xTimerPeriodInTicks,                       // The timer period in ticks.
                                       pdFALSE,                 // The timers will auto-reload themselves when they expire.
                                       ( void *) 0,             // Assign each timer a unique id equal to its array index.
                                       vTimerDLPSKeepActiveCallback
                                       );
    if(xTimersDLPSKeepActive)
    {
        if(xTimerStart(xTimersDLPSKeepActive, 0) == pdPASS)
        {
            allowedSystemEnterDlps = FALSE;
        
            portENTER_CRITICAL();
            gKeepActiveCounter ++;
            portEXIT_CRITICAL();
        }
    }
}


void SysOnIntrHandler(void)
{
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "SysOnIntr", 0);

    //Disable GPIO wakeup Interrupt, open it in DLPS_IO_EnterDlpsCb().
    System_WakeUp_Intr_Disable();
    
    //Keep Active for a while before enter DLPS
    DLPS_KeepActiveTicks(3);  //30ms
}

void WDG_SystemReset(void)
{
    /* restore reg_aon_powon_opt setting before WDG reset */
    bee_aon_gp_write(0x18, 0xa8);
    bee_aon_gp_write(0x19, 0xf0);
    bee_aon_gp_write(0x1a, 0xb4);
    bee_aon_gp_write(0x1b, 0xdd);
    bee_aon_gp_write(0x14, 0xdd); /* hw bug: write AON(0x1C)[7:0] */
    bee_aon_gp_write(0x1d, 0x1c);
    bee_aon_gp_write(0x1e, 0xc3);
    bee_aon_gp_write(0x1f, 0xf0);

    portDISABLE_INTERRUPTS();
    HalWatchDogConfig(1, 0, 1);
    HalWatchDogEnable();
    while(1); /* wait until reset */
}

UCHAR LOG_BUFFER_APP(UINT32 log_str_index, UINT8 para_num, ...)
{
    UINT8 log_length, i;
    va_list argp;
    UCHAR l_msg[MAX_LOG_MESSAGE_LEN];
    UINT32 TimeStamp = xTaskGetTickCount() * 10;

    /*
    Byte: Description
    0: 		sync(0x7E)
    1: 		length
    2: 		seqno
    3: 		CheckSum
    4-5: 	TimeStamp
    6: 		Type = 15
    7:
            bit 0: index compressed or not
            bit 1: line Num exist
            bit 2: file Num exist
            bit 3-4: args
            bit 5-7: color
    8-9: 	Message Index
    10: 	file Num
    11-12: 	line Num
    13: 	Num of args: N
    14(Totol: 4*N): args
    */
    l_msg[0] = 0x7E;				        //syntax

    l_msg[4] = (UCHAR)(TimeStamp & 0xff);	//TimeStamp
    l_msg[5] = (UCHAR)((TimeStamp>>8) & 0xff);

    l_msg[6] = MODULE_APP + MODULE_START_INDEX; 
    l_msg[7] = ((0&0x7)<<5)                 //color
               | 0x04                       //1=has file number
               | 0x02;                      //1=has line number

    l_msg[7] |= 0x01;                       //index is compressed
    l_msg[8] = (UCHAR)((log_str_index>>2) & 0xff);
    l_msg[9] = (UCHAR)((log_str_index>>10) & 0xff);

    l_msg[10] = (UCHAR)(26&0xff);		    //8 bits file_num
    l_msg[11] = (UCHAR)(0&0xff);		    //8 bits line_num
    l_msg[12] = (UCHAR)((0&0xff00)>>8);	    //8 bits line_num

    if(para_num>0)
    {
        int temp;

        if(para_num > MAX_ARGUMENT_SIZE)    //Avoid memory overflow
            para_num = MAX_ARGUMENT_SIZE;

        l_msg[7] |= 0x08;					//has str para
        l_msg[13] = (UCHAR)para_num;
        log_length = 14;

        va_start(argp, para_num);
        for(i=0; i<para_num; i++)			//para_num*32bits data
        {
            temp = va_arg(argp, int);
            l_msg[log_length] = (UCHAR)(temp&0xff);
            l_msg[log_length+1] = (UCHAR)((temp&0xff00)>>8);
            l_msg[log_length+2] = (UCHAR)((temp&0xff0000)>>16);
            l_msg[log_length+3] = (UCHAR)((temp&0xff000000)>>24);
            log_length+=4;
        }
        va_end(argp);
    }
    else
    {
        // no parameter
        log_length = 13;
    }

    l_msg[1] = log_length;				    //length

    taskENTER_CRITICAL();
    gSequenceNumber++;
    l_msg[2] = gSequenceNumber;  			//seqno
    l_msg[3] = l_msg[0]^l_msg[1]^l_msg[2];	//CheckSum
    CycQueueWrite(l_msg, log_length);
    taskEXIT_CRITICAL();

    return 0;
}
