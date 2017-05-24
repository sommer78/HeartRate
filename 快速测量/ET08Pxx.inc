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
   