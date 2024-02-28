/*
 * rtc.h
 *
 *  Created on: 02.03.2023
 *      Author: pvvx
 */

#ifndef RTC_H_
#define RTC_H_

#if	(DEV_SERVICES & SERVICE_HARD_CLOCK)

typedef struct {
  uint8_t hours;    // RTC Time Hours: 0-12 / 0-23 range if H12 (AM/PM)
  uint8_t minutes;  // RTC Time Minutes: 0-59 range.
  uint8_t seconds;  // RTC Time Seconds: 0-59 range.
  uint8_t weekday;  // RTC Date WeekDay: 0-6 range.
  uint8_t month;    // RTC Date Month
  uint8_t days;     // RTC Date Days: 1-31 range.
  uint8_t year;     // RTC Date Year: 0-99 range.
} rtc_time_t;


typedef struct _rtc_pcf_reg_t{
	uint8_t sec;
	uint8_t min;
	uint8_t hrs;
	uint8_t days;
	uint8_t wkds;
	uint8_t cmnths;
	uint8_t years;
} rtc_pcf_reg_t;

typedef union _rtc_registers_t{
	rtc_pcf_reg_t r;
	uint8_t uc[7];
} rtc_registers_t;

typedef struct __attribute__((packed)) _rtc_regs_t {
	uint8_t reg_addr;
	rtc_registers_t reg;
} rtc_regs_t;

extern uint8_t rtc_i2c_addr;
extern rtc_regs_t rtc_reg;
extern rtc_time_t rtc;
extern uint32_t rtc_sync_utime;

void init_rtc(void);
int rtc_read_all(void);
uint32_t rtc_get_utime(void);
void rtc_set_utime(uint32_t ut);

// Conversion Utilities
uint32_t rtc_to_utime(rtc_time_t *r); // convert RTC date/time structures to unix time (sec)
void utime_to_rtc(uint32_t ut, rtc_time_t *r); // convert unix time to RTC date/time
uint8_t bcd_to_byte(uint8_t bcd);
uint8_t byte_to_bcd(uint8_t b);
void rtc_to_regs(rtc_time_t *r);
void rtc_regs(rtc_time_t *r);

#endif // (DEV_SERVICES & SERVICE_HARD_CLOCK)

#endif /* RTC_H_ */
