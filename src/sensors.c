#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"

#if (DEV_SERVICES & SERVICE_THS)

#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"
//#include "stack/ble/ll/ll_pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"

//==================================== SHTC3

//  I2C addres
//#define SHTC3_I2C_ADDR		0x70

// Sensor SHTC3 https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHTC3_Datasheet.pdf
#define SHTC3_WAKEUP		0x1735 // Wake-up command of the sensor
#define SHTC3_SOFT_RESET	0x5d80 // Soft reset command
#define SHTC3_GO_SLEEP		0x98b0 // Sleep command of the sensor
#define SHTC3_MEASURE		0x6678 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHTC3_MEASURE_CS	0xA27C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHTC3_LPMEASURE		0x9C60 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHTC3_LPMEASURE_CS	0x245C // Measurement commands, Clock Stretching, Low Power Mode, Read T First
#define SHTC3_GET_ID		0xC8EF // read ID register

#define SHTC3_WAKEUP_us			240    // time us
#define SHTC3_POWER_TIMEOUT_us	240		// time us, 180..240 us
#define SHTC3_SOFT_RESET_us		240		// time us, 180..240 us
#define SHTC3_HI_MEASURE_us		11000	// time us, 10.8..12.1 ms
#define SHTC3_LO_MEASURE_us		750		// time us, 0.7..0.8 ms ?

#define SHTC3_MAX_CLK_HZ		1000000	// I2C FM+ (1MHz)
#define SHTC3_MEASURING_TIMEOUT  (SHTC3_HI_MEASURE_us * CLOCK_16M_SYS_TIMER_CLK_1US) // 11 ms

const sensor_def_cfg_t def_thcoef_shtc3 = {
		.coef.val1_k = 17500, // temp_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_k = 10000, // humi_k
		.coef.val2_z = 0, // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = SHTC3_MEASURING_TIMEOUT,
#endif
		.sensor_type = TH_SENSOR_SHTC3
};

//==================================== SHT4x

//  I2C addres
//#define SHT4x_I2C_ADDR			0x44
//#define SHT4xB_I2C_ADDR			0x45

// Sensor SHT4x https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_Datasheet.pdf
#define SHT4x_SOFT_RESET	0x94 // Soft reset command
#define SHT4x_MEASURE_HI	0xFD // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT4x_MEASURE_MD	0xF6 // Measurement commands, Clock Stretching Disabled, Medium precision, Read T First
#define SHT4x_MEASURE_LO	0xE0 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT4x_GET_ID		0x89 // read serial number

#define SHT4x_POWER_TIMEOUT_us	1000	// time us, 0.3..1 ms
#define SHT4x_SOFT_RESET_us		1000	// time us, 1 ms
#define SHT4x_HI_MEASURE_us		7000	// time us, 6.9..8.3 ms
#define SHT4x_MD_MEASURE_us		4000	// time us, 3.7..4.5 ms
#define SHT4x_LO_MEASURE_us		1600	// time us, 1.3..1.6 ms

#define SHT4x_MAX_CLK_HZ		1000000	// I2C FM+ (1MHz)
#define SHT4x_MEASURING_TIMEOUT  (SHT4x_HI_MEASURE_us * CLOCK_16M_SYS_TIMER_CLK_1US) // 7 ms

const sensor_def_cfg_t def_thcoef_sht4x = {
		.coef.val1_k = 17500, // temp_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_k = 12500, // humi_k
		.coef.val2_z = -600, // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = SHT4x_MEASURING_TIMEOUT,
#endif
		.sensor_type = TH_SENSOR_SHT4x
};

//==================================== SHT30/GXHT3x

//  I2C addres
//#define SHT30_I2C_ADDR		0x44
//#define SHT30_I2C_ADDR_MAX	0x45

#define SHT30_SOFT_RESET	0xA230 // Soft reset command
#define SHT30_RD_STATUS		0x2DF3 // Read status reg (2 bytes + crc)
#define SHT30_HIMEASURE		0x0024 // Measurement commands, Clock Stretching Disabled, Normal Mode, Read T First
#define SHT30_HIMEASURE_CS	0x062C // Measurement commands, Clock Stretching, Normal Mode, Read T First
#define SHT30_LPMEASURE		0x1624 // Measurement commands, Clock Stretching Disabled, Low Power Mode, Read T First
#define SHT30_LPMEASURE_CS	0x102C // Measurement commands, Clock Stretching, Low Power Mode, Read T First

