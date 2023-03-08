from construct_atc_mi_adapters import *

# -------------- custom_format -------------------------------------------------
# "PVVX (Custom)" advertising type, encrypted beacon unchecked

# https://github.com/pvvx/ATC_MiThermometer#custom-format-all-data-little-endian
# Min. firmware Version: 0.8

atc_flag = BitStruct(  # GPIO_TRG pin (marking "reset" on circuit board) flags:
    Padding(3),
    "humidity_trigger" / Flag,  # Bit 4 - Humidity trigger event
    "temp_trigger" / Flag,  # Bit 3 - Temperature trigger event
    "out_gpio_trg_flag" / Flag,  # Bit 2 - If this flag is set, the output GPIO_TRG pin is controlled according to the set parameters threshold temperature or humidity
    "out_gpio_trg_value" / Flag,  # Bit 1 - GPIO_TRG pin output value (pull Up/Down)
    "input_gpio_value" / Flag,  # Bit 0 - Reed Switch, input
)

custom_format = Struct(
    "version" / Computed(1),
    "size" / Int8ul,  # 18 (0x12)
    "uid" / Int8ul,  # 22 (0x16, 16-bit UUID)
    "UUID" / ByteSwapped(Const(b"\x18\x1a")),  # GATT Service 0x181A Environmental Sensing
    "MAC" / ReversedMacAddress,  # [0] - lo, .. [6] - hi digits
    "mac_vendor" / MacVendor,
    "temperature" / Int16sl_x100,
    "temperature_unit" / Computed("°C"),
    "humidity" / Int16ul_x100,
    "humidity_unit" / Computed("%"),
    "battery_v" / Int16ul_x1000,
    "battery_v_unit" / Computed("V"),
    "battery_level" / Int8ul,  # 0..100 %
    "battery_level_unit" / Computed("%"),
    "counter" / Int8ul,  # measurement count
    "flags" / atc_flag
)

# -------------- custom_enc_format ---------------------------------------------
# "PVVX (Custom)" advertising type, encrypted beacon checked

custom_enc_format = Struct(
    "version" / Computed(1),
    "size" / Int8ul,  # 14 (0x0e)
    "uid" / Int8ul,  # 22 (0x16, 16-bit UUID)
    "UUID" / ByteSwapped(Const(b"\x18\x1a")),  # GATT Service 0x181A Environmental Sensing
    "codec" / AtcMiCodec(
        Aligned(11,
            Struct(
                "temperature" / Int16sl_x100,
                "temperature_unit" / Computed("°C"),
                "humidity" / Int16ul_x100,
                "humidity_unit" / Computed("%"),
                "battery_level" / Int8ul,  # 0..100 %
                "battery_level_unit" / Computed("%"),
                "flags" / atc_flag
            )
        ),
        size_payload=6,
    ),
)

# -------------- atc1441_format ------------------------------------------------
# "ATC1441" advertising type, encrypted beacon unchecked

# https://github.com/pvvx/ATC_MiThermometer#atc1441-format

atc1441_format = Struct(
    "version" / Computed(1),
    "size" / Int8ul,  # 18
    "uid" / Int8ul,  # 0x16, 16-bit UUID
    "UUID" / ByteSwapped(Const(b"\x18\x1a")),  # GATT Service 0x181A Environmental Sensing
    "MAC" / MacAddress,  # [0] - hi, .. [6] - lo digits
    "mac_vendor" / MacVendor,
    "temperature" / Int16sb_x10,
    "temperature_unit" / Computed("°C"),
    "humidity" / Int8ul,  # 0..100 %
    "humidity_unit" / Computed("%"),
    "battery_level" / Int8ul,  # 0..100 %
    "battery_level_unit" / Computed("%"),
    "battery_v" / Int16ub_x1000,
    "battery_v_unit" / Computed("V"),
    "counter" / Int8ub  # frame packet counter
)

# -------------- atc1441_enc_format ------------------------------------------------
# "ATC1441" advertising type, encrypted beacon checked

