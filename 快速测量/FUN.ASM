
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
