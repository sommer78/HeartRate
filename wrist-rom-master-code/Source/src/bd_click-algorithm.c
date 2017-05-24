//
//  click-algorithm.c
//  baiduhealth
//
//  Created by zhangdongsheng on 13-12-21.
//  Copyright (c) 2013年 Baidu. All rights reserved.
//

#include <stdio.h>

#if defined(DEBUG_LOG) || defined (DEBUG_ACC)
#include "simple_uart.h"
#else
#define LOG(...)    do {} while(0) 
#endif


#ifdef USE_SOFT_CLICK_ALGORITHM

#define CLICK_BUFFER_SIZE  1

#define CLICK_HIGHT_THRESHOLD 1000 //in mg

#define CLICK_HIGHT_CLOSE_THRESHOLD  700 //in mg

#define CLICK_SHAKE_THRESHOLD  6500 //in mg

#define END_SHAKE_WINDOW_TIME  300  //in ms

#define LONG_TIME_AGO  (-10000)  //in ms

#define ACC_LOW_INVALID -20000 //in mg

#define ACC_HIGHT_INVALID 20000 //in mg

#define NO_CLICK_JUDGE_BY_X_Y_PECENT_X_Y  4

#define NO_CLICK_JUDGE_BY_X_Y_PECENT_Z  6

#define NO_CLICK_JUDGE_BY_X_Y_THRE  500 //in mg

#define NO_CLICK_JUDGE_BY_X_Y_WINDOW      100  //in ms

#define END_CLICK_JUDGE_BY_X_Y_WINDOW_TIME  100  //in ms


#define CLOCK_MAX_THRE_REACH_MIN_THRE_MIN_WINDOW_SIZE    31 //in ms

#define CLOCK_MAX_THRE_REACH_MIN_THRE_MAX_WINDOW_SIZE    51 //in ms

#define CLOCK_CONTINUE_MAX_WINDOW_TIME    500 //in ms

#define CLOCK_CONTINUE_MIN_WINDOW_SIZE    80 //in ms

#define THREAD_HOLD_REACH_BY_INC_MAX_TIME  31 //in ms

#define INC_COUNT_BEGIN_LOW  600 //in mg

#define REACH_THRE_TOO_SLOW_WINDOW    120 //in ms

#define CLOSE_TO_LOW_HIGH_VALUE 6000 //in mg

#define CLOSE_TO_LOW_EXTRA_TIME 20 //in mg

static char s_reach_close_to_low_hight = 0;

static char s_have_init = 0;

static unsigned char s_raw_data_index = 0;
static signed short s_raw_data_z[CLICK_BUFFER_SIZE];
static signed short s_raw_data_x[CLICK_BUFFER_SIZE];
static signed short s_raw_data_y[CLICK_BUFFER_SIZE];

static  long long  s_raw_data_t[CLICK_BUFFER_SIZE];

static  long long reach_shake_time = LONG_TIME_AGO;

static  long long s_prev_click_time = LONG_TIME_AGO;

static  long long s_reach_threshold_time = LONG_TIME_AGO;

static  long long reach_x_y_thre_time = LONG_TIME_AGO;

static unsigned char s_continue_inc_time = 0;

static long long  s_threshold_reach_too_slow_time = LONG_TIME_AGO;

static signed short s_prev_data = 0;

static unsigned char reach_threshold_tag = 0; //0 not reach, 1 reach low, 2 reach hight

static unsigned char s_click_count = 0;

static signed short s_x_axis_max = ACC_LOW_INVALID;

static signed short s_x_axis_min = ACC_HIGHT_INVALID;

static  long long s_x_axis_max_time = LONG_TIME_AGO;

static signed short s_y_axis_max = ACC_LOW_INVALID;

static signed short s_y_axis_min = ACC_HIGHT_INVALID;

static  long long s_y_axis_max_time = LONG_TIME_AGO;


static unsigned char s_one_sample_time = 0;


static char s_have_send_click = 0;

static char *s_click_algorithm_error_desc = "no error";

extern void click_event_handler(unsigned char click_count);

void check_continue_click_end(long long now)
{
    if(now - s_prev_click_time >= CLOCK_CONTINUE_MAX_WINDOW_TIME) {

        if(0 == s_have_send_click && s_click_count > 1) {
            LOG(LEVEL_INFO,"check_continue_click_end s_prev_click_time:%lld now:%lld", s_prev_click_time,now);
            LOG(LEVEL_INFO,"s_click_count: %d", s_click_count);
            click_event_handler(s_click_count);
        }

        s_have_send_click = 1;

        s_click_count = 0;
    }
}


