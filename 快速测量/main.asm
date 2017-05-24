include  ET08PXX.inc 
include  heart_measure.inc
;===========================================
; Def_FS9821.ASM				
;===========================================

; System Register Address 00h-7Fh

w		equ		0
f		equ		1
;-------------------------------------------
; system register
;-------------------------------------------		
IND0		equ		000H	; Uses contents of FSR0 to address data memory
IND1		equ		001H	; Uses contents of FSR1 to address data memory
FRS0		equ		002H	; Indirect data memory, address point 0	
FRS1		equ		003H	; Indirect data memory, address point 1	
STATUS	equ		004H	; {rst_flag[1:0], lvd36, lvd24, pd, dc,c, z}
WORK		equ		005H	; Work register
INTF		equ		006H	; {  0, 0, 0, tlif, t0if, adif, elif, e0if}
INTE		equ		007H	; {gie, 0, 0, tlie, t0ie, adie, elie, e0ie}

;-------------------------------------------
; peripheral register
;-------------------------------------------	
MCK			equ		008H	; {hck_en, 0,0,0,0,mclksel,0,mcuclksel[1:0]}
T0LOADL	equ		009H	; t0load[7:0]
T0LOADH	equ		00AH	; {0,0,0,0,t0load[11:8]}

T0DATAL	equ		00BH	;t0data[7:0]
T0DATAH	equ		00CH	;{0,0,0,0,t0data[11:8]}

T0COUTL	equ		00DH	;t0cout[7:0](R)
T0COUTH	equ		00EH	;{0,0,0,0,t0cout[11:8]}

T0CTR		equ		00FH	;{t0_en,t0ick_rate[2:0],t0ck_sel,0,pwm0_en,buz0_en}
T1LOAD	equ		010H	;t1load[7:0]
T1DATA	equ		011H	;t1data[7:0]
T1COUT	equ		012H	;t1cout[7:0]

T1CTR		equ		013H	;{t1_en,tlick_rate[2:0],t1ck_sel,0,pwm1_en,buz1_en}
ADSET		equ		014H	;{eiref_sel,ch_sel[2,0],adcks[1:0],iref_sel[1:0]}
ADCTR		equ		015H	;{aden,adstr,adlens,chgs,0,0,0,adend} 

ADOUTH  equ   016H  ;adout[11:4]
ADOUTL  equ   017H  ;{0,0,0,0,adout[3:0]}
PT1     equ   018H  ;pt1[7:0]
PT1EN   equ   019H  ;pt1en[7:6][4:0]
PT1PU   equ   01AH  ;pt1pu[7:6][4:0]
PT1SEL  equ   01BH  ;{0,0,0,pt1sel[4:1],0}
PT1MR   equ   01CH  ;pt1mr[7:0]
PT2     equ   01DH  ;pt2[7:0]
PT2EN   equ   01EH  ;pt2en[7:0]
PT2PU   equ   01FH  ;pt2pu[7:0]
PT2SEL  equ   020H  ;{0,pt2sel[6:0]}
PT2MR   equ   021H  ;pt2mr[7:0]
PT2OD   equ   022H  ;pt2od[7:0]
PT3     equ   023H  ;{0,0,0,0,0,0,pt3[1:0]}
PT3EN   equ   024H  ;{0,0,0,0,0,0,pt3en[1:0]}
PT3PU   equ   025H  ;{0,0,0,0,0,0,pt3pu[1:0]}
INTSEL  equ   026H  ;{0,0,0,0,0,0,e1m,e0m}
WDTCTR  equ   027H  ;{wdten,0,0,0,0,wts[2:0]}
CMPCTR  equ   028H  ;{dacen,cmplen,cmp2en,cmp1ref,cmp1oe,cmp2oe,cmp1_o,cmp2_o}
DACDAT  equ   029H  ;dacdat[7:0]

;-------------------------------------------
; Status register bit map
;-------------------------------------------
Z	  	equ		0
C	  	equ		1
DC		equ		2
PD		equ		3	; Power Down Flag. 
					; Clear by write 0 or power on reset.
					; Set by Sleep or Halt instruction.
LVD24   equ     4   ;vdd<2.4v
LVD36   equ     5   ;vdd<3.6v
RST_F0  equ     6 
RST_F1  equ     7   ;reset flag(inculde power up/lvd, wdt, external pin reset)


;-------------------------------------------
; Interrput flag bit map
;-------------------------------------------
E0IF   equ		0  ;external PT1 port interrput flag
E1IF   equ		1  ;external PT2 port interrput flag
ADIF   equ      2  ;ADC interrupt flag
T0IF   equ      3  ;TIME 0 overflow interrput flag
T1IF   equ      4  ;TIME 1 overflow interrput flag


;-------------------------------------------
; Interrput enable bit map
;-------------------------------------------
E0IE   equ		0  ;external PT1 port interrput enable
E1IE   equ		1  ;external PT2 port interrput enable
ADIE   equ      2  ;ADC interrupt enable
T0IE   equ      3  ;TIME 0 overflow interrput enable
T1IE   equ      4  ;TIME 1 overflow interrput enable
GIE    equ      7  ;Globle interrupt enable


;-------------------------------------------
;MCK bit map
;-------------------------------------------
MCUS_0   equ   0 
MCUS_1   equ   1  ;mcuclk frequency select 
MCLKS    equ   3  ;mclk frequency  slelect, 1--slow clock, 0--high clock
HCK_EN   equ   7  ;HIGH clock enable signal, 0--enable, 1--disable

;-------------------------------------------
;T0CTR bit map
;-------------------------------------------
BUZ0EN   equ   0  ;buzzer output enable
PWM0EN   equ   1  ;PWM output enable, if PWM0EN=1 and BUZ0EN=1, then output pwm
T0CKS    equ   3  ;T0 clock select, 1---select PT2.2 port clock, 0--select inner divide clock
T0ICKS_0 equ   4
T0ICKS_1 equ   5
T0ICKS_2 equ   6  ;{6,5,4}=000-->mcuclk, 001-->1/2mcuclk,.....,111-->1/128mcuclk
T0EN     equ   7  ;T0 enable signal

;-------------------------------------------
;T1CTR bit map
;-------------------------------------------
BUZ1EN   equ   0  ;buzzer output enable
PWM1EN   equ   1  ;PWM output enable, if PWM0EN=1 and BUZ0EN=1, then output pwm
T1CKS    equ   3  ;T1 clock select, 1---select PT2.3 port clock, 0--select inner divide clock
T1ICKS_0 equ   4
T1ICKS_1 equ   5
T1ICKS_2 equ   6  ;{6,5,4}=000-->mcuclk/2, 001-->1/4mcuclk,.....,111-->1/256mcuclk
T1EN     equ   7  ;T1 enable signal

;-------------------------------------------
;ADCSET bit map
;-------------------------------------------
IREFS_0  equ   0
IREFS_1  equ   1  ;ADC inner refrence select, {1,0}=00-->2v, 01-->3v, 10-->4v, 11-->vdd
ADCKS_0  equ   2
ADCKS_1  equ   3  ;ADC clock select, {1,0}=00-->mcuclk/16, 01--mcuclk/8, 10-->mcuclk/2, 11-->mcuclk
CHS_0    equ   4
CHS_1    equ   5
CHS_2    equ   6  ;ADC input select, (2,1,0}=000-->AIN0, 001-->AIN1, 010-->AIN2,....,110-->AIN6, 111--> 1/4VDD
EIREFS   equ   7  ;External or Internal refrence select, 1-->External refrence, 0-->Internal refrence


;-------------------------------------------
;ADCCTR bit map
;-------------------------------------------
ADEND    equ  0  ;ADC convert finish signal
CHGS     equ  4  ;ADC Globe channel select enable
ADLENS   equ  5  ;ADC convert length select, 1-->12bit adout, 0-->8bit adout
ADSTR    equ  6  ;ADC start enable
ADEN     equ  7  ;ADC enable 

;-------------------------------------------
;INTSEL bit map
;-------------------------------------------
E0M   equ  0  ;External PT1 interrput triggle level select, 1-->posedge, 0-->negedge
E1M   equ  1  ;External PT2 interrput triggle level select, 1-->posedge, 0-->negedge

;-------------------------------------------
;WDTCTR bit map
;-------------------------------------------
WTS0   equ	 0
WTS1   equ	 1
WTS2   equ	 2 ;{0,1,2}=111-->16ms,110-->32ms, ...., 000-->
WDTEN  equ  7  ;WDT software enable

;-------------------------------------------
;CMPCTR bit map
;-------------------------------------------
CMP2O   equ   0  ;CMP 2 output
CMP1O   equ   1  ;CMP 1 output
CMP2OE  equ   2  ;CMP 2 output enable
CMP1OE  equ   3  ;CMP 1 output enable
CMP1REF equ   4  ;CMP1 refrence select, 1--DAC output
CMP2EN  equ   5  ;CMP2 enable
CMP1EN  equ   6  ;CMP1 enable
DACEN   equ   7  ;DAC enable 
   
 ;心率测量
