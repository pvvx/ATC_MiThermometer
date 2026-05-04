#include "tl_common.h"
#include "app_config.h"
#if (DEVICE_TYPE == DEVICE_LYWSD02MMC)

#include "app.h"
#include "rtc.h"
#include "lcd.h"
#include "battery.h"
#include "drivers/8258/pm.h"
#include "drivers/8258/timer.h"
#include "stack/ble/ll/ll_pm.h"

#define DEF_EPD_REFRESH_CNT 2048

#define LOW     0
#define HIGH    1

#define LCD_SYM_SMILEY_NONE  0 	// "     " happy
#define LCD_SYM_SMILEY_HAPPY 5 	// "(^-^)" happy
#define LCD_SYM_SMILEY_SAD   6 	// "(-^-)" sad

/* EPD LYWSD02MMC buffer:  byte.bit

top symbols (s1):

     --6.0--         --6.7--            --8.1--         --9.0--       bat 10.6 ful 10.7
   |         |     |         |  7.4   |         |     |         |         Q 11.0
  5.6       6.3   6.5       7.2  o   7.7       8.4   8.6       9.3        % 10.4
   |         |     |         |        |         |     |         |
     --6.1--         --7.0--    7.5     --8.2--         --9.1--     o10.5 --9.7
   |         |     |         |   o    |         |     |         |     9.5|   |10.2
  5.7       6.4   6.6       7.3      8.0       8.5   8.7       9.4        --10.0
   |         |     |         |  7.6   |         |     |         |     9.6|   |10.3
     --6.2--         --7.1--     *      --8.3--         --9.2--           --10.1

bottom symbols (s2):

 11.1    -11.4--         -12.3--        -13.2--
   |   |         |     |         |    |         |
   | 11.2      11.7  12.1      12.6 13.0      13.5
   |   |         |     |         |    |         |
   |     -11.5--         -12.4--        -13.3--
   |   |         |     |         |    |         |   % 13.7 TDS 14.1
   | 11.3      12.0  12.2      12.7 13.1      13.6
   |   |         |     |         |    |         |   PM2.5 14.0
         -11.6--         -12.5--        -13.4--

bottom symbols (s3):

     --0.2--         --1.1--
   |         |     |         |
  0.0       0.5   0.7       1.4
   |         |     |         |
     --0.3--         --1.2--
   |         |     |         |
  0.1       0.6   1.0       1.5 1.6
   |         |     |         |   o/
     --0.4--         --1.3--     /o

bottom symbols (s4):

  1.7    --2.2--         --3.1--           --4.1--
   |   |         |     |         |       |         |
   |  2.0       2.5   2.7       3.4     3.7       4.4  F 4.7
   |   |         |     |         |       |         |
   |     --2.3--         --3.2--           --4.2--
   |   |         |     |         |       |         |
   |  2.1       2.6   3.0       3.5     4.0       4.5
   |   |         |     |         |  3.6  |         |   C 4.6
         --2.4--         --3.3--     *     --4.3--

        5.5 5.4  5.4 5.5
          / \      / \
    5.0(  ___  5.2 ___  )5.0
          5.3  / \ 5.3
               ___
               5.1

None: 14.2..14.7
 buf[15] = 0xff or 0x00 black
*/

#define DEF_LYWSD02MMC_SUMBOL_SIGMENTS	7
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

typedef enum {
	SYMB_0 = 0,
	SYMB_1,
	SYMB_2,
	SYMB_3,
	SYMB_4,
	SYMB_5,
	SYMB_6,
	SYMB_7,
	SYMB_8,
	SYMB_9,
	SYMB_A,
	SYMB_b,
	SYMB_C,
	SYMB_d,
	SYMB_E,
	SYMB_F,
	SYMB_H,
	SYMB_i,
	SYMB_L,
	SYMB_o,
	SYMB_U,
	SYMB_MAX
} SYMB_NUM_e;

