/******************************************************************************
 * FileName: flash_eep.h
 * Description: FLASH
 * Alternate SDK 
 * Author: PV`
 * (c) PV` 2015
*******************************************************************************/
#ifndef __FLASH_EEP_H_
#define __FLASH_EEP_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <basic_types.h>
//#include <FreeRTOS.h>
//#include <queue.h>

#define EEP_ID_VER (0x5555) // EEP ID blk: unsigned int = minimum supported version
//-----------------------------------------------------------------------------
#define FLASH_BASE_ADDR			0x00000000
#define FLASH_SIZE				(512*1024)
#define FLASH_SECTOR_SIZE		4096
#define FMEMORY_SCFG_BANK_SIZE	FLASH_SECTOR_SIZE // размер сектора, 4096 bytes
#define FMEMORY_SCFG_BANKS 		4 // кол-во секторов для работы 2..
#define FMEMORY_SCFG_BASE_ADDR	(FLASH_SIZE - (FMEMORY_SCFG_BANKS*FMEMORY_SCFG_BANK_SIZE)) // 0x7C000
//-----------------------------------------------------------------------------
enum eFMEMORY_ERRORS {
	FMEM_NOT_FOUND = -1,	//  -1 - не найден
	FMEM_FLASH_ERR = -2,	//  -2 - flash rd/wr/erase error
	FMEM_ERROR = 	-3,		//  -3 - error
	FMEM_OVR_ERR = 	-4,		//  -4 - переполнение FMEMORY_SCFG_BANK_SIZE
	FMEM_MEM_ERR = 	-5		//  -5 - heap alloc error
};
//-----------------------------------------------------------------------------
#define MAX_FOBJ_SIZE 64 // максимальный размер сохраняемых объeктов (32..512)
// extern QueueHandle_t flash_mutex;
signed short flash_read_cfg(void *ptr, unsigned short id, unsigned short maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
bool flash_write_cfg(void *ptr, unsigned short id, unsigned short size);
void flash_write_(unsigned int addr, unsigned int len, unsigned char *buf);
bool flash_supported_eep_ver(unsigned int min_ver, unsigned int new_ver);
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif /* __FLASH_EEP_H_ */
