/*
 * epd_mho_c401n.c
 * Author: kloemi & pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_MHO_C401N

#include "app.h"
#include "ble.h"
#include "lcd.h"
#include "epd.h"
#include "drivers/8258/pm.h"
#include "drivers/8258/timer.h"
#include "stack/ble/ll/ll_pm.h"

/* MHO-C401-2022 LCD buffer:  byte.bit

  7.4  8.4--8.7-- .  10.4--10.7--- .       12.4--12.7--- .     BAT 14.6
   |    |         |    |           |         |           |
  7.4  8.3       9.2 10.3        11.2      12.3        13.2   o|--- 14.4
   |    |         |    |           |         |           |     |  --14.2
  7.4  8.2--8.6--9.1 10.2--10.6--11.1      12.2--12.6--13.1    |  --14.0
   |    |         |    |           |         |           |
  7.4  8.1       9.0 10.1        11.0      12.1        13.0
   |    |         |    |           |  11.4   |           |
  7.4  8.0--8.5-- .  10.0--10.5--- .    *  12.0--12.5--- .


       1.4--1.7-- .   3.4--3.7-- .
        |         |    |         |     *7.2 П6.6      П6.6
 BLE   1.3       2.2  3.3       4.2         U6.5      U6.5
 0.4    |         |    |         |               П6.0
       1.2--1.6--2.1  3.2--3.6--4.1    (7.0 `5.2 -5.6  )7.0 ;6.2
  @     |         |    |         |               U5.4
 0.0   1.1       2.0  3.1       4.0
        |         |    |         |               %5.0
       1.0--1.5-- .   3.0--3.5-- .

Background: 15=0xff ?
None: 0.1..0.3, 0.5..0.7, 2.3..2.7, 4.3..4.7, 5.1, 5,3, 5.5, 5.7, 6.1, 6.3, 6.7, 7.1, 7.3, 7.5..7.7, 9.3..9.7, 11.3, 11.5..11.7, 13.3..13.7, 14.1, 14.3, 14.5
*/


RAM uint8_t stage_lcd;
RAM uint8_t epd_updated;
//#define DEF_EPD_REFRESH_CNT	2048
//RAM uint16_t lcd_refresh_cnt;

const uint8_t T_LUT_ping[5] = {0x07B, 0x081, 0x0E4, 0x0E7, 0x008};
const uint8_t T_LUT_init[14] = {0x082, 0x068, 0x050, 0x0E8, 0x0D0, 0x0A8, 0x065, 0x07B, 0x081, 0x0E4, 0x0E7, 0x008, 0x0AC, 0x02B };
const uint8_t T_LUT_work[9] = {0x082, 0x080, 0x000, 0x0C0, 0x080, 0x080, 0x062, 0x0AC, 0x02B};
//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const uint8_t top_left[22] =   { 8, 7,  9, 2,  9, 1,  9, 0,  8, 5,  8, 0,  8, 1,  8, 2,  8, 3,  8, 4,  8, 6};
const uint8_t top_middle[22] = {10, 7, 11, 2, 11, 1, 11, 0, 10, 5, 10, 0, 10, 1, 10, 2, 10, 3, 10, 4, 10, 6};
const uint8_t top_right[22] =  {12, 7, 13, 2, 13, 1, 13, 0, 12, 5, 12, 0, 12, 1, 12, 2, 12, 3, 12, 4, 12, 6};
const uint8_t bottom_left[22] = {1, 7,  2, 2,  2, 1,  2, 0,  1, 5,  1, 0,  1, 1,  1, 2,  1, 3,  1, 4,  1, 6};
const uint8_t bottom_right[22]= {3, 7,  4, 2,  4, 1,  4, 0,  3, 5,  3, 0,  3, 1,  3, 2,  3, 3,  3, 4,  3, 6};

// These values closely reproduce times captured with logic analyser
#define delay_SPI_end_cycle() sleep_us(2)
#define delay_EPD_SCL_pulse() sleep_us(2)

/*
Now define how each digit maps to the segments:
          1
 10 :-----------
    |           |
  9 |           | 2
    |     11    |
  8 :-----------: 3
    |           |
  7 |           | 4
    |     5     |
  6 :----------- 
*/