;内置16M振荡 烧写时1分频


;PT1
SCL EQU 4  ;作为从机scl输入
SDA EQU 3  ;作为从机sda输入输出

;PT2
SEN_LED0 EQU 2
SEN_LED1 EQU 4
AN_IN EQU 6 ;心率模拟电压输入

;======RAM====== 30H-7FH
FLAG		EQU 30H         ;程序运行控制标记
   F_MAX    EQU 0   ;采样电压上升标志
   F_MIN    EQU 1   ;1秒钟到信号     
   F_MS     EQU 2   ;1MS
     F_STOP EQU 3
   F_ERR    EQU 4   ;采样到错误电平标志 
   F_SCL    EQU 5   ;SCL电平保存
   F_SEL    EQU 6   ;=1 I2C   =0 UART
   F_ADDATA EQU 7 
     

FG EQU 31H
   F_MTOP EQU 0 ;求峰值电压
   F_MBOT EQU 1 ;求谷底电压
   F_1S   EQU 2
   F_YC   EQU 3 ;算高低压的溢出数据标志
   F_POW  EQU 4
   F_QS   EQU 5
   F_255  EQU 6
   F_SERR EQU 7


ERR_BH  EQU 32H ;错误编号
   ERR_WD EQU 0 ;未佩戴，判别高低电压差值比较接近
                ;此时发00数据 
   ERR_DD EQU 1 ;抖动，判别高低电压差值比较远，低与高之间斜率较大
                ;此时发上次数据
   ERR_ZC EQU 2 ;数据正常




TMP EQU 34H


   DB0         EQU  35H 
   DB1         EQU  36H

 VOLTAGE_DAT EQU 37H ;当前采到的电压平均值 

ADR1 EQU 37H
ADR2 EQU 38H
ADR0 EQU 39H
;ADDAT_ADDL  EQU  37H
;ADDAT_ADDH  EQU  38H
;ADDAT_MAX   EQU  39H
;ADDAT_MIN   EQU  3AH
AD_JS       EQU  3AH 	  ;ad转换次数计数C

;EXINT1 RAM
;HH EQU 3CH
;HM EQU 3DH
;HL EQU 3EH

RECEIVE_COUNT   EQU 3BH
RECEIVE_DAT     EQU 3CH
RECEIVE_ERR     EQU 3DH ;错误计数器，=ff则退出
BYTE_JS         EQU 3EH

RESULT_BAK EQU 3FH

     W_BAK EQU 41H
STATUS_BAK EQU 42H
 
TEMPH        EQU 43H 
TEMPL        EQU 44H 
   TIME_H   EQU 45H ;记录每1ms的定时数据
   TIME_L   EQU 46H

   VOL_H    EQU 47H ;第1高电压数据
   TH_H     EQU 48H
   TH_L     EQU 49H


   TIM_LB_VOLH0 EQU 4AH
   TIM_LB_VOLH1 EQU 4BH
   TIM_LB_VOLL0 EQU 4CH
   TIM_LB_VOLL1 EQU 4DH
    
   TH0_H     EQU 4EH
   TH0_L     EQU 4FH
   TH1_H     EQU 50H
   TH1_L     EQU 51H

   COUNT_255 EQU 52H
   COUNT_5S  EQU 53H
;   TOP_COUNT EQU 52H
;   BOT_COUNT EQU 53H

    H_NUMBER  EQU 56H
 SEND_NUMBER  EQU 57H
    WD_COUNT  EQU 58H ;未佩戴计数，连续10秒在未佩戴状态，清h_number值
   
   VOL_L    EQU 59H ;第1低电压数据
   TL_H     EQU 5AH
   TL_L     EQU 5BH

   NUM_L0  EQU 5CH ;RAM借用
   NUM_L1  EQU 5DH
   NUM_L2  EQU 5EH
   NUM_L3  EQU 5FH
   NUM_L4  EQU 60H ;RAM借用
   NUM_L5  EQU 61H
   NUM_L6  EQU 62H
   NUM_L7  EQU 63H
 DD_COUNT  EQU 64H


   NUM_0L EQU 65H
   NUM_1L EQU 66H
   NUM_2L EQU 67H
   NUM_3L EQU 68H


   NUM_0  EQU 6BH
   NUM_1  EQU 6CH
   NUM_2  EQU 6DH
   NUM_3  EQU 6EH
   NUM_4  EQU 6FH
   NUM_5  EQU 70H
   NUM_6  EQU 71H
   NUM_7  EQU 72H

   NUM_0_BAK EQU 73H
;   FLAGF EQU 0AH
   COUNT_1S  EQU 74H
;    S1H EQU 09H
;    S1L EQU 0BH
    S1H EQU 75H
    S1L EQU 76H
   HEART_C EQU 77H 

   ;START_JS  EQU 74H ;刚开始测试的数据，不参与阈值的滤波输出计算
;   YZ_COUNT  EQU 75H ;测量超出阈值计数
;   ERR_COUNT EQU 76H ;测量错误计数 
;   RLF_JS    EQU 77H
;78H-7FH


;立即数定义
HEART_40H EQU 03AH 
HEART_40L EQU 098H 

HEART_220H EQU 00AH 
HEART_220L EQU 0A7H 

CH_AN6       EQU 01101111B ;心率电压信号 MCUCLK/1  vref=vdd 
;CH_AN6       EQU 01101111B ;心率电压信号 MCUCLK/1  vref=vdd 

SLAVE_ADDR    EQU 0ABH
SLAVE_OFF     EQU 0ACH


AD_NUM EQU 3 ;10  ;每次ad转换的次数

;采样废数据处理定义值
TOP_THREH EQU 242  ;225 ;3V下为 2.65v
TOP_THREL EQU 70H  ;峰值电压有效范围

BOT_THREH EQU 8FH ;谷值电压有效范围
BOT_THREL EQU 30 ;3V下为 0.35v


;测量窗口时间 快速测量程序无此数据
WIN_TIMH EQU 0dh ;0FH ;0BH ;007H
WIN_TIML EQU 0ach ;0A0H ;0B8H ;0D0H

;YZ_NUM EQU 4 ;5
YZ_DATA    EQU 15 ;20 ;20 ;20 15     ;小于此数4次作为废数据大于等于4次转为正常数据
YZHL_DATA  EQU 40 ;35 ;30 ;35 ;30 30  ;超出阈值数据的最大最小数据差值

NO1_DEF_MAX EQU 105 ;120 ;91 ;第一次测量默认上限
NO1_DEF_MIN EQU 50 ;70 ;第一次测量默认下限

SLBL EQU 0E7H ; 0CFH ; 0E7H
SLBH EQU 03H ;07H ;03H

TAB_NUM_MIN EQU 0  ;0=40跳  测量最低心跳值对应数据表位置
TAB_NUM_MAX EQU 19 ;59 ;19 ;19=160跳  测量最高心跳值对应数据表位置
                   ;59=200跳
;此数据与最大测量心跳值有关，THRE_H + THRE_L 不能大于最高心跳数据的周期                     
THRE_H   EQU 250 ;210 ;250 ;001H ;判断峰谷的时间阈值340ms 60/0.34=176
THRE_L   EQU 100 ;70 ;100 ;054H ;判断峰顶的时间阈值ms 60/200=300ms


   ORG 000H
RESET:
     NOP
    GOTO START
 
	org			004H

INT_SERVER:
;    BTFSC INTF, e0if ;判断外部中断 0 标志
;     GOTO EX0_INT
    BTFSC INTF, e0if ;判断外部中断 1 标志
     GOTO EX0_INT
    BTFSC INTF, t0if ;判断定时器 0 溢出中断标志
     GOTO T0_INT
    BTFSC INTF, t1if ;判断定时器 1 溢出中断标志
     GOTO T1_INT
;    BTFSC INTF, adif ;判断 ADC 结束中断标志
;     GOTO ADC_INT
  	 CLRF INTF
	 RETFIE
       




EX0_INT:
       BCF INTF,E0IF
	   BCF INTE,E0IE
       CLRWDT
	   MOVWF W_BAK
	   MOVFW STATUS
	   MOVWF STATUS_BAK
START_CHK:
;      BTFSC PT1,SDA
      BTFSS PT1,SCL
       GOTO EX1_INT_RET    
       MOVLW 08
	   MOVWF RECEIVE_COUNT
	    MOVLW 10
	    MOVWF RECEIVE_DAT 
	    CLRF RECEIVE_ERR
       BTFSC PT1,SDA
       GOTO EX1_INT_RET

START_LOOP:;等待scl有高电平转为低电平	   
       DECFSZ RECEIVE_ERR,F;//
	   GOTO START_LOOP1            ;//
	   DECFSZ RECEIVE_DAT,F
	   GOTO START_LOOP1            ;//
       GOTO EX1_INT_RET    ;//
START_LOOP1:	       
	   BTFSC PT1,SCL
	   GOTO START_LOOP

START_END:
       BCF FLAG,F_SCL
;       BTFSC PT1,SCL
;	   BSF FLAG,F_SCL
	    CLRF RECEIVE_ERR 
	    CLRF RECEIVE_DAT 
		 clrwdt