const u8 digits[21][DEF_LYWSD02MMC_SUMBOL_SIGMENTS + 1] = {
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
    {1, 5, 6, 7, 0, 0, 0, 0}, // F
    {2, 3, 5, 6, 7, 0, 0, 0}, // H
    {5, 0, 0, 0, 0, 0, 0, 0}, // i
    {4, 5, 6, 0, 0, 0, 0, 0}, // L
    {3, 4, 5, 7, 0, 0, 0, 0}, // o
    {2, 3, 4, 5, 6, 0, 0, 0}, // U
};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const u8 sb_s1[5][DEF_LYWSD02MMC_SUMBOL_SIGMENTS*2] = {
		{6,BIT(0), 6,BIT(3), 6,BIT(4), 6,BIT(2), 5,BIT(7), 5,BIT(6), 6,BIT(1)},
		{6,BIT(7), 7,BIT(2), 7,BIT(3), 7,BIT(1), 6,BIT(6), 6,BIT(5), 7,BIT(0)},
		// "°" 7,BIT(4) "*" 7,BIT(5) "." 7,BIT(6)
		{8,BIT(1), 8,BIT(4), 8,BIT(5), 8,BIT(3), 8,BIT(0), 7,BIT(7), 8,BIT(2)},
		{9,BIT(0), 9,BIT(3), 9,BIT(4), 9,BIT(2), 8,BIT(7), 8,BIT(6), 9,BIT(1)},
		// "°" 10,BIT(5)
		{9,BIT(7), 10,BIT(2), 10,BIT(3), 10,BIT(1), 9,BIT(6), 9,BIT(5), 10,BIT(0)},
		// bat 10,BIT(6), full 10,BIT(7)
		// "Q" 10,BIT(0)
		// "%" 10,BIT(4)
};

const u8 sb_s2[3][DEF_LYWSD02MMC_SUMBOL_SIGMENTS*2] = {
		// "1" 11,BIT(1)
		{11,BIT(4), 11,BIT(7), 12,BIT(0), 11,BIT(6), 11,BIT(3), 11,BIT(2), 11,BIT(5)},
		{12,BIT(3), 12,BIT(6), 12,BIT(7), 12,BIT(5), 12,BIT(2), 12,BIT(1), 12,BIT(4)},
		{13,BIT(2), 13,BIT(5), 13,BIT(6), 13,BIT(4), 13,BIT(1), 13,BIT(0), 13,BIT(3)},
		// "%" 13,BIT(7)
		// "TDS" 14,BIT(1)
		// "PM2.5" 14,BIT(0)
};

const u8 sb_s3[2][DEF_LYWSD02MMC_SUMBOL_SIGMENTS*2] = {
		{0,BIT(2), 0,BIT(5), 0,BIT(6), 0,BIT(4), 0,BIT(1), 0,BIT(0), 0,BIT(3)},
		{1,BIT(1), 1,BIT(4), 1,BIT(5), 1,BIT(3), 1,BIT(0), 0,BIT(7), 1,BIT(2)},
		// "%" 1,BIT(6)
};

const u8 sb_s4[3][DEF_LYWSD02MMC_SUMBOL_SIGMENTS*2] = {
		// "1" 1,BIT(7)
		{2,BIT(2), 2,BIT(5), 2,BIT(6), 2,BIT(4), 2,BIT(1), 2,BIT(0), 2,BIT(3)},
		{3,BIT(1), 3,BIT(4), 3,BIT(5), 3,BIT(3), 3,BIT(0), 2,BIT(7), 3,BIT(2)},
		// "." 3,BIT(6)
		{4,BIT(1), 4,BIT(4), 4,BIT(5), 4,BIT(3), 4,BIT(0), 3,BIT(7), 4,BIT(2)},
		// "°C" 4,BIT(6)
		// "°F" 4,BIT(7)
};

RAM lcd_flg_t lcd_flg;

RAM u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];

RAM u8 stage_lcd;
RAM u8 epd_updated;

#ifdef 	DEF_EPD_REFRESH_CNT
RAM u16 lcd_refresh_cnt;
#endif

//----------------------------------
// T_LUT_init, T_LUT_work values taken from the actual device with a
// logic analyzer
//----------------------------------
const u8 T_LUT_init[14] = {0x082, 0x068, 0x050, 0x0E8, 0x0D0, 0x0A8, 0x065, 0x07B, 0x081, 0x0E4, 0x0E7, 0x00E, 0x0AC, 0x02B};
const u8 T_LUT_work[14] = {0x082, 0x080, 0x000, 0x0C0, 0x080, 0x080, 0x062, 0x07B, 0x081, 0x0E4, 0x0E7, 0x00E, 0x0AC, 0x02B};

