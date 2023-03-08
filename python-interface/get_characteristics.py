import asyncio
from bleak import BleakClient, exc
from construct_atc_mi import *
import argparse
import time

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

async def main(args):
    for times in range(args.attempts):
        if args.verbosity:
            print(f"Attempt n. {times + 1}")
        try:
            async with BleakClient(args.address, timeout=40.0) as client:
                for i in await atc_characteristics(
                        client, verbosity=args.verbosity):
                    print(i[0], i[1], i[2])
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
        time.sleep(2)
    if times + 1 == args.attempts:
        print(f"Cannot connect after {args.attempts} attempts.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        epilog='Xiaomi Mijia Thermometer - List BLE Characteristics')
    parser.add_argument(
        '-e',
        "--error",
        dest='show_error',
        action='store_true',
        help="show error information")
    parser.add_argument(
        '-v',
        "--verbosity",
        dest='verbosity',
        action='store_true',
        help="print process information")
    parser.add_argument(
        '-m',
        '--mac',
        dest='address',
        action="store",
        help='MAC Address',
        required=True)
    parser.add_argument(
        '-a',
        '--attempts',
        dest='attempts',
        type=int,
        help='max number of attempts (default=20)',
        default=20)
    asyncio.run(main(parser.parse_args()))