RECEIVE_LOOP:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
	   BTFSC PT1,SCL
        GOTO RECEIVE_SDA
	     BCF FLAG,F_SCL
	;	 clrwdt
	    GOTO RECEIVE_LOOP
RECEIVE_SDA:
       BTFSC FLAG,F_SCL   ;SCL上升沿读入信号
       GOTO RECEIVE_LOOP
	   BSF FLAG,F_SCL
	   CLRF RECEIVE_ERR  ;有数据读到，错误计数器清零
	   BCF STATUS,C
	   BTFSC PT1,SDA
	   BSF STATUS,C
	   RLF RECEIVE_DAT,F
	   DECFSZ RECEIVE_COUNT,F
	   GOTO RECEIVE_LOOP

	   MOVFW RECEIVE_DAT
	   XORLW SLAVE_OFF
	   BTFSC STATUS,Z
	   BCF FLAG,F_STOP

	   MOVFW RECEIVE_DAT
	   XORLW SLAVE_ADDR
	   BTFSS STATUS,Z
	   GOTO EX1_INT_RET
;	 MOVLW 00000000B 
; 	 MOVWF PT2EN   
/*发应答信号*/  
      CLRF RECEIVE_ERR ;//
ACK_WAIT1:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
       BTFSC PT1,SCL  ;等待scl高电平脉冲结束
	   GOTO ACK_WAIT1
	   CALL DLY_4US
       ;NOP
	   ;NOP
	   ;NOP
	   ;NOP
       ;NOP
	   ;NOP
	   ;NOP
	   ;NOP
       ;NOP
	   ;NOP
	   ;NOP
	   ;NOP
                     ;3us后切换口
       BSF PT1EN,SDA ;SDA作为输出口
       BCF PT1,SDA
      CLRF RECEIVE_ERR ;//
ACK_WAIT2:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
       BTFSS PT1,SCL
	   GOTO ACK_WAIT2

      CLRF RECEIVE_ERR ;//
ACK_WAIT3:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
       BTFSC PT1,SCL
	   GOTO ACK_WAIT3
	   NOP
       BCF PT1EN,SDA    ;sda释放
	   CALL  DLY_4US

       MOVLW 2
	   MOVWF BYTE_JS
	    CLRF RECEIVE_ERR
       MOVLW 08
	   MOVWF RECEIVE_COUNT
                    ;延时5us            
       BSF PT1EN,SDA    ;sda
       MOVFW SEND_NUMBER ;H_NUMBER ;TIMH_RESULT

	  ; movlw 0aah
	   MOVWF RECEIVE_DAT
   ;      BCF FLAG,F_SCL
;	   BTFSC PT2,SCL
SEND_LOOP1:
	   BSF FLAG,F_SCL
SEND_LOOP:
       DECFSZ RECEIVE_ERR,F  ;//
	   GOTO $+2            ;// 
	   GOTO EX1_INT_RET    ;//  
	   BTFSS PT1,SCL
	   GOTO SEND_N
       BSF FLAG,F_SCL
	   GOTO SEND_LOOP
SEND_N:
       BTFSS FLAG,F_SCL
	   GOTO SEND_LOOP
	   BCF FLAG,F_SCL
	   RLF RECEIVE_DAT,F
	   BTFSS STATUS,C
	   GOTO $+3
       BSF PT1,SDA
	   GOTO SEND_NEXT
       BCF PT1,SDA
SEND_NEXT:
       DECFSZ RECEIVE_COUNT,F
	   GOTO SEND_LOOP
	   DECFSZ BYTE_JS,F
	   GOTO SEND_WAIT0

      CLRF RECEIVE_ERR ;//
END_WAIT:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
       BTFSS PT1,SCL
	   GOTO END_WAIT


      CLRF RECEIVE_ERR ;//
END_WAIT1:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
       BTFSC PT1,SCL
	   GOTO END_WAIT1
	   BCF PT1EN,SDA

      CLRF RECEIVE_ERR ;//
STOP_WAIT:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    

      ; MOVFW PT2
	  ; ANDLW 00001100B
	  ; XORLW 00001100B
       MOVFW PT1 
	   ANDLW 00011000B
	   XORLW 00011000B
	   BTFSS STATUS,Z        
       GOTO STOP_WAIT
       CALL DLY_4US
       CALL DLY_4US
       CALL DLY_4US
       CALL DLY_4US
       CALL DLY_4US
;       MOVFW PT2
;	   ANDLW 00001100B
;	   XORLW 00001100B
       MOVFW PT1 
	   ANDLW 00011000B
	   XORLW 00011000B
	   BTFSS STATUS,Z        
       GOTO STOP_WAIT

	   GOTO EX1_INT_RET


SEND_WAIT0:	   
      CLRF RECEIVE_ERR ;//
SEND_WAIT00:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
	   BTFSS PT1,SCL
	   GOTO SEND_WAIT00 ;等低电平结束

      CLRF RECEIVE_ERR ;//
SEND_WAIT1:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
	   BTFSC PT1,SCL
	   GOTO SEND_WAIT1 ;等高电平结束
	   BCF PT1EN,SDA  ;进入低电平释放sda
 ;      CLRWDT
	   
      CLRF RECEIVE_ERR ;//
SEND_WAIT2:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
	   BTFSS PT1,SCL
	   GOTO SEND_WAIT2
	   BTFSC PT1,SDA  ;进入第9个scl时钟查ack信号
	   GOTO EX1_INT_RET ;无应答信号返回    
	   
	   
      CLRF RECEIVE_ERR ;//
SEND_WAIT3:
       DECFSZ RECEIVE_ERR,F;//
	   GOTO $+2            ;//
       GOTO EX1_INT_RET    ;//    
	   BTFSC PT1,SCL
	   GOTO SEND_WAIT3
	   MOVFW SEND_NUMBER ;H_NUMBER  ;TIML_RESULT
	   MOVWF RECEIVE_DAT
       MOVLW 08
	   MOVWF RECEIVE_COUNT
	   BSF PT1EN,SDA 
	   GOTO SEND_LOOP1 

EX1_INT_RET:
	   BCF PT1EN,SDA
        
	   MOVFW STATUS_BAK
	   MOVWF STATUS
	   MOVFW W_BAK

	   clrwdt 
	   BSF INTE,E0IE
	   RETFIE

;------------------------------
T0_INT:
       BCF INTF,T0IF
	   RETFIE

;------------------------------

T1_INT:
		BCF INTF,T1IF
		MOVWF W_BAK

       ; comf pt1,f

		MOVFW STATUS
		MOVWF STATUS_BAK
		BSF FLAG,F_MS
        MOVLW 1
 	    ADDWF TIME_L,F
		MOVLW 0
		ADDWFC TIME_H,F

        MOVLW 1
 	    ADDWF S1L,F
		MOVLW 0
		ADDWFC S1H,F

   ;     btfss S1L,0
	;	GOTO $+3
    ;    CALL SEN_ON
;		GOTO $+2
    ;    CALL SEN_OFF
;       INCF TOP_COUNT,F
;	   INCF BOT_COUNT,F
       BTFSC S1L,0
	   INCF COUNT_255,F

        MOVLW SLBL ;0E7H
		SUBWF S1L,W
		MOVLW SLBH ;003H
		SUBWFC S1H,W
		BTFSS STATUS,C
		GOTO T1_ENT_R
		CLRF S1L
		CLRF S1H
	;	MOVLW 01H
	;	MOVWF T0LOADH ;FLAGF
	;	MOVFW T0LOADL
	;	MOVWF T0LOADL

		BSF FG,F_1S
	    INCF COUNT_1S,F
		INCF COUNT_5S,F
T1_ENT_R:
;		comf pt1,f

		MOVFW STATUS_BAK
		MOVWF STATUS
		MOVFW W_BAK
		RETFIE



;求高低电平差值<70mv认为未佩戴
VOL_ABS:
      BCF FG,F_YC       
      MOVFW VOL_L
	  SUBWF VOL_H,W
	  BTFSS STATUS,C
      XORLW 0FFH
	  MOVWF TMP
	  MOVLW 1 ;1   ;3 ;4 ;5 ;2 ;7 ;18
	  SUBWF TMP,W
      BTFSC STATUS,C
	  RETURN  ;GOTO VOL_ABS_END
	  BSF FG,F_YC
	  RETURN


;TOP_THREH EQU 242  ;225 ;3V下为 2.65v
;TOP_THREL EQU 70H  ;峰值电压有效范围

;BOT_THREH EQU 8FH ;谷值电压有效范围
;BOT_THREL EQU 30 ;3V下为 0.35v

VOL_255H:
;       MOVFW VOL_H
;	   GOTO VOL_255
VOL_255L:
;       MOVFW VOL_L
VOL_255: ;连续出250ms高电平或低电平，认为刚戴，出255
       MOVFW TEMPL
       MOVWF TMP
	   MOVLW 225 ;TOP_THREH         ;
	   SUBWF TMP,W ;
	   BTFSC STATUS,C    ;
	   GOTO SET_F255
	   MOVLW BOT_THREL         ;
	   SUBWF TMP,W ;
	   BTFSS STATUS,C    ;
	   GOTO SET_F255
       CLRF COUNT_255
	  ; BCF FG,F_255
       RETURN
