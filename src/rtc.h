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
  u8 hours;    // RTC Time Hours: 0-12 / 0-23 range if H12 (AM/PM)
  u8 minutes;  // RTC Time Minutes: 0-59 range.
  u8 seconds;  // RTC Time Seconds: 0-59 range.
  u8 weekday;  // RTC Date WeekDay: 0-6 range.
  u8 month;    // RTC Date Month
  u8 days;     // RTC Date Days: 1-31 range.
  u8 year;     // RTC Date Year: 0-99 range.
} rtc_time_t;


typedef struct _rtc_pcf_reg_t{
	u8 sec;
	u8 min;
	u8 hrs;
	u8 days;
	u8 wkds;
	u8 cmnths;
	u8 years;
} rtc_pcf_reg_t;

typedef union _rtc_registers_t{
	rtc_pcf_reg_t r;
	u8 uc[7];
} rtc_registers_t;

typedef struct __attribute__((packed)) _rtc_regs_t {
	u8 reg_addr;
	rtc_registers_t reg;
} rtc_regs_t;

extern u8 rtc_i2c_addr;
extern rtc_regs_t rtc_reg;
extern rtc_time_t rtc;
extern u32 rtc_sync_utime;

void init_rtc(void);
int rtc_read_all(void);
u32 rtc_get_utime(void);
void rtc_set_utime(u32 ut);

// Conversion Utilities
u32 rtc_to_utime(rtc_time_t *r); // convert RTC date/time structures to unix time (sec)
void utime_to_rtc(u32 ut, rtc_time_t *r); // convert unix time to RTC date/time
u8 bcd_to_byte(u8 bcd);
u8 byte_to_bcd(u8 b);
void rtc_to_regs(rtc_time_t *r);
void rtc_regs(rtc_time_t *r);

#endif // (DEV_SERVICES & SERVICE_HARD_CLOCK)

#endif /* RTC_H_ */
