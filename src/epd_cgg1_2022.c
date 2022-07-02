#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if ((DEVICE_TYPE == DEVICE_CGG1) && (DEVICE_CGG1_ver == 2022))

#include "app.h"
//#include "epd.h"
#include "lcd.h"
#include "battery.h"
#include "drivers/8258/pm.h"
#include "drivers/8258/timer.h"

#define LOW     0
#define HIGH    1

/*
  CGG1-2022 LCD buffer:  byte.bit

  @     ---------5.0-------------     O   :--4.2---
 7.6    |                       |    4.3  |
       ||    |   |   |   |   |  |        4.2
 BLE   ||   5.5 5.4 5.3 5.2 5.1 |         |
 9.6   ||    |   |   |   |   |  |         :--4.1---
        |                       |         |
        -------------------------         4.2
                                          |
                                          :--4.0---

 12.4 11.4--10.7--10.4  7.4---6.7---6.4       3.4---2.7---2.4
   |    |           |    |           |         |           |
 12.3 11.3        10.3  7.3         6.3       3.3         2.3
   |    |           |    |           |         |           |
 12.2 11.2--10.6--10.2  7.2---6.6---6.2       3.2---2.6-- 2.2
   |    |           |    |           |         |           |
 12.1 11.1        10.1  7.1         6.1       3.1         2.1
   |    |           |    |           |   4.5   |           |
 12.0 11.0--10.5--10.0  7.0---6.5---6.0   *   3.0---2.5---2.0

  ------------------------------4.4--------------------------

  14.4--13.7--13.4  9.4---8.7---8.4   %   1.4---0.7---0.4
    |           |    |           |   1.6   |           |
  14.3        13.3  9.3         8.3       1.3         0.3
    |           |    |           |         |           |
  14.2--13.6--13.2  9.2---8.6---8.2       1.2---0.6---0.2
    |           |    |           |         |           |
  14.1        13.1  9.1         8.1       1.1         0.1
    |           |    |           |   4.6   |           |
  14.0--13.5--13.0  9.0---8.5---8.0   *   1.0---0.5---0.0

   0 1 2 3 4 5 6 7 8 9 a b c d e f
60ff5fff1f7f3fff5fff5fff1f1fff1f00
*/
#define DEF_EPD_SUMBOL_SIGMENTS	13
#define DEF_EPD_REFRESH_CNT		255
//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const uint8_t top_left[DEF_EPD_SUMBOL_SIGMENTS*2] = {10, 7, 10, 4, 10, 3, 10, 2, 10, 1, 10, 0, 10, 5, 11, 0, 11, 1, 11, 2, 11, 3, 11, 4, 10, 6};
const uint8_t top_middle[DEF_EPD_SUMBOL_SIGMENTS*2] = {6, 7, 6, 4, 6, 3, 6, 2, 6, 1, 6, 0, 6, 5, 7, 0, 7, 1, 7, 2, 7, 3, 7, 4, 6, 6};
const uint8_t top_right[DEF_EPD_SUMBOL_SIGMENTS*2] =  {2, 7, 2, 4, 2, 3, 2, 2, 2, 1, 2, 0, 2, 5, 3, 0, 3, 1, 3, 2, 3, 3, 3, 4, 2, 6};
const uint8_t bottom_left[DEF_EPD_SUMBOL_SIGMENTS*2] = {13, 7, 13, 4, 13, 3, 13, 2, 13, 1, 13, 0, 13, 5, 14, 0, 14, 1, 14, 2, 14, 3, 14, 4, 13, 6};
const uint8_t bottom_middle[DEF_EPD_SUMBOL_SIGMENTS*2] = {8, 7, 8, 4, 8, 3, 8, 2, 8, 1, 8, 0, 8, 5, 9, 0, 9, 1, 9, 2, 9, 3, 9, 4, 8, 6};
const uint8_t bottom_right[DEF_EPD_SUMBOL_SIGMENTS*2] =  {0, 7, 0, 4, 0, 3, 0, 2, 0, 1, 0, 0, 0, 5, 1, 0, 1, 1, 1, 2, 1, 3, 1, 4, 0, 6};
/*
Now define how each digit maps to the segments:
          1
 12 :-----------: 2
    |           |
 11 |           | 3
    |     13    |
 10 :-----------: 4
    |           |
  9 |           | 5
    |     7     |
  8 :-----------: 6
*/
const uint8_t digits[16][DEF_EPD_SUMBOL_SIGMENTS + 1] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 0},  // 0
    {2, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0},        // 1
    {1, 2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 0, 0, 0},  // 2
    {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 13, 0, 0, 0},  // 3
    {2, 3, 4, 5, 6, 10, 11, 12, 13, 0, 0, 0, 0, 0}, // 4
    {1, 2, 4, 5, 6, 7, 8, 10, 11, 12, 13, 0, 0, 0}, // 5
    {1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0}, // 6
    {1, 2, 3, 4, 5, 6, 12, 0, 0, 0, 0, 0, 0, 0},    // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0}, // 8
    {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 0, 0}, // 9
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 0, 0}, // A
    {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0}, // b
    {1, 2, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0, 0},  // C
    {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0},  // d
    {1, 2, 4, 6, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0},  // E
    {1, 2, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0}  // F
};

