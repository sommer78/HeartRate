
HEXTODEC:   ;输入X，输出：十位=DB1  个位=DB0   
    MOVWF HL
    CLRF  HM
	CLRF  HH
HTD_LOOP:
    MOVLW 10
    SUBWF HL,W
    BTFSS STATUS,C
    GOTO HTD_LOOP1 
    MOVWF HL
    INCF HM,F
    GOTO HTD_LOOP
HTD_LOOP1:
    MOVLW 10
    SUBWF HM,W
    BTFSS STATUS,C
    RETURN 
    MOVWF HM
    INCF HH,F
    GOTO HTD_LOOP1
    RETURN


DISPLAY:
     BSF PT1,LED_L1 
     BSF PT1,LED_L2 
     BSF PT1,LED_L3 

     BCF PT2,LED_A
     BCF PT2,LED_B
     BCF PT2,LED_C
     BCF PT2,LED_D
     BCF PT1,LED_E
     BCF PT2,LED_F
     BCF PT2,LED_G

     MOVFW H_NUMBER
      CALL HEXTODEC

DSP_G:
COM_SET:
                INCF BYTE_JS,F
                MOVLW 3
                SUBWF BYTE_JS,W
                BTFSC STATUS,C
                CLRF BYTE_JS        
     MOVFW BYTE_JS
     XORLW 0
     BTFSC STATUS,Z
     GOTO DSP_C1
     MOVFW BYTE_JS
     XORLW 1
     BTFSC STATUS,Z
     GOTO DSP_C2
     GOTO DSP_C3

DSP_C1:
 		  BCF PT1, LED_L3
        MOVFW HL
		 GOTO SEG
DSP_C2:
 		  BCF PT1, LED_L2
        MOVFW HM
		 GOTO SEG
DSP_C3:
 		  BCF PT1, LED_L1
        MOVFW HH
		 GOTO SEG

SEG:
      CALL LEDYM_TAB
SEG1:
     MOVWF HL

SEG_SET:
SEG_A:
      BTFSC HL,0
      BSF PT2,LED_A
SEG_B:
      BTFSC HL,1
      BSF PT2,LED_B
SEG_C:
      BTFSC HL,2
      BSF PT2,LED_C
SEG_D:
      BTFSC HL,3
      BSF PT2,LED_D
SEG_E:
      BTFSC HL,4
      BSF PT1,LED_E
SEG_F:
      BTFSC HL,5
      BSF PT2,LED_F
SEG_G:
      BTFSC HL,6
      BSF PT2,LED_G
SEG_END:
  
DSP_END:
                RETURN
LEDYM_TAB: ;7-DOT 6-G 5-F 4-E 3-D 2-C 1-B 0-A
   ADDPCW
   RETLW  03FH       ;"0"  0
   RETLW  006H       ;"1"  1
   RETLW  05BH       ;"2"  2 
   RETLW  04FH       ;"3"  3
   RETLW  066H       ;"4"  4
   RETLW  06DH       ;"5"  5
   RETLW  07DH       ;"6"  6
   RETLW  007H       ;"7"  7
   RETLW  07FH       ;"8"  8
   RETLW  06FH       ;"9"  9
   RETLW  077H       ;"A"  10
   RETLW  07CH       ;"B"  11
   RETLW  039H       ;"C"  12
   RETLW  05EH       ;"D"  13
   RETLW  079H       ;"E"  14
   RETLW  071H       ;"F"  15
   RETLW  076H       ;"H"  16
   RETLW  038H       ;"L"  17
   RETLW  037H       ;"n"  18 
   RETLW  03EH       ;"u"   19
   RETLW  073H       ;"P"   20
   RETLW  05CH       ;"o"   21
   RETLW  040H       ;"-"   22
   RETLW  000H       ;"熄灭"  23
   RETLW  050H       ;"r"   24
   RETLW  078H       ;"t"   25


   RETLW  000H       ;"自定义"

;---------------------------------------
DLY_SM:
        NOP                ;扫描按键
        NOP                ;P20=公共线 P21 PP22 P23 P24=SW
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
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
		RETURN

