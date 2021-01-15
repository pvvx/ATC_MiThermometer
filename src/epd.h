#pragma once
/* Based on source: https://github.com/znanev/ATC_MiThermometer */
#include <stdint.h>

//----------------------------------
// define some constants
//----------------------------------
#define LOW     0
#define HIGH    1

//----------------------------------
// define display commands
//----------------------------------
#define PANEL_SETTING 0x00
#define POWER_SETTING 0x01
#define POWER_OFF 0x02
#define POWER_OFF_SEQUENCE_SETTING 0x03
#define POWER_ON 0x04
#define DISPLAY_REFRESH 0x12
#define PARTIAL_DISPLAY_REFRESH 0x15
#define DATA_START_TRANSMISSION_1 0x18
#define DATA_START_TRANSMISSION_2 0x1c
#define LUT_FOR_VCOM 0x20
#define LUT_CMD_0x23 0x23
#define LUT_CMD_0x24 0x24
#define LUT_CMD_0x25 0x25
#define LUT_CMD_0x26 0x26
#define PLL_CONTROL 0x30

//----------------------------------
// define groups of segments into logical shapes
//----------------------------------
#define TOP_LEFT_1 1
#define TOP_LEFT 2
#define TOP_MIDDLE 3
#define TOP_RIGHT 4
#define BOTTOM_LEFT 5
#define BOTTOM_RIGHT 6
#define BACKGROUND 7
#define BATTERY_LOW 8
#define DASHES 9
#define FACE 10
#define FACE_SMILE 11
#define FACE_FROWN 12
#define FACE_NEUTRAL 13
#define SUN 14
#define FIXED 15
#define FIXED_DEG_C 16
#define FIXED_DEG_F 17
#define MINUS 18
#define ATC 19

