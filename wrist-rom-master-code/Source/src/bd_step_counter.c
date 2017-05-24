#include "bd_switch_sleep.h"

#include "step_counter.h"
#include "ble_flash.h"
#include "nordic_common.h"
#include "nrf_gpio.h"
#include "board_config_pins.h"
#include "bd_lis3dh_driver.h"
#include "hal_acc.h"
#include "bd_spi_master.h"

#include "spi_master_config.h" // This file must be in the application folder
#include "nrf_error.h"
#include <stdlib.h>

#include "twi_master.h"
#include "hal_vibrate.h"
#include "bd_communicate_protocol.h"
#include "bd_wall_clock_timer.h"
#include "bd_sync_data.h"
#include "bd_square_root.h"
#include "bd_level_drive_motor.h"
#include "bd_led_flash.h"
#include "bd_input_event_source.h"
#include "bd_interaction.h"
#include "bd_data_manager.h"
#include "bd_led_flash.h"
#include "bd_interaction.h"
#include "bd_private_bond.h"

#include "simple_uart.h"
#define DEEP_SLEEP_INTERVAL 5
static uint8_t sensor_started_minutes = 0;
static uint32_t acc_index = 0;


extern uint8_t *g_sports_send_buffer;
extern uint8_t *g_sleep_send_buffer;
extern BLUETOOTH_BOND_STATE private_bond_machine;

app_timer_id_t m_sensor_timer_id;
static bool sensor_timer_status = false;

static long long algorithm_timestamp = 0;

static bool algorithm_started = false;

#define FIFOMODE 1

static bool daily_target_achieved = false;
//static uint16_t s_tap_time;

static uint16_t totalSteps = 0;
float totalDistance = 0,totalCalory = 0;
static uint16_t walkingSlow = 0,walkingQuick = 0,running = 0,maxMode = 0;
extern AxesRaw_t accData[32]; // Read acc data from FIFO
extern SleepHead_t *manual_sleep_events_head;
SportsData_U mSportsData;
SportsHead_t mSportHead;
SleepData_U  mSleepData;
SleepHead_t  mSleepHead;

static uint8_t mSleep_mode = NONE_SLEEP;

//static uint32_t *sports_pointer,*sleep_pointer;
#define TAP_INTERVAL          APP_TIMER_TICKS(700, APP_TIMER_PRESCALER) /**< Battery level measurement interval (ticks). */
#define SENSOR_INTERVAL          APP_TIMER_TICKS(310, APP_TIMER_PRESCALER) /**< Battery level measurement interval (ticks). */

bool notify_steps_enable;

static uint8_t m_is_waving = 0;


/**< the setted step task for one day */

/**@brief Start timers.
*/
void sensor_timers_stop(void)
{
    uint32_t   err_code;
    if(sensor_timer_status) {

#if defined(DEBUG_LOG) || defined (DEBUG_ACC)
        LOG(LEVEL_INFO," stop sensor timer \r\n");
#endif

        err_code = app_timer_stop(m_sensor_timer_id);
        APP_ERROR_CHECK(err_code);
        sensor_timer_status = false;
        sensor_started_minutes = 0;
    }
}
void sensor_timers_start(void)
{
    uint32_t               err_code;
    if(!sensor_timer_status) {

#if defined(DEBUG_LOG) || defined (DEBUG_ACC)
        LOG(LEVEL_INFO,">>startSensor:  %d\r\n",mSleep_mode);
#endif

        err_code = app_timer_start(m_sensor_timer_id, SENSOR_INTERVAL, NULL);
        APP_ERROR_CHECK(err_code);

        sensor_timer_status = true;
        sensor_started_minutes = 0;
    }
}
uint8_t sports_mode(void)
{
    uint8_t sportMode;

    maxMode = walkingSlow > walkingQuick?walkingSlow:walkingQuick;//STEP_MODE_WALK_SLOW;
    maxMode = maxMode > running?maxMode:running;

    if(maxMode == walkingSlow) {
        sportMode = STEP_MODE_WALK_SLOW;
    } else if(maxMode == walkingQuick) {
        sportMode = STEP_MODE_WALK_QUICK;
    } else {
        sportMode = STEP_MODE_RUN;
    }
    return sportMode;
}

