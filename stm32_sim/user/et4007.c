/***************************************************************************************************

***************************************************************************************************/
#include "et4007.h"
#include "bsp_SysTick.h"
#include "usart.h"	
#include  <string.h>
/***************************************************************************************************
//#define   
***************************************************************************************************/
#define		_ET4007_ADDRESS_					0x35

#define		_ET4007_CONTROL_SEND_CODE_3_		0x54

#define		_ET4007_CONTROL_START_LEARND_		0x57
#define		_ET4007_CONTROL_READ_VERSION_			0x5c
#define		_ET4007_CONTROL_READ_CODE_			0x5d
#define		_ET4007_CONTROL_STOP_LEARN_			0x5e

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define MAXSIZE 8192
#define MAX_SIZE 1024
#define MAX_INDEX 64
#define MAX_SAMPLE 16
#define MAX_SAMPLE_INDEX 32
#define MAX_DATA 310
#define MAX_ORIGINAL_DATA 620
#define MAX_SEND_DATA 380

#define COMPARE_OFFSET 2
/***************************************************************************************************
//#CONST
***************************************************************************************************/

//const char REPORT_SERVER[] = "http:\57\57api.efxnow.com\57DEMOWebServices2.8\57Service.asmx\57Echo";

/***************************************************************************************************
//#RAM
***************************************************************************************************/

 uint8_t test[] ={0x54,0x00,0x67,0x20,0x00,0x3a,0x00,0x00,0x01,0x9e,0x00,0x6f,0x01,0x58,0x00,0x13,0x00,0x3c,0x00,0x13,0x00,0x14,0x00,0x13,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x22,0x22,0x22,0x22,0x12,0x21,0x21,0x21,0x11,0x12,0x12,0x11,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x21,0x22,0x22,0x22,0x22,0x22,0x21,0x11,0x13};

/******************************************************/
/*Funcation: et_xCal_crc                      	        				*/
/*Input:  	char *ptr	uint32_t len						*/
/*Output: 	char crc 	  								 */
/*Desc: 		get whole ptr data array crc					*/

/******************************************************/
uint8_t xCal_crc(uint8_t *ptr,int len)
{
	uint8_t crc;
 	uint8_t i;
    crc = 0;
    while(len--)
    {
       crc ^= *ptr++;
       for(i = 0;i < 8;i++)
       {
           if(crc & 0x01)
           {
               crc = (crc >> 1) ^ 0x8C;
           }else crc >>= 1;
       }
    }
    return crc;
}



	
/******************************************************/
/*Funcation: Binary2Char                      	        				*/
/*Input:  	binary - 数值数组，输入要转换的数值； 
	      len - binary数组里的数值个数（以字节为单位）； 
	     buff - 字符数组，输出转换后的字符； 
	      size - buff数组的空间（以字节为单位）。 		*/
/*Output: 	true or false									 */
/*Desc: 				*/

/******************************************************/
int Binary2Char( unsigned char*  binary,  int len, char*  buff,  int size)   
	{  
	    int         i, n;  
	  
	    n = len * 2;  
	  
	    if(n > size){
			MC_INFO("Binary2Char data large than array size");
			return 0;
	    	}
	  
	    for (i = 0; i < len; i++)  
	            sprintf(buff + i * 2, "%02X", binary[i]); 
	  
	    return  n;  
	}  
/******************************************************/
/*Funcation: Char2Binary                      	        				*/
/*Input:  	binary - 数值数组，输入要转换的数值； 
	      len - binary数组里的数值个数（以字节为单位）； 
	     buff - 字符数组，输出转换后的字符； 
	      size - buff数组的空间（以字节为单位）。 		*/
/*Output: 	true or false									 */
/*Desc: 						*/

/******************************************************/
int Char2Binary( char* token,  int len, unsigned char*  binary,  int size)   
	{  
	        const char*     p;  
	    int         i, n, m;  
	        char        buf[3] = {0,0,0};  
	  
	  
	  
	    m = len % 2 ? (len - 1) / 2 : len / 2;  
	  
	   
		if(m > size){
			MC_INFO("Binary2Char data large than array size");
			return 0;
	    	}
	      p = token;
	    // 为了提高效率，先两个两个字符地转换：  
	        for (i = 0; i < m; i++)  
	        {  
	        p = token + i * 2;  
	                buf[0] = p[0];  
	                buf[1] = p[1];  
	  
	                n = 0;  
	                sscanf(buf, "%X", &n);  
	                binary[i] = n;  
	        }  
	  
	    // 再转换最后一个字符（如果有）：  
	    if(len % 2)  
	    {  
	            buf[0] = p[2];  
	            buf[1] = 0;  
	            n = 0;  
	            sscanf(buf, "%X", &n);  
	            binary[i] = n;  
	        i++;  
	    }  
	  
	    return  i;  
	}  


