# ATC_MiThermometer
Custom firmware for the Xiaomi Thermometer LYWSD03MMC and Telink Flasher via USB to Serial converter

It is possible to update the Firmware of the Xiaomi Thermometer OTA with this WEB Tool i wrote:
https://atc1441.github.io/TelinkFlasher.html

The web flasher does work for many devices that uses the Telink TLSR82** MCUs and it can also be used to reflash the Stock firmware back to the device.

### You can support my work via PayPal: https://paypal.me/hoverboard1 this keeps projects like this coming.

## OTA
### How to flash the custom firmware:

Download the ATC_Thermometer_custom.bin file and open the Web Flasher, connect to the Xiaomi thermometer, searching may take a while as it broadcasts not so often for better battery life, after the connection is successful click on Do Activation to Authorize the Connection, while its doing so you can already select the firmware file. Be careful to select the right one as it's not possible to check the firmware further. Then click on start flashing to flash the new firmware to the Thermometer.

After the flashing is done the device should reboot, if the screen stays off pull the battery out for a short amount of time.

To flash the stock firmware back to the Thermometer just open that one while flashing.

## USB to UART
### How to flash the custom firmware or unbrick the device:

To flash new firmware via an standard USB to UART adapter simply connect the Thermometer as seen in the picture "Mi_SWS_Connection.jpg" to the USB to UART converter and run the ATCtelink.py tool with the first command to be the file you want to flash.

Example: "python3 ATCtelink.py ATC_Thermometer.bin"

it the flashing fails or no valid COMport can be found you can edit it in the Python script also try to increase the ResetTime, i will try to make that nicer in future!
So far it turned out that flashing via MAC does not work correctly, i think that is because the data will not get pushed out in real time so the Emulated SWS protocoll gets interrupted.

## Custom firmware:
To build the custom firmware on your own follow this guide to get a working TC32 Compiler environment ready where you can add the Custom Mi firmware https://github.com/Ai-Thinker-Open/Telink_825X_SDK use google translate for better reading experience.
Try to "make" the blink example included in the SDK once to see if the compiling works as it should.
You can then copy the folder "ATC_Thermometer" into the example folder and go into that with the terminal now do a "make" and it will build the custom firmware.
The newly created .bin file can then simply be flashed by either the Web Flasher or the USB to UART method.

Because of the OTA dual bank update method a firmware can be maximum 256kB in size.

### Getting the MAC of you Thermometer:
On boot the custom firmware will show the last three bytes of the MAC Address in the humidity display part on the LCD for 2 seconds each, the first three bytes are always the same so not importand.
Also the BLE name will include the last three bytes of the MAC Address

### Advertising format of the custom firmware:
The custom firmware sends every minute an update of advertising data on the UUID 0x181A with the Tempereature, Humidity and Battery data.

The format is like this: 

Byte 5-10 mac in correct order

Byte 11-12 Temperature in uint16

Byte 13 Humidity in percent

Byte 14 Battery in percent

Byte 15 frame packet counter

0x0e, 0x16, 0x1a, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xbb, 0xcc, 0xff


#### Many thanks to:

@danielkucera https://github.com/danielkucera/mi-standardauth/blob/master/provision.py

@romanhosek https://twitter.com/romanhosek
