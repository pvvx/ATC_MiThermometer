/* Edited by huncrys https://github.com/huncrys */
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_MJWSD05MMC_EN
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
#include "rtc.h"
#endif
#include "lcd.h"
#include "ble.h"
#include "battery.h"

#define lcd_send_i2c_byte(a)  send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_buf(lcd_i2c_addr, (u8 *) b, a)

RAM u8 lcd_i2c_addr;
RAM lcd_flg_t lcd_flg;
RAM u8 display_buff[LCD_BUF_SIZE];
RAM u8 display_cmp_buff[LCD_BUF_SIZE];


/* MJWSD05MMC LCD buffer:  byte.bit
                                                              __ __
  	 AM  PM 	  BLE         BAT  				  17.5( ^__^    ^   )17.5
  	 3.0 2.0	  0.0       [|17.4]#                17.6  17.7


         --3.4--         --2.4--            --1.4--         -17.0--      o 0.4
  |    |         |     |         |  1.0   |         |     |         |      +--- 0.4
  |   3.1       3.5   2.1       2.5  o   1.1       1.5   0.1      17.1     |
  |    |         |     |         |        |         |     |         |      ---- 0.5
 4.4     --3.2--         --2.2--    1.0     --1.2--         --0.2--        |
  |    |         |     |         |   o    |         |     |         |      |
  |   3.3       3.6   2.3       2.6      1.3       1.6   0.3      17.2    0.6
  |    |         |     |         |        |         |     |         |         o/ 0.7
         --3.7--         --2.7--     *      --1.7--         -17.3--           /o
                                   13.0

 Воскресенье  Понедельник  Вторник    Среда    Четверг   Пятница  Суббота
   Sunday       Monday     Tuesday  Wednesday  Thursday  Friday   Saturday
    4.5          4.6         4.7       4.3       4.2      4.1       4.0

      --5.0--         --6.0--               --7.0--         --8.0--
    |         |     |         |           |         |     |         |
   5.5       5.1   6.5       6.1      /  7.5       7.1   8.5       8.1
    |         |     |         |      /    |         |     |         |
      --5.6--         --6.6--      6.4      --7.6--         --8.6--
    |         |     |         |    /      |         |     |         |
   5.7       5.2   6.7       6.2  /      7.7       7.2   8.7       8.2
    |         |     |         |           |         |     |         |
      --5.3--         --6.3--               --7.3--         --8.3--


      --9.4--         -10.4--      10.0      -11.4--           -12.4--      o 9.0
    |         |     |         |     o      |         |       |         |      +--- 9.0
   8.4       9.5   9.1      10.5         10.1      11.5    11.1      12.5     |
    |         |     |         |    10.0    |         |       |         |      ---- 7.4
      --9.6--         --9.2--       o        -10.2--           -11.2--        |
    |         |     |         |            |         |       |         |      |
   9.4       9.7   9.3      10.6         10.3      11.6    11.3      12.6    5.4
    |         |     |         |            |         |   *   |         |
      --9.4--         -10.7--                -11.7--    11.0   -12.7--


          -13.4--         -14.4--            -15.4--       o 15.0
  |     |         |     |         |        |         |       +--- 15.0
  |   12.1      13.5  13.1      14.5     14.1      15.5      |
  |     |         |     |         |        |         |       ---- 15.1
 12.0     -12.2--         -13.2--            -14.2--         |
  |     |         |     |         |        |         |       |
  |   12.3      13.6  13.3      14.6     14.3      15.6     15.2
  |     |         |     |         |   *    |         |          o/ 15.3
          -13.7--         -14.7--    14.0    -15.7--            /o

none: 16.0..16.7
*/

#define DEF_MJWSD05MMC_SUMBOL_SIGMENTS	7
/*
Now define how each digit maps to the segments:
    -----1-----
   |           |
   6           2
   |           |
    -----7-----
   |           |
   5           3
   |           |
    -----4-----
*/