/******************************************************/
/*Funcation: et_compare_time                      	        				*/
/*Input:  	emote_data data, int high_level,
		int low_level					*/
/*Output: 	true or false									 */
/*Desc: 		compare  remote data time 				*/

/******************************************************/
  char compareCenter(int data, int center) {

        if ((data > center - COMPARE_OFFSET) && (data < center + COMPARE_OFFSET)) {

            return 1;
        }

        return 0;

    }

 char compare_time(int origHigh, int origLow, int sampHigh, int sampLow) {
        if (compareCenter(origHigh, sampHigh)) {
            if (compareCenter(origLow, sampLow)) {
                return 1;
            }
        }

        return 0;
    }


/******************************************************/
/*Funcation: et_compare_alldata                      	        			*/
/*Input:  	emote_data data, int *sample int index		*/
/*Output: 	true or false						 			*/
/*Desc: 		compare  remote data to all sample  				*/

/******************************************************/
char et_compare_alldata(int high_level,int low_level, int *sample, int index) {
	int i;
	int timeHigh, timeLow;

	for (i = 0; i < index; i += 2) {
		timeHigh = sample[i];
		timeLow = sample[i + 1];

		if (compare_time( high_level, low_level, timeHigh, timeLow)) {
			return 1;			
			}
		}
	return 0;	
}
		

void et_push_sample_time_data(int high_level,int low_level, int *sample, int index) {
	sample[index] = high_level;
	sample[index + 1] = low_level;
}

int et_sample_time_selection(int *original,int orig_count,int *sample) {
	int i, index;
	uint32_t high_level,low_level;
	
	index = 0;

	for (i = 0; i < orig_count && index < MAX_INDEX/2; i += 2) {
		high_level = original[i];
		low_level = original[i + 1];

		if (index != 0) {
			if (et_compare_alldata(high_level, low_level, sample, index)==0) {
				et_push_sample_time_data(high_level, low_level, sample, index);
				index += 2;
				if (index>MAX_SAMPLE){
					return -2;
					}
			}
		} else { /* first data send*/
			et_push_sample_time_data(high_level, low_level, sample, index);
			index += 2;
		}
	}
	
	return index;
}
/******************************************************/
/*Funcation: et_get_index                        	        			*/
/*Input:  	ir_remocon_data *ir_data
		int *sample, int index	   					*/
/*Output: 	index 	  								 */
/*Desc: 	data compare sample to get sample index  */

/******************************************************/

int et_get_index(int high_level, int low_level,int *sample, int index) {
	int i = 0;
	int timeHigh, timeLow;

	for (i = 0; i < index; i += 2) {
		timeHigh = sample[i];
		timeLow = sample[i + 1];

		if (compare_time(high_level, low_level,timeHigh,timeLow)) {
			
			return i;
			
		}
	}
	return MAX_SAMPLE_INDEX;

}
/******************************************************/
/*Funcation: et_get_data_index                        	        			*/
/*Input:  	ir_remocon_data *ir_data, char *data,
		int *sample, int index	   					*/
/*Output: 	index 	  								 */
/*Desc: 	original data to get sample index to compress data     */

/******************************************************/
int et_get_data_index(int *original, int *sample,int index,int orig_count,uint8_t *data) {
	int i, count = 0;
	char temp;
	uint32_t high_level,low_level;

	for (i = 0; i < orig_count; i += 2) {
		high_level = original[i];
		low_level = original[i + 1];

		temp = et_get_index(high_level,low_level, sample, index);
		if (temp>MAX_SAMPLE_INDEX){
			return -3;
			}
		data[count++] = (temp/2 ) ;
		if(count>MAX_ORIGINAL_DATA){
			return MAX_ORIGINAL_DATA;
			}
		
	}


	return  count;
}


