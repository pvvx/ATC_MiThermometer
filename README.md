# ATC_MiThermometer
Custom firmware for the Xiaomi Thermometer and Telink Flasher via USB to Serial converter

It is possible to update the Firmware of the Xiaomi Thermometer OTA with this WEB Tool i wrote:
https://atc1441.github.io/TelinkFlasher.html

The web flasher does work for many devices that uses the Telink TLSR82** MCUs and it can also be used to reflash the Stock firmware back to the device.

## OTA
### How to flash the custom firmware:

Downlaod the ATC_Thermometer_custom.bin file and open the Web Flasher, connect to the Xiaomi thermometer, searching may take a while as it broadcasts not so often for better battery life, after the connection is succsefull click on Do Activation to Authorizate the Connection, while its doing so you can already select the firmware file. Be carefull to select the right one as its not possible to check the firmware further. Then click on start Flashing to flash the new firmware to the Thermometer.

After the flashing is done the device should reboot, it the screen stays black pull the battery out for a short amount of time.

To flash the stock firmware back to the Thermometer just open that one while flashing.

## USB to UART
### How to flash the custom firmware or unbrick the device:

To flash new firmware via an standart USB to UART adapter simply connect the Thermometer as seen in the picture "Mi_SWS_Connection.jpg" to the USB to UART converter and run the ATCtelink.py tool with the first command to be the file you want to flash.

Example: "python3 ATCtelink.py ATC_Thermometer.bin"

it the flashing fails or no valid COMport can be found you can edit it in the Python script also try to increase the ResetTime, i will try to make that nicer in future!
So far it turned out that flashing via MAC does not work correctly, i think that is because the data will not get pushed out in realtime so the Emulated SWS protocoll gets interrupted.

## Custom firmware:
To build the custom firmware on your own follow the this guide to get a working TC32 enviroment ready where you can add the Custom Mi firmware https://github.com/Ai-Thinker-Open/Telink_825X_SDK use google translate for better reading expirience.
Try to "make" the blink example included in the SDK once to see if the compiling works as it should.
You can then copy the folder "ATC_Thermometer" into the example folder and go into into that with the terminal now do a "make" and it will build the custom firmware.
The newly created .bin file can then simply be flashed bei either the Web Flasher or the USB to UART method.

Because of the OTA dual bank update method a firmware can be maximum 256kB in size.