const u8 digits[16][DEF_MJWSD05MMC_SUMBOL_SIGMENTS + 1] = {
    {1, 2, 3, 4, 5, 6, 0, 0}, // 0
    {2, 3, 0, 0, 0, 0, 0, 0}, // 1
    {1, 2, 4, 5, 7, 0, 0, 0}, // 2
    {1, 2, 3, 4, 7, 0, 0, 0}, // 3
    {2, 3, 6, 7, 0, 0, 0, 0}, // 4
    {1, 3, 4, 6, 7, 0, 0, 0}, // 5
    {1, 3, 4, 5, 6, 7, 0, 0}, // 6
    {1, 2, 3, 0, 0, 0, 0, 0}, // 7
    {1, 2, 3, 4, 5, 6, 7, 0}, // 8
    {1, 2, 3, 4, 6, 7, 0, 0}, // 9
    {1, 2, 3, 5, 6, 7, 0, 0}, // A
    {3, 4, 5, 6, 7, 0, 0, 0}, // b
    {1, 4, 5, 6, 0, 0, 0, 0}, // C
    {2, 3, 4, 5, 7, 0, 0, 0}, // d
    {1, 4, 5, 6, 7, 0, 0, 0}, // E
    {1, 5, 6, 7, 0, 0, 0, 0}  // F
};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
//const u8 sb_s1_0[2] = {4,0x10};
const u8 sb_s1[4][DEF_MJWSD05MMC_SUMBOL_SIGMENTS*2] = {
		{3,0x10, 3,0x20, 3,0x40, 3,0x80, 3,0x08, 3,0x02, 3,0x04},
		{2,0x10, 2,0x20, 2,0x40, 2,0x80, 2,0x08, 2,0x02, 2,0x04},
		// "." 13,0x01
		// ":" 1,0x01
		{1,0x10, 1,0x20, 1,0x40, 1,0x80, 1,0x08, 1,0x02, 1,0x04},
		{17,0x01, 17,0x02, 17,0x04, 17,0x08, 0,0x08, 0,0x02, 0,0x04}
		// "°Г" 0,0x10
		// "-" 0,0x20
		// "|" 0,0x40
		// "%" 0,0x80
};

// Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
// display_buff[4] |= sb_dnd(n)
const u8 sb_dnd[7] = {0x20, 0x40, 0x80, 0x08, 0x04, 0x02, 0x01};

const u8 sb_s2[4][DEF_MJWSD05MMC_SUMBOL_SIGMENTS*2] = {
		{5,0x01, 5,0x02, 5,0x04, 5,0x08, 5,0x80, 5,0x20, 5,0x40},
		{6,0x01, 6,0x02, 6,0x04, 6,0x08, 6,0x80, 6,0x20, 6,0x40},
		// "/" 5,0x01
		{7,0x01, 7,0x02, 7,0x04, 7,0x08, 7,0x80, 7,0x20, 7,0x40},
		{8,0x01, 8,0x02, 8,0x04, 8,0x08, 8,0x80, 8,0x20, 8,0x40}
};
const u8 sb_s3[4][DEF_MJWSD05MMC_SUMBOL_SIGMENTS*2] = {
		{9,0x10, 9,0x20, 9,0x80, 9,0x10, 9,0x10, 8,0x10, 9,0x40},
		{10,0x10, 10,0x20, 10,0x40, 10,0x80, 9,0x08, 9,0x02, 9,0x04},
		// ":" 10,0x01
		{11,0x10, 11,0x20, 11,0x40, 11,0x80, 10,0x08, 10,0x02, 10,0x04},
		// "." 11,0x01
		{12,0x10, 12,0x20, 12,0x40, 12,0x80, 11,0x08, 11,0x02, 11,0x04}
};

//const u8 sb_s4_0[2] = {12,0x01};
const u8 sb_s4[4][DEF_MJWSD05MMC_SUMBOL_SIGMENTS*2] = {
		{13,0x10, 13,0x20, 13,0x40, 13,0x80, 12,0x08, 12,0x02, 12,0x04},
		{14,0x10, 14,0x20, 14,0x40, 14,0x80, 13,0x08, 13,0x02, 13,0x04},
		// "." 14,0x01
		{15,0x10, 15,0x20, 15,0x40, 15,0x80, 14,0x08, 14,0x02, 14,0x04}
		// "°Г" 15,0x01
		// "-" 15,0x02
		// "|" 15,0x04
		// "%" 15,0x08
};

