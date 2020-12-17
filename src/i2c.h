#pragma once

#include <stdint.h>

void init_i2c();
void send_i2c(uint8_t device_id, uint8_t *buffer, int dataLen);

