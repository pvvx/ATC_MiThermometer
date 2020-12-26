/*
 * flash_eep.c
 *
 *  Created on: 19/01/2015
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "flash_eep.h"

//-----------------------------------------------------------------------------
#define FEEP_ERR_PREFIX         "[FEEP Err]"
#define FEEP_WARN_PREFIX        "[FEEP Wrn]"
#define FEEP_INFO_PREFIX        "[FEEP Inf]"

#ifdef CONFIG_DEBUG_ERR_MSG
#define DBG_FEEP_ERR(...)     do {\
	if (likely(ConfigDebugErr & _DBG_FEEP_)) \
	DiagPrintf(FEEP_ERR_PREFIX __VA_ARGS__); \
}while(0)
#else
#define DBG_FEEP_ERR(...)
#endif

#ifdef CONFIG_DEBUG_WARN_MSG
#define DBG_FEEP_WARN(...)     do {\
	if (likely(ConfigDebugWarn & _DBG_FEEP_)) \
	DiagPrintf(FEEP_WARN_PREFIX __VA_ARGS__); \
}while(0)
#else
#define DBG_FEEP_WARN(...)
#endif

#ifdef CONFIG_DEBUG_INFO_MSG
#define DBG_FEEP_INFO(...)     do {\
	if (unlikely(ConfigDebugInfo & _DBG_FEEP_)) \
	DiagPrintf(FEEP_INFO_PREFIX __VA_ARGS__); \
}while(0)
#else
#define DBG_FEEP_INFO(...)
#endif

//extern void *pvPortMalloc( size_t xWantedSize );
//extern void vPortFree( void *pv );
#define malloc(a)	buf_epp // pvPortMalloc
#define free(a)	// vPortFree

#define _flash_mutex_lock()
#define _flash_mutex_unlock()
#define _flash_read_dword(a) (*(volatile u32*)(FLASH_BASE_ADDR + (a)))
#define _flash_read(a,b,c) memcpy((void *)c, (void *)(FLASH_BASE_ADDR + (unsigned int)a), b) // _flash_read(rdaddr, len, pbuf);
#define _flash_memcmp(a,b,c) memcmp((void *)(FLASH_BASE_ADDR + (unsigned int)a), c, b) // _flash_memcmp(xfaddr + fobj_head_size, size, ptr) == 0)
#define _flash_erase_sector(a) flash_erase_sector(a)
#define _flash_write_dword(a,d) { unsigned int _dw = d; flash_write_page(a, 4, (unsigned char *)&_dw); }
#define _flash_write(a,b,c) flash_write_(a,b,(unsigned char *)c) //_flash_write(wraddr, len, pbuf);

#ifndef LOCAL
#define LOCAL static
#endif
#ifndef true
#define true (1)
#endif
#ifndef false
#define false (0)
#endif

#ifndef mMIN
#define mMIN(a, b)  ((a < b)? a : b)
#endif
#define align(a) ((a + 3) & 0xFFFFFFFC)

#define FEEP_CODE_ATTR
#define FEEP_DATA_ATTR

typedef union __attribute__((packed)) // заголовок объекта сохранения feep
{
	struct {
	unsigned short size;
	unsigned short id;
	} __attribute__((packed)) n;
	unsigned int x;
} fobj_head;

#define fobj_head_size 4
#define fobj_x_free 0xffffffff
#define FMEM_ERROR_MAX 5

unsigned char buf_epp[MAX_FOBJ_SIZE+fobj_head_size];

_attribute_ram_code_ void flash_write_(unsigned int addr, unsigned int len, unsigned char *buf) {
	u32 sz = 256;
	while(len) {
		if (len < sz) sz = len;
		flash_write_page(addr, sz, buf);
		addr += sz;
		buf += sz;
		len -= sz;
	}
}
//-----------------------------------------------------------------------------
// FunctionName : get_addr_bscfg
// поиск текушего сегмента
// Return     : адрес сегмента
// ret < FMEM_ERROR_MAX - ошибка
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
LOCAL unsigned int get_addr_bscfg()
{
	unsigned int x1 = 0xFFFFFFFF, x2;
	unsigned int faddr = FMEMORY_SCFG_BASE_ADDR;
	unsigned int reta = FMEMORY_SCFG_BASE_ADDR;
	do {
		x2 = _flash_read_dword(faddr); // if(flash_read(faddr, &x2, 4)) return -(FMEM_FLASH_ERR);
		if(x2 < x1) { // поиск текущего сегмента
			x1 = x2;
			reta = faddr; // новый адрес сегмента для записи
		};
		faddr += FMEMORY_SCFG_BANK_SIZE;
	} while(faddr < (FMEMORY_SCFG_BASE_ADDR + FMEMORY_SCFG_BANKS * FMEMORY_SCFG_BANK_SIZE));

	if((x1 == 0xFFFFFFFF)&&(reta == FMEMORY_SCFG_BASE_ADDR)) { // первый старт?
		_flash_write_dword(reta, 0x7FFFFFFF); // if(flash_write(reta, &x1, 4)) return -(FMEM_FLASH_ERR);
	}
#if CONFIG_DEBUG_LOG > 3
	DBG_FEEP_INFO("base seg: %p [%p]\n", reta, _flash_read_dword(reta));
#endif
	return reta;
}
//-----------------------------------------------------------------------------
// FunctionName : get_addr_fobj
// Опции:
//  false - Поиск последней записи объекта по id и size
//  true - Поиск присуствия записи объекта по id и size
// Returns : адрес записи данных объекта
// 0 - не найден
// ret < FMEM_ERROR_MAX - ошибка
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
LOCAL unsigned int get_addr_fobj(unsigned int base, fobj_head *obj, bool flg)
{
//	if(base == 0) return 0;
	fobj_head fobj;
	unsigned int faddr = base + 4;
	unsigned int fend = base + FMEMORY_SCFG_BANK_SIZE - align(fobj_head_size);
	unsigned int reta = 0;
	do {
		fobj.x = _flash_read_dword(faddr); // if(flash_read(faddr, &fobj, fobj_head_size)) return -(FMEM_FLASH_ERR);
		if(fobj.x == fobj_x_free) break;
		if(fobj.n.size <= MAX_FOBJ_SIZE) {
			if(fobj.n.id == obj->n.id) {
				if(flg) {
					return faddr;
				}
				obj->n.size = fobj.n.size;
				reta = faddr;
			}
			faddr += align(fobj.n.size + fobj_head_size);
		}
		else faddr += align(MAX_FOBJ_SIZE + fobj_head_size);
	}
	while(faddr < fend);
	return reta;
}
//-----------------------------------------------------------------------------
// FunctionName : get_addr_fend
// Поиск последнего адреса в сегменте для записи объекта
// Returns : адрес для записи объекта
// ret < FMEM_ERROR_MAX - ошибка
// ret = 0 - не влезет, на pack
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
LOCAL unsigned int get_addr_fobj_save(unsigned int base, fobj_head obj)
{
	fobj_head fobj;
	unsigned int faddr = base + 4;
	unsigned int fend = base + FMEMORY_SCFG_BANK_SIZE - align(obj.n.size + fobj_head_size);
	do {
		fobj.x = _flash_read_dword(faddr); // if(flash_read(faddr, &fobj, fobj_head_size)) return -(FMEM_FLASH_ERR);
		if(fobj.x == fobj_x_free) {
			if(faddr < fend) {
				return faddr;
			}
			return 0; // не влезет, на pack
		}
		if(fobj.n.size <= MAX_FOBJ_SIZE) {
			faddr += align(fobj.n.size + fobj_head_size);
		}
		else faddr += align(MAX_FOBJ_SIZE + fobj_head_size);
	}
	while(faddr < fend);
	return 0; // не влезет, на pack
}
//=============================================================================
// FunctionName : pack_cfg_fmem
// Returns      : адрес для записи объекта
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
LOCAL unsigned int pack_cfg_fmem(fobj_head obj)
{
	fobj_head fobj;
	unsigned int foldseg = get_addr_bscfg(); // поиск текушего сегмента
//	if(foldseg < FMEM_ERROR_MAX) return fnewseg; // error
	unsigned int fnewseg = foldseg + FMEMORY_SCFG_BANK_SIZE;
	if(fnewseg >= (FMEMORY_SCFG_BASE_ADDR + FMEMORY_SCFG_BANKS * FMEMORY_SCFG_BANK_SIZE))
		fnewseg = FMEMORY_SCFG_BASE_ADDR;
	unsigned int faddr = foldseg;
	unsigned int rdaddr, wraddr;
	unsigned short len;
	unsigned int * pbuf = (unsigned int *) malloc(align(MAX_FOBJ_SIZE + fobj_head_size) >> 2);
	if(pbuf == NULL) {
		DBG_FEEP_ERR("pack malloc error!\n");
		return -(FMEM_MEM_ERR);
	}
#if CONFIG_DEBUG_LOG > 3
	DBG_FEEP_INFO("repack base to new seg: %p\n", fnewseg);
#endif
	if(_flash_read_dword(fnewseg) != 0xFFFFFFFF)
		_flash_erase_sector(fnewseg); // if(flash_erase_sector(fnewseg)) return -(FMEM_FLASH_ERR);
	_flash_write_dword(fnewseg, 0x7FFFFFFF); // сегмент занят
	faddr += 4;
	wraddr = fnewseg + 4;
	do {
		fobj.x = _flash_read_dword(faddr); //if(flash_read(faddr, &fobj, fobj_head_size)) return -(FMEM_FLASH_ERR); // последовательное чтение id из старого сегмента
		if(fobj.x == fobj_x_free) break;
		if(fobj.n.size > MAX_FOBJ_SIZE) len = align(MAX_FOBJ_SIZE + fobj_head_size);
		else len = align(fobj.n.size + fobj_head_size);
		if(fobj.n.id != obj.n.id &&  fobj.n.size <= MAX_FOBJ_SIZE) { // объект валидный
			if(get_addr_fobj(fnewseg, &fobj, true) == 0) { // найдем, сохранили ли мы его уже? нет
				rdaddr = get_addr_fobj(foldseg, &fobj, false); // найдем последнее сохранение объекта в старом сенгменте, size изменен
				if(rdaddr < FMEM_ERROR_MAX) return rdaddr; // ???
				if(wraddr + len >= fnewseg + FMEMORY_SCFG_BANK_SIZE) {
					DBG_FEEP_ERR("pack segment overflow!\n");
					return -(FMEM_OVR_ERR);
				};
				_flash_read(rdaddr, len, pbuf);
				// перепишем данные obj в новый сектор
				_flash_write(wraddr, len, pbuf);
			};
		};
		faddr += len;
	} while(faddr  < (foldseg + FMEMORY_SCFG_BANK_SIZE - align(fobj_head_size+1)));
	free(pbuf);
	// обратный счетчик стираний/записей секторов как id
	_flash_write_dword(fnewseg, (_flash_read_dword(foldseg) - 1)); // if(flash_write(fnewseg, &foldseg + SPI_FLASH_BASE, 4)) return -(FMEM_FLASH_ERR);
	_flash_erase_sector(foldseg);
#if CONFIG_DEBUG_LOG > 3
	DBG_FEEP_INFO("free: %d\n", FMEMORY_SCFG_BANK_SIZE - (faddr & (FMEMORY_SCFG_BANK_SIZE-1)));
#endif
	return get_addr_fobj_save(fnewseg, obj); // адрес для записи объекта;
}
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
LOCAL signed short _flash_write_cfg(void *ptr, unsigned short id, unsigned short size)
{
	fobj_head fobj;
	fobj.n.id = id;
	fobj.n.size = size;
//	bool retb = false;
	unsigned int faddr = get_addr_bscfg();

	if(faddr >= FMEM_ERROR_MAX)  {
		unsigned int xfaddr = get_addr_fobj(faddr, &fobj, false);
		if(xfaddr > FMEM_ERROR_MAX && size == fobj.n.size) {
			if(size == 0
					|| _flash_memcmp(xfaddr + fobj_head_size, size, ptr) == 0) {
#if CONFIG_DEBUG_LOG > 3
					DBG_FEEP_INFO("write obj is identical, id: %04x [%d]\n", id, size);
#endif
					return size; // уже записано то-же самое
			}
#if CONFIG_DEBUG_LOG > 100
			else {
				int i;
				uint8_t * p = (uint8_t *)(SPI_FLASH_BASE + xfaddr + fobj_head_size);
				uint8_t * r = (uint8_t *) ptr;
				for(i=0; i < size; i+=8) {
					DBG_8195A("buf[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
								i, r[i], r[i+1], r[i+2], r[i+3], r[i+4], r[i+5], r[i+6], r[i+7]);
					DBG_8195A("obj[%d]\t%02X %02X %02X %02X  %02X %02X %02X %02X\n",
								i, p[i], p[i+1], p[i+2], p[i+3], p[i+4], p[i+5], p[i+6], p[i+7]);
				}
			}
#endif
		}
	}
	DBG_FEEP_INFO("write obj id: %04x [%d]\n", id, size);
	fobj.n.size = size;
//	flash_write_protect(&flashobj, 0); // Flash Unprotect
	faddr = get_addr_fobj_save(faddr, fobj);
	if(faddr == 0) {
		faddr = pack_cfg_fmem(fobj);
		if(faddr == 0) {
			DBG_FEEP_ERR("banks overflow!\n");
			return FMEM_NOT_FOUND;
		}
	}
	else if(faddr < FMEM_ERROR_MAX) return - faddr - 1; // error

#if CONFIG_DEBUG_LOG > 3
	DBG_FEEP_INFO("write obj to faddr %p\n", faddr);
#endif
	_flash_write_dword(faddr, fobj.x); // if(flash_write(faddr, &fobj.x, 4)) return FMEM_FLASH_ERR;
	faddr+=4;
#if 1
	u32 len = (size + 3) & (~3);
	if(len) _flash_write(faddr, len, ptr);
#else
	union {
		unsigned char uc[4];
		unsigned int ud;
	}tmp;
	u32 len = (size + 3) >> 2;
	unsigned char * ps = ptr;
  	while(len--) {
		tmp.uc[0] = *ps++;
		tmp.uc[1] = *ps++;
		tmp.uc[2] = *ps++;
		tmp.uc[3] = *ps++;
		_flash_write_dword(faddr, tmp.ud); // if(flash_write(faddr, &tmp.ud, 4)) return FMEM_FLASH_ERR;
		faddr += 4;
	}
#endif
	return size;
}
//=============================================================================
//- Сохранить объект в flash --------------------------------------------------
//  Returns	: false/true
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
bool flash_write_cfg(void *ptr, unsigned short id, unsigned short size)
{
	if(size > MAX_FOBJ_SIZE) return false;
	bool retb = false;
	_flash_mutex_lock();
	if(_flash_write_cfg(ptr, id, size) >= 0) {
#if CONFIG_DEBUG_LOG > 3
		DBG_FEEP_INFO("saved ok\n");
#endif
		retb = true;
	}
	_flash_mutex_unlock();
	return retb;
}
//=============================================================================
//- Прочитать объект из flash -------------------------------------------------
//  Параметры:
//   prt - указатель, куда сохранить
//   id - идентификатор искомого объекта
//   maxsize - сколько байт сохранить максимум из найденного объекта, по ptr
//  Returns:
//  -3 - error
//  -2 - flash rd/wr/clr error
//  -1 - не найден
//   0..MAX_FOBJ_SIZE - ok, сохраненный размер объекта
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
signed short flash_read_cfg(void *ptr, unsigned short id, unsigned short maxsize)
{
    signed short rets = FMEM_ERROR;
	if (maxsize <= MAX_FOBJ_SIZE) {
		_flash_mutex_lock();
		fobj_head fobj;
		fobj.n.id = id;
		fobj.n.size = 0;
		DBG_FEEP_INFO("read obj id: %04x[%d]\n", id, maxsize);
		unsigned int faddr = get_addr_bscfg();
		if(faddr >= FMEM_ERROR_MAX) {
			faddr = get_addr_fobj(faddr, &fobj, false);
			if(faddr >= FMEM_ERROR_MAX) {
				if(maxsize != 0 && ptr != NULL)
					_flash_read(faddr + fobj_head_size, mMIN(fobj.n.size, maxsize), ptr);

#if CONFIG_DEBUG_LOG > 3
				DBG_FEEP_INFO("read ok, faddr: %p, size: %d\n", faddr,  fobj.n.size);
#endif
				rets = fobj.n.size;
			}
			else {
#if CONFIG_DEBUG_LOG > 3
				DBG_FEEP_INFO("obj not found\n");
#endif
				rets = -faddr-1;
			}
		}
		else rets = -faddr-1;
		_flash_mutex_unlock();
	}
    return rets;
}
//=============================================================================

