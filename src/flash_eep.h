/******************************************************************************
 * FileName: flash_eep.h
 * EEP Version 2.0
 * Author: pvvx
*******************************************************************************/
#ifndef __FLASH_EEP_H_
#define __FLASH_EEP_H_

#ifdef __cplusplus
extern "C" {
#endif

//--EEPROM IDs-----------------------------------------------------------------
typedef enum {
	EEP_ID_VER = 0,// EEP ID blk: unsigned int = minimum supported version
	EEP_ID_CFG,	// EEP ID config data
	EEP_ID_CFS,	// EEP ID sensor TH coefficients
	EEP_ID_CRH,	// EEP ID sensor RH coefficients
	EEP_ID_CMY,	// EEP ID sensor MY18B20 coefficients
	EEP_ID_TRG,	// EEP ID trigger data
	EEP_ID_RPC,	// EEP ID reed switch pulse counter
	EEP_ID_HXC,	// EEP ID hx71x config data
	EEP_ID_SCN,	// EEP ID scan config data
	EEP_ID_DAC,	// EEP ID DAC config
	EEP_ID_PCD,	// EEP ID pincode
	EEP_ID_CMF,	// EEP ID comfort data
	EEP_ID_DVN,	// EEP ID device name
	EEP_ID_TIM,	// EEP ID time adjust
	EEP_ID_KEY,	// EEP ID bkey
	EEP_ID_HWV, // EEP ID Mi HW version ("B1.4","B1.5",...)
	EEP_ID_MTN	// EEP ID motion sensor
} EEP_ID_e;
//--Option---------------------------------------------------------------------
#define USE_EEP_BANKS	0  // = 1 используется 2 банка, = 0 без банков
#define MAX_FOBJ_SIZE 	64 // максимальный размер сохраняемых объeктов (32..512)
//--Config---------------------------------------------------------------------
#define FLASH_BASE_ADDR			0x00000000
#define FLASH_SIZE				(512*1024)
#define FLASH_SECTOR_SIZE		4096
#define FMEMORY_EEP_BANKS_SHL 	2 // 1<<FMEMORY_EEP_BANKS_SHL = кол-во секторов для работы 2,4,8,..
#define FMEMORY_EEP_BANKS_SIZE	(FLASH_SECTOR_SIZE << FMEMORY_EEP_BANKS_SHL) // размер FMEMORY
#define FMEMORY_EEP_BASE_ADDR1	(FLASH_SIZE - (FMEMORY_EEP_BANKS_SIZE)) // 0x7C000
#if USE_EEP_BANKS
#define FMEMORY_EEP_BASE_ADDR2	(FMEMORY_EEP_BASE_ADDR1 + 0x40000) // 0x72000
#endif
//-----------------------------------------------------------------------------
typedef enum {
	FMEM_NOT_FOUND = -1,	//  -1 - не найден
	FMEM_SIZE_ERR  = -2,	//  -2 - задан неверный размер
	FMEM_OVERFLOW  = -3		//  -3 - переполнение банка
} fmemory_errors_t;
//-----------------------------------------------------------------------------
#if USE_EEP_BANKS
s32 flash_read_cfg(void *ptr, unsigned int bank, unsigned int id, size_t maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
s32 flash_write_cfg(void *ptr, unsigned int bank, unsigned int id, size_t size);
#else
s32 flash_read_cfg(void *ptr, unsigned int id, size_t maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
s32 flash_write_cfg(void *ptr, unsigned int id, size_t size);
#endif
bool flash_supported_eep_ver(u32 min_ver, u32 new_ver);
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif /* __FLASH_EEP_H_ */
