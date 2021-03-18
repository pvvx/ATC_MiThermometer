#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_CGG1
/* Based on source: https://github.com/znanev/ATC_MiThermometer */
#include "app.h"
#include "epd.h"
#include "battery.h"
#include "drivers/8258/pm.h"
#include "drivers/8258/timer.h"

#define DEF_EPD_SUMBOL_SIGMENTS	13
#define DEF_EPD_REFRESH_CNT		32

RAM uint8_t display_buff[18];
RAM uint8_t display_cmp_buff[18];
RAM uint8_t stage_lcd;
RAM uint8_t flg_lcd_init;
RAM uint8_t lcd_refresh_cnt;
RAM uint8_t epd_updated;
//----------------------------------
// LUTV, LUT_KK and LUT_KW values taken from the actual device with a
// logic analyzer
//----------------------------------
const uint8_t T_LUTV_init[15] = {0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KK_init[15] = {0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KW_init[15] = {0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KK_update[15] = {0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x87, 0x87, 0x01,  0x81, 0x81, 0x01};
const uint8_t T_LUT_KW_update[15] = {0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x47, 0x47, 0x01,  0x81, 0x81, 0x01};

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const uint8_t top_left[DEF_EPD_SUMBOL_SIGMENTS*2] = {9, 4, 8, 5, 8, 6, 8, 7, 7, 0, 7, 1, 9, 2, 14, 4, 14, 5, 14, 6, 14, 7, 13, 0, 9, 3};
const uint8_t top_middle[DEF_EPD_SUMBOL_SIGMENTS*2] = {6, 3, 5, 7, 4, 0, 4, 1, 4, 6, 4, 7, 5, 4, 5, 0, 6, 6, 6, 5, 6, 4, 6, 2, 4, 5};
const uint8_t top_right[DEF_EPD_SUMBOL_SIGMENTS*2] = {1, 6, 1, 7, 1, 0, 2, 6, 2, 3, 2, 1, 2, 0, 3, 7, 2, 2, 2, 4, 2, 7, 1, 1, 2, 5};
const uint8_t bottom_left[DEF_EPD_SUMBOL_SIGMENTS*2] = {13, 1, 13, 2, 14, 2, 14, 0, 15, 0, 16, 6, 16, 5, 16, 4, 16, 7, 15, 6, 14, 1, 14, 3, 15, 7};
const uint8_t bottom_middle[DEF_EPD_SUMBOL_SIGMENTS*2] = {9, 6, 8, 3, 8, 2, 8, 1, 8, 4, 7, 3, 10, 5, 10, 4, 10, 6, 9, 5, 9, 0, 10, 7, 9, 7};
const uint8_t bottom_right[DEF_EPD_SUMBOL_SIGMENTS*2] = {5, 1, 5, 2, 7, 7, 3, 2, 3, 4, 0, 1, 0, 0, 3, 5, 3, 3, 3, 0, 7, 6, 6, 0, 3, 1};

#define delay_SPI_end_cycle() cpu_stall_wakeup_by_timer0((CLOCK_SYS_CLOCK_1US*15)/10) // real clk 4.4 + 4.4 us : 114 kHz)
#define delay_EPD_SCL_pulse() cpu_stall_wakeup_by_timer0((CLOCK_SYS_CLOCK_1US*15)/10) // real clk 4.4 + 4.4 us : 114 kHz)

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

/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" */
_attribute_ram_code_ void show_temp_symbol(uint8_t symbol) {
	if(symbol & 0x20)
		display_buff[1] |= BIT(3) | BIT(2);
	else
		display_buff[1] &= ~(BIT(3) | BIT(2));
	if(symbol & 0x40)
		display_buff[1] |= BIT(4); //"-"
	else
		display_buff[1] &= ~BIT(4); //"-"
	if(symbol & 0x80)
		display_buff[1] |= BIT(5); // "_"
	else
		display_buff[1] &= ~BIT(5); // "_"
}
/* CGG1 no symbol 'smiley'
 * 0,1,2,3,4,5 = "   " happy
 * 6,7 = "----" sad */
_attribute_ram_code_ void show_smiley(uint8_t state){
	if(state > 5)
		display_buff[7] |= BIT(2);
	else
		display_buff[7] &= ~BIT(2);
}

_attribute_ram_code_ void show_battery_symbol(bool state){
	display_buff[5] &= ~(BIT(5) | BIT(6));
	display_buff[6] &= ~(BIT(1) | BIT(7));
	display_buff[7] &= ~(BIT(4) | BIT(5));
	if(state) {
		display_buff[7] |= BIT(4);
		if(battery_level >= 16) {
			display_buff[5] |= BIT(6);
			if(battery_level >= 33) {
				display_buff[5] |= BIT(5);
				if(battery_level >= 49) {
					display_buff[6] |= BIT(1);
					if(battery_level >= 67) {
						display_buff[6] |= BIT(7);
						if(battery_level >= 83) {
							display_buff[7] |= BIT(5);
						}
					}
				}
			}
		}
	}
}

_attribute_ram_code_ void show_ble_symbol(bool state){
	if(state)
		display_buff[9] |= BIT(1); // "*"
	else
		display_buff[9] &= ~BIT(1);
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) static void transmit(uint8_t cd, uint8_t data_to_send) {
    gpio_write(EPD_SCL, LOW);
    // enable SPI
    gpio_write(EPD_CSB, LOW);
    delay_EPD_SCL_pulse();

    // send the first bit, this indicates if the following is a command or data
    if (cd != 0)
        gpio_write(EPD_SDA, HIGH);
    else
        gpio_write(EPD_SDA, LOW);
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

/* x0.1 (-995..19995) Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void show_big_number(int16_t number){
	display_buff[1] &= ~(BIT(0) | BIT(1) | BIT(6) | BIT(7));
	display_buff[2] = 0;
	display_buff[3] &= ~(BIT(6) | BIT(7));
	display_buff[4] = 0;
	display_buff[5] &= ~(BIT(0) | BIT(4) | BIT(7));
	display_buff[6] &= ~(BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6));
	display_buff[7] &= ~(BIT(0) | BIT(1));
	display_buff[8] &= ~(BIT(5) | BIT(6) | BIT(7));
	display_buff[9] &= ~(BIT(2) | BIT(3) | BIT(4));
	display_buff[13] &= ~BIT(0);
	display_buff[14] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[15] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5));
	if(number > 19995) {
		// "Hi"
		display_buff[13] |= BIT(0);
		display_buff[14] |= BIT(4) | BIT(5) | BIT(6) | BIT(7);
		display_buff[9] |= BIT(3);
		display_buff[8] |= BIT(5) | BIT(6) | BIT(7);
		display_buff[7] |= BIT(0) | BIT(1);
		display_buff[6] |= BIT(5) | BIT(6);
		display_buff[5] |= BIT(0);
	} else if(number < -995) {
		// "Lo"
		display_buff[13] |= BIT(0);
		display_buff[14] |= BIT(4) | BIT(5) | BIT(6) | BIT(7);
		display_buff[9] |= BIT(2);
		display_buff[7] |= BIT(1);
		display_buff[6] |= BIT(5) | BIT(6);
		display_buff[5] |= BIT(0) | BIT(4);
		display_buff[4] |= BIT(1) | BIT(5) | BIT(6) | BIT(7);
	} else {
		/* number: -995..19995 */
		if(number > 1995 || number < -95) {
			// no point, show: -99..1999
			if(number < 0){
				number = -number;
				display_buff[9] = BIT(3); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[3] |= BIT(6); // point
			if(number < 0){
				number = -number;
				display_buff[9] = BIT(3); // "-"
			}
		}
		/* number: -99..1999 */
		if(number > 999) display_buff[15] |= BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5); // "1" 1000..1999
		if(number > 99) epd_set_digit(display_buff, number / 100 % 10, top_left);
		if(number > 9) epd_set_digit(display_buff, number / 10 % 10, top_middle);
		else epd_set_digit(display_buff, 0, top_middle);
		epd_set_digit(display_buff, number % 10, top_right);
	}
}

/* -9.9 .. 99.9 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void show_small_number(int16_t number, bool percent){
	display_buff[0] &= ~(BIT(0) | BIT(1) | BIT(2));
	display_buff[3] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5));
	display_buff[5] &= ~BIT(1);
	display_buff[6] &= ~BIT(0);
	display_buff[7] &= ~(BIT(3) | BIT(6) | BIT(7));
	display_buff[8] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(4));
	display_buff[9] &= ~(BIT(0) | BIT(5) | BIT(6) | BIT(7));
	display_buff[10] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[13] &= ~(BIT(1) | BIT(2));
	display_buff[14] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	display_buff[15] &= ~(BIT(0) | BIT(6) | BIT(7));
	display_buff[16] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	if(percent)
		display_buff[5] |= BIT(3); // "%"
	if(number > 999) {
		// "Hi"
		display_buff[9] |= BIT(5);
		display_buff[10] |= BIT(4) | BIT(6);
		display_buff[13] |= BIT(2);
		display_buff[14] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		display_buff[15] |= BIT(0) |BIT(6) | BIT(7);
		display_buff[16] |= BIT(4) | BIT(6) | BIT(7);
	} else if(number < -99) {
		//"Lo"
		display_buff[7] |= BIT(3);
		display_buff[8] |= BIT(1) | BIT(4);
		display_buff[9] |= BIT(5) | BIT(7);
		display_buff[10] |= BIT(4) | BIT(5) | BIT(6);
		display_buff[14] |= BIT(1) | BIT(3);
		display_buff[15] |= BIT(6);
		display_buff[16] |= BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -99..999 */
		if(number > 995 || number < -95) {
			// no point, show: -99..999
			if(number < 0){
				number = -number;
				display_buff[0] |= BIT(2); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..99.9
			display_buff[0] |= BIT(2); // point
			if(number < 0){
				number = -number;
				display_buff[15] |= BIT(7); // "-"
			}
		}
		/* number: -99..999 */
		if(number > 99) epd_set_digit(display_buff, number / 100 % 10, bottom_left);
		if(number > 9) epd_set_digit(display_buff, number / 10 % 10, bottom_middle);
		else epd_set_digit(display_buff, 0, bottom_middle);
		epd_set_digit(display_buff, number % 10, bottom_right);
	}
}

void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
    pm_wait_us(110);
	lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
    stage_lcd = 1;
    epd_updated = 0;
    flg_lcd_init = 1;
    gpio_write(EPD_RST, HIGH);
    // EPD_BUSY: Low 866 us
}

