#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#define VERSION 0x42	 // BCD format (0x34 -> '3.4')
#define EEP_SUP_VER 0x09 // EEP data minimum supported version

#define DEVICE_LYWSD03MMC   0x055B	// LCD display LYWSD03MMC
#define DEVICE_MHO_C401   	0x0387	// E-Ink display MHO-C401 2020
#define DEVICE_MHO_C401N   	0x0008	// E-Ink display MHO-C401 2022
#define DEVICE_CGG1 		0x0B48  // E-Ink display CGG1-M "Qingping Temp & RH Monitor"
#define DEVICE_CGG1_ver		2022  	// =2022 - CGG1-M version 2022, or = 0 - CGG1-M version 2020,2021
#define DEVICE_CGDK2 		0x066F  // LCD display "Qingping Temp & RH Monitor Lite"

#ifndef DEVICE_TYPE
#define DEVICE_TYPE			DEVICE_LYWSD03MMC // DEVICE_LYWSD03MMC or DEVICE_MHO_C401 or DEVICE_CGG1 (+ set DEVICE_CGG1_ver) or DEVICE_CGDK2 or DEVICE_MHO_C401N
#endif

#define BLE_SECURITY_ENABLE 1 // = 1 support pin-code
#define BLE_EXT_ADV 		1 // = 1 support extension advertise (Test Only!)

#define USE_CLOCK 			1 // = 1 display clock, = 0 smile blinking
#define USE_TIME_ADJUST		1 // = 1 time correction enabled
#define USE_FLASH_MEMO		1 // = 1 flash logger enable
#define USE_TRIGGER_OUT 	1 // = 1 use trigger out (GPIO_PA5)
#define USE_WK_RDS_COUNTER	USE_TRIGGER_OUT // = 1 wake up when the reed switch is triggered + pulse counter

#define USE_SECURITY_BEACON 1 // = 1 support encryption beacon (bindkey)
#define USE_HA_BLE_BEACON	1 // = 1 https://github.com/custom-components/ble_monitor/issues/548
#define USE_MIHOME_BEACON	1 // = 1 Compatible with MiHome beacon

#define USE_DEVICE_INFO_CHR_UUID 	1 // = 1 enable Device Information Characteristics

/* Special DIY version - Voltage Logger:
 * Temperature 0..36.00 = ADC pin PB7 input 0..3.6V, LYWSD03MMC pcb mark "B1"
 * Humidity 0..36.00 = ADC pin PC4 input 0..3.6V, LYWSD03MMC pcb mark "P9"
 * Set DIY_ADC_TO_TH 1 */
#define DIY_ADC_TO_TH 	0

#define USE_MIHOME_SERVICE			0 // = 1 MiHome service compatibility (missing in current version! Set = 0!)
#define USE_NEW_OTA					0 // = 1 keeping the old firmware, erasing the region when updating (test version only!)
#define UART_PRINT_DEBUG_ENABLE		0 // =1 use u_printf() (PA7/SWS), source: SDK/components/application/print/u_printf.c

#if DEVICE_TYPE == DEVICE_MHO_C401

// TLSR8251F512ET24
// GPIO_PA5 - used EPD_SHD
// GPIO_PA6 - used EPD_RST
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB6 - used KEY, pcb mark "P5"
// GPIO_PB7 - used EPD_SDA
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PC4 - used EPD_SHD
// GPIO_PD2 - used EPD_CSB
// GPIO_PD7 - used EPD_SCL

#define SHL_ADC_VBAT	1  // "B0P" in adc.h
#define GPIO_VBAT	GPIO_PB0 // missing pin on case TLSR8251F512ET24
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

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

#if 0 // Test

#define GPIO_TRG			GPIO_PB6	// pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6	PM_PIN_PULLDOWN_100K

#else

#define GPIO_TRG			GPIO_PA0	// none
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		0
#define PA0_OUTPUT_ENABLE	0
#define PA0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLDOWN_100K

#define GPIO_RDS 			GPIO_PB6	// Reed Switch, input, pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_1M

#endif

