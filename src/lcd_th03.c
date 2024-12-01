#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if (DEV_SERVICES & SERVICE_SCREEN) && (DEVICE_TYPE == DEVICE_ZTH03)
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"

RAM uint8_t lcd_i2c_addr;

#define I2C_TCLK_US	24 // 24 us


/*
 *  TH03 LCD buffer:  byte.bit

              --0.4--         --1.4--            --2.4--          BAT
       |    |         |     |         |        |         |        3.6
       |   0.6       0.0   1.6       1.0      2.6       2.0
       |    |         |     |         |        |         |      o 3.5
-3.3- 0.3     --0.2--         --1.2--            --2.2--          +--- 3.5
       |    |         |     |         |        |         |     3.5|
       |   0.5       0.1   1.5       1.1      2.5       2.1       ---- 3.7
       |    |         |     |         |        |         |     3.5|
              --0.7--         --1.7--     *      --2.7--          ---- 2.3
                                         1.3

                                        --4.4--         --5.4--
                                      |         |     |         |
          3.0      3.0               4.6       4.0   5.6       5.0
          / \      / \                |         |     |         |
    3.4(  ___  3.2 ___  )3.4            --4.2--         --5.2--
          3.2  / \ 3.2                |         |     |         |
               ___                   4.5       4.1   5.5       5.1     %
               3.0                    |         |     |         |     5.3
                                        --4.7--         --5.7--
                        OO 4.3

  None: 3.1
*/

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const uint8_t display_numbers[] = {
		// 76543210
		0b011110011, // 0
		0b000000011, // 1
		0b010110101, // 2
		0b010010111, // 3
		0b001000111, // 4
		0b011010110, // 5
		0b011110110, // 6
		0b000010011, // 7
		0b011110111, // 8
		0b011010111, // 9
		0b001110111, // A
		0b011100110, // b
		0b011110000, // C
		0b010100111, // d
		0b011110100, // E
		0b001110100  // F
};

#define LCD_SYM_b  0b011100110 // "b"
#define LCD_SYM_H  0b001100111 // "H"
#define LCD_SYM_h  0b001100110 // "h"
#define LCD_SYM_i  0b000100000 // "i"
#define LCD_SYM_L  0b011100000 // "L"
#define LCD_SYM_o  0b010100110 // "o"
#define LCD_SYM_t  0b011100100 // "t"
#define LCD_SYM_0  0b011110011 // "0"
#define LCD_SYM_A  0b001110111 // "A"
#define LCD_SYM_a  0b011110110 // "a"
#define LCD_SYM_P  0b001110101 // "P"

#define LCD_SYM_BLE	0x08	// connect
#define LCD_SYM_BAT	0x40	// battery


void soft_i2c_start(void) {
	gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
	gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SDA_LCD, 1); // SDA set "0"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL_LCD, 1); // SCL set "0"
	//sleep_us(10);
}

void soft_i2c_stop(void) {
	gpio_set_output_en(I2C_SDA_LCD, 1); // SDA set "0"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
}

int soft_i2c_wr_byte(uint8_t b) {
	int ret, i = 8;
	while(i--) {
		sleep_us(I2C_TCLK_US/2);
		if(b & 0x80)
			gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
		else
			gpio_set_output_en(I2C_SDA_LCD, 1); // SDA set "0"
		sleep_us(I2C_TCLK_US/2);
		gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
		sleep_us(I2C_TCLK_US);
		gpio_set_output_en(I2C_SCL_LCD, 1); // SCL set "0"
		b <<= 1;
	}
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SDA_LCD, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SCL_LCD, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	ret = gpio_read(I2C_SDA_LCD);
	gpio_set_output_en(I2C_SCL_LCD, 1); // SCL set "0"
	return ret;
}

int soft_i2c_send_buf(uint8_t addr, uint8_t * pbuf, int size) {
	int ret = 0;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0) {
		while(size--) {
			ret = soft_i2c_wr_byte(*pbuf);
			if(ret)
				break;
			pbuf++;
		}
	}
	soft_i2c_stop();
	return ret;
}
int soft_i2c_send_byte(uint8_t addr, uint8_t b) {
	int ret;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0)
		soft_i2c_wr_byte(b);
	soft_i2c_stop();
	return ret;
}

