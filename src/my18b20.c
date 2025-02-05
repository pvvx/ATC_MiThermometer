/*
 * my18b20.c
 *
 *  Created on: 2 авг. 2024 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_config.h"
#if (DEV_SERVICES & SERVICE_18B20)

#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "sensor.h"
#include "my18B20.h"

// #define GPIO_ONEWIRE	GPIO_PB1

#define MEASURE_TIMEOUT_TICK	(100 * CLOCK_16M_SYS_TIMER_CLK_1MS)
#define MIN_STEP_TICK			(495 * CLOCK_16M_SYS_TIMER_CLK_1US)

const my18b20_coef_t def_coef_my18b20 = {
		.val1_k = 409600, // temp1_k
		.val1_z = 0, // temp1_z
#if USE_SENSOR_MY18B20 == 2
		.val2_k = 409600, // temp2_k
		.val2_z = 0, // temp2_z
#endif
};

RAM my18b20_t my18b20;

static inline void onewire_pin_lo(void) {
#if USE_SENSOR_MY18B20 == 2
#if	((GPIO_ONEWIRE1 & 0xf0) == (GPIO_ONEWIRE2 & 0xf0))
	BM_CLR(reg_gpio_oen(GPIO_ONEWIRE1), (GPIO_ONEWIRE1 | GPIO_ONEWIRE2) & 0xff);
#else
	gpio_set_output_en(GPIO_ONEWIRE1, 1);
	gpio_set_output_en(GPIO_ONEWIRE2, 1);
#endif
#else
	gpio_set_output_en(GPIO_ONEWIRE1, 1);
#endif
}

static inline void onewire_pin_hi(void) {
#if USE_SENSOR_MY18B20 == 2
#if	((GPIO_ONEWIRE1 & 0xf0) == (GPIO_ONEWIRE2 & 0xf0))
	BM_SET(reg_gpio_oen(GPIO_ONEWIRE1), (GPIO_ONEWIRE1 | GPIO_ONEWIRE2) & 0xff);
#else
	gpio_set_output_en(GPIO_ONEWIRE1, 0);
	gpio_set_output_en(GPIO_ONEWIRE2, 0);
#endif
#else
	gpio_set_output_en(GPIO_ONEWIRE1, 0);
#endif
}

static inline unsigned int onewire_pin_read(void) {
	unsigned int ret = 0;
#if USE_SENSOR_MY18B20 == 2
#if	((GPIO_ONEWIRE1 & 0xf0) == (GPIO_ONEWIRE2 & 0xf0))
	unsigned char x = BM_IS_SET(reg_gpio_in(GPIO_ONEWIRE1), (GPIO_ONEWIRE1 | GPIO_ONEWIRE2) & 0xff);
	if(x & (GPIO_ONEWIRE1 & 0xff))
			ret = 1;
	if(x & (GPIO_ONEWIRE2 & 0xff))
			ret |= 2;
#else
	if(gpio_read(GPIO_ONEWIRE1))
		ret = 1;
	if(gpio_read(GPIO_ONEWIRE2))
		ret |= 2;
#endif
#else
	if(gpio_read(GPIO_ONEWIRE1))
		ret = 3;
#endif
	return ret;
}

static void onewire_bus_low(void) {
	gpio_setup_up_down_resistor(GPIO_ONEWIRE1, PM_PIN_PULLDOWN_100K);
	gpio_set_output_en(GPIO_ONEWIRE1, 1); // enable output
#if USE_SENSOR_MY18B20 == 2
	gpio_setup_up_down_resistor(GPIO_ONEWIRE2, PM_PIN_PULLDOWN_100K);
	gpio_set_output_en(GPIO_ONEWIRE2, 1); // enable output
#endif
}

static void onewire_bus_hi(void) {
	gpio_setup_up_down_resistor(GPIO_ONEWIRE1, PM_PIN_PULLUP_10K);
	gpio_set_output_en(GPIO_ONEWIRE1, 0); // disable output
#if USE_SENSOR_MY18B20 == 2
	gpio_setup_up_down_resistor(GPIO_ONEWIRE2, PM_PIN_PULLUP_10K);
	gpio_set_output_en(GPIO_ONEWIRE2, 0); // disable output
#endif
}


// return: -1 - error, 0..3 - ok
_attribute_ram_code_
int onewire_bit_read(void) {
	int ret = 0;
	unsigned char r = irq_disable();
	onewire_pin_hi();
	sleep_us(15); // 15
	if(onewire_pin_read() == 3) {
		unsigned int tt = clock_time();
		onewire_pin_lo();
		sleep_us(1); // 1..3
		onewire_pin_hi();
		sleep_us(15); // 15
		ret = onewire_pin_read();
		if(ret != 3) {
			while(onewire_pin_read() != 3) {
				if(clock_time() - tt > 65 * CLOCK_16M_SYS_TIMER_CLK_1US) {
					ret = -1;
					break;
				}
			}
		}
	} else
		ret = -1;
	irq_restore(r);
	return ret;
}

// return: -1 - error, 0 - ok
_attribute_ram_code_
int onewire_bit_write(unsigned int cbit) {
	int ret = 0;
	unsigned char r = irq_disable();
	onewire_pin_hi();
	sleep_us(15);	// 15
	if(onewire_pin_read() == 3) {
		onewire_pin_lo();
		sleep_us(1); // 1..3
		if(cbit)
			onewire_pin_hi();
		sleep_us(30); // 30
		onewire_pin_hi();
	} else
		ret = -1;
	irq_restore(r);
	return ret;
}

// return: -1 - error, 0 - ok
int onewire_tst_presence(void) {
	int ret = -1;
	onewire_bus_hi();
	sleep_us(80);	// 65..80
	if(onewire_pin_read() == 0) {
		ret = 0;
		unsigned int tt = clock_time();
		while(onewire_pin_read() != 3) {
			if(clock_time() - tt > (265-80) * CLOCK_16M_SYS_TIMER_CLK_1US) {
				ret = -1;
				break;
			}
		}
	}
	return ret;
}


// return: -1 - error, 0 - ok
int onewire_write(unsigned int value, int bitcnt) {
	int ret = 0;
	while(bitcnt--) {
		ret = onewire_bit_write(value & 1);
		value >>= 1;
		if(ret < 0) {
			break;
		}
	}
	return ret;
}

// return: -1 - error, 0 - ok
int onewire_16bit_read(short *pdata) {
	int ret = 0;
	unsigned int mask = 1;
	while(mask < 0x10000) {
		int cbit = onewire_bit_read();
		if(cbit < 0) {
			ret = -1;
			break;
		} else {
#if USE_SENSOR_MY18B20 == 2
			if(cbit & 1)
				pdata[0] |= mask;
			else
				pdata[0] &= ~mask;
			if(cbit & 2)
				pdata[1] |= mask;
			else
				pdata[1] &= ~mask;
#else
			if(cbit)
				pdata[0] |= mask;
			else
				pdata[0] &= ~mask;
#endif
		}
		mask <<= 1;
	}
	return ret;
}

#if USE_SENSOR_MY18B20 == 1

int onewire_read(unsigned int bitcnt) {
	int ret = 0;
	unsigned int mask = 1;
	while(bitcnt--) {
		int cbit = onewire_bit_read();
		if(cbit < 0) {
			ret = -1;
			break;
		} else
			if(cbit)
				ret |= mask;
		mask <<= 1;
	}
	return ret;
}

#endif

void delay_us(unsigned int us) {
	if(wrk.ble_connected)
		sleep_us(us);
	else
		pm_wait_us(us);
}

void init_my18b20(void) {
	my18b20_coef_t * ptabinit = (my18b20_coef_t *)&def_coef_my18b20;
	onewire_bus_low();
	my18b20.type =
#if USE_SENSOR_MY18B20 == 2
	 IU_SENSOR_MY18B20x2;
#else
	 IU_SENSOR_MY18B20;
#endif
	my18b20.stage = 0; // init
	my18b20.rd_ok = 0;
	my18b20.timeout = MIN_STEP_TICK;
	if(my18b20.coef.val1_k == 0) {
		memcpy(&my18b20.coef, ptabinit, sizeof(my18b20.coef));
	}
	//my18b20.cfg.sensor_type = ptabinit->sensor_type;
	delay_us(495);
	if(onewire_tst_presence() >= 0
		&& onewire_write(0x033, 8) >= 0) {
#if USE_SENSOR_MY18B20 == 2
		onewire_16bit_read((short *)&my18b20.id);
#else
		my18b20.id = onewire_read(32);
#endif
	}
	onewire_bus_low();
	my18b20.tick = clock_time();
}

void task_my18b20(void) {
	if(clock_time() - my18b20.tick > my18b20.timeout) {
		switch(my18b20.stage) {
		case 1:
			if(wrk.start_measure) {
				// start measure, measure, read measure: ~ 4 ms
				if(onewire_tst_presence() >= 0
					&& onewire_write(0x044cc, 16) >= 0) { // start measure
					// reset bus -> presence
					my18b20.stage = 2;
					my18b20.timeout = MEASURE_TIMEOUT_TICK;
				} else {
					my18b20.stage = 0;
					my18b20.timeout = MIN_STEP_TICK;
				}
				onewire_bus_low();
			}
			break;
		case 2:
				if(onewire_tst_presence() >= 0
					&& onewire_write(0x0becc, 16) >= 0 // cmd read
					&& onewire_16bit_read(my18b20.temp) >= 0) { // read measure
					measured_data.xtemp[0] = ((int)(my18b20.temp[0] * my18b20.coef.val1_k) >> 16) + my18b20.coef.val1_z; // x 0.01 C
#if USE_SENSOR_MY18B20 == 2
					measured_data.xtemp[1] = ((int)(my18b20.temp[1] * my18b20.coef.val2_k) >> 16) + my18b20.coef.val2_z; // x 0.01 C
#endif
					my18b20.rd_ok = 0xff;
					measured_data.count++;
					my18b20.tick = clock_time() - (MIN_STEP_TICK + 100 * CLOCK_16M_SYS_TIMER_CLK_1MS);
					my18b20.stage = 1;
				} else {
					//my18b20.rd_ok = 0;
					my18b20.stage = 0;
				}
				onewire_bus_low();
				my18b20.timeout = MIN_STEP_TICK;
				break;
		default:
			// init config reg: ~ 2.5 ms
			if(onewire_tst_presence() >= 0
				&& onewire_write(0x0cc, 8) >= 0 // no addr
				&& onewire_write(0x7f00ff4e, 32) >= 0) { // init config reg
				my18b20.stage = 1;
			}
			onewire_bus_low();
			my18b20.timeout = MIN_STEP_TICK;
		}
		my18b20.tick = clock_time();
	}
}

#endif // USE_SENSOR_MY18B20
