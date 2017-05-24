   
// FIT FOR MP3 V4 BOARD SHAPE----- fit for HFC-100-A-X18-mainboard.sch   
#include <avr/interrupt.h>   
#include <avr/signal.h>   
#include <avr/sleep.h>   
#include <avr/pgmspace.h>   
//#include <avr/eeprom.h>   
#include <avr/io.h>   
#include <avr/iom16.h>   

#define     voice_1             0   
#define     voice_2             1   
#define     voice_3             2   
#define     voice_4             3   
#define     voice_5             4   
#define     voice_6             5   
#define     voice_7             6   
#define     voice_8             7   
#define     voice_9             8   
#define     voice_10            9   
#define     voice_11            10   
#define     voice_12            11   
#define     voice_13            12   
#define     voice_14            13   
#define     voice_15            14   
#define     voice_16            15   
#define     voice_17            16   
#define     voice_18            17   
#define     voice_19            18   
#define     voice_20            19   
#define     voice_30            20   
#define     voice_40            21   
#define     voice_50            22   
#define     voice_60            23   
#define     voice_70            24   
#define     voice_80            25   
#define     voice_90            26   
#define     voice_100           27   
#define     voice_200           28   
#define     voice_seconds       29   
#define     voice_minute        30   
#define     voice_minutes       31   
#define     voice_time_period   32   
#define     voice_heart_rate    33   
#define     voice_hello         34   
#define     voice_goodbye       35   
#define     voice_space         36   
   
   
#define     on                  1   
#define     off                 0   
#define     yes                 1   
#define     no                  0   
#define     end                 0   
#define     start               1   
#define     ADCSRA_INIT         0x03        // ADC enable, clear IF, use CK/8 as clock source   
#define     AD_Heart_rate_wave  0x40        // AD channels selection: select ADC0; ref to AVCC; ADdata right adjust! device.   
#define     default_interval_times  4       // 20 seconds interval   
   
static uint8_t eep_heart_rate_high          __attribute__((section(".eeprom")));   
static uint8_t eep_heart_rate_low           __attribute__((section(".eeprom")));   
static uint8_t eep_interval_times           __attribute__((section(".eeprom")));   
static uint8_t eep_x_seconds                __attribute__((section(".eeprom")));   
   
unsigned char   i,temp00,temp11,temp22,temp33,system_set=1;   
unsigned char   up=0,down=0,direction=0,mode=0xf0,power_off_counter=0;      // variable defination   
unsigned char   filter_times=0;   
unsigned char   x_seconds_test=end,power_supply;   
unsigned char   power_down=off,temp3,stable_counter=0;   
unsigned char   power_on_off=off;   
unsigned char   heart_rate_high,heart_rate_low; // for example heart_rate==250, heart_rate_high=200,heart_rate_low=50   
   
unsigned int    heart_rate=0,heart_rate_ref=0x03ff;   
unsigned int    ad_temp=0xff,sample_number=0,counter=0;    
unsigned int    max=0x03ff,min=0x0000,overflow_times=0,interval_times=400;   
unsigned int    counter_before=0,counter_after=0;   
unsigned int    x_seconds_counter=0,x_seconds=200;   
unsigned int    key_up=0,key_down=0,key_instant=0;   
unsigned int    sample_result_before,sample_result_after;   
   
typedef struct    
{    
   unsigned char bit0 : 1,    
                 bit1 : 1,    
                 bit2 : 1,    
                 bit3 : 1,    
                 bit4 : 1,    
                 bit5 : 1,    
                 bit6 : 1,    
                 bit7 : 1;    
} bit_field;    
// Define macro to get the value of each bit    
#define GET_BIT(port) (*(volatile bit_field *) (_SFR_ADDR(port)))    
   
// Define functions for each bit of the I/O ports in the program    
   
