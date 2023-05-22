#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#############################################################################
# atc_mi_config.py
#############################################################################

import asyncio
import argparse
import time
import calendar
from functools import partial
import enum
import sys
from bleak import BleakClient, exc

from .atc_mi_construct import *
from .atc_mi_construct_adapters import *

notify_uuid = "00001f10-0000-1000-8000-00805f9b34fb"  # Primary Service UUID 0x1F10
characteristic_uuid = "00001f1f-0000-1000-8000-00805f9b34fb"  # Characteristic UUID 0x1F1F

CMD_ID_DNAME = 0x01  # Get/Set device name, "\0" - default: ATC_xxxx
CMD_ID_GDEVS = 0x02  # Get address devices
CMD_ID_I2C_SCAN = 0x03  # I2C scan
CMD_ID_SEN_ID = 0x05  # Get sensor ID
CMD_ID_DEV_MAC = 0x10  # Get/Set MAC [+RandMAC], [size][mac[6][randmac[2]]]
CMD_ID_MI_DNAME = 0x11  # Get/Set Mi key: DevNameId, [size]["\0"+miDevName]
CMD_ID_MI_TBIND = 0x12  # Get/Set Mi keys: Token & Bind, [size][keys]
CMD_ID_COMFORT = 0x20  # Get/set comfort parameters
CMD_ID_UTC_TIME = 0x23  # Get/set utc time
CMD_ID_TADJUST = 0x24  # Get/set adjust time clock delta (in 1/16 us for 1 sec)
CMD_ID_TRG = 0x44  # Get/set trg data
CMD_ID_CFG = 0x55  # Get/set device config

STRTIME_FORMAT = '%A, %d %B %Y %H:%M:%S'
BC_TIMEOUT = 40.0
SLEEP_TIMEOUT = 0.1

char_dict = {
    "00000004-0000-1000-8000-00805f9b34fb": [
        [[29], GreedyString("utf8"), 0, False]  # SJWS01LM Mi Flood Detector Version
    ],
    "00002a00-0000-1000-8000-00805f9b34fb": [
        [[2], GreedyString("utf8"), 0, False]  # SJWS01LM Mi Flood Detector Device name
    ],
    "00001800-0000-1000-8000-00805f9b34fb": [
        [[2], GreedyString("utf8"), 0, False]
    ],
    "0000180a-0000-1000-8000-00805f9b34fb": [
        [[13, 15, 17, 19, 21, 23], GreedyString("utf8"), 0, False]
    ],
    "0000180f-0000-1000-8000-00805f9b34fb": [
        [[16], Int8ul, 1, False],  # SJWS01LM Battery Level
        [[26], Int8ul, 1, False]  # LYWSD03MMC Battery Level
    ],
    "0000181a-0000-1000-8000-00805f9b34fb": [  # custom format
        [[30], Int16sl_x10, 2, False],  # Temperature Celsius
        [[33], Int16sl_x100, 2, False],  # Temperature
        [[36], Int16ul_x100, 2, False],  # Humidity
    ],
    "ebe0ccb0-7a0a-4b0c-8a1a-6ff2997da3a6": [  # LYWSD03MMC native format
        [[57], Int8ul, 1, False],  # Batt
        [[53], native_temp_hum_v_values, 5, False],  # Temperature and Humidity
        [[34], Int32ul, 4, False],  # Time
        [[37], Int64ul, 8, False],  # Data Count
        [[66], native_comfort_values, 6, False],  # comfortable temp and humi
    ],
    "0000fe95-0000-1000-8000-00805f9b34fb": [
        [[29], GreedyString("utf8"), 0, True],  # SJWS01LM version & service.description
        [[95], GreedyString("utf8"), 0, True],  # LYWSD03MMC version & service.description
    ]
}


class IntegerFormat(enum.Enum):
    Dec = enum.auto()
    Hex = enum.auto()