#ifdef I2C_GROUP
_attribute_ram_code_ void sleep_us16(unsigned int us16)
{
	unsigned int t = reg_system_tick;
	while((unsigned int)(reg_system_tick - t) < us16) {
	}
}
#else
extern void sleep_us16(unsigned int us16);
#endif

#define delay_SPI_end_cycle() sleep_us16(20)
#define delay_EPD_SCL_pulse() sleep_us16(10)

//----------------------------------

_attribute_ram_code_
__attribute__((optimize("-Os")))
static void lcd_set_digit(u8 *buf, u8 digit, const u8 *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_LYWSD02MMC_SUMBOL_SIGMENTS; i++) {
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

static void clear_s1(void) {
	display_buff[5] &= ~(BIT(6) | BIT(7));
	display_buff[6] = 0; // ":"
	display_buff[7] = 0;
	display_buff[8] = 0;
	display_buff[9] = 0;
	display_buff[10] &= BIT(6) | BIT(7); // bat
}

static void clear_s2(void) {
	display_buff[11] &= BIT(0); // "Q"
	display_buff[12] = 0;
	display_buff[13] = 0; // %
}

static void clear_s3(void) {
	display_buff[0] = 0;
	display_buff[1] &= BIT(7); // "%", "1" s4
}

static void clear_s4(void) {
	display_buff[1] &= ~BIT(7); // "1"
	display_buff[2] = 0;
	display_buff[3] = 0; // "."
	display_buff[4] = 0; // "F/C"
}

_attribute_ram_code_
void show_ble_symbol(bool state){
	if (state)
		display_buff[11] |= BIT(0); // "connect"
	else
		display_buff[11] &= ~BIT(0);
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
	switch(state & 3) {
	case 1:
		display_buff[5] |= BIT(1) | BIT(4) | BIT(5);
		display_buff[5] &= ~(BIT(2) | BIT(3));
		break;
	case 2:
		display_buff[5] &= ~(BIT(1) | BIT(4) | BIT(5));
		display_buff[5] |= (BIT(2) | BIT(3));
		break;
	case 3:
		display_buff[5] |= BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5);
		break;
	default: // case 0:
		display_buff[5] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5));
		break;
	}
	if (state & 4)
		display_buff[5] |= BIT(0);
	else
		display_buff[5] &= ~(BIT(0));
}

_attribute_ram_code_
u8 is_comfort(s16 t, u16 h) {
	u8 ret = LCD_SYM_SMILEY_SAD;
	if (t >= cmf.t[0] && t <= cmf.t[1] && h >= cmf.h[0] && h <= cmf.h[1])
		ret = LCD_SYM_SMILEY_HAPPY;
	return ret;
}

_attribute_ram_code_
void show_battery_symbol(bool state) {
	display_buff[10] &= ~(BIT(6) | BIT(7));
	if (state) {
		display_buff[10] |= BIT(6);
		if (measured_data.battery_level > 16) {
			display_buff[10] |= BIT(7);
		}
	}
}

/* atr:
 0x00 - " "
 0x01 - "%"
 0x02 - "C"
 0x03 - "F"
*/
static void show_atr_s1(u8 atr) {
	switch(atr) {
	case 0x01:
		display_buff[10] |= BIT(4); // "%"
		break;
	case 0x02:
		lcd_set_digit(display_buff, SYMB_C, sb_s1[4]);
		display_buff[10] |= BIT(5); // "°"
		break;
	case 0x03:
		lcd_set_digit(display_buff, SYMB_F, sb_s1[4]);
		display_buff[10] |= BIT(5); // "°"
		break;
	}
}

/* number: -999.50..9999.50, in 0.01: -99950..999950
 *  0123
 *  -999	(-9.99)	[ -999][ -9.99]
 *   -99	(-0.99)	[ -99 ][ -0.99]
 *    -9	(-0.09)	[  -9 ][ -0.09]
 *     9	(0.09)	[   9 ][  0.09]
 *    99	(0.09)	[  99 ][  0.99]
 *   999	(9.99)	[ 999 ][  9.99]
 *  9999	(99.99)	[ 9999][ 99.99]

 * atr:
  0.. SYMB_NUM_e
  0x00 - " "
  0x01 - "%"
  0x02 - "C"
  0x03 - "F"
 */