SET_F255:
	   MOVLW 250
	   SUBWF COUNT_255,W ;
	   BTFSS STATUS,C    ;
        RETURN
        BSF FG,F_255
		 CLRF COUNT_255
		 CLRF COUNT_5S
       RETURN
;=========================================
SEARCH_TOP:
      CALL VOL_255H
	  BTFSS FG,F_MTOP
      RETURN 
      MOVFW TEMPL
	  SUBWF VOL_H,W
	  BTFSC STATUS,C ;这么写法是第二次=第一次 则保留第一次数据
	  GOTO SH_LB
	  MOVFW TEMPL
	  MOVWF VOL_H
	  MOVFW TIME_H
	  MOVWF TH_H
	  MOVFW TIME_L
	  MOVWF TH_L
       CLRF TIM_LB_VOLH0
	   BSF PT2,7
	  RETURN
SH_LB:
       MOVLW 1
	   ADDWF TIM_LB_VOLH0,F
	   MOVLW THRE_H  ;THRE_L
	   SUBWF TIM_LB_VOLH0,W
	   BTFSS STATUS,C
	   RETURN
	   BSF FLAG,F_MAX
	   BSF FG,F_MBOT
	   BCF FG,F_MTOP
   
       
	   BCF PT2,7
	   RETURN

;--------------------------------------------
SEARCH_BOT:
      CALL VOL_255L
      BTFSS FG,F_MBOT
	  RETURN
      MOVFW VOL_L
	  SUBWF TEMPL,W
	  BTFSC STATUS,C
	  GOTO SL_LB

	  MOVFW TEMPL
	  MOVWF VOL_L
	  MOVFW TIME_H
	  MOVWF TL_H
	  MOVFW TIME_L
	  MOVWF TL_L
      CLRF TIM_LB_VOLL0
	  BSF PT1,0
	  RETURN
SL_LB:
      MOVLW 1
	  ADDWF TIM_LB_VOLL0,F
	   MOVLW THRE_L
	   SUBWF TIM_LB_VOLL0,W
	   BTFSS STATUS,C
       RETURN
	   BSF FLAG,F_MIN
	   BCF FG,F_MBOT
	   BSF FG,F_MTOP
	   BCF PT1,0
       RETURN
;---------------
BOT_FX:
      BCF FLAG,F_ERR
      BCF FLAG,F_MIN
      
      MOVLW BOT_THREH
	  SUBWF VOL_L,W
	  BTFSC STATUS,C
      GOTO BOT_ERR 
      MOVLW BOT_THREL
	  SUBWF VOL_L,W
	  BTFSC STATUS,C
      RETURN 
BOT_ERR:
      BSF FLAG,F_ERR
      RETURN

	  

TOP_FX:
      BCF FLAG,F_ERR
      BCF FLAG,F_MAX
      
      MOVLW TOP_THREL
	  SUBWF VOL_H,W
      BTFSS STATUS,C
	  GOTO TOP_ERR
      MOVLW TOP_THREH
	  SUBWF VOL_H,W
	  BTFSS STATUS,C
      RETURN
TOP_ERR:
      BSF FLAG,F_ERR
	  RETURN


;---------------
	   

DLY:
      MOVWF DB0
DLY_LOOP:
	  CLRWDT
	  DECFSZ DB0,F
	  GOTO DLY_LOOP
	  RETURN


DLY_4US:
       NOP
	   NOP
	   NOP
	   NOP
       NOP
	   NOP
	   NOP
	   NOP
       NOP
	   NOP
	   NOP
	   NOP
       NOP
	   NOP
       RETURN
;-----------------------------
UART_SEND:
     BCF INTE,GIE ;中断

     CALL SEND_START
	 BTFSC SEND_NUMBER,0
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,1
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,2
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,3
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,4
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,5
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,6
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
	 BTFSC SEND_NUMBER,7
     GOTO $+3
	 CALL SEND0
     GOTO $+2
	 CALL SEND1
     CALL SEND_STOP
      BSF INTE,GIE ;中断
      GOTO MATCH_R ;RETURN


SEND_START:
      BCF PT1,SDA
	 GOTO SEND0
SEND_STOP:
      BSF PT1,SDA
	 GOTO SEND1
SEND0:
      BCF PT1,SDA
	  MOVLW 102
	  GOTO DLY
   
SEND1:
      BSF PT1,SDA
	  MOVLW 102
	  GOTO DLY

;-----------------------------
SEN_ON:  ;传感器开启
      MOVLW 11101011B
	  ANDWF PT2,F
	  RETURN
SEN_OFF: ;传感器关闭
      MOVLW 00010100B
	  IORWF PT2,F
	  RETURN

;------------------------------
   
AD_PRO:   ;AD检测 数据为12位算法   
       CLRWDT
       BCF  FLAG,F_ADDATA
 	   BTFSS ADCTR,ADEND
       GOTO  AD_END
AD_DATA_INT:
      SWAPF ADOUTL,W
	  ANDLW 00001111B
	  MOVWF DB0
      SWAPF ADOUTH,W
	  ANDLW 11110000B
	  IORWF DB0,F
 
      MOVFW ADR1
	  MOVWF ADR2
	  MOVFW ADR0
	  MOVWF ADR1
	  MOVFW DB0
	  MOVWF ADR0  
AD_JG: ;AD计算
      DECFSZ AD_JS,F 
      GOTO AD_START
      MOVLW AD_NUM ;10
      MOVWF AD_JS

NUMB_PX:
       MOVLW 2 ;6
	   MOVWF DB0
NUMB_PX0:
       MOVFW ADR0
	   SUBWF ADR1,W
	   BTFSS STATUS,C
       GOTO NUMB_PX1
       MOVFW ADR0
	   MOVWF DB1
       MOVFW ADR1
	   MOVWF ADR0
	   MOVFW DB1
	   MOVWF ADR1
NUMB_PX1:
       MOVFW ADR1
	   SUBWF ADR2,W
	   BTFSS STATUS,C
       GOTO NUMB_PX2
       MOVFW ADR1
	   MOVWF DB1
       MOVFW ADR2
	   MOVWF ADR1
	   MOVFW DB1
	   MOVWF ADR2
NUMB_PX2:
       DECFSZ DB0,F
	   GOTO NUMB_PX0
          BSF FLAG,F_ADDATA 
;----------------------------------------------------------
AD_START:
        BSF ADCTR,ADSTR 
AD_END:   
        RETURN 

ram_ini:
       movlw 030h
	   movwf frs0
	   ;setdp 0
ram_ini_l:
       clrf ind0
	   incf frs0,f
	   movfw frs0
	   xorlw 7fh
	   btfss status,z
	   goto ram_ini_l
       RETURN  

NUMBER_H1:  ;40-139心跳次数表格高八位 
    ADDPCW
    RETLW            05H 
    RETLW            05H
    RETLW            05H
    RETLW            05H
    RETLW            05H
    RETLW            05H
    RETLW            05H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            04H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            03H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            02H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H

    RETLW            01H ;防数据溢出
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H


NUMBER_H2:  ;140-239心跳次数表格高八位 
    ADDPCW
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H ;
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            01H
    RETLW            00H
    RETLW            00H
    RETLW            00H
    RETLW            00H
    RETLW            00H

               
NUMBER_L1:  ;40-139心跳次数表格低八位 
    ADDPCW
    RETLW            0DCH      
    RETLW            0B7H
    RETLW            094H
    RETLW            073H
    RETLW            053H
    RETLW            035H
    RETLW            018H
    RETLW            0FCH
    RETLW            0E2H
    RETLW            0C8H
    RETLW            0B0H
    RETLW            098H
    RETLW            081H
    RETLW            06CH
    RETLW            057H
    RETLW            042H
    RETLW            02FH
    RETLW            01CH
    RETLW            00AH
    RETLW            0F8H
    RETLW            0E8H
    RETLW            0D7H
    RETLW            0C7H
    RETLW            0B8H
    RETLW            0A9H
    RETLW            09BH
    RETLW            08DH
    RETLW            07FH
    RETLW            072H   
    RETLW            065H
    RETLW            059H
    RETLW            04DH
    RETLW            041H
    RETLW            035H
    RETLW            02AH
    RETLW            020H
    RETLW            015H
    RETLW            00BH
    RETLW            001H
    RETLW            0F7H
    RETLW            0EEH
    RETLW            0E4H
    RETLW            0DBH
    RETLW            0D2H
    RETLW            0CAH
    RETLW            0C1H
    RETLW            0B9H
    RETLW            0B1H
    RETLW            0A9H
    RETLW            0A2H
    RETLW            09AH
    RETLW            093H
    RETLW            08CH
    RETLW            085H
    RETLW            07EH
    RETLW            077H
    RETLW            071H
    RETLW            06AH
    RETLW            064H
    RETLW            05EH 
    RETLW            058H
    RETLW            052H
    RETLW            04CH
    RETLW            046H
    RETLW            040H
    RETLW            03BH
    RETLW            036H
    RETLW            030H
    RETLW            02BH
    RETLW            026H
    RETLW            021H
    RETLW            01CH
    RETLW            017H
    RETLW            012H
    RETLW            00EH
    RETLW            009H
    RETLW            005H
    RETLW            000H
    RETLW            0FCH
    RETLW            0F8H
    RETLW            0F4H
    RETLW            0EFH
    RETLW            0EBH
    RETLW            0E7H
    RETLW            0E3H
    RETLW            0E0H
    RETLW            0DCH
    RETLW            0D8H
    RETLW            0D4H
    RETLW            0D1H
    RETLW            0CDH
    RETLW            0CAH
    RETLW            0C6H
    RETLW            0C3H
    RETLW            0BFH
    RETLW            0BCH
    RETLW            0B9H
    RETLW            0B5H
    RETLW            0B2H
    RETLW            0AFH

    RETLW            0ACH ;防数据溢出
    RETLW            0A9H
    RETLW            0A6H
    RETLW            0A3H
    RETLW            0A0H


