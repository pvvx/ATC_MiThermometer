/*
 * hx71X.h
 *  For HX711/HX710
 *  Created on: 26.12.2019
 *      Author: pvvx
 */

#ifndef HX71X_H_
#define HX71X_H_

#ifndef USE_SENSOR_HX71X
#define USE_SENSOR_HX71X	0
#endif // USE_SENSOR_HX71X

#if USE_SENSOR_HX71X
// Set X71X Output Data Rate 10 sps (pin RATE to "0")

#define MAX_TANK_VOLUME_10ML 32768 // 327 liters

#define SENSOR_HX71X_WAKEAP		1 // =0 - wakeap  HX71X disable (LOW POWER), =1 - wakeap enable (HX71X read measure: 10 Hz)
		// HX71x выдает сигнал занятости при каждом авто-измерении 10 или 40Hz.
		// Пока идет измерение - считываемые данные не верны!
		// Решение только одно - читать сразу по фронту готовности (set SENSOR_HX71X_WAKEAP = 1), пока идет пауза до следующего измерения
		// Иначе будут сбои в показаниях, которые не отследить

typedef struct _hx71x_cfg_t {
	u32 zero;
	u32 coef;
	u32 volume_10ml; // volume in the tank when the overflow sensor is triggered (in 10 milliliters)
} hx71x_cfg_t;

typedef struct _hx71x_t {
	hx71x_cfg_t cfg;
	u32 adc;
	u32 value;
	u32 summator;
	u32 count;
	u32 calcoef;
} hx71x_t;


typedef enum {
	HX71XMODE_A128 = 25,
	HX71XMODE_B32,
	HX71XMODE_A64
} hx71x_mode_t;


extern hx71x_t hx71x;

extern hx71x_cfg_t def_hx71x_cfg;

/*
 * HX71XMODE_A128 - Period: 94 ms, Pulse (1): 81.5 us
 */
int hx71x_get_data(hx71x_mode_t mode);
void hx71x_calibration(void);
u16 hx71x_get_volume(void); // in 10 milliliters
// void hx71x_suspend(void);
void hx71x_task(void);

inline void hx711_go_sleep(void) {
	gpio_setup_up_down_resistor(GPIO_HX71X_SCK, PM_PIN_PULLUP_1M);
}

inline void hx711_gpio_wakeup(void) {
	gpio_setup_up_down_resistor(GPIO_HX71X_SCK, PM_PIN_PULLDOWN_100K);
}

#endif // USE_SENSOR_HX71X

#endif /* HX71X_H_ */