//PORTA DEFINE                                               DDRX      PORTX   
#define     AD              GET_BIT(PINA).bit0      //INPUT  I/O  0         1   
//                                                                0         1   
//                                                                0         1   
#define     IR_EMITER       GET_BIT(PORTA).bit3     //OUTPUT I/O  1         0   
#define     LED1            GET_BIT(PORTA).bit4     //            1         1   
#define     SHUTDOWN        GET_BIT(PORTA).bit5     //            1         0   
#define     SBT             GET_BIT(PORTA).bit6     //            1         1   
#define     STAT            GET_BIT(PINA).bit7      //INPUT I/O   0         1   
   
//PORTB DEFINE    
#define     VOL_up          GET_BIT(PINB).bit0      //INPUT  I/O  0         1   
#define     VOL_down        GET_BIT(PINB).bit1      //INPUT I/O   0         1   
#define     INSTANT         GET_BIT(PINB).bit2      //INPUT I/O   0         1   
#define     U_D             GET_BIT(PORTB).bit3     //OUTPUT I/O  1         1   
#define     CS              GET_BIT(PORTB).bit4     //            1         1   
#define     OUT2            GET_BIT(PINB).bit5      //INPUT I/O   0         1   
#define     OUT1            GET_BIT(PINB).bit6      //INPUT I/O   0         1   
#define     OUT3            GET_BIT(PINB).bit7      //            0         1   
   
//PORTC DEFINE    
#define     SCL             GET_BIT(PORTC).bit0     //OUTPUT I/O  1         1   
#define     SDA             GET_BIT(PORTC).bit1     //OUTPUT I/O  1         1   
#define     M1              GET_BIT(PORTC).bit2     //            1         0   
#define     M0              GET_BIT(PORTC).bit3     //OUTPUT I/O  1         1   
#define     POWER_CTRL      GET_BIT(PORTC).bit4     //            1         0   
   
   
//PORTD DEFINE    
#define     S1              GET_BIT(PORTD).bit0     //OUTPUT I/O  1         0   
#define     S2              GET_BIT(PORTD).bit1     //OUTPUT I/O  1         1   
#define     S3              GET_BIT(PORTD).bit2     //OUTPUT I/O  1         1   
#define     S4              GET_BIT(PORTD).bit3     //OUTPUT I/O  1         1   
#define     S5              GET_BIT(PORTD).bit4     //OUTPUT I/O  1         1   
#define     S6              GET_BIT(PORTD).bit5     //OUTPUT I/O  1         1   
#define     S7              GET_BIT(PORTD).bit6     //OUTPUT I/O  1         1   
#define     S8              GET_BIT(PORTD).bit7     //OUTPUT I/O  1         1   
   
//--------------------------------------------------------------------------------------------------------------//   
void  delay_1ms(void)               //delay_1ms function   
    {      
        unsigned int i;   
        for(i=0;i<47;i++)   
        ;   
    }   
void  delay(unsigned int number)    //delay function                   
    {   
        unsigned int i=0;   
        while(i<number)   
        {      
            delay_1ms();   
            i++;   
        }      
    }   
//--------------------------------------------------------------------------------------------------------------//   
void    voice(unsigned char address)   
    {   
        SBT=0;                              // ensure that SBT was clear before lock the address of voice   
        //PORTC=address;   
        if(address&0x80)   
        {   
            S8=1;   
        }   
        else   
        {   
            S8=0;   
        }          
        if(address&0x40)   
        {   
            S7=1;   
        }   
        else   
        {   
            S7=0;   
        }   
        if(address&0x20)   
        {   
            S6=1;   
        }   
        else   
        {   
            S6=0;   
        }   
        if(address&0x10)   
        {   
            S5=1;   
        }   
        else   
        {   
            S5=0;   
        }   
        if(address&0x08)   
        {   
            S4=1;   
        }   
        else   
        {   
            S4=0;   
        }   
        if(address&0x04)   
        {   
            S3=1;   
        }   
        else   
        {   
            S3=0;   
        }   
        if(address&0x02)   
        {   
            S2=1;   
        }   
        else   
        {   
            S2=0;   
        }   
        if(address&0x01)   
        {   
            S1=1;   
        }   
        else   
        {   
            S1=0;   
        }          
        SBT=1;   
        while(OUT3 == 0);   
        while(OUT3 == 1)   
        {   
            x_seconds_counter=0;            //empty   
            power_off_counter=0;   
        }   
        SBT=0;   
    }   
