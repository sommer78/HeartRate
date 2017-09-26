/**
****************************************************************************
* @file		 utility.h
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

#ifndef __UTILITY_H__
#define __UTILITY_H__

extern void dly_cycles(uint32 cycles);
extern void dly_us(uint32 microseconds);
extern uint32 little_endian_read_32bit(const uint8 * buffer, int32 pos);
extern void hex2str(uint8* in,int inlen,uint8*out,int outlen);

#endif