int click_algorithm_accelerate_data_in(short int x, short int y, short int z,  long long timestamp)
{

    int i = 0;
#if defined(DEBUG_LOG)
    int tmp_time;
#endif
    s_raw_data_x[s_raw_data_index] = x;

    s_raw_data_y[s_raw_data_index] = y;

    if(z > 1000) {
        s_raw_data_z[s_raw_data_index] = z - 1000;
    } else {
        s_raw_data_z[s_raw_data_index] = 0;
    }
    
    s_raw_data_t[s_raw_data_index] = timestamp;

    s_raw_data_index = (s_raw_data_index + 1) % CLICK_BUFFER_SIZE;
    if (0 == s_raw_data_index) {
        for(i=0; i<CLICK_BUFFER_SIZE; i++) {

             //LOG(LEVEL_INFO,"compose:%d timestamp:%lld", s_raw_data_z[i],s_raw_data_t[i] );

            if(s_raw_data_x[i] > s_x_axis_max) {
                s_x_axis_max = s_raw_data_x[i];
                s_x_axis_max_time = s_raw_data_t[i];
            }

            if(s_raw_data_x[i] < s_x_axis_min) {
                s_x_axis_min = s_raw_data_x[i];
            }

            if(s_raw_data_y[i] > s_y_axis_max) {
                s_y_axis_max = s_raw_data_y[i];
                 s_y_axis_max_time = s_raw_data_t[i];
            }

            if(s_raw_data_y[i] < s_y_axis_min) {
                s_y_axis_min= s_raw_data_y[i];
            }

            //LOG(LEVEL_INFO,"s_prev_data:%d  s_raw_data_z[i]:%d",s_prev_data,s_raw_data_z[i]);

            if(s_raw_data_z[i] > INC_COUNT_BEGIN_LOW && s_prev_data < s_raw_data_z[i]) {
                s_continue_inc_time += s_one_sample_time;
                
            } else {
                //LOG(LEVEL_INFO,"s_continue_inc_time:%d",s_continue_inc_time);
                if(s_continue_inc_time > THREAD_HOLD_REACH_BY_INC_MAX_TIME) {
                    s_threshold_reach_too_slow_time = s_raw_data_t[i] - s_one_sample_time;
                     LOG(LEVEL_INFO,"s_threshold_reach_too_slow_time:%d",s_threshold_reach_too_slow_time);
                }

                  //if we reach the x y diff threshold, we should reset the status and judge click after end of x y threshold window
                if(((s_x_axis_max - s_x_axis_min)> NO_CLICK_JUDGE_BY_X_Y_THRE && (s_x_axis_max - s_x_axis_min)*NO_CLICK_JUDGE_BY_X_Y_PECENT_X_Y> s_prev_data*NO_CLICK_JUDGE_BY_X_Y_PECENT_Z
                    &&  s_raw_data_t[i] - s_one_sample_time - s_x_axis_max_time < NO_CLICK_JUDGE_BY_X_Y_WINDOW)
                   ||( (s_y_axis_max - s_y_axis_min) > NO_CLICK_JUDGE_BY_X_Y_THRE &&  (s_y_axis_max - s_y_axis_min)*NO_CLICK_JUDGE_BY_X_Y_PECENT_X_Y  > s_prev_data*NO_CLICK_JUDGE_BY_X_Y_PECENT_Z
                   &&  s_raw_data_t[i] - s_one_sample_time - s_y_axis_max_time < NO_CLICK_JUDGE_BY_X_Y_WINDOW))  {

                   LOG(LEVEL_INFO,"reach NO_CLICK_JUDGE_BY_X_Y_THRE");
                   LOG(LEVEL_INFO,"x:%d  z:%d",s_x_axis_max -  s_x_axis_min,s_prev_data );
                   LOG(LEVEL_INFO,"y:%d  z:%d",s_y_axis_max -s_y_axis_min,s_prev_data );
                   LOG(LEVEL_INFO,"x time:%d  y time:%d",s_x_axis_max_time ,s_y_axis_max_time );
                

                    s_reach_threshold_time = LONG_TIME_AGO;
                    reach_threshold_tag = 0;

                    s_reach_close_to_low_hight = 0;

                    s_click_count = 0;

                    s_have_send_click = 1;

                    s_prev_click_time = LONG_TIME_AGO;

                    s_prev_data = s_raw_data_z[i];

                    reach_x_y_thre_time = s_raw_data_t[i];

                    s_x_axis_max = ACC_LOW_INVALID;
                     s_x_axis_min = ACC_HIGHT_INVALID;
                    s_x_axis_max_time = LONG_TIME_AGO;
                    s_y_axis_max = ACC_LOW_INVALID;
                    s_y_axis_min = ACC_HIGHT_INVALID;
                    s_y_axis_max_time = LONG_TIME_AGO;


                    continue;

                } else {
                 
                     s_x_axis_max = ACC_LOW_INVALID;
                     s_x_axis_min = ACC_HIGHT_INVALID;
                    s_x_axis_max_time = LONG_TIME_AGO;
                    s_y_axis_max = ACC_LOW_INVALID;
                    s_y_axis_min = ACC_HIGHT_INVALID;
                    s_y_axis_max_time = LONG_TIME_AGO;

                }

#if defined(DEBUG_LOG)
                tmp_time = s_continue_inc_time;
#endif
                s_continue_inc_time = 0;


                //LOG(LEVEL_INFO,"s_prev_data:%d", s_prev_data);


                 //if the value big than shake threshold,we don't thick user click the deivice, so we need to reset the state
                if(s_prev_data > CLICK_SHAKE_THRESHOLD) {

                    LOG(LEVEL_INFO,"value reach CLICK_SHAKE_THRESHOLD s_raw_data_z[i]:%d",s_prev_data);

                    s_reach_threshold_time = LONG_TIME_AGO;
                    reach_threshold_tag = 0;

                    reach_shake_time = s_raw_data_t[i] - s_one_sample_time;

                    s_reach_close_to_low_hight = 0;

                    s_click_count = 0;

                    s_have_send_click = 1;

                    s_prev_click_time = LONG_TIME_AGO;

                    s_prev_data = s_raw_data_z[i];


                    continue;
                }

                 //if not reach the  threshold, just continue
                if(reach_threshold_tag == 0 && s_prev_data < CLICK_HIGHT_THRESHOLD ) {
                    // LOG(LEVEL_INFO,"value not reach threshold");
                    //TODO: check if need to reset s_click_count
                    check_continue_click_end(s_raw_data_t[i]);

                    s_prev_data = s_raw_data_z[i];
                    
                    continue;
                }

                //if we reach the  threshold, but is to close too pre click,we think this is vibrate
                if(reach_threshold_tag  == 0 && s_prev_data > CLICK_HIGHT_THRESHOLD
                   && s_raw_data_t[i] - s_prev_click_time - s_one_sample_time< CLOCK_CONTINUE_MIN_WINDOW_SIZE) {
                     LOG(LEVEL_INFO,"value  reach threshold but too close s_prev_click_time:%lld",s_prev_click_time);

                    //s_click_count = 0;
                    //s_prev_click_time = LONG_TIME_AGO;

                    s_prev_data = s_raw_data_z[i];

                    continue;
                }
                  
                //we reach the click threshold, but is too close to prev judge by x y time ,we should end of judge x y time
                if(s_raw_data_t[i] - reach_x_y_thre_time -  s_one_sample_time < END_CLICK_JUDGE_BY_X_Y_WINDOW_TIME) {
                    LOG(LEVEL_INFO,"two close to reach_x_y_thre_time");

                    s_prev_data = s_raw_data_z[i];

                    continue;
                }


                //we reach the click threshold, but is too close to pre shake time ,we think is sub wave og shake, just continue
                if(s_raw_data_t[i] - reach_shake_time  - s_one_sample_time< END_SHAKE_WINDOW_TIME) {
                    LOG(LEVEL_INFO,"two close to reach_shake_time");
                    s_prev_data = s_raw_data_z[i];

                    continue;
                }


                //we reach the click  threshold
                if(reach_threshold_tag == 0 
                    && s_prev_data > CLICK_HIGHT_THRESHOLD ) {

                    s_reach_threshold_time = s_raw_data_t[i] - s_one_sample_time;

                    if(s_prev_data > CLOSE_TO_LOW_HIGH_VALUE) {
                        LOG(LEVEL_INFO,"CLOSE_TO_LOW_HIGH_VALUE:%d", s_prev_data);
                        s_reach_close_to_low_hight = 1;
                    }

                     LOG(LEVEL_INFO,"s_reach_threshold_time:%lld", s_reach_threshold_time);
                     LOG(LEVEL_INFO,"s_prev_data:%d", s_prev_data);

                     reach_threshold_tag = 2;

                    s_prev_data = s_raw_data_z[i];

                   LOG(LEVEL_INFO,"inc time:%d",tmp_time);
                    
                    continue;
                }

            }


            s_prev_data = s_raw_data_z[i];

            if(reach_threshold_tag != 0) {

                //if reach low threshold too far away from reach max threshold time,we dont think this is click,and reset the s_reach_threshold_time
                if(s_raw_data_t[i] - s_reach_threshold_time > (CLOCK_MAX_THRE_REACH_MIN_THRE_MAX_WINDOW_SIZE + ((s_reach_close_to_low_hight == 0)? 0: CLOSE_TO_LOW_EXTRA_TIME))) {

                    s_reach_threshold_time = LONG_TIME_AGO;
                    reach_threshold_tag = 0;

                    //s_click_count = 0;

                    s_reach_close_to_low_hight = 0;

                     LOG(LEVEL_INFO,"reach low threshold too far away from reach max threshold time ");
                     LOG(LEVEL_INFO," s_reach_threshold_time:%lld now:%lld", s_reach_threshold_time,s_raw_data_t[i]);

                    //TODO: check if need to reset s_click_count
                    check_continue_click_end(s_raw_data_t[i]);

                    continue;
                }

                //if reach low threshold
                if(reach_threshold_tag == 2 && s_raw_data_z[i] <  CLICK_HIGHT_CLOSE_THRESHOLD ) {

                    //if reach low threshold too close to reach max threshold time,we just continue,wait for next reach low threshold
                    if(s_raw_data_t[i] - s_reach_threshold_time < CLOCK_MAX_THRE_REACH_MIN_THRE_MIN_WINDOW_SIZE) {
                        //TODO: check if need to reset s_click_count
                        check_continue_click_end(s_raw_data_t[i]);

                        LOG(LEVEL_INFO,"reach low threshold too close to reach max threshold time ");
                         LOG(LEVEL_INFO," s_reach_threshold_time:%lld now:%lld", s_reach_threshold_time,s_raw_data_t[i]);


                        continue;
                    }

                    if(s_raw_data_t[i] - s_threshold_reach_too_slow_time > REACH_THRE_TOO_SLOW_WINDOW) {
                        //we got one click here                      
                        s_click_count ++;
                        s_prev_click_time = s_raw_data_t[i];

                       LOG(LEVEL_INFO,"get click s_reach_threshold_time:%lld now:%lld", s_reach_threshold_time,s_raw_data_t[i]);

                       LOG(LEVEL_INFO,"s_click_count:%d", s_click_count);
                    }

                                    
                    
                    s_reach_threshold_time = LONG_TIME_AGO;
                    reach_threshold_tag = 0;

                    s_reach_close_to_low_hight = 0;

                    s_have_send_click = 0;

                }

            }
        }
    }
    return 0;
}

