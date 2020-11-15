#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "main.h"

const uint8_t sens_wakeup[] = {0x35,0x17};
const uint8_t sens_sleep[] = {0xB0,0x98};
const uint8_t sens_reset[] = {0x80,0x5D};

void init_sensor(){	
	send_i2c(0xE0,sens_wakeup, sizeof(sens_wakeup));
	sleep_us(240);
	send_i2c(0xE0,sens_reset, sizeof(sens_reset));
	sleep_us(240);
	send_i2c(0xE0,sens_sleep, sizeof(sens_sleep));
}

void read_sensor(int16_t *temp, uint16_t *humi){
	send_i2c(0xE0,sens_wakeup, sizeof(sens_wakeup));	
	sleep_us(240);
	uint8_t read_buff[5];
	i2c_set_id(0xE0);
	i2c_read_series(0x7CA2, 2, (uint8_t*)read_buff,  5);
	send_i2c(0xE0,sens_sleep, sizeof(sens_sleep));
	
    *temp = ((1750*(read_buff[0]<<8 | read_buff[1]))>>16)-450;
    *humi =  (100 *(read_buff[3] << 8 | read_buff[4]))>>16;
}