//--------------------------------------------------------------------------------------------------------------//   
void    port_init(void)   
    {          
        DDRA=0b01111000;                    //initial PORT A   
        PORTA=0b11010111;   
        DDRB=0b00011000;                    //initial PORT B   
        PORTB=0b11111111;   
        DDRC=0b00011111;                    //initial PORT C   
        PORTC=0b11101011;   
        DDRD=0b11111111;                    //initial PORT D   
        PORTD=0b11111110;   
    }   
//--------------------------------------------------------------------------------------------------------------//   
void    timer0_init(void)   
    {   
        TCNT0=99;                   // 256-99=156; 156*64us=9.984ms @=10ms   
        TCCR0=0x03;   
        TIMSK |= (1<<TOIE0);      // enable timer0 overflow interrupt   
    }   
void    close_timer0(void)   
    {   
        TCCR0=0x00;   
    }   
//--------------------------------------------------------------------------------------------------------------//   
void    timer2_init(void)   
    {   
        TCNT2  = 61;                // 256-61=195; 195*256us=49.92ms per cycle namely ADC's sample rate   
        TCCR2  = 0x06;              // CS22:CS21:CS20=1:1:0 1/256 prescaler   
        TIMSK |= (1<<TOIE2);      // enable timer2 overflow interrupt        
    }   
//--------------------------------------------------------------------------------------------------------------//   
   