/* Test cmd ():
 * 0400007ceaa49cacbcf0fcc804ffffffff,
 * 0400007c0449
 * 0400007cf3c8 - blink
 */
//const u8 lcd_init_cmd[] = {0xea,0xf0, 0xc0, 0xbc}; // original
const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xf0, // Blink control (BLKCTL): Off
		0xc0, // Mode Set (MODE SET): Display OFF, 1/3 Bias
		0xa4, // Display control (DISCTL): Normal mode, FRAME flip, Power save mode 1
		0xac, // Display control (DISCTL): Power save mode 1, FRAME flip, Power save mode 1
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xfc  // All pixel control (APCTL): Normal
};

// runtime: 600 us if I2C 450 kHz
_attribute_ram_code_ void send_to_lcd(void){
	unsigned int buff_index;
	u8 * p = display_buff;
	if(cfg.flg2.screen_off)
		return;
	if (lcd_i2c_addr) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
//		reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*400000)); // 400 kHz
		reg_i2c_id = lcd_i2c_addr;
		// LCD cmd:
		// 0xe8 - Set IC Operarion(ICSET): Do not execute Software Reset, Internal oscillator circuit;
		// 0x00 - ADSET 0
		reg_i2c_adr_dat = 0xE800;
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		for(buff_index = 0; buff_index < sizeof(display_buff) - 1; buff_index++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_do = *p;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		// LCD cmd: 0xc8 - Mode Set (MODE SET): Display ON, 1/3 Bias
		reg_i2c_adr = 0xC8;
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
}

_attribute_ram_code_
void update_lcd(void){
	if (memcmp(display_cmp_buff, display_buff, sizeof(display_buff))) {
		memcpy(display_cmp_buff, display_buff, sizeof(display_buff));
		lcd_flg.b.send_notify = lcd_flg.b.notify_on; // set flag LCD for send notify
		send_to_lcd();
	}
}

void init_lcd(void){
	lcd_i2c_addr = (u8) scan_i2c_addr(BU9792AFUV_I2C_ADDR << 1);
	if (lcd_i2c_addr) { // LCD BU9792AFUV/BL55028_I2C_ADDR ?
//		reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*400000)); // 400 kHz
		if(cfg.flg2.screen_off) {
			lcd_send_i2c_byte(0xEA); // BU9792AFUV reset
		} else {
			lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd)); // sleep: 15.5 uA
			pm_wait_us(200);
			//memset(display_buff, 0xff, sizeof(display_buff));
			send_to_lcd();
		}
	}
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) static void lcd_set_digit(u8 *buf, u8 digit, const u8 *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_MJWSD05MMC_SUMBOL_SIGMENTS; i++) {
        // get the segment needed to display the digit 'digit',
        // this is stored in the array 'digits'
        int segment = digits[digit][i] - 1;
        // segment = -1 indicates that there are no more segments to display
        if (segment >= 0) {
            segment_byte = segments[2 * segment];
            segment_bit = segments[1 + 2 * segment];
            buf[segment_byte] |= segment_bit;
        }
        else
            // there are no more segments to be displayed
            break;
    }
}

/* 0x00 = "  "
 * 0x01 = "°г"
 * 0x02 = " -"
 * 0x03 = "°c"
 * 0x04 = " |"
 * 0x05 = "°Г"
 * 0x06 = " г"
 * 0x07 = "°F"
 * 0x08 = "%"
 * 0x10 = ":" */
void show_symbol_s1(u8 symbol) {
/*	 o 0.4
	   +--- 0.4
	   |
	   ---- 0.5
	   |
	   |
	  0.6
	      o/ 0.7
	      /o
*/
	if (symbol & 1)
		display_buff[0] |= BIT(4); // "°г"
	else
		display_buff[0] &= ~BIT(4); // "°г"
	if (symbol & 2)
		display_buff[0] |= BIT(5); //"-"
	else
		display_buff[0] &= ~BIT(5); //"-"
	if (symbol & 4)
		display_buff[0] |= BIT(6); // "|"
	else
		display_buff[0] &= ~BIT(6); // "|"
	if (symbol & 8)
		display_buff[0] |= BIT(7); // "%"
	else
		display_buff[0] &= ~BIT(7); // "%"
	if (symbol & 0x10)
		display_buff[1] |= BIT(0); // ":"
	else
		display_buff[1] &= ~BIT(0); // ":"
}