NUMBER_L2:  ;140-239心跳次数表格低八位 
    ADDPCW
    RETLW            0ACH
    RETLW            0A9H
    RETLW            0A6H
    RETLW            0A3H
    RETLW            0A0H
    RETLW            09DH
    RETLW            09AH
    RETLW            098H
    RETLW            095H
    RETLW            092H
    RETLW            090H
    RETLW            08DH
    RETLW            08AH
    RETLW            088H
    RETLW            085H
    RETLW            083H
    RETLW            080H
    RETLW            07EH
    RETLW            07BH
    RETLW            079H
    RETLW            077H
    RETLW            074H
    RETLW            072H
    RETLW            070H
    RETLW            06DH
    RETLW            06BH
    RETLW            069H
    RETLW            067H
    RETLW            065H
    RETLW            063H
    RETLW            060H
    RETLW            05EH
    RETLW            05CH
    RETLW            05AH
    RETLW            058H
    RETLW            056H
    RETLW            054H
    RETLW            052H
    RETLW            051H
    RETLW            04FH
    RETLW            04DH
    RETLW            04BH
    RETLW            049H
    RETLW            047H
    RETLW            046H
    RETLW            044H
    RETLW            042H
    RETLW            040H
    RETLW            03FH
    RETLW            03DH
    RETLW            03BH
    RETLW            03AH
    RETLW            038H
    RETLW            036H
    RETLW            035H
    RETLW            033H
    RETLW            032H
    RETLW            030H
    RETLW            02FH
    RETLW            02DH
    RETLW            02CH
    RETLW            02AH
    RETLW            029H
    RETLW            027H
    RETLW            026H
    RETLW            024H
    RETLW            023H
    RETLW            021H
    RETLW            020H
    RETLW            01FH
    RETLW            01DH
    RETLW            01CH
    RETLW            01BH
    RETLW            019H
    RETLW            018H
    RETLW            017H
    RETLW            015H
    RETLW            014H
    RETLW            013H
    RETLW            011H
    RETLW            010H
    RETLW            00FH
    RETLW            00EH
    RETLW            00DH
    RETLW            00BH
    RETLW            00AH
    RETLW            009H
    RETLW            008H
    RETLW            007H
    RETLW            006H
    RETLW            004H
    RETLW            003H
    RETLW            002H
    RETLW            001H
    RETLW            000H
    RETLW            0FFH
    RETLW            0FEH
    RETLW            0FDH
    RETLW            0FCH
    RETLW            0FBH

    


;==============================   
START:   
   ; OSCM LVD未设置
   ;   BSF 2AH,7
     NOP
START1: 
     CLRF STATUS
     MOVLW 00000000B ;10B ;MCLK/4
	 MOVWF MCK
     BCF INTE,GIE ;禁止中断

WDT_SET:
     MOVLW 10000111b ;111B
	 MOVWF WDTCTR






IO_SET:
;I2C 口设置
PT1_SET:
		MOVLW 00000001B 
		MOVWF PT1EN
		MOVLW 11111111B 
		MOVWF PT1PU
;		MOVLW 11111111B
		MOVWF PT1
PT2_SET:
		MOVLW 10010100B 
		MOVWF PT2EN
		MOVLW 10101011B 
		MOVWF PT2PU
		MOVLW 01111111B
		MOVWF PT2
PT3_SET:
		MOVLW 00000000B 
		MOVWF PT3EN
		MOVLW 11111111B 
		MOVWF PT3PU
		MOVWF PT3
	
	    ;MOVFW STATUS
		;ANDLW 11000000B
		;XORLW 0
		;BTFSC STATUS,Z
		;GOTO TIM0_SET
		;BCF STATUS,7
		;BCF STATUS,6
		;GOTO MAIN

TIM0_SET: ;4MS
;     CLRF T0LOADH
;     MOVLW 249  ;溢出标志 T0IF
;     MOVWF T0LOADL
;     MOVLW 11000000B ;1/16M/16=1uS  1*16*250=4000uS
;     MOVWF T0CTR ;
;	 BSF INTE,T0IE  

     CALL RAM_INI



PT1_INT_SET: ;PT1.3作为外部中断口
     MOVLW 00000000B
     MOVWF INTSEL
     MOVLW 11110111B
     MOVWF PT1MR
 ;    BSF INTE,E1IE



SEL_CHECK: ;I2C UART通讯方式检测
        MOVFW PT1
		ANDLW 00100000B
		MOVWF DB1
		MOVLW 200
		CALL DLY
		MOVFW PT1
		ANDLW 00100000B
		XORWF DB1,W
		BTFSS STATUS,Z
		GOTO SEL_CHECK
        BCF FLAG,F_SEL
		BTFSC DB1,5
        BSF FLAG,F_SEL



      BTFSC FLAG,F_SEL
	  GOTO STOP_MODE
      BCF INTE,E0IE
;PT1_SET:
     MOVLW 11111111B
     MOVWF PT1MR

		MOVLW 00001000B 
		MOVWF PT1EN
		MOVLW 11111111B 
		MOVWF PT1PU
		MOVWF PT1
        CALL SEN_ON
	  GOTO AD_SET

;-------
STOP_MODE:
      CALL  SEN_OFF
      BSF INTE,E0IE
      BCF  ADCTR,ADEN
	  BCF  T1CTR,T1EN
      NOP
	  NOP
	  NOP
	  SLEEP
	  NOP
	  NOP

POWON_CHECK: ;检测上电命令
       BCF INTF,E0IF
	   BCF INTE,E0IE
       CLRWDT
;	   MOVWF W_BAK
;	   MOVFW STATUS
;	   MOVWF STATUS_BAK

REC_START_CHK:
;      BTFSC PT1,SDA
      BTFSS PT1,SCL
       GOTO  ERR_CHK    
       MOVLW 08
	   MOVWF RECEIVE_COUNT
	    MOVLW 10
	    MOVWF RECEIVE_DAT 
	    CLRF RECEIVE_ERR
       BTFSC PT1,SDA
       GOTO  ERR_CHK    

REC_START_LOOP:;等待scl有高电平转为低电平	   
       DECFSZ RECEIVE_ERR,F;//
	   GOTO REC_START_LOOP1            ;//
	   CLRWDT
	   DECFSZ RECEIVE_DAT,F
	   GOTO REC_START_LOOP1            ;//
       GOTO  ERR_CHK    
REC_START_LOOP1:	       
	   BTFSC PT1,SCL
	   GOTO REC_START_LOOP

REC_START_END:
       BCF FLAG,F_SCL
;       BTFSC PT1,SCL
;	   BSF FLAG,F_SCL
	    CLRF RECEIVE_DAT 
	    CLRF RECEIVE_ERR 
		 clrwdt

REC_LOOP:
       DECFSZ RECEIVE_ERR,F
	   GOTO $+2
       GOTO  ERR_CHK    
	   BTFSC PT1,SCL
        GOTO REC_SDA
	     BCF FLAG,F_SCL
	;	 clrwdt
	    GOTO REC_LOOP
REC_SDA:
       BTFSC FLAG,F_SCL   ;SCL上升沿读入信号
       GOTO REC_LOOP
	   BSF FLAG,F_SCL
	   CLRF RECEIVE_ERR  ;有数据读到，错误计数器清零
	   BCF STATUS,C
	   BTFSC PT1,SDA
	   BSF STATUS,C
	   RLF RECEIVE_DAT,F
	   DECFSZ RECEIVE_COUNT,F
	   GOTO REC_LOOP

	   MOVFW RECEIVE_DAT
	   XORLW SLAVE_ADDR
	   BTFSC STATUS,Z
	   GOTO POW_ON
POWON_CHECK_RST:     
    ;   GOTO START

ERR_CHK:
	  BTFSS FLAG,F_STOP
	  GOTO START1


POW_ON:
      CALL SEN_ON
      


