/*
 * logger.h
 *
 *  Created on: 29.01.2021
 *      Author: pvvx
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_
#include "app_config.h"

#if USE_FLASH_MEMO

typedef struct _memo_blk_t {
	uint32_t time;  // time (UTC)
	int16_t temp;	// x0.01 C
	uint16_t humi;  // x0.01 %
	uint16_t vbat;  // mV
}memo_blk_t, * pmemo_blk_t;

typedef struct _memo_inf_t {
	uint32_t faddr;
	uint32_t cnt_cur_sec;
}memo_inf_t;

typedef struct _memo_rd_t {
	memo_inf_t saved;
	uint32_t cnt;
	uint32_t cur;
}memo_rd_t;

typedef struct _memo_head_t {
	uint32_t id;  // = 0x55AAC0DE (MEMO_SEC_ID)
	uint16_t flg;  // = 0xffff - new sector, = 0 close sector
}memo_head_t;

extern memo_rd_t rd_memo;
extern memo_inf_t memo;

void memo_init(void);
unsigned get_memo(uint32_t bnum, pmemo_blk_t p);
void write_memo(void);

#endif // USE_FLASH_MEMO
#endif /* _LOGGER_H_ */