void show_symbol_s3(u8 symbol) {
/* o 9.0
    +--- 9.0
    |
    ---- 7.4
    |
    |
   5.4
 */
	if (symbol & 1)
		display_buff[9] |= BIT(0); // "°г"
	else
		display_buff[9] &= ~BIT(0); // "°г"
	if (symbol & 2)
		display_buff[7] |= BIT(4); //"-"
	else
		display_buff[7] &= ~BIT(4); //"-"
	if (symbol & 4)
		display_buff[5] |= BIT(4); // "|"
	else
		display_buff[5] &= ~BIT(4); // "|"
	if (symbol & 0x10)
		display_buff[10] |= BIT(0); // ":"
	else
		display_buff[10] &= ~BIT(0); // ":"
}

void show_symbol_s4(u8 symbol) {
/* o 15.0
     +--- 15.0
     |
     ---- 15.1
     |
     |
    15.2
        o/ 15.3
        /o
 */
	if (symbol & 1)
		display_buff[15] |= BIT(0); // "°г"
	else
		display_buff[15] &= ~BIT(0); // "°г"
	if (symbol & 2)
		display_buff[15] |= BIT(1); //"-"
	else
		display_buff[15] &= ~BIT(1); //"-"
	if (symbol & 4)
		display_buff[15] |= BIT(2); // "|"
	else
		display_buff[15] &= ~BIT(2); // "|"
	if (symbol & 8)
		display_buff[15] |= BIT(3); // "%"
	else
		display_buff[15] &= ~BIT(3); // "%"
}

/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */

void show_smiley(u8 state){
/*          __ __
17.5( ^__^    ^   )17.5
      17.6  17.7 */
	if (state & 1)
		display_buff[17] |= BIT(6);
	else
		display_buff[17] &= ~(BIT(6));
	if (state & 2)
		display_buff[17] |= BIT(7);
	else
		display_buff[17] &= ~(BIT(7));
	if (state & 4)
		display_buff[17] |= BIT(5);
	else
		display_buff[17] &= ~(BIT(5));
}

void show_ble_symbol(bool state){
	if (state)
		display_buff[0] |= BIT(0);
	else
		display_buff[0] &= ~(BIT(0));
}

void show_battery_symbol(bool state){
	if (state)
		display_buff[17] |= BIT(4);
	else
		display_buff[17] &= ~(BIT(4));
}

static void clear_s1(void) {
	display_buff[0] &= BIT(0); // "ble"
	display_buff[1] = 0; // ":"
	display_buff[2] &= BIT(0); //"PM"
	display_buff[3] &= BIT(0); //"AM"
	display_buff[4] &= ~(BIT(4)); // "1"
	display_buff[13] &= ~(BIT(0)); // "."
	display_buff[17] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3)); // bat, smile
}

static void clear_s2(void) {
	display_buff[5] &= BIT(4); // s3: "|"
	display_buff[6] = 0;
	display_buff[7] &= BIT(4); // s3: "-"
	display_buff[8] &= BIT(4); // s3
}

static void clear_s3(void) {
	display_buff[7] &= ~(BIT(4)); // "-" + s2
	display_buff[8] &= ~(BIT(4)); // s3
	display_buff[9] = 0; // "°F"
	display_buff[10] = 0; // ":"
	display_buff[11] = 0;
	display_buff[12] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7)); // s4
}

static void clear_s4(void) {
	display_buff[12] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3)); // s3
	display_buff[13] &= BIT(0); //s1: "."
	display_buff[14] = 0;
	display_buff[15] = 0;
}

/* number: -999.50 .19999.50, in 0.01: -99950..1999950
 *012345
 *  -999	(-9.99)	[ -999][ -9.99]
 *   -99	(-0.99)	[ -99 ][ -0.99]
 *    -9	(-0.09)	[  -9 ][ -0.09]
 *     9	(0.09)	[   9 ][  0.09]
 *    99	(0.09)	[  99 ][  0.99]
 *   999	(9.99)	[ 999 ][  9.99]
 *  9999	(99.99)	[ 9999][ 99.99]
 * 19999	(199.99)[19999][199.99] */
