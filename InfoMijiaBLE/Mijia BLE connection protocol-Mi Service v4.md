# Mijia BLE connection protocol-Mi Service

## Introduction

This document specifies the communication protocol used by BLE devices in the Xiaomi smart home. All BLE devices that need to connect to the Xiaomi smart home APP must be compatible with the broadcast and service formats specified by this protocol.

## The main function

The protocol opens up the data link from BLE devices to the Xiaomi smart home APP, supports sharing device information to mobile phones and the cloud, and also regulates the basis for interoperability between different BLE devices. In addition, through the Xiaomi Smart Home APP, more linkage rules between devices can be customized by users, and users have more room to play.

## basis

### BLE device level

According to whether the device has connection capability, the device can be divided into two levels:

- Level 1 devices only have the ability to send broadcast messages, so they do not need to do very complicated interactions.
- Level 2 devices have the ability to establish a Bluetooth link and can do more work.

### Mi Service definition

The Service UUID specified by Xiaomi is 0xFE95, and the charteristics are defined as follows:

|  UUID  | Size |  Properties  |  Description   |
| :----: | :--: | :----------: | :------------: |
| 0x0001 |  12  | Write Notify |     Token      |
| 0x0002 |  2   |     Read     |   Product ID   |
| 0x0004 |  10  |     Read     |    Version     |
| 0x0005 |  20  | Write Notify |  WIFI Config   |
| 0x0010 |  4   |    Write     | Authentication |
| 0x0013 |  20  |  Read Write  |   Device ID    |
| 0x0014 |  12  |     Read     |   Beacon Key   |
| 0x0015 |  20  | Write Notify |  Device List   |
| 0x0016 |  20  | Write Notify | Security Auth  |

### Version definition

The version format is defined as xxx_xxxx, where the underline is defined and controlled by the eco-chain company, and the underline is defined by Xiaomi, as shown below:

- The standard type (version number 1.xx) uses an obfuscated algorithm to achieve authentication. The source code is not open and is implemented by Xiaomi. After the protocol is adjusted or the code bug is fixed, the minor version number must be updated.
- The enhanced version (version number 2.xx) uses the encryption algorithm suite to achieve authentication, the protocol and source code are open source, and the minor version number is updated only when the protocol is adjusted or upgraded

### Application Service definition

|  UUID  | Size | Properties | Description |
| :----: | :--: | :--------: | :---------: |
| 0xFFE0 |  4   | Read Write | LED Control |
| 0xFFE1 |  4   |   Notify   |   Button    |

## protocol

### premise

In order to connect to the Xiaomi Smart Home APP and interconnect with other Xiaomi products, the BLE device needs to conform to the following format:

- Add Mi Service UUID and Service Data fields to the broadcast data (adv data) or scan response (scan rsp) of the device.
- BLE devices with connection capabilities need to implement a Mi Service-based security authentication process.

### Safety

The security mechanism of the Mi Smart Home Bluetooth protocol can be considered in two parts. The first part is the security between the Bluetooth device and the mobile APP, which is mainly guaranteed by the security authentication mechanism; the second part is the security of the message when the device interacts with the device through broadcast. In this part, if the device has the ability to establish a connection, it can first establish a connection to exchange keys, and then encrypt the broadcast message.

### adaptation

Mijia provides a standard authentication library between Bluetooth devices and Mijia APP, which is used to create Mi Service and perform authentication interaction on the Bluetooth device. The functions of each interface are as follows:

|        Function name        |                       Features                        |                         Description                          |
| :-------------------------: | :---------------------------------------------------: | :----------------------------------------------------------: |
|   mible_server_info_init    |             Initialize device information             |     Parameters include product ID, version number, etc.      |
| mible_server_miservice_init |                 Initialize Mi Service                 | Should be called after the device is powered on and the protocol stack is loaded |
|   mible_service_init_cmp    | Mi Service initialization completed callback function |             Weak definition, implement as needed             |
|       mible_connected       |             Connection callback function              |             Weak definition, implement as needed             |
|     mible_disconnected      |             Disconnect callback function              |             Weak definition, implement as needed             |
| mible_bonding_evt_callback  |        Authentication result callback function        |  Used to notify the application layer of the binding result  |

When the device connects to the Mijia APP for the first time, it enters the binding process. This function returns the result of successful or When the APP establishes a connection, the login process is performed. This function returns the result of login success or failure.

have to be aware of is:

- After the connection is established, the device side cannot allow any operations on the device, such as reading and writing Characteristics, without binding or logging in successfully
- After the device is re-bound successfully, all the configuration information of the previous user needs to be cleared immediately