void show_batt_cgg1(void) {
	uint16_t battery_level = 0;
	if(measured_data.battery_mv > MIN_VBAT_MV) {
		battery_level = ((measured_data.battery_mv - MIN_VBAT_MV)*10)/((MAX_VBAT_MV - MIN_VBAT_MV)/100);
		if(battery_level > 995)
			battery_level = 995;
	}
	show_small_number(battery_level, 1);
}

_attribute_ram_code_ void update_lcd(void){
	if(memcmp(&display_cmp_buff, &display_buff, sizeof(display_buff))) {
		memcpy(&display_cmp_buff, &display_buff, sizeof(display_buff));
		if(lcd_refresh_cnt) {
			lcd_refresh_cnt--;
			flg_lcd_init = 0;
			stage_lcd = 1;
		} else {
			init_lcd(); // pulse RST_N low for 110 microseconds
		}
	}
}

_attribute_ram_code_  __attribute__((optimize("-Os"))) int task_lcd(void) {
	if (gpio_read(EPD_BUSY)) {
		switch (stage_lcd) {
		case 1: // Update/Init lcd, stage 1
			// send Charge Pump ON command
			transmit(0, POWER_ON);
			// EPD_BUSY: Low 32 ms from reset, 47.5 ms in refresh cycle
			stage_lcd = 2;
			break;
		case 2: // Update/Init lcd, stage 2
			if (epd_updated == 0) {
				transmit(0, PANEL_SETTING);
				transmit(1, 0x0F);

				transmit(0, POWER_SETTING);
				transmit(1, 0x32); // transmit(1, 0x32);
				transmit(1, 0x32); // transmit(1, 0x32);
				transmit(0, POWER_OFF_SEQUENCE_SETTING);
				transmit(1, 0x00);
				// Frame Rate Control
				transmit(0, PLL_CONTROL);
				if (flg_lcd_init)
					transmit(1, 0x03);
				else {
					transmit(1, 0x07);
					transmit(0, PARTIAL_DISPLAY_REFRESH);
					transmit(1, 0x00);
					transmit(1, 0x87);
					transmit(1, 0x01);
					transmit(0, POWER_OFF_SEQUENCE_SETTING);
					transmit(1, 0x06);
				}
				// send the e-paper voltage settings (waves)
				transmit(0, LUT_FOR_VCOM);
				for (int i = 0; i < sizeof(T_LUTV_init); i++)
					transmit(1, T_LUTV_init[i]);

				if (flg_lcd_init) {
					flg_lcd_init = 0;
					transmit(0, LUT_CMD_0x23);
					for (int i = 0; i < sizeof(T_LUT_KK_init); i++)
						transmit(1, T_LUT_KK_init[i]);
					transmit(0, LUT_CMD_0x26);
					for (int i = 0; i < sizeof(T_LUT_KW_init); i++)
						transmit(1, T_LUT_KW_init[i]);
					// start an initialization sequence (white - all 0x00)
					stage_lcd = 2;
				} else {
					transmit(0, LUT_CMD_0x23);
					for (int i = 0; i < sizeof(T_LUTV_init); i++)
						transmit(1, T_LUTV_init[i]);

					transmit(0, LUT_CMD_0x24);
					for (int i = 0; i < sizeof(T_LUT_KK_update); i++)
						transmit(1, T_LUT_KK_update[i]);

					transmit(0, LUT_CMD_0x25);
					for (int i = 0; i < sizeof(T_LUT_KW_update); i++)
						transmit(1, T_LUT_KW_update[i]);

					transmit(0, LUT_CMD_0x26);
					for (int i = 0; i < sizeof(T_LUTV_init); i++)
						transmit(1, T_LUTV_init[i]);
					// send the actual data
					stage_lcd = 3;
				}
			} else {
				stage_lcd = 3;
			}
			// send the actual data
			transmit(0, DATA_START_TRANSMISSION_1);
			for (int i = 0; i < sizeof(display_buff); i++)
				transmit(1, display_buff[i]^0xFF);
			// Refresh
			transmit(0, DISPLAY_REFRESH);
			// EPD_BUSY: Low 1217 ms from reset, 608.5 ms in refresh cycle
			break;
		case 3: // Update lcd, stage 3
			// send Charge Pump OFF command
			transmit(0, POWER_OFF);
			transmit(1, 0x03);
			// EPD_BUSY: Low 9.82 ms in refresh cycle
			epd_updated = 1;
			stage_lcd = 0;
			break;
		default:
			stage_lcd = 0;
		}
	}
	return stage_lcd;
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

#endif // DEVICE_TYPE == DEVICE_MHO_C401