#endif // USE_TRIGGER_OUT

#elif DEVICE_TYPE == DEVICE_MHO_C401N

// TLSR8251F512ET24
// GPIO_PA5 - used EPD_BUSY
// GPIO_PA6 - used EPD_CSB
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB6 - used KEY, pcb mark "P5"
// GPIO_PB7 - used EPD_RST2
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PC4 - used EPD_SCL
// GPIO_PD2 - used EPD_SDA
// GPIO_PD7 - used EPD_RST

#define SHL_ADC_VBAT		1  // "B0P" in adc.h
#define GPIO_VBAT	GPIO_PB0 // missing pin on case TLSR8251F512ET24
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3

#define EPD_RST2			GPIO_PB7 // should be high
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLUP_1M

#define EPD_RST 			GPIO_PD7 // should be high
#define PULL_WAKEUP_SRC_PD7 PM_PIN_PULLUP_1M
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		1
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO

#define EPD_BUSY			GPIO_PA5
#define PULL_WAKEUP_SRC_PA5 PM_PIN_PULLUP_1M
#define PA5_INPUT_ENABLE	1
#define PA5_FUNC			AS_GPIO

#define EPD_CSB				GPIO_PA6
#define PULL_WAKEUP_SRC_PA6 PM_PIN_PULLUP_1M
#define PA6_INPUT_ENABLE	1
#define PA6_DATA_OUT		1
#define PA6_OUTPUT_ENABLE	1
#define PA6_FUNC			AS_GPIO

#define EPD_SDA				GPIO_PD2
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLDOWN_100K
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		1
#define PD2_OUTPUT_ENABLE	1
#define PD2_FUNC			AS_GPIO

#define EPD_SCL				GPIO_PC4
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M
#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	1
#define PC4_FUNC			AS_GPIO

#if USE_TRIGGER_OUT

#if 0 // Test

#define GPIO_TRG			GPIO_PB6	// pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6	PM_PIN_PULLDOWN_100K

#else

#define GPIO_TRG			GPIO_PA0	// none
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		0
#define PA0_OUTPUT_ENABLE	0
#define PA0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA0	PM_PIN_PULLDOWN_100K

#define GPIO_RDS 			GPIO_PB6	// Reed Switch, input, pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_1M

#endif

#endif // USE_TRIGGER_OUT

#elif DEVICE_TYPE == DEVICE_CGG1

// TLSR8253F512ET32
// GPIO_PA0 - used EPD_RST
// GPIO_PA1 - used EPD_SHD
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB1 - free
// GPIO_PB4 - free
// GPIO_PB5 - free (ADC1)
// GPIO_PB6 - free (ADC2)
// GPIO_PB7 - used EPD_SDA
// GPIO_PC0 - SDA, used I2C
// GPIO_PC1 - SCL, used I2C
// GPIO_PC2 - TX, free
// GPIO_PC3 - RX, free, (Trigger, Output)
// GPIO_PC4 - used KEY
// GPIO_PD2 - used EPD_CSB
// GPIO_PD3 - free (Reed Switch, input)
// GPIO_PD4 - used EPD_BUSY
// GPIO_PD7 - used EPD_SCL

#define SHL_ADC_VBAT	1  // "B0P" in adc.h
#define GPIO_VBAT	GPIO_PB0 // missing pin on case TLSR8253F512ET32
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

#define I2C_SCL 	GPIO_PC0
#define I2C_SDA 	GPIO_PC1
#define I2C_GROUP 	I2C_GPIO_GROUP_C0C1

#define EPD_SHD				GPIO_PA1 // should be high
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

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

#if 0 // = 0 test PC4 - key
#define GPIO_RDS 			GPIO_PD3	// Reed Switch, input
#define PD3_INPUT_ENABLE	1
#define PD3_DATA_OUT		0
#define PD3_OUTPUT_ENABLE	0
#define PD3_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PD3 PM_PIN_PULLUP_1M
#else
#define GPIO_RDS 			GPIO_PC4	// Reed Switch, Input
//#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_1M
#endif

