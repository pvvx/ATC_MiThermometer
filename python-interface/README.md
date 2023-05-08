# Python interfacing methods and data representation model

These programs and descriptions are provided by [Ircama](https://github.com/Ircama).

This section describes the Python components (API and command-line tools, including some GUI) to represent the data model of the BLE advertisements produced by the custom firmware, as well as a documented interface and a testing tool to receive, decode, show and edit the BLE advertisements delivered via the custom firmware. Additional features allow browsing and editing the full configuration of the device.

The base components used here are:

- [bleak](https://bleak.readthedocs.io/en/latest/), a cross-platform BLE library;
- [construct](https://construct.readthedocs.io/en/latest/intro.html), a symmetric Python library allowing to declaratively define a data structure that describes the advertisement frames produced by the custom firmware, as well as the related configuration frames;
- [construct-gallery](https://github.com/Ircama/construct-gallery), a set of tools extending [construct-editor](https://github.com/timrid/construct-editor/), which provides a GUI (based on [wxPython](https://www.wxpython.org/)) for *construct*. All these tools are used here, including plugins and [bleak_scanner_construct.py](atc_mi_interface/bleak_scanner_construct.py), a library to browse BLE advertisements.

The software in this section includes:

- a [*construct* data model](atc_mi_interface/atc_mi_construct.py) representing the *custom*, *atc1441*, *mi_like* and *bt_home* formats, respectively in clear and encrypted structures, with a documented interface to decode, browse and build the structures of BLE advertisements;
- the [`atc_mi_advertising_format(advertisement_data)`](atc_mi_interface/atc_mi_adv_format.py) function, to easily process BLE advertisements;
- an easy to use [BLE Advertisement Browser GUI](atc_mi_interface/atc_mi_advertising.py) based on the [wxPython](https://www.wxpython.org/) cross-platform desktop GUI toolkit, including controls on the BLE advertisements, map of MAC addresses, bindkeys, etc;
- a [configuration tool](atc_mi_interface/atc_mi_config.py) allowing to browse and edit the configuration parameters of the latest releases of the "pvvx" firmware, with a command-line interface and optionally with a GUI, including the possibility to use this configuration feature as an API.

## Module installation

The software can be installed as a module named *atc-mi-interface* and referenced as *atc_mi_interface*, or can be run from the current directory after downloading the whole *https://github.com/pvvx/ATC_MiThermometer* repository and after installing prerequisites.

Minimum Python version: 3.8. Update `pip` to the latest version.

### Installation as a module

The package can be installed from PyPI with the following command:

```cmd
pip install atc-mi-interface
```

If also the GUI has to be run, use the following command instead of the previous one:

```cmd
pip install atc-mi-interface[gui]
```

All prerequisites are automatically installed and the package can be immediately referenced as the *atc_mi_interface* module.

Additional prerequisites for Raspberry Pi:

```
sudo apt-get install -y libgtk-3-dev  # if GUI is needed
python3 -m pip install attrdict
```

The GUI prerequisites take sensible storage space on a Rasperry Pi.

With Python 3.11 replace *attrdict* with *attrdict3*:

```
python3 -m pip uninstall attrdict
python3 -m pip install attrdict3
```

The C compiler is needed too.

With Raspberry Pi, the *bleak* prerequisite also requires *dbus-fast*, which needs to build the Python *wheel* (related compilation takes some time).

The module also installs the following three entry-points, which can be run from the command-line: *atc_mi_advertising*, *atc_mi_config* and *atc_mi_format_test*.

To uninstall the module:

```cmd
pip uninstall -y atc-mi-interface
```

Alternative installation from GitHub:

```cmd
pip install "git+https://github.com/pvvx/ATC_MiThermometer.git#egg=atc-mi-interface&subdirectory=python-interface"
```

If also the GUI has to be run, use the following command instead of the previous one:

```cmd
pip install "git+https://github.com/pvvx/ATC_MiThermometer.git#egg=atc-mi-interface[gui]&subdirectory=python-interface"
```

Another way to install it from GitHub is the following:

```cmd
git clone https://github.com/pvvx/ATC_MiThermometer
cd ATC_MiThermometer/python-interface

pip install .
```

If also the GUI has to be run, replace `pip install .` with `pip install .[gui]`.

### Running the product from the current directory without module installation

Alternatively from the module installation, the execution can be directly performed from the current directory, using *requirements.txt* and *gui-requirements.txt* to install the prerequisites:

```bash
pip3 install -r requirements.txt
```

For using the GUI option, the installation of prerequisites needs to also add *gui-requirements.txt*:

```bash
pip3 install -r requirements.txt -r gui-requirements.txt
```

The above command can also be used to subsequently install the GUI prerequisites if the module installation was missing of the `[gui]` extra.

Example of execution of *atc_mi_advertising* from the current module directory with installation of prerequisites:

```cmd
git clone https://github.com/pvvx/ATC_MiThermometer
cd ATC_MiThermometer/python-interface

pip3 install -r requirements.txt -r gui-requirements.txt
python3 python3 -m atc_mi_interface.atc_mi_advertising
```

## Decoding and encoding

To only test decoding and encoding, the standard module installation procedure (which also loads *bleak*) is not needed and the following prerequisites can be manually installed instead:

```
python3 -m pip install construct pycryptodome arrow
```

Parsing a [*custom* frame](https://github.com/pvvx/ATC_MiThermometer#custom-format-all-data-little-endian):

```python
from atc_mi_interface import general_format
print(general_format.parse(bytes.fromhex(
    '12 16 1a 18 5c c8 ee 38 c1 a4 b0 08 c3 14 ca 0a 50 14 05')))
```

Output:

```python
Container:
    version = 1
    custom_enc_format = ListContainer:
    custom_format = ListContainer:
        Container:
            version = 1
            size = 18
            uid = 22
            UUID = b'\x18\x1a' (total 2)
            MAC = u'A4:C1:38:EE:C8:5C' (total 17)
            mac_vendor = u'Telink Semiconductor' (total 20)
            temperature = 22.24
            temperature_unit = u'°C' (total 2)
            humidity = 53.15
            humidity_unit = u'%' (total 1)
            battery_v = 2.762
            battery_v_unit = u'V' (total 1)
            battery_level = 80
            battery_level_unit = u'%' (total 1)
            counter = 20
            flags = Container:
                humidity_trigger = False
                temp_trigger = False
                out_gpio_trg_flag = True
                out_gpio_trg_value = False
                input_gpio_value = True
    atc1441_enc_format = ListContainer:
    atc1441_format = ListContainer:
    mi_like_format = ListContainer:
    bt_home_format = ListContainer:
    bt_home_enc_format = ListContainer:
```

The following example shows how to parse any type of format (*custom*, *atc1441*, *mi_like*, *bt_home*, either they are encrypted or not). The right format is automatically selected basing on the frame type:

```python
from atc_mi_interface import general_format

frame1 = bytes.fromhex('12 16 1a 18 cc bb aa 38 c1 a4 77 07 88 13 ca 0a 50 01 07')

atc_mi_data = general_format.parse(frame1)

print("temperature:", atc_mi_data.search_all("^temperature"))
print("humidity:", atc_mi_data.search_all("^humidity"))
print("battery_level:", atc_mi_data.search_all("^battery_level"))
print("battery_v:", atc_mi_data.search_all("^battery_v"))

print()

frame2 = bytes.fromhex('16 16 1e 18 ec 12 cf e5 00 00 3a f3 95 3c a0 5a 7d 03 00 2a ea f8 ea')
bindkey = bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
mac_address = bytes.fromhex("A4:C1:38:AA:BB:CC".replace(':', ''))

atc_mi_data = general_format.parse(frame2, bindkey=bindkey, mac_address=mac_address)

print("temperature:", atc_mi_data.search_all("^temperature"))
print("humidity:", atc_mi_data.search_all("^humidity"))
print("battery_level:", atc_mi_data.search_all("^battery_level"))
print("battery_v:", atc_mi_data.search_all("^battery_v"))
```

Output:

```python
temperature: [19.11, '°C']
humidity: [50.0, '%', False]
battery_level: [80, '%']
battery_v: [2.762, 'V']

temperature: [20.0, '°C']
humidity: [50.0, '%']
battery_level: [100, '%']
battery_v: []
```

Alternatively to `general_format`, also `custom_format` can be used to decode *frame1* and `bt_home_enc_format` to decode *frame2* (after importing them).

Building the frame with the reverse process:

```python
from atc_mi_interface import custom_format

custom_format.build(
    {
        "size": 18,
        "uid": 22,
        "UUID": b"\x18\x1a",
        "MAC": "A4:C1:38:AA:BB:CC",
        "temperature": 19.11,
        "humidity": 50.0,
        "battery_v": 2.762,
        "battery_level": 80,
        "counter": 1,
        "flags": {
            "humidity_trigger": False,
            "temp_trigger": False,
            "out_gpio_trg_flag": True,
            "out_gpio_trg_value": True,
            "input_gpio_value": True,
        },
    }
).hex(' ')
```

Output:

```
'12 16 1a 18 cc bb aa 38 c1 a4 77 07 88 13 ca 0a 50 01 07'
```

## Encrypting and decrypting

All encoded versions of the structures (*custom_enc_format*, *atc1441_enc_format*, *bt_home_enc_format* and *mi_like_format* with encryption flag) support the following additional parameters:

- `mac_address`: MAC address in bytes (e.g., `bytes.fromhex("A4:C1:38:AA:BB:CC".replace(":", ""))`)
- `bindkey`: Bindkey in bytes (e.g., `bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")`)

Alternatively (but not suggested), if the same MAC address and key are needed for all queries of a specific parser, the parameters can be put in *atc_mi_construct.py* within the related `"codec"` adapter, like with the following:

```python
...
    "codec" / AtcMiCodec(
        Aligned(11,
            Struct(
                ...
            )
        ),
        size_payload=6,
        mac_address=bytes.fromhex("A4:C1:38:AA:BB:CC".replace(":", "")),
        bindkey=bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
    ),
)
```

The following example uses the `general_format` to parse, that also includes encrypted formats like `custom_enc_format`:

```python
from atc_mi_interface import general_format

print(
    general_format.parse(
        bytes.fromhex("0e 16 1a 18 bd 9d c5 4e fa b5 00 0e 8b b8 07"),
        mac_address=bytes.fromhex("A4:C1:38:AA:BB:CC".replace(":", "")),
        bindkey=bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"),
    )
)
```
    
Output:

```python
Container:
    version = 1
    custom_enc_format = ListContainer:
        Container:
            version = 1
            size = 14
            uid = 22
            UUID = b'\x18\x1a' (total 2)
            codec = Container:
                temperature = 19.11
                temperature_unit = u'°C' (total 2)
                humidity = 50.0
                humidity_unit = u'%' (total 1)
                battery_level = 93
                battery_level_unit = u'%' (total 1)
                flags = Container:
                    humidity_trigger = False
                    temp_trigger = True
                    out_gpio_trg_flag = True
                    out_gpio_trg_value = True
                    input_gpio_value = True
    custom_format = ListContainer:
    atc1441_enc_format = ListContainer:
    atc1441_format = ListContainer:
    mi_like_format = ListContainer:
    bt_home_format = ListContainer:
    bt_home_enc_format = ListContainer:
```

Building the frame with the reverse process:

```python
from atc_mi_interface import custom_enc_format

custom_enc_format.build(
    {
        "size": 14,
        "uid": 22,
        "UUID": b"\x18\x1a",
        "codec": {
            "temperature": 19.11,
            "humidity": 50.0,
            "battery_level": 80,
            "counter": 1,
            "flags": {
                "humidity_trigger": False,
                "temp_trigger": False,
                "out_gpio_trg_flag": True,
                "out_gpio_trg_value": True,
                "input_gpio_value": True,
            }
        }
    },
    mac_address=bytes.fromhex("A4:C1:38:AA:BB:CC".replace(":", "")),
    bindkey=bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"),
).hex(' ')
```

Output:

```
'0e 16 1a 18 bd 9d c5 4e fa b8 08 56 6b e8 7f'
```

For the build process, generally `Container` is mapped to a dictionary (`{ ... }`), `ListContainer` to a list (`[ ... ]`) and `(enum)` with a normal `key: value` inside a dictionary (with the strings in quotes).

Another parsing example with `bt_home_enc_format`:

```python
from atc_mi_interface import bt_home_enc_format

print(
    bt_home_enc_format.parse(
        bytes.fromhex("16 16 1e 18 23 1b 90 11 7c cd e0 4b a1 73 26 5c 7d 03 00 68 8c 9b 84"),
        mac_address=bytes.fromhex("A4:C1:38:AA:BB:CC".replace(":", "")),
        bindkey=bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"),
    )
)
```

Output:

```python
Container:
    version = 1
    size = 22
    uid = 22
    UUID = b'\x18\x1e' (total 2)
    codec = Container:
        count_id = 228700
        payload = ListContainer:
            Container:
                bt_home_type = (enum) BT_HOME_temperature 8962
                data = Container:
                    temperature = 19.11
                    temperature_unit = u'°C' (total 2)
            Container:
                bt_home_type = (enum) BT_HOME_humidity 771
                data = Container:
                    humidity = 50.0
                    humidity_unit = u'%' (total 1)
            Container:
                bt_home_type = (enum) BT_HOME_battery 513
                data = Container:
                    battery_level = 80
                    battery_level_unit = u'%' (total 1)
```

Reverse process:

```python
from atc_mi_interface import bt_home_enc_format

bt_home_enc_format.build(
    {
        "size": 22,
        "uid": 22,
        "UUID": b"\x18\x1e",
        "codec": {
            "count_id": 228700,
            "payload": [
                {
                    "bt_home_type": "BT_HOME_temperature",
                    "data": {"temperature": 19.11},
                },
                {
                    "bt_home_type": "BT_HOME_humidity",
                    "data": {"humidity": 50.0},
                },
                {
                    "bt_home_type": "BT_HOME_battery",
                    "data": {"battery_level": 80},
                }
            ]
        }
    },
    mac_address=bytes.fromhex("A4:C1:38:AA:BB:CC".replace(":", "")),
    bindkey=bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"),
).hex(" ")
```

Output:

```
'16 16 1e 18 23 1b 90 11 7c cd e0 4b a1 73 26 5c 7d 03 00 68 8c 9b 84'
```

## Processing BLE advertisements

The simplest program to process BLE advertisements produced by the thermometers is the following:

```python
import asyncio
from bleak import BleakScanner
from functools import partial
from atc_mi_interface import general_format, atc_mi_advertising_format

bindkey = {
    "A4:C1:38:AA:BB:CC": bytes.fromhex("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"),
    "A4:C1:38:AA:BB:00": bytes.fromhex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
    # ...
}

async def main():
    count = [0]
    stop_event = asyncio.Event()

    def detection_callback(count, device, advertisement_data):
        format_label, adv_data = atc_mi_advertising_format(advertisement_data)
        if not adv_data:
            return
        mac_address = bytes.fromhex(device.address.replace(":", ""))
        atc_mi_data = general_format.parse(
            adv_data,
            mac_address=mac_address,
            bindkey=bindkey[mac_address] if mac_address in bindkey else None
        )
        print(f"{count[0]}. {format_label} advertisement: {atc_mi_data}. "
            f"RSSI: {device.rssi}")
        count[0] += 1
        if count[0] == 5:
            stop_event.set()

    async with BleakScanner(
        detection_callback=partial(detection_callback, count)
    ) as scanner:
        await stop_event.wait()
    print("Stopped")


asyncio.run(main())
```

The program [runs on](https://github.com/hbldh/bleak#features) Windows, Linux, OS/X, Raspberry Pi, Android. It prints the first 5 parsed frames from available thermometers, regardless their configurations. It exploits `atc_mi_advertising_format()`, which adds headers to the BLE advertisements produced by the thermometers and discovered by `BleakScanner()` (from *bleak*), so that the resulting frame can be directly processed by the *construct* structures included in the module.

After the advertisement dump (e.g., before the *count* increment), you can optionally add:

```python
        print("temperature:", atc_mi_data.search_all("^temperature"))
        print("humidity:", atc_mi_data.search_all("^humidity"))
        print("battery_level:", atc_mi_data.search_all("^battery_level"))
        print("battery_v:", atc_mi_data.search_all("^battery_v"))
```

## atc_mi_advertising: BLE Advertisement Browser GUI

[atc_mi_advertising](atc_mi_interface/atc_mi_advertising.py) is a Python GUI performing BLE advertisement analysis of data transmitted by thermometers, data editing and testing.

![Preview](images/ble_browser.gif)

To run the program (it requires the GUI prerequisites):

```bash
# run program:
python3 -m atc_mi_interface.atc_mi_advertising
```

With a proper Python installation, the entry-point `atc_mi_advertising` should also work in place of `python3 python3 -m atc_mi_interface.atc_mi_advertising`.

Program command line options:

```
usage: atc_mi_advertising [-h] [-s] [-m] [-l LOG_DATA_FILE [LOG_DATA_FILE ...]] [-i]

optional arguments:
  -h, --help            show this help message and exit
  -s, --start           start BLE
  -m, --maximized       display the frame maximized
  -l LOG_DATA_FILE [LOG_DATA_FILE ...], --load LOG_DATA_FILE [LOG_DATA_FILE ...]
                        log data file(s) to be automatically loaded at startup.
  -i, --inspectable     enable Inspection

Xiaomi Mijia Thermometer - BLE Advertisement Browser
```

Main functionalities.
- BLE client to log, browse, test and edit advertisements produced by the Xiaomi Mijia Thermometers.
- Cross-platform GUI.
- The GUI includes a BLE control (implemented with buttons) which allows starting and stopping the BLE advertisement receiver. Advertisements are logged in their reception sequence, automatically labbelled basing on the discovered format and including related MAC address, so that they can be immediately selected by the user to browse and parse data.
- A filter button can be used to enter a specific MAC address to restrict logging, a portion of it or a sequence of addresses, as well as BLE local names.
- Logged data can be saved to file in [pickle format](https://docs.python.org/3/library/pickle.html). Archives can be subsequently reloaded and appended to the current log. They can also be inspected with `python -mpickle archive-file-name.pickle`.
- Advertisement data are logged in the left panel, shown as hex bytes in the central panel and then parsed to browsable *construct* structures in the right panel.
- The program uses the data model contained in `atc_mi_construct.py` to parse and build data and a special button allows the dynamic reloading of this module, so that it can be easily edited, tested and tuned for new features.
- Everything is GUI based. Also the Python error management is wrapped into a GUI panel.
- A Python shell button allows opening an inspector shell, which also provides a special *Help* with related submenu (or by pressing F9).
- All panels allow a context menu (invoked with the right click of the mouse) with a number of special functions.
  - The left log menu panel allows renaming labels and changing MAC address, as well as its related bindkey and description. Also, by double clicking an unused area of the log panel, new frames can be added and then labelled; subsequently, a specific MAC address can be associated. For each MAC address the related bindkey and description can be edited. Log elements can be repositioned, or deleted.
  - The hex editor (central panel) allows any kind of copy/paste. Sequences of bytes can be pasted [in a number of different formats](https://github.com/timrid/construct-editor/pull/17#issuecomment-1367582581). Also a special checkbox enables pasting Python expressions. Two debugging tools are also provided (invoked with the right click of the mouse after selecting a sequence of bytes), to convert bytes into a wide set of numeric forms as well as strings; these debug panels can be used to quickly check the most appropriate conversion method for a sequence of bytes.
  - The construct right panel is also enriched by a right-click context menu controlling a number of features. Notice for instance the possibility display values in dec or hex form, or to show hidden values.
- Encryption and decryprion are fully supported and a separate central panel in the lowest part of the screen shows the bytes produced by the related codec, mapping them with the referred structure in the right panel.
- The right panel allows changing all values (apart from the "Computed" ones) by double clicking the shown values, so that a new sequence of bytes is automatically built (including encryption when needed).
- A button shows a table allowing to edit MAC addresses and their related Bindkey and description.
- The program has command line arguments and one allows loading a saved log at startup; another option automatically starts the BLE advertisement receiver.
- When starting the BLE reception, a debug window is opened in background, with the possibility to control the debug level and clear the produced data.
- The `-i` command line option is for [debugging](https://wiki.wxpython.org/How%20to%20use%20Widget%20Inspection%20Tool%20-%20WIT%20%28Phoenix%29), which can be activated with Ctrl-Alt-I, or Cmd-Alt-I on Mac.

## atc_mi_format_test

The stand-alone sample program [atc_mi_interface.atc_mi_format_test](atc_mi_interface/atc_mi_format_test.py), which can be run with `python3 -m atc_mi_interface.atc_mi_format_test`, allows browsing all available formats, with samples. It consists of a very simple source code, which sets the *gallery_descriptor* variable and then calls *ConstructGallery* passing appropriate parameters. For instance, after running this program, included sample data can be saved to *all-formats.pickle* and then loaded with *atc_mi_advertising.py* via:

```bash
python3 -m atc_mi_interface.atc_mi_advertising -m -l all-formats.pickle
```

With a proper Python installation, the entry-point `atc_mi_format_test` should also work in place of `python3 python3 -m atc_mi_interface.atc_mi_format_test`.

## Device configuration

### Connecting to a LYWSD03MMC thermometer via BLE

A simple program that connects to a LYWSD03MMC thermometer with native firmware and reads the parameters is the following:

```python
import asyncio
from bleak import BleakClient  # pip3 install bleak
from atc_mi_interface import native_temp_hum_v_values, native_comfort_values

mac_address = "A4:C1:38:AA:BB:CC"
valid_string_characteristics = [2, 13, 15, 17, 19, 21, 23, 95]
valid_bytes_characteristics = [26, 30, 33, 34, 36, 37, 57]
native_temp_hum_v_char = 53
native_comfort_char = 66

async def main(address):
    for times in range(20):
        try:
            async with BleakClient(address, timeout=60.0) as client:
                for service in client.services:
                    for char in service.characteristics:
                        name_bytes = await client.read_gatt_char(char)
                        if char.handle in valid_bytes_characteristics:
                            print(char.handle, service.description, "-", char.description, "- Value:", name_bytes.hex(' '))
                        if char.handle in valid_string_characteristics:
                            print(char.handle, service.description, "-", char.description, ":", name_bytes.decode())
                        if char.handle == native_temp_hum_v_char:
                            print(char.handle, char.description, ":", native_temp_hum_v_values.parse(name_bytes))
                        if char.handle == native_comfort_char:
                            print(char.handle, char.description, ":", native_comfort_values.parse(name_bytes))
                break
        except Exception as e:
            print(f"Retrying... ({e})")

asyncio.run(main(mac_address))
```

This program performs a BLE connection (which discarges the battery). The address must be changed withe the actual MAC address.

With native LYWSD03MMC firmware, its output is:

```python
2 Generic Access Profile - Device Name : LYWSD03MMC
13 Device Information - Model Number String : LYWSD03MMC
15 Device Information - Serial Number String : ...
17 Device Information - Firmware Revision String : 1.0.0_0130
19 Device Information - Hardware Revision String : B1.7
21 Device Information - Software Revision String : 0130
23 Device Information - Manufacturer Name String : miaomiaoce.com
26 Battery Service - Battery Level - Value: 63
30 Unknown - OTA - Value: 00
34 Unknown - Time - Value: 9c 45 09 00
37 Unknown - Data Count - Value: a7 00 00 00 a8 00 00 00
53 Temperature and Humidity : Container:
    version = 1
    temperature = 19.1
    temperature_unit = u'°C' (total 2)
    humidity = 47
    humidity_unit = u'%' (total 1)
    battery_v = 2.983
    battery_v_unit = u'V' (total 1)
57 Unknown - Batt - Value: 64
66 comfortable temp and humi : Container:
    version = 1
    temperature_high = 27.0
    temperature_low = 19.0
    humidity_high = 85
    humidity_low = 20
    humidity_unit = u'%' (total 1)
95 Xiaomi Inc. - Version : 1.0.0_0130
```

With the custom firmware v4.1, its output is:

```python
2 Generic Access Profile - Device Name : ATC_AABBCC
13 Device Information - Model Number String : LYWSD03MMC
15 Device Information - Serial Number String : ...
17 Device Information - Firmware Revision String : github.com/pvvx
19 Device Information - Hardware Revision String : B1.7
21 Device Information - Software Revision String : V4.1
23 Device Information - Manufacturer Name String : miaomiaoce.com
26 Battery Service - Battery Level - Value: 64
30 Environmental Sensing - Temperature Celsius - Value: bf 00
33 Environmental Sensing - Temperature - Value: 76 07
36 Environmental Sensing - Humidity - Value: 76 11
```

In this simple program, some numeric values and the "Environmental Sensing" values are not parsed.

A more extensive program is [atc_mi_config](atc_mi_interface/atc_mi_config.py), which parses all values and it is also able to perform device configuration.

### atc_mi_config configuration tool (with GUI)

The *atc_mi_config* Python command-line tool, which can be run with `python3 -m atc_mi_interface.atc_mi_config`, allows to:

- show the device information (for this feature, flashing the custom firmware is not needed);
- show the custom characteristics, inspecting all the parameters of the custom configuration;
- set and show the current device date, comparing it with the host time; when setting the date, the current host time is used to update the device date;
- set the time delta adjustment (using the relative number from -32767 to 32767, where the unit is 1/16 microseconds for 1 second);
- reset default device configuration to the factory settings (i.e., to the default custom firmware configuration);
- configure the firmware to perform a reboot after disconnection;
- edit the device configuration via GUI, via command-line or both;
- provide a development module that includes the `atc_mi_configuration()` API.

With a proper Python installation, the entry-point `atc_mi_config` should also work in place of `python3 python3 -m atc_mi_interface.atc_mi_config`.

### General usage

```
usage: atc_mi_config [-h] [-i] [-c] [-D] [-j DELTA] [-d] [-s STRING] [-R] [-g] [-b] -m ADDRESS [-a ATTEMPTS] [-E [EDIT_LIST ...]] [-e] [-v] [-t] [-x]

optional arguments:
  -h, --help            show this help message and exit
  -i, --info            Show device information
  -c, --chars           Show characteristics (configuration)
  -D, --set_date        Set current device date with the host time
  -j DELTA, --adjust DELTA
                        Set the time delta adjustment (-32767..32767, in 1/16 usec. for 1 sec.)
  -d, --read_date       Show date and delta time adjustment at the end
  -s STRING, --string STRING
                        Set the hex string defined in the subsequent argument
  -R, --reset           Reset default configuration
  -g, --gui             Edit the configuration using the GUI
  -b, --reboot          Set Reboot on disconnect
  -m ADDRESS, --mac ADDRESS
                        Device MAC Address (required). Example: -m A4:C1:38:AA:BB:CC
  -a ATTEMPTS, --attempts ATTEMPTS
                        Set the max number of attempts to connect the device (default=20)
  -E [EDIT_LIST ...], --edit [EDIT_LIST ...]
                        Edit one or multiple values; no value to dump editable parameters
  -e, --error           Show BLE error information
  -v, --verbosity       Show process information
  -t, --test            Show test GUI and test command-line editing
  -x, --inspectable     Enable Inspection (Ctrl-Alt-I)

Xiaomi Mijia Thermometer - Get/Set Configuration
```

### Examples of usage

```
# Printing the device information
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -i

# Printing the device configuration
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -c

# Printing the date as well as time delta adjustment
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -d

# Setting the date to the system date
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -Dd

# Setting the time delta adjustment to DELTA = -30
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -j -30

# Resetting the configuration
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -R

# Starting the GUI configuration (see installation requirements)
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -g

# Dump editable values of the configuration, so that they can be copied into the command-line edit mode
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -E

# Edit the configuration via command-line (use -E without subsequent parameters to dump th editable format)
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -E "Comfort parameters|temperature_low = -15.5" "Internal configuration|flg2|smiley = 7" "Internal configuration|flg2|screen_off = True" -E "Comfort parameters|humidity_high = 65.6"

# Set advertising mode to "atc1441"
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -E "Internal configuration|flg|advertising_type = 0"

# Set advertising mode to "custom"
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -E "Internal configuration|flg|advertising_type = 1"

# Set advertising mode to "mi_like"
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -E "Internal configuration|flg|advertising_type = 2"

# Set advertising mode to "bt_home"
python3 -m atc_mi_interface.atc_mi_config -m A4:C1:38:AA:BB:CC -E "Internal configuration|flg|advertising_type = 3"
```

Notice again that `atc_mi_config` can be used in place of `python3 python3 -m atc_mi_interface.atc_mi_config`.

Using the GUI configuration, to enable editing a non-readonly parameter you need to tiple-click it, then, after completing the editing, close the GUI to store the new configuration to the device. To discard changes, without closing the GUI press Control-C on the Python application. Parameters notified as read-only, even if changed will not be uploaded to the device.

The command-line editor is also allowed through the "-E" option: when used without additional parameters, it dumps the editable parameters so that they can be copied and reused with the command-line edit mode, which uses the same "-E" option followed by a list of configuration settings (`EDIT_LIST`). When changing a configuration via command-line, keep the same format for the configuration parameters (multiple words separated by "|") and the assignment format (" = " including spaces): just update the value (without heading and trailing spaces). Each element can be enclosed in quotes: e.g., `"Internal configuration|flg|advertising_type = 0"`. Multiple "-E" options can be used; besides, a "-E" option accepts one or multiple assignment arguments. If "-E" is used without parameters after a set of edited parameters defined though a previous "-E" option, the device is connected, but the edited configuration is not saved to the device (dry-run mode).

Notice that "Comfort parameters" will not be reset with "-R".

![atc_mi_config GUI Preview](images/atc_mi_config.gif)

### atc_mi_configuration() API interface

The *atc_mi_interface* module exposes the `atc_mi_configuration(args: argparse.Namespace)` async API.

Usage:

```python
ret, data_out = asyncio.run(atc_mi_configuration(configuration))
```

This Python function requires a configuration argument that needs to be an initialized instance of `argparse.Namespace()`, with all attributes set, as in the following example. The returned data is a tuple where the first value is the return code and the second value is the payload.

The return code (first value of the tuple) can be:

- `False`, in case of error,
- `None`, in case of successful execution of a test,
- `True`, in case of successful execution of the configuration.

The payload (second value of the tuple) is a list of dictionaries, where each dictionary has a single *key* string representing the value identifier (e.g., can be parsed to process related values with specific decoders) and the *value*, that is in turn a list of one or more values, which, e.g., can be printed with `print(*value)`.

Example:

```python
import asyncio
import argparse
from atc_mi_interface import atc_mi_configuration

configuration = argparse.Namespace()  # all attributes need to be initialized

configuration.address = 'A4:C1:38:AA:BB:CC'  # Set to the actual MAC address of the device
configuration.info = False  # True to return the device information
configuration.chars = False  # True to return the device characteristics (configuration)
configuration.gui = False  # True to edit the device configuration using the GUI. [It should be set to False with API]
configuration.edit_list = None  # set to [["config1", "config2", ...]] to edit one or multiple values (EDIT_LIST); no value (e.g., [[]]) to dump editable parameters
configuration.set_date = False  # True to set the current device date with the host time
configuration.delta = None  # set to a relative number to configure the time delta adjustment (-32767..32767, in 1/16 usec. for 1 sec.)
configuration.read_date = False  # True to return the date and the delta time adjustment (after previous actions)
configuration.string = None  # set to a hex string to store it to the device
configuration.reset = False  # True to reset the default configuration of the device
configuration.reboot = False  # True to activate a device reboot on disconnect
configuration.attempts = 20  # set the max number of attempts to connect the device
configuration.show_error = False  # True to print the BLE error information. [It should be set to False with API]
configuration.verbosity = False  # True to print the process information. [It should be set to False with API]
configuration.test = False  # True to activate the test GUI and the test command-line editing
configuration.inspectable = False  # True to enable inspection in the GUI (Ctrl-Alt-I). [It should be set to False with API]

ret, data_out = asyncio.run(atc_mi_configuration(configuration))

print("Return code (False, None, True):", ret)
for item in data_out:
    for key, value in item.items():
        print("Entity name:", key, "- Value:", *value)
```

`configuration.edit_list`, when valued, requires a list of lists of strings, where each element is a `EDIT_LIST` string assignment, like described before. The sequence of `EDIT_LIST` assignments can be added in any format, regardless it is included in the outer list, or in the nested one, or in both. To assign a null list: `configuration.edit_list = [[]]`.

# Class diagrams and technical notes

## atc_mi_construct.py

The [atc_mi_construct.py](atc_mi_interface/atc_mi_construct.py) module allows managing all data formats, including parsing frames to variables, building frames from variables, encrypting and decrypting using related keys and more. It relies on [atc_mi_construct_adapters.py](atc_mi_interface/atc_mi_construct_adapters.py) that implements the custom adapters used by *atc_mi_construct.py*.

## atc_mi_construct_adapters.py

```mermaid
classDiagram
    BtHomeCodec --|> Tunnel
    MiLikeCodec --|> BtHomeCodec
    AtcMiCodec --|> BtHomeCodec

    class AtcMiCodec{
        _decode()
        _encode()
    }

    class MiLikeCodec{
        _decode()
        _encode()
    }

    class BtHomeCodec{
        def_mac
        default_bindkey
        size_payload

        __init__()
        _decode()
        _encode()
        bindkey()
        decrypt()
        encrypt()
        mac()
    }
```

## atc_mi_advertising.py

Technically, *atc_mi_advertising.py* is a very small and basic wrapper of the separate [construct-gallery](https://github.com/Ircama/construct-gallery) Python module, which does all the logic underneath. In turn, *construct-gallery*  relies on [construct-editor](https://github.com/timrid/construct-editor), a very powerful, well-designed and extensible module which implements all the construct editor widgets.

```mermaid
classDiagram
    Main "1" *-- "1" AtcMiConstructFrame
    AtcMiConstructFrame --|> wxFrame
    AtcMiConstructFrame "1" *-- "1" AtcMiBleakScannerConstruct
    AtcMiBleakScannerConstruct --|> BleakScannerConstruct

    class AtcMiConstructFrame{
        maximized
        loadfile
        ble_start

        **kwargs
        __init__()
        on_close()
    }

    class AtcMiBleakScannerConstruct{
        ble_start
        gallery_descriptor

        filter_hint_mac="A4:C1:38"
        filter_hint_name="LYWSD03MMC, ATC"
        key_label="Bindkey"
        description_label="Description"
        col_name_width=200
        col_type_width=150
        __init__()
        bleak_advertising()
    }
```

### construct-gallery module

```mermaid
classDiagram
    BleakScannerConstruct --|> ConstructGallery
    ConstructGallery "1" *-- "1" WxConstructHexEditor

    class BleakScannerConstruct{
        filter_hint_mac
        filter_hint_name
        reference_label="MAC address"
        load_menu_label="Log Data and Configuration"
        clear_label="Log Data"
        added_data_label="Logging data"
        logging_plugin=True

        bleak_advertising(device, advertisement_data)
        on_application_close()
    }

    class ConstructGallery{
        parent

        load_menu_label="Gallery Data"
        clear_label="Gallery"
        reference_label=None
        key_label=None
        description_label=None
        added_data_label=""
        loadfile=None
        gallery_descriptor=None
        ordered_samples=None
        ref_key_descriptor=None
        default_gallery_selection=0
        col_name_width=None
        col_type_width=None
        col_value_width=None
        run_shell_plugin=True
        run_hex_editor_plugins=True

        add_data(data, reference, append_label)
    }
```

### construct-editor module

```mermaid
classDiagram
    WxConstructHexEditor --|> wxPanel

    class WxConstructHexEditor{
        parent
        construct
        contextkw
        binary
    }

note   "...
        ...
        ..."
```

## atc_mi_config.py

*atc_mi_config.py* exploits *ConfigEditorPanel* from the *construct-gallery* Python module. The *editing_structure* dictionary allows to easily keep the tool updated with possible new changes in the firmware. All internal structures of the custom firmware, like "cfg", "flg2", "comfort_values", "trigger", etc. are described in *atc_mi_construct.py*.

```mermaid
classDiagram
    Main "1" o-- "1" wxFrame
    ConfigEditor "1" --|> "1" wxFrame
    ConfigEditorPanel "1" --* "1" ConfigEditor
    WxConstructHexEditor "*" --* "1" ConfigEditorPanel
    scrolledScrolledPanel "1" <|-- "1" ConfigEditorPanel

    class ConfigEditor{
        main_panel
    }

    note for wxFrame "only when\nGUI is used"
    note for ConfigEditor "atc_mi_config.py"
    note for ConfigEditorPanel "construct-gallery\nmodule"
    class ConfigEditorPanel{
        editor_panel
    }
```