void show_s1_number_x100(s32 number, u8 atr){
	u8 buf[6] = {0};
	clear_s1();
	show_symbol_s1(atr);
	if (number >= 1999950) {
		// "Hi"
		display_buff[1] |= BIT(3);
		display_buff[2] |= BIT(1) | BIT(2) | BIT(3) | BIT(5) | BIT(6);
	} else if (number <= -99950) {
		// "Lo"
		display_buff[1] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
		display_buff[2] |= BIT(1) | BIT(3) | BIT(7);
	} else if (number > 19999 || number < -999) {
		// no point, show: -999..19999
		if (number < 0){
			number = -number;
			buf[0] = 1;
		}
		number = (number + 50) / 100; // round(div 100)
		if (number > 9999) buf[1] = 1; // "1" 10000..19999
		if (number > 999) buf[2] = number / 1000 % 10;
		if (number > 99) buf[3] = number / 100 % 10;
		if (number > 9)	buf[4] = number / 10 % 10;
		buf[5] = number % 10;
		/*
		 *012345
		 *- -999	[ -999]
		 *-  -99	[ -99 ]
		 *-   -9	[  -9 ]
		 *     9	[   9 ]
		 *    99	[  99 ]
		 *   999	[ 999 ]
		 *  9999	[ 9999]
		 * 19999	[19999] */
		if(buf[1] || buf[2]) {
			// 1000..19999
			if(buf[1]) display_buff[4] |= BIT(4); // "1" 10000..19999
			lcd_set_digit(display_buff, buf[2], sb_s1[0]);
			lcd_set_digit(display_buff, buf[3], sb_s1[1]);
			lcd_set_digit(display_buff, buf[4], sb_s1[2]);
			lcd_set_digit(display_buff, buf[5], sb_s1[3]);
		} else { // -999..999
			if(buf[0]) {
				if(buf[3]) { // -999..-100
					display_buff[3] |= BIT(2); // "-"
					lcd_set_digit(display_buff, buf[3], sb_s1[1]);
					lcd_set_digit(display_buff, buf[4], sb_s1[2]);
					lcd_set_digit(display_buff, buf[5], sb_s1[3]);
				} else { // -99..-10
					if(buf[4]) {
						display_buff[3] |= BIT(2); // "-"
						lcd_set_digit(display_buff, buf[4], sb_s1[1]);
					} else // -9..-1
						display_buff[2] |= BIT(2); // "-"
					lcd_set_digit(display_buff, buf[5], sb_s1[2]);
				}
			} else { // 0..999
				if(buf[3]) { // 10000..19999
					lcd_set_digit(display_buff, buf[3], sb_s1[1]);
					lcd_set_digit(display_buff, buf[4], sb_s1[2]);
				} else if(buf[4])
					lcd_set_digit(display_buff, buf[4], sb_s1[2]);
				lcd_set_digit(display_buff, buf[5], sb_s1[3]);
			}
		}
	} else { // show: -9.99..199.99
		display_buff[13] |= BIT(0); // point
		if (number < 0){
			number = -number;
			buf[0] = 1;
			display_buff[3] |= BIT(2); // "-"
		}
		if (number > 9999) buf[1] = 1; // "1" 10000..19999
		if (number > 999) buf[2] = number / 1000 % 10;
		if (number > 99) buf[3] = number / 100 % 10;
		if (number > 9)	buf[4] = number / 10 % 10;
		buf[5] = number % 10;
		/*
		 *012345
		 *- -999	(-9.99)	[ -9.99]
		 *-  -99	(-0.99)	[ -0.99]
		 *-   -9	(-0.09)	[ -0.09]
		 *     9	(0.09)	[  0.09]
		 *    99	(0.09)	[  0.99]
		 *   999	(9.99)	[  9.99]
		 *  9999	(99.99)	[ 99.99]
		 * 19999	(199.99)[199.99] */
		if(!buf[0]) { // 0..199.99
			if(buf[1]) {
				display_buff[4] |= BIT(4); // "1" 100.00..199.99
				lcd_set_digit(display_buff, buf[2], sb_s1[0]);
			} else if(buf[2])
				lcd_set_digit(display_buff, buf[2], sb_s1[0]);
		}
		lcd_set_digit(display_buff, buf[3], sb_s1[1]);
		lcd_set_digit(display_buff, buf[4], sb_s1[2]);
		if(buf[5]) lcd_set_digit(display_buff, buf[5], sb_s1[3]);
	}
}

