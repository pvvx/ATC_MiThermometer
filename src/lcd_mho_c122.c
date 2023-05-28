/*
 * lcd_mho_c122.c
 *
 *      Created: pvvx, 28.05.2023 
 *      Edited by: FaBjE
 *
 *  https://github.com/pvvx/ATC_MiThermometer/issues/339
 */
#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_MHO_C122
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"
/*
 *  MHO-C122 LCD buffer:  byte.bit

         --0.4--         --1.4--            --2.4--
  |    |         |     |         |        |         |
  |   0.0       0.5   1.0       1.5      2.0       2.5
  |    |         |     |         |        |         |      o 3.0
 0.3     --0.1--         --1.1--            --2.1--          +--- 3.0
  |    |         |     |         |        |         |     3.0|
  |   0.2       0.6   1.2       1.6      2.2       2.6       ---- 3.1
  |    |         |     |         |        |         |     3.0|
         --0.7--         --1.7--     *      --2.7--          ---- 3.2
                                    2.3
           --5.3--         --4.3--              1.3       1.3            
 (|)     |         |     |         |            / \       / \      
 3.6    5.6       5.2   4.6       4.2     1.3(  ___  3.1  ___  )1.3
         |         |     |         |            1.3 /3.6\ 1.3      
 BLE       --5.5--         --4.5--                  _____
 4.7     |         |     |         |                \3.5/
        5.4       5.1   4.4       4.1     	          
 BAT     |         |     |         |                  %
 5.7       --5.0--         --4.0--                   3.4

None: 3.3 ?
*/

RAM uint8_t lcd_i2c_addr;

#define lcd_send_i2c_byte(a)  send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_buf(lcd_i2c_addr, (uint8_t *) b, a)

#define LCD_SYM_H	0b01100111	// "H"
#define LCD_SYM_i	0b00000100	// "i"
#define LCD_SYM_L	0b10000101	// "L"
#define LCD_SYM_o	0b11000110	// "o"

#define LCD_SYM_BLE	0b10000000	// connect
#define LCD_SYM_BAT	0b010000000	// battery

const uint8_t lcd_init_cmd_b14[] =	{0x80,0x3B,0x80,0x02,0x80,0x0F,0x80,0x95,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x88,0x80,0x19,0x80,0x28,0x80,0xE3,0x80,0x11};
								//	{0x80,0x40,0xC0,byte1,0xC0,byte2,0xC0,byte3,0xC0,byte4,0xC0,byte5,0xC0,byte6};
const uint8_t lcd_init_clr_b14[] =	{0x80,0x40,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0,0xC0,0};

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const uint8_t display_numbers[] = {
		0b11110101, // 0
		0b01100000, // 1
		0b10110110, // 2
		0b11110010, // 3
		0b01100011, // 4
		0b11010011, // 5
		0b11010111, // 6
		0b01110000, // 7
		0b11110111, // 8
		0b11110011, // 9
		0b01110111, // A
		0b11000111, // b
		0b10010101, // C
		0b11100110, // d
		0b10010111, // E
		0b00010111};  // F
const uint8_t display_small_numbers[] = {
		0b01011111, // 0
		0b00000110, // 1
		0b00111101, // 2
		0b00101111, // 3
		0b01100110, // 4
		0b01101011, // 5
		0b01111011, // 6
		0b00001110, // 7
		0b01111111, // 8
		0b01101111, // 9
		0b01111110, // A
		0b01110011, // b
		0b01011001, // C
		0b00110111, // d
		0b01111001, // E
		0b01111000};  // F

_attribute_ram_code_
void send_to_lcd(void){
	unsigned int buff_index;
	uint8_t * p = display_buff;
	if(cfg.flg2.screen_off)
		return;
	if (lcd_i2c_addr) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		else {
			gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_10K);
			gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_10K);
		}
		reg_i2c_id = lcd_i2c_addr;
		reg_i2c_adr_dat = 0x4080;
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		reg_i2c_adr = 0xC0;
		for(buff_index = 0; buff_index < sizeof(display_buff); buff_index++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_ctrl = FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
}

