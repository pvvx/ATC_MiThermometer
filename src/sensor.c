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
#define SHTC3_LPMEASURE		0x9C60 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

RAM volatile uint32_t timer_measure_cb;

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

_attribute_ram_code_ void read_sensor_start(uint16_t mcmd) {
	send_sensor(SHTC3_WAKEUP); //	Wake-up command of the sensor
	sleep_us(240);
	reg_i2c_id = 0xE0;
	reg_i2c_adr_dat = mcmd; // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

_attribute_ram_code_ int read_sensor_cb(void) {
	int16_t _temp;
	uint16_t _humi;
	uint8_t data, crc; // calculated checksum
	int i;

	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();

	reg_i2c_id = 0xE0 | FLD_I2C_WRITE_READ_BIT;
	i = 512;
	do {
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_START;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	} while((reg_i2c_status & FLD_I2C_NAK) && i--);
	if (!i) {
		reg_i2c_ctrl = FLD_I2C_CMD_STOP;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
		send_sensor(SHTC3_SOFT_RESET); // Soft reset command
		sleep_us(240);
		send_sensor(SHTC3_GO_SLEEP); // Sleep command of the sensor
		return 0;
	}
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	data = reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	_temp = data << 8;
	crc = data ^ 0xff;
	for(i = 8; i > 0; i--) {
		if(crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	data = reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	_temp |= data;
	crc ^= data;
	for(i = 8; i > 0; i--) {
		if(crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	data = reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	_humi = reg_i2c_di << 8;
	reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	_humi |= reg_i2c_di;
	reg_i2c_ctrl = FLD_I2C_CMD_STOP;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	if(crc == data) {
		measured_data.temp = ((int32_t)(17500*_temp) >> 16) - 4500 + cfg.temp_offset * 10; // x 0.01 C
		measured_data.humi = ((uint32_t)(10000*_humi) >> 16) + cfg.humi_offset * 10; // x 0.01 %
		measured_data.count++;
		send_sensor(SHTC3_GO_SLEEP); // Sleep command of the sensor
		return 1;
	} else {
		send_sensor(SHTC3_SOFT_RESET); // Soft reset command
		sleep_us(240);
	}
	return 0;
}

_attribute_ram_code_ void read_sensor_deep_sleep(void) {
	read_sensor_start(SHTC3_MEASURE);
	gpio_setup_up_down_resistor(GPIO_PC2, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(GPIO_PC3, PM_PIN_PULLUP_1M);
	timer_measure_cb = clock_time() | 1;
}

_attribute_ram_code_ void read_sensor_low_power(void) {
	read_sensor_start(SHTC3_LPMEASURE);
}

_attribute_ram_code_ int read_sensor_sleep(void) {
	read_sensor_start(SHTC3_MEASURE);
	StallWaitMs(11);
	return read_sensor_cb();
}

