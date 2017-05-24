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