RAM uint8_t display_buff[16];
RAM uint8_t display_cmp_buff[16];
RAM uint8_t stage_lcd;
//RAM uint8_t flg_lcd_init;
RAM uint8_t lcd_refresh_cnt;
RAM uint8_t epd_updated;
//----------------------------------
// T_LUT_ping, T_LUT_init, T_LUT_work values taken from the actual device with a
// logic analyzer
//----------------------------------
const uint8_t T_LUT_ping[5] = {0x07B, 0x081, 0x0E4, 0x0E7, 0x008};
const uint8_t T_LUT_init[14] = {0x082, 0x068, 0x050, 0x0E8, 0x0D0, 0x0A8, 0x065, 0x07B, 0x081, 0x0E4, 0x0E7, 0x008, 0x0AC, 0x02B };
const uint8_t T_LUT_work[9] = {0x082, 0x080, 0x000, 0x0C0, 0x080, 0x080, 0x062, 0x0AC, 0x02B};
const uint8_t T_LUT_test[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00};

#define delay_SPI_end_cycle() cpu_stall_wakeup_by_timer0((CLOCK_SYS_CLOCK_1US*55)/10) // real clk ? kHz)
#define delay_EPD_SCL_pulse() cpu_stall_wakeup_by_timer0((CLOCK_SYS_CLOCK_1US*55)/10) // real clk ? kHz)
/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_attribute_ram_code_ void show_temp_symbol(uint8_t symbol) {
	if (symbol & 0x20)
		display_buff[4] |= BIT(2) | BIT(3); // "°Г"
	else
		display_buff[4] &= ~(BIT(2) | BIT(3)); // "°Г"
	if (symbol & 0x40)
		display_buff[4] |= BIT(1); //"-"
	else
		display_buff[4] &= ~BIT(1); //"-"
	if (symbol & 0x80)
		display_buff[4] |= BIT(0); // "_"
	else
		display_buff[4] &= ~BIT(0); // "_"
}

/* CGG1 no symbol 'smiley' !
 * =5 -> "---" happy, != 5 -> "    " sad */
_attribute_ram_code_ void show_smiley(uint8_t state){
	if (state & 1)
		display_buff[4] |= BIT(4);
	else
		display_buff[4] &= ~BIT(4);
/*
		display_buff[7] |= BIT(6);
	else
		display_buff[7] &= ~BIT(6);
*/
}

