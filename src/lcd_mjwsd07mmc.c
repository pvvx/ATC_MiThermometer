#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_MJWSD07MMC
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

//atr: 0 - Off, 1 - °C, 2 - °F, 3 - °E, 4 - =, 5 - ppm, 6 - %
typedef enum {
	SYMBOL_NONE = 0,
	SYMBOL_C,
	SYMBOL_F,
	SYMBOL_E,
	SYMBOL_EQ,
	SYMBOL_PPM,
	SYMBOL_P
}symbols_e;


/* MJWSD07MMC LCD buffer:  byte.bit

(s1)Верхняя строка:

      -17.2-         -18.4-               -19.4-         -20.4-
             |     |        |           |        |     |        |                     21.1
           17.1  18.5     18.1      / 19.5     19.1  20.5     20.1   19.0*))17.0        ^
             |     |        |      /    |        |     |        |                       |
      -17.2-         -18.2-     18.0      -19.2-         -20.2-      BLE   Bat  21.5<-21.6--> 21.2
    |        |     |        |    /      |        |     |        |   20.0   21.4
  17.2     17.3  18.6     18.3  /     19.6     19.3  20.6     20.3                     CO2
    |        |     |        |           |        |     |        |                     21.7
      -17.2-         -18.7-               -19.7-         -20.7-

(s2) Центральные, большие:

     --0.4--        --1.4--               --2.4--          --3.4--                       o 5.5
    |        |     |        |            |        |       |        |         _  _          +--- 5.5
   0.5      0.0   1.5      1.1   1.0    2.5      2.0     3.5 %5.0 5.1   5.7 /    \4.0  5.5 |
    |        |     |        |     o      |        |       |        |       | 4.1  |        ---- 5.4
     --0.1--        --1.2--               --2.1--          --5.2--                     5.5 |
    |        |     |        |    1.0     |        |       |        |        \____/         ---- 5.6
   0.6      0.2   1.6      1.3    o     2.6      2.2     3.6      5.3        4.2
    |        |     |        |            |        |   *   |        |
     --0.7--        --1.7--               --2.7--    2.3   --3.7--         ppm 4.3

 Воскресенье  Понедельник  Вторник    Среда    Четверг   Пятница  Суббота
   Sunday       Monday     Tuesday  Wednesday  Thursday  Friday   Saturday
    3.0          7.0        10.4      13.4      13.0      16.0     0.3

(s3,s4,s5) Нижняя строка:
(s3)
     --4.4--        --6.4--             --7.4--        --8.4--          _  _
    |        |     |        |          |        |     |        |   9.5 /    \9.4
   4.5      3.1   6.5      6.1  6.0   7.5      7.1   8.5      8.0     | 9.6  |
    |        |     |        |    o     |        |     |        |
     --3.2--        --6.2--             --7.2--        --8.1--         \____/
    |        |     |        |   6.0    |        |     |        |        9.7
   4.6      3.3   6.6      6.3   o    7.6      7.3   8.6      8.2  ppm
    |        |     |        |          |        |  *  |        |   8.3
     --4.7--        --6.7--             --7.7--  21.3  --8.7--

(s4)
     --9.0--        -10.0--             -11.0--        -12.0--
    |        |     |        |          |        |     |        |
   9.1     10.5  10.1     11.5 11.4  11.1     12.4  12.1     13.5     o 13.2
    |        |     |        |    o     |        |     |        |        +--- 13.2
     -10.6--        -11.6--             -12.5--        -13.6--     13.2 |
    |        |     |        |  11.4    |        |     |        |        ---- 13.1
   9.2     10.7  10.2     11.7   o   11.2     12.6  12.2     13.7  13.2 |
    |        |     |        |          |        |  *  |        |        ---- 13.3
     --9.3--        -10.3--             -11.3--  12.7  -12.3--

(s5)
          -14.4--        -15.4--           -16.4--        o 17.5
   |     |        |     |        |        |        |        +--- 17.5
   |   14.5     14.1  15.5     15.0     16.5     16.1  17.5 |
   |     |        |     |        |        |        |        ---- 17.4
  14.0    -14.2--        -15.1--           -16.2--     17.5 |
   |     |        |     |        |        |        |        ---- 17.6
   |   14.6     14.3  15.6     15.2     16.6     16.3
   |     |        |     |        |   *    |        |        o/ 17.7
          -14.7--        -15.7--    15.3   -16.7--          /o
*/

