# Mijia BLE Object Definition

This document defines the Object ID and Object format in MiBeacon usage scenarios. The device needs to broadcast the MiBeacon containing the Object when reporting events or properties. The gateway will parse the valid Object and report it to the Mijia backend, which can be displayed by the APP or perform a series of automated operations.

**BLE Object format**

|      Field      | length |     description     |
| :-------------: | :----: | :-----------------: |
|    Object ID    |   2    | Mijia definition ID |
| Object Data Len |  one   |     Data length     |
|   Object Data   |   N    |        data         |

## BLE gateway restrictions

The BLE device reports events or attributes (MiBeacon including Object) to the Xiaomi IoT platform through the gateway. In order to ensure that the gateway can report data normally and enable users to get a good user experience, the BLE gateway needs to follow the following restrictions:

- Support up to 50 devices to report information under the same gateway;
- Older gateways such as MTK7697 1.x, etc. only support access to devices of up to 15 categories;
- When there are more than 200 BLE devices broadcasting data around the gateway, the performance of the gateway will be affected;
- MiBeacon data with a length greater than 31 bytes (Byte) is not supported;
- Each BLE device can only support up to 7 objects;
- Each MiBeacon broadcast package can only contain one Object;
- The effective data length of each Object is up to 10 bytes (Byte).

## ID definition

Defined by Mijia, the distribution is as follows:

- 0x0000-0x1001 are Bluetooth general events
- 0x1002-0x1FFF are Bluetooth general attributes
- 0x2000-0x2FFF are vendor-defined attributes, and the corresponding vendor shall apply to Xiaomi
- 0x3000-0x3FFF is a manufacturer-defined event, and the corresponding manufacturer applies to Xiaomi

In order to ensure that the data is timely and effective, and not to cause heavy load pressure on the Mijia server, the Mijia BLE gateway will filter different types of data according to the ID. When the data is separated from the previously reported data by a certain time and exceeds a certain amount of change, The BLE gateway will forward the data to the Mijia server. Therefore, the time interval and variation are specifically defined, as shown in the following table.

In order to ensure that the events and attributes represented by the Object can be received by the Mijia BLE gateway, the MiBeacon containing the Object needs to be retransmitted many times. Refer to the Mijia BLE MiBeacon protocol for details.

### Passing case

|              name              |   ID   | Time interval (seconds) | Variation |
| :----------------------------: | :----: | :---------------------: | :-------: |
|           connection           | 0x0001 |            0            |     0     |
|          Easy pairing          | 0x0002 |            0            |     0     |
|              near              | 0x0003 |            0            |     0     |
|           keep away            | 0x0004 |            0            |     0     |
|        Lock (obsolete)         | 0x0005 |            0            |     0     |
|          fingerprint           | 0x0006 |            0            |     0     |
|              Gate              | 0x0007 |            0            |     0     |
|             Armed              | 0x0008 |            0            |     0     |
|       Gesture controller       | 0x0009 |            0            |     0     |
|        body temperature        | 0x000A |           60            |     0     |
|              lock              | 0x000B |            0            |     0     |
|            Flooding            | 0x000C |            0            |     0     |
|             smoke              | 0x000D |            0            |     0     |
|              Gas               | 0x000E |            0            |     0     |
| Someone is moving (with light) | 0x000F |            0            |     0     |
|      Toothbrush incident       | 0x0010 |            0            |     0     |
|        Cat eye incident        | 0x0011 |            0            |     0     |
|         Weighing event         | 0x0012 |            0            |     0     |
|             button             | 0x1001 |            0            |     0     |

### General attributes

