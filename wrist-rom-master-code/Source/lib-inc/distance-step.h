/**
 * @file distance-step.h
 * @brief
 * @author Chong Yan <yanchong01@baidu.com>
 * @version 1.0
 * @date 2013-08-26 13:27:25
 */
#ifndef DISTANCE_STEP_H_
#define DISTANCE_STEP_H_
#include "health-algorithm-inner.h"

/**
 * @brief step_count_init
 * init params used in peak detect algorithm
 * @author Chong Yan <yanchong01@baidu.com>
 * @version 1.0
 * @date 2013-08-26 13:33:08
 * @version 1.1 change the function parameters
 * @date 2013-10-12 10:24:00
 */
void step_count_init();

/**
 * @brief get_step_data
 * get step data
 * @param extra_steps, used for seven-step detect
 * @param last_step_index, used for step buffer overlap
 * @param mode, used for step_mode
 * @param calory, calory output
 * @author Chong Yan <yanchong01@baidu.com>
 * @version 1.0
 * @date 2013-08-26 13:33:49
 * @return -1: fail, >=0: steps calculated
 * @version 1.1 change the function parameters
 * @date 2013-10-12 10:24:00
 */
int get_step_data(int *extra_steps, int *last_step_index, step_mode_t *mode, float *calory);

#endif /* DISTANCE_STEP_H_ */