#define SHT30_POWER_TIMEOUT_us	1500	// time us, 0.5..1.5 ms
#define SHT30_SOFT_RESET_us		1500	// time us, 0.5..1.5 ms
#define SHT30_HI_MEASURE_us		15000	// time us, 12.5..15.5 ms
#define SHT30_MD_MEASURE_us		5500	// time us, 4.5..6.5 ms
#define SHT30_LO_MEASURE_us		3500	// time us, 2.5..4.5 ms

#define SHT30_MAX_CLK_HZ		1000000	// I2C FM+ (1MHz)
#define SHT30_MEASURING_TIMEOUT  (SHT30_HI_MEASURE_us * CLOCK_16M_SYS_TIMER_CLK_1US) // SHTV3 11 ms

const sensor_def_cfg_t def_thcoef_sht30 = { // = shtc3
		.coef.val1_k = 17500, // temp_k
		.coef.val2_k = 10000, // humi_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_z = 0,	  // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = SHT30_MEASURING_TIMEOUT,
#endif
		.sensor_type = TH_SENSOR_SHT30
};

//==================================== AHT20-30

//  I2C addres
//#define AHT2x_I2C_ADDR	0x38

#define AHT2x_CMD_INI		0x0E1  // Initialization Command
#define AHT2x_CMD_TMS		0x0AC  // Trigger Measurement Command
#define AHT2x_DATA1_TMS		0x33  // Trigger Measurement data
#define AHT2x_DATA2_TMS		0x00  // Trigger Measurement data
/* Wait 80ms for the measurement to be completed, if the read status word Bit [7] is 0, it means
the measurement is completed, and then six bytes can be read continuously */
#define AHT2x_CMD_RST		0x0BA  // Soft Reset Command
#define AHT2x_RD_STATUS		0x071  // Read Status
/*Before reading the temperature and humidity value,
get a byte of status word by sending 0x71. If the status word and 0x18 are not equal to 0x18,
initialize the 0x1B, 0x1C, 0x1E registers, details Please refer to our official website routine for
the initialization process; if they are equal, proceed to the next step.*/
#define AHT2x_DATA_LPWR		0x0800 // go into low power mode

/* After power-on, the sensor needs less than 100ms stabilization time (SCL is
high at this time) to reach the idle state and it is ready to receive commands sent by the host
(MCU).*/
#define AHT2x_POWER_TIMEOUT_us	5000	// time us, 100 ms
#define AHT2x_SOFT_RESET_us		10000	// time us, 10 ms
#define AHT2x_MEASURE_us		80000	// time us, 80 ms

#define AHT2x_MAX_CLK_HZ		400000  // 400 kHz
#define AHT2x_MEASURING_TIMEOUT  (AHT2x_MEASURE_us * CLOCK_16M_SYS_TIMER_CLK_1US) // 80 ms

const sensor_def_cfg_t def_thcoef_aht2x = {
		.coef.val1_k = 1250, // temp_k
		.coef.val2_k = 625, // temp_z
		.coef.val1_z = -5000, // humi_k
		.coef.val2_z = 0, // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = AHT2x_MEASURING_TIMEOUT,
#endif
		.sensor_type = TH_SENSOR_AHT2x
};

//==================================== CHT8305

//  I2C addres
//#define CHT8305_I2C_ADDR		0x40
//#define CHT8305_I2C_ADDR_MAX	0x43

#define CHT8xxx_REG_MID		0xfe
#define CHT8xxx_REG_VID		0xff

//  Registers
#define CHT8305_REG_TMP		0x00
#define CHT8305_REG_HMD		0x01
#define CHT8305_REG_CFG		0x02
#define CHT8305_REG_ALR		0x03
#define CHT8305_REG_VLT		0x04
#define CHT8305_REG_MID		0xfe
#define CHT8305_REG_VID		0xff