|              name               |   ID   | Time interval (seconds) | Variation |
| :-----------------------------: | :----: | :---------------------: | :-------: |
|              Sleep              | 0x1002 |           600           |     0     |
|              RSSI               | 0x1003 |            0            |    ten    |
|           temperature           | 0x1004 |           600           |    one    |
|            humidity             | 0x1006 |           600           |    one    |
|        Light illuminance        | 0x1007 |           600           |    one    |
|          Soil moisture          | 0x1008 |           600           |    one    |
|          Soil EC value          | 0x1009 |           600           |    one    |
|              Power              | 0x100A |           600           |     0     |
|              lock               | 0x100E |           60            |     0     |
|              Gate               | 0x100F |           60            |     0     |
|          formaldehyde           | 0x1010 |           600           |    one    |
|              Bind               | 0x1011 |           600           |     0     |
|             switch              | 0x1012 |           600           |     0     |
| Remaining amount of consumables | 0x1013 |           600           |     0     |
|            Flooding             | 0x1014 |           one           |    one    |
|              smoke              | 0x1015 |           one           |    one    |
|               Gas               | 0x1016 |           one           |    one    |
|          No one moves           | 0x1017 |           one           |    one    |
|         Light intensity         | 0x1018 |           one           |    one    |
|           Door sensor           | 0x1019 |           one           |    one    |
|        Weight attributes        | 0x101A |           600           |     0     |
|     No one moves over time      | 0x101B |           one           |    one    |
|          Smart pillow           | 0x101C |           60            |    one    |

### Vendor custom attributes

|                   name                   |   ID   | Time interval (seconds) | Variation |
| :--------------------------------------: | :----: | :---------------------: | :-------: |
| Body temperature (measured every second) | 0x2000 |           600           |     0     |
|             Mi Band (Huami)              | 0x2001 |           600           |    one    |
|         Vacuum cleaner (Rui Mi)          | 0x2002 |           600           |     0     |
|      Black plus bracelet (like one)      | 0x2003 |           600           |     0     |

### Vendor-defined event

|                     name                     |   ID   | Time interval (seconds) | Variation |
| :------------------------------------------: | :----: | :---------------------: | :-------: |
|        Monitor (flowers and grasses)         | 0x3000 |           600           |    one    |
|          Sensor location (Qingping)          | 0x3001 |           600           |     0     |
|         Pomodoro incident (Qingping)         | 0x3002 |            0            |     0     |
| Xiaobei toothbrush incident (Qinghe Xiaobei) | 0x3003 |            0            |     0     |

## Event format definition

### Easy pairing event

|   name    | length |                       description                        |
| :-------: | :----: | :------------------------------------------------------: |
| Object ID |   2    | Object ID to be paired, such as key press event (0x1001) |

### Fingerprint event

|     name     | length | description |
| :----------: | :----: | :---------: |
|    Key ID    |   4    |  See above  |
| Match result |  one   |  see below  |

Key ID is divided into the following categories:

- 0x00000000: the administrator of the lock
- 0xFFFFFFFF: unknown operator

The matching results are divided into the following categories:

- 0x00: match successfully
- 0x01: match failed
- 0x02: Timeout is not put
- 0x03: Low quality (too light, fuzzy)
- 0x04: insufficient area
- 0x05: The skin is too dry
- 0x06: The skin is too wet

### Gate incident

|  name  | length | description |
| :----: | :----: | :---------: |
| status |  one   |  see below  |

The status is divided into the following categories:

- 0x00: open the door
- 0x01: Close the door
- 0x02: Timeout is not closed
- 0x03: knock on the door
- 0x04: Pry the door
- 0x05: The door is stuck

### Armed event

|       name        | length |        description        |
| :---------------: | :----: | :-----------------------: |
| Whether to enable |  one   | Open (0x01), close (0x00) |
|     Timestamp     |   4    |    UTC time (optional)    |

### Gesture controller event

|  name   | length | description |
| :-----: | :----: | :---------: |
| gesture |   2    |  see below  |

Gestures are divided into the following categories:

- 0x0001: Shake
- 0x0002: flip 90 degrees
- 0x0003: flip 180 degrees
- 0x0004: plane rotation
- 0x0005: tap
- 0x0006: Nudge

### Body temperature incident

|       name       | length |                         description                          |
| :--------------: | :----: | :----------------------------------------------------------: |
| body temperature |   2    | Signed variable, the unit is 0.01 degrees, example: A7 0E = 0x0EA7 means 37.51 degrees |

### Lock event

|          name          | length |     description     |
| :--------------------: | :----: | :-----------------: |
|       operating        |  one   |      see below      |
| Key ID or Exception ID |   4    |      see below      |
|       Timestamp        |   4    | Operation timestamp |

The lower 4 bits of the operation field represent action, which is divided into the following categories:

