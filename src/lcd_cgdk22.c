#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if DEVICE_TYPE == DEVICE_CGDK22
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "app.h"
#include "battery.h"
#include "i2c.h"
#include "lcd.h"


/* t,H,h,L,o,i  0xe2,0x67,0x66,0xe0,0xC6,0x40 */
#define LCD_SYM_H   0x67    // "H"
#define LCD_SYM_i   0x40    // "i"
#define LCD_SYM_L   0xE0    // "L"
#define LCD_SYM_o   0xC6    // "o"

#define LCD_SYM_BLE 0x10    // connect
#define LCD_SYM_BAT 0x08    // battery
#define LCD_I2C_ADDR 0x7C   // I2C slave address of the LCD controller (including R/~W bit)

RAM uint8_t display_buff[12];
static RAM uint8_t display_cmp_buff[12];

#define LCD_CMD_MORE 0x80

#define LCD_CMD_ADDRESS_SET_OPERATION       0 /* Address in nibbles (half byte). 5 LSB bits. MSB bit is handled using SET_IC command */

#define LCD_CMD_SET_IC_OPERATION            0x68
#define LCD_CMD_SET_IC_EXTERNAL_CLOCK       BIT(0)
#define LCD_CMD_SET_IC_RESET                BIT(1)
#define LCD_CMD_SET_IC_RAM_MSB              BIT(2)

#define LCD_CMD_DISPLAY_CONTROL_OPERATION   0x20
#define LCD_CMD_DISPLAY_CONTROL_POWER(X)    (X) /* (X <= 3) Lower power, less consumption but less visibility. Default: 2 */
#define LCD_CMD_DISPLAY_CONTROL_FRAME_INV   4 /* Frame inversion consumes less power: use it if possible */
#define LCD_CMD_DISPLAY_CONTROL_80HZ        (0 * BIT(3))
#define LCD_CMD_DISPLAY_CONTROL_71HZ        (1 * BIT(3))
#define LCD_CMD_DISPLAY_CONTROL_64HZ        (2 * BIT(3))
#define LCD_CMD_DISPLAY_CONTROL_53HZ        (3 * BIT(3)) /* Lower refresh rate, lower consumer but can be visible flicker */

#define LCD_CMD_MODE_SET_OPERATION          0x40
#define LCD_CMD_MODE_SET_1_3_BIAS           0
#define LCD_CMD_MODE_SET_1_2_BIAS           4
#define LCD_CMD_MODE_SET_DISPLAY_OFF        0
#define LCD_CMD_MODE_SET_DISPLAY_ON         8

#define LCD_CMD_BLINK_CONTROL_OPERATION     0x70
#define LCD_CMD_BLINK_OFF                   0
#define LCD_CMD_BLINK_HALF_HZ               1
#define LCD_CMD_BLINK_1_HZ                  2
#define LCD_CMD_BLINK_2_HZ                  3

#define LCD_CMD_ALL_PIXELS_OPERATION        0x7C
#define LCD_CMD_ALL_PIXELS_ON               BIT(1)
#define LCD_CMD_ALL_PIXELS_OFF              BIT(2)


// LCD cells where we can show characters
enum cell_t {
    CELL_A, // Top Left
    CELL_B,
    CELL_C,
    CELL_X,
    CELL_Y,
    CELL_Z, // Bottom right
};

// Characters that we can render in a cell
enum lcd_char_t {
    CHR_0,
    CHR_1,
    CHR_2,
    CHR_3,
    CHR_4,
    CHR_5,
    CHR_6,
    CHR_7,
    CHR_8,
    CHR_9,
    CHR_L,
    CHR_o,
    CHR_H,
    CHR_i,
    CHR_MINUS,
    CHR_SPACE
};


// Segments of a cell, following SEG_<ROW><COL> format, where
//   ROW is a number from 1 to 5. 1 is the bottom, 3 the middle, 5 the top
//   COL is either L(eft), M(iddle) or R(ight)
enum cell_segment_t {
    SEG_2L,
    SEG_3L,
    SEG_4L,
    SEG_5L,
    SEG_1L,
    SEG_1M,
    SEG_3M,
    SEG_5M,
    SEG_1R,
    SEG_2R,
    SEG_3R,
    SEG_4R,
    SEG_5R,
    SEG_LAST, // For iteration
};

// Other segments in the LCD
enum other_segment_t {
    SEG_DEGREE_CELSIUS = 4,
    SEG_DEGREE_FAHRENHEIT,
    SEG_DEGREE_COMMON,