AD_SET:
    BSF FLAG,F_STOP

    MOVLW  01000000B ;   P0.0=ADC0 P0.1=ADC1 P0.3=ADC3 P0.4=ADC4  
    MOVWF  PT2SEL
     MOVLW 01101011B   ;01101111 EQU 00111011B ;电位器 MCUCLK/2  vref=vdd 
     MOVWF ADSET    ; 
	 MOVLW 00110000B ;12位精度 允许选择模拟通道
     MOVWF ADCTR
     BSF  ADCTR,ADEN
     BSF  ADCTR,ADSTR

TIM1_SET:
     MOVLW 249
	 MOVWF T1LOAD
     MOVLW 11010000B
     MOVWF T1CTR ;TIM1 CLOSED
	 BSF INTE,T1IE  

      BSF INTE,E0IE
   ;   BCF INTE,E0IE ;仿真时取消i2c中断
	 BSF INTE,GIE 


ERR_RET:
     NOP
     CALL RAM_INI
     BSF FLAG,F_STOP


       MOVLW AD_NUM ;10
       MOVWF AD_JS
	  MOVLW 0FFH
	  MOVWF VOL_L    
     
      MOVLW 0FFH
	  MOVWF SEND_NUMBER
      MOVWF RESULT_BAK
      BSF FG,F_POW
      BSF FG,F_MBOT 
	  BCF FG,F_MTOP


MAIN:
      CLRWDT
	  BTFSS FLAG,F_STOP
	  GOTO START
	  
      BTFSC FG,F_1S
	  ;CALL HEAT_MATCH 
	  GOTO HEAT_MATCH 
	   
MATCH_R:
      BTFSS FLAG,F_MS
      GOTO MAIN

	  BCF FLAG,F_MS  
;	  CALL DISPLAY
HEART_MEASURE:    
      CLRWDT 
       CALL AD_PRO
      BTFSS FLAG,F_ADDATA
       GOTO HEART_MEASURE 
      
      movfw 7eh
	  movwf 7fh
	  movfw 7dh
	  movwf 7eh
	  movfw 7ch
	  movwf 7dh
	  movfw 7bh
	  movwf 7ch
	  movfw 7ah
	  movwf 7bh
	  movfw 79h
	  movwf 7ah
	  movfw 78h
	  movwf 79h
	  movfw voltage_dat
	  movwf 78h
     ; MOVWF TEMPL
     ; GOTO VOL_MATCH

	  clrf templ ;连续采到的8个数据做平均
	  clrf temph
      movlw 8
	  movwf db0
	  movlw 78h
	  movwf frs0
add_l:
      movfw ind0
	  addwf templ,f
	  movlw 0
	  addwfc temph,f 
      incf frs0,f
	  decfsz db0,f
	  goto add_l

      bcf status,c
	  rrf temph,f
	  rrf templ,f
      bcf status,c
	  rrf temph,f
	  rrf templ,f
      bcf status,c
	  rrf temph,f
	  rrf templ,f


VOL_MATCH: ;电压周期采集计算
VOL_ML:
      CALL SEARCH_BOT       
      BTFSS FLAG,F_MIN
	  GOTO VOL_MH
	  CALL BOT_FX
	  BTFSC FLAG,F_ERR
	  GOTO DATA_DEL
VOL_MH:
      CALL SEARCH_TOP       
	  BTFSS FLAG,F_MAX
	  GOTO VOL_MATCH_RET
	  CALL TOP_FX
	  BTFSC FLAG,F_ERR
	  GOTO DATA_DEL_H
;求高低电平差值<70mv认为未佩戴
	  CALL VOL_ABS
	  BTFSS FG,F_YC 
	  GOTO VOL_ABS_END
    ;  INCF FF_JS,F
	;  MOVLW 50 ;125 ;125
	;  SUBWF FF_JS,W
	;  BTFSS STATUS,C
	;  GOTO MAIN
      
	 ; MOVLW 245
	 ; SUBWF VOL_H,W
	;  BTFSC STATUS,C
	;  NOP ;GOTO RESULT_ERR_DD


	  MOVLW ERR_WD
	  MOVWF ERR_BH
     
     
     
	  GOTO RESULT_ERR


VOL_ABS_END:
 ;     CLRF FF_JS
      MOVLW 0FFH
      MOVWF VOL_L
	  CLRF VOL_H

      MOVFW TH0_L
	  MOVWF TH1_L
	  MOVFW TH0_H
	  MOVWF TH1_H

      MOVFW TH_L
	  MOVWF TH0_L
	  MOVFW TH_H
	  MOVWF TH0_H
      MOVFW TH1_H
	  XORLW 0
	  BTFSS STATUS,Z
	  GOTO HEART_MATCH
      MOVFW TH1_L
	  XORLW 0
	  BTFSS STATUS,Z
	  GOTO HEART_MATCH
VOL_MATCH_RET:
	  GOTO MAIN
;---
DATA_DEL_H:
	  CALL VOL_ABS
    BTFSC FG,F_YC 
     GOTO DATA_DEL
    MOVLW ERR_DD
    MOVWF ERR_BH

DATA_DEL: ;采集出错，存储数据初始化，重新计算
     CLRF TH0_L
     CLRF TH0_H
     CLRF TH1_L
     CLRF TH1_H
	 CLRF VOL_H
    MOVLW 0FFH
	MOVWF VOL_L
     GOTO RESULT_ERR ;_DD

HEART_MATCH: ;心率计算
     MOVFW TH0_L
	 MOVWF TEMPL
	 MOVFW TH0_H
	 MOVWF TEMPH
	 MOVFW TH1_L
	 SUBWF TEMPL,F
	 MOVFW TH1_H
	 SUBWFC TEMPH,F
	 BTFSS STATUS,C
	 GOTO MAIN


;求出次数/分钟
RESULT_SET:
      MOVLW TAB_NUM_MIN ;0 
	  MOVWF TMP
	  CALL NUMBER_H1
	  MOVWF DB1
      MOVFW TMP
	  CALL NUMBER_L1
	  MOVWF DB0
	  MOVFW DB0
	  SUBWF TEMPL,W
	  MOVFW DB1
	  SUBWFC TEMPH,W
	  BTFSC STATUS,C
	  GOTO RESULT_ERR_DD ;次数小于40次
	  
      MOVLW TAB_NUM_MAX ;59
	  MOVWF TMP
	  CALL NUMBER_H2
	  MOVWF DB1
      MOVFW TMP
	  CALL NUMBER_L2
	  MOVWF DB0
	  MOVFW DB0
	  SUBWF TEMPL,W
	  MOVFW DB1
	  SUBWFC TEMPH,W
	  BTFSS STATUS,C
	  GOTO RESULT_ERR_DD ;次数大于200次 16/04/16 改为160上限

      MOVLW 99  ;判别采用哪张表格数据
	  CALL NUMBER_H1
	  MOVWF DB1
      MOVLW 99
	  CALL NUMBER_L1
	  MOVWF DB0
	  MOVFW DB0
	  SUBWF TEMPL,W
	  MOVFW DB1
	  SUBWFC TEMPH,W
	  BTFSS STATUS,C
	  GOTO SEARCH_TAB2
SEARCH_TAB1: ;搜索第1张表格数据
      MOVLW 0FFH
	  MOVWF TMP
SEARCH_TAB1_L:
      INCF TMP,F
	  MOVFW TMP
	  CALL NUMBER_H1
	  MOVWF DB1
      MOVFW TMP
	  CALL NUMBER_L1
	  MOVWF DB0
	  MOVFW DB0
	  SUBWF TEMPL,W
	  MOVFW DB1
	  SUBWFC TEMPH,W
	  BTFSS STATUS,C
      GOTO SEARCH_TAB1_L      
TAB1_REAULT:
      DECF TMP,F
	  MOVLW 40
	  ADDWF TMP,F
	  GOTO RESULT_OUT

SEARCH_TAB2: ;搜索第2张表格数据
      MOVLW 0FFH
	  MOVWF TMP
SEARCH_TAB2_L:
      INCF TMP,F
	  MOVFW TMP
	  CALL NUMBER_H2
	  MOVWF DB1
      MOVFW TMP
	  CALL NUMBER_L2
	  MOVWF DB0
	  MOVFW DB0
	  SUBWF TEMPL,W
	  MOVFW DB1
	  SUBWFC TEMPH,W
	  BTFSS STATUS,C
      GOTO SEARCH_TAB2_L      
TAB2_REAULT:
      DECF TMP,F
	  MOVLW 140
	  ADDWF TMP,F
RESULT_OUT:
      MOVFW TMP
     GOTO HEART_LB
RESULT_ERR_DD:
	 MOVLW ERR_DD
	 MOVWF ERR_BH
RESULT_ERR:
;	 MOVFW ERR_BH
;     MOVWF H_NUMBER
     GOTO MAIN
;***********************************
;以上根据采到的电压值求出当前心率
;下面主要做心率数据的平滑处理
;***********************************
HEART_LB: ;心跳数据输出滤波
      MOVLW ERR_ZC
      MOVWF ERR_BH