unsigned char eeprom_read_asm(unsigned char eepAddr_asm)   
{   
    cli();   
    asm volatile (  " aa:sbic   0x1C,1  \n\t"   
                    " rjmp  aa          \n\t"   
                    " clr   r25         \n\t"   
                    " out   0x1F,r25    \n\t"   
                    " out   0x1E,r24    \n\t"   
                    " sbi   0x1C,0      \n\t"   
                    " clr   r25         \n\t"   
                    " in    r24,0x1D    \n\t"   
                    : :   
                    );   
    sei();   
}   
//--------------------------------------------------------------------------------------------------------------//   
void eeprom_write_asm(unsigned char eepAddr_asm,unsigned char eepData_asm)   
{   
    cli();   
    asm volatile (  " bb:sbic   0x1C,1  \n\t"   
                    " rjmp  bb          \n\t"   
                    " clr   r25         \n\t"   
                    " out   0x1F,r25    \n\t"   
                    " out   0x1E,r24    \n\t"   
                    " out   0x1D,r22    \n\t"   
                    " sbi   0x1C,2      \n\t"   
                    " sbi   0x1C,1      \n\t"   
                    : :   
                    );   
    sei();   
}   
//--------------------------------------------------------------------------------------------------------------//   
//--------------------------------------------------------------------------------------------------------------//   
uint16_t ReadADC(unsigned char AD_CHANNEL)   
{   
    uint16_t adResult;   
    ADMUX  = AD_CHANNEL;            // ADCn   
    ADCSRA = ADCSRA_INIT;           // division factor between the system clock frequency and the input clock to the ADC,clk/8   
    ADCSRA |= (1<<ADEN);          // ADC enable   
    ADCSRA |= (1<<ADSC);          // ADC start conversion   
    while(!(ADCSRA & (1<<ADIF))); // wait for conversion done, ADIF flag active .   
    ADCSRA |= (0<<ADIF);          // clear ADIF   
    ADCSRA |= (0<<ADEN);          // ADC disable   
    adResult = ADCL;                // ADCL as low byte of adResult   
    adResult += (ADCH << 8);      // ADCH multi 8 times as high byte of adResult   
    return (adResult);   
}   
//--------------------------------------------------------------------------------------------------------------//   
void Filter(uint16_t sample_result)   
{   
    sample_result_before=sample_result_after;   
    sample_result_after=sample_result;   
    if(sample_result_before<sample_result_after)   
    {      
        max=sample_result;   
        if((max-min)>500)    heart_rate_ref=(max/2);   
        else heart_rate_ref=0x03ff;   
    }      
    if(sample_result_before>sample_result_after)     min=sample_result_after;   
    if((sample_result_before<sample_result_after)&&(sample_result>=heart_rate_ref))   up=1;   // symbol of rise up   
    else if((sample_result_before>sample_result_after)&&(sample_result<100))          down=1; // symbol of flow down    
    if ((up==0)&&(down==1))     down=0;    
    if ((up==1)&&(down==1))         // strain the wave by software    
    {   
        heart_rate++;                   // beaten times increase   
        power_off_counter=0;   
        LED1=on;   
        up=0;                           // clear the rise symbol    
        down=0;                         // clear the down symbol           
    }   
}   
//--------------------------------------------------------------------------------------------------------------//   
void level(unsigned int value)   
    {   
        unsigned char level_hundred,level_ten,level_one;   
        level_hundred=(value/100);   
        heart_rate_high=(level_hundred*100);   
        if(level_hundred==1)    voice(voice_100);   
        else if(level_hundred==2) voice(voice_200);   
        level_ten=(value%100);   
        heart_rate_low=level_ten;   
        if(level_ten<=20)   
        {   
            if(level_ten==1)        voice(voice_1);   
            else if(level_ten==2)   voice(voice_2);   
            else if(level_ten==3)   voice(voice_3);   
            else if(level_ten==4)   voice(voice_4);   
            else if(level_ten==5)   voice(voice_5);   
            else if(level_ten==6)   voice(voice_6);   
            else if(level_ten==7)   voice(voice_7);   
            else if(level_ten==8)   voice(voice_8);   
            else if(level_ten==9)   voice(voice_9);   
            else if(level_ten==10)  voice(voice_10);   
            else if(level_ten==11)  voice(voice_11);   
            else if(level_ten==12)  voice(voice_12);   
            else if(level_ten==13)  voice(voice_13);   
            else if(level_ten==14)  voice(voice_14);   
            else if(level_ten==15)  voice(voice_15);   
            else if(level_ten==16)  voice(voice_16);   
            else if(level_ten==17)  voice(voice_17);   
            else if(level_ten==18)  voice(voice_18);   
            else if(level_ten==19)  voice(voice_19);   
            else if(level_ten==20)  voice(voice_20);           
        }   
        else   
        {   
            level_ten=(heart_rate_low/10);   
            if(level_ten==2)        voice(voice_20);   
            else if(level_ten==3)   voice(voice_30);   
            else if(level_ten==4)   voice(voice_40);   
            else if(level_ten==5)   voice(voice_50);   
            else if(level_ten==6)   voice(voice_60);   
            else if(level_ten==7)   voice(voice_70);   
            else if(level_ten==8)   voice(voice_80);   
            else if(level_ten==9)   voice(voice_90);   
            level_one=(heart_rate_low%10);   
            if(level_one==1)        voice(voice_1);   
            else if(level_one==2)   voice(voice_2);   
            else if(level_one==3)   voice(voice_3);   
            else if(level_one==4)   voice(voice_4);   
            else if(level_one==5)   voice(voice_5);   
            else if(level_one==6)   voice(voice_6);   
            else if(level_one==7)   voice(voice_7);   
            else if(level_one==8)   voice(voice_8);   
            else if(level_one==9)   voice(voice_9);   
        }   
           
    }   
unsigned int heart_rate_cal(void)   
    {   
        unsigned int heart_rate_total;   
        if((interval_times==6000)||(interval_times==2400))   
        {   
            heart_rate_total=heart_rate;// test time=60s   
        }   
        else if(interval_times==1200)   // test time=30s   
        {   
            heart_rate_total=(heart_rate*2);   
        }   
        else if(interval_times==900)    // test time=20s   
        {   
            heart_rate_total=(heart_rate*3);   
        }   
        else if(interval_times==600)    // test time=15s   
        {   
            heart_rate_total=(heart_rate*4);   
        }   
        else if(interval_times==400)    // test time=10s   
        {   
            heart_rate_total=(heart_rate*6);   
        }   
        return (heart_rate_total);   
    }   
