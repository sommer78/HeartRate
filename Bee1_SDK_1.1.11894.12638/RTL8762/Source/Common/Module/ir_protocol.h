#ifndef _IRDA_PROTOCOL_H
#define _IRDA_PROTOCOL_H
#include "rtl_types.h"
//#define UINT8 unsigned char
//#define UINT16 unsigned short

/**
 * @brief IR SPEC definition. For example: NEC, Sony RC6 ...
 *
*/
typedef enum
{
	MODE_NEC  = 1, /**< NEC protocol  */
	MODE_RC5  = 2, /**< RC5 protocol  */
	MODE_RC6  = 3, /**< RC6 protocol  */
	MODE_SONY = 4, /**< Sony protocol */
}IRSpecMode;

/**
 * @brief IR data length. This value will depends on IR protocol.
 *
*/
typedef enum
{
	NEC_LENGTH  = 68, /**< NEC data length  */
	RC5_LENGTH  = 80, /**< RC5 data length  */
	RC6_LENGTH  = 70, /**< RC6 data length  */
	SONY_LENGTH = 90, /**< SONY data length */
}IRModeLength;

typedef enum
{
    IR_KEY_POWER   = 0,  /**< Key Power */
    IR_KEY_MUTE    = 1,  /**< Key Mute */
    IR_KEY_0       = 2,  /**< Key 0 */
    IR_KEY_1       = 3,  /**< Key 1 */
    IR_KEY_2       = 4,  /**< Key 2 */
    IR_KEY_3       = 5,  /**< Key 3 */
    IR_KEY_4       = 6,  /**< Key 4 */
    IR_KEY_5       = 7,  /**< Key 5 */
    IR_KEY_6       = 8,  /**< Key 6 */
    IR_KEY_7       = 9,  /**< Key 7 */
    IR_KEY_8       = 10, /**< Key 8 */
    IR_KEY_9       = 11, /**< Key 9 */
    IR_KEY_CH_INC  = 12, /**< Key: Channel increase */
    IR_KEY_CH_DEC  = 13, /**< Key: Channel decrease */
    IR_KEY_VOL_INC = 14, /**< Key: Volume increase */
    IR_KEY_VOL_DEC = 15, /**< Key: Volume decrease */
    IR_KEY_TOOL    = 16, /**< Key: Tool */
    IR_KEY_UP      = 17, /**< Key: Up */
    IR_KEY_DOWN    = 18, /**< Key: Down */
    IR_KEY_LEFT    = 19, /**< Key: Left */
    IR_KEY_RIGHT   = 20, /**< Key: Right */
    IR_KEY_OK      = 21, /**< Key: OK */
    IR_KEY_PIP     = 22, /**< Key: PIP */
    IR_KEY_POP     = 23, /**< Key: POP */
    IR_KEY_GUIDE   = 24, /**< Key: Guide */
    IR_KEY_KMP     = 25, /**< Key: ???? */
    IR_KEY_JUMP    = 26, /**< Key: Jump */
    IR_KEY_EXIT    = 27, /**< Key: Exit */
    IR_KEY_MENU    = 28, /**< Key: Menu */
} IR_KEY_DEF;

/**
 * @brief IR key definition.
 *
 * Each key definition will contains IR protocol/ key name
 * key address/ key value.
 */
typedef struct _IR_KEY_DATA_DEF_
{
    IRSpecMode   IR_mode;        /**< IR protocol. Like NEC, RC6... */
    IR_KEY_DEF   IR_Key_Name;    /**< IR function key name. Like "Power", "Volume up" .. */
    uint32_t     IR_Key_Address; /**< IR key address */
    uint32_t     IR_Key_Value;   /**< IR key value */
} IR_KEY_DATA_DEF, *PIR_KEY_DATA_DEF;


typedef enum {
	    IR_UNKNOW =0,  
        IR_NEC,
        IR_NEC_EXTENDED,
     //   IR_JVC,
     //   IR_RC5,
     //   IR_RC6,
     //   IR_SONY_12BITS,
     //   IR_SONY_15BITS,
     //   IR_SONY_20BITS,
     //   IR_SHARP,
       // IR_PACE
}IRDA__TYPE_TAG;

#define MAX_IRDA_BUF_SIZ  140
#define PULSE_HIGH  0x8000
#define PULSE_LOW   0x0000 


#define PULSE_HIGH  0x8000
#define PULSE_LOW   0x0000 
#define WAVEFORM_HIGH   1
#define WAVEFORM_LOW    0 
#define LOG_HIGH		1
#define LOG_LOW			0 

typedef enum{
	IRDA_UNKNOW=0,
	
}IRDA_TYPE_TAG;

typedef struct IRDA_BUF_TAG		IRDA_BUF;
typedef struct IRDA_SPEC_TAG	IRDA_SPEC;

#define MAX_HEADDER_LEN			16
#define MAX_LOG_WAVFORM_SIZE	2
//#define MAX_CODE_SIZE           64
#define MAX_CODE_SIZE           12


enum irda_ret{
 IRDA_ERROR	   =-1,
 IRDA_SUCCEED =0,
 IRDA_HEADER_ERROR =1,
 IRDA_DATA_ERROR

};

struct IRDA_SPEC_TAG{
	
		UINT8  HeaderLen;
		UINT16 Header_Context[MAX_HEADDER_LEN];
		UINT16 Log0_Context[MAX_LOG_WAVFORM_SIZE]; 
		UINT16 Log1_Context[MAX_LOG_WAVFORM_SIZE]; 
		UINT16 Stop_Context;
		UINT16 Dif_Base;
		UINT16 Dif_Divisor; 

};
struct IRDA_BUF_TAG{
	UINT8    CarrierFrequency_kHz;
	UINT8    Code[MAX_CODE_SIZE];
	UINT8    CodeLen;
    UINT16   pBuf[MAX_IRDA_BUF_SIZ];
	//UINT16   *pBuf;
    UINT16  pBufLen;
	
};

int irda_nec_encode(UINT8 address,UINT8 command,IRDA_BUF *pIrda);
int irda_nec_decode(UINT8 *address,UINT8 *command,IRDA_BUF *pIrda);
int irda_invse_Pulse1(UINT16 *buf,UINT16 length );

extern const IR_KEY_DATA_DEF IR_Key_Definitions[29];

#endif
