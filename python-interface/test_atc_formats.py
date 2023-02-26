import construct_atc_mi
import wx
import sys
from construct_gallery import ConstructGallery, GalleryItem
import construct_editor.core.custom as custom
from collections import OrderedDict

custom.add_custom_tunnel(construct_atc_mi.BtHomeCodec, "BtHomeCodec")
custom.add_custom_tunnel(construct_atc_mi.AtcMiCodec, "AtcMiCodec")
custom.add_custom_tunnel(construct_atc_mi.MiLikeCodec, "MiLikeCodec")
custom.add_custom_adapter(
    construct_atc_mi.ExprAdapter,
    "ComputedValue",
    custom.AdapterObjEditorType.String)
custom.add_custom_adapter(
    construct_atc_mi.ReversedMacAddress,
    "ReversedMacAddress",
    custom.AdapterObjEditorType.String)
custom.add_custom_adapter(
    construct_atc_mi.MacAddress,
    "MacAddress",
    custom.AdapterObjEditorType.String)

app = wx.App(False)
frame = wx.Frame(None, title="ATC MI Formats", size=(1000, 500))

gallery_descriptor = {
    "general_format": GalleryItem(
        construct=construct_atc_mi.general_format,
    ),
    "custom_format": GalleryItem(
        construct=construct_atc_mi.custom_format,
    ),
    "custom_enc_format": GalleryItem(
        construct=construct_atc_mi.custom_enc_format,
    ),
    "mi_like_format": GalleryItem(
        construct=construct_atc_mi.mi_like_format,
    ),
    "atc1441_format": GalleryItem(
        construct=construct_atc_mi.atc1441_format,
    ),
    "atc1441_enc_format": GalleryItem(
        construct=construct_atc_mi.atc1441_enc_format,
    ),
    "bt_home_format": GalleryItem(
        construct=construct_atc_mi.bt_home_format,
    ),
    "bt_home_enc_format": GalleryItem(
        construct=construct_atc_mi.bt_home_enc_format,
    ),
}

ordered_samples = OrderedDict(
# ordered_samples OrderedDict for the overall data, independent from the
# previous gallery_descriptor. Notice that if a reference is used "mac_address",
# reference_label and key_label must be set in ConstructGallery
    [
        (
            "custom",
            {
                "binary": bytes.fromhex(
                    "12 16 1a 18 cc bb aa 38 c1 a4 6c 07 fa 13 d6 0a 52 09 0f"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "custom_enc",
            {
                "binary": bytes.fromhex(
                    "0e 16 1a 18 bd 86 c5 3f fa b9 00 c1 51 58 59"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "atc1441",
            {
                "binary": bytes.fromhex(
                    "10 16 1a 18 a4 c1 38 aa bb cc 00 ce 33 43 0a 6e c3"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "atc1441_enc",
            {
                "binary": bytes.fromhex(
                    "0b 16 1a 18 bd e9 b7 5d b8 56 f3 03"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "bt_home (temp, humidity, batt%)",
            {
                "binary": bytes.fromhex(
                    "11 16 1c 18 02 00 56 23 02 6c 07 03 03 ff 13 02 01 4e"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "bt_home (batt v)",
            {
                "binary": bytes.fromhex(
                    "0d 16 1c 18 02 00 63 02 10 01 03 0c d9 0a"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "bt_home_enc (temp, humidity, batt%)",
            {
                "binary": bytes.fromhex(
                    "16 16 1e 18 83 93 db a2 55 4c d8 04 be ab 78 cf b3 00 "
                    "00 7d 6a 1c d2"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "bt_home_enc (batt v)",
            {
                "binary": bytes.fromhex(
                    "12 16 1e 18 12 79 94 44 eb 3a 3f e4 b3 00 00 59 62 3a 29"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "mi_like (temp, humidity)",
            {
                "binary": bytes.fromhex(
                    "15 16 95 fe 50 58 5b 05 1b cc bb aa 38 c1 a4 0d 10 04 "
                    "be 00 00 02"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "mi_like (batt)",
            {
                "binary": bytes.fromhex(
                    "12 16 95 fe 50 58 5b 05 22 cc bb aa 38 c1 a4 0a 10 01 "
                    "4f"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "mi_like_enc (temperature)",
            {
                "binary": bytes.fromhex(
                    "1a 16 95 fe 58 58 5b 05 f4 cc bb aa 38 c1 a4 13 df 92 "
                    "a8 1c b3 00 00 ac c4 6f da"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "mi_like_enc (humidity %)",
            {
                "binary": bytes.fromhex(
                    "1a 16 95 fe 58 58 5b 05 95 cc bb aa 38 c1 a4 d5 25 d9 "
                    "74 ec 14 00 00 9b e1 de 3c"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
        (
            "mi_like_enc (battery %)",
            {
                "binary": bytes.fromhex(
                    "19 16 95 fe 58 58 5b 05 ed cc bb aa 38 c1 a4 de 6d f0 "
                    "74 b3 00 00 f2 ef 0e 53"),
                "mac_address": "A4:C1:38:AA:BB:CC",
            },
        ),
    ]
)

ref_key_descriptor = {  # ref_key_descriptor dictionary for the overall data
    "A4:C1:38:AA:BB:CC": {"bindkey": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"}
}


ConstructGallery(frame,
    gallery_descriptor=gallery_descriptor,
    ordered_samples=ordered_samples,
    ref_key_descriptor=ref_key_descriptor,
    reference_label="MAC address",
    key_label="Bindkey",
    description_label="Description",
    col_name_width=200,
    col_type_width=150
)

frame.Show(True)
app.MainLoop()
