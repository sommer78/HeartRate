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
