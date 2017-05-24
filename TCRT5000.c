#include "wx_lrwpan.h"
#include "ioCC2430.h"  
 
#ifdef LRWPAN_RFD

#define ADC_AIN5 0x05         
#define uchar unsigned char
#define uint unsigned int


uchar   up=0,down=0;      // variable defination         
uchar   heart_rate_high,heart_rate_low; // for example heart_rate==250, heart_rate_high=200,heart_rate_low=50    
   
uint    heart_rate=0,heart_rate_ref=0x03ff;   
uint    ad_temp=0xff,sample_number=0;    
uint    max=0x03ff,min=0x0000,interval_times=400;   
uint    counter_before=0,counter_after=0;      
uint    sample_result_before,sample_result_after;   
   

//--------------------------------------------------------------------------------------------------------------//    
void  delay_1ms(void)               //delay_1ms function    
    {      
        uint i;   
        for(i=0;i<47;i++)   
        ;   
    }   
void  delay(uint number)    //delay function                    
    {   
        uint i=0;   
        while(i<number)   
        {      
            delay_1ms();   
            i++;   
        }      
    }   
//--------------------------------------------------------------------------------------------------------------//    

//--------------------------------------------------------------------------------------------------------------//    
void  port_init(void)   //I/O口初始化
    {          
        P0SEL&=0x00;
        P0DIR&=0x00;   
        P1SEL&=0x00;
        P1DIR&=0x00;
        P2SEL&=0x00;
        P2DIR&=0x00;
    }   
//--------------------------------------------------------------------------------------------------------------//    
void    timer1_init(void)   
    {   
        EA=1;
        T1IE=1;  //T1中断使能
        T1CNTL=99;     // 256-99=156; 156*64us=9.984ms @=10ms    
        T1CTL=0x03;   
        T1IF=0;   //清中断标志
    }   
void    close_timer1(void)   
    {   
        T1CTL=0x00;   
    }   
//--------------------------------------------------------------------------------------------------------------//    
void    timer2_init(void)   
    {   
        EA=1;
        T2IE=1;  //T2中断使能
        T2TLD=61;                // 256-61=195; 195*256us=49.92ms per cycle namely ADC's sample rate    
        T2CNF=0x06;             
        T2IF=0; //清中断标志   
    }   
//--------------------------------------------------------------------------------------------------------------//    
   

//--------------------------------------------------------------------------------------------------------------//    
//--------------------------------------------------------------------------------------------------------------//    
uint ReadADC(void)   
{   
    uint adResult;   
    ADCCFG|=(0x01<<ADC_AIN5);   //使能通道5-P0_5  
    IEN0=0x82;                      // ADC enable    
    ADCCON1|=0x40;          // ADC start conversion    
    while(!(ADCCON1&0x80)); // wait for conversion done     
    IEN0=0x80;          // ADC disable    
    adResult = ADCL;                // ADCL as low byte of adResult    
    adResult += (ADCH << 8);      // ADCH multi 8 times as high byte of adResult    
    return (adResult);   
}   
//--------------------------------------------------------------------------------------------------------------//    
void Filter(uint sample_result)   
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
        up=0;                           // clear the rise symbol     
        down=0;                         // clear the down symbol            
    }   
}   
//--------------------------------------------------------------------------------------------------------------//    
uint heart_rate_cal(void)   
    {   
        uint heart_rate_total;   
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
int main(void)                                  // main function start    
  {
      while(!(ADCCON1&0x80));                      //等待转换完成
      port_init();                          
      timer1_init();                          // timer0 initialization    
      timer2_init();                          // timer2 initialization           
      T2CNF=0; 
      ADCCON1|=0x40;                         //触发AD转换
      while(1)   
      {   
        ad_temp=ReadADC();                        // sample heart_rate    
        Filter(ad_temp);                          // get the max value from sample datas 
      }   
  }  

#endif /* LRWPAN_RFD*/