;--测量的真实数据保存，在一定偏差范围内作为准确数据刷新其它数据 
;并快速显示    
      MOVFW NUM_2L
	  MOVWF NUM_3L
      MOVFW NUM_1L
	  MOVWF NUM_2L
      MOVFW NUM_0L
	  MOVWF NUM_1L
	  MOVFW TMP
	  MOVWF NUM_0L
      MOVFW NUM_3L
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO  DATA_PH
	;  GOTO  DATA_PH

D_PXS:
      MOVFW NUM_0L
	  MOVWF NUM_L0
      MOVFW NUM_1L
	  MOVWF NUM_L1
      MOVFW NUM_2L
	  MOVWF NUM_L2
      MOVFW NUM_3L
	  MOVWF NUM_L3
 
D_PX:
       MOVLW 3 
	   MOVWF TEMPH
D_PX0:
       
       MOVFW NUM_L0
	   SUBWF NUM_L1,W
	   BTFSS STATUS,C
       GOTO D_PX1
       MOVFW NUM_L0
	   MOVWF TEMPL
       MOVFW NUM_L1
	   MOVWF NUM_L0
	   MOVFW TEMPL
	   MOVWF NUM_L1
D_PX1:
       MOVFW NUM_L1
	   SUBWF NUM_L2,W
	   BTFSS STATUS,C
       GOTO D_PX2
       MOVFW NUM_L1
	   MOVWF TEMPL
       MOVFW NUM_L2
	   MOVWF NUM_L1
	   MOVFW TEMPL
	   MOVWF NUM_L2
D_PX2:
       MOVFW NUM_L2
	   SUBWF NUM_L3,W
	   BTFSS STATUS,C
       GOTO D_PX3
       MOVFW NUM_L2
	   MOVWF TEMPL
       MOVFW NUM_L3
	   MOVWF NUM_L2
	   MOVFW TEMPL
	   MOVWF NUM_L3
D_PX3:
       DECFSZ TEMPH,F
	   GOTO D_PX0
       MOVFW NUM_L3
	   SUBWF NUM_L0,F
	   MOVLW 6
	   SUBWF NUM_L0,W
	   BTFSC STATUS,C
	   GOTO DATA_PH
       MOVFW NUM_L1
	   ADDWF NUM_L2,F
	   RRF NUM_L2,F



	    MOVFW NUM_L2
		SUBWF HEART_C,W
		BTFSS STATUS,C
		GOTO DATA_PH
		MOVFW NUM_L2
		MOVWF HEART_C
		MOVWF H_NUMBER 
        MOVWF SEND_NUMBER
		MOVWF NUM_0
        MOVWF NUM_0_BAK
        MOVWF NUM_0
        MOVWF NUM_1
        MOVWF NUM_2
        MOVWF NUM_3
        MOVWF NUM_4
        MOVWF NUM_5
        MOVWF NUM_6
        MOVWF NUM_7
		GOTO RESULT_S1
;------------------------------
DATA_PH: ;数据平滑
      MOVFW NUM_1L
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO TB_CHK


	  MOVFW NUM_0L ;开机后检测到了两次数据
	  SUBWF NUM_1L,W
	  BTFSS STATUS,C
      XORLW 0FFH
	  MOVWF TEMPL
	  MOVLW 10 ;15   
	  SUBWF TEMPL,W
      BTFSC STATUS,C
	  GOTO RESULT_ERR_DD  ;前后两次差值大于15，做抖动处理
      CLRF TEMPH
	  MOVFW NUM_0L
	  MOVWF TEMPL
	  MOVFW NUM_1L
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
	  BCF STATUS,C
	  RRF TEMPH,F
	  RRF TEMPL,F
	  MOVFW TEMPL
	  MOVWF TMP  ;把当次和上次的平均数输出处理

      

TB_CHK: ;突变数据检测
      MOVFW NUM_0_BAK ;上次输入数据的备份
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO TB_CHK_END

	  MOVFW NUM_0_BAK
	  SUBWF TMP,W
	  BTFSS STATUS,C
      XORLW 0FFH
	  MOVWF TEMPL
	  MOVLW 6 ;10 ;15 
	  SUBWF TEMPL,W
      BTFSS STATUS,C
	  GOTO TB_CHK_END


	  MOVFW NUM_0_BAK ;上次与这次的测量偏差大于6，
	                  ;在原来基础上+2 -2
	  SUBWF TMP,W
	  BTFSS STATUS,C
      GOTO XZ5

     BSF FG,F_QS ;当前数据很大于上次数据
     MOVFW TMP
	 MOVWF NUM_0_BAK
	 MOVLW 2 ;3 ;5   
	 ADDWF NUM_0,W
	 MOVWF TMP
	 MOVLW 160    ;防止数据溢出
	 SUBWF TMP,W
     BTFSS STATUS,C
      GOTO NOR_DATA
     MOVLW 160
	 MOVWF TMP
      GOTO NOR_DATA
	   
XZ5:

     BCF FG,F_QS ;当前数据很小于上次数据
     MOVFW TMP
	 MOVWF NUM_0_BAK
     MOVLW 2 ;3 ;5     
	 SUBWF NUM_0,W
	 MOVWF TMP
	 MOVLW 40     ;防止数据溢出
	 SUBWF TMP,W
	 BTFSC STATUS,C
     GOTO NOR_DATA
	 MOVLW 40
	 MOVWF TMP
	 GOTO NOR_DATA

	        
TB_CHK_END:
     MOVFW TMP
	 MOVWF NUM_0_BAK          

NOR_DATA: ;正常数据
      MOVFW NUM_6
	  MOVWF NUM_7
      MOVFW NUM_5
	  MOVWF NUM_6
      MOVFW NUM_4
	  MOVWF NUM_5
      MOVFW NUM_3
	  MOVWF NUM_4

      MOVFW NUM_2
	  MOVWF NUM_3
      MOVFW NUM_1
	  MOVWF NUM_2
      MOVFW NUM_0
	  MOVWF NUM_1
      MOVFW TMP
	  MOVWF NUM_0

	  MOVFW NUM_3
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO MAIN
      
NUMPX_S:
      MOVFW NUM_0
	  MOVWF NUM_L0
      MOVFW NUM_1
	  MOVWF NUM_L1
      MOVFW NUM_2
	  MOVWF NUM_L2
      MOVFW NUM_3
	  MOVWF NUM_L3
      MOVFW NUM_4
	  MOVWF NUM_L4
      MOVFW NUM_5
	  MOVWF NUM_L5
      MOVFW NUM_6
	  MOVWF NUM_L6
      MOVFW NUM_7
	  MOVWF NUM_L7

 
NUM_PX:
       MOVLW 7 ;3 
	   MOVWF TMP
NUM_PX0:
       MOVFW NUM_L0
	   SUBWF NUM_L1,W
	   BTFSS STATUS,C
       GOTO NUM_PX1
       MOVFW NUM_L0
	   MOVWF TEMPL
       MOVFW NUM_L1
	   MOVWF NUM_L0
	   MOVFW TEMPL
	   MOVWF NUM_L1
NUM_PX1:
       MOVFW NUM_L1
	   SUBWF NUM_L2,W
	   BTFSS STATUS,C
       GOTO NUM_PX2
       MOVFW NUM_L1
	   MOVWF TEMPL
       MOVFW NUM_L2
	   MOVWF NUM_L1
	   MOVFW TEMPL
	   MOVWF NUM_L2
NUM_PX2:
       MOVFW NUM_L2
	   SUBWF NUM_L3,W
	   BTFSS STATUS,C
       GOTO NUM_PX3
       MOVFW NUM_L2
	   MOVWF TEMPL
       MOVFW NUM_L3
	   MOVWF NUM_L2
	   MOVFW TEMPL
	   MOVWF NUM_L3
NUM_PX3:
;       DECFSZ TMP,F
;	   GOTO NUM_PX0
       MOVFW NUM_L3
	   SUBWF NUM_L4,W
	   BTFSS STATUS,C
       GOTO NUM_PX4
       MOVFW NUM_L3
	   MOVWF TEMPL
       MOVFW NUM_L4
	   MOVWF NUM_L3
	   MOVFW TEMPL
	   MOVWF NUM_L4
NUM_PX4:
       MOVFW NUM_L4
	   SUBWF NUM_L5,W
	   BTFSS STATUS,C
       GOTO NUM_PX5
       MOVFW NUM_L4
	   MOVWF TEMPL
       MOVFW NUM_L5
	   MOVWF NUM_L4
	   MOVFW TEMPL
	   MOVWF NUM_L5
NUM_PX5:
       MOVFW NUM_L5
	   SUBWF NUM_L6,W
	   BTFSS STATUS,C
       GOTO NUM_PX6
       MOVFW NUM_L5
	   MOVWF TEMPL
       MOVFW NUM_L6
	   MOVWF NUM_L5
	   MOVFW TEMPL
	   MOVWF NUM_L6