#define DEF_MJWSD07MMC_SUMBOL_SIGMENTS	7
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

const u8 digits[16][DEF_MJWSD07MMC_SUMBOL_SIGMENTS + 1] = {
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
const u8 sb_s1[4][DEF_MJWSD07MMC_SUMBOL_SIGMENTS*2] = {
		{17,0x04, 17,0x02, 17,0x08, 17,0x04, 17,0x04, 17,0x04, 17, 0},
		{18,0x10, 18,0x02, 18,0x08, 18,0x80, 18,0x40, 18,0x20, 18,0x04},
		// "/" 18,0x01
		{19,0x10, 19,0x02, 19,0x08, 19,0x80, 19,0x40, 19,0x20, 19,0x04},
		{20,0x10, 20,0x02, 20,0x08, 20,0x80, 20,0x40, 20,0x20, 20,0x04}
};

const u8 sb_s2[4][DEF_MJWSD07MMC_SUMBOL_SIGMENTS*2] = {
		{0,0x10, 0,0x01, 0,0x04, 0,0x80, 0,0x40, 0,0x20, 0,0x02},
		{1,0x10, 1,0x02, 1,0x08, 1,0x80, 1,0x40, 1,0x20, 1,0x04},
		// ":" 1,0x01
		{2,0x10, 2,0x01, 2,0x04, 2,0x80, 2,0x40, 2,0x20, 2,0x02},
		// "." 1,0x08
		// "%" 5,0x01
		{3,0x10, 5,0x02, 5,0x08, 3,0x80, 3,0x40, 3,0x20, 5,0x04}
};

const u8 sb_s3[4][DEF_MJWSD07MMC_SUMBOL_SIGMENTS*2] = {
		{4,0x10, 3,0x02, 3,0x08, 4,0x80, 4,0x40, 4,0x20, 3,0x04},
		{6,0x10, 6,0x02, 6,0x08, 6,0x80, 6,0x40, 6,0x20, 6,0x04},
		// ":" 6,0x01
		{7,0x10, 7,0x02, 7,0x08, 7,0x80, 7,0x40, 7,0x20, 7,0x04},
		// "." 21,0x08
		{8,0x10, 8,0x01, 8,0x04, 8,0x80, 8,0x40, 8,0x20, 8,0x02}
};

const u8 sb_s4[4][DEF_MJWSD07MMC_SUMBOL_SIGMENTS*2] = {
		{9,0x01, 10,0x20, 10,0x80, 9,0x08, 9,0x04, 9,0x02, 10,0x40},
		{10,0x01, 11,0x20, 11,0x80, 10,0x08, 10,0x04, 10,0x02, 11,0x40},
		// ":" 11,0x10
		{11,0x01, 12,0x10, 12,0x40, 11,0x08, 11,0x04, 11,0x02, 12,0x20},
		// "." 12,0x80
		{12,0x01, 13,0x20, 13,0x80, 12,0x08, 12,0x04, 12,0x02, 13,0x40},
};

const u8 sb_s5[3][DEF_MJWSD07MMC_SUMBOL_SIGMENTS*2] = {
		// "1" 14,0x01
		{14,0x10, 14,0x02, 14,0x08, 14,0x80, 14,0x40, 14,0x20, 14,0x04},
		{15,0x10, 15,0x01, 15,0x04, 15,0x80, 15,0x40, 15,0x20, 15,0x02},
		// "." 15,0x01
		{16,0x10, 16,0x02, 16,0x08, 16,0x80, 16,0x40, 16,0x20, 16,0x04}
};

// a8/88
static void clear_s1(void) {
	display_buff[17] &= ~(BIT(2) | BIT(3) | BIT(5)); // "d"
	display_buff[18] = 0; // "8/"
	display_buff[19] &= BIT(0); //"*" ))
	display_buff[20] &= BIT(0); //"BLE"
}

// 88:8.8ppm°E
static void clear_s2(void) {
	display_buff[0] &= BIT(3); //"Saturday"
	display_buff[1] = 0;
	display_buff[2] = 0;
	display_buff[3] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[4] &= ~(BIT(3)); // "ppm"
	display_buff[5] &= BIT(7);
}

// 88:8.8ppm
static void clear_s3(void) {
	display_buff[3] &= ~(BIT(1) | BIT(2) | BIT(3));
	display_buff[4] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	display_buff[6] = 0;
	display_buff[7] &= BIT(0); // "Monday"
	display_buff[8] = 0;
	display_buff[21] &= ~(BIT(3)); // "."
}

// 88:8.8°E
static void clear_s4(void) {
	display_buff[9] &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
	display_buff[10] &= BIT(4); // "Tuesday"
	display_buff[11] = 0;
	display_buff[12] = 0;
	display_buff[13] &= BIT(0); // "Thursday"
}

// 188:8°E%
static void clear_s5(void) {
	display_buff[14] = 0;
	display_buff[15] = 0;
	display_buff[16] &= BIT(0); // "Friday"
	display_buff[17] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
}

void show_ble_symbol(bool state){
	if (state) {
		display_buff[20] |= BIT(0);
	} else {
		display_buff[20] &= ~(BIT(0));
	}
}

// Off, *, *))
void show_connect_symbol(int state){
	if (state) {
		display_buff[19] |= BIT(0);
		if (state == 2) {
			display_buff[17] |= BIT(0);
		}
	} else {
		display_buff[17] &= ~(BIT(0));
		display_buff[19] &= ~(BIT(0));
	}
}

// Off, On
void show_battery_symbol(bool state){
	if (state) {
		display_buff[21] |= BIT(4);
	} else {
		display_buff[21] &= ~(BIT(4));
	}
}

// CO2
void show_symbol_so2(void) {
	display_buff[21] &= ~(0b11100110);
	if(measured_data.co2 < 400)
		display_buff[21] |= 0b11000000;
	else if(measured_data.co2 < 1000)
		display_buff[21] |= 0b11100000;
	else if(measured_data.co2 < 1500)
		display_buff[21] |= 0b11000010;
	else
		display_buff[21] |= 0b11000100;
}

// Off, °C, °F, °E, =, -, ppm, %
void show_symbol_s2(symbols_e symbol) {
	//display_buff[5] &= ~(BIT(0) | BIT(4) | BIT(5) | BIT(6)); // "°E"
	//display_buff[4] &= ~(BIT(3)); // "ppm"
	if (symbol == SYMBOL_C) {
		display_buff[5] |= BIT(5) | BIT(6); // "°C"
	} else 	if (symbol == SYMBOL_F) {
		display_buff[5] |= BIT(4) | BIT(5); // "°F"
	} else 	if (symbol == SYMBOL_E) {
		display_buff[5] |= BIT(4) | BIT(5) | BIT(6); // "°E"
	} else 	if (symbol == SYMBOL_EQ) {
		display_buff[5] |= BIT(4) | BIT(6); // "="
	} else 	if (symbol == SYMBOL_PPM) {
		display_buff[4] |= BIT(3); // "ppm"
	} else 	if (symbol == SYMBOL_P) {
		display_buff[5] |= BIT(0); // %
	}
}

// Off, 1, 2, 3
void show_s2_busy_symbol(int state){
	display_buff[4] &= ~(BIT(0) | BIT(1) | BIT(2));
	display_buff[5] &= ~(BIT(7));
	if (state) {
		display_buff[4] |= BIT(1);
		if (state == 1) {
			display_buff[4] |= BIT(0);
		} else if (state == 2) {
			display_buff[4] |= BIT(0) | BIT(2);
		} else if (state == 3) {
			display_buff[4] |= BIT(0) | BIT(2);
			display_buff[5] |= BIT(7);
		}
	}
}

// Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
const u8 sb_dnd[] = {3,0x01, 7,0x01, 10,0x10, 13,0x10, 13,0x01, 16,0x01, 0,0x08, 0xff, 0xff};

static void lcd_set_dnd(u8 *lcd_buf, int n, const u8 *tab_idx_bits) {
    int i = 0;
    u8 buf_idx, bits;
    while(1) {
    	buf_idx = tab_idx_bits[0];
    	if(buf_idx == 0xff)
    		break;
		bits = tab_idx_bits[1];
    	tab_idx_bits += 2;
    	if(n == i) {
    		lcd_buf[buf_idx] |= bits;
    	} else {
    		lcd_buf[buf_idx] &= ~bits;
    	}
    	i++;
    }
}

// Off, ppm
void show_symbol_s3(symbols_e state){
	if (state) {
		display_buff[8] |= BIT(3); // "ppm"
	} else {
		display_buff[8] &= ~(BIT(3)); // ppm
	}
}

// Off, 1, 2, 3
void show_s3_busy_symbol(int state){
	display_buff[9] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7));
	if (state) {
		display_buff[9] |= BIT(6);
		if (state == 1) {
			display_buff[9] |= BIT(4);
		} else if (state == 2) {
			display_buff[9] |= BIT(4) | BIT(7);
		} else if (state == 3)
			display_buff[9] |= BIT(4) | BIT(5) | BIT(7);
	}
}

