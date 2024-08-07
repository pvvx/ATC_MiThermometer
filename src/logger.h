/*
 * logger.h
 *
 *  Created on: 29.01.2021
 *      Author: pvvx
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_
#include "app_config.h"

#if (DEV_SERVICES & SERVICE_HISTORY)

#ifndef USE_MEMO_1M
#define USE_MEMO_1M		1
#endif

#define MEMO_SEC_ID		0x55AAC0DE // sector head
#define FLASH_ADDR_START_MEMO	0x40000
#define FLASH_ADDR_END_MEMO		0x74000 // 49 sectors

#define FLASH1M_ADDR_START_MEMO	0x80000
#define FLASH1M_ADDR_END_MEMO	0x100000 // 128 sectors

typedef struct _memo_blk_t {
	uint32_t time;  // time (UTC)
	int16_t val1; // temp;	// x0.01 C
	uint16_t val2; // humi;  // x0.01 %
	uint16_t val0; // vbat;  // mV
}memo_blk_t, * pmemo_blk_t;

typedef struct _memo_inf_t {
	uint32_t faddr;
	uint32_t cnt_cur_sec;
#if USE_MEMO_1M
	uint32_t sectors;
	uint32_t start_addr;
	uint32_t end_addr;
#endif
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
void clear_memo(void);
unsigned get_memo(uint32_t bnum, pmemo_blk_t p);
void write_memo(void);

#endif // #if (DEV_SERVICES & SERVICE_HISTORY)
#endif /* _LOGGER_H_ */