int init_click_algorithm(int sample_rate)
{

    if(0 != s_have_init) {
        s_click_algorithm_error_desc = "click algorithm aready been init";
        return -1;
    }

    s_one_sample_time = (1000/sample_rate);

    s_raw_data_index = 0;

    s_reach_threshold_time = LONG_TIME_AGO;

    reach_threshold_tag = 0;

    s_prev_click_time = LONG_TIME_AGO;

    reach_shake_time = LONG_TIME_AGO;

    s_click_count = 0;

    s_have_send_click = 0;

    s_continue_inc_time = 0;

    s_prev_data = 0;

    s_continue_inc_time = 0;

    s_reach_close_to_low_hight = 0;


    reach_x_y_thre_time = LONG_TIME_AGO;

    s_x_axis_max = ACC_LOW_INVALID;
    s_x_axis_min = ACC_HIGHT_INVALID;
    s_x_axis_max_time = LONG_TIME_AGO;
    s_y_axis_max = ACC_LOW_INVALID;
    s_y_axis_min = ACC_HIGHT_INVALID;
    s_y_axis_max_time = LONG_TIME_AGO;

    s_have_init = 1;

    return 0;
}

int click_algorithm_finalize()
{
    if(0 == s_have_init) {
        s_click_algorithm_error_desc = "click algorithm have not init";
        return -1;
    }

    s_have_init =0;
    return 0;
}

char* click_algorithm_error_desc()
{
    return s_click_algorithm_error_desc;
}
#endif
