#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "main.h"

void erase_mi_data(){
	uint8_t read = 0x00;//ERASE THE MI ID to prevent blocking :D
	flash_write_page(0x78000, 1, &read);
	if(read != 0xff)
	flash_erase_sector(0x78000);	
}
void init_flash(){
	erase_mi_data();
}
