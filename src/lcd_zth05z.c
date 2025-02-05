#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_ZTH05Z
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"
#include "battery.h"

RAM u8 lcd_i2c_addr;

#define lcd_send_i2c_byte(a)  send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_buf(lcd_i2c_addr, (u8 *) b, a)


/*
 *  LYWSD03MMC LCD buffer:  byte.bit

         --0.4--         --1.4--            --2.4--          BAT
  |    |         |     |         |        |         |        3.5
  |   0.5       0.0   1.5       1.0      2.5       2.0
  |    |         |     |         |        |         |      o 3.6
 0.3     --0.1--         --1.1--            --2.1--          +--- 3.6
  |    |         |     |         |        |         |     3.6|
  |   0.6       0.2   1.6       1.2      2.6       2.2       ---- 3.7
  |    |         |     |         |        |         |     3.6|
         --0.7--         --1.7--     *      --2.7--          ---- 2.3
                                    1.3
                                        --4.4--         --5.4--
                                      |         |     |         |
          3.0      3.0               4.5       4.0   5.5       5.0
          / \      / \                |         |     |         |
    3.4(  ___  3.1 ___  )3.4            --4.1--         --5.1--
          3.1  / \ 3.1                |         |     |         |
               ___                   4.6       4.2   5.6       5.2     %
               3.0                    |         |     |         |     5.3
                                        --4.7--         --5.7--
                        oo 4.3
None: 3.2, 3.3
*/

/* t,H,h,L,o,i */
#define LCD_SYM_H	0x67	// "H"
#define LCD_SYM_i	0x40	// "i"
#define LCD_SYM_L	0xE0	// "L"
#define LCD_SYM_o	0xC6	// "o"

/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
		// 76543210
		0b011110101, // 0  0xf5
		0b000000101, // 1  0x05
		0b011010011, // 2  0xd3
		0b010010111, // 3  0x97
		0b000100111, // 4  0x27
		0b010110110, // 5  0xb6
		0b011110110, // 6  0xf6
		0b000010101, // 7  0x15
		0b011110111, // 8  0xf7
		0b010110111, // 9  0xb7
		0b001110111, // A  0x77
		0b011100110, // b  0xe6
		0b011110000, // C  0xf0
		0b011000111, // d  0xc7
		0b011110010, // E  0xf2
		0b001110010  // F  0x72
};

/* Test cmd ():
 * 0400007ceaa49cacbcf0fcc808ffffffff,
 * 0400007cf3c8 - blink
 */
//const u8 lcd_init_cmd[] = {0xb6,0xfc, 0xc8, 0xe8, 0x08, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC 2Operation(ICSET): Software Reset, Internal oscillator circuit
		0xd8, // Mode Set (MODE SET): Display enable, 1/3 Bias, power saving
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xf0, // blink control off,  0xf2 - blink
		0xfc, // All pixel control (APCTL): Normal
		0x08,
		0x00,0x00,000,0x11,0x00,0x00
};

_attribute_ram_code_
void send_to_lcd(void){
	if(cfg.flg2.screen_off)
		return;
	if (lcd_i2c_addr) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		u8 * p = display_buff;
		reg_i2c_id = lcd_i2c_addr;
		reg_i2c_adr = 0x08;	// addr:8
		reg_i2c_do = *p++;
		reg_i2c_di = *p++;
		reg_i2c_ctrl = FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_START;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		reg_i2c_do = *p++;
		reg_i2c_di = *p++;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_DI;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		reg_i2c_do = *p++;
		reg_i2c_di = *p;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
}


void init_lcd(void){
	lcd_i2c_addr = (u8) scan_i2c_addr(BL55028_I2C_ADDR << 1);
	if (lcd_i2c_addr) { // B1.9
		if(cfg.flg2.screen_off) {
			lcd_send_i2c_byte(0xEA); // lcd reset
		} else {
			lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd));
		}
		return;
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
	display_buff[3] &= ~(BIT(6)| BIT(7));
	if(symbol & 0x20) {
		display_buff[3] |= BIT(6);
	}
	if(symbol & 0x40) {
		display_buff[3] |= BIT(7);
	}
	if(symbol & 0x80) {
		display_buff[2] |= BIT(3);
	} else {
		display_buff[2] &= ~BIT(3);
	}

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
	display_buff[3] &= ~(BIT(0) | BIT(1)| BIT(4));
	if(state & 1) {
		display_buff[3] |= BIT(0);
	}
	if(state & 2) {
		display_buff[3] |= BIT(1);
	}
	if(state & 4) {
		display_buff[3] |= BIT(4);
	}
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
		display_buff[3] |= BIT(5);
	else
		display_buff[3] &= ~BIT(5);
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_
void show_big_number_x10(s16 number){
	display_buff[2] &= BIT(3); // "_" F
	if (number > 19995) {
   		display_buff[0] = LCD_SYM_H; // "H"
   		display_buff[1] = LCD_SYM_i; // "i"
	} else if (number < -995) {
   		display_buff[0] = LCD_SYM_L; // "L"
   		display_buff[1] = LCD_SYM_o; // "o"
	} else {
		display_buff[0] = 0;
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			display_buff[1] = 0; // no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[0] = BIT(1); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[1] = BIT(3); // point,
			if (number < 0){
				number = -number;
				display_buff[0] = BIT(1); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999)
			display_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99)
			display_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9)
			display_buff[1] |= display_numbers[number / 10 % 10];
		else
			display_buff[1] |= 0xF5; // "0"
	    display_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_attribute_ram_code_
void show_small_number(s16 number, bool percent){
	display_buff[4] &= BIT(3); // and "oo"
	display_buff[5] = percent? BIT(3): 0; // "%"
	if (number > 99) {
		display_buff[4] |= LCD_SYM_H; // "H"
		display_buff[5] |= LCD_SYM_i; // "i"
	} else if (number < -9) {
		display_buff[4] |= LCD_SYM_L; // "L"
		display_buff[5] |= LCD_SYM_o; // "o"
	} else {
		if (number < 0) {
			number = -number;
			display_buff[4] |= BIT(1); // "-"
		}
		if (number > 9)
			display_buff[4] |= display_numbers[number / 10];
		display_buff[5] |= display_numbers[number %10];
	}
}

void show_ota_screen(void) {
	display_buff[0] = BIT(1); // "_"
	display_buff[1] = BIT(1); // "_"
	display_buff[2] = BIT(1); // "_"
	display_buff[3] &= BIT(5); // "bat"
	display_buff[4] = BIT(1) | BIT(3); // "ble"
	display_buff[5] = BIT(1);
	send_to_lcd();
	lcd_send_i2c_byte(0xf2); // flash screen
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

#if	USE_DISPLAY_CLOCK
//_attribute_ram_code_
void show_clock(void) {
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = (tmp / 60) % 24;
	display_buff[0] = display_numbers[hrs / 10];
	display_buff[1] = display_numbers[hrs % 10];
	display_buff[2] = 0;
	display_buff[3] = 0;
	display_buff[4] = display_numbers[min / 10];
	display_buff[5] = display_numbers[min % 10];
}
#endif // USE_DISPLAY_CLOCK

#endif // DEVICE_TYPE == DEVICE_ZTH05Z
