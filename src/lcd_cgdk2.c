#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_CGDK2
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "i2c.h"
#include "lcd.h"
#include "battery.h"

RAM uint8_t lcd_i2c_addr;

#define lcd_send_i2c_byte(a)  send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_buf(lcd_i2c_addr, (uint8_t *) b, a)


/*
  CGDK2 LCD buffer:  byte.bit

       ---------1.5-------------      O 17.6  :--17.6---
       |                       |              |
 BLE  ||    |   |   |   |   |  |             17.6
 1.4  ||   1.6 1.7 1.3 1.2 1.1 |              |
      ||    |   |   |   |   |  |              :--17.5---
       |                       |              |
       -------------------------             17.6
                                              |
                                              :--17.4---

   |   2.7---2.3---5.7  3.3---4.7---5.6       0.7---0.6--17.3
   |    |           |    |           |         |           |
   |   2.6         3.7  3.2         4.3       0.3        17.2
   |    |           |    |           |         |           |
  1.0  2.5---2.2---3.6  3.1---4.6---4.2       0.2---0.5--17.1
   |    |           |    |           |         |           |
   |   2.4         3.5  3.0         4.1       0.1        17.0
   |    |           |    |           |   5.4   |           |
   |   2.0---2.1---3.4  4.4---4.5---4.0   *   0.0---0.4---5.5

  ------------------------------5.4--------------------------

   5.3---6.7---8.3  7.7---7.3---8.2   %   9.7---9.3--17.7
    |           |    |           |   8.1   |           |
   5.2         6.3  7.6         8.7       9.6        10.7
    |           |    |           |         |           |
   5.1---6.6---6.2  7.5---7.2---8.6       9.5---9.2--10.6
    |           |    |           |         |           |
   5.0         6.1  7.4         8.5       9.4        10.5
    |           |    |           |   8.0   |           |
   6.4---6.5---6.0  7.0---7.1---8.4   *   9.0---9.1--10.4


none: 1.4, 10.0-10.3, 11.0-16.7
*/

#define DEF_CGDK22_SUMBOL_SIGMENTS	13
/*
Now define how each digit maps to the segments:
  12-----1-----2
   |           |
  11           3
   |           |
  10----13-----4
   |           |
   9           5
   |           |
   8-----7-----6
*/

