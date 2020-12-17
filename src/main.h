/*
 * main.h
 *
 *  Created on: 15.11.2020
 *      Author: pvvx
 */

#ifndef MAIN_H_
#define MAIN_H_

void init_ble(void);
void init_lcd(void);
void init_flash(void);

void init_i2c();
void send_i2c(uint8_t device_id, const uint8_t *buffer, int dataLen);

void init_sensor(void);
void read_sensor(int16_t *temp, uint16_t *humi);

void init_lcd();
void send_to_lcd_long(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6);
void show_smiley(uint8_t state);
void show_battery_symbol(bool state);
void show_ble_symbol(bool state);
void show_temp_symbol(uint8_t symbol);
void show_number(uint8_t position,uint8_t number);
void update_lcd();
void show_atc_mac(void);
void show_small_number(uint16_t number, bool percent);
void show_big_number(int16_t number, bool point);

uint16_t get_battery_mv();
void adc_bat_init(void);
uint8_t get_battery_level(uint16_t battery_mv);

void cmd_parser(void * p);

bool ble_get_connected(void);
void ble_send_temp(uint16_t temp);
void ble_send_humi(uint16_t humi);
void ble_send_battery(uint8_t value);
void set_adv_data(int16_t temp, uint16_t humi, uint8_t battery_level, uint16_t battery_mv);
void blt_pm_proc(void);

#endif /* MAIN_H_ */