/* number in 0.1 (-995..1995) show: -99..-9.9..199.9..1999 */
void show_s1_number_x10(s32 number, u8 atr){
	clear_s1();
	show_symbol_s1(atr);
	if (number > 99995) {
		// "Hi"
		display_buff[1] |= BIT(3);
		display_buff[2] |= BIT(1) | BIT(2) | BIT(3) | BIT(5) | BIT(6);
	} else if (number < -9995) {
		// "Lo"
		display_buff[1] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
		display_buff[2] |= BIT(1) | BIT(3) | BIT(7);
	} else {
		if (number > 1999 || number < -99) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[3] |= BIT(2); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[13] |= BIT(0); // point
			if (number < 0){
				number = -number;
				display_buff[3] |= BIT(2); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[4] |= BIT(4); // "1" 1000..1999
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s1[0]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s1[1]);
		else lcd_set_digit(display_buff, 0, sb_s1[1]);
		lcd_set_digit(display_buff, number % 10, sb_s1[2]);
	}
}


/* number in 0.1 (-9995..29995) show: -999..-99.9..299.9..2999 */
void show_s3_number_x10(s32 number, u8 atr){
	clear_s3();
	show_symbol_s3(atr);
	if (number >= 29995) {
		// "Hi"
		display_buff[10] |= BIT(1) | BIT(2) | BIT(3);
		display_buff[11] |= BIT(3) | BIT(5) | BIT(6);
	} else if (number <= -9995) {
		// "Lo"
		display_buff[10] |= BIT(1) | BIT(3);
		display_buff[11] |= BIT(2) | BIT(3) | BIT(7);
		display_buff[12] |= BIT(6) | BIT(7);
	} else {
		if (number > 2999 || number < -999) {
			// no point, show: -999..2999
			if (number < 0){
				number = -number;
				display_buff[9] |= BIT(6); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -99.9..299.9
			display_buff[11] |= BIT(0); // point
			if (number < 0){
				number = -number;
				display_buff[9] |= BIT(6); // "-"
			}
		}
		/* number: -999..2999 */
		if (number > 999) lcd_set_digit(display_buff, number / 1000 % 10, sb_s3[0]);
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s3[1]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s3[2]);
		else lcd_set_digit(display_buff, 0, sb_s3[2]);
		lcd_set_digit(display_buff, number % 10, sb_s3[3]);
	}
}

/* number in 0.1 (-99.5 .1999.5) -99..-9.9..199.9..1999 */
void show_s4_number_x10(s32 number, u8 atr){
	clear_s4();
	show_symbol_s4(atr);
	if (number > 19995) {
		// "Hi"
		display_buff[13] |= BIT(1) | BIT(2) | BIT(3);
		display_buff[14] |= BIT(3) | BIT(5) | BIT(6);
	} else if (number < -9995) {
		// "Lo"
		display_buff[13] |= BIT(1) | BIT(3) | BIT(7);
		display_buff[14] |= BIT(2) | BIT(3);
		display_buff[15] |= BIT(6) | BIT(7);
	} else {
		if (number > 999 || number < -1999) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[12] |= BIT(2); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.99..199.99
			display_buff[14] |= BIT(0); // point
			if (number < 0){
				number = -number;
				display_buff[12] |= BIT(2); // "-"
			}
		}
		/* number: -9999..19999 */
		if (number > 999) display_buff[12] |= BIT(0); // "1" 10000..19999
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s4[0]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s4[1]);
		else lcd_set_digit(display_buff, 0, sb_s4[1]);
		lcd_set_digit(display_buff, number % 10, sb_s4[2]);
	}
}