- 0000b: unlock outside the door
- 0001b: Locked (If it is not possible to distinguish the lock inside or outside the door, use this type to report)
- 0010b: Turn on anti-lock
- 0011b: Release the anti-lock
- 0100b: unlock inside the door
- 0101b: The door is locked
- 0110b: Turn on child lock
- 0111b: Turn off the child lock
- 1000b: Lock outside the door
- 1111b: Abnormal

The upper 4 bits of the operation field represent the method, which is divided into the following categories:

- 0000b: Bluetooth mode
- 0001b: Password method
- 0010b: Biological characteristics (fingerprints, faces, veins, palm prints, etc.)
- 0011b: Key method
- 0100b: Turntable method
- 0101b: NFC method
- 0110b: One-time password
- 0111b: Two-step verification
- 1001b：Homekit
- 1000b: Coercion method
- 1010b: Manual
- 1011b: Automatic
- 1111b: Abnormal

Key ID is divided into the following categories:

- 0x00000000: the administrator of the lock
- 0xFFFFFFFF: unknown operator
- 0xDEADBEEF: invalid operator
- 0x00000000-0x7FFFFFFF: Bluetooth (up to 2147483647)
- 0x80010000-0x8001FFFF: biometrics-fingerprints (up to 65536)
- 0x80020000-0x8002FFFF: password (maximum 65536)
- 0x80030000-0x8003FFFF: keys (up to 65536)
- 0x80040000-0x8004FFFF: NFC (up to 65536)
- 0x80050000-0x8005FFFF: Two-step verification (up to 65536)
- 0x80060000-0x8006FFFF: Biometrics-human faces (up to 65536)
- 0x80070000-0x8007FFFF: Biometrics-finger veins (up to 65536)
- 0x80080000-0x8008FFFF: Biometrics-palm prints (up to 65536)

The ID starting with 0xC0DE indicates an exception, and the exceptions triggered externally include:

- 0xC0DE0000: Frequent unlocking with incorrect password
- 0xC0DE0001: Unlock frequently with wrong fingerprint
- 0xC0DE0002: Operation timeout (password input timeout)
- 0xC0DE0003: Lock picking
- 0xC0DE0004: Reset button is pressed
- 0xC0DE0005: The wrong key is frequently unlocked
- 0xC0DE0006: Foreign body in the keyhole
- 0xC0DE0007: The key has not been taken out
- 0xC0DE0008: Error NFC frequently unlocks
- 0xC0DE0009: Timeout is not locked as required
- 0xC0DE000A: Failure to unlock frequently in multiple ways
- 0xC0DE000B: Unlocking the face frequently fails
- 0xC0DE000C: Failure to unlock the vein frequently
- 0xC0DE000D: Hijacking alarm
- 0xC0DE000E: unlock inside the door after arming
- 0xC0DE000F: Frequent palm prints fail to unlock
- 0xC0DE0010: The safe was moved

Internally triggered exceptions include:

- 0xC0DE1000: The battery level is less than 10%
- 0xC0DE1001: The battery is less than 5%
- 0xC0DE1002: The fingerprint sensor is abnormal
- 0xC0DE1003: The accessory battery is low
- 0xC0DE1004: mechanical failure
- 0xC0DE1005: The lock sensor is faulty

### Flooding incident

|       name        | length |                     description                      |
| :---------------: | :----: | :--------------------------------------------------: |
| Flooding incident |  one   | Flooding alarm (0x01), flooding alarm release (0x00) |

### Smoke incident

|      name      | length |                         description                          |
| :------------: | :----: | :----------------------------------------------------------: |
| Smoke incident |  one   | Normal monitoring (0x00), fire alarm (0x01), equipment failure (0x02), equipment self-check (0x03), analog alarm (0x04) |

### Gas incident

|     name     | length |                         description                          |
| :----------: | :----: | :----------------------------------------------------------: |
| Gas incident |  one   | Normal monitoring (0x00), gas leakage alarm (0x01), equipment failure (0x02), sensor life expiration (0x03), sensor preheating (0x04), equipment self-checking (0x05), analog alarm (0x06) |

### Someone moving event (with light)

|      name       | length |                         description                          |
| :-------------: | :----: | :----------------------------------------------------------: |
| Light intensity |   3    | The unit of light intensity is Lux, the value range is 0-120000 |

Remarks: This event is only used for human sensors with light sensors at the same time, and a separate human sensor can use "Proximity Event (0x0003)"

### Toothbrush incident

