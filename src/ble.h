#pragma once

#include <stdbool.h>
#include <stdint.h>

extern uint8_t ota_is_working;
extern uint8_t ble_connected;
extern u8 batteryValueInCCC[2];
extern u8 tempValueInCCC[2];
extern u8 humiValueInCCC[2];
extern u8 RxTxValueInCCC[2];

extern u8 my_RxTx_Data[16];

void init_ble();
void set_adv_data(int32_t temp, uint32_t humi, uint8_t battery_level, uint32_t battery_mv);
bool ble_get_connected();
void ble_send_temp(int16_t temp);
void ble_send_humi(uint16_t humi);
void ble_send_battery(uint8_t value);
void ble_send_all(void);
void ble_send_cfg(void);
void user_set_rf_power(uint8_t e, uint8_t *p, int n);
int otaWritePre(void * p);
int RxTxWrite(void * p);
