/*
 * scanning.h
 *
 *  Created on: 31 янв. 2025 г.
 *      Author: pvvx
 */

#ifndef _SCANING_H_
#define _SCANING_H_

#define SCAN_USE_BINDKEY	0

// saved to EEP_ID_SCN
typedef struct {
	u32 interval;		// интервал вызова в сек, =0 - отключено
	u8 	MAC[6]; 		// MAC сервера [0] - lo, .. [6] - hi digits
#if SCAN_USE_BINDKEY
	u8 	bindkey[16]; 	// for ext dev MAC
#endif
} scan_cfg_t;

typedef struct {
	u32	start_tik;		// = 0 - сканирование отключено (разрешить sleep), !=0 - штамп времени старта сканирования
	u32	start_time;		// = wrk.utc_time_sec при старте каждого интервала
	scan_cfg_t cfg;
	u8 	enabled;
} scan_wrk_t;

extern scan_wrk_t scan;

//////////////////////////////////////////////////////////
// scan stop
//////////////////////////////////////////////////////////
inline void scan_stop(void) {
	scan.enabled = 0; // stop scan
	scan.start_tik = 0;
}

void scan_init(void);
void scan_wakeup(void);
void scan_start(void);
void scan_task(void);

#endif /* _SCANNING_H_ */