//  Config register mask
#define CHT8305_CFG_SOFT_RESET          0x8000
#define CHT8305_CFG_CLOCK_STRETCH       0x4000
#define CHT8305_CFG_HEATER              0x2000
#define CHT8305_CFG_MODE                0x1000
#define CHT8305_CFG_VCCS                0x0800
#define CHT8305_CFG_TEMP_RES            0x0400
#define CHT8305_CFG_HUMI_RES            0x0300
#define CHT8305_CFG_ALERT_MODE          0x00C0
#define CHT8305_CFG_ALERT_PENDING       0x0020
#define CHT8305_CFG_ALERT_HUMI          0x0010
#define CHT8305_CFG_ALERT_TEMP          0x0008
#define CHT8305_CFG_VCC_ENABLE          0x0004
#define CHT8305_CFG_VCC_RESERVED        0x0003

#define CHT8305_MID	0x5959
#define CHT8305_VID	0x8305

#define CHT8305_POWER_TIMEOUT_us	5000	// time us, 5 ms
#define CHT8305_SOFT_RESET_us		5000	// time us, 5 ms
#define CHT8305_MEASURE_us			(6500+6500)	// time us, 6.5 T + 6.5 H ms

#define CHT8305_MAX_CLK_HZ			400000	// 400 kHz
#define CHT8305_MEASURING_TIMEOUT  (CHT8305_MEASURE_us * CLOCK_16M_SYS_TIMER_CLK_1US) // 13 ms

const sensor_def_cfg_t def_thcoef_cht8305 = {
		.coef.val1_k = 16500, // temp_k
		.coef.val2_k = 10000, // humi_k
		.coef.val1_z = -4000, // temp_z
		.coef.val2_z = 0, // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = CHT8305_MEASURING_TIMEOUT,
#endif
		.sensor_type = TH_SENSOR_CHT8305
};

//==================================== CHT8215/CHT8310

#define CHT8215_POWER_TIMEOUT_us	5000	// time us, 5 ms
#define CHT8215_SOFT_RESET_us		5000	// time us, 5 ms
#define CHT8215_MEASURE_us			(6500+6500)	// time us, 6.5 T + 6.5 H ms

#define CHT8215_MAX_CLK_HZ			400000	// 400 kHz
#define CHT8215_MEASURING_TIMEOUT   3 // none

#define CHT8215_I2C_ADDR0	0x40
#define CHT8215_I2C_ADDR1	0x44
#define CHT8215_I2C_ADDR2	0x48
#define CHT8215_I2C_ADDR3	0x4C

//	Registers
#define CHT8215_REG_TMP		0x00
#define CHT8215_REG_HMD		0x01
#define CHT8215_REG_STA		0x02
#define CHT8215_REG_CFG		0x03
#define CHT8215_REG_CRT		0x04
#define CHT8215_REG_TLL		0x05
#define CHT8215_REG_TLM		0x06
#define CHT8215_REG_HLL		0x07
#define CHT8215_REG_HLM		0x08
#define CHT8215_REG_OST		0x0f
#define CHT8215_REG_RST		0xfc
#define CHT8215_REG_MID		0xfe
#define CHT8215_REG_VID		0xff

//	Status register mask
#define CHT8215_STA_BUSY	0x8000
#define CHT8215_STA_THI		0x4000
#define CHT8215_STA_TLO		0x2000
#define CHT8215_STA_HHI		0x1000
#define CHT8215_STA_HLO		0x0800

//	Config register mask
#define CHT8215_CFG_MASK		0x8000
#define CHT8215_CFG_SD			0x4000
#define CHT8215_CFG_ALTH		0x2000
#define CHT8215_CFG_EM			0x1000
#define CHT8215_CFG_EHT			0x0100
#define CHT8215_CFG_TME			0x0080
#define CHT8215_CFG_POL			0x0020
#define CHT8215_CFG_ALT			0x0018
#define CHT8215_CFG_CONSEC_FQ	0x0006
#define CHT8215_CFG_ATM			0x0001

#define CHT8215_MID	0x5959
#define CHT8215_VID	0x1582

const sensor_def_cfg_t def_thcoef_cht8215 = {
		.coef.val1_k = 25606, // temp_k
		.coef.val1_z = 0, // temp_z
		.coef.val2_k = 20000, // humi_k
		.coef.val2_z = 0, // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = CHT8215_MEASURING_TIMEOUT,
#endif
		.sensor_type = TH_SENSOR_CHT8215
};

//===================================

RAM sensor_cfg_t sensor_cfg;

