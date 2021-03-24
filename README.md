# Xiaomi Mijia (LYWSD03MMC) & Xiaomi Miaomiaoce (MHO-C401) & CGG1 Mijia Bluetooth Thermometer Firmware

This repository contains custom firmware for two Xiaomi Mijia Smart Bluetooth Thermometer & Hygrometer devices.

The custom firmware can be flashed _via a modern browser_ and _over-the-air (OTA)_ without opening the device and unlocks several customization options. You can go back to the original firmware at any time.

**Key features**

* Supports connections using **PIN-code** and encrypted **bindkey** beacon.
* **3 LCD Display Screens** (Looping): Temperature & Humidity & Comfort, Temperature & Battery Level, Clock
* **Measurement values recording** & Charting. See [Reading Measurements from Flash](#reading-measurements-from-flash)
* **Adjustable correction offsets** and **Comfort zones**
* Improved battery life
* Concurrent support for Xiaomi, ATC and Custom Bluetooth Advertisement format
* Adjustable RF TX Power & Bluetooth advertising interval

|Xiaomi Mijia (LYWSD03MMC) | [Xiaomi Miaomiaoce (MHO-C401)](https://pvvx.github.io/MHO_C401) | [Qingping Temp & RH Monitor (CGG1-Mijia)](https://pvvx.github.io/CGG1) |
|:--:|:--:|:--:|
|  <img src="https://tasmota.github.io/docs/_media/bluetooth/LYWSD03MMC.png" alt="Xiaomi Mijia (LYWSD03MMC)" width="160"/> |  <img src="https://tasmota.github.io/docs/_media/bluetooth/MHO-C401.png" alt="Xiaomi Miaomiaoce (MHO-C401)" width="160"/> | <img src="https://pvvx.github.io/CGG1/img/CGG1.png" alt="E-ink CGG1 'Qingping Temp & RH Monitor', Xiaomi Mijia DevID: 0x0B48" width="160"/> |

**Table of content**

<!-- TOC depthFrom:2 depthTo:3 -->

- [Getting Started](#getting-started)
    - [Flashing or Updating the Firmware (OTA)](#flashing-or-updating-the-firmware-ota)
    - [Configuration](#configuration)
- [Firmware](#firmware)
    - [Firmware Binaries](#firmware-binaries)
    - [Firmware version history](#firmware-version-history)
- [Applications](#applications)
    - [Reading Measurements from Flash](#reading-measurements-from-flash)
    - [Reading Measurements in Connected Mode](#reading-measurements-in-connected-mode)
    - [Reading Measurements in Advertising Mode](#reading-measurements-in-advertising-mode)
- [Technical specifications](#technical-specifications)
    - [Average power consumption](#average-power-consumption)
    - [Bluetooth Advertising Formats](#bluetooth-advertising-formats)
    - [Bluetooth Connection Mode](#bluetooth-connection-mode)
    - [Temperature or humidity trigger on GPIO PA5 (label on the "reset" pin)](#temperature-or-humidity-trigger-on-gpio-pa5-label-on-the-reset-pin)
    - [Interface for receiving and displaying data on the LCD.](#interface-for-receiving-and-displaying-data-on-the-lcd)
    - [The USB-COM adapter writes the firmware in explorer. Web version.](#the-usb-com-adapter-writes-the-firmware-in-explorer-web-version)
- [Related Work](#related-work)
- [Resources & Links](#resources--links)
    - [CJMCU-2557 BQ25570](#cjmcu-2557-bq25570)

<!-- /TOC -->


## Getting Started 

You can conveniently flash, update and configure the bluetooth thermometers remotely using a bluetooth connection and a modern web browser.

### Flashing or Updating the Firmware (OTA)

To flash or update the firmware, use a Google Chrome, Microsoft Edge or Opera Browser.

1. Go to the [Over-the-air Webupdater Page `TelinkMiFlasher.html`](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html) *
2. If using Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. `chrome://flags/#enable-experimental-web-platform-features` for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then **restart the browser**.
3. In the Telink Flasher Page: Press `Connect`: The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. `LYWSD03MMC`) to pair. 
4. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
5. Now you can press the _Custom Firmware ver x.x_ button to directly flash the custom firmware. Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser
6. Press _Start Flashing_.

 * Use [TelinkOTA](https://pvvx.github.io/ATC_MiThermometer/TelinkOTA.html) to flash old or alternative versions ([ATC1441](https://atc1441.github.io/TelinkFlasher.html)).

### Configuration
After you have flashed the firmware, the device has changed it's bluetooth name to something like `ATC_F02AED`. Using the [`TelinkMiFlasher.html`](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html) you have various configuration options.

**General process:**
1. Press _Connect_
2. Select the according device to connect/pair (i.e.  `ATC_F02AED`)
3. Several configuration options appear
4. Choose _Send Config_ to send changed value to the device. Or press _Set default_ and then _Send config_ to revert to the Defaults

| Option | Description |
| ------ | ----------- |
| _Temperature and Humidity offset_ | Enter a value to correct the offset of the Temperature or Humidity displayed: For example `-1.4` will decrease the Temperature by 1.4°
|  _Smiley or Comfort_ | Choose a static smiley or check the "Comfort" Radio box to change the smiley depending on current Temperature and Humidity. |
| _Comfort Parameters_ | Defines the Lower (Lo) and Upper (Hi) Range for Temperature and Humidity interpreted as comfort zone. In the default configuration a smiley will appear.
| _Advertising Type_ | Type of supported [Bluetooth Advertising Formats](#bluetooth-advertising-formats). By default all formats are enabled.
| _Set time_ | sends the current time to the device
| _Comfort, Show batt, Clock_ | Ticking the according boxes you can enable interval rotation between different LCD screens. See the example video below.


**Example of LCD display modes**

You can configure different LCD Display modes using _Comfort, Show batt_ and _Clock_ configuration checkboxes. The enabled LCD Display Modes will appear one-by-one in a loop. 

[![YoutubeVideo](https://img.youtube.com/vi/HzYh1vq8ikM/0.jpg)](https://youtu.be/HzYh1vq8ikM)

Battery and clock display are enabled in the settings. The rest of the settings is kept default. The video contains 2 cycles.

> 1. Temperature and humidity
> 2. Temperature and % of battery
> 3. Temperature and humidity
> 4. Hours and minutes


## Firmware
### Firmware Binaries
You can directly update/flash the firmware without downloading the binaries below.

**Custom Firmware Versions:**

* [LYWSD03MMC Custom Firmware Version 2.8](https://github.com/pvvx/ATC_MiThermometer/raw/master/ATC_Thermometer28c.bin)
* [MHO-C401 Custom Firmware Version 2.8](https://github.com/pvvx/ATC_MiThermometer/raw/master/MHO_C401_v28c.bin)
* [CGG1 Custom Firmware Version 2.8](https://github.com/pvvx/ATC_MiThermometer/raw/master/CGG1_v28c.bin)

**Original Manufacturer Firmware Version**

In case you want to go back to the original firmware, you can download them here:

* [LYWSD03MMC Original Firmware v1.0.0_0109](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_LYWSD03MMC_v1.0.0_0109.bin)
* [MHO-C401 Original Firmware v1.0.0_0010](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_MHO_C401_v1.0.0_0010.bin)
* [CGG1 Original Firmware v1.0.1_0093](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_CGG1_v1.0.1_0093.bin)

### Firmware version history

| Version | Changes |
| ------: | ------- |
|     1.2 | Bind, Set Pin-code, Support MHO-C401 |
|     1.3 | Get/Set comfort parameters |
|     1.4 | Get/Set device name, Get/Set MAC |
|     1.5 | Add Standard Device Information Characteristics
|     1.6 | Fix alternation of Advertising in mi mode
|     1.7 | Authorization and encryption in permissions to access GAP ATT attributes, if pin code is enabled
|     1.8 | Time display (instead of a blinking smile)
|     1.9 | Recording measurements to flash memory (cyclic buffer for 19632 measurements)
|     2.0 | Recording measurements with averaging to flash memory
|     2.1 | Periodic display refresh for MHO-C401 <br> 'Erase mi-keys' option to return to original firmware
|     2.2 | Added parameter "Encrypted Mi Beacon"
|     2.3 | Added "Delete all records"
|     2.4 | Added parameter "Clock time step"
|     2.5 | Remove TRG/FLG errors, minor optimization
|     2.6 | Expanding the ranges of threshold parameters (TRG)
|     2.7 | Reducing power consumption of MHO-C401 (EDP update), adding version for CGG1
|     2.8 | Added saving bindkey to EEP if mi-keys are erased, reduced TX power to 0 dB for defaults.


## Applications

### Reading Measurements from Flash
[GraphMemo.html](https://pvvx.github.io/ATC_MiThermometer/GraphMemo.html)

![FlashData](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/FlashData.gif)

### Reading Measurements in Connected Mode
[GraphAtc.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc.html)
[GraphAtc1.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc1.html)
[GraphAtc2.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc2.html)
[DevPoint.html](https://pvvx.github.io/ATC_MiThermometer/DevPoint.html)

![GraphAtc_html](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/GraphAtc_html.gif) 

### Reading Measurements in Advertising Mode
This requires the _Experimental Web Platform Features_ flag enabled in your browser. See [Flashing or Updating the Firmware (OTA)](#flashing-or-updating-the-firmware-ota).

[Advertising.html](https://pvvx.github.io/ATC_MiThermometer/Advertising.html)

## Technical specifications

### Average power consumption

**Xiaomi Mijia (LYWSD03MMC)**

Using the default settings for advertising interval of 2.5 seconds
and measurement interval of 10 seconds:

* Bluetooth Advertisement: 15..20 uA 3.3V ([CR2032](https://pvvx.github.io/ATC_MiThermometer/CustPower.html) [over 6 months](https://github.com/pvvx/ATC_MiThermometer/issues/23#issuecomment-766898945))
* Bluetooth Connection: 14..25 uA 3.3V (CR2032 over 6 months)


**Xiaomi Miaomiaoce (MHO-C401)**

Using the default settings for advertising interval of 2.5 seconds and measurement interval of 20 seconds:

* Bluetooth Advertisement: 12..30 uA 3.3V ([depends on the amount of temperature or humidity changes over time to display](https://pvvx.github.io/MHO_C401/power_altfw.html))
* Bluetooth Connection: 15..30 uA 3.3V (depends on the amount of temperature or humidity changes over time to display)

### Bluetooth Advertising Formats
The Firmware can be configured to support one of four different Bluetooth advertisements data formats. Supports bindkey beacon encryption.

You can also configure to transferring everything in turn (round-robin)
#### atc1441 format
UUID 0x181A - size 16: [atc1441 format](https://github.com/atc1441/ATC_MiThermometer#advertising-format-of-the-custom-firmware) 

#### Custom format (all data little-endian):  
UUID 0x181A - size 19: Custom format (all data little-endian):  

   ```c
   uint8_t     size;   // = 19
   uint8_t     uid;    // = 0x16, 16-bit UUID
   uint16_t    UUID;   // = 0x181A, GATT Service 0x181A Environmental Sensing
   uint8_t     MAC[6]; // [0] - lo, .. [6] - hi digits
   int16_t     temperature;    // x 0.01 degree
   uint16_t    humidity;       // x 0.01 %
   uint16_t    battery_mv;     // mV
   uint8_t     battery_level;  // 0..100 %
   uint8_t     counter;        // measurement count
   uint8_t     flags;  // GPIO_TRG pin (marking "reset" on circuit board) flags: 
                       // bit0: GPIO_TRG pin input value (real level)
                       // bit1: GPIO_TRG pin output value (pull Up/Down)
                       // bit2: Output GPIO_TRG pin is controlled according to the set parameters
                       // bit3: Temperature trigger event
                       // bit4: Humidity trigger event
   ```

#### Xiaomi Battery Charge
3. UUID 0xFE95 - 0x0A: [Xiaomi](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/README.md) - battery charge level 0..100%, battery voltage in mV

#### Xiaomi Temperature & Humidity
4. UUID 0xFE95 - 0x0D: [Xiaomi](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/README.md) - temperature x0.1C, humidity x0.1%

#### Xiaomi Encrypted (bindkey enable)
5. UUID 0xFE95 - 0x0A, 0x04, 0x06 [Xiaomi](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/Mijia%20BLE%20Object%20Definition.md) - battery charge level 0..100%, temperature x0.1C, humidity x0.1% (All data are averaged over a period of 16 measurements)

### Bluetooth Connection Mode
+ Primary Service - Environmental Sensing Service (0x181A):
 * Characteristic UUID 0x2A1F - Notify temperature x0.1C
 * Characteristic UUID 0x2A6E - Notify temperature x0.01C
 * Characteristic UUID 0x2A6F - Notify about humidity x0.01%
+ Primary Service - Battery Service (0x180F):
 * Characteristic UUID 0x2A19 - Notify the battery charge level 0..99%
+ Primary Service (0x1F10):
 * Characteristic UUID 0x1F1F - Notify, frame id 0x33 (configuring or making a request): temperature x0.01C, humidity x0.01%, battery charge level 0..100%, battery voltage in mV, GPIO-pin flags and triggers.

### Temperature or humidity trigger on GPIO PA5 (label on the "reset" pin)
![trg_menu](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/trg_menu.gif)

![trg_grf](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/trg_grf.gif)

Hysteresis: 
> * =0 off, 
> * if less than zero - activation on decrease, 
> * if more than zero - activation on excess

Setting the pin to "1" or "0" works if both hysteresis are set to zero (TRG off). 

![OnOff](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/OnOff.gif)

### Interface for receiving and displaying data on the LCD.
>* LCD shows: 
> * Big number: -99.5..1999.5 
> * Small number: -9..99
> * Smiley, battery, degrees
> * Setting the display time limit in sec

![ShowData](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/ShowData.gif) 

### The USB-COM adapter writes the firmware in explorer. Web version.
>* Connect only TX-SWS and GND wires.

[USBCOMFlashTx.html](https://pvvx.github.io/ATC_MiThermometer/USBCOMFlashTx.html)

![USBCOMFlashTxHtml](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/USBCOMFlashTxHtml.gif)


#### Chipset
> * TLSR8251F512ET24 (TLSR8258 in 24-pin TQFN). SoC: TC32 32-bit MCU 48Mhz, 64 KiB SRAM, 512 KiB Flash (GD25LE40C), Bluetooth 5.0: Mesh, 6LoWPAN, Zigbee, RF4CE, HomeKit, Long Range, Operating temperature: -40°C to +85°C, Power supply: 1.8V to 3.6V.
> * SHTV3 sensor. Measurement range: Temperature -40°C to +125°C, Humidity 0 to 100 %RH. Power supply: 1.8V to 3.6V
> * IST3055NA0 LCD controller 

[LYWSD03MMC BoardPinout](https://github.com/pvvx/ATC_MiThermometer/blob/master/BoardPinout)

![TabPins](https://github.com/pvvx/ATC_MiThermometer/blob/master/BoardPinout/TabPins.gif)

#### Building the firmware

1. Go to [wiki.telink-semi.cn](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/IDE-for-TLSR8-Chips/) and get the IDE for TLSR8 Chips.
2. Clone https://github.com/Ai-Thinker-Open/Telink_825X_SDK
3. Install the IDE and import the 'ATC_MiThermometer' project
4. Change 'Linked resource' and 'C/C++ Build/Build command'. 
5. Compile the project


## Related Work

ATC_MiThermometer is based on the original work of [@atc1441](https://twitter.com/atc1441), who developed the [initial custom firmware version and the web-based OTA flasher (Source)](https://github.com/atc1441/ATC_MiThermometer).



## Resources & Links
* [Telink Linux Tool Chain 2020-06-26](https://yadi.sk/d/pt_qTBB-t24i9A)

* [MHO-C401 Info](https://pvvx.github.io/MHO_C401/)

* [CGG1-Mijia Info](https://pvvx.github.io/CGG1)

* [Build Telink EVK on E104-BT10-G/N module (TLSR8269)](https://github.com/pvvx/TLSR8269-EVK)

* [Alternative programmer TLSR SWire on module TB-04/03F or E104-BT10](https://github.com/pvvx/TLSRPGM)

* [Additional information on the format Advertising for Xiaomi LYWSD03MMC](https://github.com/Magalex2x14/LYWSD03MMC-info)

* [Reading and recovering Mi-Home keys](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/)

* [HTML files](https://github.com/pvvx/pvvx.github.io)

* [LYWSD03MMC Forum link (russian)](https://esp8266.ru/forum/threads/tlsr8251-lcd-termometr-lywsd03mmc-xiaomi-bluetooth-termometr.5263/)

* [MHO-C401 Forum link (russian)](https://esp8266.ru/forum/threads/tlsr8251f512et24-e-inc-display-termometr-mho-c401-bluetooth-termometr.5446/)

### CJMCU-2557 BQ25570

![CJMCU-2557](https://raw.githubusercontent.com/pvvx/pvvx.github.io/master/ATC_MiThermometer/img/CJMCU-2557.jpg)

## Control function ID when connected

#### Primary Service UUID 0x1F10, Characteristic UUID 0x1F1F

|  ID  | Command                                       |
| :--: | --------------------------------------------- |
| 0x01 | Get/Set device name                           |
| 0x10 | Get/Set MAC                                   |
| 0x11 | Get/Set Mi key: DevNameID                     |
| 0x12 | Get/Set Mi keys: Token & Bind                 |
| 0x15 | Get all Mi keys                               |
| 0x16 | Restore prev mi token & bindkeys              |
| 0x17 | Delete all Mi keys                            |
| 0x18 | Get/set binkey in EEP                         |
| 0x20 | Get/Set comfort parameters                    |
| 0x22 | Get/Set show LCD ext.data                     |
| 0x23 | Get/Set Time                                  |
| 0x24 | Get/set adjust time clock delta               |
| 0x33 | Start/Stop notify measures in connection mode |
| 0x35 | Read memory measures                          |
| 0x36 | Clear memory measures                         |
| 0x44 | Get/Set TRG config                            |
| 0x45 | Set TRG output pin                            |
| 0x4A | Get/Set TRG data (not save to Flash)          |
| 0x55 | Get/Set device config                         |
| 0x56 | Set default device config                     |
| 0x5A | Get/Set device config (not save to Flash)     |
| 0x60 | Get/Set LCD buffer                            |
| 0x61 | Start/Stop notify LCD buffer                  |
| 0x70 | Set PinCode                                   |
| 0x71 | Request Mtu Size Exchange                     |

---

![foto](https://raw.githubusercontent.com/pvvx/pvvx.github.io/master/SensorsTH.jpg)