void show_clock_s3(void) {
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
	u8 hrs = rtc.hours;
	if(cfg.flg.time_am_pm) {
		if(hrs > 12)
			hrs -= 12;
		else if(!hrs)
			hrs = 12;
	}
	clear_s3();
	display_buff[10] = BIT(0); // ":"
	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s3[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s3[1]);
	lcd_set_digit(display_buff, rtc.minutes / 10 % 10, sb_s3[2]);
	lcd_set_digit(display_buff, rtc.minutes % 10, sb_s3[3]);
#else
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = tmp / 60 % 24;

	clear_s3();
	display_buff[10] = BIT(0); // ":"

	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s3[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s3[1]);
	lcd_set_digit(display_buff, min / 10 % 10, sb_s3[2]);
	lcd_set_digit(display_buff, min % 10, sb_s3[3]);
#endif
}

void show_clock_s1(void) {
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
	clear_s1();
	display_buff[1] = BIT(0); // ":"
	u8 hrs = rtc.hours;
	if(cfg.flg.time_am_pm) {
		if(hrs > 12)
			hrs -= 12;
		else if(!hrs)
			hrs = 12;
	}
	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s1[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s1[1]);
	lcd_set_digit(display_buff, rtc.minutes / 10 % 10, sb_s1[2]);
	lcd_set_digit(display_buff, rtc.minutes % 10, sb_s1[3]);
#else
	u32 tmp = wrk.utc_time_sec / 60;
	u32 min = tmp % 60;
	u32 hrs = tmp / 60 % 24;

	clear_s1();
	display_buff[1] = BIT(0); // ":"

	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s1[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s1[1]);
	lcd_set_digit(display_buff, min / 10 % 10, sb_s1[2]);
	lcd_set_digit(display_buff, min % 10, sb_s1[3]);
#endif
}

void show_data_s2(u8 flg) {
	u8 mh, ml, dh, dl;
	clear_s2();
	display_buff[6] = BIT(4); // "/"
	display_buff[4] &= BIT(4); // s1: "1"
	if((flg & MASK_FLG3_WEEKDAY) == 0) {
		display_buff[4] |= sb_dnd[rtc.weekday];
	}

	if (rtc.month >= 10) {
		mh = 1;
		ml = rtc.month - 10;
	} else {
		mh = 0;
		ml = rtc.month;
	}
	dh = 0;
	dl = rtc.days;
	while(dl >= 10) {
		dl -= 10;
		dh++;
	}
	lcd_set_digit(display_buff, mh, sb_s2[0]);
	lcd_set_digit(display_buff, ml, sb_s2[1]);
	lcd_set_digit(display_buff, dh, sb_s2[2]);
	lcd_set_digit(display_buff, dl, sb_s2[3]);
}

void show_battery_s1(u8 level) {
	clear_s1();
	display_buff[0] |= BIT(7); // "%"
	if(level > 99)
		lcd_set_digit(display_buff, 1, sb_s1[0]);
	lcd_set_digit(display_buff, level / 10 % 10, sb_s1[1]);
	lcd_set_digit(display_buff, level % 10, sb_s1[2]);
}

void show_low_bat(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	// s1 "Lo"
	display_buff[1] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
	display_buff[2] |= BIT(1) | BIT(3) | BIT(7);
	// "bat"
	display_buff[17] |= BIT(4);
	//show_s1_number_x100(measured_data.average_battery_mv * 10, 0);
	send_to_lcd();
}

void show_ota_screen(void) {
	clear_s1();
	display_buff[0] = BIT(2);
	display_buff[1] = BIT(2);
	display_buff[2] = BIT(2);
	display_buff[3] = BIT(2);
	send_to_lcd();
	lcd_send_i2c_byte(0xf2);
}

void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

_attribute_ram_code_
u8 is_comfort(s16 t, u16 h) {
	u8 ret = LCD_SYM_SMILEY_SAD;
	if (t >= cmf.t[0] && t <= cmf.t[1] && h >= cmf.h[0] && h <= cmf.h[1])
		ret = LCD_SYM_SMILEY_HAPPY;
	return ret;
}