// Off, °C, °F, °E, =
void show_symbol_s4(symbols_e symbol) {
	display_buff[13] &= ~(BIT(1) | BIT(2) | BIT(3)); // "°E"
	if (symbol == SYMBOL_C) {
		display_buff[13] |= BIT(2) | BIT(3); // "°C"
	} else 	if (symbol == SYMBOL_F) {
		display_buff[13] |= BIT(1) | BIT(2); // "°F"
	} else 	if (symbol == SYMBOL_E) {
		display_buff[13] |= BIT(1) | BIT(2) | BIT(3); // "°E"
	} else 	if (symbol == SYMBOL_EQ) {
		display_buff[13] |= BIT(1) | BIT(3); // "="
	}
}

// Off, °C, °F, °E, =, -, Off, %
void show_symbol_s5(symbols_e symbol) {
	display_buff[17] &= ~(BIT(4) | BIT(5) | BIT(6) | BIT(7)); // "°E%"
	if (symbol == SYMBOL_C) {
		display_buff[17] |= BIT(5) | BIT(6); // "°C"
	} else 	if (symbol == SYMBOL_F) {
		display_buff[17] |= BIT(4) | BIT(5); // "°F"
	} else 	if (symbol == SYMBOL_E) {
		display_buff[17] |= BIT(4) | BIT(5) | BIT(6); // "°E"
	} else 	if (symbol == SYMBOL_EQ) {
		display_buff[17] |= BIT(4) | BIT(6); // "="
	} else 	if (symbol == SYMBOL_P) {
		display_buff[17] |= BIT(7); // "%"
	}
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
static void lcd_set_digit(u8 *buf, u8 digit, const u8 *segments) {
    // set the segments, there are up to 11 segments in a digit
    int segment_byte;
    int segment_bit;
    for (int i = 0; i < DEF_MJWSD07MMC_SUMBOL_SIGMENTS; i++) {
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

/* Test cmd ():
 * 0400007ceaa49cacbcf0fcc804ffffffff,
 * 0400007c0449
 * 0400007cf3c8 - blink
 */
//const u8 lcd_init_cmd[] = {0xea,0xf0, 0xc0, 0xac, 0x00[18]}; // original
const u8 lcd_init_cmd[]	=	{
		// LCD controller initialize:
		0xea, // Set IC Operation(ICSET): Software Reset, Internal oscillator circuit
		0xf0, // Blink control (BLKCTL): Off
		0xc0, // Mode Set (MODE SET): Display OFF, 1/3 Bias
		0xa4, // Display control (DISCTL): Normal mode, FRAME flip, Power save mode 1
		0xac, // Display control (DISCTL): Power save mode 1, FRAME flip, Power save mode 1
		0xbc, // Display control (DISCTL): Power save mode 3, FRAME flip, Power save mode 1
		0xfc, // All pixel control (APCTL): Normal
		0,	  // ADSet 0
		// clear buffer (26 bytes)
		0,0,0,0,0, 0,0,0,0,0, //10
		0,0,0,0,0, 0,0,0,0,0, //20
		0,0,0,0,0, 0 // 26
};

// runtime: 600 us if I2C 450 kHz
_attribute_ram_code_ void send_to_lcd(void){
	u8 * p = display_buff;
	if(cfg.flg2.screen_off)
		return;
	if (lcd_i2c_addr) {
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
//		reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*400000)); // 400 kHz
		reg_i2c_id = lcd_i2c_addr;
		// Send 3 bytes:
		// LCD cmd: 0xE8 - Set IC Operarion(ICSET):
		//		    Do not execute Software Reset, Internal oscillator circuit;
		// LCD cmd: 0xC8 - Mode Set (MODE SET): Display ON, 1/3 Bias
		// LCD cmd: 0x00 - ADSET 0
		reg_i2c_dat_ctrl = ((FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_DI) << 24) | 0x00C8E8;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		// Send 22 bytes:
		for(int i = 0; i < sizeof(display_buff) - 1; i++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_do = *p;
		reg_i2c_ctrl = FLD_I2C_CMD_DO | FLD_I2C_CMD_STOP;
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
	lcd_i2c_addr = (u8) scan_i2c_addr(AIP31729_I2C_ADDR << 1);
	if (lcd_i2c_addr) {
//		reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*400000)); // 400 kHz
		if(cfg.flg2.screen_off) {
			lcd_send_i2c_byte(0xEA); // AIP31729 reset
		} else {
			lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd)); // sleep: 15.5 uA
			pm_wait_us(200);
			//memset(display_buff, 0xff, sizeof(display_buff));
			send_to_lcd();
		}
	}
}

void reinit_lcd(void) {
	memset(display_cmp_buff, 0, sizeof(display_cmp_buff));
	init_lcd();
}


/* number in 0.1 (-9995..9999.5) show: -999..-99.9..999.9..9999
 * atr: 0 - Off, 1 - °C, 2 - °F, 3 - °E, 4 - =, 5 - ppm, 6 - % */
static void show_s2_number_x10(s32 number, u8 atr){
	clear_s2();
	show_symbol_s2(atr);
	if (number > 99995) {
		// "Hi"
		display_buff[0] |= BIT(0) | BIT(1) | BIT(2) | BIT(5) | BIT(6);
		display_buff[1] |= BIT(6);
	} else if (number < -9995) {
		// "Lo"
		display_buff[0] |= BIT(5) | BIT(6) | BIT(7);
		display_buff[1] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
	} else {
		if (number > 9999 || number < -999) {
			// no point, show: -999..9999
			if (number < 0){
				number = -number;
				display_buff[0] |= BIT(1); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -99.9..999.9
			display_buff[2] |= BIT(3); // point
			if (number < 0){
				number = -number;
				display_buff[0] |= BIT(1); // "-"
			}
		}
		/* number: -999..9999 */
		if (number > 999) lcd_set_digit(display_buff, number / 1000 % 10, sb_s2[0]);
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s2[1]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s2[2]);
		else lcd_set_digit(display_buff, 0, sb_s2[2]);
		lcd_set_digit(display_buff, number % 10, sb_s2[3]);
	}
}

/* number in 1 (-999..9999) show: -999..9999
 * atr: 0 - Off, 1 - °C, 2 - °F, 3 - °E, 4 - =, 5 - ppm, 6 - % */
static void show_s2_number_x1(s32 number, u8 atr){
	clear_s2();
	show_symbol_s2(atr);
	if (number > 9999) {
		// "Hi"
		display_buff[0] |= BIT(0) | BIT(1) | BIT(2) | BIT(5) | BIT(6);
		display_buff[1] |= BIT(6);
	} else if (number < -999) {
		// "Lo"
		display_buff[0] |= BIT(5) | BIT(6) | BIT(7);
		display_buff[1] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
	} else {
		if (number < 0){
			number = -number;
			display_buff[0] |= BIT(1); // "-"
		}
		/* number: -999..9999 */
		if (number > 999) lcd_set_digit(display_buff, number / 1000 % 10, sb_s2[0]);
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s2[1]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s2[2]);
		else lcd_set_digit(display_buff, 0, sb_s2[2]);
		lcd_set_digit(display_buff, number % 10, sb_s2[3]);
	}
}


#if 0
/* number in 0.1 (-9995..9999.5) show: -999..-99.9..999.9..9999
 * atr: 0 - Off, != 0 ppm */
static void show_s3_number_x10(s32 number, u8 atr){
	clear_s3();
	show_symbol_s3(atr);
	if (number > 99995) {
		// "Hi"
		display_buff[3] |= BIT(1) | BIT(2) | BIT(3);
		display_buff[4] |= BIT(5) | BIT(6);
		display_buff[6] |= BIT(6);
	} else if (number < -9995) {
		// "Lo"
		display_buff[4] |= BIT(5) | BIT(6) | BIT(7);
		display_buff[6] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
	} else {
		if (number > 9999 || number < -999) {
			// no point, show: -999..9999
			if (number < 0){
				number = -number;
				display_buff[3] |= BIT(2); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -99.9..999.9
			display_buff[21] |= BIT(3); // point
			if (number < 0){
				number = -number;
				display_buff[3] |= BIT(2); // "-"
			}
		}
		/* number: -999..9999 */
		if (number > 999) lcd_set_digit(display_buff, number / 1000 % 10, sb_s3[0]);
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s3[1]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s3[2]);
		else lcd_set_digit(display_buff, 0, sb_s3[2]);
		lcd_set_digit(display_buff, number % 10, sb_s3[3]);
	}
}
#endif

/* number in 1 (-999..9999) show: -999..9999
 * atr: 0 - Off, != 0 ppm */
static void show_s3_number_x1(s32 number, u8 atr){
	clear_s3();
	show_symbol_s3(atr);
	if (number > 9999) {
		// "Hi"
		display_buff[3] |= BIT(1) | BIT(2) | BIT(3);
		display_buff[4] |= BIT(5) | BIT(6);
		display_buff[6] |= BIT(6);
	} else if (number < -999) {
		// "Lo"
		display_buff[4] |= BIT(5) | BIT(6) | BIT(7);
		display_buff[6] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
	} else {
		if (number < 0){
			number = -number;
			display_buff[3] |= BIT(2); // "-"
		}
		/* number: -999..9999 */
		if (number > 999) lcd_set_digit(display_buff, number / 1000 % 10, sb_s3[0]);
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s3[1]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s3[2]);
		else lcd_set_digit(display_buff, 0, sb_s3[2]);
		lcd_set_digit(display_buff, number % 10, sb_s3[3]);
	}
}


/* number in 0.1 (-9995..99995) show: -999..-99.9..999.9..9999
 * atr: Off, °C, °F, °E, = */
static void show_s4_number_x10(s32 number, int atr){
	clear_s4();
	show_symbol_s4(atr);
	if (number > 99995) {
		// "Hi"
		display_buff[9] |= BIT(1) | BIT(2);
		display_buff[10] |= BIT(2) | BIT(5) | BIT(6) | BIT(7);
	} else if (number < -9995) {
		// "Lo"
		display_buff[9] |= BIT(1) | BIT(2) | BIT(3);
		display_buff[10] |= BIT(2) | BIT(3);
		display_buff[11] |= BIT(6) | BIT(7);
	} else {
		if (number > 9999 || number < -999) {
			// no point, show: -999..9999
			if (number < 0){
				number = -number;
				display_buff[10] |= BIT(6); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -99.9..999.9
			display_buff[12] |= BIT(7); // point
			if (number < 0){
				number = -number;
				display_buff[10] |= BIT(6); // "-"
			}
		}
		/* number: -999..9999 */
		if (number > 999) lcd_set_digit(display_buff, number / 1000 % 10, sb_s4[0]);
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s4[1]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s4[2]);
		else lcd_set_digit(display_buff, 0, sb_s4[2]);
		lcd_set_digit(display_buff, number % 10, sb_s4[3]);
	}
}

/* number in 0.1 (-99.5 .1999.5) -99..-9.9..199.9..1999
 * atr: 0 - Off, 1 - °C, 2 - °F, 3 - °E, 4 - =, 5 - Off, 6 - % */
static void show_s5_number_x10(s32 number, u8 atr){
	clear_s5();
	show_symbol_s5(atr);
	if (number > 19995) {
		// "Hi"
		display_buff[14] |= BIT(1) | BIT(2) | BIT(3) | BIT(5) | BIT(6);
		display_buff[15] |= BIT(6);
	} else if (number < -9995) {
		// "Lo"
		display_buff[14] |= BIT(5) | BIT(6) | BIT(7);
		display_buff[15] |= BIT(1) | BIT(2) | BIT(6) | BIT(7);
	} else {
		if (number > 999 || number < -1999) {
			// no point, show: -99..1999
			if (number < 0){
				number = -number;
				display_buff[14] |= BIT(2); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.99..199.99
			display_buff[15] |= BIT(3); // point
			if (number < 0){
				number = -number;
				display_buff[14] |= BIT(2); // "-"
			}
		}
		/* number: -9999..19999 */
		if (number > 999) display_buff[14] |= BIT(0); // "1" 10000..19999
		if (number > 99) lcd_set_digit(display_buff, number / 100 % 10, sb_s5[0]);
		if (number > 9) lcd_set_digit(display_buff, number / 10 % 10, sb_s5[1]);
		else lcd_set_digit(display_buff, 0, sb_s5[1]);
		lcd_set_digit(display_buff, number % 10, sb_s5[2]);
	}
}


static void show_clock_s2(void) {
	clear_s2();
	display_buff[1] = BIT(0); // ":"
	u8 hrs = rtc.hours;
	if(cfg.flg.time_am_pm) {
		if(hrs > 12)
			hrs -= 12;
		else if(!hrs)
			hrs = 12;
	}
	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s2[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s2[1]);
	lcd_set_digit(display_buff, rtc.minutes / 10 % 10, sb_s2[2]);
	lcd_set_digit(display_buff, rtc.minutes % 10, sb_s2[3]);
}

static void show_clock_s3(void) {
	clear_s3();
	display_buff[6] = BIT(0); // ":"
	u8 hrs = rtc.hours;
	if(cfg.flg.time_am_pm) {
		if(hrs > 12)
			hrs -= 12;
		else if(!hrs)
			hrs = 12;
	}
	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s3[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s3[1]);
	lcd_set_digit(display_buff, rtc.minutes / 10 % 10, sb_s3[2]);
	lcd_set_digit(display_buff, rtc.minutes % 10, sb_s3[3]);
}

static void show_clock_s4(void) {
	clear_s4();
	display_buff[11] = BIT(4); // ":"
	u8 hrs = rtc.hours;
	if(cfg.flg.time_am_pm) {
		if(hrs > 12)
			hrs -= 12;
		else if(!hrs)
			hrs = 12;
	}
	lcd_set_digit(display_buff, hrs / 10 % 10, sb_s4[0]);
	lcd_set_digit(display_buff, hrs % 10, sb_s4[1]);
	lcd_set_digit(display_buff, rtc.minutes / 10 % 10, sb_s4[2]);
	lcd_set_digit(display_buff, rtc.minutes % 10, sb_s4[3]);
}

static void show_data_s1(u8 flg_wd) {
	u8 mh, ml, dh, dl;
	clear_s1();
	display_buff[18] |= BIT(0); // "/"
	lcd_set_dnd(display_buff, (!flg_wd)? rtc.weekday : 0xff,  sb_dnd);
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
	lcd_set_digit(display_buff, mh, sb_s1[0]);
	lcd_set_digit(display_buff, ml, sb_s1[1]);
	lcd_set_digit(display_buff, dh, sb_s1[2]);
	lcd_set_digit(display_buff, dl, sb_s1[3]);
}

/*
static void show_battery_s1(u8 level) {
	clear_s2();
	display_buff[21] |= BIT(4); // "bat"
	if(level > 99) {
		lcd_set_digit(display_buff, 1, sb_s2[1]);
		lcd_set_digit(display_buff, 0, sb_s1[2]);
		lcd_set_digit(display_buff, 0, sb_s1[3]);
	} else {
		lcd_set_digit(display_buff, level / 10 % 10, sb_s1[2]);
		lcd_set_digit(display_buff, level % 10, sb_s1[3]);
	}
}
*/

void show_low_bat(void) {
	memset(&display_buff, 0, sizeof(display_buff));
	// s1 "Lo"
	display_buff[19] |= BIT(5) | BIT(6) | BIT(7);
	display_buff[20] |= BIT(2) | BIT(3) | BIT(6) | BIT(7);
	// "bat"
	display_buff[20] |= BIT(0);
	show_s2_number_x10(measured_data.average_battery_mv * 10, 0);
	send_to_lcd();
}

void show_ota_screen(void) {
	clear_s2();
	display_buff[0] = BIT(1); // "-"
	display_buff[1] = BIT(2); // "-"
	display_buff[2] = BIT(1); // "-"
	display_buff[5] = BIT(2); // "-"
	send_to_lcd();
	lcd_send_i2c_byte(0xf2);
}

void show_reboot_screen(void) {
	memset(&display_buff, 0xff, sizeof(display_buff));
	send_to_lcd();
}

_attribute_ram_code_
void lcd(void) {
	if(cfg.flg2.screen_off) {
		return;
	}
	u8 screen_type = cfg.flg2.screen_type;
	if(lcd_flg.chow_ext_ut >= wrk.utc_time_sec)
		screen_type = SCR_TYPE_EXT;
	show_ble_symbol(wrk.ble_connected);
#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
	show_connect_symbol(ext_key.rest_adv_int_tad & 2);
#endif
	show_data_s1(cfg.flg3.not_day_of_week);
	show_symbol_so2();
	switch(screen_type) {
		case SCR_TYPE_TEMP:
			if (cfg.flg.temp_F_or_C) {
				show_s2_number_x10(((measured_data.temp_x01 / 5) * 9) + 320, SYMBOL_F); // convert C to F
			} else {
				show_s2_number_x10(measured_data.temp_x01, SYMBOL_C);
			}
			show_s3_number_x1(measured_data.co2, SYMBOL_PPM);
			show_clock_s4();
			show_s5_number_x10(measured_data.humi_x01, SYMBOL_P);
			show_battery_symbol(measured_data.average_battery_mv < 2400);
			break;
		case SCR_TYPE_HUMI:
			show_s2_number_x10(measured_data.humi_x01, SYMBOL_P);
			show_s3_number_x1(measured_data.co2, SYMBOL_PPM);
			show_clock_s4();
			if (cfg.flg.temp_F_or_C)
				show_s5_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, SYMBOL_F); // convert C to F
			else
				show_s5_number_x10(measured_data.temp_x01, SYMBOL_C);
			show_battery_symbol(measured_data.average_battery_mv < 2400);
			break;
		case SCR_TYPE_BAT_P: // ppm
			show_s2_number_x1(measured_data.co2, SYMBOL_PPM);
			show_clock_s3();
			if (cfg.flg.temp_F_or_C)
				show_s4_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, SYMBOL_F); // convert C to F
			else
				show_s4_number_x10(measured_data.temp_x01, SYMBOL_C);
			show_s5_number_x10(measured_data.humi_x01, SYMBOL_P);
			show_battery_symbol(measured_data.average_battery_mv < 2400);
			break;
		case SCR_TYPE_BAT_V:
			show_battery_symbol(1);
			show_s2_number_x1((measured_data.average_battery_mv), SYMBOL_NONE);
			show_s3_number_x1(measured_data.co2, SYMBOL_PPM);
			if (cfg.flg.temp_F_or_C)
				show_s4_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, SYMBOL_F); // convert C to F
			else
				show_s4_number_x10(measured_data.temp_x01, SYMBOL_C);
			show_s5_number_x10(measured_data.humi_x01, SYMBOL_P);
			break;
		case SCR_TYPE_EXT:
			show_s2_number_x10(ext.number, ext.flg.symbol);
			show_battery_symbol(ext.flg.battery);
			show_s2_busy_symbol(ext.flg.busy);
			show_s3_number_x1(measured_data.co2, SYMBOL_PPM);
			if (cfg.flg.temp_F_or_C)
				show_s4_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, SYMBOL_F); // convert C to F
			else
				show_s4_number_x10(measured_data.temp_x01, SYMBOL_C);
			show_s5_number_x10(measured_data.humi_x01, SYMBOL_P);
			break;
		default: // SCR_TYPE_TIME
			show_clock_s2();
			show_s3_number_x1(measured_data.co2, SYMBOL_PPM);
			if (cfg.flg.temp_F_or_C)
				show_s4_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10, SYMBOL_F); // convert C to F
			else
				show_s4_number_x10(measured_data.temp_x01, SYMBOL_C);
			show_s5_number_x10(measured_data.humi_x01, SYMBOL_P);
			show_battery_symbol(measured_data.average_battery_mv < 2400);
	}
}

#endif // DEVICE_TYPE == DEVICE_MJWSD07MMC