const uint8_t digits[16][DEF_CGDK22_SUMBOL_SIGMENTS + 1] = {
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

//----------------------------------
// define segments
// the data in the arrays consists of {byte, bit} pairs of each segment
//----------------------------------
const uint8_t top_left[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {2, 3, 5, 7, 3, 7, 3, 6, 3, 5, 3, 4, 2, 1, 2, 0, 2, 4, 2, 5, 2, 6, 2, 7, 2, 2};
const uint8_t top_middle[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {4, 7, 5, 6, 4, 3, 4, 2, 4, 1, 4, 0, 4, 5, 4, 4, 3, 0, 3, 1, 3, 2, 3, 3, 4, 6};
const uint8_t top_right[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {0, 6, 17, 3, 17, 2, 17, 1, 17, 0, 5, 5, 0, 4, 0, 0, 0, 1, 0, 2, 0, 3, 0, 7, 0, 5};
const uint8_t bottom_left[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {6, 7, 8, 3, 6, 3, 6, 2, 6, 1, 6, 0, 6, 5, 6, 4, 5, 0, 5, 1, 5, 2, 5, 3, 6, 6};
const uint8_t bottom_middle[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {7, 3, 8, 2, 8, 7, 8, 6, 8, 5, 8, 4, 7, 1, 7, 0, 7, 4, 7, 5, 7, 6, 7, 7, 7, 2};
const uint8_t bottom_right[DEF_CGDK22_SUMBOL_SIGMENTS*2] = {9, 3, 17, 7, 10, 7, 10, 6, 10, 5, 10, 4, 9, 1, 9, 0, 9, 4, 9, 5, 9, 6, 9, 7, 9, 2};

/* LCD controller initialize:
1. 0xea - Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
2. 0xbe - Display control (DISCTL): Power save mode3, FRAME flip 1, Normal mode // (0xb6) Power save mode2
3. 0xf0 - Blink control (BLKCTL): Off
4. 0xfc - All pixel control (APCTL): Normal
*/
//const uint8_t lcd_init_cmd[] = {0xea,0xbe,0xf0,0xfc}; // sleep all 16.6 uA
/* LCD controller initialize
1. 0xea - Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
2. 0xf0 - Blink control (BLKCTL): Off
4. 0xc0 - Mode Set (MODE SET): Display ON ?
2. 0xbc - Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
*/
const uint8_t lcd_init_cmd[] = {0xea,0xf0, 0xc0, 0xbc}; // sleep all 9.4 uA

/*
static void lcd_send_i2c_buf(uint8_t * dataBuf, uint32_t dataLen) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	uint8_t * p = dataBuf;
	reg_i2c_id = lcd_i2c_addr;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	while (dataLen--) {
		reg_i2c_do = *p++;
		reg_i2c_ctrl = FLD_I2C_CMD_DO;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
    reg_i2c_ctrl = FLD_I2C_CMD_STOP;
    while (reg_i2c_status & FLD_I2C_CMD_BUSY);
}
*/

_attribute_ram_code_
void send_to_lcd(void){
	unsigned int buff_index;
	uint8_t * p = display_buff;
	if(cfg.flg2.screen_off)
		return;
	if (lcd_i2c_addr) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		reg_i2c_id = lcd_i2c_addr;
		reg_i2c_adr_dat = 0xE800; // 0xe8 - Set IC Operarion(ICSET): Do not execute Software Reset, Internal oscillator circuit; 0x00 - ADSET
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

		reg_i2c_adr = 0xC8; // 0xc8 - Mode Set (MODE SET): Display ON, 1/3 Bias
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	}
}

void init_lcd(void){
	lcd_i2c_addr = (uint8_t) scan_i2c_addr(CGDK2_I2C_ADDR << 1);
	if (lcd_i2c_addr) { // LCD CGDK2_I2C_ADDR ?
		if(cfg.flg2.screen_off) {
			lcd_send_i2c_byte(0xEA); // BU9792AFUV reset
		} else {
			lcd_send_i2c_buf((uint8_t *) lcd_init_cmd, sizeof(lcd_init_cmd));
			pm_wait_us(200);
			send_to_lcd();
		}
	}
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
static void cgdk22_set_digit(uint8_t *buf, uint8_t digit, const uint8_t *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_CGDK22_SUMBOL_SIGMENTS; i++) {
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
		display_buff[17] |= BIT(6);
	else
		display_buff[17] &= ~(BIT(6));
	if (symbol & 0x40)
		display_buff[17] |= BIT(5); //"-"
	else
		display_buff[17] &= ~BIT(5); //"-"
	if (symbol & 0x80)
		display_buff[17] |= BIT(4); // "_"
	else
		display_buff[17] &= ~BIT(4); // "_"
}

/* CGDK22 no symbol 'smiley' !
 * =5 -> "---" happy, != 5 -> "    " sad
_attribute_ram_code_ void show_smiley(uint8_t state){
	(void) state;
//	if (state & 1)
//		display_buff[x] |= BIT(x);
//	else
//		display_buff[x] &= ~BIT(x);
}
*/
_attribute_ram_code_
void show_battery_symbol(bool state){
	display_buff[1] &= ~(BIT(1) | BIT(2) | BIT(3) | BIT(5) | BIT(6) | BIT(7));
	if (state) {
		display_buff[1] |= BIT(5);
		if (measured_data.battery_level >= 16) {
			display_buff[1] |= BIT(1);
			if (measured_data.battery_level >= 33) {
				display_buff[1] |= BIT(2);
				if (measured_data.battery_level >= 49) {
					display_buff[1] |= BIT(3);
					if (measured_data.battery_level >= 67) {
						display_buff[1] |= BIT(7);
						if (measured_data.battery_level >= 83) {
							display_buff[1] |= BIT(6);
						}
					}
				}
			}
		}
	}
}

/* CGDK22 no symbol 'ble' ! */
_attribute_ram_code_
void show_ble_symbol(bool state){
	if (state)
		display_buff[1] |= BIT(4);
	else
		display_buff[1] &= ~BIT(4);
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_big_number_x10(int16_t number){
	display_buff[0] = 0;
	display_buff[1] &= ~(BIT(0));
	display_buff[2] = 0;
	display_buff[3] = 0;
	display_buff[4] = 0;
	display_buff[5] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[17] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	if (number > 19995) {
		// "Hi"
		display_buff[2] |= BIT(0) | BIT(2) | BIT(4) | BIT(5) | BIT(6) | BIT(7);
		display_buff[3] |= BIT(0) | BIT(1) | BIT(3) | BIT(4) | BIT(5) | BIT(6);
		display_buff[4] |= BIT(4);
	} else if (number < -995) {
		// "Lo"
		display_buff[2] |= BIT(0) | BIT(1) | BIT(4) | BIT(5) | BIT(6) | BIT(7);
		display_buff[3] |= BIT(0) | BIT(1) | BIT(4);
		display_buff[4] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[2] |= BIT(2); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..199.9
			display_buff[5] |= BIT(4); // point
			if (number < 0){
				number = -number;
				display_buff[2] |= BIT(2); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) display_buff[1] |= BIT(0); // "1" 1000..1999
		if (number > 99) cgdk22_set_digit(display_buff, number / 100 % 10, top_left);
		if (number > 9) cgdk22_set_digit(display_buff, number / 10 % 10, top_middle);
		else cgdk22_set_digit(display_buff, 0, top_middle);
		cgdk22_set_digit(display_buff, number % 10, top_right);
	}
}

/* number in 0.1 (-99..999) -> show:  -9.9 .. 99.9 */
_attribute_ram_code_
__attribute__((optimize("-Os"))) void show_small_number_x10(int16_t number, bool percent){
	display_buff[5] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	display_buff[6] = 0;
	display_buff[7] = 0;
	display_buff[8] = 0;
	display_buff[9] = 0;
	display_buff[10] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[17] &= ~(BIT(7));
	if (percent)
		display_buff[8] |= BIT(1); // "%"
	if (number > 9995) {
		// "Hi"
		display_buff[5] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		display_buff[6] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(6);
		display_buff[7] |= BIT(0) | BIT(4) | BIT(5) | BIT(7);
	} else if (number < -995) {
		//"Lo"
		display_buff[5] |= BIT(0) | BIT(1) | BIT(2) | BIT(3);
		display_buff[6] |= BIT(0) | BIT(4) | BIT(5);
		display_buff[7] |= BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5);
		display_buff[8] |= BIT(4) | BIT(5) | BIT(6);
	} else {
		/* number: -99..999 */
		if (number > 995 || number < -95) {
			// no point, show: -99..999
			if (number < 0){
				number = -number;
				display_buff[6] |= BIT(6); // "-"
			}
			number = (number / 10) + ((number % 10) > 5); // round(div 10)
		} else { // show: -9.9..99.9
			display_buff[8] |= BIT(0); // point
			if (number < 0){
				number = -number;
				display_buff[6] |= BIT(6); // "-"
			}
		}
		/* number: -99..999 */
		if (number > 99) cgdk22_set_digit(display_buff, number / 100 % 10, bottom_left);
		if (number > 9) cgdk22_set_digit(display_buff, number / 10 % 10, bottom_middle);
		else cgdk22_set_digit(display_buff, 0, bottom_middle);
		cgdk22_set_digit(display_buff, number % 10, bottom_right);
	}
}

void show_batt_cgdk2(void) {
	uint16_t battery_level = 0;
	if (measured_data.average_battery_mv > MIN_VBAT_MV) {
		battery_level = ((measured_data.average_battery_mv - MIN_VBAT_MV)*10)/((MAX_VBAT_MV - MIN_VBAT_MV)/100);
		if (battery_level > 995)
			battery_level = 995;
	}
	show_small_number_x10(battery_level, false);
}

void show_ota_screen(void) {
	memset(&display_buff, 0, sizeof(display_buff));
#if 0
	// "0ta"
	display_buff[0] = BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(6);
	display_buff[1] = BIT(4); // "ble"
	display_buff[2] = ~BIT(2);
	display_buff[3] = 0xff;
	display_buff[4] = BIT(0) | BIT(4) | BIT(5) | BIT(6);
	display_buff[5] = BIT(4) | BIT(5) | BIT(7);
	display_buff[17] = BIT(0) | BIT(1) | BIT(2) | BIT(3);
#else
	display_buff[0] = BIT(5); // "-"
	display_buff[1] = BIT(4); // "ble"
	display_buff[2] = BIT(2); // "-"
	display_buff[4] = BIT(6); // "-"
	display_buff[5] = BIT(4); // "-----"
#endif

	send_to_lcd();
	lcd_send_i2c_byte(0xf2);
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
	memset(display_buff, 0, sizeof(display_buff));
	cgdk22_set_digit(display_buff, min / 10 % 10, bottom_left);
	cgdk22_set_digit(display_buff, min % 10, bottom_middle);
	cgdk22_set_digit(display_buff, hrs / 10 % 10, top_left);
	cgdk22_set_digit(display_buff, hrs % 10, top_middle);
}
#endif // USE_CLOCK

#endif // DEVICE_TYPE == DEVICE_CGDK2
