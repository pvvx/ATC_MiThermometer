/*
 * scanning.h
 *
 *  Created on: 31 янв. 2025 г.
 *      Author: pvvx
 */

#ifndef _SCANING_H_
#define _SCANING_H_

#define SCAN_USE_BINDKEY	0

typedef struct {
	u32	start_tik;		// = 0 - сканирование отключено (разрешить sleep), !=0 - штамп времени старта сканирования
	u32	start_time;		//
	u32	localt;			// +3 часа
	u32 interval;		// интервал вызова в сек
	u8 	MAC[6]; 		// [0] - lo, .. [6] - hi digits
#if SCAN_USE_BINDKEY
	u8 	bindkey[16]; 	// for ext dev MAC
#endif
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
