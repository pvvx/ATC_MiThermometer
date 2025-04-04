#include "tl_common.h"
#include "app_config.h"
#if (DEV_SERVICES & SERVICE_SCREEN) && (DEVICE_TYPE == DEVICE_MJWSD06MMC)
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"

RAM u8 lcd_i2c_addr;


/*
 *  MJWSD06MMC LCD buffer:  byte.bit

                              OO 4.3               /+ 2.5
                             
           --0.4--         --4.4--            --3.4--          BAT
    |    |         |     |         |        |         |        2.0
    |   5.0       5.5   4.0       4.5      3.0       3.5
    |    |         |     |         |        |         |      o 2.2
   5.3     --5.1--         --4.1--            --3.1--          +--- 2.2
    |    |         |     |         |        |         |     2.2|
    |   5.2       5.6   4.2       4.6      3.2       3.6       ---- 2.1
    |    |         |     |         |        |         |     2.2|
           --5.7--         --4.7--     *      --3.7--          ---- 2.3
                                      3.3

                                     --1.7--         --0.7--
                                   |         |     |         |
       2.4      2.4               1.3       1.6   0.3       0.6
       / \      / \                |         |     |         |
 1.0(  ___  2.7 ___  )1.0            --1.2--         --0.2--
       2.7  / \ 2.7                |         |     |         |
            ___                   1.1       1.5   0.1       0.5     %
            2.4                    |         |     |         |     0.0
                                     --1.4--         --0.4--

  None: 2.6
*/
/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
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
#define LCD_SYM1_b  0b11000111 // "b"
#define LCD_SYM1_H  0b01100111 // "H"
#define LCD_SYM1_h  0b01100110 // "h"
#define LCD_SYM1_i  0b01000000 // "i"
#define LCD_SYM1_L  0b10000101 // "L"
#define LCD_SYM1_o  0b11000110 // "o"
#define LCD_SYM1_t  0b10000111 // "t"
#define LCD_SYM1_0  0b11110101 // "0"
#define LCD_SYM1_A  0b01110111 // "A"
#define LCD_SYM1_a  0b11110110 // "a"
#define LCD_SYM1_P  0b00110111 // "P"

const u8 display_small_numbers[] = {
        //76543210
		0b11111010, // 0
		0b00001010, // 1
		0b11010110, // 2
		0b11110100, // 3
		0b01101100, // 4
		0b10111100, // 5
		0b10111110, // 6
		0b11100000, // 7
		0b11111110, // 8
		0b11111100, // 9
		0b11101110, // A
		0b00111110, // b
		0b10011010, // C
		0b01110110, // d
		0b10011110, // E
		0b10001110};  // F

#define LCD_SYM2_b  0b00111110 // "b"
#define LCD_SYM2_H  0b01101110 // "H"
#define LCD_SYM2_h  0b00101110 // "h"
#define LCD_SYM2_i  0b00100000 // "i"
#define LCD_SYM2_L  0b00011010 // "L"
#define LCD_SYM2_o  0b00110110 // "o"
#define LCD_SYM2_t  0b00011110 // "t"
#define LCD_SYM2_0  0b11111010 // "0"
#define LCD_SYM2_A  0b11101110 // "A"
#define LCD_SYM2_a  0b11110110 // "a"
#define LCD_SYM2_P  0b11001110 // "P"


#define lcd_send_i2c_byte(a)	send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)	send_i2c_buf(lcd_i2c_addr, (u8 *) b, a)

const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0x80, // load data pointer (ADSET)
		0xf0, // blink control (BLKCTL) off,  0xf2 - blink
		0xfc, // All pixel control (APCTL): Normal
		0x0b,
		0x00, 0x00,000,0x00,0x00,0x00,0x00,0x00,0x00
};

void init_lcd(void){
	lcd_i2c_addr = AIP31620E_I2C_ADDR << 1;
	display_cmp_buff[0] = 0x0b;
	if(cfg.flg2.screen_off) {
		if(lcd_send_i2c_byte(0xD0) || lcd_send_i2c_byte(0xEA)) // LCD reset
			lcd_i2c_addr = 0;
	} else {
		if(lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd)))
			lcd_i2c_addr = 0;
		else {
			pm_wait_us(200);
			send_to_lcd();
		}
	}
}

