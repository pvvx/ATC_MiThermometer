## **Mijia BLE Broadcasting Protocol-Mi Beacon**

###### (version 4)

**Overview**

Mi Beacon is part of the MIOT Bluetooth protocol, which regulates the broadcast format of devices based on Bluetooth 4.0 and above, so that the Xiaomi Smart Home APP and MIUI can be identified and the devices can be linked.

**definition**

As long as the broadcast message contains the following specified information, it can be considered as conforming to the Xiaomi broadcast format:

·     «Service Data» (0x16) in Advertising contains Mi Service (0xFE95)

·     «Manufacturer Specific Data» (0xFF) in Scan Response contains the Xiaomi company identification code (0x038F)

Both in Advertising and Scan Response, a unified format definition is adopted. See the next chapter for details. For Scan Response, it is recommended not to add the optional MAC and Capability fields to accommodate more Object data.

format

| name          | Types of | length | Is it  mandatory | Description                           |
| ------------- | -------- | ------ | ---------------- | ------------------------------------- |
| Frame Control | Bitmap   | 2      | Yes              | Control bit                           |
| Product ID    | U16      | 2      | Yes              | Product unique  ID                    |
| Frame Counter | U8       | 1      | Yes              | Serial number  (for deduplication)    |
| MAC Address   | U8       | 6      | no               | Device MAC  address                   |
| Capability    | U8       | 1      | no               | Equipment  capacity                   |
| Object        | U8       | N      | no               | Event or  attribute                   |
| Random Number | U8       | 3      | no               | Required field  if encryption is used |

Frame Control

| Numbering | name             | Description                                                  |
| --------- | ---------------- | ------------------------------------------------------------ |
| 0         | Time Protocol    | Set to  indicate request timing                              |
| 1 ~ 2     | (Reserved)       | Keep                                                         |
| 3         | Encrypted        | Set to  indicate that the packet is encrypted                |
| 4         | MAC              | Set to  indicate that the MAC address is included            |
| 5         | Capability       | Set to include  Capability                                   |
| 6         | Object           | Set to include  Object                                       |
| 7 ~ 8     | (Reserved)       | Keep                                                         |
| 9         | Binding  Confirm | Set to  indicate a binding confirmation packet               |
| 10        | Secure Auth      | Set to  indicate that the device supports secure chip authentication |
| 11        | Secure Login     | Set to  indicate the use of symmetric encryption for login, otherwise use asymmetric  encryption for login |
| 12 ~ 15   | Version          | Device version  number (currently 4)                         |

Object

| name   | length | Description                 |
| ------ | ------ | --------------------------- |
| ID     | 2      | Event or  attribute ID      |
| Length | 1      | Event or  attribute length  |
| Data   | N      | Event or  attribute content |

Capability

| Numbering | name        | Description                                                  |
| --------- | ----------- | ------------------------------------------------------------ |
| 0         | Connectable | Set to  indicate that the device has the ability to establish a connection |
| 1         | Centralable | Set to  indicate that the device has the ability to do Central |
| 2         | Encryptable | Set to  indicate that the device has encryption capability   |
| 3 ~ 4     | BondAbility | Device binding  type                                         |
| 5 ~ 7     | (Reserved)  | Keep                                                         |

There are four types of device binding:

·     00b: User chooses independently or based on RSSI

·     01b: Scan first, connect after the device sends a confirmation package

·     10b: Connect directly after scanning, and confirm the device by vibrating

·     11b: Special for Combo type equipment