void handle_distance_event(algorithm_event_t *event)
{
    ASSERT(event->event_common.type == DISTANCE_EVENT);


    if(event->distance.new_steps > 0) {
        uint8_t sportMode;
        if(totalSteps == 0) { //except the first frame
            mSportHead.length ++; // every time total steps change from 0 to non-zero, a new frame data
        }
        if(mSportHead.length == 0) {
            mSportHead.length = 1;
        }

        totalSteps += event->distance.new_steps;
        set_global_step_counts_today(get_global_step_counts_today() + event->distance.new_steps);

#if defined(DEBUG_LOG) || defined (DEBUG_ACC1)

        LOG(LEVEL_INFO,"\r\n-----g:%d:tSteps:%d status:%d\r\n",get_global_step_counts_today(),totalSteps, is_bluetooth_connected());
#endif

        if(event->distance.new_distances >0) {
            totalDistance += event->distance.new_distances;
        }
        if(event->distance.new_calory > 0) {
            totalCalory += event->distance.new_calory;
        }

        switch(event->distance.mode) {
            case STEP_MODE_WALK_SLOW :
                walkingSlow ++;
                break;
            case STEP_MODE_WALK_QUICK:
                walkingQuick ++;
                break;
            case STEP_MODE_RUN:
                running ++;
                break;
            default:
                break;
        }
        sportMode = sports_mode();
        mSportsData.bits.mode = sportMode;
        mSportsData.bits.steps = totalSteps;

        if( get_global_step_counts_today() >= get_daily_target() && !daily_target_achieved) {
            daily_target_achieved = true;
            notification_start(NOTIFICATION_TARGET,0);
        } else if( get_global_step_counts_today() < get_daily_target() ) {
            daily_target_achieved = false;
        }

        send_all_data(false);

    }

}

void handle_sleep_event(algorithm_event_t *event)
{
    ASSERT(event->event_common.type == SLEEP_EVENT);
    
#ifndef USE_SOFT_CLICK_ALGORITHM
    if(event->sleep.mode == DEEP_SLEEP) {
        sensor_timers_stop();
        hal_acc_config_wake_int(true);
    }
    // when exit deep sleep restart timer
    if(mSleep_mode == DEEP_SLEEP && event->sleep.mode!= DEEP_SLEEP) {
        if(!sensor_timer_status){
            sensor_timers_start();
            hal_acc_config_wake_int(false);
        }
    }
#endif
    mSleep_mode = event->sleep.mode;

#ifdef DEBUG_LOG

    UTCTimeStruct * tm = get_wall_clock_time();
    LOG(LEVEL_INFO,"system_clock: [%d/%02d/%02d %02d:%02d:%02d]\r\n",tm->year,tm->month,tm->day,tm->hour,tm->minutes,tm->seconds);
#endif

    sleep_evt_handler(event);
    
    send_all_data(false);

}

#ifdef SPORTS_DATA_MOCK_TEST
void mock_minute_timer_handler(UTCTimeStruct *tm);
UTCTime get_wall_clock_time_counter(void);
#endif

void health_algorithm_cb_implement(algorithm_event_t *event, void* user_data)
{

#ifdef SPORTS_DATA_MOCK_TEST
    static int inner_step_count = 1;
    static UTCTime init_time =0;
    if(init_time == 0) {
        init_time = get_wall_clock_time_counter(); //get current time
    }

    init_time = (((init_time - 1) / 60) + 1) * 60;    //round up to 60

    uint8_t j = 0;

    time_union_t sport_head_time;
    
    sport_head_time.data = 0;
    sport_head_time.time.year = mSportHead.Date.date.year;
    sport_head_time.time.month = mSportHead.Date.date.month;
    sport_head_time.time.day = mSportHead.Date.date.day;

    uint32_t sport_head_seconds = convert_time_to_Second(sport_head_time);

    algorithm_event_t sport_event;
    
    for(j = 0;(j<15) && (inner_step_count < 30000);++j,inner_step_count++) {
       
        sport_event.event_common.type = DISTANCE_EVENT;
        sport_event.distance.new_steps = 1;
        sport_event.distance.new_distances = 0.5;
        sport_event.distance.new_calory = 0.1;
        sport_event.distance.mode = (step_mode_t) (j%3);
        
        handle_distance_event(&sport_event);

        if(inner_step_count) {
            UTCTimeStruct time;
            ConvertToUTCTime(&time,init_time); 

            mock_minute_timer_handler(&time);

            init_time += 60;
            #if defined(DEBUG_LOG)
                LOG(LEVEL_INFO, "inner step_count is %d",inner_step_count);
            #endif
        }
    }

    

#else    

    UNUSED_VARIABLE(user_data);
    if(event->event_common.type == DISTANCE_EVENT) {
        handle_distance_event(event);
    }

#endif

#ifdef SLEEP_DATA_MOCK_TEST    
    static int count = 0; 
    uint8_t i;

    time_union_t sleep_head_time;
    
    sleep_head_time.data = 0;
    sleep_head_time.time.year = mSleepHead.Date.date.year;
    sleep_head_time.time.month= mSleepHead.Date.date.month;
    sleep_head_time.time.day= mSleepHead.Date.date.day;

    uint32_t sleepHead_seconds = convert_time_to_Second(sleep_head_time);

    algorithm_event_t sleep_event;
    for(i=0; (i<10) && (count < 30000) ; i++,count++) {
        sleep_event.event_common.type = SLEEP_EVENT;
        if(i%2 == 0) {
            sleep_event.sleep.mode = DEEP_SLEEP;
        } else { 
            sleep_event.sleep.mode = NONE_SLEEP;
        }

        sleep_event.sleep.starting_time_stamp = sleepHead_seconds + 60*i;
        handle_sleep_event(&sleep_event);
    }
    LOG(LEVEL_INFO,"Mock generate %d events",count);
#else 

    if(event->event_common.type == SLEEP_EVENT) {   //      SLEEP_EVENT
        handle_sleep_event(event);
    }


#endif

}