#if (USE_SENSOR_AHT20_30 || USE_SENSOR_SHT4X ||  USE_SENSOR_SHTC3 || USE_SENSOR_SHT30)

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001


_attribute_ram_code_
static uint8_t sensor_crc(uint8_t crc) {
	int i;
	for(i = 8; i > 0; i--) {
		if (crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	return crc;
}
#endif

#if USE_SENSOR_AHT20_30

_attribute_ram_code_
static uint8_t sensor_crc_buf(uint8_t * msg, int len) {
	int i;
	uint8_t crc = 0xff;
	for(i = 0; i < len; i++) {
		crc = sensor_crc(crc ^ msg[i]);
	}
	return crc;
}

_attribute_ram_code_
static int start_measure_aht2x(void) {
	const uint8_t data[3] = {AHT2x_CMD_TMS, AHT2x_DATA1_TMS, AHT2x_DATA2_TMS};
	// 04000070ac3300
	return send_i2c_buf(sensor_cfg.i2c_addr, (uint8_t *)data, sizeof(data));
}

_attribute_ram_code_ __attribute__((optimize("-Os")))
static int read_sensor_aht2x(void) {
	uint32_t _temp;
	uint8_t data[7];
	// 04000771
	if(read_i2c_buf(sensor_cfg.i2c_addr, data, sizeof(data)) == 0
	    && (data[0] & 0x80) == 0
	    && sensor_crc_buf(data, sizeof(data)) == 0) {
			_temp = (data[3] & 0x0F) << 16 | (data[4] << 8) | data[5];
			measured_data.temp = ((uint32_t)(_temp * sensor_cfg.coef.val1_k) >> 16) + sensor_cfg.coef.val1_z; // x 0.01 C // 16500 -4000
			_temp = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4);
			measured_data.humi = ((uint32_t)(_temp * sensor_cfg.coef.val2_k) >> 16) + sensor_cfg.coef.val2_z; // x 0.01 % // 10000 -0
			if (measured_data.humi < 0) measured_data.humi = 0;
			else if (measured_data.humi > 9999) measured_data.humi = 9999;
			measured_data.count++;
#if !SENSOR_SLEEP_MEASURE
			if(!start_measure_aht2x()) // start measure T/H
#endif
				return 1;
	}
	sensor_cfg.i2c_addr = 0;
	return 0;
}

#endif

#if USE_SENSOR_CHT8305
_attribute_ram_code_
__attribute__((optimize("-Os")))
static int read_sensor_cht8305(void) {
	uint32_t _temp, i = 3;
	uint8_t reg_data[4];
	while(i--) {
		if (!read_i2c_buf(sensor_cfg.i2c_addr, reg_data, sizeof(reg_data))) {
			_temp = (reg_data[0] << 8) | reg_data[1];
			measured_data.temp = ((uint32_t)(_temp * sensor_cfg.coef.val1_k) >> 16) + sensor_cfg.coef.val1_z; // x 0.01 C // 16500 -4000
			_temp = (reg_data[2] << 8) | reg_data[3];
			measured_data.humi = ((uint32_t)(_temp * sensor_cfg.coef.val2_k) >> 16) + sensor_cfg.coef.val2_z; // x 0.01 % // 10000 -0
			if (measured_data.humi < 0) measured_data.humi = 0;
			else if (measured_data.humi > 9999) measured_data.humi = 9999;
			measured_data.count++;
#if !SENSOR_SLEEP_MEASURE
			send_i2c_byte(sensor_cfg.i2c_addr, CHT8305_REG_TMP); // start measure T/H
#endif
			return 1;
		}
	}
	sensor_cfg.i2c_addr = 0;
	return 0;
}
#endif

#if USE_SENSOR_CHT8215
_attribute_ram_code_
__attribute__((optimize("-Os")))
static int read_sensor_cht8215(void) {
	uint32_t _temp, i = 3;
	uint8_t reg_data[4];
	while(i--) {
		if ((!read_i2c_byte_addr(sensor_cfg.i2c_addr, CHT8215_REG_TMP, reg_data, 2))
			&&(!read_i2c_byte_addr(sensor_cfg.i2c_addr, CHT8215_REG_HMD, &reg_data[2], 2))) {
			_temp = (reg_data[0] << 8) | reg_data[1];
			measured_data.temp = ((uint32_t)(_temp * sensor_cfg.coef.val1_k) >> 16) + sensor_cfg.coef.val1_z; // x 0.01 C // 16500 -4000
			_temp = (reg_data[2] << 8) | reg_data[3];
			measured_data.humi = ((uint32_t)(_temp * sensor_cfg.coef.val2_k) >> 16) + sensor_cfg.coef.val2_z; // x 0.01 % // 10000 -0
			if (measured_data.humi < 0) measured_data.humi = 0;
			else if (measured_data.humi > 9999) measured_data.humi = 9999;
			measured_data.count++;
			return 1;
		}
	}
	sensor_cfg.i2c_addr = 0;
	return 0;
}
#endif

#if (USE_SENSOR_SHT4X || USE_SENSOR_SHTC3 || USE_SENSOR_SHT30)
_attribute_ram_code_ __attribute__((optimize("-Os")))
static int read_sensor_sht30_shtc3_sht4x(void) {
	int ret = 0;
	uint16_t _temp;
	uint16_t _humi;
	uint8_t data, crc; // calculated checksum
	int i;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
#if (I2C_MAX_SPEED > 400000)
	uint8_t r = reg_i2c_speed;
	reg_i2c_speed = (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*I2C_MAX_SPEED)); // 700 kHz
#endif
#if (DEVICE_TYPE == DEVICE_CGDK2) // SHTC3
	if(sensor_cfg.id == 0xBDC3)
		reg_i2c_id = FLD_I2C_WRITE_READ_BIT;
	else
		reg_i2c_id = sensor_cfg.i2c_addr | FLD_I2C_WRITE_READ_BIT;
#else
	reg_i2c_id = sensor_cfg.i2c_addr | FLD_I2C_WRITE_READ_BIT;
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
#if 1	// use humi CRC
			if (crc != data) {
				reg_i2c_ctrl = FLD_I2C_CMD_STOP;
				while (reg_i2c_status & FLD_I2C_CMD_BUSY);
				continue;
			}
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			_humi = data << 8;
			crc = sensor_crc(data ^ 0xff);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
			_humi |= data;
			crc = sensor_crc(crc ^ data);
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			if (crc == data) {
#else
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			data = reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
				_humi = data << 8;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			_humi |= reg_i2c_di;
			reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			if (crc == data && _temp != 0xffff) {
#endif
				measured_data.temp = ((int32_t)(_temp * sensor_cfg.coef.val1_k) >> 16) + sensor_cfg.coef.val1_z; // x 0.01 C //17500 - 4500
				measured_data.humi = ((uint32_t)(_humi * sensor_cfg.coef.val2_k) >> 16) + sensor_cfg.coef.val2_z; // x 0.01 %	   // 10000 -0
				if (measured_data.humi < 0) measured_data.humi = 0;
				else if (measured_data.humi > 9999) measured_data.humi = 9999;
				measured_data.count++;
#if USE_SENSOR_SHTC3
				if (sensor_cfg.sensor_type == TH_SENSOR_SHTC3) {
					send_i2c_word(sensor_cfg.i2c_addr, SHTC3_GO_SLEEP); // Sleep command of the sensor
				} else
#endif
#if !SENSOR_SLEEP_MEASURE
#if USE_SENSOR_SHT4X
				if(sensor_cfg.sensor_type == TH_SENSOR_SHT4x) {
					send_i2c_byte(sensor_cfg.i2c_addr, SHT4x_MEASURE_HI);
				} else
#endif // USE_SENSOR_SHT4X
#if USE_SENSOR_SHT30
				if(sensor_cfg.sensor_type == TH_SENSOR_SHT30) {
					send_i2c_word(sensor_cfg.i2c_addr, SHT30_HIMEASURE); // start measure T/H
				} else
#endif //USE_SENSOR_SHT30
#endif
				{}
				ret = 1;
				break;
			}
		}
	} while (i--);