|       name       | length |                         description                          |
| :--------------: | :----: | :----------------------------------------------------------: |
|     Types of     |  one   |           0: Start of brushing, 1: End of brushing           |
| Score (optional) |  one   | This parameter can be added to the end of brushing event: the score of this brushing, 0~100 |

### Cat eye incident

|    name    | length |                         description                          |
| :--------: | :----: | :----------------------------------------------------------: |
| Event type |  one   | 0: someone stays, 1: someone passes by, 2: someone rings the bell, 3: someone leaves a message, 4: damages the device, 5: duress alarm, 6: abnormal unlock |

### Weighing event

|      name      | length |                       description                       |
| :------------: | :----: | :-----------------------------------------------------: |
| Weighing value |   2    |                      Unit is grams                      |
| Weighing type  |  one   | 0: current weight, 1: reduce weight, 2: increase weight |

### Key event

|   name   | length |                         description                          |
| :------: | :----: | :----------------------------------------------------------: |
|  Index   |   2    |                 Key number, value range 0~9                  |
| Types of |  one   | Single click (0x00), double click (0x01), long press (0x02), triple click (0x03) |

### Flower and grass detector event

|  name  | length |         description          |
| :----: | :----: | :--------------------------: |
| status |  one   | Normal (0x00), unplug (0x01) |

### Qingping sensor event

|   name   | length |                  description                  |
| :------: | :----: | :-------------------------------------------: |
| position |  one   | Separate from the base (0x00), connect (0x01) |

### Qingping Pomodoro Event

|    name    | length |                         description                          |
| :--------: | :----: | :----------------------------------------------------------: |
| Event type |  one   | 0-Start of Pomodoro, 1-End of Pomodoro, 2-Start of rest, 3-End of rest |

### Beckham toothbrush incident

|       name       | length |                         description                          |
| :--------------: | :----: | :----------------------------------------------------------: |
|     Types of     |  one   |           0: Start of brushing, 1: End of brushing           |
|    Timestamp     |   4    |                           UTC time                           |
| Score (optional) |  one   | This parameter can be added to the end of brushing event: the score of this brushing, 0~100 |

## Attribute format definition

### Sleep attributes

|  name  | length |              description               |
| :----: | :----: | :------------------------------------: |
| status |  one   | No sleep (0x00), falling asleep (0x01) |

### RSSI attributes

| name | length |      description      |
| :--: | :----: | :-------------------: |
| RSSI |  one   | Signal strength value |

### Temperature attributes

|    name     | length |                         description                          |
| :---------: | :----: | :----------------------------------------------------------: |
| temperature |   2    | Signed variable, unit 0.1 degree, example: 1A 01 = 0x011A means 28.2 degree |

### Humidity properties

|   name   | length |                         description                          |
| :------: | :----: | :----------------------------------------------------------: |
| humidity |   2    | Humidity percentage, ranging from 0-1000, for example, 346 means humidity 34.6% |

### Illumination properties

|  name  | length |   description   |
| :----: | :----: | :-------------: |
| Luxury |   3    | Range: 0-120000 |

### Soil moisture properties

|   name   | length |            description            |
| :------: | :----: | :-------------------------------: |
| humidity |  one   | Humidity percentage, range: 0-100 |

### Soil EC properties

|   name   | length |        description        |
| :------: | :----: | :-----------------------: |
| EC value |   2    | Unit us/cm, range: 0-5000 |

### Power attribute

|        name        | length | description  |
| :----------------: | :----: | :----------: |
| Battery percentage |  one   | Range: 0-100 |

### Lock attribute

|  name  | length | description |
| :----: | :----: | :---------: |
| status |  one   |  see below  |

- bit 0: square tongue state (1: eject; 0: retract)
- bit 1: dull state (1: eject; 0: retract)
- bit 2: oblique tongue state (1: eject; 0: retract)
- bit 3: Child lock status (1: open; 0: close)

All normal combination states:

- 0x00: unlock state (all bolts retracted)
- 0x04: The lock tongue pops out (oblique tongue pops out)
- 0x05: Lock + lock tongue eject (square tongue, oblique tongue eject)
- 0x06: Reverse lock + tongue ejection (dull tongue, oblique tongue ejection)
- 0x07: All lock tongues pop out (square tongue, dull tongue, oblique tongue pop out)

