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

// Sensor SHTC3 https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHTC3_Datasheet.pdf
//#define SHTC3_I2C_ADDR		0x70
#define SHTC3_WAKEUP		0x1735 // Wake-up command of the sensor
#define SHTC3_WAKEUP_us		240    // time us
#define SHTC3_SOFT_RESET	0x5d80 // Soft reset command
#define SHTC3_SOFT_RESET_us	240    // time us
#define SHTC3_GO_SLEEP		0x98b0 // Sleep command of the sensor
#define SHTC3_MEASURE		0x6678 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHTC3_LPMEASURE		0x9C60 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First

// Sensor SHT4x https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_Datasheet.pdf
//#define SHT4x_I2C_ADDR		0x44
#define SHT4x_SOFT_RESET	0x94 // Soft reset command
#define SHT4x_SOFT_RESET_us	900  // max 1 ms
#define SHT4x_MEASURE_HI	0xFD // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT4x_MEASURE_HI_us 7000 // 6.9..8.2 ms
#define SHT4x_MEASURE_LO	0xE0 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT4x_MEASURE_LO_us 1700 // 1.7 ms


#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

RAM volatile uint32_t timer_measure_cb;
RAM uint8_t sensor_i2c_addr;

static _attribute_ram_code_ void send_sensor_word(uint16_t cmd) {
	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = sensor_i2c_addr;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

static _attribute_ram_code_ void send_sensor_byte(uint8_t cmd) {
	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = sensor_i2c_addr;
	reg_i2c_adr = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

static void soft_reset_sensor(void) {
	if(sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_SOFT_RESET); // Soft reset command
		sleep_us(SHTC3_SOFT_RESET_us); // 240 us
		send_sensor_word(SHTC3_GO_SLEEP); // Sleep command of the sensor
	} else {
		send_sensor_byte(SHT4x_SOFT_RESET); // Soft reset command
		sleep_us(SHT4x_SOFT_RESET_us); // max 1 ms
	}
}

void init_sensor() {
	if((sensor_i2c_addr = (uint8_t) scan_i2c_addr(SHTC3_I2C_ADDR << 1)) != 0) {
		send_sensor_word(SHTC3_WAKEUP); //	Wake-up command of the sensor
		cfg.hw_cfg.shtc3 = 1; // =1 - sensor SHTC3
		sleep_us(SHTC3_WAKEUP_us);	// 240 us
		soft_reset_sensor();
		return;
	}
	cfg.hw_cfg.shtc3 = 0; // = 0 - sensor SHT4x or ?
	if((sensor_i2c_addr = (uint8_t) scan_i2c_addr(SHT4x_I2C_ADDR << 1)) != 0) {
		soft_reset_sensor();
		return;
	}
	// no i2c sensor ? sensor_i2c_addr = 0
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) int read_sensor_cb(void) {
	uint16_t _temp;
	uint16_t _humi;
	uint8_t data, crc; // calculated checksum
	int i;
	if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
	if(sensor_i2c_addr == 0) {
		init_sensor();
		return 0;
	}
	reg_i2c_id = sensor_i2c_addr | FLD_I2C_WRITE_READ_BIT;
	i = 512;
	do {
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_START;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
		if(reg_i2c_status & FLD_I2C_NAK) {
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while(reg_i2c_status & FLD_I2C_CMD_BUSY);
		} else { // ACK ok
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
				measured_data.humi = ((uint32_t)(10000*_humi) >> 16) + cfg.humi_offset * 10; // x 0.01 %
				if(measured_data.humi < 0) measured_data.humi = 0;
				else if(measured_data.humi > 9999) measured_data.humi = 9999;
				measured_data.count++;
				if(sensor_i2c_addr == (SHTC3_I2C_ADDR << 1))
					send_sensor_word(SHTC3_GO_SLEEP); // Sleep command of the sensor
				return 1;
			}
		}
	} while(i--);
	soft_reset_sensor();
	return 0;
}

_attribute_ram_code_ void read_sensor_deep_sleep(void) {
	if(sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_WAKEUP); //	Wake-up command of the sensor
		sleep_us(SHTC3_WAKEUP_us - 5);	// 240 us
		send_sensor_word(SHTC3_MEASURE);
	} else // if(sensor_i2c_addr == (SHT4x_I2C_ADDR << 1))
		send_sensor_byte(SHT4x_MEASURE_HI);
	gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_1M);
	timer_measure_cb = clock_time() | 1;
}

_attribute_ram_code_ void read_sensor_low_power(void) {
	if(sensor_i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_WAKEUP); //	Wake-up command of the sensor
		sleep_us(SHTC3_WAKEUP_us);	// 240 us
		send_sensor_word(SHTC3_LPMEASURE);
	} else { // if((sensor_i2c_addr) == SHT4x_I2C_ADDR << 1) {
		send_sensor_byte(SHT4x_MEASURE_LO);
		sleep_us(SHT4x_MEASURE_LO_us); // 1700 us
	}
}