bool is_algorithm_started(void)
{
    return algorithm_started;
}

void stop_health_algorithm(void)
{
     algorithm_started = false;
     health_algorithm_finalize();
}

uint8_t start_health_algorithm(userprofile_union_t *user_profile)
{
    int err_code= 0;
    float hight_cm = user_profile->bit_field.hight * 1.0;
    float weight_kg = user_profile->bit_field.weight * 1.0;
    float age = user_profile->bit_field.age;
    char isfemale = user_profile->bit_field.gender;

    if(age <= 0) {
        age = DEFAULT_AGE;
    }
    if(weight_kg <= 0) {
        weight_kg = DEFAULT_WEIGHT_CM;
    }
    if(hight_cm <= 0) {
        hight_cm = DEFAULT_HIGHT_CM;
    }
    if(isfemale != GENDER_MALE && isfemale!=GENDER_FEMALE) {
        isfemale = DEFAULT_GENDER;
    }
#ifdef DEBUG_ACC
    LOG(LEVEL_INFO,"f:%d a:%f h%f w%f\r\n",isfemale,age,hight_cm,weight_kg);
#endif
#ifdef DEBUG_LOG

    UTCTimeStruct * tm = get_wall_clock_time();
    LOG(LEVEL_INFO,"new day: %d-%d-%d=%d:%d:%d\r\n",tm->year, tm->month, tm->day, tm->hour, tm->minutes, tm->seconds);
#endif

    if(algorithm_started == true) {
#ifndef USE_SOFT_CLICK_ALGORITHM
        sensor_timers_stop();
#endif
        stop_health_algorithm();
    }

    err_code = init_health_algorithm(25,hight_cm,weight_kg,age,isfemale);
    ASSERT(err_code == 0);

    algorithm_started = true;
    
    err_code = register_health_algorithm_callback(health_algorithm_cb_implement,(void *)NULL);
    ASSERT(err_code == 0);
    hal_acc_enable();


    update_algorithm_timestamp();
    
#ifndef USE_SOFT_CLICK_ALGORITHM
    sensor_timers_start();
#endif

    return NRF_SUCCESS;
}

void restart_health_algorithm(void)
{

#ifdef DEBUG_LOG
    UTCTimeStruct * tm = get_wall_clock_time();
    LOG(LEVEL_INFO,"new day: %d-%d-%d=%d:%d:%d\r\n",tm->year, tm->month, tm->day, tm->hour, tm->minutes, tm->seconds);
#endif

#ifdef FEATURE_MOTOR_LRA

    hal_vibrate_init();
#endif  //FEATURE_MOTOR_LRA

    if(0 == mSportHead.length) {
        reset_sports_head();
        reset_cur_sports_data();
    }
    if(0 == mSleepHead.length) {
        reset_sleep_head();
    }
    if(0 == manual_sleep_events_head->length) {
        reset_sleep_setting_head();
    }

    start_health_algorithm(get_user_profile());

}

uint8_t reset_health_algorithm_data(void)
{
    reset_sports_head();
    reset_sleep_head();
    reset_sleep_setting_head();
    reset_cur_sports_data();

    totalSteps= 0;
    totalDistance=totalCalory=0.0;
    walkingSlow=walkingQuick=running=0;

    set_global_step_counts_today(0);


    reset_sport_data();
    reset_sleep_data();

#ifdef FEATURE_MOTOR_LRA

    hal_vibrate_init();
#endif

    return 1;
}

void LIS3DH_INT1_event_handler(uint8_t pin_no)
{
    u8_t int1res;

    if(mSleep_mode != NONE_SLEEP) {        
        hal_acc_config_wake_int(false);
        sensor_timers_start();
    }

    spi_master_enable(SPI0);
    LIS3DH_GetInt1Src(&int1res);

    LIS3DH_RESET_MEM();
#ifdef DEBUG_LOG1

    char index,value;
    for(index = 0x07; index < 0x3D; index ++) {
        LIS3DH_ReadReg(index, &value);
        LOG(LEVEL_INFO,"config add:0x%x==v:0x%x \n",index,value);
    }
#endif
    spi_master_disable(SPI0);


}