static void show_s1_number_x100(s32 number, u8 atr){
	u8 buf[5] = {0};
	clear_s1();
	show_atr_s1(atr);
	if (number >= 999950) {
		// "Hi"
		lcd_set_digit(display_buff, SYMB_H, sb_s1[1]);
		lcd_set_digit(display_buff, SYMB_i, sb_s1[2]);
	} else if (number <= -99950) {
		// "Lo"
		lcd_set_digit(display_buff, SYMB_L, sb_s1[1]);
		lcd_set_digit(display_buff, SYMB_o, sb_s1[2]);
	}
	if (number < 0){
		number = -number;
		buf[0] = 1;
	}
	if (number > 9999 || number < -999) {
		// no point, show: -999..19999
		if (number < 0){
			number = -number;
			buf[0] = 1;
		}
		number = (number + 50) / 100; // round(div 100)
		if (number > 999) buf[1] = number / 1000 % 10;
		if (number > 99) buf[2] = number / 100 % 10;
		if (number > 9)	buf[3] = number / 10 % 10;
		buf[4] = number % 10;
		/*
		 *012345
		 *- -999	[ -999]
		 *-  -99	[ -99 ]
		 *-   -9	[  -9 ]
		 *     9	[   9 ]
		 *    99	[  99 ]
		 *   999	[ 999 ]
		 *  9999	[ 9999] */
		if(buf[1]) {
			// 1000..9999
			lcd_set_digit(display_buff, buf[1], sb_s1[0]);
			lcd_set_digit(display_buff, buf[2], sb_s1[1]);
			lcd_set_digit(display_buff, buf[3], sb_s1[2]);
		} else { // -999..999
			if(buf[0]) { // number < 0
				if(buf[2]) { // -0999..-0100
					display_buff[6] |= BIT(1); // "-"
					lcd_set_digit(display_buff, buf[2], sb_s1[1]);
					lcd_set_digit(display_buff, buf[3], sb_s1[2]);
				} else { // -99..-10
					if(buf[3]) {
						display_buff[7] |= BIT(0); // "-"
						lcd_set_digit(display_buff, buf[3], sb_s1[2]);
					} else // -9..-1
						display_buff[8] |= BIT(2); // "-"
				}
			} else { // 0..999
				if(buf[2]) { // 100..999
					lcd_set_digit(display_buff, buf[2], sb_s1[1]);
					lcd_set_digit(display_buff, buf[3], sb_s1[2]);
				} else if(buf[3])
					lcd_set_digit(display_buff, buf[3], sb_s1[2]);
			}
		}
		lcd_set_digit(display_buff, buf[4], sb_s1[3]);
	} else { // show: -9.99..199.99
		display_buff[7] |= BIT(6); // point
		if (buf[0]){
			display_buff[3] |= BIT(2); // "-"
		}
		if (number > 999) buf[1] = number / 1000 % 10;
		if (number > 99) buf[2] = number / 100 % 10;
		if (number > 9)	buf[3] = number / 10 % 10;
		buf[4] = number % 10;
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
				lcd_set_digit(display_buff, buf[1], sb_s1[0]);
			}
		}
		lcd_set_digit(display_buff, buf[2], sb_s1[1]);
		lcd_set_digit(display_buff, buf[3], sb_s1[2]);
		if(buf[4]) lcd_set_digit(display_buff, buf[4], sb_s1[3]);
	}
}

void show_low_bat(void) {
	display_buff[10] |= BIT(6); // low bat
	show_s1_number_x100(measured_data.battery_mv* 100, 0);
	lcd_set_digit(display_buff, SYMB_U, sb_s1[4]);
	while(task_lcd()) pm_wait_ms(10);
}

static void show_clock_s1(void) {
	u8 hrs = rtc.hours;
	clear_s1();
	if(cfg.flg.time_am_pm) {
		if(hrs > 12)
			hrs -= 12;
		else if(!hrs)
			hrs = 12;
		if(cfg.flg3.show_day_of_week) // 0 = Sunday
			lcd_set_digit(display_buff, rtc.weekday + 1, sb_s1[4]);
	} else {
		if(cfg.flg3.show_day_of_week) // 0 = Monday
			lcd_set_digit(display_buff, (rtc.weekday) ? rtc.weekday : 7, sb_s1[4]);
	}
	display_buff[7] = BIT(4) | BIT(5); // ":"
	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s1[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s1[1]);
	lcd_set_digit(display_buff, rtc.minutes / 10 % 10, sb_s1[2]);
	lcd_set_digit(display_buff, rtc.minutes % 10, sb_s1[3]);
}