# encrypted custom beacon
# https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-842846036

atc1441_enc_format = Struct(
    "version" / Computed(1),
    "size" / Int8ul,  # 14 (0x0e)
    "uid" / Int8ul,  # 22 (0x16, 16-bit UUID)
    "UUID" / ByteSwapped(Const(b"\x18\x1a")),  # GATT Service 0x181A Environmental Sensing
    "codec" / AtcMiCodec(
        Aligned(8,
            Struct(
                "temperature" / ExprAdapter(Int8sl,  # -40...87 °C with half degree precision
                    obj_ / 2 - 40, lambda obj, ctx: int((float(obj) + 40) * 2)),
                "temperature_unit" / Computed("°C"),
                "humidity" / ExprAdapter(Int8ul,  # half unit precision
                    obj_ / 2, lambda obj, ctx: int(float(obj) * 2)),
                "humidity_unit" / Computed("%"),
                "batt_trg" / BitStruct(
                    "out_gpio_trg_flag" / Flag,  # If this flag is set, the output GPIO_TRG pin is controlled according to the set parameters threshold temperature or humidity
                    "battery_level" / BitsInteger(7),  # 0..100 %
                    "battery_level_unit" / Computed("%"),
                )
            )
        ),
        size_payload=3,
    ),
)

# -------------- mi_like_format ------------------------------------------------
# "MIJIA (MiHome)" advertising type, encrypted beacon either checked or unchecked

# Can be clear or encrypted
# https://github.com/pvvx/ATC_MiThermometer/tree/master/InfoMijiaBLE

mi_like_data = Struct(  # https://github.com/pvvx/ATC_MiThermometer/blob/master/src/mi_beacon.h#L72-L97
    "type" / Select(
        Enum(Int16ul,
            XIAOMI_DATA_ID_Sleep                =0x1002,
            XIAOMI_DATA_ID_RSSI                 =0x1003,
            XIAOMI_DATA_ID_Temperature          =0x1004,
            XIAOMI_DATA_ID_Humidity             =0x1006,
            XIAOMI_DATA_ID_LightIlluminance     =0x1007,
            XIAOMI_DATA_ID_SoilMoisture         =0x1008,
            XIAOMI_DATA_ID_SoilECvalue          =0x1009,
            XIAOMI_DATA_ID_Power                =0x100A,  # Battery
            XIAOMI_DATA_ID_TempAndHumidity      =0x100D,
            XIAOMI_DATA_ID_Lock                 =0x100E,
            XIAOMI_DATA_ID_Gate                 =0x100F,
            XIAOMI_DATA_ID_Formaldehyde         =0x1010,
            XIAOMI_DATA_ID_Bind                 =0x1011,
            XIAOMI_DATA_ID_Switch               =0x1012,
            XIAOMI_DATA_ID_RemAmCons            =0x1013,  # Remaining amount of consumables
            XIAOMI_DATA_ID_Flooding             =0x1014,
            XIAOMI_DATA_ID_Smoke                =0x1015,
            XIAOMI_DATA_ID_Gas                  =0x1016,
            XIAOMI_DATA_ID_NoOneMoves           =0x1017,
            XIAOMI_DATA_ID_LightIntensity       =0x1018,
            XIAOMI_DATA_ID_DoorSensor           =0x1019,
            XIAOMI_DATA_ID_WeightAttributes     =0x101A,
            XIAOMI_DATA_ID_NoOneMovesOverTime   =0x101B,  # No one moves over time
            XIAOMI_DATA_ID_SmartPillow          =0x101C,
            UNBOUND_DEVICE                      =0x0128,
        ),
        Enum(Int8ul,
            XIAOMI_UNBOUND                      =0x08,
        )
    ),
    "data" / Switch(this.type,  # https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/Mijia%20BLE%20Object%20Definition.md
        {
            "XIAOMI_DATA_ID_Temperature": Struct(  # 04
                "type_length" / Const(b"\x02"),
                "temperature" / Int16sl_x10,
                "temperature_unit" / Computed("°C"),
            ),
            "XIAOMI_DATA_ID_Humidity": Struct(  # 06
                "type_length" / Const(b"\x02"),  # ranging from 0-1000
                "humidity" / Int16ul_x10,  # 0..100 %
                "humidity_unit" / Computed("%"),
            ),
            "XIAOMI_DATA_ID_SoilECvalue": Struct(  # 09
                "type_length" / Const(b"\x02"),
                "conductivity" / Int16sl_x10,  # range: 0-5000
                "conductivity_unit" / Computed("uS/cm"),
            ),
            "XIAOMI_DATA_ID_Power": Struct(  # 0A
                "battery_length" / Const(b"\x01"),
                "battery_level" / Int8ul,  # 0..100 %
                "battery_level_unit" / Computed("%"),
            ),
            "XIAOMI_DATA_ID_TempAndHumidity": Struct(  # 0D
                "type_length" / Const(b"\x04"),
                "temperature" / Int16sl_x10,
                "temperature_unit" / Computed("°C"),
                "humidity" / Int16ul_x10,
                "humidity_unit" / Computed("%"),
            ),
            "XIAOMI_DATA_ID_SoilMoisture": Struct(  # 08
                "type_length" / Const(b"\x01"),
                "moisture_level" / Int8ul,  # 0..100 %
                "moisture_level_unit" / Computed("%"),
            ),
            "XIAOMI_DATA_ID_LightIlluminance": Struct(  # 07
                "type_length" / Const(b"\x03"),
                "illuminance" / Int24ul,  # Range: 0-120000
                "illuminance_unit" / Computed("L"),
            ),
            "UNBOUND_DEVICE": Const(b"\x00"),
        }
    )
)