#if (I2C_MAX_SPEED > 400000)
	reg_i2c_speed = r;
#endif
	if(!ret)
		sensor_cfg.i2c_addr = 0;
	return ret;
}
#endif

static int check_sensor(void) {
#if USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215
	int test_i2c_addr = CHT8305_I2C_ADDR << 1;
#else
#if (USE_SENSOR_SHT4X || USE_SENSOR_SHT30)
	int test_i2c_addr = SHT4x_I2C_ADDR << 1; // or SHT30_I2C_ADDR
#endif
#endif
	uint8_t buf[8];
	sensor_def_cfg_t *ptabinit = NULL;
	sensor_cfg.sensor_type = TH_SENSOR_NONE;
#if SENSOR_SLEEP_MEASURE
	sensor_cfg.measure_timeout = 16384*CLOCK_16M_SYS_TIMER_CLK_1US;
#endif
	sensor_cfg.id = 0;
#if USE_SENSOR_SHTC3
	// cfg.hw_cfg.shtc3 = 0;
	// Scan I2C addr 0x70
	if ((sensor_cfg.i2c_addr = (uint8_t) scan_i2c_addr(SHTC3_I2C_ADDR << 1)) != 0) {
		// SHTC3
		if(!send_i2c_word(SHTC3_I2C_ADDR << 1, SHTC3_WAKEUP)) { //	Wake-up command of the SHTC3 sensor
			sleep_us(SHTC3_WAKEUP_us);	// 240 us
			// cfg.hw_cfg.shtc3 = 1; // = 1 - sensor SHTC3
			if(!send_i2c_word(sensor_cfg.i2c_addr, SHTC3_GET_ID) // Get ID
			&& !read_i2c_buf(sensor_cfg.i2c_addr, buf, 3)
			&& buf[2] == sensor_crc(buf[1] ^ sensor_crc(buf[0] ^ 0xff))) { // = 0x5b
				sensor_cfg.id = (0x00C3<<16) | (buf[0] << 8) | buf[1]; // = 0x8708
			} else {
				// DEVICE_CGDK2 and bad sensor
				sensor_cfg.id = 0xBDC3;
			}
			cfg.flg.lp_measures = 0;
			ptabinit = (sensor_def_cfg_t *)&def_thcoef_shtc3;
		}
		send_i2c_word(sensor_cfg.i2c_addr, SHTC3_GO_SLEEP); // Sleep command of the sensor
	} else
#endif
#if USE_SENSOR_AHT20_30
	// Scan I2C addr 0x38
	 if ((sensor_cfg.i2c_addr = (uint8_t) scan_i2c_addr(AHT2x_I2C_ADDR << 1)) != 0) {
		// AHT2x..30
		// pm_wait_us(AHT2x_POWER_TIMEOUT_us);
		if(!send_i2c_byte(sensor_cfg.i2c_addr, AHT2x_CMD_RST)) { // Soft reset command
			pm_wait_us(AHT2x_SOFT_RESET_us);
			// 0401017071 -> I2C addres 0x70, write 1 bytes, read: 18
			if(!read_i2c_byte_addr(sensor_cfg.i2c_addr, AHT2x_RD_STATUS, buf, 1)) { // buf[0] = 0x18
				sensor_cfg.id = (0x0020 << 16) | buf[0];
//				start_measure_aht2x();
				ptabinit = (sensor_def_cfg_t *)&def_thcoef_aht2x;
			}
		}
	} else
#endif
	// Scan I2C addr 0x40..0x46
	 {
#if (USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215 || USE_SENSOR_SHT4X || USE_SENSOR_SHT30)
		do {
			if((sensor_cfg.i2c_addr = (uint8_t) scan_i2c_addr(test_i2c_addr)) != 0) {
#if (USE_SENSOR_CHT8305 || USE_SENSOR_CHT8215)
				if(sensor_cfg.i2c_addr <= (CHT8305_I2C_ADDR_MAX << 1)) {
					// I2C addr 0x40..0x43
					// CHT8305/CHT8315
					if (!read_i2c_byte_addr(sensor_cfg.i2c_addr, CHT8305_REG_MID, buf, 2) // Get MID
						&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, CHT8305_REG_VID, &buf[2], 2)) {
						sensor_cfg.id = (buf[2] << 24) | (buf[3] << 16) | (buf[0] << 8) | buf[1];
#if USE_SENSOR_CHT8305
						if(sensor_cfg.id == ((CHT8305_VID << 16) | CHT8305_MID)) {
							// Soft reset command
							buf[0] = CHT8305_REG_CFG;
							buf[1] = (CHT8305_CFG_SOFT_RESET | CHT8305_CFG_MODE) >> 8;
							buf[2] = (CHT8305_CFG_SOFT_RESET | CHT8305_CFG_MODE) & 0xff;
							send_i2c_buf(sensor_cfg.i2c_addr, buf, 3);
							pm_wait_us(CHT8305_SOFT_RESET_us);
							// Configure
//							buf[0] = CHT8305_REG_CFG;
							buf[1] = CHT8305_CFG_MODE >> 8;
							buf[2] = CHT8305_CFG_MODE & 0xff;
							send_i2c_buf(sensor_cfg.i2c_addr, buf, 3);
							pm_wait_us(CHT8305_SOFT_RESET_us);
							//sensor_cfg.sensor_type = TH_SENSOR_CHT8305;
							ptabinit = (sensor_def_cfg_t *)&def_thcoef_cht8305;
							send_i2c_byte(sensor_cfg.i2c_addr, CHT8305_REG_TMP); // start measure T/H
//							pm_wait_us(CHT8305_MEASURE_us);
//							read_sensor_cht8305();
							break;
						} else
#endif
#if USE_SENSOR_CHT8215
						if(sensor_cfg.id == ((CHT8215_VID << 16) | CHT8215_MID)) {
							//sensor_cfg.sensor_type = TH_SENSOR_CHT8215;
							if(measurement_step_time >= 5000 * CLOCK_16M_SYS_TIMER_CLK_1MS) { // > 5 sec
								buf[0] = CHT8215_REG_CRT;
								buf[1] = 0x03;
								buf[2] = 0;
								send_i2c_buf(sensor_cfg.i2c_addr, buf, 3); // Set conversion ratio 5 sec
							}
							ptabinit = (sensor_def_cfg_t *)&def_thcoef_cht8215;
							break;
						} else
#endif
							sensor_cfg.i2c_addr = 0;
					}
				} else
#endif // USE_SENSOR_CHT8305
				{
					// I2C addr 0x44..0x46
#if USE_SENSOR_SHT4X
					// SHT4x
					if(!send_i2c_byte(sensor_cfg.i2c_addr, SHT4x_SOFT_RESET)) { // Soft reset command
						sleep_us(SHT4x_SOFT_RESET_us);
						if(!send_i2c_byte(sensor_cfg.i2c_addr, SHT4x_GET_ID)) { // Get ID
							sleep_us(SHT4x_SOFT_RESET_us);
							if(read_i2c_buf(sensor_cfg.i2c_addr,  buf, 6) == 0
							&& buf[2] == sensor_crc(buf[1] ^ sensor_crc(buf[0] ^ 0xff))
							&& buf[5] == sensor_crc(buf[4] ^ sensor_crc(buf[3] ^ 0xff))
							) {
								sensor_cfg.id = (buf[3] << 24) | (buf[4] << 16) | (buf[0] << 8) | buf[1];
								ptabinit = (sensor_def_cfg_t *)&def_thcoef_sht4x;
								break;
							}
						}
					}
#endif // USE_SENSOR_SHT4X
#if USE_SENSOR_SHT30
#if USE_SENSOR_SHT4X
					if(!ptabinit)
#endif // USE_SENSOR_SHT4X
					{
						// SHT30
						if(!send_i2c_word(sensor_cfg.i2c_addr, SHT30_SOFT_RESET)) { // Soft reset command
							sleep_us(SHT30_SOFT_RESET_us);
							// read status reg
							if(!send_i2c_word(sensor_cfg.i2c_addr, SHT30_RD_STATUS)
							&& !read_i2c_buf(sensor_cfg.i2c_addr, buf, 3)
							&& buf[2] == sensor_crc(buf[1] ^ sensor_crc(buf[0] ^ 0xff))) {
								sensor_cfg.id = (0x0030 << 16) | (buf[0] << 8) | buf[1];
								ptabinit = (sensor_def_cfg_t *)&def_thcoef_sht30;
								/* if(!send_i2c_word(SHT30_HIMEASURE)) { // start measure T/H
								} */
								break;
							}
						}
					}
#endif // USE_SENSOR_SHT30
				}
				break;
			}
			test_i2c_addr += 2;
		} while(test_i2c_addr <= (SHT4x_I2C_ADDR_MAX << 1));
#endif // (USE_SENSOR_CHT8305 || USE_SENSOR_SHT4X || USE_SENSOR_SHT30)
	}
	if(ptabinit) {
		if(sensor_cfg.coef.val1_k == 0) {
			memcpy(&sensor_cfg.coef, ptabinit, sizeof(sensor_cfg.coef));
		}
#if SENSOR_SLEEP_MEASURE
		sensor_cfg.measure_timeout = ptabinit->measure_timeout;
#endif
		sensor_cfg.sensor_type = ptabinit->sensor_type;
	} else
		sensor_cfg.i2c_addr = 0;
	// no i2c sensor ? sensor_cfg.i2c_addr = 0
	return sensor_cfg.i2c_addr;
}

