# ATC_MiThermometer API Reference

Comprehensive documentation for all functions, interfaces, and data structures in the ATC_MiThermometer firmware project.

**Table of Contents**
- [Overview](#overview)
- [C Firmware API](#c-firmware-api)
  - [Application Core (app.h)](#application-core-apph)
  - [BLE Interface (ble.h)](#ble-interface-bleh)
  - [LCD Display (lcd.h)](#lcd-display-lcdh)
  - [Sensors (sensor.h)](#sensors-sensorh)
  - [BTHome Beacon (bthome_beacon.h)](#bthome-beacon-bthome_beaconh)
  - [Mi Beacon (mi_beacon.h)](#mi-beacon-mi_beaconh)
  - [Custom Beacon (custom_beacon.h)](#custom-beacon-custom_beaconh)
  - [I2C Interface (i2c.h)](#i2c-interface-i2ch)
  - [Command Parser (cmd_parser.h)](#command-parser-cmd_parserh)
  - [Battery Management (battery.h)](#battery-management-batteryh)
  - [Logger/History (logger.h)](#loggerhistory-loggerh)
  - [Trigger System (trigger.h)](#trigger-system-triggerh)
  - [Reed Switch Counter (rds_count.h)](#reed-switch-counter-rds_counth)
  - [External OTA (ext_ota.h)](#external-ota-ext_otah)
  - [Cryptography (ccm.h)](#cryptography-ccmh)
  - [BME280 Sensor (bme280.h)](#bme280-sensor-bme280h)
  - [DS18B20 Sensor (my18b20.h)](#ds18b20-sensor-my18b20h)
  - [HX71X Scale Sensor (hx71x.h)](#hx71x-scale-sensor-hx71xh)
- [Python Interface API](#python-interface-api)
  - [Construct Definitions (atc_mi_construct.py)](#construct-definitions-atc_mi_constructpy)
  - [Construct Adapters (atc_mi_construct_adapters.py)](#construct-adapters-atc_mi_construct_adapterspy)
  - [Configuration Tool (atc_mi_config.py)](#configuration-tool-atc_mi_configpy)
  - [BLE Advertising (atc_mi_advertising.py)](#ble-advertising-atc_mi_advertisingpy)
  - [Advertisement Format (atc_mi_adv_format.py)](#advertisement-format-atc_mi_adv_formatpy)

---

## Overview

ATC_MiThermometer is a custom firmware for Xiaomi Mijia BLE thermometers based on the Telink TLSR8251 chipset. The firmware supports multiple device models and provides enhanced functionality including:

- Multiple BLE advertising formats (ATC1441, PVVX Custom, Mi, BTHome v2)
- Optional encryption with bindkey
- Temperature, humidity, and battery monitoring
- LCD display control
- Data logging/history
- Reed switch/door sensor support
- Over-the-air (OTA) updates
- Trigger/threshold alerting

---

## C Firmware API

### Application Core (app.h)

The main application header defining configuration structures, device states, and core application interfaces.

#### Enumerations

##### `HW_VERSION_ID`
Hardware version identifiers for supported devices.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `HW_VER_LYWSD03MMC_B14` | LYWSD03MMC B1.4 with SHTV3 sensor |
| 1 | `HW_VER_MHO_C401` | MHO-C401 with SHTV3 sensor |
| 2 | `HW_VER_CGG1` | CGG1 with SHTV3 sensor |
| 3 | `HW_VER_LYWSD03MMC_B19` | LYWSD03MMC B1.9 with SHT4x sensor |
| 4 | `HW_VER_LYWSD03MMC_B16` | LYWSD03MMC B1.6 with SHT4x sensor |
| 5 | `HW_VER_LYWSD03MMC_B17` | LYWSD03MMC B1.7 with SHT4x sensor |
| 6 | `HW_VER_CGDK2` | CGDK2 with SHTV3 sensor |
| 7 | `HW_VER_CGG1_2022` | CGG1 2022 version with SHTV3 sensor |
| 8 | `HW_VER_MHO_C401_2022` | MHO-C401 2022 version with SHTV3 sensor |
| 9 | `HW_VER_MJWSD05MMC` | MJWSD05MMC with SHT4x sensor |
| 10 | `HW_VER_LYWSD03MMC_B15` | LYWSD03MMC B1.5 with SHTV3 sensor |
| 11 | `HW_VER_MHO_C122` | MHO-C122 with SHTV3 sensor |
| 12 | `HW_VER_MJWSD05MMC_EN` | MJWSD05MMC English version with SHT4x sensor |
| 13 | `HW_VER_MJWSD06MMC` | MJWSD06MMC |
| 14 | `HW_VER_LYWSD03MMC_NB16` | LYWSD03MMC NB16 |
| 15 | `HW_VER_EXTENDED` | Extended/DIY devices |

##### `ADV_TYPE_ENUM`
Advertising format types.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `ADV_TYPE_ATC` | ATC1441 format (legacy) |
| 1 | `ADV_TYPE_PVVX` | PVVX Custom format |
| 2 | `ADV_TYPE_MI` | Xiaomi Mi format |
| 3 | `ADV_TYPE_BTHOME` | BTHome v2 format (default) |

##### `CONNECTED_FLG_BITS_e`
Connection state flags.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `CONNECTED_FLG_ENABLE` | Connection enabled |
| 1 | `CONNECTED_FLG_PAR_UPDATE` | Connection parameters updated |
| 2 | `CONNECTED_FLG_BONDING` | Bonding/pairing success |
| 7 | `CONNECTED_FLG_RESET_OF_DISCONNECT` | Reset on disconnect flag |

##### `OTA_STAGES_e`
OTA update stages.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `OTA_NONE` | No OTA in progress |
| 1 | `OTA_WORK` | OTA active |
| 2 | `OTA_WAIT` | OTA waiting |
| 3 | `OTA_EXTENDED` | Extended OTA mode |

#### Data Structures

##### `cfg_t`
Device configuration structure stored in flash memory.

```c
typedef struct __attribute__((packed)) _cfg_t {
    struct __attribute__((packed)) {
        u8 advertising_type : 2;    // 0-atc1441, 1-Custom, 2-Mi, 3-BTHome
        u8 comfort_smiley   : 1;    // Enable comfort smiley display
        u8 show_time_smile  : 1;    // Show time instead of smile (if USE_CLOCK)
        u8 temp_F_or_C      : 1;    // Temperature unit: 0=Celsius, 1=Fahrenheit
        u8 show_batt_enabled: 1;    // Show battery on display
        u8 tx_measures      : 1;    // Send measurements in connected mode
        u8 lp_measures      : 1;    // Low power measurement mode
    } flg;
    struct __attribute__((packed)) {
        u8 smiley       : 3;        // Smiley face type (0-7)
        u8 adv_crypto   : 1;        // Enable encrypted advertising
        u8 adv_flags    : 1;        // Include advertising flags
        u8 bt5phy       : 1;        // Support BT5.0 All PHY
        u8 longrange    : 1;        // Enable Long Range mode
        u8 screen_off   : 1;        // Turn off screen (v4.3+)
    } flg2;
    struct __attribute__((packed)) {
        u8 adv_interval_delay : 4;  // Random delay 0-15 in 0.625ms
        u8 reserved           : 2;
        u8 date_ddmm          : 1;  // Date format DD/MM
        u8 not_day_of_week    : 1;  // Hide day of week
    } flg3;
    u8 event_adv_cnt;               // Event advertising count (min 5)
    u8 advertising_interval;        // Interval * 62.5ms (1-160)
    u8 measure_interval;            // Measure interval multiplier (2-25)
    u8 rf_tx_power;                 // TX power (130-191)
    u8 connect_latency;             // Connection latency * 20ms
    u8 min_step_time_update_lcd;    // LCD update time * 50ms
    u8 hw_ver;                      // Hardware version (read only)
    u8 averaging_measurements;      // Averaging count (0=off, 1-255)
} cfg_t;
```

##### `measured_data_t`
Structure holding current sensor measurements.

```c
typedef struct _measured_data_t {
    u16 average_battery_mv; // Battery voltage in mV (averaged)
    s16 temp;               // Temperature x0.01 C
    s16 humi;               // Humidity x0.01 %
    u16 count;              // Measurement counter
    u32 pressure;           // Pressure (if BME280 enabled)
    u16 co2;                // CO2 in ppm (if SCD41 enabled)
    s16 xtemp[N];           // External temperature sensors
    u16 battery_mv;         // Current battery voltage
    s16 temp_x01;           // Temperature x0.1 C
    s16 humi_x01;           // Humidity x0.1 %
    u8  humi_x1;            // Humidity x1 %
    u8  battery_level;      // Battery level 0-100%
    s32 energy;             // Energy measurement (INA226)
} measured_data_t;
```

##### `work_flg_t`
Runtime working flags and timers.

```c
typedef struct _work_flg_t {
    u32 utc_time_sec;           // UTC time in seconds since epoch
    u32 utc_time_sec_tick;      // Clock counter in 1/16 us
    u32 utc_time_tick_step;     // Clock adjustment value
    u32 adv_interval;           // Advertising interval in 0.625ms
    u32 connection_timeout;     // Connection timeout in 10ms
    u32 measurement_step_time;  // Measurement interval time
    u32 tim_measure;            // Measurement timer
    u8  ble_connected;          // Connection state flags
    u8  ota_is_working;         // OTA state (OTA_STAGES_e)
    volatile u8 start_measure;  // Start measurement flag
    u8  tx_measures;            // Measurement transfer counter
    union {
        u8 all_flgs;
        struct {
            u8 send_measure   : 1;  // Send measurement flag
            u8 update_lcd     : 1;  // Update LCD flag
            u8 update_adv     : 1;  // Update advertising flag
            u8 th_sensor_read : 1;  // Sensor read complete flag
        } b;
    } msc;
    u8 adv_interval_delay;      // Random advertising delay
} work_flg_t;
```

##### `external_data_t`
External data for LCD display.

```c
typedef struct __attribute__((packed)) _external_data_t {
    s16 big_number;     // Main display number x0.1 (-995..19995)
    s16 small_number;   // Small display number x1 (-9..99)
    u16 vtime_sec;      // Validity time in seconds
    struct __attribute__((packed)) {
        u8 smiley       : 3;    // Smiley type (0-7)
        u8 percent_on   : 1;    // Show percent symbol
        u8 battery      : 1;    // Show battery symbol
        u8 temp_symbol  : 3;    // Temperature symbol type
    } flg;
} external_data_t;
```

##### `scomfort_t`
Comfort zone temperature and humidity thresholds.

```c
typedef struct _comfort_t {
    s16 t[2];   // Temperature range [low, high] x0.01 C
    u16 h[2];   // Humidity range [low, high] x0.01 %
} scomfort_t;
```

#### Functions

##### `void ev_adv_timeout(u8 e, u8 *p, int n)`
Advertising timeout event callback.

**Parameters:**
- `e` - Event type
- `p` - Pointer to event data
- `n` - Data length

**Description:** Called when advertising duration times out. Handles BLE advertising state transitions.

---

##### `void test_config(void)`
Validate and sanitize configuration values.

**Description:** Checks all configuration parameters for valid ranges and corrects invalid values to defaults.

---

##### `void set_hw_version(void)`
Detect and set hardware version.

**Description:** Automatically detects the hardware version based on device characteristics and sets `cfg.hw_ver`.

---

##### `u8 *str_bin2hex(u8 *d, u8 *s, int len)`
Convert binary data to hexadecimal string.

**Parameters:**
- `d` - Destination buffer for hex string
- `s` - Source binary data
- `len` - Number of bytes to convert

**Returns:** Pointer to destination buffer

---

##### `void blc_newMacAddress(int flash_addr, u8 *mac_pub, u8 *mac_rand)`
Set new MAC address.

**Parameters:**
- `flash_addr` - Flash address for MAC storage
- `mac_pub` - Public MAC address (6 bytes)
- `mac_rand` - Random MAC address part (2 bytes)

**Description:** Configures a new BLE MAC address and stores it in flash.

---

##### `void SwapMacAddress(u8 *mac_out, u8 *mac_in)`
Swap MAC address byte order.

**Parameters:**
- `mac_out` - Output buffer (6 bytes)
- `mac_in` - Input MAC address (6 bytes)

---

##### `void flash_erase_mac_sector(u32 faddr)`
Erase flash sector containing MAC address.

**Parameters:**
- `faddr` - Flash address of sector to erase

---

##### `void bindkey_init(void)`
Initialize encryption bindkey.

**Description:** Loads or generates the 16-byte encryption bindkey used for encrypted BLE advertising.

---

##### `void set_default_cfg(void)`
Reset configuration to defaults.

**Description:** Sets all configuration values to their factory defaults.

---

##### `static inline u8 get_key2_pressed(void)`
Check if key2 (reset button) is pressed.

**Returns:** Non-zero if key is pressed, 0 otherwise

---

### BLE Interface (ble.h)

BLE stack interface, GATT services, and advertising functions.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `BTHOME_UUID16` | 0xFCD2 | BTHome service UUID |
| `ADV_BUFFER_SIZE` | 28 or 59 | Advertising buffer size |
| `SEND_BUFFER_SIZE` | 20 | ATT MTU - 3 |
| `DEF_CON_INERVAL` | 16 | Default connection interval (20ms) |
| `DEF_CONNECT_LATENCY` | 49 | Default connection latency (1 sec) |
| `CONNECTABLE_ADV_INERVAL` | 1600 | Connectable advertising interval (1 sec) |

#### Enumerations

##### `EXT_ADV_e`
Extension advertising modes.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `EXT_ADV_Off` | Legacy advertising |
| 1 | `EXT_ADV_Coded` | LE Long Range (PHY Coded) |
| 2 | `EXT_ADV_1M` | Extension advertising (PHY 1M) |

##### `ATT_HANDLE`
GATT attribute handles enumeration.

| Handle | Description |
|--------|-------------|
| `GenericAccess_PS_H` | Generic Access service |
| `GenericAccess_DeviceName_DP_H` | Device name characteristic |
| `BATT_LEVEL_INPUT_DP_H` | Battery level characteristic |
| `TEMP_LEVEL_INPUT_DP_H` | Temperature characteristic |
| `HUMI_LEVEL_INPUT_DP_H` | Humidity characteristic |
| `OTA_CMD_OUT_DP_H` | OTA command characteristic |
| `RxTx_CMD_OUT_DP_H` | RxTx data characteristic |
| `Mi_PS_H` | Mi advertising service |

#### Data Structures

##### `adv_buf_t`
Advertising buffer structure.

```c
typedef struct _adv_buf_t {
    u32 send_count;     // Advertising counter (= beacon_nonce.cnt32)
    u8  meas_count;     // Counter until next measurement
    u8  call_count;     // Data iteration counter
    u8  update_count;   // Refresh flag (0=refresh, 0xff=wait)
    u8  ext_adv_init;   // Extension advertising initialized flag
    u8  data_size;      // Advertising data size
    u8  flag[3];        // Advertising type flags
    u8  data[ADV_BUFFER_SIZE];  // Advertising payload
} adv_buf_t;
```

##### `gap_periConnectParams_t`
GAP peripheral connection parameters.

```c
typedef struct {
    u16 intervalMin;    // Min connection interval (0x0006-0x0C80 * 1.25ms)
    u16 intervalMax;    // Max connection interval
    u16 latency;        // Connection latency (0x0000-0x03E8)
    u16 timeout;        // Supervision timeout (0x000A-0x0C80 * 10ms)
} gap_periConnectParams_t;
```

#### Functions

##### `void app_enter_ota_mode(void)`
Enter OTA firmware update mode.

**Description:** Prepares the device for receiving an over-the-air firmware update.

---

##### `void set_adv_data(void)`
Set advertising data payload.

**Description:** Constructs the advertising packet based on current configuration and measurement data.

---

##### `void my_att_init(void)`
Initialize ATT attribute table.

**Description:** Sets up the GATT attribute table with all services and characteristics.

---

##### `void init_ble(void)`
Initialize BLE stack.

**Description:** Configures and starts the BLE stack, sets up advertising parameters.

---

##### `void ble_set_name(void)`
Set BLE device name.

**Description:** Updates the BLE device name from configuration or uses default "ATC_XXXX".

---

##### `void ble_send_measures(void)`
Send measurement data via BLE notification.

**Description:** Transmits current temperature, humidity, and battery measurements to connected client.

---

##### `void ble_send_ext(void)`
Send external data via BLE notification.

**Description:** Transmits external sensor data to connected client.

---

##### `void ble_send_lcd(void)`
Send LCD buffer via BLE notification.

**Description:** Transmits current LCD display buffer for remote viewing.

---

##### `void ble_send_cmf(void)`
Send comfort zone parameters via BLE notification.

**Description:** Transmits comfort temperature and humidity thresholds.

---

##### `void ble_send_trg(void)`
Send trigger configuration via BLE notification.

**Description:** Transmits trigger threshold settings.

---

##### `void ble_send_trg_flg(void)`
Send trigger flags via BLE notification.

**Description:** Transmits current trigger state flags.

---

##### `void set_adv_con_time(int restore)`
Set advertising connectable time.

**Parameters:**
- `restore` - Non-zero to restore normal interval, 0 for fast advertising

**Description:** Adjusts advertising interval for key press events or normal operation.

---

##### `void send_memo_blk(void)`
Send history data block via BLE notification.

**Description:** Transmits a block of logged measurement history data.

---

##### `int otaWritePre(void *p)`
OTA write pre-handler.

**Parameters:**
- `p` - Pointer to write data

**Returns:** 0 on success

**Description:** Validates and processes incoming OTA firmware data.

---

##### `int RxTxWrite(void *p)`
RxTx characteristic write handler.

**Parameters:**
- `p` - Pointer to received command data

**Returns:** 0 on success

**Description:** Processes commands received on the RxTx characteristic.

---

##### `void set_pvvx_adv_data(void)`
Set PVVX custom advertising data.

**Description:** Constructs advertising payload in PVVX custom format.

---

##### `void set_atc_adv_data(void)`
Set ATC1441 advertising data.

**Description:** Constructs advertising payload in ATC1441 format.

---

##### `void set_mi_adv_data(void)`
Set Mi advertising data.

**Description:** Constructs advertising payload in Xiaomi Mi format.

---

##### `void load_adv_data(void)`
Load advertising data for extension advertising.

**Description:** Prepares advertising data for BT5 extension advertising modes.

---

##### Inline Functions

```c
inline void ble_send_temp01(void)
```
Send temperature (x0.1 C) via notification.

```c
inline void ble_send_temp001(void)
```
Send temperature (x0.01 C) via notification.

```c
inline void ble_send_humi(void)
```
Send humidity (x0.01 %) via notification.

```c
inline void ble_send_ana(void)
```
Send analog value via notification.

```c
inline void ble_send_battery(void)
```
Send battery level (0-100%) via notification.

```c
inline void ble_send_cfg(void)
```
Send device configuration via notification.

---

### LCD Display (lcd.h)

LCD display driver interface for various display controllers.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `B14_I2C_ADDR` | 0x3C | LYWSD03MMC B1.4 LCD I2C address |
| `B19_I2C_ADDR` | 0x3E | LYWSD03MMC B1.9 LCD I2C address |
| `BU9792AFUV_I2C_ADDR` | 0x3E | BU9792AFUV LCD I2C address |
| `SMILE_HAPPY` | 5 | Happy smiley index |
| `SMILE_SAD` | 6 | Sad smiley index |
| `TMP_SYM_C` | 0xA0 | Celsius symbol |
| `TMP_SYM_F` | 0x60 | Fahrenheit symbol |
| `LCD_BUF_SIZE` | 6-18 | Display buffer size (device dependent) |

#### Enumerations

##### `SCR_TYPE_ENUM` (MJWSD05MMC)
Screen display types.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `SCR_TYPE_TIME` | Time display |
| 1 | `SCR_TYPE_TEMP` | Temperature display |
| 2 | `SCR_TYPE_HUMI` | Humidity display |
| 3 | `SCR_TYPE_BAT_P` | Battery percentage |
| 4 | `SCR_TYPE_BAT_V` | Battery voltage |
| 5 | `SCR_TYPE_EXT` | External data display |

#### Data Structures

##### `lcd_flg_t`
LCD control flags.

```c
typedef struct _lcd_flg_t {
    u32 chow_ext_ut;             // External data validity timer
    u32 min_step_time_update_lcd; // Minimum LCD update interval
    u32 tim_last_chow;           // Last display update timer
    u8  show_stage;              // Display update stage
    u8  update_next_measure;     // Update on next measurement flag
    u8  update;                  // Update required flag
    union {
        struct {
            u8 ext_data_buf : 1; // Show external buffer
            u8 notify_on    : 1; // Send notifications
            u8 res          : 5;
            u8 send_notify  : 1; // Send notify flag
        } b;
        u8 all_flg;
    };
} lcd_flg_t;
```

#### Functions

##### `void lcd(void)`
Main LCD update function.

**Description:** Updates the LCD display with current measurement data, handles display rotation.

---

##### `void init_lcd(void)`
Initialize LCD controller.

**Description:** Configures the LCD controller via I2C/SPI and initializes the display buffer.

---

##### `void send_to_lcd(void)`
Send display buffer to LCD.

**Description:** Transfers the display buffer to the LCD controller hardware.

---

##### `void update_lcd(void)`
Trigger LCD update.

**Description:** Marks the LCD for update on next refresh cycle.

---

##### `void show_temp_symbol(u8 symbol)`
Display temperature unit symbol.

**Parameters:**
- `symbol` - Symbol code (0x00=none, 0x60=F, 0xA0=C, etc.)

**Description:** Shows temperature unit symbol on display.

**Symbol codes:**
- 0x00 = "  " (off)
- 0x20 = "F" (partial)
- 0x40 = " -"
- 0x60 = "F"
- 0x80 = " _"
- 0xA0 = "C"
- 0xC0 = " ="
- 0xE0 = "E"

---

##### `void show_smiley(u8 state)`
Display smiley face.

**Parameters:**
- `state` - Smiley type (0-7)

**Description:** Shows comfort indicator smiley on display.

**Smiley types (LYWSD03MMC):**
- 0 = Off
- 1 = " ^_^ "
- 2 = " -^- "
- 3 = " ooo "
- 4 = "(   )"
- 5 = "(^_^)" happy
- 6 = "(-^-)" sad
- 7 = "(ooo)"

---

##### `void show_battery_symbol(bool state)`
Show/hide battery symbol.

**Parameters:**
- `state` - true to show, false to hide

---

##### `void show_big_number_x10(s16 number)`
Display main number with auto decimal point.

**Parameters:**
- `number` - Value x0.1 (-995 to 19995)

**Description:** Displays number with automatic decimal point placement: -99 to -9.9 to 199.9 to 1999.

---

##### `void show_ble_symbol(bool state)`
Show/hide BLE connection symbol.

**Parameters:**
- `state` - true to show, false to hide

---

##### `void show_small_number(s16 number, bool percent)`
Display small number.

**Parameters:**
- `number` - Value (-9 to 99)
- `percent` - Show percent symbol

---

##### `void show_clock(void)`
Display current time.

**Description:** Shows current time from RTC on display (if USE_DISPLAY_CLOCK enabled).

---

##### `void show_ota_screen(void)`
Display OTA update screen.

**Description:** Shows OTA update progress indicator.

---

##### `void show_reboot_screen(void)`
Display reboot screen.

**Description:** Shows reboot message before device restart.

---

##### `int task_lcd(void)`
LCD task handler.

**Returns:** 0 on completion

**Description:** State machine for multi-stage LCD updates (e-ink displays).

---

### Sensors (sensor.h)

Temperature/humidity sensor abstraction layer.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `AHT2x_I2C_ADDR` | 0x38 | AHT20/AHT30 I2C address |
| `CHT8305_I2C_ADDR` | 0x40 | CHT8305 I2C address |
| `SHT30_I2C_ADDR` | 0x44 | SHT30 I2C address |
| `SHT4x_I2C_ADDR` | 0x44 | SHT4x I2C address |
| `SHTC3_I2C_ADDR` | 0x70 | SHTC3 I2C address |

#### Enumerations

##### `TH_SENSOR_TYPES`
Sensor type identifiers.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `TH_SENSOR_NONE` | No sensor |
| 1 | `TH_SENSOR_SHTC3` | Sensirion SHTC3 |
| 2 | `TH_SENSOR_SHT4x` | Sensirion SHT40/41/45 |
| 3 | `TH_SENSOR_SHT30` | Sensirion SHT30 |
| 4 | `TH_SENSOR_CHT8305` | CHT8305 |
| 5 | `TH_SENSOR_AHT2x` | ASAIR AHT20/AHT30 |
| 6 | `TH_SENSOR_CHT8215` | CHT8215 |
| 7 | `IU_SENSOR_INA226` | INA226 current sensor |
| 8 | `IU_SENSOR_MY18B20` | DS18B20 temperature |
| 9 | `IU_SENSOR_MY18B20x2` | Dual DS18B20 |
| 10 | `IU_SENSOR_HX71X` | HX711/HX710 scale |
| 11 | `IU_SENSOR_PWMRH` | PWM humidity sensor |
| 12 | `IU_SENSOR_NTC` | NTC thermistor |
| 13 | `IU_SENSOR_INA3221` | INA3221 3-channel current |
| 14 | `IU_SENSOR_SCD41` | SCD41 CO2 sensor |
| 15 | `IU_SENSOR_BME280` | BME280 pressure/temp/humi |

#### Data Structures

##### `sensor_coef_t`
Sensor calibration coefficients.

```c
typedef struct _thsensor_coef_t {
    u32 val1_k;     // Temperature coefficient (multiplier)
    u32 val2_k;     // Humidity coefficient (multiplier)
    s16 val1_z;     // Temperature offset
    s16 val2_z;     // Humidity offset
} sensor_coef_t;
```

##### `sensor_cfg_t`
Sensor configuration structure.

```c
typedef struct _sensor_cfg_t {
    sensor_coef_t coef;     // Calibration coefficients
    u32 id;                 // Sensor ID/serial
    u8  i2c_addr;           // I2C address
    u8  sensor_type;        // Sensor type (TH_SENSOR_TYPES)
    u32 wait_tik;           // Wait timer (SCD41)
    volatile u32 time_measure;  // Measurement timer
    u32 measure_timeout;    // Measurement timeout
} sensor_cfg_t;
```

#### Functions

##### `void init_sensor(void)`
Initialize temperature/humidity sensor.

**Description:** Detects and initializes the connected sensor, loads calibration data.

---

##### `void start_measure_sensor_deep_sleep(void)`
Start sensor measurement for deep sleep mode.

**Description:** Initiates a sensor measurement cycle optimized for low power operation.

---

##### `int read_sensor_cb(void)`
Read sensor callback.

**Returns:** 0 on success, error code on failure

**Description:** Reads measurement data from the sensor and updates `measured_data`.

---

### BTHome Beacon (bthome_beacon.h)

BTHome v2 BLE advertising format implementation.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `ADV_BTHOME_UUID16` | 0xFCD2 | BTHome service UUID |
| `BtHomeID_Info` | 0x40 | Unencrypted info byte |
| `BtHomeID_Info_Encrypt` | 0x41 | Encrypted info byte |

#### Enumerations

##### `BtHomeIDs_e`
BTHome object IDs (sensor types).

| Value | Constant | Description |
|-------|----------|-------------|
| 0x00 | `BtHomeID_PacketId` | Packet counter (uint8) |
| 0x01 | `BtHomeID_battery` | Battery level (uint8, %) |
| 0x02 | `BtHomeID_temperature` | Temperature (int16, 0.01C) |
| 0x03 | `BtHomeID_humidity` | Humidity (uint16, 0.01%) |
| 0x04 | `BtHomeID_pressure24` | Pressure (uint24, 0.01hPa) |
| 0x05 | `BtHomeID_illuminance` | Light (uint24, 0.01lux) |
| 0x06 | `BtHomeID_weight` | Weight (uint16, 0.01kg) |
| 0x08 | `BtHomeID_dewpoint` | Dew point (int16, 0.01C) |
| 0x09 | `BtHomeID_count8` | Counter (uint8) |
| 0x0C | `BtHomeID_voltage` | Voltage (uint16, 0.001V) |
| 0x10 | `BtHomeID_switch` | Switch (uint8) |
| 0x11 | `BtHomeID_opened` | Door/window (uint8) |
| 0x12 | `BtHomeID_co2` | CO2 (uint16, ppm) |
| 0x3D | `BtHomeID_count16` | Counter (uint16) |
| 0x3E | `BtHomeID_count32` | Counter (uint32) |
| 0x43 | `BtHomeID_current` | Current (uint16, 0.001A) |
| 0x45 | `BtHomeID_temperature_01` | Temperature (int16, 0.1C) |
| 0x4A | `BtHomeID_voltage_01` | Voltage (uint16, 0.1V) |
| 0x5D | `BtHomeID_current_i16` | Current signed (int16, 0.001A) |

#### Data Structures

##### `bthome_beacon_nonce_t`
Encryption nonce for BTHome.

```c
typedef struct __attribute__((packed)) _bthome_beacon_nonce_t {
    u8  mac[6];     // Device MAC address
    u16 uuid16;     // Service UUID (0xFCD2)
    u8  info;       // Info byte (0x41 for encrypted)
    u32 cnt32;      // Packet counter
} bthome_beacon_nonce_t;
```

##### `adv_bthome_data1_t`
Primary BTHome data structure.

```c
typedef struct __attribute__((packed)) _adv_bthome_data1_t {
    u8  b_id;           // BtHomeID_battery
    u8  battery_level;  // 0-100%
    u8  t_id;           // BtHomeID_temperature
    s16 temperature;    // x0.01 C
    u8  h_id;           // BtHomeID_humidity
    u16 humidity;       // x0.01 %
    // ... additional fields for pressure, voltage, etc.
} adv_bthome_data1_t;
```

#### Functions

##### `void bthome_beacon_init(void)`
Initialize BTHome beacon.

**Description:** Sets up encryption nonce and prepares BTHome advertising.

---

##### `void bthome_data_beacon(void)`
Generate unencrypted BTHome data beacon.

**Description:** Creates BTHome advertising packet with sensor data.

---

##### `void bthome_encrypt_data_beacon(void)`
Generate encrypted BTHome data beacon.

**Description:** Creates encrypted BTHome advertising packet using bindkey.

---

##### `void bthome_event_beacon(u8 n)`
Generate BTHome event beacon.

**Parameters:**
- `n` - Event type (RDS_TYPES)

**Description:** Creates BTHome advertising packet for door/switch events.

---

##### `void bthome_encrypt_event_beacon(u8 n)`
Generate encrypted BTHome event beacon.

**Parameters:**
- `n` - Event type (RDS_TYPES)

**Description:** Creates encrypted BTHome event advertising packet.

---

### Mi Beacon (mi_beacon.h)

Xiaomi Mi BLE advertising format implementation.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `ADV_XIAOMI_UUID16` | 0xFE95 | Xiaomi service UUID |
| `XIAOMI_DEV_VERSION` | 5 | Mi beacon protocol version |

#### Enumerations

##### `XIAOMI_DEV_ID`
Xiaomi device type IDs.

| Value | Constant | Description |
|-------|----------|-------------|
| 0x055B | `XIAOMI_DEV_ID_LYWSD03MMC` | LYWSD03MMC |
| 0x0387 | `XIAOMI_DEV_ID_MHO_C401` | MHO-C401 |
| 0x0347 | `XIAOMI_DEV_ID_CGG1` | CGG1 |
| 0x0B48 | `XIAOMI_DEV_ID_CGG1_ENCRYPTED` | CGG1 encrypted |
| 0x066F | `XIAOMI_DEV_ID_CGDK2` | CGDK2 |
| 0x2832 | `XIAOMI_DEV_ID_MJWSD05MMC` | MJWSD05MMC |
| 0x55B5 | `XIAOMI_DEV_ID_MJWSD06MMC` | MJWSD06MMC |

##### `XIAOMI_DATA_ID`
Mi data object types.

| Value | Constant | Description |
|-------|----------|-------------|
| 0x1004 | `XIAOMI_DATA_ID_Temperature` | Temperature |
| 0x1006 | `XIAOMI_DATA_ID_Humidity` | Humidity |
| 0x100A | `XIAOMI_DATA_ID_Power` | Battery |
| 0x100D | `XIAOMI_DATA_ID_TempAndHumidity` | Combined T+H |
| 0x1019 | `XIAOMI_DATA_ID_DoorSensor` | Door sensor |

#### Data Structures

##### `adv_mi_fctrl_t`
Mi beacon frame control field.

```c
typedef union _adv_mi_fctrl_t {
    struct __attribute__((packed)) {
        u16 Factory         : 1;
        u16 Connected       : 1;
        u16 Central         : 1;
        u16 isEncrypted     : 1;    // Encryption flag
        u16 MACInclude      : 1;    // MAC in payload
        u16 CapabilityInclude : 1;  // Capability included
        u16 ObjectInclude   : 1;    // Object data included
        u16 Mesh            : 1;
        u16 registered      : 1;    // Device bound
        u16 solicited       : 1;    // Binding request
        u16 AuthMode        : 2;    // Auth mode (0-3)
        u16 version         : 4;    // Protocol version
    } bit;
    u16 word;
} adv_mi_fctrl_t;
```

##### `adv_mi_head_t`
Mi beacon header structure.

```c
typedef struct __attribute__((packed)) _adv_mi_head_t {
    u8  size;           // Packet size
    u8  uid;            // 0x16 (Service Data)
    u16 UUID;           // 0xFE95 (Xiaomi)
    adv_mi_fctrl_t fctrl;  // Frame control
    u16 dev_id;         // Device type
    u8  counter;        // Frame counter
} adv_mi_head_t;
```

#### Functions

##### `void mi_beacon_init(void)`
Initialize Mi beacon encryption.

**Description:** Sets up encryption context for Mi beacon format.

---

##### `void mi_beacon_summ(void)`
Average measurements for Mi beacon.

**Description:** Calculates averaged measurement values for Mi beacon transmission.

---

##### `void mi_data_beacon(void)`
Generate unencrypted Mi data beacon.

**Description:** Creates Mi format advertising packet with sensor data.

---

##### `void mi_encrypt_data_beacon(void)`
Generate encrypted Mi data beacon.

**Description:** Creates encrypted Mi format advertising packet.

---

##### `void mi_event_beacon(u8 n)`
Generate Mi event beacon.

**Parameters:**
- `n` - Event type (RDS_TYPES)

**Description:** Creates Mi format event advertising packet.

---

##### `void mi_encrypt_event_beacon(u8 n)`
Generate encrypted Mi event beacon.

**Parameters:**
- `n` - Event type (RDS_TYPES)

---

### Custom Beacon (custom_beacon.h)

PVVX custom and ATC1441 BLE advertising formats.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `ADV_CUSTOM_UUID16` | 0x181A | Environmental Sensing UUID |

#### Data Structures

##### `adv_custom_t`
PVVX custom format (unencrypted).

```c
typedef struct __attribute__((packed)) _adv_custom_t {
    u8  size;           // 18
    u8  uid;            // 0x16 (Service Data)
    u16 UUID;           // 0x181A
    u8  MAC[6];         // MAC address (LSB first)
    s16 temperature;    // x0.01 C
    u16 humidity;       // x0.01 %
    u16 battery_mv;     // Battery mV
    u8  battery_level;  // 0-100%
    u8  counter;        // Frame counter
    u8  flags;          // Trigger flags
} adv_custom_t;
```

##### `adv_atc1441_t`
ATC1441 format (mixed endianness).

```c
typedef struct __attribute__((packed)) _adv_atc1441_t {
    u8  size;           // 16
    u8  uid;            // 0x16
    u16 UUID;           // 0x181A (little-endian)
    u8  MAC[6];         // MAC address (MSB first, big-endian!)
    u8  temperature[2]; // x0.1 C (big-endian!)
    u8  humidity;       // x1 %
    u8  battery_level;  // 0-100%
    u8  battery_mv[2];  // mV (big-endian!)
    u8  counter;        // Frame counter
} adv_atc1441_t;
```

##### `adv_cust_enc_t`
PVVX encrypted format.

```c
typedef struct __attribute__((packed)) _adv_cust_enc_t {
    adv_cust_head_t head;   // Header (5 bytes)
    adv_cust_data_t data;   // Encrypted data (6 bytes)
    u8 mic[4];              // Message integrity code
} adv_cust_enc_t;
```

#### Functions

##### `void pvvx_data_beacon(void)`
Generate PVVX custom data beacon.

**Description:** Creates PVVX custom format advertising packet.

---

##### `void atc_data_beacon(void)`
Generate ATC1441 data beacon.

**Description:** Creates ATC1441 format advertising packet.

---

##### `void pvvx_encrypt_data_beacon(void)`
Generate encrypted PVVX beacon.

**Description:** Creates encrypted PVVX custom format packet.

---

##### `void atc_encrypt_data_beacon(void)`
Generate encrypted ATC1441 beacon.

**Description:** Creates encrypted ATC1441 format packet.

---

##### `void pvvx_event_beacon(u8 n)`
Generate PVVX event beacon.

**Parameters:**
- `n` - Event type (RDS_TYPES)

---

##### `void default_event_beacon(void)`
Generate default event beacon.

**Description:** Creates a default event advertising packet.

---

### I2C Interface (i2c.h)

I2C/SMBus communication interface.

#### Data Structures

##### `i2c_utr_t`
Universal I2C transaction structure.

```c
typedef struct _i2c_utr_t {
    unsigned char mode;     // bit0-6: restart byte, bit7: STOP/START
    unsigned char rdlen;    // bit0-6: read length, bit7: final ACK
    unsigned char wrdata[1]; // Address + write data
} i2c_utr_t;
```

#### Functions

##### `void init_i2c(void)`
Initialize I2C interface.

**Description:** Configures I2C GPIO pins and initializes the bus.

---

##### `int scan_i2c_addr(int address)`
Scan for I2C device.

**Parameters:**
- `address` - 7-bit I2C address to probe

**Returns:** 0 if device found, non-zero otherwise

---

##### `int send_i2c_byte(u8 i2c_addr, u8 cmd)`
Send single byte to I2C device.

**Parameters:**
- `i2c_addr` - 7-bit device address
- `cmd` - Byte to send

**Returns:** 0 on success

---

##### `int send_i2c_word(u8 i2c_addr, u16 cmd)`
Send 16-bit word to I2C device.

**Parameters:**
- `i2c_addr` - 7-bit device address
- `cmd` - Word to send (MSB first)

**Returns:** 0 on success

---

##### `int send_i2c_buf(u8 i2c_addr, u8 *dataBuf, u32 dataLen)`
Send buffer to I2C device.

**Parameters:**
- `i2c_addr` - 7-bit device address
- `dataBuf` - Data buffer to send
- `dataLen` - Number of bytes to send

**Returns:** 0 on success

---

##### `int read_i2c_buf(u8 i2c_addr, u8 *dataBuf, u32 dataLen)`
Read buffer from I2C device.

**Parameters:**
- `i2c_addr` - 7-bit device address
- `dataBuf` - Buffer to receive data
- `dataLen` - Number of bytes to read

**Returns:** 0 on success

---

##### `int read_i2c_byte_addr(u8 i2c_addr, u8 reg_addr, u8 *dataBuf, u32 dataLen)`
Read from I2C register.

**Parameters:**
- `i2c_addr` - 7-bit device address
- `reg_addr` - Register address
- `dataBuf` - Buffer to receive data
- `dataLen` - Number of bytes to read

**Returns:** 0 on success

---

##### `int I2CBusUtr(void *outdata, i2c_utr_t *tr, u32 wrlen)`
Universal I2C transaction.

**Parameters:**
- `outdata` - Output buffer for read data
- `tr` - Transaction descriptor
- `wrlen` - Write data length

**Returns:** Number of bytes read, or negative error

**Description:** Performs complex I2C transactions with restart conditions.

---

### Command Parser (cmd_parser.h)

BLE command interface for device configuration.

#### Enumerations

##### `CMD_ID_KEYS`
Command identifiers.

| Value | Constant | Description |
|-------|----------|-------------|
| 0x00 | `CMD_ID_DEV_ID` | Get device ID/version |
| 0x01 | `CMD_ID_DNAME` | Get/Set device name |
| 0x02 | `CMD_ID_GDEVS` | Get I2C device addresses |
| 0x03 | `CMD_ID_I2C_SCAN` | I2C bus scan |
| 0x04 | `CMD_ID_I2C_UTR` | Universal I2C transaction |
| 0x05 | `CMD_ID_SEN_ID` | Get sensor ID |
| 0x10 | `CMD_ID_DEV_MAC` | Get/Set MAC address |
| 0x18 | `CMD_ID_BKEY` | Get/Set bindkey |
| 0x20 | `CMD_ID_COMFORT` | Get/Set comfort parameters |
| 0x22 | `CMD_ID_EXTDATA` | Get/Set external display data |
| 0x23 | `CMD_ID_UTC_TIME` | Get/Set UTC time |
| 0x24 | `CMD_ID_TADJUST` | Get/Set time adjustment |
| 0x25 | `CMD_ID_CFS` | Get/Set sensor config |
| 0x33 | `CMD_ID_MEASURE` | Start/stop measurements |
| 0x35 | `CMD_ID_LOGGER` | Read history |
| 0x36 | `CMD_ID_CLRLOG` | Clear history |
| 0x40 | `CMD_ID_RDS` | Get/Set reed switch config |
| 0x44 | `CMD_ID_TRG` | Get/Set trigger config |
| 0x55 | `CMD_ID_CFG` | Get/Set device config |
| 0x56 | `CMD_ID_CFG_DEF` | Set default config |
| 0x60 | `CMD_ID_LCD_DUMP` | Get/Set LCD buffer |
| 0x70 | `CMD_ID_PINCODE` | Set PIN code |
| 0x72 | `CMD_ID_REBOOT` | Reboot device |
| 0x73 | `CMD_ID_SET_OTA` | Configure extended OTA |

#### Data Structures

##### `dev_services_t`
Device services bitmap.

```c
typedef struct _dev_services_t {
    u32 ota         : 1;    // OTA support
    u32 ota_ext     : 1;    // Extended OTA
    u32 pincode     : 1;    // PIN code support
    u32 bindkey     : 1;    // Encryption support
    u32 history     : 1;    // Data logging
    u32 screen      : 1;    // LCD display
    u32 long_range  : 1;    // LE Long Range
    u32 ths         : 1;    // T&H sensor
    u32 rds         : 1;    // Reed switch
    u32 key         : 1;    // Button support
    u32 out_pins    : 1;    // Output pins
    u32 inp_pins    : 1;    // Input pins
    u32 time_adj    : 1;    // Time adjustment
    u32 hadr_clock  : 1;    // Hardware RTC
    u32 th_trg      : 1;    // T&H triggers
    u32 reserved    : 18;
} dev_services_t;
```

##### `dev_id_t`
Device identification response.

```c
typedef struct _dev_id_t {
    u8  pid;            // Packet ID (CMD_ID_DEV_ID)
    u8  revision;       // Protocol revision
    u16 hw_version;     // Hardware version
    u16 sw_version;     // Software version (BCD)
    u16 dev_spec_data;  // Device-specific data
    u32 services;       // Supported services bitmap
} dev_id_t;
```

#### Functions

##### `void cmd_parser(void *p)`
Parse and execute BLE command.

**Parameters:**
- `p` - Pointer to command data

**Description:** Main command handler for RxTx characteristic writes.

---

##### `u32 find_mi_keys(u16 chk_id, u8 cnt)`
Find Mi keys in flash.

**Parameters:**
- `chk_id` - Key type to find
- `cnt` - Key index

**Returns:** Flash address of key, or 0 if not found

---

##### `u8 get_mi_keys(u8 chk_stage)`
Get Mi keys.

**Parameters:**
- `chk_stage` - Retrieval stage

**Returns:** Key status

---

### Battery Management (battery.h)

Battery voltage monitoring and level calculation.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `MAX_VBAT_MV` | 3000 | 100% battery voltage (mV) |
| `MIN_VBAT_MV` | 2200 | 0% battery voltage (mV) |
| `LOW_VBAT_MV` | 2800 | Low battery threshold (mV) |
| `END_VBAT_MV` | 2000 | Shutdown voltage (mV) |

#### Functions

##### `u16 get_adc_mv(u32 p_ain)`
Read ADC voltage.

**Parameters:**
- `p_ain` - ADC channel

**Returns:** Voltage in millivolts

---

##### `u16 get_battery_mv(void)`
Read battery voltage.

**Returns:** Battery voltage in millivolts

**Description:** Macro that calls `get_adc_mv(SHL_ADC_VBAT)`.

---

##### `u8 get_battery_level(u16 battery_mv)`
Calculate battery percentage.

**Parameters:**
- `battery_mv` - Battery voltage in mV

**Returns:** Battery level 0-100%

---

### Logger/History (logger.h)

Measurement data logging to flash memory.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `MEMO_SEC_ID` | 0x55AAC0DE | Sector header magic |
| `FLASH_ADDR_START_MEMO` | 0x40000 | 512KB flash start |
| `FLASH_ADDR_END_MEMO` | 0x74000 | 512KB flash end |
| `FLASH1M_ADDR_START_MEMO` | 0x80000 | 1MB flash start |
| `FLASH1M_ADDR_END_MEMO` | 0x100000 | 1MB flash end |

#### Data Structures

##### `memo_blk_t`
Single measurement log entry.

```c
typedef struct _memo_blk_t {
    u32 time;   // UTC timestamp
    s16 val1;   // Temperature x0.01 C
    u16 val2;   // Humidity x0.01 %
    u16 val0;   // Battery voltage mV
} memo_blk_t;
```

##### `memo_inf_t`
Logger state information.

```c
typedef struct _memo_inf_t {
    u32 faddr;          // Current flash address
    u32 cnt_cur_sec;    // Entries in current sector
    u32 sectors;        // Total sectors (1MB flash)
    u32 start_addr;     // Log start address
    u32 end_addr;       // Log end address
} memo_inf_t;
```

#### Functions

##### `void memo_init(void)`
Initialize logger.

**Description:** Scans flash for existing logs, prepares for new entries.

---

##### `void clear_memo(void)`
Clear all logged data.

**Description:** Erases all measurement history from flash.

---

##### `unsigned get_memo(u32 bnum, pmemo_blk_t p)`
Read logged entry.

**Parameters:**
- `bnum` - Entry number (0 = newest)
- `p` - Pointer to receive entry

**Returns:** 1 on success, 0 if entry doesn't exist

---

##### `void write_memo(void)`
Write current measurement to log.

**Description:** Saves current measurement data to flash.

---

### Trigger System (trigger.h)

Temperature/humidity threshold triggers and GPIO control.

#### Data Structures

##### `trigger_flg_t`
Trigger state flags.

```c
typedef struct __attribute__((packed)) _trigger_flg_t {
    u8 rds1_input   : 1;    // Reed switch 1 input
    u8 trg_output   : 1;    // Trigger GPIO output value
    u8 trigger_on   : 1;    // Trigger active
    u8 temp_out_on  : 1;    // Temperature trigger fired
    u8 humi_out_on  : 1;    // Humidity trigger fired
    u8 key_pressed  : 1;    // Key pressed
    u8 rds2_input   : 1;    // Reed switch 2 input
} trigger_flg_t;
```

##### `rds_type_t`
Reed switch type configuration.

```c
typedef struct __attribute__((packed)) _rds_type_t {
    u8 type1        : 2;    // RS1 type (RDS_TYPES)
    u8 type2        : 2;    // RS2 type (RDS_TYPES)
    u8 rs1_invert   : 1;    // RS1 invert logic
    u8 rs2_invert   : 1;    // RS2 invert logic
} rds_type_t;
```

##### `trigger_t`
Complete trigger configuration.

```c
typedef struct __attribute__((packed)) _trigger_t {
    s16 temp_threshold;     // Temperature threshold x0.01 C
    s16 humi_threshold;     // Humidity threshold x0.01 %
    s16 temp_hysteresis;    // Temperature hysteresis
    s16 humi_hysteresis;    // Humidity hysteresis
    u16 rds_time_report;    // Reed switch report interval (sec)
    rds_type_t rds;         // Reed switch types
    union {
        trigger_flg_t flg;
        u8 flg_byte;
    };
} trigger_t;
```

#### Functions

##### `void set_trigger_out(void)`
Update trigger output state.

**Description:** Evaluates thresholds and sets trigger GPIO accordingly.

---

##### `void test_trg_on(void)`
Test if triggers should activate.

**Description:** Checks current measurements against thresholds.

---

### Reed Switch Counter (rds_count.h)

Reed switch/door sensor support.

#### Enumerations

##### `RDS_TYPES`
Reed switch operating modes.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `RDS_NONE` | Disabled |
| 1 | `RDS_SWITCH` | Binary switch |
| 2 | `RDS_COUNTER` | Pulse counter |
| 3 | `RDS_CONNECT` | Connect on trigger |

#### Data Structures

##### `rds_count_t`
Reed switch state.

```c
typedef struct _rds_count_t {
    u32 report_tick;    // Report interval timer
    union {
        u8  count1_byte[4];
        u16 count1_short[2];
        u32 count1;     // Pulse counter
    };
    u8 event;           // Current event
} rds_count_t;
```

#### Functions

##### `void rds_init(void)`
Initialize reed switch GPIO.

**Description:** Configures reed switch input pins and pullups.

---

##### `void rds_suspend(void)`
Suspend reed switch monitoring.

**Description:** Disables pullups for low power sleep.

---

##### `void rds_task(void)`
Reed switch polling task.

**Description:** Checks reed switch state and generates events.

---

##### Inline Functions

```c
static inline u8 get_rds1_input(void)
```
Get reed switch 1 state (with inversion).

```c
static inline void rds1_input_on(void)
```
Enable reed switch 1 pullup.

```c
static inline void rds1_input_off(void)
```
Disable reed switch 1 pullup.

```c
static inline void rds_input_on(void)
```
Enable all reed switch pullups.

---

### External OTA (ext_ota.h)

Extended over-the-air firmware update support.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `ID_BOOTABLE` | 0x544C4E4B | "TLNK" bootable signature |

#### Enumerations

##### `EXT_OTA_ENUM`
Extended OTA return codes.

| Value | Constant | Description |
|-------|----------|-------------|
| 0 | `EXT_OTA_OK` | Success |
| 1 | `EXT_OTA_WORKS` | In progress |
| 2 | `EXT_OTA_BUSY` | Flash busy |
| 3 | `EXT_OTA_READY` | Ready for data |
| 4 | `EXT_OTA_EVENT` | Event occurred |
| 0xFE | `EXT_OTA_ERR_PARM` | Invalid parameter |

#### Data Structures

##### `ext_ota_t`
Extended OTA state.

```c
typedef struct _ext_ota_t {
    u32 start_addr;     // OTA start address
    u32 ota_size;       // Size in KB
    u32 check_addr;     // Current check/erase address
} ext_ota_t;
```

#### Functions

##### `void tuya_zigbee_ota(void)` / `void big_to_low_ota(void)`
Process Zigbee/Big OTA image.

**Description:** Handles large OTA images stored in upper flash.

---

##### `u32 get_mi_hw_version(void)`
Get Mi hardware version from flash.

**Returns:** Hardware version code

---

##### `void set_SerialStr(void)`
Set device serial number string.

**Description:** Generates serial number from MAC address.

---

##### `u8 check_ext_ota(u32 ota_addr, u32 ota_size)`
Validate extended OTA parameters.

**Parameters:**
- `ota_addr` - Proposed OTA address
- `ota_size` - Proposed OTA size in KB

**Returns:** EXT_OTA_ENUM status code

---

##### `void clear_ota_area(void)`
Erase OTA flash area.

**Description:** Incrementally erases flash sectors for OTA.

---

### Cryptography (ccm.h)

AES-CCM encryption for beacon data.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `CCM_ENCRYPT` | 0 | Encryption mode |
| `CCM_DECRYPT` | 1 | Decryption mode |

#### Functions

##### `int ccm_auth_crypt(int mode, const unsigned char *key, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len, const unsigned char *input, size_t length, unsigned char *output, unsigned char *tag, size_t tag_len)`
Perform CCM encryption or decryption.

**Parameters:**
- `mode` - CCM_ENCRYPT or CCM_DECRYPT
- `key` - 16-byte AES key (bindkey)
- `iv` - Nonce/IV
- `iv_len` - Nonce length (2-8 bytes)
- `add` - Additional authenticated data
- `add_len` - AAD length
- `input` - Input plaintext/ciphertext
- `length` - Input length
- `output` - Output buffer
- `tag` - Authentication tag buffer
- `tag_len` - Tag length (4, 6, 8, 10, 14, or 16)

**Returns:** 0 on success

---

##### `inline int aes_ccm_encrypt_and_tag(...)`
CCM encryption with tag generation.

**Parameters:** Same as `ccm_auth_crypt` (without mode)

**Returns:** 0 on success

---

##### `int aes_ccm_auth_decrypt(...)`
CCM decryption with tag verification.

**Parameters:** Same as `ccm_auth_crypt` (without mode)

**Returns:** 0 on success, error if tag mismatch

---

### BME280 Sensor (bme280.h)

Bosch BME280 pressure/temperature/humidity sensor driver.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `BME280_I2C_ADDR` | 0x76 | Default I2C address |
| `BME280_ID` | 0x60 | Chip ID |

#### Register Addresses

| Register | Address | Description |
|----------|---------|-------------|
| `BME280_RA_CHIPID` | 0xD0 | Chip ID register |
| `BME280_RA_SOFTRESET` | 0xE0 | Soft reset register |
| `BME280_RA_CTRL_HUMI` | 0xF2 | Humidity control |
| `BME280_RA_CTRL_MEAS` | 0xF4 | Measurement control |
| `BME280_RA_PRESSURE` | 0xF7 | Pressure data (3 bytes) |
| `BME280_RA_TEMP` | 0xFA | Temperature data (3 bytes) |
| `BME280_RA_HUMI` | 0xFD | Humidity data (2 bytes) |

#### Data Structures

##### `bme280_dig_t`
BME280 calibration data.

```c
typedef struct {
    u16 T1; s16 T2; s16 T3;         // Temperature cal
    u16 P1; s16 P2-P9;              // Pressure cal
    u8 H1; s16 H2; u8 H3;           // Humidity cal
    s16 H4; s16 H5; s8 H6;
} bme280_dig_t;
```

---

### DS18B20 Sensor (my18b20.h)

Dallas DS18B20 1-Wire temperature sensor driver.

#### Data Structures

##### `my18b20_coef_t`
DS18B20 calibration coefficients.

```c
typedef struct _my18b20_coef_t {
    u32 val1_k;     // Temperature multiplier
    s16 val1_z;     // Temperature offset
    // val2_k/val2_z for dual sensor
} my18b20_coef_t;
```

##### `my18b20_t`
DS18B20 sensor state.

```c
typedef struct {
    my18b20_coef_t coef;    // Calibration
    u32 id;                 // ROM ID
    u8  res;                // Resolution
    u8  type;               // Sensor type
    u8  rd_ok;              // Read success flag
    u8  stage;              // State machine stage
    u32 tick;               // Timing counter
    u32 timeout;            // Conversion timeout
    s16 temp[N];            // Temperature readings
} my18b20_t;
```

#### Functions

##### `void init_my18b20(void)`
Initialize DS18B20 sensor.

**Description:** Searches for and configures DS18B20 sensors on 1-Wire bus.

---

##### `void task_my18b20(void)`
DS18B20 polling task.

**Description:** State machine for temperature conversion and reading.

---

### HX71X Scale Sensor (hx71x.h)

HX711/HX710 load cell ADC driver.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `MAX_TANK_VOLUME_10ML` | 32768 | Max volume (327 liters) |

#### Enumerations

##### `hx71x_mode_t`
HX71X gain/channel modes.

| Value | Constant | Description |
|-------|----------|-------------|
| 25 | `HX71XMODE_A128` | Channel A, gain 128 |
| 26 | `HX71XMODE_B32` | Channel B, gain 32 |
| 27 | `HX71XMODE_A64` | Channel A, gain 64 |

#### Data Structures

##### `hx71x_cfg_t`
HX71X configuration.

```c
typedef struct _hx71x_cfg_t {
    u32 zero;           // Zero offset
    u32 coef;           // Scale coefficient
    u32 volume_10ml;    // Full tank volume
} hx71x_cfg_t;
```

##### `hx71x_t`
HX71X state.

```c
typedef struct _hx71x_t {
    hx71x_cfg_t cfg;    // Configuration
    u32 adc;            // Raw ADC value
    u32 value;          // Scaled value
    u32 summator;       // Averaging accumulator
    u32 count;          // Sample count
    u32 calcoef;        // Calibration coefficient
} hx71x_t;
```

#### Functions

##### `int hx71x_get_data(hx71x_mode_t mode)`
Read HX71X ADC value.

**Parameters:**
- `mode` - Gain/channel mode

**Returns:** 24-bit ADC value, or negative on error

---

##### `void hx71x_calibration(void)`
Calibrate scale to zero.

**Description:** Sets current reading as zero point.

---

##### `u16 hx71x_get_volume(void)`
Calculate liquid volume.

**Returns:** Volume in 10mL units

---

##### `void hx71x_task(void)`
HX71X polling task.

**Description:** Reads and averages scale measurements.

---

##### Inline Functions

```c
inline void hx711_go_sleep(void)
```
Put HX711 in power-down mode.

```c
inline void hx711_gpio_wakeup(void)
```
Wake HX711 from power-down.

---

## Python Interface API

### Construct Definitions (atc_mi_construct.py)

BLE advertising format definitions using the `construct` library.

#### Advertising Formats

##### `custom_format`
PVVX custom format (unencrypted).

**Fields:**
- `version` - Format version (1)
- `size` - Packet size (18)
- `uid` - 0x16 (Service Data)
- `UUID` - 0x181A (Environmental Sensing)
- `MAC` - Device MAC address (reversed)
- `mac_vendor` - Vendor lookup
- `temperature` - Temperature in 0.01C
- `humidity` - Humidity in 0.01%
- `battery_v` - Battery voltage in 0.001V
- `battery_level` - Battery 0-100%
- `counter` - Frame counter
- `flags` - Trigger flags (atc_flag)

##### `custom_enc_format`
PVVX custom format (encrypted).

Uses `AtcMiCodec` for encryption/decryption with bindkey.

##### `atc1441_format`
ATC1441 legacy format (unencrypted).

**Note:** Uses mixed endianness - MAC and temperature are big-endian!

##### `atc1441_enc_format`
ATC1441 format (encrypted).

Temperature range: -40 to 87C with 0.5C precision.

##### `mi_like_format`
Xiaomi Mi format (clear or encrypted).

**Frame Control Bits:**
- `Mesh` - Mesh network flag
- `Object_Include` - Data objects included
- `Capability_Include` - Capability info
- `MAC_Include` - MAC in packet
- `isEncrypted` - Encryption flag
- `version` - Protocol version
- `registered` - Device bound flag

##### `bt_home_format`
BTHome v1 format (unencrypted).

UUID: 0x181C (User Data Service)

##### `bt_home_enc_format`
BTHome v1 format (encrypted).

UUID: 0x181E (Bond Management Service)

##### `bt_home_v2_format`
BTHome v2 format (recommended).

UUID: 0xFCD2 (BTHome Service)

**DevInfo Byte:**
- `Version` - BTHome version (2)
- `Trigger` - Trigger-based advertising
- `Encryption` - Encryption enabled

##### `general_format`
Combined format that attempts to parse all types.

#### Configuration Structures

##### `cfg`
Device configuration structure.

**Fields:**
- `firmware_version` - Major.Minor version
- `flg` - Feature flags (advertising type, display options)
- `flg2` - Extended flags (smiley, crypto, longrange)
- `temp_offset` - Temperature calibration offset
- `humi_offset` - Humidity calibration offset
- `advertising_interval` - Interval in 62.5ms units
- `measure_interval` - Measurement multiplier
- `rf_tx_power` - TX power setting
- `connect_latency` - Connection latency
- `min_step_time_update_lcd` - LCD update rate
- `hw_cfg` - Hardware configuration
- `averaging_measurements` - Averaging count

##### `comfort_values`
Comfort zone thresholds.

##### `trigger`
Trigger configuration including thresholds and reed switch settings.

##### `mac_address`
MAC address with random suffix.

##### `token_bind_mi_keys`
Mi encryption keys (Token + Bind).

---

### Construct Adapters (atc_mi_construct_adapters.py)

Custom adapters for parsing and building advertising data.

#### Classes

##### `BtHomeCodec(Tunnel)`
Encryption codec for BTHome v1.

**Constructor Parameters:**
- `subcon` - Subconstruct for data
- `bindkey` - 16-byte encryption key (optional)
- `mac_address` - Device MAC (optional)

**Methods:**
- `decrypt(ctx, nonce, encrypted_data, mic, update)` - Decrypt data
- `encrypt(ctx, nonce, msg, update)` - Encrypt data
- `_decode(obj, ctx, path)` - Parse encrypted packet
- `_encode(obj, ctx, path)` - Build encrypted packet

##### `BtHomeV2Codec(BtHomeCodec)`
Encryption codec for BTHome v2.

**Differences from v1:**
- Includes device_info byte in nonce
- Different UUID (0xFCD2)

##### `AtcMiCodec(BtHomeCodec)`
Encryption codec for PVVX/ATC1441 encrypted formats.

**Nonce construction:** `MAC_reversed + header_bytes + counter`

##### `MiLikeCodec(BtHomeCodec)`
Encryption codec for Xiaomi Mi format.

**Nonce construction:** `MAC_reversed + dev_id + frame_cnt + count_id`

#### Adapters

##### `MacAddress`
Standard MAC address adapter (MSB first).

```python
MacAddress = ExprAdapter(Byte[6],
    decoder = lambda obj, ctx: ":".join("%02x" % b for b in obj).upper(),
    encoder = lambda obj, ctx: bytes.fromhex(re.sub(r'[.:\- ]', '', obj))
)
```

##### `ReversedMacAddress`
Reversed MAC address adapter (LSB first).

##### Numeric Adapters

| Adapter | Description |
|---------|-------------|
| `Int16ul_x1000` | Unsigned 16-bit / 1000 |
| `Int16ul_x100` | Unsigned 16-bit / 100 |
| `Int16ul_x10` | Unsigned 16-bit / 10 |
| `Int16sl_x100` | Signed 16-bit / 100 |
| `Int16sl_x10` | Signed 16-bit / 10 |
| `Int24ul_x100` | Unsigned 24-bit / 100 |
| `Int24ul_x1000` | Unsigned 24-bit / 1000 |

#### Functions

##### `normalize_report(report)`
Clean up construct parsing output.

**Parameters:**
- `report` - Raw construct output string

**Returns:** Cleaned report string

---

### Configuration Tool (atc_mi_config.py)

BLE device configuration utility.

#### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `notify_uuid` | "00001f10-..." | Service UUID |
| `characteristic_uuid` | "00001f1f-..." | Characteristic UUID |
| `BC_TIMEOUT` | 40.0 | BLE connection timeout |
| `SLEEP_TIMEOUT` | 0.1 | Inter-command delay |

#### Command IDs

| ID | Constant | Description |
|----|----------|-------------|
| 0x01 | `CMD_ID_DNAME` | Device name |
| 0x02 | `CMD_ID_GDEVS` | Internal devices |
| 0x03 | `CMD_ID_I2C_SCAN` | I2C scan |
| 0x05 | `CMD_ID_SEN_ID` | Sensor ID |
| 0x10 | `CMD_ID_DEV_MAC` | MAC address |
| 0x20 | `CMD_ID_COMFORT` | Comfort parameters |
| 0x23 | `CMD_ID_UTC_TIME` | UTC time |
| 0x44 | `CMD_ID_TRG` | Trigger data |
| 0x55 | `CMD_ID_CFG` | Configuration |

#### Functions

##### `async atc_characteristics(client, verbosity=False)`
Read device characteristics.

**Parameters:**
- `client` - BleakClient instance
- `verbosity` - Print debug output

**Returns:** List of [handle, description, data] tuples

---

##### `get_display_report(editing_structure, tag)`
Generate display-friendly report.

**Parameters:**
- `editing_structure` - Command structure dictionary
- `tag` - Data tag ("binary", "sample_binary")

**Returns:** Formatted report string

---

##### `get_editable_report(editing_structure, tag)`
Generate editable parameter report.

**Parameters:**
- `editing_structure` - Command structure dictionary
- `tag` - Data tag

**Returns:** Report with `path = value` format

---

##### `traverse_construct(construct, path, struct)`
Recursively traverse construct structure.

**Parameters:**
- `construct` - Construct definition
- `path` - Current path string
- `struct` - Parsed structure

**Returns:** Report string

---

##### `gui_edit(editing_structure, args)`
Open GUI configuration editor.

**Parameters:**
- `editing_structure` - Command structure dictionary
- `args` - Command line arguments

**Description:** Opens wxPython GUI for editing configuration.

---

##### `edit_value(editing_structure, editable, tag, new_tag)`
Edit a single configuration value.

**Parameters:**
- `editing_structure` - Command structure dictionary
- `editable` - "path = value" string
- `tag` - Source data tag
- `new_tag` - Destination data tag

**Returns:** True on success

---

##### `async atc_mi_configuration(args)`
Main configuration coroutine.

**Parameters:**
- `args` - Parsed command line arguments

**Returns:** (success, editing_structure, data_out) tuple

---

##### `main()`
Command-line entry point.

**Arguments:**
- `-m MAC` - Device MAC address (required)
- `-i` - Show device information
- `-c` - Show characteristics
- `-g` - GUI editor
- `-E [values]` - Edit values
- `-D` - Set device date
- `-d` - Read date
- `-R` - Reset to defaults
- `-b` - Reboot device
- `-v` - Verbose output

---

### BLE Advertising (atc_mi_advertising.py)

GUI BLE advertisement browser.

#### Classes

##### `AtcMiConstructFrame(wx.Frame)`
Main application window.

**Constructor Parameters:**
- `maximized` - Start maximized
- `loadfile` - Log file to load
- `ble_start` - Auto-start BLE scanning

##### `AtcMiBleakScannerConstruct(BleakScannerConstruct)`
BLE scanner with format detection.

**Methods:**

###### `bleak_advertising(device, advertisement_data)`
Handle received advertisement.

**Parameters:**
- `device` - BLE device object
- `advertisement_data` - Advertisement payload

**Description:** Parses advertisement, identifies format, and displays in GUI.

#### Functions

##### `main()`
Application entry point.

**Arguments:**
- `-s` - Start BLE scanning immediately
- `-m` - Maximize window
- `-l FILE` - Load log file
- `-i` - Enable inspection
- `-V` - Print version

---

### Advertisement Format (atc_mi_adv_format.py)

Advertisement format detection.

#### Data Structures

##### `gatt_dict`
Format detection dictionary.

| Key | GATT UUID | Length | Header |
|-----|-----------|--------|--------|
| `atc1441` | 0x181A | 13 | 16 1A 18 |
| `custom` | 0x181A | 15 | 16 1A 18 |
| `custom_enc` | 0x181A | 11 | 16 1A 18 |
| `atc1441_enc` | 0x181A | 8 | 16 1A 18 |
| `mi_like` | 0xFE95 | varies | 16 95 FE |
| `bt_home` | 0x181C | varies | 16 1C 18 |
| `bt_home_enc` | 0x181E | varies | 16 1E 18 |
| `bt_home_v2` | 0xFCD2 | varies | 16 D2 FC |

#### Functions

##### `atc_mi_advertising_format(advertisement_data)`
Detect advertisement format.

**Parameters:**
- `advertisement_data` - BLE advertisement data

**Returns:** (format_name, raw_bytes) tuple

**Description:** Matches advertisement against known formats by GATT UUID and packet length.

---

## Appendix

### Supported Devices

| Device | Hardware Version | Sensor | Display |
|--------|-----------------|--------|---------|
| LYWSD03MMC B1.4 | 0 | SHTC3 | LCD |
| LYWSD03MMC B1.5 | 10 | SHTC3 | LCD |
| LYWSD03MMC B1.6 | 4 | SHT4x | LCD |
| LYWSD03MMC B1.7 | 5 | SHT4x | LCD |
| LYWSD03MMC B1.9 | 3 | SHT4x | LCD |
| MHO-C401 | 1 | SHTC3 | E-ink |
| MHO-C401N 2022 | 8 | SHTC3 | E-ink |
| MHO-C122 | 11 | SHTC3 | LCD |
| CGG1 | 2 | SHTC3 | E-ink |
| CGG1 2022 | 7 | SHTC3 | E-ink |
| CGDK2 | 6 | SHTC3 | LCD |
| MJWSD05MMC | 9 | SHT4x | LCD |
| MJWSD05MMC EN | 12 | SHT4x | LCD |
| MJWSD06MMC | 13 | - | LCD |

### BLE UUID Reference

| UUID | Service |
|------|---------|
| 0x1800 | Generic Access |
| 0x1801 | Generic Attribute |
| 0x180A | Device Information |
| 0x180F | Battery Service |
| 0x181A | Environmental Sensing |
| 0x181C | User Data |
| 0x181E | Bond Management |
| 0xFCD2 | BTHome |
| 0xFE95 | Xiaomi Inc. |
| 0x1F10 | Custom RxTx Service |

### Error Codes

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | I2C communication error |
| 2 | Sensor not found |
| 3 | Invalid parameter |
| 4 | Flash write error |
| 5 | Encryption error |

---

*Documentation generated for ATC_MiThermometer firmware*
*Version: 4.x*
*Author: pvvx and contributors*