_attribute_ram_code_ void show_battery_symbol(bool state){
	display_buff[5] = 0;
	if (state) {
		display_buff[5] |= BIT(0);
		if (measured_data.battery_level >= 16) {
			display_buff[5] |= BIT(1);
			if (measured_data.battery_level >= 33) {
				display_buff[5] |= BIT(2);
				if (measured_data.battery_level >= 49) {
					display_buff[5] |= BIT(3);
					if (measured_data.battery_level >= 67) {
						display_buff[5] |= BIT(4);
						if (measured_data.battery_level >= 83) {
							display_buff[5] |= BIT(5);
						}
					}
				}
			}
		}
	}
}

_attribute_ram_code_ void show_ble_symbol(bool state){
	if (state)
		display_buff[9] |= BIT(6); // "ble"
	else
		display_buff[9] &= ~BIT(6);
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) static void epd_set_digit(uint8_t *buf, uint8_t digit, const uint8_t *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_EPD_SUMBOL_SIGMENTS; i++) {
        // get the segment needed to display the digit 'digit',
        // this is stored in the array 'digits'
        int segment = digits[digit][i] - 1;
        // segment = -1 indicates that there are no more segments to display
        if (segment >= 0) {
            segment_byte = segments[2 * segment];
            segment_bit = segments[1 + 2 * segment];
            buf[segment_byte] |= (1 << segment_bit);
        }
        else
            // there are no more segments to be displayed
            break;
    }
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number){
	display_buff[2] = 0;
	display_buff[3] = 0;
	display_buff[4] &= ~(BIT(5)); // point
	display_buff[6] = 0;
	display_buff[7] &= BIT(6); // @
	display_buff[10] = 0;
	display_buff[11] = 0;
	display_buff[12] = 0;
	if (number > 19995) {
		// "Hi"
		display_buff[11] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4);
		display_buff[10] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(6);
		display_buff[7] |= BIT(0) | BIT(1) | BIT(2) | BIT(4);
	} else if (number < -995) {
		// "Lo"
		display_buff[11] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4);
		display_buff[10] |= BIT(0) | BIT(5);
		display_buff[7] |= BIT(0) | BIT(1) | BIT(2);
		display_buff[6] |= BIT(0) | BIT(1) | BIT(2) | BIT(5) | BIT(6);
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[10] |= BIT(6); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[4] |= BIT(5); // point
			if (number < 0){
				number = -number;
				display_buff[10] |= BIT(6); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[12] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4); // "1" 1000..1999
		if (number > 99) epd_set_digit(display_buff, number / 100 % 10, top_left);
		if (number > 9) epd_set_digit(display_buff, number / 10 % 10, top_middle);
		else epd_set_digit(display_buff, 0, top_middle);
		epd_set_digit(display_buff, number % 10, top_right);
	}
}

/* number in 0.1 (-99..999) -> show:  -9.9 .. 99.9 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void show_small_number_x10(int16_t number, bool percent){
	display_buff[0] = 0;
	display_buff[1] = 0;
	display_buff[4] &= ~(BIT(6));
	display_buff[8] = 0;
	display_buff[9] &= BIT(6); // BLE
	display_buff[13] = 0;
	display_buff[14] = 0;
	if (percent)
		display_buff[1] |= BIT(6); // "%"
	if (number > 9995) {
		// "Hi"
		display_buff[14] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4);
		display_buff[13] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(6);
		display_buff[9] |= BIT(0) | BIT(1) | BIT(2) | BIT(4);
	} else if (number < -995) {
		// "Lo"
		display_buff[14] |= BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4);
		display_buff[13] |= BIT(0) | BIT(5);
		display_buff[9] |= BIT(0) | BIT(1) | BIT(2);
		display_buff[8] |= BIT(0) | BIT(1) | BIT(2) | BIT(5) | BIT(6);
	} else {
		/* number: -99..999 */
		if (number > 995 || number < -95) {
			// no point, show: -99..999
			if (number < 0){
				number = -number;
				display_buff[13] |= BIT(6); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..99.9
			display_buff[4] |= BIT(6); // point
			if (number < 0){
				number = -number;
				display_buff[13] |= BIT(6); // "-"
			}
		}
		/* number: -99..999 */
		if (number > 99) epd_set_digit(display_buff, number / 100 % 10, bottom_left);
		if (number > 9) epd_set_digit(display_buff, number / 10 % 10, bottom_middle);
		else epd_set_digit(display_buff, 0, bottom_middle);
		epd_set_digit(display_buff, number % 10, bottom_right);
	}
}