int et_compress_data(uint8_t *data, uint8_t *compress,int data_length){
   int compress_length;
   int temp;
   int j = 0;
   int i = 0;
   if (data_length % 2 == 1) {
        compress_length  = data_length/ 2 + 1;
        } else {
        compress_length = data_length / 2 ;

   }
      
     
   while(i<data_length){

            temp = (data[i++] << 4) & 0xf0;
            if(i!=data_length) {
                temp |= (data[i++]) & 0x0f;
            }
          
            compress[j] = temp;
          
            j++;

        }

        return compress_length;
    }



   


/******************************************************/
/*Funcation: et_compress_sample                        	        */
/*Input:  	int *in int index			   */
/*Output: 	char *out  	   */
/*Desc: 	change int sample to double char sample  */

/******************************************************/
int et_compress_sample(int *sample,uint8_t *zp_sample,int sample_len) {
        int i, j = 0;
      
        for (i = 0; i <sample_len; i++) {
            zp_sample[j++] = (char) ((sample[i] >> 8) & 0xff);
            zp_sample[j++] = (char) sample[i];
        }
        return j;
    }

/******************************************************/
/*Funcation: et_compress_original_data                        	        */
/*Input:  	ir_remocon_data *ir_data			   */
/*Output: 	ir_remocon_data *ir_data	ir_data length   	   */
/*Desc: 	translate original consumer data to ET compress data	   */

/******************************************************/

int et_compress_original_data(int *original, uint8_t freq,int orig_count ,uint8_t *et_data) {
	
	uint8_t temp[MAX_SEND_DATA];
	uint8_t data[MAX_ORIGINAL_DATA];
	int sample[MAX_INDEX];
	uint8_t zp_sample[MAX_INDEX];
	uint8_t zp_data[MAX_DATA];
	int i;

	int length;
	int sample_len;
	int data_len;
	int couple_len;
	int zp_sample_len;
	memset(data,0x00,MAX_ORIGINAL_DATA);
	memset(temp,0x00,MAX_SEND_DATA);
	
	memset(sample,0x00,MAX_INDEX);
	memset(zp_sample,0x00,MAX_INDEX);
	memset(zp_data,0x00,MAX_DATA);
	
	sample_len = et_sample_time_selection(original,orig_count,sample);
	if (sample_len<0){
		MC_INFO( "  et_sample_time_selection  error \n\r");
		return sample_len;
		}
	

	couple_len = et_get_data_index(original, sample,sample_len,orig_count,data);
		
	if (couple_len<0){
		MC_INFO("  et_get_data_index  error \n\r");
		return couple_len;
		}


	data_len = et_compress_data(data,zp_data,couple_len);
	if (data_len<0){
		MC_INFO( "  et_compress_data  error \n\r");
		return data_len;
		}
	
	zp_sample_len = et_compress_sample(sample,zp_sample,sample_len);
	
	

	
	
	length= MAX_INDEX + data_len  +10;

	for (i=0;i<zp_sample_len;i++){
		temp[i] = zp_sample[i];
	}
	for (i=0;i<data_len;i++){
		temp[i  + MAX_INDEX] = zp_data[i];
		//printk("temp[%d] is 0x%x",i,ir_data->data[i]);
		}

	et_data[0] = _ET4007_CONTROL_SEND_CODE_3_;
	et_data[1] = (length>>8)&0xff;
	
	et_data[2] = length&0xff;
	et_data[3] = freq;
	
	et_data[4] = (couple_len>>8)&0xff;
	et_data[5] = (couple_len)&0xff;;  
	et_data[6] = 0x00;	
	et_data[7] = 0x00;
	et_data[8] = 0x01;
	et_data[9] = xCal_crc(temp,length-10);
	
		for (i=0;i<MAX_INDEX+data_len;i++){
			et_data[i+10]= temp[i];
			}

	return length;

}


