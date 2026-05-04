/*
 * flash_eep.c
 *
 *  EEP Version 2.0
 *
 *  Author: pvvx
 */
//#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#if 1 // USE_EEP
#include "flash_eep.h"
//#include "sws_printf.h"

#define FEEP_CODE_ATTR
#define FEEP_DATA_ATTR
#ifndef LOCAL
#define LOCAL static
#endif

#ifndef true
#define true (1)
#endif
#ifndef false
#define false (0)
#endif

#ifndef min
#define min(a, b)  ((a < b)? a : b)
#endif

#if SWS_PRINTF
#define eep_printf	sws_printf
#define eep1_printf(...)
//#define eep1_printf(...)
#else
#define eep_printf(...)
#define eep1_printf(...)
#endif

typedef union __attribute__((packed)) // заголовок объекта сохранения feep
{
	struct __attribute__((packed)) {
	u8 id;    // если id = 0 - резерв: объект удален, = 0xff - данные не записались
	u8 size;  // если size > MAX_FOBJ_SIZE - ошибка записи
	} n;
	u16 x; // если это obj b x = 0xffff - конец записей.
  	       // если это старт сектора и x = 0x00ff - текущий сектор.
} fobj_head_t;

typedef struct __attribute__((packed)) // заголовок объекта сохранения feep
{
	fobj_head_t h; // size <= MAX_FOBJ_SIZE, id = 0x01..0xfe
	u8 data[MAX_FOBJ_SIZE];
} feep_obj_t;

#define head_sector 0x00ff // head:id == 0xff && head:size == 0
#define fobj_x_free 0xffff

#define _flash_write(a,b,c) flash_write_page(FLASH_BASE_ADDR + (a),b,(unsigned char *)c)
#define _flash_read(a,b,c) flash_read_page(FLASH_BASE_ADDR + (a), b, (u8 *)c)
#define _flash_erase_sector(a) flash_erase_sector(FLASH_BASE_ADDR + (a))

/*----------------------------------------------------
 FunctionName: _flash_read_head
 -----------------------------------------------------*/
static inline void _flash_read_head(u32 addr, fobj_head_t *phead) {
	_flash_read(addr, sizeof(fobj_head_t), phead);
}

/*----------------------------------------------------
 FunctionName: _flash_write_obj
 -----------------------------------------------------*/
static inline void _flash_write_obj(u32 faddr, feep_obj_t * pobj)
{
	eep1_printf("EEP#wr_obj: %02x,%04x\n", faddr, pobj->h.x);
	// сначала записать размер и данные
	_flash_write(faddr + sizeof(pobj->h.n.id),
			sizeof(pobj->h.n.size) + pobj->h.n.size,
			&pobj->h.n.size);
	// далее записать id
	_flash_write(faddr, sizeof(pobj->h.n.id), &pobj->h.n.id);
}

/*-----------------------------------
 FunctionName : get_addr_bscfg
 Поиск текушего сектора в банке
 Return : адрес сектора
 ------------------------------------*/
