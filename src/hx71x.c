/*
 * hx71x.c
 *
 *  Created on: 24.12.2019
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_config.h"
#if USE_SENSOR_HX71X
#include "app.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "hx71x.h"

RAM hx71x_t hx71x;

hx71x_cfg_t def_hx71x_cfg = {
	.zero = 2079150000, // 0x7BED4FB0
	.coef = 56450, // ed. ADC in 10 milliliters: (2728325504-2079150000)/11500 = 0xDC82
	.volume_10ml = 11500 // in 10 milliliters -> 115 liters
};
// set config: 49b04fed7b82dC0000ec2c0000  // b04fed7b82dc0000ec2c0000806721a0

// HX71XMODE_A128 - Period: 94 ms, Pulse (1): 81.5 us
_attribute_ram_code_
int hx71x_get_data(hx71x_mode_t mode) {
		u32 i = mode;
		u32 x = 0x100 >> (mode & 3);

		u8 tmp_s = GPIO_HX71X_SCK & 0xff;
		u32 pcClkReg = (0x583+((GPIO_HX71X_SCK>>8)<<3)); // reg_gpio_out() register GPIO output


		write_reg8(pcClkReg, read_reg8(pcClkReg) & (~tmp_s)); // preset PD_SCK output 0

		reg_gpio_oen(GPIO_HX71X_SCK) &= ~tmp_s; // Enable PD_SCK output

		u8 tmp_d = GPIO_HX71X_DOUT & 0xff;
//		reg_gpio_ie(GPIO_HX71X_DOUT) |= tmp_d; // Enable PD_DOUT input (see "app_config.h")

		u32 pcRxReg = (0x580+((GPIO_HX71X_DOUT>>8)<<3)); // reg_gpio_in() register GPIO input


//		sleep_us(1);

		// HX71x выдает сигнал занятости при каждом авто-измерении 10 или 40Hz.
		// Пока идет измерение - считываемые данные не верны!
		// Решение только одно - читать сразу по фронту готовности (set SENSOR_HX71X_WAKEAP = 1), пока идет пауза до следующего измерения
		// Иначе будут сбои в показаниях, которые не отследить

		u32 dout = 0;

		u8 r = irq_disable();

		if(read_reg8(pcRxReg) & tmp_d)
				x |= 1;
		while(i--) {
			sleep_us(1);
			write_reg8(pcClkReg, read_reg8(pcClkReg) | tmp_s);	// PD_SCK set "1"
			sleep_us(1);
			dout <<= 1;
			if(read_reg8(pcRxReg) & tmp_d)
				dout |= x;
			write_reg8(pcClkReg, read_reg8(pcClkReg) & (~tmp_s)); // PD_SCK set "0"
		}
		reg_gpio_oen(GPIO_HX71X_SCK) |= tmp_s; // Disable PD_SCK output
//		reg_gpio_ie(GPIO_HX71X_DOUT) &= ~tmp_d; // Disable PD_DOUT input

		irq_restore(r);

		return dout;
}


_attribute_ram_code_
u16 hx71x_get_volume(void) { // in 10 milliliters
	u16 value;
	if(hx71x.count > 1) {
		value = (u16)((u32)(hx71x.summator / hx71x.count));
		hx71x.count = 1;
		hx71x.summator = value;
	} else
		value = (u16)hx71x.summator;
	return value;
}

// volume in the tank when the overflow sensor is triggered (in 10 milliliters)
void hx71x_calibration(void) {
	if(hx71x.cfg.volume_10ml && hx71x.value) {
		u32 coef = hx71x.value / hx71x.cfg.volume_10ml;
		u32 delta = hx71x.cfg.coef >> 4; // div 16 -> 6.25%
		if(coef < hx71x.cfg.coef + delta && coef > hx71x.cfg.coef - delta) {
			if(!hx71x.calcoef)
				hx71x.calcoef = coef;
			hx71x.calcoef += coef;
			hx71x.calcoef >>= 1;
			hx71x.cfg.coef = hx71x.calcoef;
		}
	}
}

_attribute_ram_code_
void hx71x_task(void) {
	u32 value;
	if(BM_IS_SET(reg_gpio_in(GPIO_HX71X_DOUT), GPIO_HX71X_DOUT & 0xff) == 0) {
		// HX71X_DOUT = "0"
		value = hx71x_get_data(HX71XMODE_A128);
		if((value & 1) == 0) {
			value += 0x80000000;
			hx71x.adc = value;
			if(value > hx71x.cfg.zero) {
				value -= hx71x.cfg.zero;
				hx71x.value = value;
				value /= hx71x.cfg.coef; // in 10 milliliters
				if(value > MAX_TANK_VOLUME_10ML)
					value = MAX_TANK_VOLUME_10ML-1;
			} else {
				value = 0;
				hx71x.value = value;
			}
			hx71x.summator += value;
			hx71x.count++;
		}
	}
}

/*
_attribute_ram_code_
void hx71x_suspend(void) {
	cpu_set_gpio_wakeup(GPIO_HX71X_DOUT, Level_Low, 1);  // pad wakeup deepsleep enable
	bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
}
*/

#endif // USE_SENSOR_HX71X