#ifndef USE_SOFT_CLICK_ALGORITHM
app_timer_id_t m_double_tap_timer_id;

static uint8_t doubleTaped_cnt;

#define TAP_BUFF_LEN 52
#define FIRST_TAP_THRESHOLD 15 // 0.46g

signed char  accx[TAP_BUFF_LEN] = {0};
signed char  accy[TAP_BUFF_LEN] = {0};
signed char  accz[TAP_BUFF_LEN] = {0};


#define NO_CLICK_JUDGE_BY_X_Y_PECENT_X_Y  10

#define NO_CLICK_JUDGE_BY_X_Y_PECENT_Z_DOUBLE  8

#define NO_CLICK_JUDGE_BY_X_Y_PECENT_Z_SINGLE  12


#define ACC_LOW_INVALID   (0)//in mg

#define ACC_HIGHT_INVALID (1000) //in mg


static int16_t  s_x_axis_max = ACC_LOW_INVALID;

static int16_t s_x_axis_min = ACC_HIGHT_INVALID;

static int16_t  s_y_axis_max = ACC_LOW_INVALID;

static int16_t  s_y_axis_min = ACC_HIGHT_INVALID;

static int16_t  s_z_axis_max = ACC_LOW_INVALID;

static int16_t  s_z_axis_min = ACC_HIGHT_INVALID;

int8_t max_diff(int8_t array[], uint32_t start, uint32_t end);
bool valid_tap(int8_t array[], uint8_t start, uint8_t end, uint8_t wave_threshold);
int8_t max_diff(int8_t array[], uint32_t start, uint32_t end)
{    
    ASSERT(end >= start);
    int8_t max_diff;
    uint32_t ii = 0;
    int8_t max = array[start%TAP_BUFF_LEN];
    int8_t min = max;
    uint32_t max_index = 0, min_index = 0;
    int32_t count = end - start;
    for(ii = start%TAP_BUFF_LEN; count >= 0; ii ++, count --){
        LOG(LEVEL_INFO,"index: %d -v %d ", ii%TAP_BUFF_LEN, array[ii%TAP_BUFF_LEN]);

        if(max < array[ii%TAP_BUFF_LEN]){
            max = array[ii%TAP_BUFF_LEN];
            max_index  = ii%TAP_BUFF_LEN;
        }
        if(min > array[ii%TAP_BUFF_LEN]){
            min = array[ii%TAP_BUFF_LEN];
            min_index  = ii%TAP_BUFF_LEN;
        }
    }

    LOG(LEVEL_INFO,"index: %d - %d ", max_index, min_index);

    max_diff = max - min;

    LOG(LEVEL_INFO,"%d = %d - %d ", max_diff, max, min);
    return max_diff;
}
bool valid_tap(int8_t array[], uint8_t start, uint8_t end, uint8_t wave_threshold)
{
    ASSERT(end >= start);
    bool valid = false;
    uint8_t ii = 0;
    uint8_t max_a = abs(array[start]);
    uint8_t max_b = abs(array[start + 26]);
    uint8_t index_a = 0, index_b = 26;
    int count = TAP_BUFF_LEN/2;
    for(ii = acc_index%TAP_BUFF_LEN; count > 0; ii ++, count --)
    {       
       LOG(LEVEL_INFO, ";%d ; %d", ii%TAP_BUFF_LEN, abs(array[ii%TAP_BUFF_LEN]));
       if(max_a < abs(array[ii%TAP_BUFF_LEN])){
            max_a = abs(array[ii%TAP_BUFF_LEN]);
            index_a = ii%TAP_BUFF_LEN;
        }
    }
    count = TAP_BUFF_LEN/2;
    for(ii = (acc_index+TAP_BUFF_LEN/2)%TAP_BUFF_LEN; count > 0; ii ++, count --)
    {       
        LOG(LEVEL_INFO, "-;%d ; %d", ii%TAP_BUFF_LEN, abs(array[ii%TAP_BUFF_LEN]));
        if(max_b < abs(array[ii%TAP_BUFF_LEN])){
            max_b = abs(array[ii%TAP_BUFF_LEN]);
            index_b = ii%TAP_BUFF_LEN;
        }
    }

    LOG(LEVEL_INFO, "a: %d, %d||b: %d, %d", index_a, max_a, index_b, max_b);
/*
    if(index_a > index_b){
        index_tmp = index_a;
        index_a = index_b;
        index_b = index_tmp;
    }*/
    if(max_a > TAP_THRESHOLD && max_b > TAP_THRESHOLD ){
        if(max_diff(array, index_a + 11, index_a + 19) < wave_threshold){ // 0.31g
            valid =  true;
        }  
    }

    return valid;

}