NUM_PX6:
       MOVFW NUM_L6
	   SUBWF NUM_L7,W
	   BTFSS STATUS,C
       GOTO NUM_PX7
       MOVFW NUM_L6
	   MOVWF TEMPL
       MOVFW NUM_L7
	   MOVWF NUM_L6
	   MOVFW TEMPL
	   MOVWF NUM_L7
NUM_PX7:
       DECFSZ TMP,F
	   GOTO NUM_PX0
 

;      MOVFW NUM_L0
;	  XORLW 0
;	  BTFSC STATUS,Z
;	  GOTO MAIN      
;      MOVFW NUM_L1
;	  XORLW 0
;	  BTFSC STATUS,Z
;	  GOTO MAIN ;NUM_R1      

;      MOVFW NUM_L2
;	  XORLW 0
;	  BTFSC STATUS,Z
;	  GOTO NUM_R2      
	  
      MOVFW NUM_L3
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO MAIN ;NUM_DATA4      

      MOVFW NUM_L4
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO NUM_DATA4      

      MOVFW NUM_L5
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO NUM_DATA5
	        
      MOVFW NUM_L6
	  XORLW 0
	  BTFSC STATUS,Z
	  GOTO NUM_DATA6


      MOVFW NUM_L7
	  XORLW 0
	  BTFSS STATUS,Z
	  GOTO NUM_DATA8 ;4      
	  GOTO NUM_DATA7      

;NUM_R3:
;      MOVFW NUM_L1
;	  MOVWF TEMPL
;	  GOTO RESULT_S
;NUM_R1:
;      MOVFW NUM_L0
;	  MOVWF TEMPL
;	  GOTO RESULT_S
;NUM_R2:
;      CLRF TEMPL
;	  CLRF TEMPH
;	  MOVFW NUM_L0
;	  ADDWF TEMPL,F
;	  MOVLW 0
;	  ADDWFC TEMPH,F
;	  MOVFW NUM_L1
;	  ADDWF TEMPL,F
;	  MOVLW 0
;	  ADDWFC TEMPH,F
;      BCF STATUS,C
;	  RRF TEMPH,F
;      RRF TEMPL,F 
;     GOTO RESULT_S
	   


NUM_DATA4: ;4个数据处理，取中间2数据平均
      BCF FG,F_POW
      CLRF TEMPL
	  CLRF TEMPH
	  MOVFW NUM_L1
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
	  MOVFW NUM_L2
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
      BCF STATUS,C
	  RRF TEMPH,F
      RRF TEMPL,F 
;次4组数据为初次测量，强制限定数据范围在60-99
      MOVLW 100
      SUBWF TEMPL,W
	  BTFSC STATUS,C
	  GOTO SET_99
      MOVLW 60
      SUBWF TEMPL,W
	  BTFSS STATUS,C
	  GOTO SET_60
     MOVFW TEMPL
	 GOTO NUM_SET 
SET_60:
     MOVLW 60
	 GOTO SET_99
SET_99:
     MOVLW 99
NUM_SET:
     MOVWF NUM_0   
     MOVWF NUM_1   
     MOVWF NUM_2   
     MOVWF NUM_3   
     GOTO RESULT_S

NUM_DATA5: 
	  MOVFW NUM_L2
	  MOVWF TEMPL
     GOTO RESULT_S

NUM_DATA6: ;6个数据处理，取中间2数据平均
      BCF FG,F_POW
      CLRF TEMPL
	  CLRF TEMPH
	  MOVFW NUM_L2
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
	  MOVFW NUM_L3
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
      BCF STATUS,C
	  RRF TEMPH,F
      RRF TEMPL,F 
     GOTO RESULT_S

NUM_DATA7: 
	  MOVFW NUM_L3
	  MOVWF TEMPL
     GOTO RESULT_S
NUM_DATA8: ;8个数据处理，取中间4数据平均
      BCF FG,F_POW
      CLRF TEMPL
	  CLRF TEMPH
	  MOVFW NUM_L2
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
	  MOVFW NUM_L3
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F

	  MOVFW NUM_L4
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F
	  MOVFW NUM_L5
	  ADDWF TEMPL,F
	  MOVLW 0
	  ADDWFC TEMPH,F


      BCF STATUS,C
	  RRF TEMPH,F
      RRF TEMPL,F 
      BCF STATUS,C
	  RRF TEMPH,F
      RRF TEMPL,F 
     GOTO RESULT_S





RESULT_S:
      MOVFW TEMPL
      MOVWF HEART_C 
RESULT_S1:
     GOTO MAIN

;==============================
HEAT_MATCH:
      BCF FG,F_1S
      BTFSS FG,F_255
	  GOTO H_MATCH
	  MOVLW 5
	  SUBWF COUNT_5S,W
	  BTFSS STATUS,C
	  GOTO SEND_255
	  BCF FG,F_255

H_MATCH:
      MOVFW ERR_BH
	  XORLW ERR_WD
	  BTFSS STATUS,Z
	  GOTO ERR_DDCL
ERR_WDCL: ;未戴判别处理
	  BSF FG,F_POW
	  INCF WD_COUNT,F
	  MOVLW 10 ;5
	  SUBWF WD_COUNT,W
	  BTFSS STATUS,C
	  GOTO MAT_END ;U_SEND 
SEND_255:
	  CLRF HEART_C
      BSF FG,F_POW
      GOTO MAT_END ;U_SEND
ERR_DDCL: ;抖动判断处理
      CLRF WD_COUNT
      MOVFW ERR_BH
	  XORLW ERR_DD
	  BTFSS STATUS,Z
	  GOTO ZC_DATA

;      BTFSS COUNT_1S,0
;      GOTO MAT_END
      MOVLW 4
	  SUBWF COUNT_5S,W
	  BTFSS STATUS,C
	  GOTO MAT_END
	  CLRF COUNT_5S


	  INCF DD_COUNT,F
      MOVLW 0 ;2 ;4
	  SUBWF DD_COUNT,W
	  BTFSS STATUS,C
      GOTO MAT_END ;ZC_DATA1 
;抖动处理，默认向上+2，-1 数据不大于121
      BTFSS DD_COUNT,0
	  GOTO $+4 
	  INCF H_NUMBER,F
	  INCF H_NUMBER,F
	  INCF H_NUMBER,F
	  DECF H_NUMBER,F
      MOVLW 122
      SUBWF H_NUMBER,W
	  BTFSS STATUS,C
      GOTO MAT_END 
      MOVLW 3
	  SUBWF H_NUMBER,F

      GOTO MAT_END 
ZC_DATA:
      CLRF DD_COUNT
ZC_DATA1:
      MOVLW 40
      SUBWF H_NUMBER,W ;MOVFW H_NUMBER
	  BTFSC STATUS,C
	  GOTO $+4
	  MOVFW HEART_C
	  MOVWF H_NUMBER
	   GOTO MAT_END  ;RETURN ;当前显示0返回

	  MOVFW H_NUMBER
	  SUBWF HEART_C,W
	  BTFSC STATUS,Z
	  GOTO MAT_END ;RETURN
      BTFSS STATUS,C
	  GOTO MAT_S
MAT_B: ;当前值大于显示值+1
      ;MOVWF TMP
	  ;MOVLW 3
	  ;SUBWF TMP,W
	  ;BTFSS STATUS,C
	  ;GOTO $+3
	  ;BTFSC FG,F_QS 
      ;INCF H_NUMBER,F
      INCF H_NUMBER,F
      GOTO MAT_END
     
MAT_S:	  
      DECF H_NUMBER,F
      MOVFW HEART_C
	  SUBWF H_NUMBER,W
	  SUBLW 4
      BTFSC STATUS,C
      GOTO MAT_END
      BTFSS FG,F_QS
      DECF H_NUMBER,F
      DECF H_NUMBER,F
MAT_END:
      MOVFW HEART_C
	  XORLW 0
	  BTFSC STATUS,Z
	   CLRF H_NUMBER
      MOVFW H_NUMBER
	  MOVWF SEND_NUMBER

	
U_SEND:
      MOVFW SEND_NUMBER
	  XORLW 0
	  BTFSC STATUS,Z
	  DECF SEND_NUMBER,F
      MOVLW 255
	  XORWF SEND_NUMBER,W
      BTFSC STATUS,Z
	  GOTO U_SEND1
      MOVLW 161
	  SUBWF SEND_NUMBER,W
      BTFSC STATUS,C
	  GOTO ERR_GO  ;ERR_RET ;POW_ON
	  MOVLW 40
	  SUBWF SEND_NUMBER,W
	  BTFSS STATUS,C
	  GOTO ERR_GO ;ERR_RET ;POW_ON

U_SEND1:
     BCF FG,F_SERR
     MOVFW SEND_NUMBER
	 MOVWF RESULT_BAK
U_SEND2:
	  BTFSS FLAG,F_SEL
	  GOTO  UART_SEND
	;  BTFSC FG,F_SERR
	  GOTO MATCH_R ;RETURN
ERR_GO:
      BSF FG,F_SERR
	 MOVFW RESULT_BAK
     MOVWF SEND_NUMBER
     GOTO U_SEND2
  END
