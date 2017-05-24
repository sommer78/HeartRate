#include "ir_protocol.h"
#include "trace.h"

/*****************************************************************************************************************************
 * NEC : pulse distance encoding
 * Carrier frequency = 38kHz
 * duty factor = 1/3
 * period = 26.5us
 * first pulse : 9ms 4.5ms
 *      H     L
 * 0 : 560us 560us
 * 1 : 560us 1690us
 *
 * Address (8 bits) is sent first, then ~Address
 * Command (8 bits) follows, then ~Command
 * LSB is sent first !
 *
 * NEC extended allows 16 bits addresses. In this case, the 8 bits following
 * Address are not ~Address, but the address extension.
 */

#define ABS_F(a,b) (a>b) ? (a-b):(b-a) ; 

static uint16_t HL_Time2TxBufCount(uint32_t a, uint32_t b) 
{ 
    return ((a & 0x8000)| ((a &0x7FFF) / b)); 
}

static uint16_t TxBufCount2Time(uint32_t a, uint32_t b) 
{ 
    return  ((a & 0x7FFF) * b); 
}
static uint16_t HL_WAVE(uint32_t a) 
{ 
    return  ((a & 0x8000) >> 15); 
}
static uint8_t BIT_N (uint32_t a, uint32_t b)
{
    return ((a >> b) & 0x01); 
} 

const IRDA_SPEC NEC_SPEC = 
{
         2,                                                         //  HeaderLen;
        {PULSE_HIGH |9500    , PULSE_LOW |4500  },                  //uint16_t HeaderContext[MAX_HEADDER_LEN];
        {PULSE_HIGH |560     , PULSE_LOW|560    },                  //Log0[MAX_LOG_WAVFORM_SIZE]; 
        {PULSE_HIGH |560     , PULSE_LOW|1690   },                  //Log1[MAX_LOG_WAVFORM_SIZE]; 
         PULSE_HIGH |560,
         560,
         4
};