/* number in x1 (0..1999)*/
static void show_s2_number(u32 number, u8 atr){
	clear_s2();
	if(atr)
		display_buff[13] |= BIT(7); // "%"

	if (number > 1999) {
		// "Hi"
		lcd_set_digit(display_buff, SYMB_H, sb_s4[1]);
		lcd_set_digit(display_buff, SYMB_i, sb_s4[2]);
	} else {
		/* number: 0..1999 */
		if (number > 999) display_buff[11] |= BIT(1); // "1" 1000..1999
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s2[0]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s2[1]);
		//else lcd_set_digit(display_buff, 0, sb_s2[1]);
		lcd_set_digit(display_buff, number % 10, sb_s2[2]);
	}
}

static void show_s2_mmdd(void) {
	u8 ml, dh, dl;
	clear_s2();
	if (rtc.month >= 10) {
		display_buff[11] |= BIT(1); // "1"
		ml = rtc.month - 10;
	} else {
		ml = rtc.month;
	}
	dh = 0;
	dl = rtc.days;
	while(dl >= 10) {
		dl -= 10;
		dh++;
	}
	lcd_set_digit(display_buff, ml, sb_s2[0]);
	lcd_set_digit(display_buff, dh, sb_s2[1]);
	lcd_set_digit(display_buff, dl, sb_s2[2]);
}

/* number in x1 (0..99)*/
static void show_s3_number(u32 number, u8 atr){
	clear_s3();
	if(atr)
		display_buff[1] |= BIT(6); // "%"

	if (number > 99) {
		// "Hi"
		lcd_set_digit(display_buff, SYMB_H, sb_s3[0]);
		lcd_set_digit(display_buff, SYMB_i, sb_s3[1]);
	} else {
		/* number: 0..99 */
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s3[0]);
		//else lcd_set_digit(display_buff, 0, sb_s3[0]);
		lcd_set_digit(display_buff, number % 10, sb_s3[1]);
	}
}

/* number in 0.1 (-995..1995) show: -99..-9.9..199.9..1999 [C/F]*/
static void show_s4_number_x10(s32 number, u8 atr){
	clear_s4();

	switch(atr) {
	case 1:
		display_buff[4] |= BIT(6); // "C"
		break;
	case 2:
		display_buff[4] |= BIT(7); // "F"
		break;
	}

	if (number > 99995) {
		// "Hi"
		lcd_set_digit(display_buff, SYMB_H, sb_s4[0]);
		lcd_set_digit(display_buff, SYMB_i, sb_s4[1]);
	} else if (number < -9995) {
		// "Lo"
		lcd_set_digit(display_buff, SYMB_L, sb_s4[0]);
		lcd_set_digit(display_buff, SYMB_o, sb_s4[1]);
	} else {
		if (number > 1999 || number < -99) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[2] |= BIT(3); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[3] |= BIT(6); // point
			if (number < 0){
				number = -number;
				display_buff[2] |= BIT(3); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[1] |= BIT(7); // "1" 1000..1999
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s4[0]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s4[1]);
		else lcd_set_digit(display_buff, 0, sb_s4[1]);
		lcd_set_digit(display_buff, number % 10, sb_s4[2]);
	}
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
static void transmit(u8 cd, u8 data_to_send) {
    gpio_write(EPD_SCL, LOW);
    gpio_write(EPD_CSB, LOW);
    delay_EPD_SCL_pulse();

    // send the first bit, this indicates if the following is a command or data
    gpio_write(EPD_SDA, cd);
    delay_EPD_SCL_pulse();
    gpio_write(EPD_SCL, HIGH);
    delay_EPD_SCL_pulse();

    // send 8 bits
    for (int i = 0; i < 8; i++) {
        // start the clock cycle
        gpio_write(EPD_SCL, LOW);
        // set the MOSI according to the data
        if (data_to_send & 0x80)
            gpio_write(EPD_SDA, HIGH);
        else
            gpio_write(EPD_SDA, LOW);
        // prepare for the next bit
        data_to_send = (data_to_send << 1);
        delay_EPD_SCL_pulse();
        // the data is read at rising clock (halfway the time MOSI is set)
        gpio_write(EPD_SCL, HIGH);
        delay_EPD_SCL_pulse();
    }

    // finish by ending the clock cycle and disabling SPI
    gpio_write(EPD_SCL, LOW);
    delay_SPI_end_cycle();
    gpio_write(EPD_CSB, HIGH);
    delay_SPI_end_cycle();
}

_attribute_ram_code_
static void transmit_blk(u8 cd, const u8 * pdata, size_t size_data) {
	for (int i = 0; i < size_data; i++)
		transmit(cd, pdata[i]);
}

_attribute_ram_code_
void update_lcd(void){
	if(cfg.flg2.screen_off) {
		stage_lcd = 0;
		return;
	}
	if (!stage_lcd) {
		if (memcmp(display_cmp_buff, display_buff, sizeof(display_buff))) {
			memcpy(display_cmp_buff, display_buff, sizeof(display_buff));
			lcd_flg.b.send_notify = lcd_flg.b.notify_on; // set flag LCD for send notify
#ifdef 	DEF_EPD_REFRESH_CNT
			if (lcd_refresh_cnt) {
				lcd_refresh_cnt--;
			} else {
				init_lcd(); // pulse RST_N low for 110 microseconds
//				lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
//			    epd_updated = 0;
			}
#endif
			stage_lcd = 1;
		}
	}
}

void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
    sleep_us(110);
#ifdef 	DEF_EPD_REFRESH_CNT
	lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
#endif
    stage_lcd = 1;
    epd_updated = 0;
    gpio_write(EPD_RST, HIGH);
	display_buff[15] = 0;
    bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
}

