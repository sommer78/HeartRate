/**
****************************************************************************
* @file mxd2660_spi.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2016/11/18
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#ifndef __MXD2660_SPI_H__
#define __MXD2660_SPI_H__


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "system_config.h"
#include "mxdaqua.h"


/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */
typedef enum 
{
	SPI_MODE_SLV=0,
	SPI_MODE_MST=1
}SPI_MODE;

typedef enum 
{
	CPOL_CPHA_0_0=0,
    CPOL_CPHA_0_1=1,
	CPOL_CPHA_1_0=2, 
	CPOL_CPHA_1_1=3,
}CPOL_CPHA_MODE;

typedef enum 
{
	FRAME_8_BIT=0x00,
	FRAME_16_BIT=0x01,
	FRAME_32_BIT=0x02,
	FRAME_N_BIT=0x03
}FRAME_SIZE;

typedef enum
{
	TRX_DUPLEX=0,
	TX_ONLY=1,
	RX_ONLY=2
}SPI_TRX_MODE;

typedef enum
{
	SPI_RATE_8M = 0,
	SPI_RATE_4M = 1,
	SPI_RATE_2M = 3,
	SPI_RATE_1M = 7,
	SPI_RATE_500K = 15,
	SPI_RATE_100K = 79,
	SPI_RATE_31P25K = 255
}SPI_RATE;



typedef enum
{
	SPI_RX_TO        = 0,
	SPI_TFIFO_UNTH   = 1,
	SPI_TFIFO_EMPT   = 2,
	SPI_RFIFO_OVTH   = 3,
	SPI_RFIFO_OVFL   = 4,
	SPI_RFIFO_EMPT_N = 5,
	SPI_RFIFO_UDFL   = 6,
	SPI_TFIFO_UDFL   = 7,
	SPI_TFIFO_OVFL   = 8
}SPI_ISR_MASK;




#endif /* _SPI_H_ */

