#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "drivers/8258/gpio_8258.h"
#include "main.h"

const uint8_t lcd_init_cmd[] = {0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
RAM uint8_t display_buff[6];
const uint8_t display_numbers[16] = {0xF5,0x05,0xD3,0x97,0x27,0xb6,0xf6,0x15,0xf7,0xb7,0x77,0xe6,0xf0,0xc7,0xf2,0x72};

void init_lcd(){	

	gpio_set_func(GPIO_PB6, AS_GPIO);//LCD on low temp needs this, its an unknown pin going to the LCD controller chip
	gpio_set_output_en(GPIO_PB6, 0);
	gpio_set_input_en(GPIO_PB6, 1); 
	gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K);
	
//	sleep_us(50000);
	cpu_stall_wakeup_by_timer0(50*CLOCK_SYS_CLOCK_1MS);
	
	send_i2c(0x78,lcd_init_cmd, sizeof(lcd_init_cmd));
	send_to_lcd_long(0x00,0x00,0x00,0x00,0x00,0x00);	
}
	
void send_to_lcd_long(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6){
    uint8_t lcd_set_segments[] =    {0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6,0xC0,0x00,0xC0,0x00};
	send_i2c(0x78,lcd_set_segments, sizeof(lcd_set_segments));
}
	
void send_to_lcd(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6){
    uint8_t lcd_set_segments[] =    {0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6};
	send_i2c(0x78,lcd_set_segments, sizeof(lcd_set_segments));
}

void update_lcd(){
	send_to_lcd(display_buff[0],display_buff[1],display_buff[2],display_buff[3],display_buff[4],display_buff[5]);
}

void show_number(uint8_t position,uint8_t number){
	if(position>5 || position == 2 || number >9)return;	
    display_buff[position] = display_numbers[number] & 0xF7;
}

void show_temp_symbol(uint8_t symbol){/*1 = C, 2 = F*/
	display_buff[2] &= ~0xE0;
	if(symbol==1)display_buff[2]|=0xA0;
	else if(symbol==2)display_buff[2]|=0x60;
}

void show_ble_symbol(bool state){
	if(state)
		display_buff[2] |= 0x10;
	else 
		display_buff[2] &= ~0x10;
}

void show_battery_symbol(bool state){
	if(state)
		display_buff[1] |= 0x08;
	else 
		display_buff[1] &= ~0x08;
}

void show_smiley(uint8_t state){/*0=off, 1=happy, 2=sad*/
	display_buff[2] &= ~0x07;
	if(state==1)display_buff[2]|=0x05;
	else if(state==2)display_buff[2]|=0x06;
}

void show_atc(){
	send_to_lcd(0x00,0x00,0x05,0xc2,0xe2,0x77);
}

void show_atc_mac(){
	extern u8  mac_public[6];
	send_to_lcd(display_numbers[mac_public[2] &0x0f],display_numbers[mac_public[2]>>4],0x05,0xc2,0xe2,0x77);
	//sleep_ms(1800);
	cpu_stall_wakeup_by_timer0(1800*CLOCK_SYS_CLOCK_1MS);
	send_to_lcd(0x00,0x00,0x05,0xc2,0xe2,0x77);
	//sleep_ms(200);
	cpu_stall_wakeup_by_timer0(200*CLOCK_SYS_CLOCK_1MS);
	send_to_lcd(display_numbers[mac_public[1] &0x0f],display_numbers[mac_public[1]>>4],0x05,0xc2,0xe2,0x77);
	//sleep_ms(1800);
	cpu_stall_wakeup_by_timer0(1800*CLOCK_SYS_CLOCK_1MS);
	send_to_lcd(0x00,0x00,0x05,0xc2,0xe2,0x77);
	//sleep_ms(200);
	cpu_stall_wakeup_by_timer0(200*CLOCK_SYS_CLOCK_1MS);
	send_to_lcd(display_numbers[mac_public[0] &0x0f],display_numbers[mac_public[0]>>4],0x05,0xc2,0xe2,0x77);
	//sleep_ms(1800);
	cpu_stall_wakeup_by_timer0(1800*CLOCK_SYS_CLOCK_1MS);
}

void show_big_number(int16_t number, bool point){
	if(number >1999)return;	
	if(number < -99)return;
	display_buff[5] = (number > 999)?0x08:0x00; 
	if(number < 0){
		number = -number;
		display_buff[5] = 2; 
	}
	display_buff[4] = point?0x08:0x00; 
	if(number > 99)display_buff[5] |= display_numbers[number / 100 % 10] & 0xF7;
	if(number > 9)display_buff[4] |= display_numbers[number / 10 % 10] & 0xF7;
	if(number < 9)display_buff[4] |= display_numbers[0] & 0xF7;
    display_buff[3] = display_numbers[number %10] & 0xF7;
}

void show_small_number(uint16_t number, bool percent){
	if(number >99)return;	
	display_buff[0] = percent?0x08:0x00;
	display_buff[1] = display_buff[1] & 0x08;
	if(number > 9)display_buff[1] |= display_numbers[number / 10 % 10] & 0xF7;
    display_buff[0] |= display_numbers[number %10] & 0xF7;
}
