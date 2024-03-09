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
#define SHTC3_MEASURE_CS	0xA27C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHTC3_LPMEASURE		0x9C60 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHTC3_LPMEASURE_CS	0x245C // Measurement commands, Clock Stretching, Low Power Mode, Read T First
#define SHTC3_GET_ID		0xC8EF // read ID register

// Sensor SHT4x https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_Datasheet.pdf
//#define SHT4x_I2C_ADDR		0x44
#define SHT4x_SOFT_RESET	0x94 // Soft reset command
#define SHT4x_SOFT_RESET_us	900  // max 1 ms
#define SHT4x_MEASURE_HI	0xFD // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT4x_MEASURE_HI_us 7000 // 6.9..8.2 ms
#define SHT4x_MEASURE_LO	0xE0 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT4x_MEASURE_LO_us 1700 // 1.7 ms
#define SHT4x_GET_ID		0x89 // read ID register

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

RAM thsensor_cfg_t thsensor_cfg;
/*
const thsensor_coef_t def_thcoef_cht8215 = {
		.temp_k = 25606,
		.humi_k = 20000,
		.temp_z = 0,
		.humi_z = 0
};

const thsensor_coef_t def_thcoef_cht8305 = {
		.temp_k = 16500,
		.humi_k = 10000,
		.temp_z = -4000,
		.humi_z = 0
};

const thsensor_coef_t def_thcoef_aht30 = {
		.temp_k = 1250,
		.humi_k = 625,
		.temp_z = -5000,
		.humi_z = 0
};
*/

const thsensor_coef_t def_thcoef_shtc3 = {
		.temp_k = 17500,
		.temp_z = -4500,
		.humi_k = 10000,
		.humi_z = 0
};

const thsensor_coef_t def_thcoef_sht4x = {
		.temp_k = 17500,
		.temp_z = -4500,
		.humi_k = 12500,
		.humi_z = -600
};

static _attribute_ram_code_ void send_sensor_word(uint16_t cmd) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = thsensor_cfg.i2c_addr;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
}

#if (DEVICE_TYPE == DEVICE_CGDK2)
static _attribute_ram_code_ void send_bdsensor_word(uint16_t cmd) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = 0;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
}
#endif

static _attribute_ram_code_ void send_sensor_byte(uint8_t cmd) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = thsensor_cfg.i2c_addr;
	reg_i2c_adr = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
}

_attribute_ram_code_ void sensor_go_sleep(void) {
	if (thsensor_cfg.i2c_addr == (SHTC3_I2C_ADDR << 1))
		send_sensor_word(SHTC3_GO_SLEEP); // Sleep command of the sensor
}

static void soft_reset_sensor(void) {
	if (thsensor_cfg.i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_SOFT_RESET); // Soft reset command
		sleep_us(SHTC3_SOFT_RESET_us); // 240 us
	} else if (thsensor_cfg.i2c_addr) {
		send_sensor_byte(SHT4x_SOFT_RESET); // Soft reset command
		sleep_us(SHT4x_SOFT_RESET_us); // max 1 ms
	}
}