void reinit_lcd(void) {
	memset(display_cmp_buff, 0, sizeof(display_cmp_buff));
	init_lcd();
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
int task_lcd(void) {
	if (gpio_read(EPD_BUSY)) {
		switch (stage_lcd) {
		case 1: // Update/Init, stage 1
			if (epd_updated == 0) {
				transmit_blk(0, T_LUT_init, sizeof(T_LUT_init));
			} else {
				transmit_blk(0, T_LUT_work, sizeof(T_LUT_work));
			}
			transmit(0, 0x040);
			transmit(0, 0x0A9);
			transmit(0, 0x0A8);
			transmit_blk(1, display_cmp_buff, sizeof(display_cmp_buff));
			transmit(0, 0x0AB);
			transmit(0, 0x0AA);
			transmit(0, 0x0AF);
			if (epd_updated) {
				stage_lcd = 2;
				// EPD_BUSY: ~675 ms
			} else {
				epd_updated = 1;
				// EPD_BUSY: ~1687 ms
			}
			// sleep_us(200); // Waiting for EPD BUSY to be setting?
			break;
		case 2: // Update, stage 2
			transmit(0, 0x0AE);
			transmit(0, 0x028);
			transmit(0, 0x0AD);
			stage_lcd = 0;
			break;
		default:
			stage_lcd = 0;
		}
	}
	return stage_lcd;
}

void show_battery(void) {
	show_s2_number(measured_data.battery_level, 1);
	show_battery_symbol(1);
}

_attribute_ram_code_
__attribute__((optimize("-O2")))
void lcd(void) {
	if(cfg.flg2.screen_off) {
		return;
	}
	show_ble_symbol(wrk.ble_connected);
	if(lcd_flg.chow_ext_ut >= wrk.utc_time_sec) {
		show_s1_number_x100(ext.number, ext.flg.temp_symbol);
		show_battery_symbol(ext.flg.battery);
		show_smiley(ext.flg.smiley);
	} else {
		show_clock_s1();
		if (cfg.flg.comfort_smiley) // smiley = comfort
			show_smiley(is_comfort(measured_data.temp, measured_data.humi));
		else
			show_smiley(cfg.flg2.smiley);
		if(cfg.flg.show_battery || measured_data.battery_level < 16)
			show_battery_symbol(1);
	}
	if(cfg.flg.show_battery) {
		show_s2_number(measured_data.battery_level, 1);
	} else {
		show_s2_mmdd();
	}
	show_s3_number(measured_data.humi_x1, 1);
	if (cfg.flg.temp_F_or_C) {
		// convert C to F
		show_s4_number_x10(((measured_data.temp_x01 / 5) * 9) + 320, 2); // convert C to F
	} else {
		show_s4_number_x10(measured_data.temp_x01, 1);
	}
}

#endif