uint8_t read_sensor_data() {
    
    uint8_t transfer_size = 0;
    uint8_t acc_ii = 0;
    
    hal_acc_GetFifoData(&transfer_size);

    for(acc_ii = 0; acc_ii < transfer_size; acc_ii ++, acc_index ++) {
        accx[acc_index%TAP_BUFF_LEN] = accData[acc_ii].AXIS_X >> 8; 
        accy[acc_index%TAP_BUFF_LEN] = accData[acc_ii].AXIS_Y >> 8; 
        accz[acc_index%TAP_BUFF_LEN] = accData[acc_ii].AXIS_Z >> 8; 
    }

    return transfer_size;
}


void pass_sensor_data_to_health_algorithm(uint8_t data_size) {

    int i = 0;
    int error_no = 0;
    int16_t accX,accY,accZ;
    int32_t compose;

    if(false == algorithm_started) {
        return;
    }
    
     for(i = 0; i < data_size; i += 4 ) {
        accX = (accData[i].AXIS_X>>3);  // mg
        accY = (accData[i].AXIS_Y>>3);
        accZ = (accData[i].AXIS_Z>>3);
        compose = SquareRoot(accX*accX + accY*accY + accZ*accZ);
#ifdef DEBUG_ACC1
        LOG(LEVEL_INFO,"%d;-%d;%d;%d;%d\r\n",acc_ii,accX,accY,accZ,compose);
#endif

        error_no = health_algorithm_data_in_accelerate(accX,accY,accZ,compose,algorithm_timestamp);
        ASSERT(0 == error_no);
        
        algorithm_timestamp += 40;
    }


}

void tap_detect_schedule(void * p_event_data, uint16_t event_size)
{    
    uint32_t err_code;
    int  i = 0;
    uint16_t z_value;
   
    uint8_t data_size = 0;

    uint8_t delta_x, delta_y, delta_z;

    uint8_t x_pulse_cnt = 0, y_pulse_cnt = 0, z_pulse_cnt = 0;
    
    uint8_t * double_tap_count = (uint8_t * )p_event_data;
    
    if(event_size != sizeof(uint8_t)) {
        return;
    } 

    data_size = read_sensor_data();

    pass_sensor_data_to_health_algorithm(data_size); 
    
    for(i = 0; i < TAP_BUFF_LEN ; i ++) {
        
        if(accx[i] > s_x_axis_max) {
            s_x_axis_max = accx[i];
        }

        if(accx[i] < s_x_axis_min) {
            s_x_axis_min = accx[i];
        }
 

        if(accy[i] > s_y_axis_max) {
            s_y_axis_max = accy[i];
        }

        if(accy[i] < s_y_axis_min) {
            s_y_axis_min= accy[i];
        }

         if(accz[i] > s_z_axis_max) {
            s_z_axis_max = accz[i];
        }

        if(accz[i] < s_z_axis_min) {
            s_z_axis_min= accz[i];
        }

        delta_x = abs( accx[i] - accx[i-1]);
        if(delta_x > 56){ // 1.7g
            x_pulse_cnt ++;
        }
        delta_y = abs( accy[i] - accy[i-1]);
        if(delta_y > 56){ // 1.7g
            y_pulse_cnt ++;
        }
        delta_z = abs( accz[i] - accz[i-1]);
        if(delta_z > 56){ // 1.7g
            z_pulse_cnt ++;
        }

    }

    if(*double_tap_count == 1) {
        z_value =  (s_z_axis_max - s_z_axis_min)*NO_CLICK_JUDGE_BY_X_Y_PECENT_Z_SINGLE;
    } else {
        z_value =  (s_z_axis_max - s_z_axis_min)*NO_CLICK_JUDGE_BY_X_Y_PECENT_Z_DOUBLE;
    }

#ifdef DEBUG_LOG
    LOG(LEVEL_INFO,"x max:%d ,x min : %d \n",s_x_axis_max,s_x_axis_min);
    LOG(LEVEL_INFO,"y max:%d ,y min : %d \n",s_y_axis_max, s_y_axis_min);
    LOG(LEVEL_INFO,"z max:%d ,z min : %d  \n",s_z_axis_max, s_z_axis_min);
 #endif

    if((s_x_axis_max - s_x_axis_min)*NO_CLICK_JUDGE_BY_X_Y_PECENT_X_Y > z_value
        ||(s_y_axis_max - s_y_axis_min)*NO_CLICK_JUDGE_BY_X_Y_PECENT_X_Y > z_value ) {
        doubleTaped_cnt = 0;
    }

    if(*double_tap_count == 1) {
        if(z_pulse_cnt > 1 && z_pulse_cnt < 6 && valid_tap(accz,0,TAP_BUFF_LEN - 1, FIRST_TAP_THRESHOLD)){
            if(!(x_pulse_cnt < z_pulse_cnt && y_pulse_cnt < z_pulse_cnt)) {
                doubleTaped_cnt = 0;
            }
        }
        else{
            doubleTaped_cnt = 0;
        }
    } else {
        if(x_pulse_cnt > z_pulse_cnt || y_pulse_cnt > z_pulse_cnt){
                doubleTaped_cnt = 0;
         }
    }

    if(0 == doubleTaped_cnt) {
#ifdef DEBUG_LOG
        LOG(LEVEL_INFO,"no judge by x y \n");
#endif
    }

    

    if(*double_tap_count == 1 && doubleTaped_cnt != 0) {
#ifdef DEBUG_LOG
    LOG(LEVEL_INFO,"start single tap notification \n");
 #endif
        if(check_has_bonded()){
                notification_start(NOTIFICATION_ANIMATE,1);
        }
        err_code = app_timer_start(m_double_tap_timer_id, TAP_INTERVAL, NULL);
        APP_ERROR_CHECK(err_code);
    }

    s_x_axis_max = ACC_LOW_INVALID;

    s_x_axis_min = ACC_HIGHT_INVALID;

    s_y_axis_max = ACC_LOW_INVALID;

    s_y_axis_min = ACC_HIGHT_INVALID;

    s_z_axis_max = ACC_LOW_INVALID;

    s_z_axis_min = ACC_HIGHT_INVALID;
    
    
}