_attribute_ram_code_
uint8_t sensor_crc(uint8_t crc) {
	int i;
	for(i = 8; i > 0; i--) {
		if (crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	return crc;
}

uint32_t get_sensor_id(void) {
	static const uint8_t utr_c3_gid[] = { 0x02,0x03,SHTC3_I2C_ADDR << 1, SHTC3_GET_ID & 0xff, (SHTC3_GET_ID >> 8) & 0xff };
	i2c_utr_t utr;
	uint8_t buf_id[6];
	uint32_t id = 0;
	if(thsensor_cfg.i2c_addr == (SHTC3_I2C_ADDR << 1)) {

		if(!I2CBusUtr(&buf_id, (i2c_utr_t *)&utr_c3_gid, sizeof(utr_c3_gid) - 3)
			&& buf_id[2] == sensor_crc(buf_id[1] ^ sensor_crc(buf_id[0] ^ 0xff))) { // = 0x5b
			id = (buf_id[0] << 8) | buf_id[1]; // = 0x8708
		}
	} else if(thsensor_cfg.i2c_addr)	{
		if(!send_i2c_byte(thsensor_cfg.i2c_addr, SHT4x_GET_ID)) {
			utr.mode = 0;
			utr.rdlen = 6;
			utr.wrdata[0] = thsensor_cfg.i2c_addr | 1;
			sleep_us(SHT4x_SOFT_RESET_us);
			if(I2CBusUtr(&buf_id, (i2c_utr_t *)&utr, 0) == 0
			&& buf_id[2] == sensor_crc(buf_id[1] ^ sensor_crc(buf_id[0] ^ 0xff))
			&& buf_id[5] == sensor_crc(buf_id[4] ^ sensor_crc(buf_id[3] ^ 0xff))
			) {
				id = (buf_id[3] << 24) | (buf_id[4] << 16) | (buf_id[0] << 8) | buf_id[1];
			}
		}
	}
	return id;
}

static int check_sensor(void) {
	uint8_t *ptabinit = NULL;
	if ((thsensor_cfg.i2c_addr = (uint8_t) scan_i2c_addr(SHTC3_I2C_ADDR << 1)) != 0) {
		cfg.hw_cfg.shtc3 = 1; // = 1 - sensor SHTC3
		ptabinit = (uint8_t *)&def_thcoef_shtc3;
	} else {
		thsensor_cfg.i2c_addr = (uint8_t) scan_i2c_addr(SHT4x_I2C_ADDR << 1);
		cfg.hw_cfg.shtc3 = 0; // = 0 - sensor SHT4x or ?
		if(!thsensor_cfg.i2c_addr)
			thsensor_cfg.i2c_addr = (uint8_t) scan_i2c_addr(SHT4xB_I2C_ADDR << 1);
		if(thsensor_cfg.i2c_addr)
			ptabinit = (uint8_t *)&def_thcoef_sht4x;
	}
	soft_reset_sensor();
	if(thsensor_cfg.coef.temp_k == 0 && ptabinit) {
		memcpy(&thsensor_cfg.coef, ptabinit, sizeof(thsensor_cfg.coef));
	}
	// no i2c sensor ? thsensor_cfg.i2c_addr = 0
	return thsensor_cfg.i2c_addr;
}

void init_sensor(void) {
	//scan_i2c_addr(0);
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	sleep_us(SHTC3_WAKEUP_us);	// 240 us
	//	Wake-up command of the SHTC3 sensor
	send_i2c_word(SHTC3_I2C_ADDR << 1, SHTC3_WAKEUP);
	sleep_us(SHTC3_WAKEUP_us);	// 240 us
	check_sensor();
	thsensor_cfg.id = get_sensor_id();
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) int read_sensor_cb(void) {
	uint16_t _temp;
	uint16_t _humi;
	uint8_t data, crc; // calculated checksum
	int i;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
	if (thsensor_cfg.i2c_addr == 0) {
		if(check_sensor())
			sensor_go_sleep();
		return 0;
	}
#if (DEVICE_TYPE == DEVICE_CGDK2)
	if(thsensor_cfg.id)
		reg_i2c_id = thsensor_cfg.i2c_addr | FLD_I2C_WRITE_READ_BIT;
	else
		reg_i2c_id = FLD_I2C_WRITE_READ_BIT;
#else
	reg_i2c_id = thsensor_cfg.i2c_addr | FLD_I2C_WRITE_READ_BIT;
#endif
	i = 512;
	do {
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_START;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		if (reg_i2c_status & FLD_I2C_NAK) {
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		} else { // ACK ok
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_temp = data << 8;
			crc = sensor_crc(data ^ 0xff);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_temp |= data;
			crc = sensor_crc(crc ^ data);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			_humi = reg_i2c_di << 8;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			_humi |= reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			if (crc == data && _temp != 0xffff) {
				measured_data.temp = ((int32_t)(thsensor_cfg.coef.temp_k * _temp) >> 16) + thsensor_cfg.coef.temp_z; // x 0.01 C
				measured_data.humi = ((uint32_t)(thsensor_cfg.coef.humi_k * _humi) >> 16) + thsensor_cfg.coef.humi_z; // x 0.01 %
				if (measured_data.humi < 0) measured_data.humi = 0;
				else if (measured_data.humi > 9999) measured_data.humi = 9999;
				measured_data.count++;
				// sensor_go_sleep(); // Sleep command of the sensor
				if (thsensor_cfg.i2c_addr == (SHTC3_I2C_ADDR << 1))
					send_sensor_word(SHTC3_GO_SLEEP); // Sleep command of the sensor
				return 1;
			}
		}
	} while (i--);
	check_sensor();
	// Sleep command of the sensor = sensor_go_sleep();
	sensor_go_sleep();
	return 0;
}

_attribute_ram_code_ void start_measure_sensor_deep_sleep(void) {
	if (thsensor_cfg.i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_WAKEUP); //	Wake-up command of the sensor
		sleep_us(SHTC3_WAKEUP_us - 5);	// 240 us
#if (DEVICE_TYPE == DEVICE_CGDK2)
		if(thsensor_cfg.id)
			send_sensor_word(SHTC3_MEASURE);
		else
			send_bdsensor_word(SHTC3_MEASURE_CS);
#else
		send_sensor_word(SHTC3_MEASURE);
#endif
	} else if (thsensor_cfg.i2c_addr) {
		send_sensor_byte(SHT4x_MEASURE_HI);
	} else
		return;
	thsensor_cfg.time_measure = clock_time() | 1;
}

_attribute_ram_code_ void start_measure_sensor_low_power(void) {
	if (thsensor_cfg.i2c_addr == (SHTC3_I2C_ADDR << 1)) {
		send_sensor_word(SHTC3_WAKEUP); //	Wake-up command of the sensor
		sleep_us(SHTC3_WAKEUP_us);	// 240 us
#if (DEVICE_TYPE == DEVICE_CGDK2)
		if(thsensor_cfg.id)
			send_sensor_word(SHTC3_LPMEASURE);
		else
			send_bdsensor_word(SHTC3_LPMEASURE_CS);
#else
		send_sensor_word(SHTC3_LPMEASURE);
#endif
	} else if (thsensor_cfg.i2c_addr) {
		send_sensor_byte(SHT4x_MEASURE_LO);
		sleep_us(SHT4x_MEASURE_LO_us); // 1700 us
	}
}
