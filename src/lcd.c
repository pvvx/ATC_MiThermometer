#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"

const uint8_t lcd_init_cmd[] = {0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
RAM uint8_t display_buff[6];
RAM uint8_t display_cmp_buff[6];
/* t,H,h,L,o,i  0xe2,0x67,0x66,0xe0,0xC6,0x40 */
/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const uint8_t display_numbers[] = {0xf5,0x05,0xd3,0x97,0x27,0xb6,0xf6,0x15,0xf7,0xb7, 0x77,0xe6,0xf0,0xc7,0xf2,0x72 };

const uint8_t lcd_ota_img[6] = {1,1,0x57,1,1,1};

void lcd_ota(void) {
	memcpy(&display_buff, &lcd_ota_img, sizeof(display_buff));
	update_lcd();
}

void init_lcd(void){
	gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K); // LCD on low temp needs this, its an unknown pin going to the LCD controller chip
	
	pm_wait_ms(50);
	
	send_i2c(0x78,(uint8_t *) lcd_init_cmd, sizeof(lcd_init_cmd));
	send_to_lcd_long(0x00,0x00,0x00,0x00,0x00,0x00);	
}
	
_attribute_ram_code_ void send_to_lcd_long(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6){
    uint8_t lcd_set_segments[] =    {0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6,0xC0,0x00,0xC0,0x00};
	send_i2c(0x78,lcd_set_segments, sizeof(lcd_set_segments));
}

_attribute_ram_code_ void send_to_lcd(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6){
    uint8_t lcd_set_segments[] =    {0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6};
	send_i2c(0x78,lcd_set_segments, sizeof(lcd_set_segments));
}

_attribute_ram_code_ void update_lcd(){
	if(memcmp(&display_cmp_buff, &display_buff, sizeof(display_buff)))
		send_to_lcd(display_buff[0],display_buff[1],display_buff[2],display_buff[3],display_buff[4],display_buff[5]);
	memcpy(&display_cmp_buff, &display_buff, sizeof(display_buff));
}
/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_attribute_ram_code_ void show_temp_symbol(uint8_t symbol) {
	display_buff[2] &= ~0xE0;
	display_buff[2] |= symbol & 0xE0;
}
/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */
_attribute_ram_code_ void show_smiley(uint8_t state){
	display_buff[2] &= ~0x07;
	display_buff[2] |= state & 0x07;
}

_attribute_ram_code_ void show_ble_symbol(bool state){
	if(state)
		display_buff[2] |= 0x10;
	else 
		display_buff[2] &= ~0x10;
}

_attribute_ram_code_ void show_battery_symbol(bool state){
	if(state)
		display_buff[1] |= 0x08;
	else 
		display_buff[1] &= ~0x08;
}

/* x0.1 (-995..19995) Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_ void show_big_number(int16_t number){
//	display_buff[4] = point?0x08:0x00;
	if(number > 19995) {
   		display_buff[3] = 0;
   		display_buff[4] = 0x40; // "i"
   		display_buff[5] = 0x67; // "H"
	} else if(number < -995) {
   		display_buff[3] = 0;
   		display_buff[4] = 0xc6; // "o"
   		display_buff[5] = 0xe0; // "L"
	} else {
		display_buff[5] = 0;
		/* number: -995..19995 */
		if(number > 1995 || number < -95) {
			display_buff[4] = 0; // no point, show: -99..1999
			if(number < 0){
				number = -number;
				display_buff[5] = 2; // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[4] = 0x08; // point,
			if(number < 0){
				number = -number;
				display_buff[5] = 2; // "-"
			}
		}
		/* number: -99..1999 */
		if(number > 999) display_buff[5] |= 0x08; // "1" 1000..1999
		if(number > 99) display_buff[5] |= display_numbers[number / 100 % 10];
		if(number > 9) display_buff[4] |= display_numbers[number / 10 % 10];
		else display_buff[4] |= 0xF5; // "0"
	    display_buff[3] = display_numbers[number %10];
	}
}

/* -9 .. 99 */
_attribute_ram_code_ void show_small_number(int16_t number, bool percent){
	display_buff[1] = display_buff[1] & 0x08; // and battery
	display_buff[0] = percent?0x08:0x00;
	if(number > 99) {
		display_buff[0] |= 0x40; // "i"
		display_buff[1] |= 0x67; // "H"
	} else if(number < -9) {
		display_buff[0] |= 0xc6; // "o"
		display_buff[1] |= 0xe0; // "L"
	} else {
		if(number < 0) {
			number = -number;
			display_buff[1] = 2; // "-"
		}
		if(number > 9) display_buff[1] |= display_numbers[number / 10 % 10];
		display_buff[0] |= display_numbers[number %10];
	}
}

#if	USE_CLOCK
_attribute_ram_code_ void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;
	display_buff[0] = display_numbers[min % 10];
	display_buff[1] = display_numbers[min / 10 % 10];
	display_buff[2] = 0;
	display_buff[3] = display_numbers[hrs % 10];
	display_buff[4] = display_numbers[hrs / 10 % 10];
	display_buff[5] = 0;
}
#endif // USE_CLOCK

#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC
