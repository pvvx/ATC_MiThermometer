# ATC_MiThermometer & MHO-C401


Initial forked from https://github.com/atc1441/ATC_MiThermometer

(Thanks @atc1441 for initial parsing and js code for OTA)

[LYWSD03MMC Custom Firmware Version 2.1](https://github.com/pvvx/ATC_MiThermometer/raw/master/ATC_Thermometer21.bin)

[MHO-C401 Custom Firmware Version 2.1a](https://github.com/pvvx/ATC_MiThermometer/raw/master/MHO_C401_v21a.bin)

[LYWSD03MMC Original Firmware v1.0.0_0106](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_LYWSD03MMC_v1.0.0_0106.bin)

[MHO-C401 Original Firmware v1.0.0_0010](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_MHO_C401_v1.0.0_0010.bin)

### OTA and Custom Setup
[TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html)

### History Firmware versions:
>1.2 Bind, Set Pin-code, Support MHO-C401<br>
>1.3 Get/Set comfort parameters<br>
>1.4 Get/Set device name, Get/Set MAC<br>
>1.5 Add Standard Device Information Characteristics<br>
>1.6 Fix alternation of Advertising in mi mode<br>
>1.7 Authorization and encryption in permissions to access GAP ATT attributes, if pin code is enabled<br>
>1.8 Time display (instead of a blinking smile)<br>
>1.9 Recording measurements to flash memory (cyclic buffer for 19632 measurements)<br>
>2.0 Recording measurements with averaging to flash memory<br>
>2.1 Periodic display refresh for MHO-C401, 'Erase mi-keys' option to return to original firmware<br>

### Average consumption:
>* #### LYWSD03MMC - Default settings (Advertising interval of 2.5 seconds, the measurement interval 10 sec):
> * Advertisement: 20..25 uA 3.3V (CR2032 over 6 months)
> * Connection: 19..25 uA 3.3V (CR2032 over 6 months)
>* #### MHO-C401 - Default settings (Advertising interval of 2.5 seconds, the measurement interval 20 sec):
> * Advertisement: 12..30 uA 3.3V (depends on the amount of temperature or humidity changes over time to display)
> * Connection: 15..30 uA 3.3V (depends on the amount of temperature or humidity changes over time to display)

**Advertising in 4 formats:**

1. UUID 0x181A - size 16: [atc1441 format](https://github.com/atc1441/ATC_MiThermometer#advertising-format-of-the-custom-firmware) 
2. UUID 0x181A - size 19: custom - temperature x0.01C, humidity x0.01%, battery voltage in mV, battery charge level 0..100%, measurement count, GPIO-pin flags (mark “reset”) and triggers.
3. UUID 0xFE95 - 0x0A: Xiaomi - battery charge level 0..100%, battery voltage in mV
4. UUID 0xFE95 - 0x0D: Xiaomi - temperature x0.1C, humidity x0.1%

 ++ Configuring mode of transferring everything in turn 

**In Connection mode:**

+ PrimaryService - Environmental Sensing Service (0x181A):
- Characteristic UUID 0x2A1F - Notify temperature x0.1C
- Characteristic UUID 0x2A6E - Notify temperature x0.01C
- Characteristic UUID 0x2A6F - Notify about humidity x0.01%
+ PrimaryService - Battery Service (0x180F):
- Characteristic UUID 0x2A19 - Notify the battery charge level 0..99%
+ PrimaryService (0x1F10):
- Characteristic UUID 0x1F1F - Notify, frame id 0x33 (configuring or making a request): temperature x0.01C, humidity x0.01%, battery charge level 0..100%, battery voltage in mV, GPIO-pin flags and triggers.

### Reading Measurements from Flash
[GraphMemo.html](https://pvvx.github.io/ATC_MiThermometer/GraphMemo.html)

![FlashData](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/FlashData.gif)

### Reading Measurements in Connected Mode
[GraphAtc.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc.html)
[GraphAtc1.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc1.html)
[GraphAtc2.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc2.html)
[DevPoint.html](https://pvvx.github.io/ATC_MiThermometer/DevPoint.html)

![GraphAtc_html](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/GraphAtc_html.gif) 

### Reading Measurements in Advertising Mode (experimental web platform features)
[Advertising.html](https://pvvx.github.io/ATC_MiThermometer/Advertising.html)

### Temperature or humidity trigger on GPIO PA5 (label on the "reset" pin)
![trg_menu](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/trg_menu.gif)

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

### Sample show LCD

[Display on LCD in a loop:](https://youtu.be/HzYh1vq8ikM)

(Battery and clock display are enabled in the settings. The rest of the settings in default.)

1. Temperature and humidity
2. Temperature and % of battery
3. Temperature and humidity
4. Hours and minutes

The video contains 2 cycles.

#### Chipset:
> * TLSR8251F512ET24 (TLSR8258 in 24-pin TQFN). SoC: TC32 32-bit MCU 48Mhz, 64 KiB SRAM, 512 KiB Flash (GD25LE40C), Bluetooth 5.0: Mesh, 6LoWPAN, Zigbee, RF4CE, HomeKit, Long Range, Operating temperature: -40°C to +85°C, Power supply: 1.8V to 3.6V.
> * SHTV3 sensor. Measurement range: Temperature -40°C to +125°C, Humidity 0 to 100 %RH. Power supply: 1.8V to 3.6V
> * IST3055NA0 LCD controller 

[LYWSD03MMC BoardPinout](https://github.com/pvvx/ATC_MiThermometer/blob/master/BoardPinout)

![TabPins](https://github.com/pvvx/ATC_MiThermometer/blob/master/BoardPinout/TabPins.gif)

#### Building firmware:
1. Go to [wiki.telink-semi.cn](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/IDE-for-TLSR8-Chips/) and getting IDE for TLSR8 Chips.
2. Clone https://github.com/Ai-Thinker-Open/Telink_825X_SDK
3. Install IDE and export the 'ATC_MiThermometer' project.
4. Change 'Linked resousrce' and 'C/C++ Build/Build command'. Compile.

[Telink Linux Tool Chain 2020-06-26](https://yadi.sk/d/pt_qTBB-t24i9A)

[MHO-C401 Info](https://pvvx.github.io/MHO_C401/)

[Build Telink EVK on E104-BT10-G/N module (TLSR8269)](https://github.com/pvvx/TLSR8269-EVK)

[Alternative programmer TLSR SWire on module TB-04/03F or E104-BT10](https://github.com/pvvx/TLSRPGM)

[Additional information on the format Advertising for Xiaomi LYWSD03MMC](https://github.com/Magalex2x14/LYWSD03MMC-info)

[Reading and recovering Mi-Home keys](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/)

[HTML files](https://github.com/pvvx/pvvx.github.io/tree/master/ATC_MiThermometer)

[LYWSD03MMC Forum link (russian)](https://esp8266.ru/forum/threads/tlsr8251-lcd-termometr-lywsd03mmc-xiaomi-bluetooth-termometr.5263/)

[MHO-C401 Forum link (russian)](https://esp8266.ru/forum/threads/tlsr8251f512et24-e-inc-display-termometr-mho-c401-bluetooth-termometr.5446/)

#### CJMCU-2557 BQ25570

![CJMCU-2557](https://raw.githubusercontent.com/pvvx/pvvx.github.io/master/ATC_MiThermometer/img/CJMCU-2557.jpg)

