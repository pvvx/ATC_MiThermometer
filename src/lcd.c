#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"

#include "i2c.h"
#include "lcd.h"

const uint8_t lcd_init_cmd[] = {0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
RAM uint8_t display_buff[6];
RAM uint8_t display_cmp_buff[6];
/* t,H,h,L,o,i  0xe2,0x67,0x66,0xe0,0xC6,0x40 */
/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const uint8_t display_numbers[] = {0xf5,0x05,0xd3,0x97,0x27,0xb6,0xf6,0x15,0xf7,0xb7, 0x77,0xe6,0xf0,0xc7,0xf2,0x72 };

void init_lcd(){	
	gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K); // LCD on low temp needs this, its an unknown pin going to the LCD controller chip
	
	StallWaitMs(50);
	
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

#if 0
_attribute_ram_code_ void show_atc(){
	send_to_lcd(0x00,0x00,0x05,0xc2,0xe2,0x77);
}
void show_atc_mac(){
	extern u8  mac_public[6];
	send_to_lcd(display_numbers[mac_public[2] &0x0f],display_numbers[mac_public[2]>>4],0x05,0xc2,0xe2,0x77);
	StallWaitMs(1800);
	send_to_lcd(0x00,0x00,0x05,0xc2,0xe2,0x77);
	StallWaitMs(200);
	send_to_lcd(display_numbers[mac_public[1] &0x0f],display_numbers[mac_public[1]>>4],0x05,0xc2,0xe2,0x77);
	StallWaitMs(1800);
	send_to_lcd(0x00,0x00,0x05,0xc2,0xe2,0x77);
	StallWaitMs(200);
	send_to_lcd(display_numbers[mac_public[0] &0x0f],display_numbers[mac_public[0]>>4],0x05,0xc2,0xe2,0x77);
	StallWaitMs(1800);
}
#endif
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
		if(number > 99)display_buff[5] |= display_numbers[number / 100 % 10] & 0xF7; //
		if(number > 9)display_buff[4] |= display_numbers[number / 10 % 10] & 0xF7;
		if(number < 9)display_buff[4] |= 0xF5; // "0"
	    display_buff[3] = display_numbers[number %10] & 0xF7;
	}
}

/* -9 .. 99 */
_attribute_ram_code_ void show_small_number(int16_t number, bool percent){
	display_buff[1] = display_buff[1] & 0x08; // battery
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
		if(number > 9) display_buff[1] |= display_numbers[number / 10 % 10] & 0xF7;
		display_buff[0] |= display_numbers[number %10] & 0xF7;
	}
}