void init_sensor(void) {
	//scan_i2c_addr(0);
	send_i2c_byte(0, 0x06); // Reset command using the general call address
	sleep_us(SHTC3_WAKEUP_us);	// 240 us
	check_sensor();
}

_attribute_ram_code_ __attribute__((optimize("-Os")))
int read_sensor_cb(void) {
	if (sensor_cfg.i2c_addr != 0) {
#if USE_SENSOR_CHT8305
		if(sensor_cfg.sensor_type == TH_SENSOR_CHT8305) {
			if (read_sensor_cht8305()) return 1;
		} else
#endif
#if USE_SENSOR_CHT8215
		if(sensor_cfg.sensor_type == TH_SENSOR_CHT8215) {
			if (read_sensor_cht8215()) return 1;
		} else
#endif
#if USE_SENSOR_AHT20_30
		if(sensor_cfg.sensor_type == TH_SENSOR_AHT2x) {
			if (read_sensor_aht2x()) return 1;
		} else
#endif
#if (USE_SENSOR_SHT4X || USE_SENSOR_SHTC3 || USE_SENSOR_SHT30)
		if(sensor_cfg.sensor_type != TH_SENSOR_NONE) {
			if (read_sensor_sht30_shtc3_sht4x()) return 1;
		} else
#endif
		{}
	}
	check_sensor();
	return 0;
}

