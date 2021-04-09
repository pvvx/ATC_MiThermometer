#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#define VERSION 0x30	 // BCD format (0x34 -> '3.4')
#define EEP_SUP_VER 0x09 // EEP data minimum supported version

#define DEVICE_LYWSD03MMC   0x055B	// LCD display LYWSD03MMC
#define DEVICE_MHO_C401   	0x0387	// E-Ink display MHO-C401
#define DEVICE_CGG1 		0x0B48  // E-Ink display CGG1-M "Qingping Temp & RH Monitor"

#define DEVICE_TYPE			DEVICE_LYWSD03MMC // DEVICE_LYWSD03MMC or DEVICE_MHO_C401 or DEVICE_CGG1

#define BLE_SECURITY_ENABLE 1
#define BLE_HOST_SMP_ENABLE BLE_SECURITY_ENABLE

#define USE_TRIGGER_OUT 	1 // use trigger out (GPIO_PA5)
#define USE_CLOCK 			1 // = 1 display clock, = 0 smile blinking
#define USE_TIME_ADJUST		1 // = 1 time correction enabled
#define USE_FLASH_MEMO		1 // = 1 flash logger enable

#define USE_DEVICE_INFO_CHR_UUID 	1 // = 1 enable Device Information Characteristics
#define USE_MIHOME_SERVICE			0 // = 1 MiHome service compatibility (missing in current version! Set = 0!)
#define USE_MIHOME_BEACON			1 // = 1 Compatible with MiHome beacon encryption
#define USE_NEW_OTA					0 // = 1 keeping the old firmware, erasing the region when updating (test version only!)

#if DEVICE_TYPE == DEVICE_MHO_C401

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3

#define EPD_SHD				GPIO_PC4 // should be high
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_10K

#define EPD_BUSY			GPIO_PA5
#define PULL_WAKEUP_SRC_PA5 PM_PIN_PULLUP_1M
#define PA5_INPUT_ENABLE	1
#define PA5_FUNC			AS_GPIO

#define EPD_RST				GPIO_PA6
#define PULL_WAKEUP_SRC_PA6 PM_PIN_PULLUP_1M
#define PA6_INPUT_ENABLE	1
#define PA6_DATA_OUT		1
#define PA6_OUTPUT_ENABLE	1
#define PA6_FUNC			AS_GPIO

#define EPD_CSB				GPIO_PD2
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLUP_1M
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		1
#define PD2_OUTPUT_ENABLE	1
#define PD2_FUNC			AS_GPIO

#define EPD_SDA				GPIO_PB7
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PB7_INPUT_ENABLE	1
#define PB7_DATA_OUT		1
#define PB7_OUTPUT_ENABLE	1
#define PB7_FUNC			AS_GPIO

#define EPD_SCL				GPIO_PD7
#define PULL_WAKEUP_SRC_PD7 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		0
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PB6	// pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6	PM_PIN_PULLDOWN_100K

#endif // USE_TRIGGER_OUT

#elif DEVICE_TYPE == DEVICE_CGG1

#define I2C_SCL 	GPIO_PC0
#define I2C_SDA 	GPIO_PC1
#define I2C_GROUP 	I2C_GPIO_GROUP_C0C1

#define EPD_BUSY			GPIO_PD4
#define PULL_WAKEUP_SRC_PD4 PM_PIN_PULLUP_1M
#define PD4_INPUT_ENABLE	1
#define PD4_FUNC			AS_GPIO

#define EPD_RST				GPIO_PA0
#define PULL_WAKEUP_SRC_PA0 PM_PIN_PULLUP_1M
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		1
#define PA0_OUTPUT_ENABLE	1
#define PA0_FUNC			AS_GPIO

#define EPD_SHD				GPIO_PA1 // should be high
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

#define EPD_CSB				GPIO_PD2
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLUP_1M
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		1
#define PD2_OUTPUT_ENABLE	1
#define PD2_FUNC			AS_GPIO

#define EPD_SDA				GPIO_PB7
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PB7_INPUT_ENABLE	1
#define PB7_DATA_OUT		1
#define PB7_OUTPUT_ENABLE	1
#define PB7_FUNC			AS_GPIO

#define EPD_SCL				GPIO_PD7
#define PULL_WAKEUP_SRC_PD7 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		0
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO

// PC4 - key
#define GPIO_KEY			GPIO_PC4
#define PC4_INPUT_ENABLE	1

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PC3
#define PC3_INPUT_ENABLE	1
#define PC3_DATA_OUT		0
#define PC3_OUTPUT_ENABLE	0
#define PC3_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLDOWN_100K

#endif // USE_TRIGGER_OUT

#elif DEVICE_TYPE == DEVICE_LYWSD03MMC

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PA5	// pcb mark "reset"
#define PA5_INPUT_ENABLE	1
#define PA5_DATA_OUT		0
#define PA5_OUTPUT_ENABLE	0
#define PA5_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA5	PM_PIN_PULLDOWN_100K

#endif // USE_TRIGGER_OUT
#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC

#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		250  //ms

/* DEVICE_LYWSD03MMC Average consumption (Show battery on, Comfort on, advertising 2 sec, measure 10 sec):
 * 16 MHz - 17.43 uA
 * 24 MHz - 17.28 uA
 * 32 MHz - 17.36 uA
 * (TX +3 dB)
 * Average consumption Original Xiaomi LYWSD03MMC (advertising 1700 ms, measure 6800 ms):
 * 18.64 uA (TX 0 dB)
 */
#define CLOCK_SYS_CLOCK_HZ  	24000000 // 16000000, 24000000, 32000000, 48000000
enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

#define pm_wait_ms(t) cpu_stall_wakeup_by_timer0(t*CLOCK_SYS_CLOCK_1MS);
#define pm_wait_us(t) cpu_stall_wakeup_by_timer0(t*CLOCK_SYS_CLOCK_1US);

#define RAM _attribute_data_retention_ // short version, this is needed to keep the values in ram after sleep

/* Flash map:
  0x00000 Old Firmware bin
  0x20000 OTA New bin storage Area
  0x40000 User Data Area (Logger, saving measurements) (FLASH_ADDR_START_MEMO)
  0x74000 Pair & Security info (CFG_ADR_BIND)
  0x76000 MAC address (CFG_ADR_MAC)
  0x78000 User Data Area (EEP, saving configuration) (FMEMORY_SCFG_BASE_ADDR)
  0x80000 End Flash (FLASH_SIZE)
 */
/* flash sector address with binding information */
#define		CFG_ADR_BIND	0x74000 //no master, slave device (blt_config.h)

#include "vendor/common/default_config.h"

#if defined(__cplusplus)
}
#endif
