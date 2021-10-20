#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#if (HTSENSOR == HTSENSOR_SHV4)
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"
#include "stack/ble/ll/ll_pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#endif

#define SHV4_TRESET_us		900  // max 1 ms
#define SHV4_I2C_ADDR		0x44
#define SHV4_SOFT_RESET		0x94 // Soft reset command
#define SHV4_MEASURE_HI		0xFD // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHV4_MEASURE_LO		0xE0 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

RAM volatile uint32_t timer_measure_cb;

_attribute_ram_code_ void send_sensor(uint8_t cmd) {
	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = SHV4_I2C_ADDR<<1;
	reg_i2c_adr = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

void init_sensor(){
	send_sensor(SHV4_SOFT_RESET); // Soft reset command
	sleep_us(SHV4_TRESET_us); // max 1 ms
}

_attribute_ram_code_ void read_sensor_start(uint8_t mcmd) {
#if	USE_TRIGGER_OUT && defined(GPIO_RDS)
	rds_input_on();
#endif
	reg_i2c_id = SHV4_I2C_ADDR<<1;
	reg_i2c_adr = mcmd; // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) int read_sensor_cb(void) {
	uint16_t _temp;
	uint16_t _humi;
	uint8_t data, crc; // calculated checksum
	int i;

	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();

	reg_i2c_id = (SHV4_I2C_ADDR<<1) | FLD_I2C_WRITE_READ_BIT;
	i = 512;
	do {
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_START;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	} while((reg_i2c_status & FLD_I2C_NAK) && i--);
	if (!i) {
		reg_i2c_ctrl = FLD_I2C_CMD_STOP;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
		send_sensor(SHV4_SOFT_RESET); // Soft reset command
		//sleep_us(SHV4_TRESET_us);
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
	if(crc == data && _temp != 0xffff) {
		measured_data.temp = ((int32_t)(17500*_temp) >> 16) - 4500 + cfg.temp_offset * 10; // x 0.01 C
		measured_data.humi = ((uint32_t)(10000*_humi) >> 16) - 600 + cfg.humi_offset * 10; // x 0.01 %
		if(measured_data.humi < 0) measured_data.humi = 0;
		else if(measured_data.humi > 9999) measured_data.humi = 9999;
		measured_data.count++;
		return 1;
	} else {
		send_sensor(SHV4_SOFT_RESET); // Soft reset command
		//sleep_us(SHV4_TRESET_us);
	}
	return 0;
}

_attribute_ram_code_ void read_sensor_deep_sleep(void) {
	read_sensor_start(SHV4_MEASURE_HW);
	gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_1M);
	timer_measure_cb = clock_time() | 1;
}

_attribute_ram_code_ void read_sensor_low_power(void) {
	read_sensor_start(SHV4_MEASURE_LO);
	sleep_us(1700);
}

_attribute_ram_code_ int read_sensor_sleep(void) {
	read_sensor_start(SHV4_MEASURE_HI);
	pm_wait_ms(SENSOR_MEASURING_TIMEOUT_ms);
	return read_sensor_cb();
}

#endif // (HTSENSOR == HTSENSOR_SHV4)