FEEP_CODE_ATTR
#if USE_EEP_BANKS
LOCAL u32 get_addr_bscfg(u8 nv) {
	u32 faddr = (nv)? FMEMORY_EEP_BASE_ADDR2 : FMEMORY_EEP_BASE_ADDR1;
#else
LOCAL u32 get_addr_bscfg(void) {
	u32 faddr = FMEMORY_EEP_BASE_ADDR1;
#endif
	fobj_head_t rh;
	u32 fend = faddr + FMEMORY_EEP_BANKS_SIZE;
	u32 reta = 0;
	do {
		_flash_read_head(faddr, &rh);
		// поиск текущего сектора
		if (rh.x == head_sector) { // head:id == 0xff && head:size == 0
			reta = faddr; // текущий сектор записей
		}
		faddr += FLASH_SECTOR_SIZE;
	} while (faddr < fend);
	if(!reta) {
		// очистить сектор
#if USE_EEP_BANKS
		reta = (nv)? FMEMORY_EEP_BASE_ADDR2 : FMEMORY_EEP_BASE_ADDR1;
#else
		reta = FMEMORY_EEP_BASE_ADDR1;
#endif
		eep1_printf("bseep_ead: %06x\n", reta);
		_flash_erase_sector(reta);
		// отметить новый сектор как текущий (obj:size = 0)
		rh.n.size = 0;
		_flash_write(reta + sizeof(rh.n.id), sizeof(rh.n.size), &rh.n.size);
	}
	return reta;
}
/*----------------------------------------------------
 FunctionName : get_addr_wrobj
 Поиск в секторе адреса для записи
 Return: адрес объекта
  = 0 - не найден - на pack
 -----------------------------------------------------*/
FEEP_CODE_ATTR
LOCAL u32 get_addr_wrobj(u32 faddr)
{
	fobj_head_t rh;
	u32 fend = (faddr + FLASH_SECTOR_SIZE) & (~(FLASH_SECTOR_SIZE-1));
	u32 reta = 0;
	do {
		_flash_read_head(faddr, &rh);
		eep1_printf("wrobj_rdh: %06x,%04x\n", faddr, rh.x);
		if (rh.x == fobj_x_free) {
			// пустой объект, конец записей
			reta = faddr;
			break;
		}
		if (rh.n.size <= MAX_FOBJ_SIZE) {
			faddr += rh.n.size + sizeof(rh);
		} else { // объект не прописался (сбой во время записи объекта и размера)
			faddr += MAX_FOBJ_SIZE + sizeof(rh);
		}
	} while (faddr < fend);
	return reta;
}
/*----------------------------------------------------
 FunctionName : get_addr_idobj
 Return : адрес объекта
  0 - не найден
 -----------------------------------------------------*/
FEEP_CODE_ATTR
LOCAL u32 get_addr_idobj(u32 faddr, fobj_head_t * pobj)
{
	fobj_head_t rh;
	u32 fend = (faddr + FLASH_SECTOR_SIZE) & (~(FLASH_SECTOR_SIZE-1));
	u32 reta = 0;
	do {
		_flash_read_head(faddr, &rh);
		eep1_printf("idobj_rdh: %06x,%04x\n", faddr, rh.x);
		if (rh.x == fobj_x_free)
			break;
		if (rh.n.size <= MAX_FOBJ_SIZE) {
			if (rh.n.id == pobj->n.id) {
				pobj->n.size = rh.n.size;
				reta = faddr;
			}
			faddr += rh.n.size + sizeof(rh);
		} else { // объект не прописался (сбой во время записи объекта и размера)
			faddr += MAX_FOBJ_SIZE + sizeof(rh);
		}
	}
	while (faddr < fend);
	return reta;
}

#define set_flag_eep_id(b, n)	b[(n)>>3] |= 1 << ((n) & 7)
#define check_flag_eep_id(b, n)	b[(n)>>3] &= 1 << ((n) & 7)

/*=============================================================================
   FunctionName : pack_eep_fmem

   return: адрес для записи в новом секторе
  ---------------------------------------------------------------------------*/
FEEP_CODE_ATTR
#if USE_EEP_BANKS
LOCAL u32 pack_eep_fmem(unsigned int bank, u32 sec_faddr) {
	eep_printf("EEP#pack: %d\n", bank);
#else
LOCAL u32 pack_eep_fmem(u32 sec_faddr) {
	eep_printf("EEP#pack\n");
#endif
	u8 buf_id[256>>3];
	feep_obj_t fobj;
	fobj_head_t rh;
	u32 fnewseg, faddr, rdaddr, wraddr, endrdaddr;
	memset((u8 *)buf_id, 0, sizeof(buf_id));
	// вычислить следующий сектор банка и конец текущего сектора
	fnewseg = sec_faddr + FLASH_SECTOR_SIZE;
	endrdaddr = fnewseg;
#if USE_EEP_BANKS
	faddr = (bank)? FMEMORY_EEP_BASE_ADDR2 : FMEMORY_EEP_BASE_ADDR1;
	if (fnewseg >= faddr + FMEMORY_EEP_BANKS_SIZE)
		fnewseg = faddr;
#else
	if (fnewseg >= FMEMORY_EEP_BASE_ADDR1 + FMEMORY_EEP_BANKS_SIZE)
		fnewseg = FMEMORY_EEP_BASE_ADDR1;
#endif
	// очистить новый сектор
	eep1_printf("pkeep_ers: %06x\n", fnewseg);
	_flash_erase_sector(fnewseg);
	// перепаковать объекты в новый сектор
	rdaddr = sec_faddr + sizeof(rh);
	wraddr = fnewseg + sizeof(rh);
	while(rdaddr < endrdaddr) {
		_flash_read_head(rdaddr, &rh);
		eep1_printf("pkeep_rdo: %06x,%04x\n", rdaddr, rh.x);
		// далее нет записей?
		if(rh.x == fobj_x_free)
			break;
		// пустая запись или id не прописался?
		if (rh.n.size == 0 || rh.n.id == 0xff) {
			// шаг на следующий адрес в старом секторе
			rdaddr += sizeof(rh) + rh.n.size;
			continue;
		}
		// объект не прописался (сбой во время записи объекта и размера)?
		if(rh.n.size > MAX_FOBJ_SIZE) {
			rdaddr += sizeof(rh) + MAX_FOBJ_SIZE;
			continue;
		}
		// объект уже записан в новый сектор ?
		if(check_flag_eep_id(buf_id, rh.n.id)) {
			// уже записан в новый сектор
			// шаг на следующий адрес в старом секторе
			rdaddr += sizeof(rh) + rh.n.size;
			continue;
		}
/*
		// объект уже записан в новый сектор ?
		if(get_addr_idobj(fnewseg + sizeof(rh), &rh)) {
			// уже записан в новый сектор
			// шаг на следующий адрес в старом секторе
			rdaddr += sizeof(rh) + rh.n.size;
			continue;
		}
*/
		// ещё не записан в новый сектор
		// найти последнюю запись с этим id в старом секторе
		fobj.h.n.id = rh.n.id;
		if((faddr = get_addr_idobj(rdaddr, &fobj.h)) != 0) {
			// есть такой id
			set_flag_eep_id(buf_id, rh.n.id); // выставить флаг обработки
			if(fobj.h.n.size > MAX_FOBJ_SIZE) {
				// объект не прописался (сбой во время записи данных и размера)
				rdaddr += sizeof(rh) + MAX_FOBJ_SIZE;
				continue;
			}
			// объект не удален (size != 0)?
			if(fobj.h.n.size)  {
				// переписать в новый сектор
				_flash_read(faddr + sizeof(rh), fobj.h.n.size, fobj.data);
				_flash_write_obj(wraddr, &fobj);
				// шаг на следующий адрес в новом секторе
				wraddr += fobj.h.n.size + sizeof(rh);
				if(wraddr >= fnewseg + FLASH_SECTOR_SIZE)
					break;
			}
		}
		// шаг на следующий адрес в старом секторе
		rdaddr += sizeof(rh) + rh.n.size;
	}
	// отметить новый сектор как текущий (obj:size = 0)
	rh.n.size = 0;
	_flash_write(fnewseg + sizeof(rh.n.id), sizeof(rh.n.size), &rh.n.size);
	// закрыть старый сектор (obj:id = 0)
	rh.n.id = 0;
	_flash_write(sec_faddr, sizeof(rh.n.id), &rh.n.id);
	if(wraddr >= fnewseg + FLASH_SECTOR_SIZE)
		wraddr = 0;
	eep1_printf("pkeep_end: %06x\n", wraddr);
	return wraddr;
}
//=============================================================================
//- Сохранить объект в flash --------------------------------------------------
//  Returns	: false/true
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
#if USE_EEP_BANKS
s32 flash_write_cfg(void *ptr, unsigned int bank, unsigned int id, size_t size) {
	eep_printf("EEP#wr_obj[%d]: %d,%02x\n", size, bank, id);
#else
s32 flash_write_cfg(void *ptr, unsigned int id, size_t size) {
	eep_printf("EEP#wr_obj[%d]: %02x\n", size, id);
#endif
	u32 faddr, saddr;
	feep_obj_t fobj;
	if (size > MAX_FOBJ_SIZE) {
		return FMEM_SIZE_ERR;
	}
	fobj.h.n.id = id;
	// получить адрес текущего сектора
#if USE_EEP_BANKS
	saddr = get_addr_bscfg(bank);
#else
	saddr = get_addr_bscfg();
#endif
	eep1_printf("wreep_sec: %06x\n", saddr);
	// проверить на повторную запись
	// получить адрес объекта, если он уже был записан
	faddr = get_addr_idobj(saddr + sizeof(fobj.h), &fobj.h);
	eep1_printf("wreep_aid: %06x,%04x\n", faddr, fobj.h.x);
	// объект есть и размер данных сопадает?
	if(faddr && fobj.h.n.size == size) {
		// объект не удален (size != 0)
		if(size) {
			// прочитать ранее записанные данные
			_flash_read(faddr + sizeof(fobj.h), fobj.h.n.size, fobj.data);
			eep1_printf("wreep_rad: %06x,%04x\n", faddr, fobj.h.x);
			// сравнить данные объекта
			if(!memcmp(ptr, &fobj.data, size)) {
				eep_printf("EEP#wr_eq\n");
				return size; // данные идентичны - объект уже записан
			}
		} else {
			eep_printf("EEP#wr_eq\n");
			return size; // данные идентичны - объект уже записан и размер 0
		}
	}
	// найти адрес для записи
	faddr = get_addr_wrobj(saddr + sizeof(fobj.h));
	eep1_printf("wreep_awr: %06x\n", faddr);
	// сектор заполнен?
	if(!faddr || faddr + size + sizeof(fobj.h) >= saddr + FLASH_SECTOR_SIZE) {
		// перепаковать в новый сектор и получить адрес для записи
#if USE_EEP_BANKS
		faddr = pack_eep_fmem(bank, saddr);
#else
		faddr = pack_eep_fmem(saddr);
#endif
		// адрес нового сектора
		saddr = faddr & (~(FLASH_SECTOR_SIZE - 1));
		eep1_printf("wreep_new: %06x\n", saddr);
	}
	if(!faddr || faddr + size + sizeof(fobj.h) >= saddr + FLASH_SECTOR_SIZE) {
		eep_printf("EEP#wr_overflow\n");
		return FMEM_OVERFLOW; // не влезет: переполнение банка
	}
	// записать объект
	if(size) {
		memcpy(fobj.data, ptr, size);
	}
	fobj.h.n.size = size;
	_flash_write_obj(faddr, &fobj);
	eep_printf("EEP#wr_ok\n");
	return size;
}
//=============================================================================
void flash_erase_all_cfg(void) {
	u32 faddr = FMEMORY_EEP_BASE_ADDR1;
	u32 fend = faddr + FMEMORY_EEP_BANKS_SIZE;
	eep_printf("EEP#erase_all\n");
	while(faddr < fend) {
		_flash_erase_sector(faddr);
		faddr += FLASH_SECTOR_SIZE;
	}
#if USE_EEP_BANKS
	faddr = FMEMORY_EEP_BASE_ADDR2;
	fend = faddr + FMEMORY_EEP_BANKS_SIZE;
	while(faddr < fend) {
		_flash_erase_sector(faddr);
		faddr += FLASH_SECTOR_SIZE;
	}
#endif
}
//=============================================================================
//- Прочитать объект из flash -------------------------------------------------
//  Параметры:
//   prt - указатель, куда сохранить
//   id - идентификатор искомого объекта
//   maxsize - сколько байт сохранить максимум из найденного объекта, по ptr
//  Returns:
//  -3 - переполнение банка
//  -2 - задан неверный размер
//  -1 - не найден
//   0..MAX_FOBJ_SIZE - ok, сохраненный размер объекта
//-----------------------------------------------------------------------------
FEEP_CODE_ATTR
#if USE_EEP_BANKS
s32 flash_read_cfg(void *ptr, unsigned int bank, unsigned int id, size_t maxsize) {
	eep_printf("EEP#rd_obj[%d]: %d,%02x\n", maxsize, bank, id);
#else
s32 flash_read_cfg(void *ptr, unsigned int id, size_t maxsize) {
	eep_printf("EEP#rd_obj[%d]: %02x\n", maxsize, id);
#endif
	u32 faddr, saddr;
	fobj_head_t rh;
	if (maxsize <= MAX_FOBJ_SIZE) {
		rh.n.id = id;
#if USE_EEP_BANKS
		saddr = get_addr_bscfg(bank);
#else
		saddr = get_addr_bscfg();
#endif
		eep1_printf("rdeep_sec: %06x\n", saddr);
		faddr = get_addr_idobj(saddr + sizeof(rh), &rh);
		eep1_printf("rdeep_aid: %06x\n", faddr);
		if(faddr) {
			_flash_read(faddr + sizeof(rh), min(rh.n.size, maxsize), ptr);
			eep1_printf("rdeep_len: %d\n", rh.n.size);
			return (s32)rh.n.size;
		}
	    return FMEM_NOT_FOUND;
	}
    return FMEM_SIZE_ERR;
}
//=============================================================================
FEEP_CODE_ATTR
/* ver = (SW version << 16) | (HW ) version
 * return: true = first start */
bool flash_supported_eep_ver(u32 min_ver, u32 new_ver) {
	bool ret = false;
	u32 tmp;
#if USE_EEP_BANKS
	if (flash_read_cfg(&tmp, 0, EEP_ID_VER, sizeof(tmp))
#else
	if (flash_read_cfg(&tmp, EEP_ID_VER, sizeof(tmp))
#endif
		== sizeof(tmp)
		&& (tmp & 0xffff) == (new_ver & 0xffff)
		&& tmp >= min_ver) {
		eep_printf("EEP#init: next start\n");
	} else {
		flash_erase_all_cfg();
		ret = true;
	}
	eep_printf("EEP#init: %08x:%08x\n", tmp, new_ver);
	tmp = new_ver;
#if USE_EEP_BANKS
	flash_write_cfg(&tmp, 0, EEP_ID_VER, sizeof(tmp));
#else
	flash_write_cfg(&tmp, EEP_ID_VER, sizeof(tmp));
#endif
	return ret;
}

#endif // USE_EEP