#endif // USE_TRIGGER_OUT

#if DIY_ADC_TO_TH // Special version: Temperature 0..36 = ADC pin PB5 input 0..3.6В
#define CHL_ADC1 			6	// B5P
#define GPIO_ADC1 			GPIO_PB5	// ADC1 input
#define PB5_OUTPUT_ENABLE	0
#define PB5_FUNC			AS_ADC
#endif

#if DIY_ADC_TO_TH // Special version: Humidity 0..36 = ADC pin PB6 input 0..3.6В
#define CHL_ADC2 			7	// B6P
#define GPIO_ADC2 			GPIO_PB6	// ADC2 input
#define PB6_OUTPUT_ENABLE	0
#define PB6_DATA_STRENGTH	0
#define PB6_FUNC			AS_GPIO
#endif

#elif DEVICE_TYPE == DEVICE_LYWSD03MMC

// TLSR8251F512ET24
// GPIO_PA5 - DM, free, pcb mark "reset" (TRG)
// GPIO_PA6 - DP, free, pcb mark "P8" (RDS)
// GPIO_PA7 - SWS, free, pcb mark "P14", (debug TX)
// GPIO_PB6 - used LCD, set "1"
// GPIO_PB7 - free, pcb mark "B1" (ADC1)
// GPIO_PC2 - SDA, used I2C, pcb mark "P12"
// GPIO_PC3 - SCL, used I2C, pcb mark "P15"
// GPIO_PC4 - free, pcb mark "P9" (ADC2)
// GPIO_PD2 - CS/PWM, free
// GPIO_PD7 - free [B1.4], UART TX LCD [B1.6], pcb mark "P7"

#define SHL_ADC_VBAT	1  // "B0P" in adc.h
#define GPIO_VBAT	GPIO_PB0 // missing pin on case TLSR8251F512ET24
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PA5	// Trigger, output, pcb mark "reset"
#define PA5_INPUT_ENABLE	1
#define PA5_DATA_OUT		0
#define PA5_OUTPUT_ENABLE	0
#define PA5_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA5	PM_PIN_PULLDOWN_100K

#define GPIO_RDS 			GPIO_PA6	// Reed Switch, input, pcb mark "P8"
#define PA6_INPUT_ENABLE	1
#define PA6_DATA_OUT		0
#define PA6_OUTPUT_ENABLE	0
#define PA6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA6 PM_PIN_PULLUP_1M

#define PULL_WAKEUP_SRC_PD7	PM_PIN_PULLUP_1M // UART TX (B1.6)
//#define PD7_INPUT_ENABLE	1
//#define PD7_FUNC			AS_UART

#define PULL_WAKEUP_SRC_PB6 PM_PIN_PULLUP_10K // LCD on low temp needs this, its an unknown pin going to the LCD controller chip

#endif // USE_TRIGGER_OUT

#if DIY_ADC_TO_TH // Special version: Temperature 0..36 = ADC pin PB7 input 0..3.6В, pcb mark "B1"
#define CHL_ADC1 			8	// B7P
#define GPIO_ADC1 			GPIO_PB7	// ADC input, pcb mark "B1"
#define PB7_OUTPUT_ENABLE	0
#define PB7_FUNC			AS_ADC
#endif

#if DIY_ADC_TO_TH // Special version: Humidity 0..36 = ADC pin PC4 input 0..3.6В, pcb mark "P9"
#define CHL_ADC2 			9	// C4P
#define GPIO_ADC2 			GPIO_PC4	// ADC input, pcb mark "P9"
#define PC4_OUTPUT_ENABLE	0
#define PC4_DATA_STRENGTH	0
#define PC4_FUNC			AS_GPIO
#endif

#elif DEVICE_TYPE == DEVICE_CGDK2