#define lcd_send_i2c_byte(a)  soft_i2c_send_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  soft_i2c_send_buf(lcd_i2c_addr, (uint8_t *) b, a)

#if 1
const uint8_t lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0x80, // load data pointer
		0xf0, // blink control off,  0xf2 - blink
		0xfc, // All pixel control (APCTL): Normal
		0x60,
		0x00, 0x00,000,0x00,0x00,0x00,0x00,0x00,0x00
};

#else

const uint8_t lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xf0, // blink control off,  0xf2 - blink
		0x00, 0xff,0xff,0xff,0xff,0xff,0xff
};

#endif

void init_lcd(void){
	lcd_i2c_addr = TH03_I2C_ADDR << 1;
	//display_cmp_buff[0] = 0;
	if(cfg.flg2.screen_off) {
		if(lcd_send_i2c_byte(0xD0) || lcd_send_i2c_byte(0xEA)) // LCD reset
			lcd_i2c_addr = 0;
	} else {
		if(lcd_send_i2c_buf((uint8_t *) lcd_init_cmd, sizeof(lcd_init_cmd)))
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
	if (lcd_i2c_addr) {
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
void show_temp_symbol(uint8_t symbol) {
	if (symbol & 0x20)
		display_buff[3] |= BIT(5);
	else
		display_buff[3] &= ~(BIT(5));
	if (symbol & 0x40)
		display_buff[3] |= BIT(7); //"-"
	else
		display_buff[3] &= ~BIT(7); //"-"
	if (symbol & 0x80)
		display_buff[2] |= BIT(3); // "_"
	else
		display_buff[2] &= ~BIT(3); // "_"
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
	display_buff[3] &= ~0x15;
	state = (state & 1) | ((state << 1) & 4) | ((state << 2) & 0x10);
	display_buff[3] |= state;
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
		display_buff[3] |= BIT(6);
	else
		display_buff[3] &= ~BIT(6);
}

/* number in 0.1 (-19995..19995), Show: -1999 .. 1999 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number){
	display_buff[2] &= BIT(3); // F/C "_"
	if (number > 19995) {
   		display_buff[0] = LCD_SYM_H; // "H"
   		display_buff[1] = LCD_SYM_i; // "i"
	} else if (number < -19995) {
   		display_buff[0] = LCD_SYM_L; // "L"
   		display_buff[1] = LCD_SYM_o; // "o"
	} else {
		display_buff[0] = 0;
		display_buff[1] = 0;
		/* number: -19995..19995 */
		if (number > 1995 || number < -1995) {
			display_buff[1] = 0; // no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[0] = BIT(2); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -199.9..199.9
			display_buff[1] = BIT(3); // point,
			if (number < 0){
				number = -number;
				display_buff[0] = BIT(2); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99) display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9) display_buff[1] |= display_numbers[number / 10 % 10];
		else display_buff[1] |= LCD_SYM_0; // "0"
	    display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_small_number(int16_t number, bool percent){
	display_buff[4] &= BIT(3); // connect
	display_buff[5] = percent? BIT(3) : 0;
	if (number > 99) {
		display_buff[4] |= LCD_SYM_H; // "H"
		display_buff[5] |= LCD_SYM_i; // "i"
	} else if (number < -9) {
		display_buff[4] |= LCD_SYM_L; // "L"
		display_buff[5] |= LCD_SYM_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			display_buff[4] = BIT(2); // "-"
		}
		if (number > 9) display_buff[4] |= display_numbers[number / 10 % 10];
		display_buff[5] |= display_numbers[number %10];
	}
}

void show_ota_screen(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	display_buff[0] = BIT(2); // "_"
	display_buff[1] = BIT(2); // "_"
	display_buff[2] = BIT(2); // "_"
	display_buff[4] = BIT(3); // "ble"
	send_to_lcd();
	lcd_send_i2c_byte(0xf2);
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

#if	USE_DISPLAY_CLOCK
_attribute_ram_code_
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = (tmp / 60) % 24;
	display_buff[0] = 0;
	display_buff[1] = display_numbers[(hrs / 10) % 10];
	display_buff[2] = display_numbers[hrs % 10];
	display_buff[3] &= BIT(6); // bat
	display_buff[4] &= BIT(3); // connect
	display_buff[4] |= display_numbers[(min / 10) % 10];
	display_buff[5] = display_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

#endif // DEVICE_TYPE == DEVICE_TH03