_attribute_ram_code_
void start_measure_sensor_deep_sleep(void) {
	if (sensor_cfg.i2c_addr != 0) {
#if USE_SENSOR_CHT8305
		if(sensor_cfg.sensor_type == TH_SENSOR_CHT8305) {
			scan_i2c_addr(sensor_cfg.i2c_addr); // wakeup?
			send_i2c_byte(sensor_cfg.i2c_addr, CHT8305_REG_TMP); // start measure T/H
		} else
#endif // USE_SENSOR_CHT8305
#if USE_SENSOR_AHT20_30
		if(sensor_cfg.sensor_type == TH_SENSOR_AHT2x) {
			start_measure_aht2x();
		} else
#endif // USE_SENSOR_AHT20_30
#if USE_SENSOR_SHTC3
		if(sensor_cfg.sensor_type == TH_SENSOR_SHTC3) {
			send_i2c_word(sensor_cfg.i2c_addr, SHTC3_WAKEUP); //	Wake-up command of the sensor
			sleep_us(SHTC3_WAKEUP_us - 5);	// 240 us
#if (DEVICE_TYPE == DEVICE_CGDK2)
			if(sensor_cfg.id)
				send_i2c_word(sensor_cfg.i2c_addr, SHTC3_MEASURE);
			else
				send_i2c_word(0, SHTC3_MEASURE_CS);
#else
			send_i2c_word(sensor_cfg.i2c_addr, SHTC3_MEASURE);
#endif
		} else
#endif	// USE_SENSOR_SHTC3
#if USE_SENSOR_SHT4X
		if(sensor_cfg.sensor_type == TH_SENSOR_SHT4x) {
			send_i2c_byte(sensor_cfg.i2c_addr, SHT4x_MEASURE_HI);
		} else
#endif // USE_SENSOR_SHT4X
#if USE_SENSOR_SHT30
		if(sensor_cfg.sensor_type == TH_SENSOR_SHT30) {
			send_i2c_word(sensor_cfg.i2c_addr, SHT30_HIMEASURE); // start measure T/H
		} else
#endif //USE_SENSOR_SHT30
		{};
	}

#if SENSOR_SLEEP_MEASURE
//	sensor_cfg.time_measure = clock_time() | 1;
#endif
	return;
}

#endif // (DEV_SERVICES & SERVICE_THS)