int et4007_Learndata_UnCompress(char *datas, int *code_buffer, int *freq) {

	int temp;
	uint8_t  n_Crc;
	uint8_t  n_PartIndexCount;
	uint8_t  n_Sample;
	uint8_t  n_Index;
	uint8_t  n_Freq;

	uint8_t * learn_buffer;

	uint8_t unzip_end = 1;
	uint8_t dat_temp;

	uint8_t PartIndexCount;
	int Sample0_nHighLevel;
	int Sample0_nLowLevel;
	int Sample1_nHighLevel;
	int Sample1_nLowLevel;

	uint8_t n_PartIndexCount_p;
	uint8_t n_Sample_p;
	uint8_t n_Index_p;

	int m, n;
	uint8_t shift = 0x80;

	n_Crc = datas[0];
	n_PartIndexCount = datas[1];
	n_Sample = datas[2];
	n_Index = datas[3];
	n_Freq = datas[4];
	 MC_INFO("freq = %x", n_Freq);
	 MC_INFO("n_Crc = %x", n_Crc);
	 MC_INFO("n_PartIndexCount = %d", n_PartIndexCount);
	 MC_INFO("n_Sample = %d", n_Sample);
	
		 
	if(n_PartIndexCount<2)
		return -2;
	if(n_Sample<1)
		return -3;
	if(n_Index<2)
		return -4;
	
	if((n_Freq>0x80)||(n_Freq<0x15))
		return -1;
	if((n_PartIndexCount+n_Sample+n_Index)>386)
		return -6;
	n_Freq++;
	*freq = 2500000 / n_Freq;
 	MC_INFO("freq = %d ",*freq);
	 
	learn_buffer = datas + 5;

	if( n_Crc != xCal_crc(learn_buffer, n_PartIndexCount + n_Sample + n_Index )) return -5;
	// ALOGD("crc right");
	n = 0;
	n_PartIndexCount_p = 0;
	n_Sample_p = n_PartIndexCount;
	n_Index_p = n_PartIndexCount + n_Sample;

	PartIndexCount = learn_buffer[n_PartIndexCount_p];
	Sample0_nHighLevel = 0x00000000;
	Sample0_nLowLevel = 0x00000000;
	Sample1_nHighLevel = (int) learn_buffer[n_Sample_p];
	Sample1_nHighLevel <<= 8;
	Sample1_nHighLevel |= (int) learn_buffer[n_Sample_p + 1];
	Sample1_nLowLevel = (int) learn_buffer[n_Sample_p + 2];
	Sample1_nLowLevel <<= 8;
	Sample1_nLowLevel |= (int) learn_buffer[n_Sample_p + 3];

	dat_temp = learn_buffer[n_Index_p];
	while (PartIndexCount--) {
		if (0x00 == shift) {
			n_Index_p++;
			dat_temp = learn_buffer[n_Index_p];
			shift = 0x80;
		}
		if (dat_temp & shift) {
			code_buffer[n] = Sample1_nHighLevel;
			n++;
			code_buffer[n] = Sample1_nLowLevel * 8 / n_Freq;
			code_buffer[n]++;
			n++;
			if (0x0000ffff == Sample1_nLowLevel) {
				unzip_end = 0;
				break;
			}
		} else {
			code_buffer[n] = Sample0_nHighLevel;
			n++;
			code_buffer[n] = Sample0_nLowLevel * 8 / n_Freq;
			code_buffer[n]++;
			n++;
		}
		shift >>= 1;
	}
	n_PartIndexCount_p++;
 //	ALOGD("PartIndexCount is right n = %d",n);
	while (unzip_end) {
		PartIndexCount = learn_buffer[n_PartIndexCount_p];
		Sample0_nHighLevel = (int) learn_buffer[n_Sample_p];
		Sample0_nHighLevel <<= 8;
		Sample0_nHighLevel |= (int) learn_buffer[n_Sample_p + 1];
		Sample0_nLowLevel = (int) learn_buffer[n_Sample_p + 2];
		Sample0_nLowLevel <<= 8;
		Sample0_nLowLevel |= (int) learn_buffer[n_Sample_p + 3];
		Sample1_nHighLevel = (int) learn_buffer[n_Sample_p + 4];
		Sample1_nHighLevel <<= 8;
		Sample1_nHighLevel |= (int) learn_buffer[n_Sample_p + 5];
		Sample1_nLowLevel = (int) learn_buffer[n_Sample_p + 6];
		Sample1_nLowLevel <<= 8;
		Sample1_nLowLevel |= (int) learn_buffer[n_Sample_p + 7];
		while (PartIndexCount--) {
			if (0x00 == shift) {
				n_Index_p++;
				dat_temp = learn_buffer[n_Index_p];
				shift = 0x80;
			}
			if (dat_temp & shift) {
				code_buffer[n] = Sample1_nHighLevel;
				n++;
				code_buffer[n] = Sample1_nLowLevel * 8 / n_Freq;
				code_buffer[n]++;
				n++;
			//	ALOGD("unzip   n = %d",n);
				if (0x0000ffff == Sample1_nLowLevel) {
					unzip_end = 0;
					break;

				if(n>960){
					unzip_end = 0;
					break;

					}
				}
			} else {
				code_buffer[n] = Sample0_nHighLevel;
				n++;
				code_buffer[n] = Sample0_nLowLevel * 8 / n_Freq;
				code_buffer[n]++;
				n++;
			//	ALOGD("unzip   n = %d",n);
				if(n>960){
					unzip_end = 0;
					break;

					}
			}
			shift >>= 1;
		}
		n_PartIndexCount_p++;
		n_Sample_p += 4;
	}
	//ALOGD("unzip_end is right n = %d",n);
	return n;
}