    SEG_BIG_ONE_HUNDRED = 2 * 8,
    SEG_BATTERY_L1,
    SEG_BATTERY_L2,
    SEG_BATTERY_L3,
    SEG_BLUETOOTH,
    SEG_BATTERY,
    SEG_BATTERY_L5,
    SEG_BATTERY_L4,

    SEG_BIG_DECIMAL_DOT = 6 * 8 + 4,

    SEG_SMALL_DECIMAL_DOT = 9 * 8,
    SEG_PERCENTAGE,
};

// Segments associated with each char
static const uint16_t char_segment_bitmap[] = {
    // 0
    BIT(SEG_1M) | BIT(SEG_2L) | BIT(SEG_2R) | BIT(SEG_3L) | BIT(SEG_3R) | BIT(SEG_4L) | BIT(SEG_4R) | BIT(SEG_5M),
    // 1
    BIT(SEG_1R) | BIT(SEG_2R) | BIT(SEG_3R) | BIT(SEG_4R) | BIT(SEG_5R),
    // 2
    BIT(SEG_1M) | BIT(SEG_2L) | BIT(SEG_3M) | BIT(SEG_4R) | BIT(SEG_5M),
    // 3
    BIT(SEG_1M) | BIT(SEG_2R) | BIT(SEG_3M) | BIT(SEG_4R) | BIT(SEG_5M),
    // 4
    BIT(SEG_1R) | BIT(SEG_2R) | BIT(SEG_3M) | BIT(SEG_3R) | BIT(SEG_4L) | BIT(SEG_4R) | BIT(SEG_5L),
    // 5
    BIT(SEG_1M) | BIT(SEG_2R) | BIT(SEG_3L) | BIT(SEG_3M) | BIT(SEG_4L) | BIT(SEG_5L) | BIT(SEG_5M),
    // 6
    BIT(SEG_1M) | BIT(SEG_2R) | BIT(SEG_2L) | BIT(SEG_3L) | BIT(SEG_3M) | BIT(SEG_4L) | BIT(SEG_5M),
    // 7
    BIT(SEG_1R) | BIT(SEG_2R) | BIT(SEG_3R) | BIT(SEG_4R) | BIT(SEG_5M) | BIT(SEG_5R),
    // 8
    BIT(SEG_1M) | BIT(SEG_2L) | BIT(SEG_2R) | BIT(SEG_3M) | BIT(SEG_4L) | BIT(SEG_4R) | BIT(SEG_5M),
    // 9
    BIT(SEG_1R) | BIT(SEG_2R) | BIT(SEG_3M) | BIT(SEG_3R) | BIT(SEG_4L) | BIT(SEG_4R) | BIT(SEG_5M),
    // L
    BIT(SEG_1R) | BIT(SEG_1M) | BIT(SEG_2L) | BIT(SEG_3L) | BIT(SEG_4L),
    // o
    BIT(SEG_1M) | BIT(SEG_2L) | BIT(SEG_2R) | BIT(SEG_3M),
    // H
    BIT(SEG_1L) | BIT(SEG_1R) | BIT(SEG_2L) | BIT(SEG_2R) | BIT(SEG_3L) | BIT(SEG_3M) | BIT(SEG_3R)
        | BIT(SEG_4L) | BIT(SEG_4R) | BIT(SEG_5L) | BIT(SEG_5R),
    // i
    BIT(SEG_1L) | BIT(SEG_2L) | BIT(SEG_5L),
    // MINUS
    BIT(SEG_3M),
    // SPACE
    0,
};

static const uint8_t cell_c_segment_bit[] = {
    9,  //SEG_2L,
    10, //SEG_3L,
    11, //SEG_4L,
    15, //SEG_5L,
    8,  //SEG_1L,
    12, //SEG_1M,
    13, //SEG_3M,
    14, //SEG_5M,
    53, //SEG_1R,
    0,  //SEG_2R,
    1,  //SEG_3R,
    2,  //SEG_4R,
    3,  //SEG_5R,
};

static const uint8_t segment_5r_bit[] = {
    6*8 + 7,  // CELL_A
    6*8 + 6,  // CELL_B
    6*8 + 0,  // CELL_C
    9*8 + 3,  // CELL_X
    9*8 + 2,  // CELL_Y
    0*8 + 7,  // CELL_Z
};

static const uint8_t cell_base_bit[] = {
    3*8  + 0, // CELL_A
    4*8  + 4, // CELL_B
    0,
    6*8  + 4, // CELL_X
    8*8  + 0, // CELL_Y
    10*8 + 0  // CELL_Z
};