//--------------------------------------------------------------------------------------------------------------//   
void scankey(void)   
    {   
        temp00=power_off_counter;   
        if(INSTANT == 0)   
        {   
            key_instant++;   
            if(key_instant>=40)     
            {   
                //LED1          =on;   
                power_on_off    =off;   
                power_down      =on;   
            }   
        }   
        if((VOL_up == 0) && (VOL_down == 1))   
        {   
            key_up++;   
            if(key_up>=15)           // mode 0   
            {   
                if(interval_times==6000)   
                {   
                    voice(voice_5);   
                    voice(voice_minutes);   
                    x_seconds=1200;         //test time = 60 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==2400)                  
                {   
                    interval_times=6000;   
                    voice(voice_5);   
                    voice(voice_minutes);   
                    x_seconds=1200;         //test time = 60 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==1200)                  
                {   
                    interval_times=2400;   
                    voice(voice_2);   
                    voice(voice_minutes);   
                    x_seconds=1200;         //test time = 60 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==900)                   
                {   
                    interval_times=1200;   
                    voice(voice_1);   
                    voice(voice_minute);   
                    x_seconds=600;          //test time = 30 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==600)                   
                {   
                    interval_times=900;   
                    voice(voice_40);   
                    voice(voice_5);   
                    voice(voice_seconds);   
                    x_seconds=400;          //test time = 20 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==400)                   
                {   
                    interval_times=600;   
                    voice(voice_30);   
                    voice(voice_seconds);   
                    x_seconds=300;          //test time = 15 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
            }   
        }   
        if((VOL_up == 1) && (VOL_down == 0))   
        {   
            key_down++;   
            if(key_down>=15)   
            {   
                if(interval_times==400)   
                {   
                    voice(voice_20);   
                    voice(voice_seconds);   
                    x_seconds=200;          //test time = 10 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==600)   
                {   
                    interval_times=400;   
                    voice(voice_20);   
                    voice(voice_seconds);   
                    x_seconds=200;          //test time = 10 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==900)   
                {   
                    interval_times=600;   
                    voice(voice_30);   
                    voice(voice_seconds);   
                    x_seconds=300;          //test time = 15 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==1200)   
                {   
                    interval_times=900;   
                    voice(voice_40);   
                    voice(voice_5);   
                    voice(voice_seconds);   
                    x_seconds=400;          //test time = 20 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==2400)   
                {   
                    interval_times=1200;   
                    voice(voice_1);   
                    voice(voice_minute);   
                    x_seconds=600;          //test time = 30 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
                else if(interval_times==6000)   
                {   
                    interval_times=2400;   
                    voice(voice_2);   
                    voice(voice_minutes);   
                    x_seconds=1200;         //test time = 60 seconds   
                    heart_rate=0;   
                    x_seconds_test=end;   
                    x_seconds_counter=0;   
                    overflow_times=0;   
                    system_set=1;   
                }   
            }   
        }   
        if((VOL_up == 0) && (VOL_down == 0))   
        {   
        }   
        if((VOL_up == 1) && (VOL_down == 1) && (INSTANT == 1))   
        {   
            if((key_up>1) && (key_up<15))    
            {   
                CS=1;   
                U_D=1;                          // volue up   
                CS=0;                           // volume control chip select enable   
                for(i=0;i<5;i++)   
                {   
                    U_D=0;   
                    U_D=1;   
                }   
                voice(voice_hello);   
            }   
            if((key_down>1) && (key_down<15))    
            {   
                CS=1;   
                U_D=0;                          // volume down   
                CS=0;                           // volume control chip select enable   
                for(i=0;i<5;i++)   
                {   
                    U_D=0;   
                    U_D=1;   
                }   
                voice(voice_hello);   
            }   
            if((key_instant>1) && (key_instant<15))   
            {   
                if((counter_after>=40) && (counter_after<=290))   level(counter_after);       // read out heart rate   
            }   
            key_up=0;   
            key_down=0;   
            key_instant=0;   
        }   
        power_off_counter=temp00;          
    }   
//--------------------------------------------------------------------------------------------------------------//   
SIGNAL(SIG_OVERFLOW0)                               //timer0 overflow interrupt serve program   
    {   
        TCNT0 = 99;                                 // 256-99=156; 156*64us=9.984ms @=10ms   
        ad_temp=ReadADC(AD_Heart_rate_wave);        // sample heart_rate   
        Filter(ad_temp);                            // get the max value from sample datas         
    }   
//--------------------------------------------------------------------------------------------------------------//   
SIGNAL(SIG_OVERFLOW2)                               //timer2 overflow interrupt serve program   
    {      
        TCNT2 = 61;   
        x_seconds_counter++;                        //empty   
        power_off_counter++;   
        overflow_times++;      
        if(x_seconds_counter>=x_seconds)             // 200*50ms=10,000ms=10s   
        {   
            counter=heart_rate_cal();   
            counter_before = counter_after;   
            counter_after  = counter;   
            x_seconds_counter=0;   
            x_seconds_test=end;   
        }                       // overflow_times increase by one means we get a 50ms interval   
        else if(overflow_times>=(interval_times-x_seconds))   
        {          
            x_seconds_test=start;   
            overflow_times=0;   
        }      
    }   
//--------------------------------------------------------------------------------------------------------------//   
SIGNAL(_VECTOR(0))              //BOD reset   
    {   
        eeprom_write_asm(&eep_heart_rate_high,heart_rate_high);   
        eeprom_write_asm(&eep_heart_rate_low,heart_rate_low);   
        temp22=interval_times/100;   
        eeprom_write_asm(&eep_interval_times,temp22);   
        temp33=x_seconds/100;   
        eeprom_write_asm(&eep_x_seconds,temp33);   
    }          
//--------------------------------------------------------------------------------------------------------------//   
int     main(void)                                  // main function start   
    {   
        while(1)   
        {   
            port_init();                            // ATmega16L's I/O port initialization.   
            M1=0;   
            M0=1;   
            SBT=0;   
            while(power_on_off==off)   
            {   
                if(STAT==1) LED1=on;            //LED1=on;    
                else LED1=off;   
                temp00=0;   
                while(INSTANT==0)   
                {   
                    //LED1=on;   
                    delay(50);                      // delay 50ms   
                    temp00++;   
                    if(temp00>=40)               // wait 2 seconds then turn on the unit.   
                    {   
                        power_on_off    =   on;   
                        POWER_CTRL      =   on;   
                        break;                     
                    }   
                }   
            }   
            LED1=off;   
            power_down=off;                         // clear power_down sleep mode   
            OSCCAL = pgm_read_byte(0X3FFE);   
            IR_EMITER = on;                         // close IR sensor   
            temp00= eeprom_read_asm(&eep_heart_rate_high);   
            temp11= eeprom_read_asm(&eep_heart_rate_low);   
            temp22= eeprom_read_asm(&eep_interval_times);   
            temp33= eeprom_read_asm(&eep_x_seconds);               
            if((temp00==0x00)&&(temp11==0x00)&&(temp22==0x00)&&(temp33==0x00))   
            {   
                eeprom_write_asm(&eep_heart_rate_high,0);   
                eeprom_write_asm(&eep_heart_rate_low,30);   
                eeprom_write_asm(&eep_interval_times,default_interval_times);   
                eeprom_write_asm(&eep_x_seconds,2);    
            }   
            heart_rate_high =eeprom_read_asm(&eep_heart_rate_high);   
            heart_rate_low  =eeprom_read_asm(&eep_heart_rate_low);   
            interval_times  =eeprom_read_asm(&eep_interval_times)*100;   
            x_seconds       =eeprom_read_asm(&eep_x_seconds)*100;   
            counter_after   =heart_rate_high+heart_rate_low;   
            voice(voice_hello);   
            for(i=0;i<10;i++)   
            {   
                LED1=on;   
                delay(100);   
                LED1=off;   
                delay(100);   
            }   
            voice(voice_time_period);   
            if(interval_times==6000)       
            {   
                voice(voice_5);   
                voice(voice_minutes);   
            }   
            else if(interval_times==2400)   
            {   
                voice(voice_2);   
                voice(voice_minutes);   
            }   
            else if(interval_times==1200)   
            {   
                voice(voice_1);   
                voice(voice_minute);   
            }   
            else if(interval_times==900)   
            {   
                voice(voice_40);   
                voice(voice_5);   
                voice(voice_seconds);   
            }   
            else if(interval_times==600)   
            {   
                voice(voice_30);   
                voice(voice_seconds);   
            }   
            else if(interval_times==400)   
            {   
                voice(voice_20);   
                voice(voice_seconds);   
            }   
            sei();   
            timer0_init();                          // timer0 initialization   
            timer2_init();                          // timer2 initialization   
            x_seconds_counter=0;   
            heart_rate=0;   
            power_off_counter=0;       
            while(power_down==off)   
            {   
                IR_EMITER = on;                         // ** open IR sensor       
                if(x_seconds_test==start)   
                {   
                    if(power_off_counter>=200)   
                    {   
                        power_off_counter=0;   
                        power_down=on;   
                        break;   
                    }   
                    delay(50);   
                    LED1 = off;   
                    if(power_down==on)break;   
                    set_sleep_mode(SLEEP_MODE_IDLE);    // set sleep mode --IDLE mode   
                    MCUCR |= (1<<SE);                 // Idle sleep mode selection && sleep enable   
                    sleep_mode();   
                    MCUCR |= (0<<SE);                 // sleep mode disable   
                    scankey();   
                    overflow_times=0;   
                }   
                else   
                {   
                    close_timer0();   
                    if(system_set==0)   
                    {   
                        if(counter_before>=counter_after)   
                        {   
                            if((counter_after>=40) && (counter_after<=290) && (counter_before-counter_after<90))   
                            {   
                                level(counter_after);   
                            }                                  
                        }   
                        else   
                        {   
                            if((counter_after>=40) && (counter_after<=290) && (counter_after-counter_before<90))   
                            {   
                                level(counter_after);   
                            }   
                        }   
                    }   
                    system_set=0;   
                    overflow_times=0;               // start count rest/pause time--not detect heart beat.   
                    timer2_init();   
                    while(x_seconds_test==end)   
                    {   
                        if((interval_times-x_seconds-overflow_times)<=200)   
                        {   
                            IR_EMITER = on;                         // ** open IR sensor   
                        }   
                        else    IR_EMITER = off;                        // ** close IR sensor   
                        x_seconds_counter=0;   
                        power_off_counter=0;   
                        if(x_seconds_test==start)break;   
                        scankey();   
                        if(power_down==on)break;   
                        set_sleep_mode(SLEEP_MODE_IDLE);    // set sleep mode --IDLE mode   
                        MCUCR |= (1<<SE);                 // Idle sleep mode selection && sleep enable   
                        sleep_mode();   
                        MCUCR |= (0<<SE);                 // sleep mode disable   
                    }   
                    if(power_down==on)break;   
                    x_seconds_counter=0;   
                    power_off_counter=0;   
                    heart_rate=0;   
                    timer0_init();                          // timer0 initialization   
                    timer2_init();                          // timer2 initialization                           
                }              
            }   
            close_timer0();   
            TCCR2=0;   
            IR_EMITER = off;                            // ** close IR sensor   
            eeprom_write_asm(&eep_heart_rate_high,heart_rate_high);   
            eeprom_write_asm(&eep_heart_rate_low,heart_rate_low);              
            temp22=interval_times/100;   
            eeprom_write_asm(&eep_interval_times,temp22);   
            temp33=x_seconds/100;   
            eeprom_write_asm(&eep_x_seconds,temp33);       
            LED1=on;   
            delay(1000);   
            voice(voice_goodbye);   
            power_on_off=off;   
            x_seconds_counter=0;   
            power_off_counter=0;   
            POWER_CTRL=off;                             // now it's safe to power off the unit.   
            while(INSTANT==0);   
        }   
    }  