void init_lcd(void){
	lcd_i2c_addr = (uint8_t) scan_i2c_addr(B14_I2C_ADDR << 1);
	if (lcd_i2c_addr) {
// 		GPIO_PB6 set in app_config.h!
//		gpio_setup_up_down_resistor(GPIO_PB6, PM_PIN_PULLUP_10K); // LCD on low temp needs this, its an unknown pin going to the LCD controller chip
		if(!cfg.flg2.screen_off) {
			pm_wait_ms(50);
			lcd_send_i2c_buf((uint8_t *) lcd_init_cmd_b14, sizeof(lcd_init_cmd_b14));
			lcd_send_i2c_buf((uint8_t *) lcd_init_clr_b14, sizeof(lcd_init_clr_b14));
		}
	}
}

/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_attribute_ram_code_
void show_temp_symbol(uint8_t symbol) {
	display_buff[3] &= ~(BIT(0) | BIT(1) | BIT(2));
	if (symbol & 0x20)
		display_buff[3] |= BIT(0);
	if (symbol & 0x40)
		display_buff[3] |= BIT(1); //"-"
	if (symbol & 0x80)
		display_buff[3] |= BIT(2); // "_"
}

/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */
_attribute_ram_code_
void show_smiley(uint8_t state){
	display_buff[1] &= ~BIT(3);
	display_buff[3] &= ~(BIT(5) | BIT(6));

	if(state & 1)
		display_buff[3] |= BIT(5); //Happy mouth
	if(state & 2)
		display_buff[3] |= BIT(6); //Sad mouth
	if(state & 4)
		display_buff[1] |= BIT(3); //Smiley contour
}

_attribute_ram_code_
void show_ble_symbol(bool state){
	if (state)
		display_buff[4] |= LCD_SYM_BLE;
	else 
		display_buff[4] &= ~LCD_SYM_BLE;
}

_attribute_ram_code_
void show_battery_symbol(bool state){
	if (state)
		display_buff[5] |= LCD_SYM_BAT;
	else 
		display_buff[5] &= ~LCD_SYM_BAT;
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number){
	display_buff[0] = 0;
	display_buff[1] &= BIT(3); // smiley contour
	display_buff[2] = 0;

	if (number > 19995) {
   		display_buff[0] |= LCD_SYM_H; // "H"
   		display_buff[1] |= LCD_SYM_i; // "i"
	} else if (number < -995) {
   		display_buff[0] |= LCD_SYM_L; // "L"
   		display_buff[1] |= LCD_SYM_o; // "o"
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			if (number < 0){
				number = -number;
				display_buff[0] |= BIT(1); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[2] = BIT(3); // point
			if (number < 0){
				number = -number;
				display_buff[0] |= BIT(1); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99) display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9) display_buff[1] |= display_numbers[number / 10 % 10];
		else display_buff[1] |= display_numbers[0]; // "0"
	    display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_small_number(int16_t number, bool percent){
	display_buff[4] &= BIT(7); // and ble
	display_buff[5] &= BIT(7); // and battery

	if (percent)
		display_buff[3] |= BIT(4); // %
	else
		display_buff[3] &= ~BIT(4); // %

	if (number > 99) {
		display_buff[5] |= BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(6) ; // "H"
		display_buff[4] |= BIT(4); // "i"
	} else if (number < -9) {
		display_buff[5] |= BIT(0) | BIT(4) | BIT(6); // "L"
		display_buff[4] |= BIT(0) | BIT(1) | BIT(4) | BIT(5); // "o"
	} else {
		if (number < 0) {
			number = -number;
			display_buff[5] |= BIT(5); // "-"
		}
		if (number > 9) 
			display_buff[5] |= display_small_numbers[number / 10 % 10];

		display_buff[4] |= display_small_numbers[number %10];
	}
}

void show_ota_screen(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	display_buff[4] = BIT(7); // "ble"
	display_buff[0] = BIT(1); // "_"
	display_buff[1] = BIT(1); // "_"
	display_buff[2] = BIT(1); // "_"
	send_to_lcd();
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

#if	USE_CLOCK
_attribute_ram_code_
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;

	display_buff[0] = display_numbers[hrs % 10];
	display_buff[1] = display_numbers[hrs / 10 % 10];
	display_buff[2] = 0;
	display_buff[3] = 0;

	display_buff[5] &= ~BIT(7); // and battery
	display_buff[5] |= display_small_numbers[min % 10];
	
	display_buff[4] &= ~BIT(7); // and ble
	display_buff[4] |= display_small_numbers[min / 10 % 10];
}
#endif // USE_CLOCK

#endif // DEVICE_TYPE == DEVICE_MHO_C122
