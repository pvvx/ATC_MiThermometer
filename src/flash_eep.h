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

// EEPROM IDs
#define EEP_ID_CFG (0x0CFC) // EEP ID config data
#define EEP_ID_CFS (0x0CF5) // EEP ID sensor TH coefficients
#define EEP_ID_CMY (0x0B20) // EEP ID sensor MY18B20 coefficients
#define EEP_ID_TRG (0x0DFE) // EEP ID trigger data
#define EEP_ID_RPC (0x0DF5) // EEP ID reed switch pulse counter
#define EEP_ID_HXC (0x53A3) // EEP ID hx71x config data
#define EEP_ID_SCN (0x2CA8) // EEP ID scan config data
#define EEP_ID_DAC (0xCDAC) // EEP ID DAC config
#define EEP_ID_PCD (0xC0DE) // EEP ID pincode
#define EEP_ID_CMF (0x0FCC) // EEP ID comfort data
#define EEP_ID_DVN (0x0DB5) // EEP ID device name
#define EEP_ID_TIM (0x0ADA) // EEP ID time adjust
#define EEP_ID_KEY (0xBEAC) // EEP ID bkey
#define EEP_ID_HWV (0x1234) // EEP ID Mi HW version ("B1.4","B1.5",...)
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
bool flash_supported_eep_ver(unsigned int min_ver, unsigned int new_ver);
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif /* __FLASH_EEP_H_ */
