/**
 * @file distance-calculate.h
 * @brief common variables used in distance algorithm
 * @author Chong Yan <yanchong01@baidu.com>
 * @version 1.0
 * @date 2013-09-18 13:15:59
 */

#ifndef DISTANCE_CALCULATE_H_
#define DISTANCE_CALCULATE_H_

#define PROCESS_BUF_SIZE 30
#define PEAK_SAVE_MAX 15
#define SMOOTH_WIDTH 3
#define HALF_SMOOTH_WIDTH 2
#define GROUP_SIZE 5
#define HALF_GROUP_SIZE 3
#define SLOPE_THRESHOLD 0
#define PEAK_VALUE_THRESHOLD 1150

extern int g_peak_save_index;
extern unsigned char g_peak_save[PEAK_SAVE_MAX];
extern char g_step_save[PEAK_SAVE_MAX];
extern short g_step_freq_save[PEAK_SAVE_MAX];
extern unsigned char g_raw_data_index;
extern short g_raw_data[PROCESS_BUF_SIZE];

extern short g_x_data[PROCESS_BUF_SIZE];
extern short g_y_data[PROCESS_BUF_SIZE];
extern short g_z_data[PROCESS_BUF_SIZE];

extern long long g_raw_data_x[PROCESS_BUF_SIZE];

extern float g_hight_cm;
extern float g_weight_kg;
extern float g_age;
extern char g_isfemale;

#endif /* DISTANCE_CALCULATE_H_ */