static void lcd_send_start() {
    if((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
        init_i2c();
    reg_i2c_id = LCD_I2C_ADDR;
    reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

static void lcd_send_stop() {
    reg_i2c_ctrl = FLD_I2C_CMD_STOP;
    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

static void lcd_send_byte(uint8_t byte) {
    reg_i2c_do = byte;
    reg_i2c_ctrl = FLD_I2C_CMD_DO;
    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
}

static inline void lcd_send_cmd(uint8_t cmd) {
    lcd_send_byte(LCD_CMD_MORE | cmd);
}

static void lcd_send_last_cmd(uint8_t cmd, uint8_t * dataBuf, uint32_t dataLen) {
    lcd_send_byte(cmd);
    while (dataLen--) lcd_send_byte(*dataBuf++);
    lcd_send_stop();
}

void init_lcd(void){
    // Ensure than 100us has been elapsed since the IC was powered on
    pm_wait_us(100);

    // Ensure that there is no open I2C transaction
    lcd_send_start();
    lcd_send_stop();

     // Send reset command
    lcd_send_start();
    lcd_send_last_cmd(LCD_CMD_SET_IC_OPERATION
            | LCD_CMD_SET_IC_RESET, 0, 0);

    // Configure and clean the display
    memset(display_buff, 0, sizeof display_buff);
    memset(display_cmp_buff, 0, sizeof display_cmp_buff);
    lcd_send_start();
    lcd_send_cmd(LCD_CMD_DISPLAY_CONTROL_OPERATION
            | LCD_CMD_DISPLAY_CONTROL_FRAME_INV
            | LCD_CMD_DISPLAY_CONTROL_64HZ
            | LCD_CMD_DISPLAY_CONTROL_POWER(1));
    lcd_send_cmd(LCD_CMD_MODE_SET_OPERATION | LCD_CMD_MODE_SET_DISPLAY_ON);
    lcd_send_last_cmd(LCD_CMD_ADDRESS_SET_OPERATION, display_buff, sizeof display_buff);
}

_attribute_ram_code_ void update_lcd(){
    for (int i = 0; i < sizeof display_buff; ++i) {
        if (display_buff[i] != display_cmp_buff[i]) {
            int j = 12;
            while (j > i && display_buff[j-1] == display_cmp_buff[j-1]) {
                --j;
            }
            lcd_send_start();
            lcd_send_last_cmd(LCD_CMD_ADDRESS_SET_OPERATION + i * 2, display_buff + i, j - i);
            memcpy(display_cmp_buff + i, display_buff + i, j - i);
            return;
        }
    }
}


static _attribute_ram_code_ void draw_segment(int bit, int value) {
    int byte = bit / 8;
    if (byte < sizeof display_buff) {
        uint8_t mask = 1 << (bit % 8);
        if (value) {
            display_buff[byte] |= mask;
        } else {
            display_buff[byte] &= ~mask;
        }
    }
}

static _attribute_ram_code_ int get_cell_segment_bit(enum cell_t cell, enum cell_segment_t segment) {
    if (cell == CELL_C) {
        // Digit C does not follow the pattern of other digits
        return cell_c_segment_bit[segment];
    } else if (segment == SEG_5R) {
        // 5R segments do not follow the pattern of the other segments
        return segment_5r_bit[cell];
    } else {
        // The remaining segments can be calculated easily if we swap nibbles
        int bit = cell_base_bit[cell] + segment;
        if (bit % 8 < 4) {
            bit += 4;
        } else {
            bit -= 4;
        }
        return bit;
    }
}

static _attribute_ram_code_ void draw_cell(enum cell_t cell, enum lcd_char_t c) {
    uint16_t bits = char_segment_bitmap[c];
    for (int segment = 0; segment < SEG_LAST; ++segment) {
        draw_segment(get_cell_segment_bit(cell, segment), bits & BIT(segment));
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
_attribute_ram_code_ void show_temp_symbol(uint8_t symbol) {
    draw_segment(SEG_DEGREE_COMMON, symbol & 0x20);
    draw_segment(SEG_DEGREE_FAHRENHEIT, symbol & 0x40);
    draw_segment(SEG_DEGREE_CELSIUS, symbol & 0x80);
}

/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */
_attribute_ram_code_ void show_smiley(uint8_t state){
    // No smiley in this LCD
}

_attribute_ram_code_ void show_ble_symbol(bool state){
    draw_segment(SEG_BLUETOOTH, state);
}

_attribute_ram_code_ void show_battery_symbol(bool state){
    draw_segment(SEG_BATTERY, state);
    draw_segment(SEG_BATTERY_L1, state && battery_level >= 16);
    draw_segment(SEG_BATTERY_L2, state && battery_level >= 33);
    draw_segment(SEG_BATTERY_L3, state && battery_level >= 49);
    draw_segment(SEG_BATTERY_L4, state && battery_level >= 67);
    draw_segment(SEG_BATTERY_L5, state && battery_level >= 83);
}

static _attribute_ram_code_ void draw_number(enum cell_t where, int16_t number, int dot_segment, int hundreds_segment)
{
    int max_value = (hundreds_segment ? 1999 : 999);
    bool show_decimal = (number >= -99 && number <= max_value);
    if (!show_decimal) {
        // Divide by ten and round: first add 5 to absolute value
        number += (number < 0 ? -5 : 5);
        // Then divide by then (which always rounds towards zero
        number /= 10;
    }
    draw_segment(dot_segment, show_decimal);
    if (hundreds_segment) {
        draw_segment(hundreds_segment, number >= 1000 && number <= 1999);
    }
    if (number < -99) {
        draw_cell(where++, CHR_L);
        draw_cell(where++, CHR_o);
        draw_cell(where, CHR_SPACE);
    } else if (number > max_value) {
        draw_cell(where++, CHR_H);
        draw_cell(where++, CHR_i);
        draw_cell(where, CHR_SPACE);
    } else if (number < 0) {
        number = -number;
        draw_cell(where+2, CHR_0 + number % 10);
        number /= 10;
        draw_cell(where+1, CHR_0 + number);
        draw_cell(where, CHR_MINUS);
    } else {
        draw_cell(where+2, CHR_0 + number % 10);
        number /= 10;
        draw_cell(where+1, CHR_0 + number % 10);
        number /= 10;
        if (number == 0) {
            draw_cell(where, CHR_SPACE);
        } else {
            draw_cell(where, CHR_0 + number % 10);
        }
    }
}


/* x0.1 (-995..19995) Show: -99 .. -9.9 .. 199.9 .. 1999 */
static RAM int16_t last_big_number = -32768;
_attribute_ram_code_ void show_big_number_x10(int16_t number){
    if (last_big_number != number) {
        draw_number(CELL_A, number, SEG_BIG_DECIMAL_DOT, SEG_BIG_ONE_HUNDRED);
        last_big_number = number;
    }
}

/* -9.9 .. 99.9 */
static RAM int16_t last_small_number = -32768;
_attribute_ram_code_ void show_small_number_x10(int16_t number, bool percent){
    if (last_small_number != number) {
        draw_number(CELL_X, number, SEG_SMALL_DECIMAL_DOT, 0);
        last_small_number = number;
    }
    draw_segment(SEG_PERCENTAGE, percent);
}


void show_batt_cgdk22(void) {
    uint16_t battery_level = 0;
    if(measured_data.battery_mv > MIN_VBAT_MV) {
        battery_level = ((measured_data.battery_mv - MIN_VBAT_MV)*10)/((MAX_VBAT_MV - MIN_VBAT_MV)/100);
        if (battery_level > 999) {
            battery_level = 999;
        }
    }
    show_small_number_x10(battery_level, false);
}

#if USE_CLOCK
_attribute_ram_code_ void show_clock(void) {
    uint32_t tmp = utc_time_sec / 60;
    uint32_t min = tmp % 60;
    uint32_t hrs = tmp / 60 % 24;
    draw_segment(SEG_BIG_ONE_HUNDRED, false);
    draw_segment(SEG_PERCENTAGE, false);
    draw_segment(SEG_BIG_DECIMAL_DOT, false);
    draw_segment(SEG_SMALL_DECIMAL_DOT, false);
    show_temp_symbol(0);
    draw_cell(CELL_A, CHR_0 + hrs / 10);
    draw_cell(CELL_B, CHR_0 + hrs % 10);
    draw_cell(CELL_C, CHR_SPACE);
    draw_cell(CELL_X, CHR_0 + min / 10);
    draw_cell(CELL_Y, CHR_0 + min % 10);
    draw_cell(CELL_Z, CHR_SPACE);
    last_big_number = -32768;
    last_small_number = -32678;
}
#endif // USE_CLOCK

#endif // DEVICE_TYPE == DEVICE_CGDK2