void show_batt_cgg1(void) {
	uint16_t battery_level = 0;
	if (measured_data.battery_mv > MIN_VBAT_MV) {
		battery_level = ((measured_data.battery_mv - MIN_VBAT_MV)*10)/((MAX_VBAT_MV - MIN_VBAT_MV)/100);
		if (battery_level > 995)
			battery_level = 995;
	}
	show_small_number_x10(battery_level, false);
}

#if	USE_CLOCK
_attribute_ram_code_ void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;
	memset(display_buff, 0, sizeof(display_buff));
	epd_set_digit(display_buff, min / 10 % 10, bottom_left);
	epd_set_digit(display_buff, min % 10, bottom_middle);
	epd_set_digit(display_buff, hrs / 10 % 10, top_left);
	epd_set_digit(display_buff, hrs % 10, top_middle);
}
#endif // USE_CLOCK

_attribute_ram_code_ __attribute__((optimize("-Os"))) static void transmit(uint8_t cd, uint8_t data_to_send) {
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

_attribute_ram_code_ static void transmit_blk(uint8_t cd, const uint8_t * pdata, size_t size_data) {
	for (int i = 0; i < size_data; i++)
		transmit(cd, pdata[i]);
}

_attribute_ram_code_ void update_lcd(void){
	if (!stage_lcd) {
		if (memcmp(&display_cmp_buff, &display_buff, sizeof(display_buff))) {
			memcpy(&display_cmp_buff, &display_buff, sizeof(display_buff));
			if (lcd_refresh_cnt) {
				lcd_refresh_cnt--;
			} else {
				init_lcd(); // pulse RST_N low for 110 microseconds
//				lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
//			    epd_updated = 0;
			}
			stage_lcd = 1;
		}
	}
}

void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
    pm_wait_us(110);
	lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
    stage_lcd = 1;
    epd_updated = 0;
    gpio_write(EPD_RST, HIGH);
	display_buff[15] = 0;
}

_attribute_ram_code_  __attribute__((optimize("-Os"))) int task_lcd(void) {
	if (gpio_read(EPD_BUSY)) {
		switch (stage_lcd) {
		case 1: // Update/Init, stage 1
			transmit_blk(0, T_LUT_ping, sizeof(T_LUT_ping));
			stage_lcd = 2;
			break;
		case 2: // Update/Init, stage 2
			if (epd_updated == 0) {
				transmit_blk(0, T_LUT_init, sizeof(T_LUT_init));
			} else {
				transmit_blk(0, T_LUT_work, sizeof(T_LUT_work));
			}
			stage_lcd = 3;
			break;
		case 3: // Update/Init, stage 3
			transmit(0, 0x040);
			transmit(0, 0x0A9);
			transmit(0, 0x0A8);
			transmit_blk(1, display_buff, sizeof(display_buff));
			transmit(0, 0x0AB);
			transmit(0, 0x0AA);
			transmit(0, 0x0AF);
			if (epd_updated) {
				stage_lcd = 4;
				// EPD_BUSY: ~500 ms
			} else {
				epd_updated = 1;
				stage_lcd = 2;
				// EPD_BUSY: ~1000 ms
			}
			break;
		case 4: // Update, stage 4
			transmit(0, 0x0AE);
			transmit(0, 0x028);
			transmit(0, 0x0AD);
		default:
			stage_lcd = 0;
		}
	}
	return stage_lcd;
}

#endif // DEVICE_TYPE == DEVICE_MHO_C401