void LIS3DH_INT2_event_handler(uint8_t pin_no)
{
    u8_t  click_res;
    spi_master_enable(SPI0);
    LIS3DH_GetClickResponse(&click_res);
    spi_master_disable(SPI0);

    switch(click_res) {
        case LIS3DH_DCLICK_Z_N:
        case LIS3DH_DCLICK_Z_P:
            doubleTaped_cnt ++;
#ifdef DEBUG_LOG
                LOG(LEVEL_INFO,"interrupt2 \n");
#endif
            if(!sensor_timer_status){ //  any time click, check sensor timer status
                sensor_timers_start();
                hal_acc_config_wake_int(false);
            }

            if(m_is_waving == 0){ 
                if(doubleTaped_cnt == 1 || doubleTaped_cnt == 2){
                    app_sched_event_put(&doubleTaped_cnt,sizeof(uint8_t),tap_detect_schedule);
                } else {
                    if(doubleTaped_cnt > 5) {
                        doubleTaped_cnt = 0;
                    }
                }
            } else {
#ifdef DEBUG_LOG
                LOG(LEVEL_INFO,"tap by wave \n");
#endif
                doubleTaped_cnt = 0;
             }

            break;
        default:
            break;
    }
}

void tap_timer_handler(void * val)
{
    PressType type;
#ifdef DEBUG_LOG
    LOG(LEVEL_INFO,"tap_timer_handler doubleTaped_cnt: %d \n",doubleTaped_cnt);
#endif

    if(doubleTaped_cnt == 1) {
        type = SHORT_PRESS;
        app_sched_event_put(&type,sizeof(PressType),input_event_handle_schedule);
    } else if(doubleTaped_cnt >= 2 ) { // 2.5g
        type = LONG_PRESS;
        app_sched_event_put(&type,sizeof(PressType),input_event_handle_schedule);
    }
    doubleTaped_cnt = 0;

}

/*bool is_waving(int8_t array[], uint8_t start, uint8_t end, uint8_t threshold)
{
    ASSERT(end >= start);

    uint8_t ii = 0;
    bool l_is_waving = false;
    int8_t max = array[start];
    int8_t min = array[start];
    int16_t sum_z = 0;
    int8_t avg_acc_z;
    int8_t bottom_threshold = 0;
    uint8_t bottom_index = 0;
    for(ii = start; ii < end; ii ++)
    {       
        sum_z += array[ii];
       if(max < array[ii]){
            max = array[ii];
        }
 
        if(min > array[ii]){
            min = array[ii];
            bottom_index = ii;
 
        }
    }
    avg_acc_z = sum_z/(end - start + 1);
    if(avg_acc_z < 0){  // means user may be in bed 
        return false;
    }
    bottom_threshold = avg_acc_z - 11; // 11: 0.3g ~
//    cycle = top_index - bottom_index;
#ifdef DEBUG_ACC1
    LOG(LEVEL_INFO, "%d -- %d|  avg:%d___b_thr: %d", max, min, avg_acc_z, bottom_threshold);
#endif
    if(min <(threshold>2) && max-min > threshold){ // changes bigger than 1g, and bottom less than 0.5g
        if(bottom_index !=start && bottom_index != end){
            if(array[bottom_index - 1] < bottom_threshold && array[bottom_index + 1]< bottom_threshold){
                l_is_waving = true;
            }
        }
        else if(bottom_index == start){
            if(array[bottom_index + 1] < bottom_threshold && array[bottom_index + 2]< bottom_threshold){
                l_is_waving = true;
            }
        }
        else if(bottom_index == end){
            if(array[bottom_index - 1] < bottom_threshold && array[bottom_index - 2]< bottom_threshold){
                l_is_waving = true;
            }
        }

    }
    else {
        l_is_waving = false;
    }
    return l_is_waving;
}
*/



