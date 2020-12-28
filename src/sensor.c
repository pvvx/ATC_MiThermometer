#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"
#include "stack/ble/ll/ll_pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"

#define SHTC3_WAKEUP		0x1735 // Wake-up command of the sensor
#define SHTC3_SOFT_RESET	0x5d80 // Soft reset command
#define SHTC3_GO_SLEEP		0x98b0 // Sleep command of the sensor
#define SHTC3_MEASURE		0x6678 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First

RAM uint32_t timer_measure_cb;

_attribute_ram_code_ void send_sensor(uint16_t cmd) {
	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = 0xE0;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

void init_sensor(){
	send_sensor(SHTC3_WAKEUP); //	Wake-up command of the sensor
	sleep_us(240);
	send_sensor(SHTC3_SOFT_RESET); // Soft reset command
	sleep_us(240);
	send_sensor(SHTC3_GO_SLEEP); // Sleep command of the sensor
}

_attribute_ram_code_ void read_sensor_start(void) {
	send_sensor(SHTC3_WAKEUP); //	Wake-up command of the sensor
	sleep_us(240);
	reg_i2c_id = 0xE0;
	reg_i2c_adr_dat = SHTC3_MEASURE; // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

_attribute_ram_code_ void read_sensor_cb(void) {
	int16_t _temp;
	uint16_t _humi;
	init_i2c();

	reg_i2c_id = 0xE0 | FLD_I2C_WRITE_READ_BIT;
	int i = 512;
	do {
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_START;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	} while((reg_i2c_status & FLD_I2C_NAK) && i--);

	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	_temp = reg_i2c_di << 8;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	_temp |= reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	measured_data.temp = ((int32_t)(17500*_temp) >> 16) - 4500 + cfg.temp_offset * 10; // x 0.01 C
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	(void)reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	_humi = reg_i2c_di << 8;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	_humi |= reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_STOP;
	measured_data.humi = ((uint32_t)(10000*_humi) >> 16) + cfg.humi_offset * 10; // x 0.01 %
	if(measured_data.humi > 9999) measured_data.humi = 9999;
	else if(measured_data.humi < 0) measured_data.humi = 0;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	measured_data.count++;
	send_sensor(SHTC3_GO_SLEEP); // Sleep command of the sensor
}

_attribute_ram_code_ void read_sensor_deep_sleep(void) {
	read_sensor_start();
	gpio_setup_up_down_resistor(GPIO_PC2, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(GPIO_PC3, PM_PIN_PULLUP_1M);
	timer_measure_cb = (clock_time() + 11 * CLOCK_16M_SYS_TIMER_CLK_1MS);
}

_attribute_ram_code_ void read_sensor_sleep(void) {
	read_sensor_start();
	StallWaitMs(11);
	read_sensor_cb();
}

