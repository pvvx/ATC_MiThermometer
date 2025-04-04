/*
 * logger.c
 *
 *  Created on: 29.01.2021
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_config.h"
#if (DEV_SERVICES & SERVICE_HISTORY)
#include "app.h"
#include "drivers.h"
#include "flash_eep.h"
#include "logger.h"
#include "ble.h"

#define MEMO1M_SEC_COUNT	((FLASH1M_ADDR_END_MEMO - FLASH1M_ADDR_START_MEMO) / FLASH_SECTOR_SIZE) // 52 or 128 sectors
#define MEMO1M_SEC_RECS		((FLASH_SECTOR_SIZE-sizeof(memo_head_t))/sizeof(memo_blk_t)) // 1 sector = 409 records
//#define MEMO1M_REC_COUNT	(MEMO1M_SEC_RECS*(MEMO1M_SEC_COUNT-1))// max 51*409 = 20859 records or 127*409 = 51943 records

#define MEMO_SEC_COUNT		((FLASH_ADDR_END_MEMO - FLASH_ADDR_START_MEMO) / FLASH_SECTOR_SIZE) // 52 or 128 sectors
#define MEMO_SEC_RECS		((FLASH_SECTOR_SIZE-sizeof(memo_head_t))/sizeof(memo_blk_t)) // 1 sector = 409 records
//#define MEMO_REC_COUNT		(MEMO_SEC_RECS*(MEMO_SEC_COUNT-1))// max 51*409 = 20859 records or 127*409 = 51943 records

#define _flash_erase_sector(a) flash_erase_sector(FLASH_BASE_ADDR + a)
#define _flash_write_dword(a,d) { unsigned int _dw = d; flash_write(FLASH_BASE_ADDR + a, 4, (unsigned char *)&_dw); }
#define _flash_write(a,b,c) flash_write(FLASH_BASE_ADDR + a, b, (unsigned char *)c)
#define _flash_read(a,b,c) flash_read_page(FLASH_BASE_ADDR + a, b, (u8 *)c)

#if (DEV_SERVICES & SERVICE_PRESSURE)
#define measured_val0	measured_data.pressure
#elif USE_SENSOR_INA3221
#define measured_val0	measured_data.voltage[0]
#else
#define measured_val0	measured_data.battery_mv
#endif

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
#define measured_val1	measured_data.temp
#define measured_val2	measured_data.humi
#elif (DEV_SERVICES & SERVICE_18B20) && (USE_SENSOR_MY18B20 == 1)
#define measured_val1	measured_data.xtemp[0]
#define measured_val2	0
#elif (DEV_SERVICES & SERVICE_18B20) && (USE_SENSOR_MY18B20 == 2)
#define measured_val1	measured_data.xtemp[0]
#define measured_val2	measured_data.xtemp[1]
#elif (DEV_SERVICES & SERVICE_IUS)
#if USE_SENSOR_INA3221
#define measured_val1	measured_data.current[1]
#define measured_val2	measured_data.current[0]
#else
#define measured_val1	measured_data.current
#define measured_val2	measured_data.voltage
#endif
#endif

typedef struct _summ_data_t {
#if USE_SENSOR_INA3221
	u32	val0; //
	s32	val1; //
	s32	val2; //
#else
	u32	val0; // battery_mv; // mV
	s32	val1; // temp, temp1, ; // x 0.01 C
#if ((DEV_SERVICES & SERVICE_THS) == 0) && (DEV_SERVICES & SERVICE_18B20) && (USE_SENSOR_MY18B20 == 2)
	s32	val2; // temp2, current; // x 0.01 C, x ? mA
#else
	u32	val2; // humi, voltage; // x 0.01 %, x ? mV
#endif
#endif
	u32 	count;
} summ_data_t;
RAM summ_data_t summ_data;

RAM memo_inf_t memo;
RAM memo_rd_t rd_memo;

#if USE_MEMO_1M
#define MEMO_SECTORS		memo.sectors
#define MEMO_START_ADDR		memo.start_addr
#define MEMO_END_ADDR		memo.end_addr
#else
#define MEMO_SECTORS		MEMO_SEC_RECS
#define MEMO_START_ADDR		FLASH_ADDR_START_MEMO
#define MEMO_END_ADDR		FLASH_ADDR_END_MEMO
#endif

static u32 test_next_memo_sec_addr(u32 faddr) {
	u32 mfaddr = faddr;
	if (mfaddr >= MEMO_END_ADDR)
		mfaddr = MEMO_START_ADDR;
	else if (mfaddr < MEMO_START_ADDR)
		mfaddr = MEMO_END_ADDR - FLASH_SECTOR_SIZE;
	return mfaddr;
}

static void memo_sec_init(u32 faddr) {
	u32 mfaddr = faddr;
	mfaddr &= ~(FLASH_SECTOR_SIZE-1);
	_flash_erase_sector(mfaddr);
	_flash_write_dword(mfaddr, MEMO_SEC_ID);
	memo.faddr = mfaddr + sizeof(memo_head_t);
	memo.cnt_cur_sec = 0;
}

static void memo_sec_close(u32 faddr) {
	u32 mfaddr = faddr;
	u16 flg = 0;
	mfaddr &= ~(FLASH_SECTOR_SIZE-1);
	_flash_write(mfaddr + sizeof(memo_head_t) - sizeof(flg), sizeof(flg), &flg);
	memo_sec_init(test_next_memo_sec_addr(mfaddr + FLASH_SECTOR_SIZE));
}

#if 0
void memo_init_count(void) {
	memo_head_t mhs;
	u32 cnt, i = 0;
	u32 faddr = memo.faddr & (~(FLASH_SECTOR_SIZE-1));
	cnt = memo.faddr - faddr - sizeof(memo_head_t); // смещение в секторе
	cnt /= sizeof(memo_blk_t);
	do {
		faddr = test_next_memo_sec_addr(faddr - FLASH_SECTOR_SIZE);
		_flash_read(faddr, &mhs, sizeof(mhs));
		i++;
	} while (mhs.id == MEMO_SEC_ID && mhs.flg == 0 && i < MEMO_SEC_COUNT);
	cnt += i *MEMO_SEC_RECS;
	memo.count = cnt;
}
#endif

__attribute__((optimize("-Os")))
void memo_init(void) {
	memo_head_t mhs;
	u32 tmp, fsec_end;
	u32 faddr;
#if USE_MEMO_1M
	u8 buf[4];
	flash_read_id(buf);
	if(buf[2] == 0x14) {
		memo.start_addr = FLASH1M_ADDR_START_MEMO;
		memo.end_addr = FLASH1M_ADDR_END_MEMO;
		memo.sectors = MEMO1M_SEC_RECS;
	} else {
		memo.start_addr = FLASH_ADDR_START_MEMO;
		memo.end_addr = FLASH_ADDR_END_MEMO;
		memo.sectors = MEMO_SEC_RECS;
	}
#endif
	memo.cnt_cur_sec = 0;
	faddr = MEMO_START_ADDR;
	while (faddr < MEMO_END_ADDR) {
		_flash_read(faddr, sizeof(mhs), &mhs);
		if (mhs.id != MEMO_SEC_ID) {
			memo_sec_init(faddr);
			return;
		} else if (mhs.flg == 0xffff) {
			fsec_end = faddr + FLASH_SECTOR_SIZE;
			faddr += sizeof(memo_head_t);
			while (faddr < fsec_end) {
				_flash_read(faddr, sizeof(tmp), &tmp);
				if (tmp == 0xffffffff) {
					memo.faddr = faddr;
					return;
				}
				wrk.utc_time_sec = tmp + 5;
				memo.cnt_cur_sec++;
				faddr += sizeof(memo_blk_t);
			}
			memo_sec_close(fsec_end - FLASH_SECTOR_SIZE);
			return;
		}
		faddr += FLASH_SECTOR_SIZE;
	}
	memo_sec_init(MEMO_START_ADDR);
	return;
}

void clear_memo(void) {
	u32 tmp;
	u32 faddr = MEMO_START_ADDR + FLASH_SECTOR_SIZE;
	memo.cnt_cur_sec = 0;
	while (faddr < MEMO_END_ADDR) {
		_flash_read(faddr, sizeof(tmp), &tmp);
		if (tmp == MEMO_SEC_ID)
			_flash_erase_sector(faddr);
		faddr += FLASH_SECTOR_SIZE;
	}
	memo_sec_init(MEMO_START_ADDR);
	return;
}

//_attribute_ram_code_
__attribute__((optimize("-Os")))
unsigned get_memo(u32 bnum, pmemo_blk_t p) {
	memo_head_t mhs;
	u32 faddr;
	faddr = rd_memo.saved.faddr & (~(FLASH_SECTOR_SIZE-1));
	if (bnum > rd_memo.saved.cnt_cur_sec) {
		bnum -= rd_memo.saved.cnt_cur_sec;
		faddr -= FLASH_SECTOR_SIZE;
		if (faddr < MEMO_START_ADDR)
			faddr = MEMO_END_ADDR - FLASH_SECTOR_SIZE;
		while (bnum > MEMO_SECTORS) {
			bnum -= MEMO_SECTORS;
			faddr -= FLASH_SECTOR_SIZE;
			if (faddr < MEMO_START_ADDR)
				faddr = MEMO_END_ADDR - FLASH_SECTOR_SIZE;
		}
		bnum = MEMO_SECTORS - bnum;
		_flash_read(faddr, sizeof(mhs), &mhs);
		if (mhs.id != MEMO_SEC_ID || mhs.flg != 0)
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
__attribute__((optimize("-Os")))
void write_memo(void) {
	memo_blk_t mblk;
	if (cfg.averaging_measurements == 1) {
		mblk.val0 = measured_val0;
		mblk.val1 = measured_val1;
		mblk.val2 = measured_val2;
	} else {
		summ_data.val0 += measured_val0;
		summ_data.val1 += measured_val1;
		summ_data.val2 += measured_val2;
		summ_data.count++;
		if (cfg.averaging_measurements > summ_data.count)
			return;
		if(wrk.ble_connected && bls_pm_getSystemWakeupTick() - clock_time() < 125*CLOCK_16M_SYS_TIMER_CLK_1MS)
			return;
#if	USE_SENSOR_INA3221
		mblk.val0 = (s16)(summ_data.val0/summ_data.count);
		mblk.val1 = (s16)(summ_data.val1/summ_data.count);
		mblk.val2 = (s16)(summ_data.val2/summ_data.count);
#else
		mblk.val0 = (u16)(summ_data.val0/summ_data.count);
		mblk.val1 = (s16)(summ_data.val1/(s32)summ_data.count);
#if ((DEV_SERVICES & SERVICE_THS) == 0) && (DEV_SERVICES & SERVICE_18B20) && (USE_SENSOR_MY18B20 == 2)
		mblk.val2 = (s16)(summ_data.val2/summ_data.count);  // temperature
#else
		mblk.val2 = (u16)(summ_data.val2/summ_data.count); // humidity
#endif
#endif
		memset(&summ_data, 0, sizeof(summ_data));
	}
	/* default c4: dcdc 1.8V  -> GD flash; 48M clock may error, need higher DCDC voltage
	           c6: dcdc 1.9V
	analog_write(0x0c, 0xc6);
	*/
	if (wrk.utc_time_sec == 0xffffffff)
		mblk.time = 0xfffffffe;
	else
		mblk.time = wrk.utc_time_sec;
	u32 faddr = memo.faddr;
	if (!faddr) {
		memo_init();
		faddr = memo.faddr;
	}
	_flash_write(faddr, sizeof(memo_blk_t), &mblk);
	faddr += sizeof(memo_blk_t);
	faddr &= (~(FLASH_SECTOR_SIZE-1));
	if (memo.cnt_cur_sec >= MEMO_SECTORS - 1 ||
		(memo.faddr & (~(FLASH_SECTOR_SIZE-1))) != faddr) {
		memo_sec_close(memo.faddr);
	} else {
		memo.cnt_cur_sec++;
		memo.faddr += sizeof(memo_blk_t);
	}
}

#endif // #if (DEV_SERVICES & SERVICE_HISTORY)