void sensor_timer_handle(void * val)
{

    uint16_t data_size = 0;

    data_size = read_sensor_data();

    pass_sensor_data_to_health_algorithm(data_size); 
/*
    if(is_waving(accz, 0, 25, 32)){
        m_is_waving ++;
    }
    if(is_waving(accz, 25, 50, 32)){
        m_is_waving ++;
    }

 
    if(m_is_waving > 2){
#ifdef DEBUG_ACC
        LOG(LEVEL_INFO,"START~~~~~waving: %d",m_is_waving);
#endif

        m_is_waving = 2;
    }
    if( m_is_waving !=0 ){
        -- m_is_waving;
#ifdef DEBUG_ACC
            if( !m_is_waving ){

                LOG(LEVEL_INFO,"stop____waving: %d",m_is_waving);
            }
#endif
    }
*/
}

#else

static long long click_algorithm_timestamp = 0;

void LIS3DH_INT2_event_handler(uint8_t pin_no)
{
    u8_t  click_res;
    spi_master_enable(SPI0);
    LIS3DH_GetClickResponse(&click_res);
    spi_master_disable(SPI0);

    switch(click_res) {
        case LIS3DH_DCLICK_Z_N:
        case LIS3DH_DCLICK_Z_P:

            break;
        default:
            break;
    }
}


int click_algorithm_accelerate_data_in(short int x, short int y, short int z,  long long timestamp);

void click_event_handler(unsigned char click_count)
{
    PressType type;


    if(click_count >= 2 && click_count <= 3) {
        type = SHORT_PRESS;
        app_sched_event_put(&type,sizeof(PressType),input_event_handle_schedule);
    } else if(click_count >= 4 ) {
        type = LONG_PRESS;
        app_sched_event_put(&type,sizeof(PressType),input_event_handle_schedule);
    }
}

void sensor_timer_handle(void * val)
{
    int error_no = 0;

    uint8_t transfer_size;
    int32_t compose;
    int16_t accX,accY,accZ;
    uint8_t acc_ii = 0;
    hal_acc_GetFifoData(&transfer_size);

    for(; acc_ii < transfer_size; acc_ii ++, acc_index ++) {
        if(acc_ii % 1 == 0) {
            accX = (accData[acc_ii].AXIS_X>>3);  // mg
            accY = (accData[acc_ii].AXIS_Y>>3);
            accZ = (accData[acc_ii].AXIS_Z>>3);
           
            error_no = click_algorithm_accelerate_data_in(accX,accY,accZ,click_algorithm_timestamp);
            ASSERT(0 == error_no);
            click_algorithm_timestamp += 10;

            if(acc_ii % 4 == 0 && true == algorithm_started ){
                 compose = SquareRoot(accX*accX + accY*accY + accZ*accZ);
                 error_no = health_algorithm_data_in_accelerate(accX,accY,accZ,compose,algorithm_timestamp);
                ASSERT(0 == error_no);
                algorithm_timestamp += 40;

             }
        }
    }
    
}

#endif

/*
when started, expire every 60 seconds
*/
void update_algorithm_timestamp(void)
{
    algorithm_timestamp = get_wall_clock_time_counter();
    algorithm_timestamp *= 1000;
    
#ifdef USE_SOFT_CLICK_ALGORITHM
    click_algorithm_timestamp = get_wall_clock_time_counter();
    click_algorithm_timestamp *= 1000;
 #endif
    
}
void save_quarter_distance_data(uint8_t sportMode)
{
        if(totalSteps) {
            mSportsData.bits.Calory = ((uint32_t)(totalCalory*1000) & 0x7ffff);
            mSportsData.bits.Distance = (uint16_t)totalDistance;
            mSportsData.bits.mode = sportMode;
            mSportsData.bits.steps = totalSteps;

            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 0] = mSportsData.data >> 56; //OFFSET
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 1] = mSportsData.data >> 48;
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 2] = mSportsData.data >> 40;
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 3] = mSportsData.data >> 32;
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 4] = mSportsData.data >> 24;
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 5] = mSportsData.data >> 16;
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 6] = mSportsData.data >> 8;
            g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 7] = mSportsData.data;

