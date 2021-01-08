# ATC_MiThermometer


Initial forked from https://github.com/atc1441/ATC_MiThermometer

Firmware Version 1.1

Documentation under construction ...


### Average consumption:
>* #### Default settings (Advertising interval of 2.5 seconds, the measurement interval 10 sec):
> * Advertisement: 15.5 uA
> * Connection: 15..17 uA

**Advertising in 4 formats:**

1. UUID 0x181A - size 16: atc1441 format 
2. UUID 0x181A - size 19: custom - temperature x0.01C, humidity x0.01%, battery charge level 0..100%, battery voltage in mV, GPIO-pin flags (mark “reset”) and triggers.
3. UUID 0xFE95 - 0x0A: Xiaomi - battery charge level 0..100%, battery voltage in mV
4. UUID 0xFE95 - 0x0D: Xiaomi - temperature x0.1C, humidity x0.1%

 ++ Сonfiguring mode of transferring everything in turn 

**In Connection mode:**

- Characteristic UUID 0x2A1F - Notify temperature x0.1C
- Characteristic UUID 0x2A6E - Notify temperature x0.01C
- Characteristic UUID 0x2A6F - Notify about humidity x0.01%
- Characteristic UUID 0x2A19 - Notify the battery charge level 0..99%
- Characteristic UUID 0x2803 - Notify, frame id 0x33 (сonfiguring or making a request): temperature x0.01C, humidity x0.01%, battery charge level 0..100%, battery voltage in mV, GPIO-pin flags and triggers.

### Reading Measurements in Connected Mode
![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/GraphAtc_html.gif) 


### Temperature or humidity trigger on GPIO PA5 (label on the "reset" pin)
![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/trg_menu.gif)

![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/OnOff.gif)


### Interface for receiving and displaying data on the LCD.
>* LCD shows: 
> * Big number: -99.5..1999.5 
> * Small number: -9..99
> * Smiley, battery, degrees
> + Setting the display time limit in sec
>
![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/ShowData.gif) 


### The USB-COM adapter writes the firmware in explorer. Web version. 
>* Connect only TX-SWS and GND wires.
![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/USBCOMFlashTxHtml.gif) 


### Reading and recovering Mi-Home keys
>* Stage 1:
![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/KeysProgStage1.gif) 


>* Stage 2:
![SCH](https://github.com/pvvx/ATC_MiThermometer/blob/master/KeysProgStage2.gif) 