mi_like_format = Struct(
    "version" / Computed(2),
    "size" / Int8ul,  # e.g., 21
    "uid" / Int8ul,  # 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
    "UUID" / ByteSwapped(Const(b"\xfe\x95")),  # 16-bit UUID for Members 0xFE95 Xiaomi Inc.
    "ctrl" / BitStruct(  # Frame Control (https://github.com/pvvx/ATC_MiThermometer/blob/master/src/mi_beacon.h#L104-L124)
        "Mesh" / Flag,  # 0: does not include Mesh; 1: includes Mesh. For standard BLE access products and high security level access, this item is mandatory to 0. This item is mandatory for Mesh access to 1. For more information about Mesh access, please refer to Mesh related documents
        "Object_Include" / Flag,  # 0: does not contain Object; 1: contains Object
        "Capability_Include" / Flag,  # 0: does not include Capability; 1: includes Capability. Before the device is bound, this bit is forced to 1
        "MAC_Include" / Flag,  # 0: Does not include the MAC address; 1: includes a fixed MAC address (the MAC address is included for iOS to recognize this device and connect)
        "isEncrypted" / Flag,  # 0: The package is not encrypted; 1: The package is encrypted
        "Reserved" / BitsInteger(3),
        "version" / BitsInteger(4),  # Version number (currently v5)
        "Auth_Mode" / BitsInteger(2),  # 0: old version certification; 1: safety certification; 2: standard certification; 3: reserved
        "solicited" / Flag,  # 0: No operation; 1: Request APP to register and bind. It is only valid when the user confirms the pairing by selecting the device on the developer platform, otherwise set to 0. The original name of this item was bindingCfm, and it was renamed to solicited "actively request, solicit" APP for registration and binding
        "registered" / Flag,  # 0: The device is not bound; 1: The device is registered and bound. This item is used to indicate whether the device is reset
    ),
    "device_id" / Enum(Int16ul,  # Device type (https://github.com/pvvx/ATC_MiThermometer/blob/master/src/mi_beacon.h#L14-L35)
        XIAOMI_DEV_ID_LYWSDCGQ       = 0x01AA,
        XIAOMI_DEV_ID_CGG1           = 0x0347,
        XIAOMI_DEV_ID_CGG1_ENCRYPTED = 0x0B48,
        XIAOMI_DEV_ID_CGDK2          = 0x066F,
        XIAOMI_DEV_ID_LYWSD02        = 0x045B,
        XIAOMI_DEV_ID_LYWSD03MMC     = 0x055B,
        XIAOMI_DEV_ID_CGD1           = 0x0576,
        XIAOMI_DEV_ID_MHO_C303       = 0x06d3,
        XIAOMI_DEV_ID_MHO_C401       = 0x0387,
        XIAOMI_DEV_ID_JQJCY01YM      = 0x02DF,
        XIAOMI_DEV_ID_HHCCJCY01      = 0x0098,
        XIAOMI_DEV_ID_GCLS002        = 0x03BC,
        XIAOMI_DEV_ID_HHCCPOT002     = 0x015D,
        XIAOMI_DEV_ID_WX08ZM         = 0x040A,
        XIAOMI_DEV_ID_MCCGQ02HL      = 0x098B,
        XIAOMI_DEV_ID_YM_K1501       = 0x0083,
        XIAOMI_DEV_ID_YM_K1501EU     = 0x0113,
        XIAOMI_DEV_ID_V_SK152        = 0x045C,
        XIAOMI_DEV_ID_SJWS01LM       = 0x0863,
        XIAOMI_DEV_ID_MJYD02YL       = 0x07F6
    ),
    "counter" / Int8ul,  # 0..0xff..0 frame/measurement count
    "MAC" / ReversedMacAddress,  # [0] - lo, .. [6] - hi digits
    "mac_vendor" / MacVendor,
    "data_point" / Switch(this.ctrl.isEncrypted,
        {
            True: MiLikeCodec(
                Struct(
                    "count_id" / Int24ul,
                    "payload" / GreedyRange(mi_like_data)
                ),
                size_payload=5,
            ),
            False: GreedyRange(mi_like_data)
        }
    )
)