/***************************************************************************************************
//#PROGRAM
***************************************************************************************************/

void ET4007_GPIO_Config( void )
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	

  //打开PB口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//PB5,PE5引脚设置
  GPIO_InitStructure.GPIO_Pin = PIN_SDA|PIN_SCL;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//初始化对应的端口
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	ET4007_SET_SDA_HIGH;
	ET4007_SET_SCL_HIGH;
 GPIO_InitStructure.GPIO_Pin = PIN_BUSY;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//初始化对应的端口
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 
}

void ET4007_SET_SDA_INPUT( void )
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	



  GPIO_InitStructure.GPIO_Pin = PIN_SDA;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//初始化对应的端口
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 
}

void ET4007_SET_SDA_OUTPUT( void )
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;


  GPIO_InitStructure.GPIO_Pin = PIN_SDA;
	//端口速度
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//初始化对应的端口
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 
}




/******************************************************/
/*Funcation: ET4007 GPIO I2C start bit	                                */
/*Input: 		   						   */
/*Output: 					   						   */
/******************************************************/

void ET4007_start(void)
{
//	ET4007_SET_SDA_OUTPUT();
	ET4007_SET_SDA_HIGH;
//	ET4007_SET_SCL_OUTPUT;
	ET4007_SET_SCL_HIGH;
	ET4007_SET_SDA_LOW;
	Delay_us(200);
	Delay_us(200);
	Delay_us(200);
	ET4007_SET_SCL_LOW;
	Delay_us(200);
	Delay_us(200);
}
/******************************************************/
/*Funcation: ET4007 GPIO I2C stop bit	                                */
/*Input: 		   						   */
/*Output: 					   						   */
/******************************************************/

void ET4007_stop(void)
{
	Delay_us(200);
	ET4007_SET_SDA_LOW;
//	ET4007_SET_SDA_OUTPUT();
	Delay_us(200);
	ET4007_SET_SCL_HIGH;
	Delay_us(200);
	ET4007_SET_SDA_HIGH;
	Delay_us(200);
//	ET4007_SET_SDA_INPUT();
	Delay_us(200);
}


/******************************************************/
/*Funcation: ET4007 GPIO I2C writebyte	                                */
/*Input: 	uint8_t dat			   						   */
/*Output: 					   						   */
/******************************************************/

uint8_t ET4007_writebyte(uint8_t dat)  //?????
{
	uint8_t i,dat_temp,err;
	
	err=0;
	dat_temp=dat;
//	ET4007_SET_SDA_OUTPUT();
	for( i = 0; i != 8; i++ )
	{
		Delay_us(10);
		if( dat_temp & 0x80 ) 
		{
			ET4007_SET_SDA_HIGH;
		}
		else 
		{
			ET4007_SET_SDA_LOW;
		}
		Delay_us(50);
		ET4007_SET_SCL_HIGH;
		dat_temp <<= 1;
		Delay_us(50);  // ????
		ET4007_SET_SCL_LOW;
		Delay_us(10);
	}
	ET4007_SET_SDA_HIGH;
//	ET4007_SET_SDA_INPUT;
	Delay_us(50);
	ET4007_SET_SCL_HIGH;
	Delay_us(50);
	//if(ET4007_GET_SDA_STATE())err=1;
	//else err=0;
	ET4007_SET_SCL_LOW;
	Delay_us(50);
	return err;
}



/******************************************************/
/*Funcation: ET4007 GPIO I2C readbyte	                                */
/*Input: 					   							   */
/*Output: 	one byte 			   						   */
/******************************************************/