editing_structure = {
    CMD_ID_DNAME: {
        "name": "Device name",
        "sample_binary": b"ATC_AABBCC",
        "construct": Struct(
            "Device name" / GreedyString("utf8"),
        ),
        "read_only": False,
        "size": 110,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_GDEVS: {
        "name": "Internal devices",
        "sample_binary": b'\x88x',
        "construct": i2c_devices,
        "read_only": True,
        "size": 250,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_I2C_SCAN: {
        "name": "I2C scan",
        "sample_binary": b'x\x88',
        "construct": i2c_devices,
        "read_only": True,
        "size": 250,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_SEN_ID: {
        "name": "Sensor ID",
        "sample_binary": b'?\x0f4\xf5',
        "construct": Hex(GreedyBytes),
        "read_only": True,
        "size": 85,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_DEV_MAC: {
        "name": "MAC address",
        "sample_binary": bytes.fromhex("08 cc bb aa 38 c1 a4 01 00"),
        "construct": mac_address,
        "read_only": False,
        "size": 200,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_MI_DNAME: {
        "name": "DevNameID Mi key",
        "sample_binary": b"\x00blt.3.129vOawE1GATC",
        "construct": device_name,
        "read_only": True,
        "size": 150,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 2,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_MI_TBIND: {
        "name": "Token & Bind Mi keys",
        "sample_binary": b"r\xc9.\x9e\xd6!?v3vW\x7f"
                         b"/\x15\x18Q\r\x8a\x8dC?\xc5\xc62Q\x16\xc8\xda",
        "construct": token_bind_mi_keys,
        "read_only": True,
        "size": 150,
        "IntegerFormat": IntegerFormat.Hex,
        "start_data": 2,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_COMFORT: {
        "name": "Comfort parameters",
        "sample_binary": b"4\x08(\n\xb8\x0bp\x17",
        "construct": comfort_values,
        "read_only": False,
        "size": 240,
        "IntegerFormat": IntegerFormat.Dec,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_UTC_TIME: {
        "name": "Current and last set date",
        "sample_binary": b"\xe6l2d\x00\x00\x00\x00",
        "construct": current_last_date,
        "read_only": False,
        "size": 200,
        "IntegerFormat": IntegerFormat.Dec,
        "start_data": 1,
        "slice_begin": 0,
        "slice_end": 4
    },
    CMD_ID_TADJUST: {
        "name": "Time Tick Step",
        "sample_binary": b'\xf4#\xf4\x00',
        "construct": time_tick_step,
        "read_only": True,
        "size": 150,
        "IntegerFormat": IntegerFormat.Dec,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_TRG: {
        "name": "Trigger data",
        "sample_binary": b'4\x08\x88\x13\xc9\xff\x00\x00\x10\x0e\x13\x04',
        "construct": trigger,
        "read_only": False,
        "size": 505,
        "IntegerFormat": IntegerFormat.Dec,
        "start_data": 1,
        "slice_begin": None,
        "slice_end": None
    },
    CMD_ID_CFG: {
        "name": "Internal configuration",
        "sample_binary": b'C\x85\x10\x00\x00(\x04\xa911\x04\xb4',
        "construct": cfg,
        "read_only": False,
        "size": 875,
        "IntegerFormat": IntegerFormat.Dec,
        "start_data": 1,
        "slice_begin": 1,
        "slice_end": None
    },
}

read_only_parameters = [
    "MAC address|length",
    "MAC address|hex RandMAC digits",
    "Internal configuration|firmware_version|major",
    "Internal configuration|firmware_version|minor",
    "Current and last set date|Last_set_date_local",
    "Trigger data|rds|reserved_for_types",
    "Internal configuration|hw_cfg|reserved"
]

def traverse_construct(construct, path, struct):
    report = ""
    EQUAL_SIGN = " = "
    NEWLINE = "\n"
    PATH_SEPARATOR = "|"
    for item in struct:  # item is iterable, otherwise IndexError exception
        if isinstance(item, str) and item.startswith("_"):
            continue
        try:
            subs = struct[item]
        except IndexError:
            continue
        pathname = path + PATH_SEPARATOR + item
        try:
            subcons = getattr(construct, item)
        except AttributeError:
            subcons = construct  # construct does not have an item sub-construct
        try:
            report += traverse_construct(subcons, pathname, subs)
        except TypeError:  # if not iterable
            if pathname in read_only_parameters:
                continue
            if subcons is not None and subcons.subcon.__class__.__name__ in [
                    "Computed", "Switch", "Const", "IfThenElse"]:
                continue
            if subs.__class__.__name__ == "Arrow":
                report += pathname + EQUAL_SIGN + subs.isoformat() + NEWLINE
                continue
            if subs.__class__.__name__ == "bool":
                report += pathname + EQUAL_SIGN + str(bool(subs)) + NEWLINE
                continue
            if subs.__class__.__name__ in ["EnumIntegerString"]:
                report += pathname + EQUAL_SIGN + str(int(subs)) + NEWLINE
                continue
            report += pathname + EQUAL_SIGN + repr(subs) + NEWLINE
    return report


async def atc_characteristics(client, verbosity=False):
    char_list = []
    for service in client.services:
        if verbosity:
            print(">Service", service)
        for char in service.characteristics:
            if "read" not in char.properties:
                continue
            if verbosity:
                name_bytes = await client.read_gatt_char(char.uuid)
                print("    Characteristic", char, "=", name_bytes.hex(' '),
                      "=", name_bytes)
            if service.uuid in char_dict:
                for item in char_dict[service.uuid]:
                    if char.handle not in item[0]:
                        continue
                    name_bytes = await client.read_gatt_char(char.uuid)
                    if not item[2] or len(name_bytes) == item[2]:
                        if item[3]:
                            char_list.append([char.handle, service.description,
                                              item[1].parse(name_bytes)])
                        else:
                            try:
                                string = item[1].parse(name_bytes)
                            except Exception:
                                string = name_bytes.hex(' ').upper()
                            char_list.append([char.handle, char.description,
                                              string])

    return char_list


def get_display_report(tag):
    report = ""
    for key, item in editing_structure.items():
        if tag not in item:
            continue
        report += normalize_report(
            f'{key}: {item["name"]}\n'
            f'{item["construct"].parse(item[tag])}\n'
        ) + '\n'
    return report


def get_editable_report(tag):
    report = ""
    for key, item in editing_structure.items():
        if tag not in item or item["read_only"]:
            continue
        report += traverse_construct(
            item["construct"],
            item["name"],
            item["construct"].parse(item[tag])
        )
    return report


def gui_edit(args: argparse.Namespace):
    try:
        import wx
    except (ImportError, ModuleNotFoundError):
        print("wxPython was not imported. Run the following:\n"
            "pip3 install -r requirements.txt -r gui-requirements.txt")
        sys.exit(2)
    try:
        from construct_gallery import ConfigEditorPanel
    except (ImportError, ModuleNotFoundError):
        print('Failed to import the "construct_gallery" module.\nPlease use '
            "an updated version of construct_gallery. Run the following:\n"
            "pip3 install -r requirements.txt -r gui-requirements.txt")
        sys.exit(2)
    try:
        import construct_editor.core.custom as custom
    except (ImportError, ModuleNotFoundError):
        print('Failed to import the "construct_editor" module.\n'
            "Run the following:\n"
            "pip3 install -r requirements.txt -r gui-requirements.txt")
        sys.exit(2)

    custom.add_custom_tunnel(BtHomeCodec, "BtHomeCodec")
    custom.add_custom_tunnel(AtcMiCodec, "AtcMiCodec")
    custom.add_custom_tunnel(MiLikeCodec, "MiLikeCodec")
    custom.add_custom_adapter(
        ExprAdapter,
        "ComputedValue",
        custom.AdapterObjEditorType.String)
    custom.add_custom_adapter(
        ReversedMacAddress,
        "ReversedMacAddress",
        custom.AdapterObjEditorType.String)
    custom.add_custom_adapter(
        MacAddress,
        "MacAddress",
        custom.AdapterObjEditorType.String)

    class ConfigEditor(wx.Frame):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)

            self.SetTitle("Configuration Editor")
            self.SetSize(1000, 600)
            self.Center()

            self.main_panel = ConfigEditorPanel(
                self,
                editing_structure=editing_structure,
                name_size=180,
                type_size=160,
                value_size=200
            )

    if args.inspectable:
        import wx.lib.mixins.inspection as wit
        app = wit.InspectableApp()
    else:
        app = wx.App(False)
    frame = ConfigEditor(None)
    app.frame = frame  # app.frame.main_panel.editor_panel[1].root_obj
    frame.Show(True)
    if not args.test:
        frame.ToggleWindowStyle(wx.STAY_ON_TOP)
    app.MainLoop()
    for char in frame.main_panel.editor_panel:
        editing_structure[char][
            "new_binary"] = frame.main_panel.editor_panel[char].binary
    return


def assign_pathname(constr, editable_path, value):
    if len(editable_path) == 0:
        return False
    if len(editable_path) == 1:
        class_name = constr[editable_path[0]].__class__.__name__
        if class_name == "bool":
            constr[editable_path[0]] = value.lower() in ['true', '1', 'yes']
        elif class_name in ["int", "EnumIntegerString"]:
            constr[editable_path[0]] = int(value)
        elif class_name in ["Arrow"]:
            constr[editable_path[0]] = arrow.get(value)
        else:
            constr[editable_path[0]] = value.lstrip('\'"').rstrip('\'"')
        return True
    return assign_pathname(constr[editable_path.pop(0)], editable_path, value)


def edit_value(editable, tag, new_tag):
    EQUAL_SIGN = " = "
    PATH_SEPARATOR = "|"
    assignment = editable.split(EQUAL_SIGN)
    if len(assignment) != 2:
        return False
    value = assignment[1]
    editable_path = assignment[0].split(PATH_SEPARATOR)
    name = editable_path.pop(0)
    for _, item in editing_structure.items():
        if item["name"] == name:
            break
    if item["name"] != name:
        return False
    if item['read_only']:
        return False
    if tag not in item:
        return False
    if new_tag in item:
        tag = new_tag
    constr = item["construct"].parse(item[tag])
    if assign_pathname(constr, editable_path, value):
        try:
            item[new_tag] = item["construct"].build(constr)
        except ValueError:
            return False
        return True
    return False


async def atc_mi_configuration(args: argparse.Namespace):
    char_n = [0]
    data_out = []

    if args.verbosity:
        print("Command line arguments:", args)

    def notification_handler(
            char_n: int,
            args: argparse.Namespace,
            handle: int, data: bytes) -> None:
        n = "%0.2X: " % char_n[0]
        if args.verbosity:
            print(handle, n, data, data.hex(' ').upper())
        if data[0] not in editing_structure:
            if args.verbosity:
                print("Error: no data")
            return
        item = editing_structure[data[0]]
        start_data = item.get("start_data") or 0
        item["binary"] = (item.get("binary") or b'') + data[start_data:]
        if args.verbosity:
            print("Binary:", item["binary"])
        if args.verbosity and start_data == 2:
            item["length"] = data[1] + (item.get("length") or 0)
            print("Length:", item["length"])

    if args.test:
        if args.edit_list:
            edited = False
            for editable_group in args.edit_list:
                if editable_group == []:
                    data_out.append({"test_editable": [
                        get_editable_report("sample_binary")]})
                    if edited:
                        data_out.append(
                            {"test_edited_header": ["Edited components:"]})
                        data_out.append(
                            {"test_edited": [get_editable_report("binary")]})
                    return None, data_out
                for editable in editable_group:
                    if edit_value(editable, 'sample_binary', "binary"):
                        edited = True
                        data_out.append({"test_edit_ok": [
                            f'Successfully edited "{editable}"']})
                    else:
                        data_out.append({"test_edit_error": [
                            f'Error: cannot edit "{editable}"']})
                        return False, data_out
        data_out.append({"test_gui_start": ["TEST GUI STARTED"]})
        for _, item in editing_structure.items():
            if "binary" not in item:
                item["binary"] = item["sample_binary"]
        data_out.append({"test_display_report": [get_display_report("binary")]})
        gui_edit(args)
        data_out.append({"test_gui_end": ["TEST GUI TERMINATED"]})
        return None, data_out

    for times in range(args.attempts):
        if args.verbosity:
            print(f"Attempt n. {times + 1}")
        try:
            async with BleakClient(args.address, timeout=BC_TIMEOUT) as client:
                if args.verbosity:
                    print(f"Connected: {client.is_connected}")

                paired = await client.pair(protection_level=2)
                if args.verbosity:
                    print(f"Paired: {paired}")

                if args.info:
                    for i in await atc_characteristics(
                            client, verbosity=args.verbosity):
                        data_out.append({"atc_chars": [i[0], i[1], i[2]]})

                await client.start_notify(
                    characteristic_uuid, partial(
                        notification_handler, char_n, args))

                if args.reset:
                    data_out.append(
                        {"notification_reset": ["Reset default configuration"]})
                    char_n[0] = 0xEE56
                    await client.write_gatt_char(
                        characteristic_uuid, b'\x56', response=True)
                    await asyncio.sleep(SLEEP_TIMEOUT)
                if args.chars or args.gui or args.edit_list:
                    for c in editing_structure:
                        char_n[0] = c
                        await client.write_gatt_char(
                            characteristic_uuid, bytes([c]), response=True)
                        await asyncio.sleep(SLEEP_TIMEOUT)
                    if not args.edit_list == [[]]:
                        data_out.append(
                            {"display_report": [get_display_report("binary")]})
                edited = False
                if args.edit_list:
                    for editable_group in args.edit_list:
                        if editable_group == []:
                            data_out.append({"display_editable": [
                                get_editable_report("binary")]})
                            if edited:
                                data_out.append({"notification_test_edit": [
                                    "Test edit components (not saved):"]})
                                data_out.append({"editable_report": [
                                    get_editable_report("new_binary")]})
                            return None, data_out
                        for editable in editable_group:
                            if edit_value(editable, 'binary', "new_binary"):
                                data_out.append({"edited_ok": [
                                    f'Successfully edited "{editable}"']})
                                edited = True
                            else:
                                data_out.append({"edited_error": [
                                    f'Error: cannot edit "{editable}"']})
                                return False, data_out
                    if args.gui:  # allow using updated values with the GUI
                        for _, item in editing_structure.items():
                            item["old_binary"] = item["binary"]
                            if "new_binary" in item:
                                item["binary"] = item["new_binary"]
                if args.gui:
                    gui_edit(args)
                if args.gui or edited:
                    for _, item in editing_structure.items():
                        if "old_binary" in item:
                            item["binary"] = item["old_binary"]
                    for char, item in editing_structure.items():
                        slice_begin = item.get("slice_begin")
                        slice_end = item.get("slice_end")
                        new_conf = item.get("new_binary")
                        if (new_conf is not None and
                                item["binary"] != new_conf
                                and not item["read_only"]):
                            data_out.append({"setting_char": [
                                f"Setting characteristic {hex(char)}."]})
                            char_n[0] = 0xEE00 + char  # Set configuration
                            set_conf = bytes([char]) + new_conf[
                                                       slice_begin:slice_end]
                            await client.write_gatt_char(
                                characteristic_uuid,
                                set_conf,
                                response=True)
                            data_out.append({"set_chars": [
                                f"Characteristic {hex(char)} has been set."]})
                            await asyncio.sleep(SLEEP_TIMEOUT)
                if args.set_date:
                    data_out.append(
                        {"setting_date": ["Setting date:", args.set_date]})
                    set_date = b'\x23' + Int32ul.build(
                        calendar.timegm(time.localtime(time.time())))
                    char_n[0] = 0xEE23  # Set utc time
                    await client.write_gatt_char(
                        characteristic_uuid, set_date, response=True)
                    await asyncio.sleep(SLEEP_TIMEOUT)
                if args.delta is not None:
                    data_out.append({"setting_time_adj": [
                        "Setting time delta adjustment:", args.delta]})
                    adj = b'\x24' + Int16sl.build(args.delta)
                    char_n[0] = 0xEE24  # Set adjust time clock delta (in 1/16 us for 1 sec)
                    await client.write_gatt_char(
                        characteristic_uuid, adj, response=True)
                    await asyncio.sleep(SLEEP_TIMEOUT)
                if args.read_date:
                    data_out.append(
                        {"read_date": ["Date and time delta adjustment:"]})
                    for c in [
                        CMD_ID_UTC_TIME,  # Get utc time
                        CMD_ID_TADJUST,  # Get adjust time clock delta (in 1/16 us for 1 sec)
                    ]:
                        char_n[0] = c
                        await client.write_gatt_char(
                            characteristic_uuid, bytes([c]), response=True)
                        await asyncio.sleep(SLEEP_TIMEOUT)
                    data_out.append(
                        {"display_report_date": [get_display_report("binary")]})
                if args.string:
                    char_n[0] = 0xEEFF
                    data_out.append({"write_string": [
                        "Write hex", bytes.fromhex(args.string).hex(' ')]})
                    await client.write_gatt_char(
                        characteristic_uuid,
                        bytes.fromhex(args.string),
                        response=True)
                    await asyncio.sleep(SLEEP_TIMEOUT)
                if args.reboot:
                    data_out.append({"notification_set_reboot": [
                        "Set Reboot on disconnect"]})
                    char_n[0] = 0xEE72
                    await client.write_gatt_char(
                        characteristic_uuid, b'\x72')
                    await asyncio.sleep(SLEEP_TIMEOUT)
                await asyncio.sleep(1)

                await client.stop_notify(characteristic_uuid)
                await asyncio.sleep(SLEEP_TIMEOUT)
                break
        except OSError as e:
            if args.show_error:
                print("get_services_task interrupted.", e)
        except asyncio.exceptions.TimeoutError as e:
            if args.show_error:
                print("Connection timeout error.", e)
        except asyncio.exceptions.CancelledError as e:
            if args.show_error:
                print("Connection error (Cancelled).", e)
        except exc.BleakError as e:
            if args.show_error:
                print("GATT error:", e)
        try:
            time.sleep(1)
        except KeyboardInterrupt:
            print('Interrupted')
    if times + 1 == args.attempts:
        data_out.append(
            {"timeout": [f"Cannot connect after {args.attempts} attempts."]})
    return True, data_out


def main():
    parser = argparse.ArgumentParser(
        prog='atc_mi_config',
        epilog='Xiaomi Mijia Thermometer - Get/Set Configuration')
    config_group = parser.add_mutually_exclusive_group()
    parser.add_argument(
        '-m',
        '--mac',
        dest='address',
        action="store",
        help='Device MAC Address (required). Example: -m A4:C1:38:AA:BB:CC',
        required=True)
    parser.add_argument(
        '-i',
        "--info",
        dest='info',
        action='store_true',
        help="Show device information")
    config_group.add_argument(
        '-c',
        "--chars",
        dest='chars',
        action='store_true',
        help="Show characteristics (configuration)")
    parser.add_argument(
        '-g',
        "--gui",
        dest='gui',
        action='store_true',
        help="Edit the configuration using the GUI")
    parser.add_argument(
        '-E',
        "--edit",
        nargs='*',
        dest='edit_list',
        action='append',
        help="Edit one or multiple values; "
            "no value to dump editable parameters")
    parser.add_argument(
        '-D',
        "--set_date",
        dest='set_date',
        action='store_true',
        help="Set current device date with the host time")
    parser.add_argument(
        "-j",
        '--adjust',
        dest="delta",
        type=int,
        help=
        'Set the time delta adjustment '
        '(-32767..32767, in 1/16 usec. for 1 sec.)',
        default=None)
    parser.add_argument(
        '-d',
        "--read_date",
        dest='read_date',
        action='store_true',
        help="Show date and delta time adjustment at the end")
    parser.add_argument(
        '-s',
        "--string",
        dest='string',
        action="store",
        help="Set the hex string defined in the subsequent argument")
    parser.add_argument(
        '-R',
        "--reset",
        dest='reset',
        action='store_true',
        help="Reset default configuration")
    parser.add_argument(
        '-b',
        "--reboot",
        dest='reboot',
        action='store_true',
        help="Set Reboot on disconnect")
    parser.add_argument(
        '-a',
        '--attempts',
        dest='attempts',
        type=int,
        help='Set the max number of attempts to connect the device '
            '(default=20)',
        default=20)
    parser.add_argument(
        '-e',
        "--error",
        dest='show_error',
        action='store_true',
        help="Show BLE error information")
    parser.add_argument(
        '-v',
        "--verbosity",
        dest='verbosity',
        action='store_true',
        help="Show process information")
    parser.add_argument(
        '-t',
        "--test",
        dest='test',
        action='store_true',
        help="Show test GUI and test command-line editing")
    parser.add_argument(
        '-x',
        "--inspectable",
        dest='inspectable',
        action='store_true',
        help="Enable Inspection (Ctrl-Alt-I)")

    ret = False
    data_out = None
    try:
        ret, data_out = asyncio.run(atc_mi_configuration(parser.parse_args()))
    except KeyboardInterrupt:
        print('Interrupted')
    if data_out:
        for item in data_out:
            for _, value in item.items():
                print(*value)
    if ret is False:
        sys.exit(1)
    sys.exit(0)


if __name__ == "__main__":
    main()
