/*
 * sensor_lx.c
 *
 *  Created on: 28 апр. 2026 г.
 *      Author: pvvx
 */


#include "tl_common.h"
#if (DEV_SERVICES & SERVICE_ILLUMI)
#include "drivers.h"
#include "battery.h"
#include "sensor.h"
//#include "rh.h"
#include "app.h"
//#include "flash_eep.h"

//#define USE_ILLUMI_AVERAGE_SHL 	2 // 2

#if USE_ILLUMI_AVERAGE_SHL
RAM struct {
	u32 summ;
	u16 cnt;
} illumi_summ;
#else
RAM u32 old_lx;
#endif

void read_illumi_sensor(void) {
	u32 adcvbat, adclx;
//	gpio_write(GPIO_ILLUMI_ON, ILLUMI_POEWR_ON);
	gpio_set_output_en(GPIO_ILLUMI_ON, 1);
	gpio_setup_up_down_resistor(GPIO_ILLUMI, PM_PIN_PULLUP_10K);
	check_battery();
	adcvbat = adc_average; // Ubat = adc value x4 (3.3V ~11504)
	get_adc_mv(SHL_ADC_ILLUMI);
	adclx = adc_average; // Usense = adc value x4 (3.3V ~11504)
//	gpio_write(GPIO_ILLUMI_ON, !ILLUMI_POEWR_ON);
	gpio_set_output_en(GPIO_ILLUMI_ON, 0);
	gpio_setup_up_down_resistor(GPIO_ILLUMI, PM_PIN_UP_DOWN_FLOAT);

	if(adcvbat > adclx) {
		adclx = adcvbat - adclx; // Ubat - Usense = Ur
		adclx <<= 16;
		adclx /= adcvbat; // Ub/Ur = 0..65535
		adclx *= adclx;
		adclx >>= 16;	 // 0..0xfffe
		adclx *= adclx;
		adclx >>= 13; // 0..524272
	} else
		adclx = 0;
	// max adclx = 524272
#if USE_ILLUMI_AVERAGE_SHL
	illumi_summ.summ += adclx;
	illumi_summ.cnt++;
	if(illumi_summ.cnt >= (1<<USE_ILLUMI_AVERAGE_SHL)) {
		adclx = illumi_summ.summ >> USE_ILLUMI_AVERAGE_SHL;
		illumi_summ.summ -= adclx;
		illumi_summ.cnt--;
	} else {
		adclx = illumi_summ.summ / illumi_summ.cnt;
	}
#else
	if(!old_lx)
		old_lx = adclx;
	old_lx += adclx;
	adclx = old_lx >> 1;
	old_lx -= adclx;
#endif
	measured_data.illumi = adclx;
}

#endif // (DEV_SERVICES & SERVICE_ILLUMI)