const uint8_t digits[16][11] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0},  // 0
    {2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0},   // 1
    {1, 2, 3, 5, 6, 7, 8, 10, 11, 0, 0}, // 2
    {1, 2, 3, 4, 5, 6, 10, 11, 0, 0, 0}, // 3
    {2, 3, 4, 8, 9, 10, 11, 0, 0, 0, 0}, // 4
    {1, 3, 4, 5, 6, 8, 9, 10, 11, 0, 0}, // 5
    {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0}, // 6
    {1, 2, 3, 4, 10, 0, 0, 0, 0, 0, 0},  // 7
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // 8
    {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 0}, // 9
    {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 0}, // A
    {3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0}, // b
    {5, 6, 7, 8, 11, 0, 0, 0, 0, 0, 0},  // c
    {2, 3, 4, 5, 6, 7, 8, 11, 0, 0, 0},  // d
    {1, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0}, // E
    {1, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}  // F
};

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
		display_buff[14] |= BIT(4); // "Г", "%", "( )", "."
	else
		display_buff[14] &= ~BIT(4); // "Г", "%", "( )", "."
	if (symbol & 0x40)
		display_buff[14] |= BIT(2); //"-"
	else
		display_buff[14] &= ~BIT(2); //"-"
	if (symbol & 0x80)
		display_buff[14] |= BIT(0); // "_"
	else
		display_buff[14] &= ~BIT(0); // "_"
}
/* 0 = "   " off,
 * 1 = " o "
 * 2 = "o^o"
 * 3 = "o-o"
 * 4 = "oVo"
 * 5 = "vVv" happy
 * 6 = "^-^" sad
 * 7 = "oOo" */
_attribute_ram_code_
void show_smiley(uint8_t state){
 	// off
	display_buff[5] &= ~(BIT(2) | BIT(4) | BIT(6)); // do not reset %
	display_buff[6] = 0;
	display_buff[7] &= ~(BIT(4));
	if (state) {
		display_buff[7] |= BIT(0); /* (  ) */
	} 
	switch(state & 7) {
		case 1:
			display_buff[5] |= BIT(4);
			display_buff[6] |= BIT(0);
			break;
		case 2:
			display_buff[5] |= BIT(6);
			display_buff[6] |= BIT(4) | BIT(6);
			break;
		case 3:
			display_buff[6] |= BIT(0) | BIT(4) | BIT(6);
		    display_buff[7] |= BIT(2);			
			break;
		case 4:
			display_buff[5] |= BIT(2);
			display_buff[6] |= BIT(0) | BIT(2) | BIT(4) | BIT(6);
		    display_buff[7] |= BIT(2);			
			break;
		case 5:
			display_buff[5] |= BIT(4) | BIT(6);
			display_buff[6] |= BIT(4) | BIT(6);
			break;
		case 6:
			display_buff[6] |= BIT(0) | BIT(4) | BIT(6);
			break;
		case 7:
			display_buff[5] |= BIT(4);
			display_buff[6] |= BIT(0) | BIT(4) | BIT(6);
			break;
	}
}

_attribute_ram_code_
void show_battery_symbol(bool state){
 	if (state)
		display_buff[14] |= BIT(6);
	else
		display_buff[14] &= ~BIT(6);
}

_attribute_ram_code_
void show_ble_symbol(bool state){
 	if (state)
		display_buff[0] |= BIT(4); // "*"
	else
		display_buff[0] &= ~BIT(4);
}