#ifdef DEBUG_LOG

            LOG(LEVEL_INFO,"mSportsData.mOffset:%x\n",mSportsData.bits.offset);
            LOG(LEVEL_INFO,"mOffsetH:%x\n",g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 0]);
            LOG(LEVEL_INFO,"mOffsetL:%x\n",g_sports_send_buffer[SPORT_HEAD_LEN + ((mSportHead.length-1) <<3) + 1]);
#endif


            if(mSportHead.length == SPORTS_MAX_GROUP_NUM) {
                if(save_sport_group_data(mSportHead.length) == NRF_SUCCESS) {
                    mSportHead.length = 0;
                } else {
                    // TODO:
                }
            }

        }
        // clear status
        mSportsData.bits.offset ++;
        totalSteps= 0;
        totalDistance=totalCalory=0.0;
        walkingSlow=walkingQuick=running=0;
        mSportsData.bits.mode= 0;
        mSportsData.bits.steps= 0;
        mSportsData.bits.Calory = 0;
        mSportsData.bits.Distance = 0;
        mSportsData.bits.active_time = 0;
}

#ifdef SPORTS_DATA_MOCK_TEST

void minute_timer_handler(void *val)
{
    
}

void mock_minute_timer_handler(UTCTimeStruct *tm)
{
    uint8_t sportMode;
    
    sportMode = sports_mode();

    if( tm->hour == 0 && tm->minutes == 0 && tm->seconds ==0 ) {
#ifdef DEBUG_LOG
//        LOG(LEVEL_INFO,"new day: %d-%d-%d=%d:%d:%d\r\n",tm->year, tm->month, tm->day, tm->hour, tm->minutes, tm->seconds);
#endif
        set_global_step_counts_today(0);
        daily_target_achieved = false;
    }

    if(sportMode > walkingSlow)
        mSportsData.bits.active_time ++;

    if(mSleep_mode == DEEP_SLEEP) { // SLEEPING
        if(sensor_timer_status == false) { // sensor timer was stoped to save power
            update_algorithm_timestamp();
            health_algorithm_data_in_accelerate(0,0,0,0,algorithm_timestamp);
        } else { // movements triggered the sensor to work again, if after ten minutes still in deep sleep, stop it
#ifdef DEBUG_ACC
            LOG(3,"mSleep_mode%d,timer%d;/n",mSleep_mode, sensor_timer_status);
#endif
            if(sensor_started_minutes ++ > DEEP_SLEEP_INTERVAL) {
                hal_acc_config_wake_int(true);
                sensor_timers_stop();
            }
        }
    }
    if(tm->minutes%15 == 0) {
        save_quarter_distance_data(sportMode);
    }


}

#else //SPORTS_DATA_MOCK_TEST

void minute_timer_handler(void * val)
{
    uint8_t sportMode;
    UTCTimeStruct * tm = get_wall_clock_time();
    sportMode = sports_mode();


    if( tm->hour == 0 && tm->minutes == 0 && tm->seconds ==0 ) {
#ifdef DEBUG_LOG
//        LOG(LEVEL_INFO,"new day: %d-%d-%d=%d:%d:%d\r\n",tm->year, tm->month, tm->day, tm->hour, tm->minutes, tm->seconds);
#endif

        set_global_step_counts_today(0);
        daily_target_achieved = false;
    }

    if(sportMode > walkingSlow)
        mSportsData.bits.active_time ++;

    if(mSleep_mode == DEEP_SLEEP) { // SLEEPING
        if(sensor_timer_status == false) { // sensor timer was stoped to save power
            update_algorithm_timestamp();
            health_algorithm_data_in_accelerate(0,0,0,0,algorithm_timestamp);
        } else { // movements triggered the sensor to work again, if after ten minutes still in deep sleep, stop it
#ifdef DEBUG_ACC
            LOG(3,"mSleep_mode%d,timer%d;/n",mSleep_mode, sensor_timer_status);
#endif

            if(sensor_started_minutes ++ > DEEP_SLEEP_INTERVAL) {
                hal_acc_config_wake_int(true);
                sensor_timers_stop();
            }
        }
    }
    if(tm->minutes%15 == 0) {
        save_quarter_distance_data(sportMode);
    }


}

#endif //SPORTS_DATA_MOCK_TEST



void set_daily_target_achieved(bool value)
{
    daily_target_achieved = value;
}

bool get_daily_target_achieved(void)
{
    return daily_target_achieved; 
}
