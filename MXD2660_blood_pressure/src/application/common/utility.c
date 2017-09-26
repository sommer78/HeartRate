/**
****************************************************************************
* @file		 utility.c
* @brief	 the common function all declaration here
* @author	 xiaoyu
* @version	 V1.0.0
* @date		 2017/01/19
* @history	  
* @note		  
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/
#include "system_config.h"
#include "utility.h"

void dly_cycles(uint32 cycles)
{
    uint32 i, cnt;
    cnt = (cycles >> 2);
    for(i = 0; i < cnt; i++)
        __NOP();
}

void dly_us(uint32 microseconds)
{
    uint32 cnt;
    cnt = (microseconds << 4);
    dly_cycles(cnt);
}

uint32 little_endian_read_32bit(const uint8 * buffer, int32 pos)
{
    return ((uint32) buffer[pos]) | (((uint32)buffer[(pos) + 1]) << 8) | (((uint32)buffer[(pos) + 2]) << 16) | (((uint32) buffer[(pos) + 3]) << 24);
}

void hex2str(uint8* in,int inlen,uint8*out,int outlen)
{
    int i=0;
    if (inlen*2>outlen)return;
    for (i=0;i<inlen;i++)
    {
        uint8 lo=in[i]&0x0f;
        uint8 hi=(in[i]>>4)&0xf;
        if (hi>9)
            out[2*i]=(hi-10)+'A';
        else
            out[2*i]=hi+'0';
        if (lo>9)
            out[2*i+1]=(lo-10)+'A';
        else
            out[2*i+1]=lo+'0';


    }
}