// TLSR8253F512ET32
// GPIO_PA0 - free, UART_RX, pcb mark "TP3", (Reed Switch, input)
// GPIO_PA1 - free
// GPIO_PA7 - SWS, free, (debug TX), pcb mark "TP17"
// GPIO_PB1 - free, UART_TX, pcb mark "TP1", (TRG)
// GPIO_PB4 - free
// GPIO_PB5 - free
// GPIO_PB6 - free, (ADC2)
// GPIO_PB7 - free, (ADC1)
// GPIO_PC0 - SDA, used I2C
// GPIO_PC1 - SCL, used I2C
// GPIO_PC2 - free
// GPIO_PC3 - free
// GPIO_PC4 - used KEY
// GPIO_PD2 - free
// GPIO_PD3 - free
// GPIO_PD4 - free
// GPIO_PD7 - free

#define SHL_ADC_VBAT	1  // "B0P" in adc.h
#define GPIO_VBAT	GPIO_PB0 // missing pin on case TLSR8253F512ET32
#define PB0_INPUT_ENABLE	1
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

#define I2C_SCL 	GPIO_PC0
#define I2C_SDA 	GPIO_PC1
#define I2C_GROUP 	I2C_GPIO_GROUP_C0C1

// PC4 - key
#define GPIO_KEY			GPIO_PC4
#define PC4_INPUT_ENABLE	1

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PB1
#define PB1_INPUT_ENABLE	1
#define PB1_DATA_OUT		0
#define PB1_OUTPUT_ENABLE	0
#define PB1_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB1	PM_PIN_PULLDOWN_100K

#if 0 // = 0 test PC4 - key

#define GPIO_RDS 			GPIO_PA0	// Reed Switch, Input
#define PA0_INPUT_ENABLE	1
#define PA0_DATA_OUT		0
#define PA0_OUTPUT_ENABLE	0
#define PA0_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA0 PM_PIN_PULLUP_1M

#else

#define GPIO_RDS 			GPIO_PC4	// Reed Switch, Input
//#define PC4_INPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	0
#define PC4_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_1M
#endif

#endif // USE_TRIGGER_OUT

#if DIY_ADC_TO_TH // Special version: Temperature 0..36 = ADC pin PB7 input 0..3.6В
#define CHL_ADC1 			8	// B7P
#define GPIO_ADC1 			GPIO_PB7	// ADC1 input
#define PB7_OUTPUT_ENABLE	0
#define PB7_FUNC			AS_ADC
#endif

#if DIY_ADC_TO_TH // Special version: Humidity 0..36 = ADC pin PB6 input 0..3.6В
#define CHL_ADC2 			7	// B6P
#define GPIO_ADC2 			GPIO_PB6	// ADC2 input
#define PB6_OUTPUT_ENABLE	0
#define PB6_DATA_STRENGTH	0
#define PB6_FUNC			AS_GPIO
#endif

#endif // DEVICE_TYPE == ?

#if UART_PRINT_DEBUG_ENABLE
#define PRINT_BAUD_RATE 1500000 // real ~1000000
#define DEBUG_INFO_TX_PIN	GPIO_PA7 // SWS
#define PA7_DATA_OUT		1
#define PA7_OUTPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PA7 PM_PIN_PULLUP_1M
#define PA7_FUNC		AS_GPIO
#endif // UART_PRINT_DEBUG_ENABLE


#define MODULE_WATCHDOG_ENABLE		0 //
#define WATCHDOG_INIT_TIMEOUT		15000  //ms (min 5000 ms if pincode)

/* DEVICE_LYWSD03MMC Average consumption (Show battery on, Comfort on, advertising 2.0 sec, measure 10 sec):
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
  0x77000 Customize freq_offset adjust cap value (CUST_CAP_INFO_ADDR)
  0x78000 User Data Area (EEP, saving configuration) (FMEMORY_SCFG_BASE_ADDR), conflict in master mode (CFG_ADR_PEER)
  0x80000 End Flash (FLASH_SIZE)
 */
/* flash sector address with binding information */
#define		CFG_ADR_BIND	0x74000 //no master, slave device (blt_config.h)

#define BLE_HOST_SMP_ENABLE BLE_SECURITY_ENABLE

#define CHG_CONN_PARAM	// test

#include "vendor/common/default_config.h"

#if defined(__cplusplus)
}
#endif