# -------------- bt_home_format ------------------------------------------------
# "BTHome" advertising type, encrypted beacon unchecked

bt_home_data = Struct(
    "bt_home_type" / Enum(Int16ub,
        # a: 0=uint (unsigned), 2=sint (signed), 4=float, 6=string, 8=MAC
        # b: number of bytes; ab=SD; cc=DID=device id
        #                          abcc
        BT_HOME_packet_id       =0x0200,  # uint8
        BT_HOME_battery         =0x0201,  # uint8, %
        BT_HOME_temperature     =0x2302,  # sint16, 0.01 °C
        BT_HOME_humidity        =0x0303,  # uint16, 0.01 %
        BT_HOME_pressure        =0x0404,  # uint24, 0.01 hPa
        BT_HOME_illuminance     =0x0405,  # uint24, 0.01 lux
        BT_HOME_weight          =0x0306,  # uint16, 0.01 kg
        BT_HOME_dewpoint        =0x2308,  # sint16, 0.01 °C
        BT_HOME_count_i         =0x0209,  # uint8
        BT_HOME_count_s         =0x0309,  # uint16
        BT_HOME_count_m         =0x0409,  # uint24
        BT_HOME_count_l         =0x0509,  # uint32
        BT_HOME_energy          =0x040a,  # uint24, 0.001 kWh
        BT_HOME_power           =0x040b,  # uint24, 0.01 W
        BT_HOME_voltage         =0x030c,  # uint16, 0.001 V
        BT_HOME_pm2x5           =0x030d,  # uint16, kg/m3
        BT_HOME_pm10            =0x030e,  # uint16, kg/m3
        BT_HOME_boolean         =0x020f,  # uint8
        BT_HOME_switch          =0x0210,  # uint8
        BT_HOME_opened          =0x0211,  # uint8
    ),
    "data" / Switch(this.bt_home_type,
        {
            "BT_HOME_packet_id": Struct(
                "packet_id" / Int8ul,  # integer (0..255)
            ),
            "BT_HOME_count_l": Struct(
                "count_l" / Int32ul,  # integer (0..4294967295)
            ),
            "BT_HOME_count": Struct(
                "count" / Int8ul,  # integer (0..255)
            ),
            "BT_HOME_boolean": BitStruct(
                Padding(7),
                "boolean" / Flag,  # boolean
            ),
            "BT_HOME_switch": BitStruct(
                Padding(7),
                "switch" / Flag,  # boolean
            ),
            "BT_HOME_opened": BitStruct(
                Padding(7),
                "opened" / Flag,  # boolean
            ),
            "BT_HOME_voltage": Struct(
                "battery_v" / Int16ul_x1000,
                "battery_v_unit" / Computed("V"),
            ),
            "BT_HOME_temperature": Struct(
                "temperature" / Int16sl_x100,
                "temperature_unit" / Computed("°C"),
            ),
            "BT_HOME_humidity": Struct(
                "humidity" / Int16ul_x100,
                "humidity_unit" / Computed("%"),
            ),
            "BT_HOME_battery": Struct(
                "battery_level" / Int8ul,  # 0..100 %
                "battery_level_unit" / Computed("%"),
            ),
        }
    )
)