_attribute_ram_code_
void send_to_lcd(void){
	if(cfg.flg2.screen_off)
		return;
	if(lcd_i2c_addr) {
		lcd_send_i2c_buf(display_cmp_buff, sizeof(display_cmp_buff));
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
void show_temp_symbol(u8 symbol) {
	display_buff[2] &= ~(BIT(1) | BIT(2) | BIT(3));
	if (symbol & 0x20)
		display_buff[2] |= BIT(2); // "°Г"
	if (symbol & 0x40)
		display_buff[2] |= BIT(1); // "-"
	if (symbol & 0x80)
		display_buff[2] |= BIT(3); // "_"
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
void show_smiley(u8 state){
	display_buff[2] &= ~(BIT(4) | BIT(7));
	display_buff[1] &= ~BIT(0);
	if(state & 1)
		display_buff[2] |= BIT(4);
	if(state & 2)
		display_buff[2] |= BIT(7);
	if(state & 4)
		display_buff[1] |= BIT(0);

}

_attribute_ram_code_
void show_ble_symbol(bool state){
	if (state)
		display_buff[4] |= BIT(3);
	else 
		display_buff[4] &= ~BIT(3);
}

_attribute_ram_code_
void show_battery_symbol(bool state){
	if (state)
		display_buff[2] |= BIT(0);
	else
		display_buff[2] &= ~BIT(0);
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_big_number_x10(s16 number){
	display_buff[4] &= BIT(3);
	display_buff[5] = 0;
	if (number > 19995) {
   		display_buff[4] = LCD_SYM1_H; // "H"
   		display_buff[3] = LCD_SYM1_i; // "i"
	} else if (number < -995) {
   		display_buff[4] = LCD_SYM1_L; // "L"
   		display_buff[3] = LCD_SYM1_o; // "o"
	} else {
		/* number: -995..19995 */
		if (number > 1999 || number < -99) {
			/* number: -995..-100, 2000..19995 */
			// round(div 10)
			number += 5;
			number /= 10;
			// show no point: -99..-10, 200..1999
			display_buff[3] = 0;
		} else {
			// show point: -9.9..199.9
			display_buff[3] = BIT(3); // point
		}
		/* show: -99..1999 */
		if (number < 0) {
			number = -number;
			display_buff[5] = BIT(1); // "-"
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[5] = BIT(3); // "1" 1000..1999
		if (number > 99) display_buff[5] |= display_numbers[number / 100 % 10];
		if (number > 9) display_buff[4] |= display_numbers[number / 10 % 10];
		else display_buff[4] |= LCD_SYM1_0; // "0"
	    display_buff[3] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_small_number(s16 number, bool percent){
	display_buff[1] &= BIT(0); //  "(smile)"
	display_buff[0] = percent? BIT(0) : 0;
	if (number > 99) {
		display_buff[1] |= LCD_SYM2_H; // "H"
		display_buff[0] |= LCD_SYM2_i; // "i"
	} else if (number < -9) {
		display_buff[1] |= LCD_SYM2_L; // "L"
		display_buff[0] |= LCD_SYM2_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			display_buff[1] = BIT(2); // "-"
		}
		if (number > 9) display_buff[1] |= display_small_numbers[number / 10 % 10];
		display_buff[0] |= display_small_numbers[number %10];
	}
}

void show_ota_screen(void) {
	display_buff[0] = 0; // " "
	display_buff[1] = 0; // " "
	display_buff[2] &= BIT(0) | BIT(5); // "bat" & "/+"
	display_buff[5] = LCD_SYM1_0; // "O"
	display_buff[4] = LCD_SYM1_t | BIT(3); // "t" | "ble"
	display_buff[3] = LCD_SYM1_a; // "a"
	send_to_lcd();
	lcd_send_i2c_byte(0xf2);
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	display_buff[0] = 0; // " "
	display_buff[1] = 0; // " "
	display_buff[2] = 0; // " "
	display_buff[3] = LCD_SYM1_o; // "o"
	display_buff[4] = LCD_SYM1_o; // "o"
	display_buff[5] = LCD_SYM1_o; // "o"
	send_to_lcd();
}

#if	USE_DISPLAY_CLOCK
_attribute_ram_code_
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = (tmp / 60) % 24;
	display_buff[5] = 0;
	display_buff[4] &= BIT(3); // ble
	display_buff[4] = display_numbers[(hrs / 10) % 10];
	display_buff[3] = display_numbers[hrs % 10];
	display_buff[2] &= BIT(0) | BIT(5); // "bat" & "/+"
	display_buff[1] = display_small_numbers[(min / 10) % 10];
	display_buff[0] = display_small_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

#endif // DEVICE_TYPE == DEVICE_MJWSD06MMC
