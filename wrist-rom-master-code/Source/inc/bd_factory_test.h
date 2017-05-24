#ifndef BD_FACTORY_TEST__
#define BD_FACTORY_TEST__

#include "board_config_pins.h"
#include <stdint.h>

#define LED_SENSOR_TEST_RESULT BAIDU_LED_3

#define GRAVITY_VALUE (8192)
#define MIN_SENSOR_VALUE ((GRAVITY_VALUE - 819)*(GRAVITY_VALUE - 819))
#define MAX_SENSOR_VALUE ((GRAVITY_VALUE + 819)*(GRAVITY_VALUE + 819))


#define FACTORY_TEST_FLAG_OFF (8)
#define SERIAL_NUMBER_LENGTH (32)


void bootup_check(void);
void do_test(uint8_t *data, uint16_t length);
uint8_t is_factory_test_done(void);

#endif