# https://github.com/custom-components/ble_monitor/issues/548

bt_home_format = Struct(  # Simplified formatting
    "version" / Computed(1),
    "size" / Int8ul,
    "uid" / Int8ul,  # 0x16, 16-bit UUID
    "UUID" / ByteSwapped(Const(b"\x18\x1c")),  # BT_HOME_GATT, SERVICE_UUID_USER_DATA, HA_BLE, no security
    "bt_home_data" / GreedyRange(bt_home_data)
)

# -------------- bt_home_enc_format ------------------------------------------------
# "BTHome" advertising type, encrypted beacon checked

bt_home_enc_format = Struct(  # Simplified formatting
    "version" / Computed(1),
    "size" / Int8ul,
    "uid" / Int8ul,  # 0x16, 16-bit UUID
    "UUID" / ByteSwapped(Const(b"\x18\x1e")),  # Bond Management Service
    "codec" / BtHomeCodec(
        Struct(
            "count_id" / Int32ul,  # https://github.com/custom-components/ble_monitor/issues/548#issuecomment-1059874327
            "payload" / GreedyRange(bt_home_data)
        ),
        size_payload=11,
    ),
)

# -------------- general_format ------------------------------------------------
# All format types are embraced

general_format = Struct(
    "version" / Computed(1),
    "custom_enc_format" / GreedyRange(custom_enc_format),
    "custom_format" / GreedyRange(custom_format),
    "atc1441_enc_format" / GreedyRange(atc1441_enc_format),
    "atc1441_format" / GreedyRange(atc1441_format),
    "mi_like_format" / GreedyRange(mi_like_format),
    "bt_home_format" / GreedyRange(bt_home_format),
    "bt_home_enc_format" / GreedyRange(bt_home_enc_format),
)

# -------------- LYWSD03MMC native structures ----------------------------------

# BLE client connection, characteristic id 53 (Temperature and Humidity):
native_temp_hum_v_values = Struct(
    "version" / Computed(1),
    "temperature" / Int16sl_x100,
    "temperature_unit" / Computed("°C"),
    "humidity" / Int8ul,  # 0..100 %
    "humidity_unit" / Computed("%"),
    "battery_v" / Int16ul_x1000,
    "battery_v_unit" / Computed("V"),
)

# BLE client connection, characteristic id 66 (comfortable temp and humi):
native_comfort_values = Struct(
    "version" / Computed(1),
    "temperature_high" / Int16sl_x100,
    "temperature_low" / Int16sl_x100,
    "humidity_high" / Int8ul,  # 0..100 %
    "humidity_low" / Int8ul,  # 0..100 %
    "humidity_unit" / Computed("%"),
)
