 #ifndef _HEALTH_ALGORITHM_INNER_H_
 #define _HEALTH_ALGORITHM_INNER_H_

#include "health-algorithm.h"

typedef struct health_algorithm_implement {
    int (*init_algorithm)(int sample_rate, float hight_cm, float weight_kg, float age, char isfemale);
    int (*algorithm_finalize)(void);
    int (*accelerate_data_in)(short int x,short int y,short int z, int compose,long long timestamp);
    char *(*get_error_desc)();
} health_algorithm_implement_t;

typedef health_algorithm_implement_t* health_algorithm_implement_p;

int init_sleep_algorithm(int sample_rate, float hight_cm, float weight_kg, float age, char isfemale);
int sleep_detect_finalize(void);
int sleep_algorithm_accelerate_data_in(short int x,short int y,short int z, int compose,long long timestamp);
char * sleep_detect_error_desc(void);

int init_distance_algorithm(int sample_rate, float hight_cm, float weight_kg, float age, char isfemale);
int distance_algorithm_finalize(void);
int distance_algorithm_accelerate_data_in(short int x,short int y,short int z, int compose,long long timestamp);
char * distance_algorithm_error_desc(void);

int generate_event(algorithm_event_t *event);

#endif
