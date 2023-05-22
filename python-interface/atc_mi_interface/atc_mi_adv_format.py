gatt_dict = {
    "atc1441": {
        "gatt": '0000181a-0000-1000-8000-00805f9b34fb',  # Environmental Sensing
        "length": 13,
        "header": bytes.fromhex("161a18"),
    },
    "custom": {
        "gatt": '0000181a-0000-1000-8000-00805f9b34fb',
        "length": 15,
        "header": bytes.fromhex("161a18"),
    },
    "custom_enc": {
        "gatt": '0000181a-0000-1000-8000-00805f9b34fb',
        "length": 11,
        "header": bytes.fromhex("161a18"),
    },
    "atc1441_enc": {
        "gatt": '0000181a-0000-1000-8000-00805f9b34fb',
        "length": 8,
        "header": bytes.fromhex("161a18"),
    },
    "mi_like": {
        "gatt": '0000fe95-0000-1000-8000-00805f9b34fb',  # Xiaomi Inc.
        "length": None,
        "header": bytes.fromhex("1695fe"),
    },
    "bt_home": {
        "gatt": '0000181c-0000-1000-8000-00805f9b34fb',  # SERVICE_UUID_USER_DATA, HA_BLE, no security
        "length": None,
        "header": bytes.fromhex("161c18"),
    },
    "bt_home_enc": {
        "gatt": '0000181e-0000-1000-8000-00805f9b34fb',
        "length": None,
        "header": bytes.fromhex("161e18"),
    }
}


def atc_mi_advertising_format(advertisement_data):
    if not advertisement_data.service_data:
        return "", ""
    invalid_length = None
    for t in gatt_dict.keys():
        gatt_d = gatt_dict[t]
        if gatt_d["gatt"] in advertisement_data.service_data:
            payload = advertisement_data.service_data[gatt_d["gatt"]]
            if gatt_d["length"] and len(payload) != gatt_d["length"]:
                invalid_length = len(payload)
                continue
            header = gatt_d["header"]
            return t, bytes([len(header) + len(payload)]) + header + payload
    if invalid_length is not None:
        return "Unknown-length-" + str(invalid_length), ""
    return "Unknown", ""
