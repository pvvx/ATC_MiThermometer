#ifndef _BATTERY_H_
#define _BATTERY_H_


#define MAX_VBAT_MV		3000 // 3100 mV - > battery = 100% no load, 2950 at load (during measurement)
#define MIN_VBAT_MV		2200 // 2200 mV - > battery = 0%

#define LOW_VBAT_MV		2800 // level set LOW_CONNECT_LATENCY
#define END_VBAT_MV		2000 // It is not recommended to write Flash below 2V, go to deep-sleep

u16 get_adc_mv(u32 p_ain);

#define get_battery_mv() get_adc_mv(SHL_ADC_VBAT)	// Channel B0P/B5P

u8 get_battery_level(u16 battery_mv);

#endif // _BATTERY_H_