/* NEC protocol */
const IR_KEY_DATA_DEF IR_Key_Definitions[] =
{
    /* NEC protocol */
    {MODE_NEC, IR_KEY_POWER,   0x20, 0xFA},
    {MODE_NEC, IR_KEY_MUTE,    0x20, 0xDA},
    {MODE_NEC, IR_KEY_0,       0x20, 0xCA},
    {MODE_NEC, IR_KEY_1,       0x20, 0x0A},
    {MODE_NEC, IR_KEY_2,       0x20, 0x0A},
    {MODE_NEC, IR_KEY_3,       0x20, 0x48},
    {MODE_NEC, IR_KEY_4,       0x20, 0xF2},
    {MODE_NEC, IR_KEY_5,       0x20, 0x32},
    {MODE_NEC, IR_KEY_6,       0x20, 0x70},
    {MODE_NEC, IR_KEY_7,       0x20, 0xD2},
    {MODE_NEC, IR_KEY_8,       0x20, 0x12},
    {MODE_NEC, IR_KEY_9,       0x20, 0x50},
    {MODE_NEC, IR_KEY_CH_INC,  0x20, 0x02},
    {MODE_NEC, IR_KEY_CH_DEC,  0x20, 0xBA},
    {MODE_NEC, IR_KEY_VOL_INC, 0x20, 0xC0},
    {MODE_NEC, IR_KEY_VOL_DEC, 0x20, 0xF8},
    {MODE_NEC, IR_KEY_TOOL,    0x20, 0x10},
    {MODE_NEC, IR_KEY_UP,      0x20, 0x9A},
    {MODE_NEC, IR_KEY_DOWN,    0x20, 0x8A},
    {MODE_NEC, IR_KEY_LEFT,    0x20, 0x6A},
    {MODE_NEC, IR_KEY_RIGHT,   0x20, 0x28},
    {MODE_NEC, IR_KEY_OK,      0x20, 0xAA},
    {MODE_NEC, IR_KEY_PIP,     0x20, 0x62},
    {MODE_NEC, IR_KEY_POP,     0x20, 0xA2},
    {MODE_NEC, IR_KEY_GUIDE,   0x20, 0x42},
    {MODE_NEC, IR_KEY_KMP,     0x20, 0x82},
    {MODE_NEC, IR_KEY_JUMP,    0x20, 0x4A},
    {MODE_NEC, IR_KEY_EXIT,    0x20, 0x08},
    {MODE_NEC, IR_KEY_MENU,    0x20, 0x90},
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int irda_invse_Pulse1(uint16_t *buf, uint16_t length )
{
    uint16_t i=0;
    
    for (i=0; i< length;i++)
    {
        if (buf[i] > PULSE_HIGH)
        {
            buf[i] = buf[i] & (~PULSE_HIGH);
        }
        else 
        {   
            buf[i] = buf[i] | PULSE_HIGH;
        }
    }
    return IRDA_SUCCEED;    
}

static int irda_txbuf(IRDA_BUF *pIrdabuf,IRDA_SPEC *pSpec)
{
    int i = 0,n = 0;
    uint16_t  BaseTime= 1000/pIrdabuf->CarrierFrequency_kHz;
    uint16_t  bufLen=0;

    uint16_t  Log1[MAX_LOG_WAVFORM_SIZE];
    uint16_t  Log0[MAX_LOG_WAVFORM_SIZE];
    uint8_t   Code=0;

    if (pIrdabuf->CarrierFrequency_kHz ==0)
    {
        return IRDA_ERROR;
    }
    
    BaseTime= 1000/pIrdabuf->CarrierFrequency_kHz;

    for (i=0; i < MAX_LOG_WAVFORM_SIZE; i++)
    {
            Log1[i] = HL_Time2TxBufCount(pSpec->Log1_Context[i],BaseTime);// (pSpec->Log1_Context[i] &0x8000 )| ((pSpec->Log1_Time[i]&0x7FFf) / BaseTime);
            Log0[i] = HL_Time2TxBufCount(pSpec->Log0_Context[i],BaseTime);//(pSpec->Log0_Context[i] &0x8000 )| ((pSpec->Log0_Time[i]&0x7FFF) / BaseTime);

    }
    
    //header 
    for (i=0; i<pSpec->HeaderLen; i++)
    {
        pIrdabuf->pBuf[i]=  HL_Time2TxBufCount(pSpec->Header_Context[i],BaseTime);
    }
    
    bufLen = pSpec->HeaderLen; 
    for (i=0; i<pIrdabuf->CodeLen; i++)
    {
        Code= pIrdabuf->Code[i];
        for (n=0;n<8;n++)
        {   
            /* Change encoding bit order */
            if (BIT_N(Code,n)== 0x01)//if (BIT_N(Code,7-n)== 0x01)
            {//log 1
                    pIrdabuf->pBuf[bufLen] = Log1[0];
                    pIrdabuf->pBuf[bufLen+1] = Log1[1];
            }
            else
            {//log 0
                    pIrdabuf->pBuf[bufLen] = Log0[0];
                    pIrdabuf->pBuf[bufLen+1] = Log0[1];         
            }
            bufLen+=MAX_LOG_WAVFORM_SIZE;
        }
    }

    //Stop
    if(pSpec->Stop_Context !=0x0000)
    {
        pIrdabuf->pBuf[bufLen] = HL_Time2TxBufCount(pSpec->Stop_Context,BaseTime);
        bufLen+=1;
    }
    pIrdabuf->pBufLen=bufLen;

    return IRDA_SUCCEED;
}

int irda_nec_encode(UINT8 address, UINT8 command, IRDA_BUF *pIrda)
{
    int ret = 0;
    IRDA_BUF *nec_Irdabuf = pIrda;
    
    //nec_Irdabuf->CarrierFrequency_kHz = pIrda->CarrierFrequency_kHz;
    nec_Irdabuf->CarrierFrequency_kHz = 38; //38kHz
    nec_Irdabuf->CodeLen = 4;
    nec_Irdabuf->Code[0] = address;
    nec_Irdabuf->Code[1] = ~address;
    nec_Irdabuf->Code[2] = command;
    nec_Irdabuf->Code[3] = ~command;

    ret= irda_txbuf(nec_Irdabuf,(IRDA_SPEC *)&NEC_SPEC);

#ifdef DBG_ON
    DBGPRINTF("=========== NEC Tx buf===========================\n");
    DBGPRINTF("address = 0x%x command = 0x%x \n",address, command); 
    DBGPRINTF("BufLen= 0x%x\n",pIrda->pBufLen);

    for (i=0;i<pIrda->pBufLen;i++)
    {
        if (i%8 ==0)
            DBGPRINTF("\n"); 
        DBGPRINTF("%.4x ,",pIrda->pBuf[i]);      

    }
    DBGPRINTF("\n"); 
    DBGPRINTF("============= ret = %d===============\n",ret); 
#endif

    return ret;
}

static int irda_rxbuf_LogData_Check(UINT16 *buf,
                                    UINT16 *pLog,
                                    UINT16 LogWavFormLen,
                                    UINT16 BaseTime,
                                    UINT16 Dif_Base,
                                    UINT16 Dif_BaseTime)
{
        UINT16  Time1=0,Time2=0;        
        UINT16  i=0;
        UINT16  BufValue;
        UINT16  Dif= 0;
        UINT16  temp=0;
        for (i=0;i<LogWavFormLen;i++)
        {
                BufValue = buf[i];
                
                //check H_L
                if (HL_WAVE(BufValue) != HL_WAVE(pLog[i]))
                {   
                    return -1;
                }
                //Check Time
                Time1=  TxBufCount2Time(BufValue,BaseTime); 
                Time2=  pLog[i] & 0x7FFF;
        
                Dif= (Time2/Dif_Base) *Dif_BaseTime;

                temp =  ABS_F(Time1,Time2);
                if (temp > Dif)
                {
                    return -1;
                } 
        }       

    return 0;
}

static int irda_rxbuf(IRDA_BUF *pIrdabuf,IRDA_SPEC *pSpec)
{
    UINT16  BaseTime= 0;
    UINT16  Dif_BaseTime= 0;
    UINT16  Dif= 0;
    UINT16  BufCount=0;
    UINT16  temp=0;
    UINT16  Time1,Time2;
    UINT8   BitNum=0;
    UINT8   ByteNum=0;
    UINT8   data;
    UINT8   BIT_TYPE=0;
    UINT16  BufLen=0;

    if (pIrdabuf->CarrierFrequency_kHz ==0)
        return IRDA_ERROR;

    if (irda_invse_Pulse1(pIrdabuf->pBuf,pIrdabuf->pBufLen) !=0)
        return IRDA_ERROR;
    
    BaseTime= 1000/pIrdabuf->CarrierFrequency_kHz;

    if (pSpec->Dif_Divisor == 0)
        pSpec->Dif_Divisor=1;

    Dif_BaseTime= pSpec->Dif_Base /pSpec->Dif_Divisor;

    if (pSpec->Stop_Context != 0x0000)
    {
       BufLen=pIrdabuf->pBufLen-=2; 
    }
    else
    {
      BufLen=pIrdabuf->pBufLen-1; 
    }

    //Check header
    while (BufCount < pSpec->HeaderLen)
    {
        if (HL_WAVE(pIrdabuf->pBuf[BufCount]) != HL_WAVE(pIrdabuf->pBuf[BufCount])) 
             return IRDA_HEADER_ERROR;

        Time1=  TxBufCount2Time(pIrdabuf->pBuf[BufCount],BaseTime); 
        Time2=  pSpec->Header_Context[BufCount] & 0x7FFF;
        
        Dif= Time2/(pSpec->Dif_Base) *Dif_BaseTime;

        temp =  ABS_F(Time1,Time2);
       // printf("[%d]%d",BufCount,temp);

        if (temp > Dif)
            return IRDA_HEADER_ERROR;     
        BufCount++;
    }

    BitNum=0;
    ByteNum=0;
    data=0;

    while (BufCount < BufLen)
    {
        //check log 0
        if (irda_rxbuf_LogData_Check(&pIrdabuf->pBuf[BufCount],
                                     pSpec->Log0_Context,
                                     MAX_LOG_WAVFORM_SIZE,
                                     BaseTime,
                                     pSpec->Dif_Base,
                                     Dif_BaseTime) == 0)
        {
            BIT_TYPE=LOG_LOW;
            BufCount+= MAX_LOG_WAVFORM_SIZE;
        } 
        else if (irda_rxbuf_LogData_Check(&pIrdabuf->pBuf[BufCount],
                                          pSpec->Log1_Context,
                                          MAX_LOG_WAVFORM_SIZE,
                                          BaseTime,
                                          pSpec->Dif_Base,
                                          Dif_BaseTime)== 0)
        {
            BIT_TYPE=LOG_HIGH;
            BufCount+= MAX_LOG_WAVFORM_SIZE;
        }
        else
        {
            DBG_BUFFER(MODULE_IR,
                       LEVEL_ERROR,
                       "IRDA_DATA_ERROR",
                       0);
            return IRDA_DATA_ERROR;
        }
        /* Change data's bit order */
        data |= (BIT_TYPE<<BitNum);
        //data |= (BIT_TYPE<<(7-BitNum));
        BitNum++;
#ifdef DBG_ON
        DBGPRINTF("(%d/%d) data = %x --> %d \n",BitNum,ByteNum,data,BIT_TYPE);
#endif
        if (BitNum >=8)
        {
            pIrdabuf->Code[ByteNum]=data;
            ByteNum++;
            BitNum=0;
            data=0;
            pIrdabuf->CodeLen=ByteNum;
        
        }
    }
    return IRDA_SUCCEED;
}
int irda_nec_decode(UINT8 *address,UINT8 *command,IRDA_BUF *pIrda)
{
//  IRDA_BUF *nec_Irdabuf = &Irdabuf;
    int ret=0;
    //pIrda->CarrierFrequency_kHz = pIrda->CarrierFrequency_kHz;
    pIrda->CodeLen=0;
    pIrda->Code[0]= 0;
    pIrda->Code[1]= 0;
    pIrda->Code[2]=  0;
    pIrda->Code[3]=  0;

    ret= irda_rxbuf(pIrda,(IRDA_SPEC *)&NEC_SPEC);
    
    *address = pIrda->Code[0];
    *command=  pIrda->Code[2];
#ifdef DBG_ON
    DBGPRINTF("=========== NEC Rx Code===========================\n");
    DBGPRINTF("address = 0x%x command = 0x%x \n",*address, *command);   
    DBGPRINTF("\n");   
#endif
    return ret;
}


