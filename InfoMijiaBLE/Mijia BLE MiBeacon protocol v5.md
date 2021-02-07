# BLE MiBeacon protocol 

(Version 5)

In order to facilitate users to quickly discover and establish a connection with BLE devices when using the Mijia APP and Bluetooth gateway, the Xiaomi IoT platform adds the Xiaomi service data (ServiceData UUID 0xFE95) in the broadcast of the BLE device (based on BLE protocol 4.0). Mibeacon), enables the BLE device to identify its own identity and type when broadcasting data, and can be recognized and connected by the user or Bluetooth gateway in time; in addition, in order to better improve the intelligent capabilities of BLE devices, the BLE MiBeacon protocol also supports development According to actual use needs, the user chooses to add the [Object field](https://iot.mi.com/new/doc/embedded-development/module/ble/object-definition) , and reports the event information and status information (attributes) of the BLE device to the Xiaomi IoT platform through the gateway, so as to realize functions such as remote reporting of device status and intelligent linkage.

- The broadcast message with the following information in the **broadcast identifier** is a broadcast conforming to the MiBeacon protocol:

  - "Service Data" (0x16) in advertising contains Mi Service (UUID: 0xFE95)

  - The "Manufacturer Specific Data" (0xFF) in the scan response contains the Xiaomi company identification code (ID: 0x038F)

    > **Note:** "advertising" or "scan response" are defined in a unified format.

- **Use BLE MiBeacon protocol**

  - Developers can directly call the encapsulated interface of the Xiaomi IoT platform to send broadcast data;
  - Do not splice the MiBeacon field to send out broadcast data by yourself, please use the interface directly `mibeacon_obj_enque()`to put the message to be sent into the sending queue, so that the device sends out broadcast at the specified time interval;
  - For the method of using the BLE MiBeacon protocol, please refer to the demo programs [mijia_ble_standard](https://github.com/MiEcosystem/mijia_ble_standard) (standard) and [mijia_ble_secure](https://github.com/MiEcosystem/mijia_ble_secure) (high security level).

## BLE MiBeacon protocol field

### Service Data or Manu Data format

- C.1, determine whether to include according to the Frame Control field definition;
- C.2, determine whether to include it according to the Capability field.

| Field | Name                    | Types of | Length (byte)                 | Mandatory (M) / Optional (O) | Description                                                  |
| :---- | :---------------------- | :------- | :---------------------------- | :--------------------------- | :----------------------------------------------------------- |
| one   | Frame Control           | Bitmap   | 2                             | M                            | Control bit, see the specific definition in the table below  |
| 2     | Product ID              | U16      | 2                             | M                            | Product ID, unique for each type of product, pid need [millet IoT Developer Platform](https://iot.mi.com/new/index) applications |
| 3     | Frame Counter           | U8       | one                           | M                            | Serial number, used for de-duplication, different event or attribute reporting requires different Frame Counter |
| 4     | MAC Address             | U8       | 6                             | C.1                          | Device Mac address                                           |
| five  | Capability              | U8       | one                           | C.1                          | Equipment capacity, see the following table definition for details |
| 6     | I/O capability          | U8       | 2                             | C.2                          | I/O capability. This field is currently only used for high-security BLE access, and only MiBeacon v5 is supported. It is only used before binding; when the binding is completed and an event is reported (such as door opening, door closing), this field is no longer needed. See the definition in the table below for details |
| 7     | Object                  | U8       | n (according to actual needs) | C.1                          | Trigger events or broadcast properties, see Mijia BLE Object protocol for details |
| 8     | Random Number           | U8       | 3                             | C.1                          | If encrypted, it is a required field, combined with Frame Counter to become a 4-byte Counter for anti-replay |
| nine  | Message Integrity Check | U8       | 4                             | C.1                          | If encrypted, it is a required field, MIC four bytes         |

> **Note:** If the field contains too much data and exceeds the length of the Beacon, it is recommended to broadcast in multiple beacons, such as the first packet to broadcast Object and the second packet to broadcast MAC; Scan Response is not allowed in the v5 version.

## Frame Control field definition

| Bit     | Name               | Description                                                  |
| :------ | :----------------- | :----------------------------------------------------------- |
| 0       | Reserved           | reserved text                                                |
| one     | Reserved           | reserved text                                                |
| 2       | Reserved           | Keep                                                         |
| 3       | isEncrypted        | 0: The package is not encrypted; 1: The package is encrypted |
| 4       | MAC Include        | 0: Does not include the MAC address; 1: includes a fixed MAC address (the MAC address is included for iOS to recognize this device and connect) |
| five    | Capability Include | 0: does not include Capability; 1: includes Capability. Before the device is bound, this bit is forced to 1 |
| 6       | Object Include     | 0: does not contain Object; 1: contains Object               |
| 7       | Mesh               | 0: does not include Mesh; 1: includes Mesh. For standard BLE access products and high security level access, this item is mandatory to 0. This item is mandatory for Mesh access to 1. For more information about Mesh access, please refer to Mesh related documents |
| 8       | registered         | 0: The device is not bound; 1: The device is registered and bound. This item is used to indicate whether the device is reset |
| nine    | solicited          | 0: No operation; 1: Request APP to register and bind. It is only valid when the user confirms the pairing by selecting the device on the developer platform, otherwise set to 0. The original name of this item was bindingCfm, and it was renamed to solicited "actively request, solicit" APP for registration and binding |
| 10 ~ 11 | Auth Mode          | 0: old version certification; 1: safety certification; 2: standard certification; 3: reserved |
| 12 ~ 15 | version            | Version number (currently v5)                                |

### Device confirmation is paired

When the device is paired, the developer needs to perform a confirmation operation (such as a button) on the device to start pairing. When the device is paired in this way:

- When the device broadcasts normally, the developer needs to set the value of the [solicited position](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) in [MiBeacon](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) to 0 (no operation);
- When the user triggers the device confirmation (such as confirming by pressing the key), the value of the [solicited position](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) in [MiBeacon](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) needs to be modified to 1 (request [Mijia](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) APP to bind the device), and it needs to be restored to 0 (no operation) after 2~3 seconds;
- When the [Mijia](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) APP detects that the value of the [solicited position](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) in the BLE device broadcast is set to 1, it can establish a connection with the device and perform authentication.

### App confirmation is pairing

When there are multiple devices nearby, the user chooses to pair with the specified device on the [Mijia](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) APP. When the devices are paired in this way, the value of the [solicited position](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) should be set to 0 (no operation);

### RSSI match is matched

When the device is close to the gateway or user terminal and the signal strength is higher than the specified threshold, the pairing can start. When the device is paired in this way:

- Developers need to fill in the RSSI threshold on the Xiaomi IoT developer platform;
- The value of the [solicited position](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon) should be set to 0 (no operation).

> **Note: The** developer `advertising_init()`can modify the value of the solicited position in the Bluetooth broadcast by passing in the parameter (0 or 1) by calling the interface . For the detailed calling method, please refer to the Demo program.

## Capability field definition

| Bit   | Name        | Description                                                |
| :---- | :---------- | :--------------------------------------------------------- |
| 0     | Connectable | Not used temporarily                                       |
| one   | Centralized | Not used temporarily                                       |
| 2     | Encryptable | Not used temporarily                                       |
| 3 ~ 4 | BondAbility | 0, no binding, 1, front binding, 2, back binding, 3, Combo |
| five  | I / O       | 1, contains the I/O Capability field                       |
| 6 ~ 7 | Reserved    | Keep                                                       |

> **Explanation: The** BondAbility field indicates how to determine which device to bind when there are multiple identical devices nearby.

**Device binding method**

- No binding: APP selects pairing, and RSSI matches the pairing;
- Pre-binding: the device confirms pairing, it needs to scan first, and the device sends a confirmation packet (solicited in Frame Control) before establishing a connection;
- Post-binding: connect directly after scanning, and confirm the device by vibrating;
- Combo: Only chips that support Combo are available (this binding method needs to be selected on the Xiaomi IoT developer platform and is consistent with this)

## I / O Capability

| Byte | Name                | Description                                               |
| :--- | :------------------ | :-------------------------------------------------------- |
| 0    | Base I/O capability | 0-3: basic input capability; 4-7: basic output capability |
| one  | Reserved            | Keep                                                      |

Base I/O capability types can be divided into Input/Output two types.

| Bit  | Description                      |
| :--- | :------------------------------- |
| 0    | The device can enter 6 digits    |
| one  | The device can enter 6 letters   |
| 2    | Device can read NFC tag          |
| 3    | The device can recognize QR Code |
| 4    | The device can output 6 digits   |
| five | The device can output 6 letters  |
| 6    | Device can generate NFC tag      |
| 7    | The device can generate QR Code  |

> **Note:** This field is only used in high security level access.