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
	u32 time;  // time (UTC)
	s16 val1; // temp;	// x0.01 C
	u16 val2; // humi;  // x0.01 %
	u16 val0; // vbat;  // mV
}memo_blk_t, * pmemo_blk_t;

typedef struct _memo_inf_t {
	u32 faddr;
	u32 cnt_cur_sec;
#if USE_MEMO_1M
	u32 sectors;
	u32 start_addr;
	u32 end_addr;
#endif
}memo_inf_t;

typedef struct _memo_rd_t {
	memo_inf_t saved;
	u32 cnt;
	u32 cur;
}memo_rd_t;

typedef struct _memo_head_t {
	u32 id;  // = 0x55AAC0DE (MEMO_SEC_ID)
	u16 flg;  // = 0xffff - new sector, = 0 close sector
}memo_head_t;

extern memo_rd_t rd_memo;
extern memo_inf_t memo;

void memo_init(void);
void clear_memo(void);
unsigned get_memo(u32 bnum, pmemo_blk_t p);
void write_memo(void);

#endif // #if (DEV_SERVICES & SERVICE_HISTORY)
#endif /* _LOGGER_H_ */