_attribute_ram_code_
__attribute__((optimize("-O2")))
void lcd(void) {
	if(cfg.flg2.screen_off) {
		return;
	}
	u8 screen_type = cfg.flg2.screen_type;
	if(lcd_flg.chow_ext_ut >= wrk.utc_time_sec)
		screen_type = SCR_TYPE_EXT;
	show_ble_symbol(wrk.ble_connected
#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
		 || (ext_key.rest_adv_int_tad & 2)
#endif
	);
	show_battery_symbol(measured_data.average_battery_mv < 2400);
	switch(screen_type) {
		case SCR_TYPE_TEMP:
			if (cfg.flg.temp_F_or_C) {
				if(cfg.flg.x100)
					show_s1_number_x100(((measured_data.temp / 5) * 9) + 3200, LCD_SYM_F); // convert C to F
				else
					show_s1_number_x10(((measured_data.temp_x01 / 5) * 9) + 320, LCD_SYM_F); // convert C to F
			} else {
				if(cfg.flg.x100)
					show_s1_number_x100(measured_data.temp, LCD_SYM_C);
				else
					show_s1_number_x10(measured_data.temp_x01, LCD_SYM_C);
			}
			show_clock_s3();
			show_s4_number_x10(measured_data.humi_x01, LCD_SYM_P);
			break;
		case SCR_TYPE_HUMI:
			if(cfg.flg.x100)
				show_s1_number_x100(measured_data.humi, LCD_SYM_P);
			else
				show_s1_number_x10(measured_data.humi_x01, LCD_SYM_P);
			show_clock_s3();
			if (cfg.flg.temp_F_or_C)
				show_s4_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, LCD_SYM_F); // convert C to F
			else
				show_s4_number_x10(measured_data.temp_x01, LCD_SYM_C);
			break;
		case SCR_TYPE_BAT_P:
			show_battery_s1(measured_data.battery_level);
			show_battery_symbol(1);
			if (cfg.flg.temp_F_or_C)
				show_s3_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, LCD_SYM_F); // convert C to F
			else
				show_s3_number_x10(measured_data.temp_x01, LCD_SYM_C);
			show_s4_number_x10(measured_data.humi_x01, LCD_SYM_P);
			break;
		case SCR_TYPE_BAT_V:
			show_s1_number_x100((measured_data.average_battery_mv / 10), 0);
			show_battery_symbol(1);
			if (cfg.flg.temp_F_or_C)
				show_s3_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, LCD_SYM_F); // convert C to F
			else
				show_s3_number_x10(measured_data.temp_x01, LCD_SYM_C);
			show_s4_number_x10(measured_data.humi_x01, LCD_SYM_P);
			break;
		case SCR_TYPE_EXT:
			show_s1_number_x100(ext.number, ext.flg.temp_symbol);
			show_battery_symbol(ext.flg.battery);
			show_smiley(ext.flg.smiley);
			if (cfg.flg.temp_F_or_C)
				show_s3_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, LCD_SYM_F); // convert C to F
			else
				show_s3_number_x10(measured_data.temp_x01, LCD_SYM_C);
			show_s4_number_x10(measured_data.humi_x01, LCD_SYM_P);
			break;
		default: // SCR_TYPE_TIME
			show_clock_s1();
			if (cfg.flg.temp_F_or_C)
				show_s3_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, LCD_SYM_F); // convert C to F
			else
				show_s3_number_x10(measured_data.temp_x01, LCD_SYM_C);
			show_s4_number_x10(measured_data.humi_x01, LCD_SYM_P);
	}
	display_buff[3] &= ~(BIT(0));
	display_buff[2] &= ~(BIT(0));
	if(cfg.flg.time_am_pm) {
		if(rtc.hours >= 12) {
			display_buff[2] |= BIT(0);
		} else {
			display_buff[3] |= BIT(0);
		}
	}
	if (screen_type != SCR_TYPE_EXT) {
		if (cfg.flg.comfort_smiley) // smiley = comfort
			show_smiley(is_comfort(measured_data.temp, measured_data.humi));
		else
			show_smiley(LCD_SYM_SMILEY_NONE);
	}
	show_data_s2(cfg.flg3.not_day_of_week);
}


#endif // DEVICE_TYPE == DEVICE_MJWSD05MMC_EN
