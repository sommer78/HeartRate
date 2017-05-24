#include "board_config_pins.h"

#ifdef FEATURE_MOTOR_LRA
#include "drv2605.h"
void hal_vibrate_init()
{
    drv260x_init();
}
void hal_vibrate_once(int idx,int wait)
{
    drv260x_vib(idx,wait);
}
#endif //FEATURE_MOTOR_LRA

