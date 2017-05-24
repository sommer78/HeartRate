#ifndef DISTANCE_STEPLENGTH_H_
#define DISTANCE_STEPLENGTH_H_

#define ZOOMSIZE 1000

//各特征值的系数及常数项
#define K_INTERCEPT 487176
#define K_DIFF_MIN_MAX -52
#define K_STEP_HZ 23
#define K_SEX -35201
#define K_DT -23
#define K_AMEAN_DT -19
#define K_AMEAN_DT_DT 31
#define K_DIFFMINMAX_AMEAN 24
#define K_X_Y_Z 107

//对各特征值进行规划，对应的训练集的平均值
#define PLAN_MEAN_DIFF_MIN_MAX 1313
#define PLAN_MEAN_STEP_HZ 2120
#define PLAN_MEAN_DT 522
#define PLAN_MEAN_AMEAN_DT 707
#define PLAN_MEAN_AMEAN_DT_DT 384
#define PLAN_MEAN_DIFFMINMAX_AMEAN 943
#define PLAN_MEAN_X_Y_Z 948

 //对各特征值进行规划，对应的训练集的平均值
#define PLAN_SD_DIFF_MIN_MAX 699
#define PLAN_SD_STEP_HZ 478
#define PLAN_SD_DT 139
#define PLAN_SD_AMEAN_DT 203
#define PLAN_SD_AMEAN_DT_DT 215
#define PLAN_SD_DIFFMINMAX_AMEAN 425
#define PLAN_SD_X_Y_Z 741

float get_distance_data(int extra_steps, int nstep);

#endif /* DISTANCE_STEPLENGTH_H_ */ 
