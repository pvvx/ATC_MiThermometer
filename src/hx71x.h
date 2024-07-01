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

// Set X71X Output Data Rate 10 sps (pin RATE to "0")

#define MAX_TANK_VOLUME_10ML 32768 // 327 liters

typedef struct _hx71x_cfg_t {
	uint32_t zero;
	uint32_t coef;
	uint32_t volume_10ml; // volume in the tank when the overflow sensor is triggered (in 10 milliliters)
} hx71x_cfg_t;

typedef struct _hx71x_t {
	hx71x_cfg_t cfg;
	uint32_t adc;
	uint32_t value;
	uint32_t summator;
	uint32_t count;
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
uint16_t hx71x_get_volume(void); // in 10 milliliters
//void hx71x_suspend(void);
void hx71x_task(void);

inline void hx711_go_sleep(void) {
	gpio_setup_up_down_resistor(GPIO_HX71X_SCK, PM_PIN_PULLUP_1M);
}

inline void hx711_gpio_wakeup(void) {
	gpio_setup_up_down_resistor(GPIO_HX71X_SCK, PM_PIN_PULLDOWN_100K);
}



#endif /* HX71X_H_ */