uint8_t ET4007_readbyte(void)
{
	uint8_t dat,i;
	ET4007_SET_SDA_HIGH;
	ET4007_SET_SDA_INPUT();
	//Delay_us(50); 
	dat = 0;
	for( i = 0; i != 8; i++ )
	{
		Delay_us(50); 
		ET4007_SET_SCL_HIGH;
		Delay_us(50); 
		dat <<= 1;
	 if( Bit_SET == GPIO_ReadInputDataBit( GPIOA, PIN_SDA ) ) 
      dat |= 0X01; 
   
		//if( ET4007_GET_SDA_STATE ) dat++;
		ET4007_SET_SCL_LOW;
		Delay_us(50);
	}
	ET4007_SET_SDA_OUTPUT();
	Delay_us(50);
	ET4007_SET_SDA_LOW;
	Delay_us(50);
	ET4007_SET_SCL_HIGH;
	Delay_us(50);
	ET4007_SET_SCL_LOW;
	Delay_us(50);
	return dat;
}
/************************????**********************/
void send_normal_remote(){
int i;
int len;
len = sizeof(test);
	ET4007_start();
	ET4007_writebyte(_ET4007_ADDRESS_);
	for(i=0;i<len;i++)
	{
		ET4007_writebyte(test[i]);
	}
	ET4007_stop();
}
/******************************************************/
/*Funcation: read ET4007yd18 version code					   */
/*Input: 					   							   */
/*Output: 	ir_version[] version number					   */
/******************************************************/

 int ET_IR_read_device_info(char ir_version[])
{
	

	int i;
	//char ir_version[4];
//	printk(KERN_INFO"%s called\n", __func__);
	//char buf_ir[8];

	
	ET4007_start(); 
	ET4007_writebyte(_ET4007_ADDRESS_);
	ET4007_writebyte(_ET4007_CONTROL_READ_VERSION_);
	for(i=0;i<4;i++){
		ir_version[i]=ET4007_readbyte();
	//	MC_INFO("\r\n ir_version:0x%x    \r\n",ir_version[i]);
		}
	ET4007_stop();
	



	
	return 0;
}


int  ET_IR_read_learn_data(char *buf_ir_test)
{
	int i;
	int ET_crc;
	int ET_PartIndexCount;
	int ET_Sample;
	int ET_Index;
	int ET_Freq;
	int m = 0;
	int max = 0;



	
	ET4007_start(); 
	ET4007_writebyte(_ET4007_ADDRESS_);
	ET4007_writebyte(_ET4007_CONTROL_READ_CODE_);

	ET_crc = ET4007_readbyte();
	buf_ir_test[m++] = ET_crc;
	ET_PartIndexCount = ET4007_readbyte();
	buf_ir_test[m++] = ET_PartIndexCount;
	ET_Sample = ET4007_readbyte();
	buf_ir_test[m++] = ET_Sample;
		ET_Index = ET4007_readbyte();
	buf_ir_test[m++] = ET_Index;
		ET_Freq = ET4007_readbyte();
	buf_ir_test[m++] = ET_Freq;

	max = ET_PartIndexCount+ET_Sample+ET_Index;
	MC_INFO("max = %d   \r\n",max);
	if(max>380){
		ET4007_stop();
		return 0;
		}



	
	for(i=0;i<max;i++)
		{
		buf_ir_test[m]=ET4007_readbyte();
		m++;
		}
	
	ET4007_stop();

	return	m;

}

/******************************************************/
/*Funcation: read ET4007 learn start 					   */
/*Input: 	 	   				   */
/*Output: 					   */
/******************************************************/
 void ET_IR_learn_start(void)
{
	
	ET4007_start();
	ET4007_writebyte(_ET4007_ADDRESS_);
	ET4007_writebyte(_ET4007_CONTROL_START_LEARND_);
	ET4007_stop();
	
}


/******************************************************/
/*Funcation: read ET4007 learn stop 					   */
/*Input: 	 	   				   */
/*Output: 					   */
/******************************************************/
 void ET_IR_learn_stop(void)
{
	
	ET4007_start();
	ET4007_writebyte(_ET4007_ADDRESS_);
	ET4007_writebyte(_ET4007_CONTROL_STOP_LEARN_);
	ET4007_stop();
	
}



