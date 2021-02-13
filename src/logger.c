/*
 * logger.c
 *
 *  Created on: 29.01.2021
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if USE_FLASH_MEMO
#include "app.h"
#include "drivers.h"
#include "flash_eep.h"
#include "logger.h"
#include "ble.h"

#define FLASH_ADDR_START_MEMO	0x40000
#define FLASH_ADDR_END_MEMO		0x74000 // 49 sectors

#define MEMO_SEC_COUNT		((FLASH_ADDR_END_MEMO - FLASH_ADDR_START_MEMO) / FLASH_SECTOR_SIZE) // 49 sectors
#define MEMO_SEC_RECS		((FLASH_SECTOR_SIZE-sizeof(memo_head_t))/sizeof(memo_blk_t)) // -  sector: 409 records
//#define MEMO_REC_COUNT		(MEMO_SEC_RECS*(MEMO_SEC_COUNT-1))// max 48*409 = 20041 records

#define MEMO_SEC_ID		0x55AAC0DE // sector head

#define _flash_erase_sector(a) flash_erase_sector(FLASH_BASE_ADDR + a)
#define _flash_write_dword(a,d) { unsigned int _dw = d; flash_write_all_size(FLASH_BASE_ADDR + a, 4, (unsigned char *)&_dw); }
#define _flash_write(a,b,c) flash_write_all_size(FLASH_BASE_ADDR + a, b, (unsigned char *)c)
#define _flash_read(a,b,c) flash_read_page(FLASH_BASE_ADDR + a, b, (u8 *)c)

typedef struct _summ_data_t {
	uint32_t	battery_mv; // mV
	int32_t		temp; // x 0.01 C
	uint32_t	humi; // x 0.01 %
	uint32_t 	count;
} summ_data_t;
RAM summ_data_t summ_data;

RAM memo_inf_t memo;
RAM memo_rd_t rd_memo;

static uint32_t test_next_memo_sec_addr(uint32_t faddr) {
	uint32_t mfaddr = faddr;
	if (mfaddr >= FLASH_ADDR_END_MEMO)
		mfaddr = FLASH_ADDR_START_MEMO;
	else if (mfaddr < FLASH_ADDR_START_MEMO)
		mfaddr = FLASH_ADDR_END_MEMO - FLASH_SECTOR_SIZE;
	return mfaddr;
}

static void memo_sec_init(uint32_t faddr) {
	uint32_t mfaddr = faddr;
	mfaddr &= ~(FLASH_SECTOR_SIZE-1);
	_flash_erase_sector(mfaddr);
	_flash_write_dword(mfaddr, MEMO_SEC_ID);
	memo.faddr = mfaddr + sizeof(memo_head_t);
	memo.cnt_cur_sec = 0;
}

static void memo_sec_close(uint32_t faddr) {
	uint32_t mfaddr = faddr;
	uint16_t flg = 0;
	mfaddr &= ~(FLASH_SECTOR_SIZE-1);
	_flash_write(mfaddr + sizeof(memo_head_t) - sizeof(flg), sizeof(flg), &flg);
	memo_sec_init(test_next_memo_sec_addr(mfaddr + FLASH_SECTOR_SIZE));
}

#if 0
void memo_init_count(void) {
	memo_head_t mhs;
	uint32_t cnt, i = 0;
	uint32_t faddr = memo.faddr & (~(FLASH_SECTOR_SIZE-1));
	cnt = memo.faddr - faddr - sizeof(memo_head_t); // смещение в секторе
	cnt /= sizeof(memo_blk_t);
	do {
		faddr = test_next_memo_sec_addr(faddr - FLASH_SECTOR_SIZE);
		_flash_read(faddr, &mhs, sizeof(mhs));
		i++;
	} while(mhs.id == MEMO_SEC_ID && mhs.flg == 0 && i < MEMO_SEC_COUNT);
	cnt += i *MEMO_SEC_RECS;
	memo.count = cnt;
}
#endif

void memo_init(void) {
	memo_head_t mhs;
	uint32_t tmp, fsec_end;
	uint32_t faddr = FLASH_ADDR_START_MEMO;
	memo.cnt_cur_sec = 0;
	while(faddr < FLASH_ADDR_END_MEMO) {
		_flash_read(faddr, sizeof(mhs), &mhs);
		if(mhs.id != MEMO_SEC_ID) {
			memo_sec_init(faddr);
			return;
		} else if(mhs.flg == 0xffff) {
			fsec_end = faddr + FLASH_SECTOR_SIZE;
			faddr += sizeof(memo_head_t);
			while(faddr < fsec_end) {
				_flash_read(faddr, sizeof(tmp), &tmp);
				if(tmp == 0xffffffff) {
					memo.faddr = faddr;
					return;
				}
				utc_time_sec = tmp + 5;
				memo.cnt_cur_sec++;
				faddr += sizeof(memo_blk_t);
			}
			memo_sec_close(fsec_end - FLASH_SECTOR_SIZE);
			return;
		}
		faddr += FLASH_SECTOR_SIZE;
	}
	memo_sec_init(FLASH_ADDR_START_MEMO);
	return;
}

_attribute_ram_code_
unsigned get_memo(uint32_t bnum, pmemo_blk_t p) {
	memo_head_t mhs;
	uint32_t faddr;
	faddr = rd_memo.saved.faddr & (~(FLASH_SECTOR_SIZE-1));
	if(bnum > rd_memo.saved.cnt_cur_sec) {
		bnum -= rd_memo.saved.cnt_cur_sec;
		faddr -= FLASH_SECTOR_SIZE;
		if(faddr < FLASH_ADDR_START_MEMO)
			faddr = FLASH_ADDR_END_MEMO - FLASH_SECTOR_SIZE;
		while(bnum > MEMO_SEC_RECS) {
			bnum -= MEMO_SEC_RECS;
			faddr -= FLASH_SECTOR_SIZE;
			if(faddr < FLASH_ADDR_START_MEMO)
				faddr = FLASH_ADDR_END_MEMO - FLASH_SECTOR_SIZE;
		}
		bnum = MEMO_SEC_RECS - bnum;
		_flash_read(faddr, sizeof(mhs), &mhs);
		if(mhs.id != MEMO_SEC_ID || mhs.flg != 0)
			return 0;
	} else {
		bnum = rd_memo.saved.cnt_cur_sec - bnum;
	}
	faddr += sizeof(memo_head_t); // смещение в секторе
	faddr += bnum * sizeof(memo_blk_t); // * size memo
	_flash_read(faddr, sizeof(memo_blk_t), p);
	return 1;
}

_attribute_ram_code_
void write_memo(void) {
	memo_blk_t mblk;
	if(cfg.averaging_measurements == 1) {
		mblk.temp = measured_data.temp;
		mblk.humi = measured_data.humi;
		mblk.vbat = measured_data.battery_mv;
	} else {
		summ_data.temp += measured_data.temp;
		summ_data.humi += measured_data.humi;
		summ_data.battery_mv += measured_data.battery_mv;
		summ_data.count++;
		if(cfg.averaging_measurements > summ_data.count)
			return;
		mblk.temp = (int16_t)(summ_data.temp/(int32_t)summ_data.count);
		mblk.humi = (uint16_t)(summ_data.humi/summ_data.count);
		mblk.vbat = (uint16_t)(summ_data.battery_mv/summ_data.count);
		memset(&summ_data, 0, sizeof(summ_data));
	}
	/* default c4: dcdc 1.8V  -> GD flash; 48M clock may error, need higher DCDC voltage
	           c6: dcdc 1.9V
	analog_write(0x0c, 0xc6);
	*/
	if(utc_time_sec == 0xffffffff)
		mblk.time = 0xfffffffe;
	else
		mblk.time = utc_time_sec;
	uint32_t faddr = memo.faddr;
	if(!faddr) {
		memo_init();
		faddr = memo.faddr;
	}
	_flash_write(faddr, sizeof(memo_blk_t), &mblk);
	faddr += sizeof(memo_blk_t);
	faddr &= (~(FLASH_SECTOR_SIZE-1));
	if(memo.cnt_cur_sec >= MEMO_SEC_RECS - 1 ||
		(memo.faddr & (~(FLASH_SECTOR_SIZE-1))) != faddr) {
		memo_sec_close(memo.faddr);
	} else {
		memo.cnt_cur_sec++;
		memo.faddr += sizeof(memo_blk_t);
	}
}

#endif // USE_FLASH_MEMO
