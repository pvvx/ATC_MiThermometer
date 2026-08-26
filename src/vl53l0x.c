/*
 * vl53l0x.c
 *
 *  Created on: 21 авг. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"

#if 1 // USE_SENSOR_VL53L0X

#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"


#define VL53L0X_I2C_ADDR		0x29

// register addresses from API vl53l0x_device.h (ordered as listed there)
typedef enum {
	SYSRANGE_START = 0x00,

	SYSTEM_THRESH_HIGH = 0x0C,
	SYSTEM_THRESH_LOW = 0x0E,

	SYSTEM_SEQUENCE_CONFIG = 0x01,
	SYSTEM_RANGE_CONFIG = 0x09,
	SYSTEM_INTERMEASUREMENT_PERIOD = 0x04,

	SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0A,

	GPIO_HV_MUX_ACTIVE_HIGH = 0x84,

	SYSTEM_INTERRUPT_CLEAR = 0x0B,

	RESULT_INTERRUPT_STATUS = 0x13,
	RESULT_RANGE_STATUS = 0x14,

	RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN = 0xBC,
	RESULT_CORE_RANGING_TOTAL_EVENTS_RTN = 0xC0,
	RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF = 0xD0,
	RESULT_CORE_RANGING_TOTAL_EVENTS_REF = 0xD4,
	RESULT_PEAK_SIGNAL_RATE_REF = 0xB6,

	ALGO_PART_TO_PART_RANGE_OFFSET_MM = 0x28,

	I2C_SLAVE_DEVICE_ADDRESS = 0x8A,

	MSRC_CONFIG_CONTROL = 0x60,

	PRE_RANGE_CONFIG_MIN_SNR = 0x27,
	PRE_RANGE_CONFIG_VALID_PHASE_LOW = 0x56,
	PRE_RANGE_CONFIG_VALID_PHASE_HIGH = 0x57,
	PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT = 0x64,

	FINAL_RANGE_CONFIG_MIN_SNR = 0x67,
	FINAL_RANGE_CONFIG_VALID_PHASE_LOW = 0x47,
	FINAL_RANGE_CONFIG_VALID_PHASE_HIGH = 0x48,
	FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,

	PRE_RANGE_CONFIG_SIGMA_THRESH_HI = 0x61,
	PRE_RANGE_CONFIG_SIGMA_THRESH_LO = 0x62,

	PRE_RANGE_CONFIG_VCSEL_PERIOD = 0x50,
	PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x51,
	PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x52,

	SYSTEM_HISTOGRAM_BIN = 0x81,
	HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT = 0x33,
	HISTOGRAM_CONFIG_READOUT_CTRL = 0x55,

	FINAL_RANGE_CONFIG_VCSEL_PERIOD = 0x70,
	FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x71,
	FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x72,
	CROSSTALK_COMPENSATION_PEAK_RATE_MCPS = 0x20,

	MSRC_CONFIG_TIMEOUT_MACROP = 0x46,

	SOFT_RESET_GO2_SOFT_RESET_N = 0xBF,
	IDENTIFICATION_MODEL_ID = 0xC0,
	IDENTIFICATION_REVISION_ID = 0xC2,

	OSC_CALIBRATE_VAL = 0xF8,

	GLOBAL_CONFIG_VCSEL_WIDTH = 0x32,
	GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = 0xB0,
	GLOBAL_CONFIG_SPAD_ENABLES_REF_1 = 0xB1,
	GLOBAL_CONFIG_SPAD_ENABLES_REF_2 = 0xB2,
	GLOBAL_CONFIG_SPAD_ENABLES_REF_3 = 0xB3,
	GLOBAL_CONFIG_SPAD_ENABLES_REF_4 = 0xB4,
	GLOBAL_CONFIG_SPAD_ENABLES_REF_5 = 0xB5,

	GLOBAL_CONFIG_REF_EN_START_SELECT = 0xB6,
	DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = 0x4E,
	DYNAMIC_SPAD_REF_EN_START_OFFSET = 0x4F,
	POWER_MANAGEMENT_GO1_POWER_FORCE = 0x80,

	VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = 0x89,

	ALGO_PHASECAL_LIM = 0x30,
	ALGO_PHASECAL_CONFIG_TIMEOUT = 0x30,
} regVL53L0X_e;

typedef struct {
	u8 mode;
	u8 addr;
	u8 val;
}rw_regs_t;

typedef enum {
	RW_MODE_WR = 0,
	RW_MODE_OR = 1,
	RW_MODE_AND = 2,
	RW_MODE_SAVE = 3,
	RW_MODE_CAL = 4,
	RW_MODE_END = 0xff
} RW_MODE_e;

#define CALIBRATION_TIMEOUT (32*CLOCK_16M_SYS_TIMER_CLK_1MS)

int performSingleRefCalibration(u8 vhv_init_byte) {
	u8 flg;
	u32 tt = clock_time();
	if (send_i2c_byte(sensor_cfg.i2c_addr, SYSRANGE_START, 0x01 | vhv_init_byte))
		return 1;
	do {
		if (read_i2c_byte_addr(sensor_cfg.i2c_addr, RESULT_INTERRUPT_STATUS, &flg, 1))
			return 1;
		if(clock_time() - tt > CALIBRATION_TIMEOUT)
			return 2;
	} while((flg & 0x07) == 0);
	if (send_i2c_byte(sensor_cfg.i2c_addr, SYSTEM_INTERRUPT_CLEAR, 0x01))
		return 1;
	return send_i2c_byte(sensor_cfg.i2c_addr, SYSRANGE_START, 0x00);
}


const rw_regs_t tab_init1 = {
		// sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
		{RW_MODE_OR, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, 1},
		// "Set I2C standard mode"
		{RW_MODE_WR, 0x88, 0x00},
		{RW_MODE_WR, 0x80, 0x01},
		{RW_MODE_WR, 0xff, 0x01},
		{RW_MODE_WR, 0x00, 0x00},
		{RW_MODE_SAVE, 0x91, 0},
		{RW_MODE_WR, 0x00, 0x01},
		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x80, 0x00},
		// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
		{RW_MODE_OR, MSRC_CONFIG_CONTROL, 0x12},
		// set final range signal rate limit to 0.25 MCPS (million counts per second)
		{RW_MODE_WR, FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, 32}, // 0.25 * 128
		{RW_MODE_WR, SYSTEM_SEQUENCE_CONFIG, 0xFF},
		// ???
		// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)
		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00},
		{RW_MODE_WR, DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C},
		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4},
		// ???
		// -- VL53L0X_load_tuning_settings()
		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x00, 0x00},

		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x09, 0x00},
		{RW_MODE_WR, 0x10, 0x00},
		{RW_MODE_WR, 0x11, 0x00},

		{RW_MODE_WR, 0x24, 0x01},
		{RW_MODE_WR, 0x25, 0xFF},
		{RW_MODE_WR, 0x75, 0x00},

		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x4E, 0x2C},
		{RW_MODE_WR, 0x48, 0x00},
		{RW_MODE_WR, 0x30, 0x20},

		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x30, 0x09},
		{RW_MODE_WR, 0x54, 0x00},
		{RW_MODE_WR, 0x31, 0x04},
		{RW_MODE_WR, 0x32, 0x03},
		{RW_MODE_WR, 0x40, 0x83},
		{RW_MODE_WR, 0x46, 0x25},
		{RW_MODE_WR, 0x60, 0x00},
		{RW_MODE_WR, 0x27, 0x00},
		{RW_MODE_WR, 0x50, 0x06},
		{RW_MODE_WR, 0x51, 0x00},
		{RW_MODE_WR, 0x52, 0x96},
		{RW_MODE_WR, 0x56, 0x08},
		{RW_MODE_WR, 0x57, 0x30},
		{RW_MODE_WR, 0x61, 0x00},
		{RW_MODE_WR, 0x62, 0x00},
		{RW_MODE_WR, 0x64, 0x00},
		{RW_MODE_WR, 0x65, 0x00},
		{RW_MODE_WR, 0x66, 0xA0},

		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x22, 0x32},
		{RW_MODE_WR, 0x47, 0x14},
		{RW_MODE_WR, 0x49, 0xFF},
		{RW_MODE_WR, 0x4A, 0x00},

		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x7A, 0x0A},
		{RW_MODE_WR, 0x7B, 0x00},
		{RW_MODE_WR, 0x78, 0x21},

		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x23, 0x34},
		{RW_MODE_WR, 0x42, 0x00},
		{RW_MODE_WR, 0x44, 0xFF},
		{RW_MODE_WR, 0x45, 0x26},
		{RW_MODE_WR, 0x46, 0x05},
		{RW_MODE_WR, 0x40, 0x40},
		{RW_MODE_WR, 0x0E, 0x06},
		{RW_MODE_WR, 0x20, 0x1A},
		{RW_MODE_WR, 0x43, 0x40},

		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x34, 0x03},
		{RW_MODE_WR, 0x35, 0x44},

		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x31, 0x04},
		{RW_MODE_WR, 0x4B, 0x09},
		{RW_MODE_WR, 0x4C, 0x05},
		{RW_MODE_WR, 0x4D, 0x04},

		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x44, 0x00},
		{RW_MODE_WR, 0x45, 0x20},
		{RW_MODE_WR, 0x47, 0x08},
		{RW_MODE_WR, 0x48, 0x28},
		{RW_MODE_WR, 0x67, 0x00},
		{RW_MODE_WR, 0x70, 0x04},
		{RW_MODE_WR, 0x71, 0x01},
		{RW_MODE_WR, 0x72, 0xFE},
		{RW_MODE_WR, 0x76, 0x00},
		{RW_MODE_WR, 0x77, 0x00},

		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x0D, 0x01},

		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x80, 0x01},
		{RW_MODE_WR, 0x01, 0xF8},

		{RW_MODE_WR, 0xFF, 0x01},
		{RW_MODE_WR, 0x8E, 0x01},
		{RW_MODE_WR, 0x00, 0x01},
		{RW_MODE_WR, 0xFF, 0x00},
		{RW_MODE_WR, 0x80, 0x00},

		// -- VL53L0X_SetGpioConfig()
		{RW_MODE_WR, SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04},
		{RW_MODE_AND, GPIO_HV_MUX_ACTIVE_HIGH, 0xEF}, // active low
		{RW_MODE_WR, SYSTEM_INTERRUPT_CLEAR, 0x01},
		
        // -- VL53L0X_SetSequenceStepEnable()
		{RW_MODE_WR, SYSTEM_SEQUENCE_CONFIG, 0xE8},
		// -- VL53L0X_perform_vhv_calibration()
		{RW_MODE_WR, SYSTEM_SEQUENCE_CONFIG, 0x01},
		{RW_MODE_CAL, 0xff, 0x40},
		// -- VL53L0X_perform_phase_calibration()
		{RW_MODE_WR, SYSTEM_SEQUENCE_CONFIG, 0x02},
		{RW_MODE_CAL, 0xff, 0x00},

		// "restore the previous Sequence Config"
		{RW_MODE_WR, SYSTEM_SEQUENCE_CONFIG, 0xE8},
		// end
		{RW_MODE_END, 0xff, 0xff}
};

int rw_sensor(rw_regs_t * pwrktab, u8 *psave) {
	int ret = 0;
	u8 b;
	do {
		switch(pwrktab->mode) {
		case RW_MODE_WR:
			ret = send_i2c_byte(sensor_cfg.i2c_addr, pwrktab->addr, pwrktab->val);
			break;
		case RW_MODE_OR:
			ret = read_i2c_byte_addr(sensor_cfg.i2c_addr, pwrktab->addr, &b, 1);
			if(!ret) {
				b |= pwrktab->val;
				ret = send_i2c_byte(sensor_cfg.i2c_addr, pwrktab->addr, b);
			}
			break;
		case RW_MODE_AND:
			ret = read_i2c_byte_addr(sensor_cfg.i2c_addr, pwrktab->addr, &b, 1);
			if(!ret) {
				b &= pwrktab->val;
				ret = send_i2c_byte(sensor_cfg.i2c_addr, pwrktab->addr, b);
			}
			break;
		case RW_MODE_SAVE:
			ret = read_i2c_byte_addr(sensor_cfg.i2c_addr, pwrktab->addr, psave[pwrktab->val], 1);
			break;
		case RW_MODE_CAL:
			ret = performSingleRefCalibration(pwrktab->val);
			break;
		default:
			return 0;
		}
		pwrktab++;
	} while(!ret);
	return ret;
}


void init_sensor(void) {
	u32 tmp;
	sensor_cfg.id = 0;
	sensor_cfg.sensor_type = ID_SENSOR_NONE;
	sensor_cfg.i2c_addr = (u8) scan_i2c_addr(VL53L0X_I2C_ADDR << 1);
	if (sensor_cfg.i2c_addr) {
		do {
			// check model ID register
			if (read_i2c_byte_addr(sensor_cfg.i2c_addr, IDENTIFICATION_MODEL_ID, &sensor_cfg.id, 4))
				break;
			if (read_i2c_byte_addr(sensor_cfg.i2c_addr, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, &tmp, 1))
				break;
			tmp |= 1;
			if (send_i2c_byte(sensor_cfg.i2c_addr, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, tmp))
				break;
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0x88, 0x00))
				break;
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0x80, 0x01))
				break;
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0xff, 0x01))
				break;
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0x00, 0x00))
				break;
			if (read_i2c_byte_addr(sensor_cfg.i2c_addr, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, &sensor_cfg.stp, 1))
				break;
			// "Set I2C standard mode"
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0x00, 0x01))
				break;
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0xff, 0x00))
				break;
			if (send_i2c_byte(sensor_cfg.i2c_addr, 0x80, 0x01))
				break;
			// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
			writeReg(MSRC_CONFIG_CONTROL, readReg(MSRC_CONFIG_CONTROL) | 0x12);

			// set final range signal rate limit to 0.25 MCPS (million counts per second)
			setSignalRateLimit(0.25);

			writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);

		} while(0);
	}
}


bool VL53L0X_init(bool io_2v8)
{
  // check model ID register (value specified in datasheet)
  if (readReg(IDENTIFICATION_MODEL_ID) != 0xEE) { return false; }

  // VL53L0X_DataInit() begin

  // sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
  if (io_2v8)
  {
    writeReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
      readReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01); // set bit 0
  }

  // "Set I2C standard mode"
  writeReg(0x88, 0x00);

  writeReg(0x80, 0x01);
  writeReg(0xFF, 0x01);
  writeReg(0x00, 0x00);
  stop_variable = readReg(0x91);
  writeReg(0x00, 0x01);
  writeReg(0xFF, 0x00);
  writeReg(0x80, 0x00);

  // disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
  writeReg(MSRC_CONFIG_CONTROL, readReg(MSRC_CONFIG_CONTROL) | 0x12);

  // set final range signal rate limit to 0.25 MCPS (million counts per second)
  setSignalRateLimit(0.25);

  writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);

  // VL53L0X_DataInit() end

  // VL53L0X_StaticInit() begin

  uint8_t spad_count;
  bool spad_type_is_aperture;
  if (!getSpadInfo(&spad_count, &spad_type_is_aperture)) { return false; }

  // The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
  // the API, but the same data seems to be more easily readable from
  // GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
  uint8_t ref_spad_map[6];
  readMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

  // -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

  writeReg(0xFF, 0x01);
  writeReg(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
  writeReg(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
  writeReg(0xFF, 0x00);
  writeReg(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

  uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
  uint8_t spads_enabled = 0;

  for (uint8_t i = 0; i < 48; i++)
  {
    if (i < first_spad_to_enable || spads_enabled == spad_count)
    {
      // This bit is lower than the first one that should be enabled, or
      // (reference_spad_count) bits have already been enabled, so zero this bit
      ref_spad_map[i / 8] &= ~(1 << (i % 8));
    }
    else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
    {
      spads_enabled++;
    }
  }

  writeMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

  // -- VL53L0X_set_reference_spads() end

  // -- VL53L0X_load_tuning_settings() begin
  // DefaultTuningSettings from vl53l0x_tuning.h

  writeReg(0xFF, 0x01);
  writeReg(0x00, 0x00);

  writeReg(0xFF, 0x00);
  writeReg(0x09, 0x00);
  writeReg(0x10, 0x00);
  writeReg(0x11, 0x00);

  writeReg(0x24, 0x01);
  writeReg(0x25, 0xFF);
  writeReg(0x75, 0x00);

  writeReg(0xFF, 0x01);
  writeReg(0x4E, 0x2C);
  writeReg(0x48, 0x00);
  writeReg(0x30, 0x20);

  writeReg(0xFF, 0x00);
  writeReg(0x30, 0x09);
  writeReg(0x54, 0x00);
  writeReg(0x31, 0x04);
  writeReg(0x32, 0x03);
  writeReg(0x40, 0x83);
  writeReg(0x46, 0x25);
  writeReg(0x60, 0x00);
  writeReg(0x27, 0x00);
  writeReg(0x50, 0x06);
  writeReg(0x51, 0x00);
  writeReg(0x52, 0x96);
  writeReg(0x56, 0x08);
  writeReg(0x57, 0x30);
  writeReg(0x61, 0x00);
  writeReg(0x62, 0x00);
  writeReg(0x64, 0x00);
  writeReg(0x65, 0x00);
  writeReg(0x66, 0xA0);

  writeReg(0xFF, 0x01);
  writeReg(0x22, 0x32);
  writeReg(0x47, 0x14);
  writeReg(0x49, 0xFF);
  writeReg(0x4A, 0x00);

  writeReg(0xFF, 0x00);
  writeReg(0x7A, 0x0A);
  writeReg(0x7B, 0x00);
  writeReg(0x78, 0x21);

  writeReg(0xFF, 0x01);
  writeReg(0x23, 0x34);
  writeReg(0x42, 0x00);
  writeReg(0x44, 0xFF);
  writeReg(0x45, 0x26);
  writeReg(0x46, 0x05);
  writeReg(0x40, 0x40);
  writeReg(0x0E, 0x06);
  writeReg(0x20, 0x1A);
  writeReg(0x43, 0x40);

  writeReg(0xFF, 0x00);
  writeReg(0x34, 0x03);
  writeReg(0x35, 0x44);

  writeReg(0xFF, 0x01);
  writeReg(0x31, 0x04);
  writeReg(0x4B, 0x09);
  writeReg(0x4C, 0x05);
  writeReg(0x4D, 0x04);

  writeReg(0xFF, 0x00);
  writeReg(0x44, 0x00);
  writeReg(0x45, 0x20);
  writeReg(0x47, 0x08);
  writeReg(0x48, 0x28);
  writeReg(0x67, 0x00);
  writeReg(0x70, 0x04);
  writeReg(0x71, 0x01);
  writeReg(0x72, 0xFE);
  writeReg(0x76, 0x00);
  writeReg(0x77, 0x00);

  writeReg(0xFF, 0x01);
  writeReg(0x0D, 0x01);

  writeReg(0xFF, 0x00);
  writeReg(0x80, 0x01);
  writeReg(0x01, 0xF8);

  writeReg(0xFF, 0x01);
  writeReg(0x8E, 0x01);
  writeReg(0x00, 0x01);
  writeReg(0xFF, 0x00);
  writeReg(0x80, 0x00);

  // -- VL53L0X_load_tuning_settings() end

  // "Set interrupt config to new sample ready"
  // -- VL53L0X_SetGpioConfig() begin

  writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
  writeReg(GPIO_HV_MUX_ACTIVE_HIGH, readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
  writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);

  // -- VL53L0X_SetGpioConfig() end

  measurement_timing_budget_us = getMeasurementTimingBudget();

  // "Disable MSRC and TCC by default"
  // MSRC = Minimum Signal Rate Check
  // TCC = Target CentreCheck
  // -- VL53L0X_SetSequenceStepEnable() begin

  writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

  // -- VL53L0X_SetSequenceStepEnable() end

  // "Recalculate timing budget"
  setMeasurementTimingBudget(measurement_timing_budget_us);

  // VL53L0X_StaticInit() end

  // VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

  // -- VL53L0X_perform_vhv_calibration() begin

  writeReg(SYSTEM_SEQUENCE_CONFIG, 0x01);
  if (!performSingleRefCalibration(0x40)) { return false; }

  // -- VL53L0X_perform_vhv_calibration() end

  // -- VL53L0X_perform_phase_calibration() begin

  writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02);
  if (!performSingleRefCalibration(0x00)) { return false; }

  // -- VL53L0X_perform_phase_calibration() end

  // "restore the previous Sequence Config"
  writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

  // VL53L0X_PerformRefCalibration() end

  return true;
}

#endif // USE_SENSOR_VL53L0X
