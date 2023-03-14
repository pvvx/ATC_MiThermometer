/*
 * rtc_pcf85163.c
 *
 *  Created on: 01.03.2023
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if	USE_RTC
#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "i2c.h"
#include "rtc.h"
#include "app.h"
#define PCF85163_I2C_ADDR 0x51
RAM uint8_t rtc_i2c_addr;
RAM rtc_regs_t rtc_reg;
RAM rtc_time_t rtc;
RAM uint32_t rtc_sync_utime;

#define rtc_send_i2c_rega(a)  	send_i2c_byte(rtc_i2c_addr, a)
#define rtc_set_reg(a, d) send_i2c_word(rtc_i2c_addr, a | (d << 8))
#define rtc_send_i2c_buf(b, s)  send_i2c_buf(rtc_i2c_addr, (uint8_t *) b, s)

int rtc_read_all(void) {
	int ret = -1;
	if (rtc_i2c_addr) {
		// runtime: 240 us if I2C 450 kHz
		ret = read_i2c_byte_addr(rtc_i2c_addr, 2, rtc_reg.reg.uc,
				sizeof(rtc_reg.reg.uc));
	}
	return ret;
}

void rtc_set_regs(void) {
	if (rtc_i2c_addr) {
		rtc_reg.reg_addr = 2;
		rtc_send_i2c_buf(&rtc_reg, sizeof(rtc_reg));
	}
}

// set regs: control_1, control_2
static const uint8_t rtc_init_b1[3] = { 0, 0, 0 };
// disable alarm, clk_out, timer
static const uint8_t rtc_init_b2[7] = { 9, 0x80, 0x80, 0x80, 0x80, 0x03, 0x03 };

void init_rtc(void) {
	// set regs: control_1, control_2
	rtc_i2c_addr = (uint8_t) scan_i2c_addr(PCF85163_I2C_ADDR << 1);
	if (rtc_i2c_addr) {
		// set regs: control_1, control_2
		rtc_send_i2c_buf(rtc_init_b1, sizeof(rtc_init_b1));
		// disable alarm, clk_out, timer
		rtc_send_i2c_buf(rtc_init_b2, sizeof(rtc_init_b2));

		uint32_t utime = rtc_get_utime();

		if (rtc_reg.reg.r.sec & 0x80) {
//			&& utc_time_sec > utime
//			&& utc_time_sec > 946684800) // Sat Jan 01 2000 00:00:00
			rtc_set_utime(utc_time_sec);
		} else {
			utc_time_sec = utime;
			rtc_sync_utime = 0;
		}
	}
}

//------------------------- Conversion Utilities
#define RTC_UNIX_DAYS	((uint32_t)2440588) // 01 Jan 1970 12:00:00 (Unix time)
/* Convert Date/Time structures to unix time (sec) */
uint32_t rtc_to_utime(rtc_time_t *r) {
	uint8_t a, m;
	uint16_t y;
	uint32_t ut;
	// Calculate some coefficients
	a = (uint8_t)(14U - r->month) / 12;
	y = (uint16_t)(r->year + 6800) - a; // years since 1 March, 4801 BC (2000+4800)
	m = (uint8_t)(r->month + (12 * a) - 3);
	// Compute Julian day number (from Gregorian calendar date)
	ut = r->days;
	ut += ((153 * m) + 2) / 5; // Number of days since 1 march
	ut += 365 * y;
	ut += y / 4;
	ut -= y / 100;
	ut += y / 400;
	ut -= 32045;
	// Subtract number of days passed before base date from Julian day number
	ut -= RTC_UNIX_DAYS;
	// Convert days to seconds
	ut *= 86400;
	// Increase epoch time by specified time (in seconds)
	ut += r->hours * 3600;
	ut += r->minutes * 60;
	ut += r->seconds;
	// Number of seconds passed since the base date
	return ut;
}

/* Convert unix time to RTC date/time */
void utime_to_rtc(uint32_t ut, rtc_time_t *r) {
	uint32_t a, b, c, d;
	// calculate utime (Julian day number) from a specified epoch value
	a = (ut / 86400) + RTC_UNIX_DAYS;
	// day of week
	r->weekday = (uint8_t)((a + 1) % 7);
	// calculate intermediate values
	a += 32044;
	b = ((4 * a) + 3) / 146097;
	a -= (146097 * b) / 4;
	c = ((4 * a) + 3) / 1461;
	a -= (1461 * c) / 4;
	d = ((5 * a) + 2) / 153;
	// date and time
	r->days = (uint8_t)(a - (((153 * d) + 2) / 5) + 1);
	r->month = (uint8_t)(d + 3 - (12 * (d / 10)));
	r->year = (uint8_t)((100 * b) + c - 6800 + (d / 10));
	r->hours = (uint8_t)((ut / 3600) % 24);
	r->minutes = (uint8_t)((ut / 60) % 60);
	r->seconds = (uint8_t)(ut % 60);
}

_attribute_ram_code_ uint8_t byte_to_bcd(uint8_t b) {
	uint8_t bcd = 0;
	while (b >= 10) {
		b -= 10;
		bcd += 0x10;
	}
	return bcd + b;
}

_attribute_ram_code_ uint8_t bcd_to_byte(uint8_t bcd) {
	return ((bcd & 0xf0) >> 4) * 10 + (bcd & 0x0f);
}

void rtc_to_regs(rtc_time_t *r) {
	rtc_reg.reg.r.sec = byte_to_bcd(r->seconds);
	rtc_reg.reg.r.min = byte_to_bcd(r->minutes);
	rtc_reg.reg.r.hrs = byte_to_bcd(r->hours);
	rtc_reg.reg.r.wkds = r->weekday & 0x7;
	rtc_reg.reg.r.days = byte_to_bcd(r->days);
	rtc_reg.reg.r.cmnths = byte_to_bcd(r->month);
	rtc_reg.reg.r.years = byte_to_bcd(r->year);
}

void rtc_regs(rtc_time_t *r) {
	r->seconds = bcd_to_byte(rtc_reg.reg.r.sec & 0x7f);
	r->minutes = bcd_to_byte(rtc_reg.reg.r.min & 0x7f);
	r->hours = bcd_to_byte(rtc_reg.reg.r.hrs & 0x3f);
	r->weekday = rtc_reg.reg.r.wkds & 0x07;
	r->days = bcd_to_byte(rtc_reg.reg.r.days & 0x3f);
	r->month = bcd_to_byte(rtc_reg.reg.r.cmnths & 0x1f);
	r->year = bcd_to_byte(rtc_reg.reg.r.years);
}

// -----------------------

void rtc_set_utime(uint32_t ut) {
	utime_to_rtc(ut, &rtc);
	rtc_to_regs(&rtc);
	rtc_set_regs();
	rtc_sync_utime = 0;
}

uint32_t rtc_get_utime(void) {
	if(rtc_read_all() == 0)
		rtc_regs(&rtc);
	else
		utime_to_rtc(utc_time_sec, &rtc);
	return rtc_to_utime(&rtc);
}

#endif // USE_RTC