void show_connected_symbol(bool state){
 	if (state)
		display_buff[0] |= BIT(0);
	else
		display_buff[0] &= ~BIT(0);
 	SET_LCD_UPDATE();
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
static void epd_set_digit(uint8_t *buf, uint8_t digit, const uint8_t *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < 11; i++) {
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
_attribute_ram_code_
__attribute__((optimize("-Os")))
void show_big_number_x10(int16_t number){
	display_buff[8] = 0;
	display_buff[9] = 0;
	display_buff[10] = 0;
	display_buff[11] = 0; //"_"
	display_buff[12] = 0;
	display_buff[13] = 0;
	if (number > 19995) {
		// "Hi"
   		display_buff[8] = BIT(4);
   		display_buff[9] = BIT(0) | BIT(3);
   		display_buff[10] = BIT(0) | BIT(1) | BIT(6) | BIT(7);
   		display_buff[11] = BIT(4) | BIT(5) | BIT(6) | BIT(7);
	} else if (number < -995) {
		// "Lo"
   		display_buff[8] = BIT(4) | BIT(5);
   		display_buff[9] = BIT(0) | BIT(3) | BIT(4) | BIT(5);
   		display_buff[10] = BIT(3) | BIT(4) | BIT(5);
   		display_buff[11] = BIT(5) | BIT(6) | BIT(7);
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			display_buff[11] &= ~BIT(4); // no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[8] = BIT(6); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[11] |= BIT(4); // point
			if (number < 0){
				number = -number;
				display_buff[8] = BIT(6); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[7] |= BIT(4); // "1" 1000..1999
		if (number > 99) epd_set_digit(display_buff, number / 100 % 10, top_left);
		if (number > 9) epd_set_digit(display_buff, number / 10 % 10, top_middle);
		else epd_set_digit(display_buff, 0, top_middle);
		epd_set_digit(display_buff, number % 10, top_right);
	}
}

/* -9 .. 99 */
_attribute_ram_code_
__attribute__((optimize("-Os")))
void show_small_number(int16_t number, bool percent){
	display_buff[1] = 0;
	display_buff[2] = 0;
	display_buff[3] = 0;
	display_buff[4] = 0;
	display_buff[5] &= ~BIT(0);
	if (percent)
		display_buff[5] |= BIT(0); // "%" TODO 'C', '.', '(  )' ?
	if (number > 99) {
		// "Hi"
		display_buff[1] |= BIT(1) | BIT(4);
		display_buff[2] |= BIT(0) | BIT(3);
		display_buff[3] |= BIT(2) | BIT(5);
		display_buff[4] |= BIT(4) | BIT(7);
	} else if (number < -9) {
		//"Lo"
		display_buff[1] |= BIT(2) | BIT(3);
		display_buff[2] |= BIT(4) | BIT(7);
		display_buff[3] |= BIT(6) | BIT(3);
		display_buff[4] |= BIT(2);
	} else {
		if (number < 0) {
			number = -number;
			display_buff[1] |= BIT(6); // "-"
		}
		if (number > 9) epd_set_digit(display_buff, number / 10 % 10, bottom_left);
		epd_set_digit(display_buff, number % 10, bottom_right);
	}
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
static void transmit(uint8_t cd, uint8_t data_to_send) {
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
static void transmit_blk(uint8_t cd, const uint8_t * pdata, size_t size_data) {
	for (int i = 0; i < size_data; i++)
		transmit(cd, pdata[i]);
}

void init_lcd(void) {
	// pulse RST_N low for 110 microseconds
    gpio_write(EPD_RST, LOW);
	display_buff[15] = 0;
//	lcd_refresh_cnt = DEF_EPD_REFRESH_CNT;
    stage_lcd = 1;
    epd_updated = 0;
    sleep_us(110);
    gpio_write(EPD_RST, HIGH);
    bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
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
#if 0
			if (lcd_refresh_cnt)
				lcd_refresh_cnt--;
			else if(wrk.ble_connected == 0)
				init_lcd(); // pulse RST_N low for 110 microseconds
#endif
			lcd_flg.b.send_notify = lcd_flg.b.notify_on; // set flag LCD for send notify
			stage_lcd = 1;
		}
	}
}

_attribute_ram_code_
int task_lcd(void) {
	if(cfg.flg2.screen_off) {
		stage_lcd = 0;
		return stage_lcd;
	}
	if (gpio_read(EPD_BUSY)) {
		switch (stage_lcd) {
		case 1: // Update/Init, stage 1
			transmit_blk(0, T_LUT_ping, sizeof(T_LUT_ping));
			stage_lcd = 2;
			break;
		case 2: // Update/Init, stage 2
			if (epd_updated) {
				transmit_blk(0, T_LUT_work, sizeof(T_LUT_work));
			} else {
				transmit_blk(0, T_LUT_init, sizeof(T_LUT_init));
			}
			stage_lcd = 3;
			break;
		case 3: // Update/Init, stage 3
			transmit(0, 0x040);
			transmit(0, 0x0A9);
			transmit(0, 0x0A8);
			transmit_blk(1, display_cmp_buff, sizeof(display_cmp_buff));
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
			// sleep_us(200); // Waiting for EPD BUSY to be setting?
			break;
		case 4: // Update, stage 4
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

#if	USE_DISPLAY_CLOCK
_attribute_ram_code_
void show_clock(void) {
	uint32_t tmp = utc_time_sec / 60;
	uint32_t min = tmp % 60;
	uint32_t hrs = tmp / 60 % 24;
	memset(display_buff, 0, sizeof(display_buff));
	epd_set_digit(display_buff, min / 10 % 10, bottom_left);
	epd_set_digit(display_buff, min % 10, bottom_right);
	epd_set_digit(display_buff, hrs / 10 % 10, top_left);
	epd_set_digit(display_buff, hrs % 10, top_middle);
}
#endif // USE_DISPLAY_CLOCK

#endif // DEVICE_TYPE == DEVICE_MHO_C401N