### Door attributes

|  name  | length |                         description                          |
| :----: | :----: | :----------------------------------------------------------: |
| status |  one   | Open the door (0x00), close the door (0x01), abnormal (0xFF) |

### Formaldehyde properties

|        name        | length |                         description                          |
| :----------------: | :----: | :----------------------------------------------------------: |
| Formaldehyde value |   2    | The accuracy is 0.01mg/m3, example: 10 00 = 0x0010 means 0.16mg/m3 |

### Binding properties

|  name  | length |         description          |
| :----: | :----: | :--------------------------: |
| status |  one   | Unbound (0x00), bound (0x01) |

### Switch attribute

|  name  | length |        description        |
| :----: | :----: | :-----------------------: |
| switch |  one   | Close (0x00), open (0x01) |

### Consumables remaining quantity attributes

|   name    | length |            description            |
| :-------: | :----: | :-------------------------------: |
| remaining |  one   | Remaining percentage, range 0~100 |

### Flooding properties

|      name       | length |              description               |
| :-------------: | :----: | :------------------------------------: |
| Submerged state |  one   | Submerged (0x01), not submerged (0x00) |

### Smoke properties

|    name     | length |                         description                          |
| :---------: | :----: | :----------------------------------------------------------: |
| Smoke state |  one   | Normal monitoring (0x00), fire alarm (0x01), equipment failure (0x02) |

### Gas properties

|    name    | length |              description               |
| :--------: | :----: | :------------------------------------: |
| Gas status |  one   | With leakage (0x01), no leakage (0x00) |

### Unmanned mobile properties

|      name      | length |                  description                   |
| :------------: | :----: | :--------------------------------------------: |
| length of time |   4    | The duration of the unmanned state, in seconds |

Remarks: If someone moves, you need to report the attribute of unmanned movement with time 0

### Light intensity attributes

|      name       | length |              description               |
| :-------------: | :----: | :------------------------------------: |
| Light intensity |  one   | Strong light (0x01), weak light (0x00) |

Note: If the child device can report accurate illuminance, please use the illuminance attribute (0x1007) to report

### Door magnetic properties

|        name        | length |                         description                          |
| :----------------: | :----: | :----------------------------------------------------------: |
| Door sensor status |  one   | 0x00: open the door, 0x01: close the door, 0x02: not closed after timeout, 0x03: device reset |

### Weight attributes

|         name         | length |  description  |
| :------------------: | :----: | :-----------: |
| Current weight value |   2    | Unit is grams |

### No one moves over time

|  name  | length |                         description                          |
| :----: | :----: | :----------------------------------------------------------: |
| status |  one   | 0 means someone is moving, 1 means no one is moving in X seconds |

> **Note: The** user configures X seconds for unmanned movement on the plug-in side, the firmware side stores this set value, and reports this object when the time expires.

### Smart pillow

|  name  | length |                         description                          |
| :----: | :----: | :----------------------------------------------------------: |
| status |  one   | Current state of the smart pillow: 0x00: "Get out of bed" (not on the pillow); 0x01: "In bed" (lie on the pillow); 0x02-0xFF reserved |

### Measure body temperature in seconds

|       name       | length |       description       |
| :--------------: | :----: | :---------------------: |
| Skin temperature |   2    | Accurate to 0.01 degree |
| PCB temperature  |   2    | Accurate to 0.01 degree |
|      Power       |  one   |   Battery percentage    |

### Huami bracelet attributes

|    name    | length |                description                |
| :--------: | :----: | :---------------------------------------: |
| Step count |   2    |               Current step                |
|   Sleep    |  one   |    Fall asleep (0x01), wake up (0x02)     |
|    RSSI    |  one   | Absolute value of current signal strength |

### Ruimi vacuum cleaner properties

| name | length |                         description                          |
| :--: | :----: | :----------------------------------------------------------: |
| mode |  one   | Charging (0x00), standby (0x01), standard (0x02), strong (0x03), abnormal (0xFF) |
| Gear |  one   |                    Current standard gear                     |

### Black plus bracelet attributes

|    name    | length |       description       |
| :--------: | :----: | :---------------------: |
| Step count |   2    |    Steps in the day     |
| Heart rate |  one   |     Last heart rate     |
|   status   |  one   | Current activity status |