#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"

_attribute_ram_code_ void init_i2c(){
	i2c_gpio_set(I2C_GROUP); // I2C_GPIO_GROUP_C0C1, I2C_GPIO_GROUP_C2C3, I2C_GPIO_GROUP_B6D7, I2C_GPIO_GROUP_A3A4
	i2c_master_init(0x78, (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*750000)) );
	reg_i2c_mode &= ~FLD_I2C_HOLD_MASTER; // Disable clock stretching for Sensor
}

_attribute_ram_code_ void send_i2c(uint8_t device_id, uint8_t *buffer, int dataLen){
	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
	i2c_set_id(device_id);
	i2c_write_series(0, 0, (uint8_t*)buffer,dataLen);
}
