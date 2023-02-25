# Library module used by atc_mi_advertising.py

# This module shall be reloadable, including relevant "construct" submodules

import construct_atc_mi
from construct_gallery import GalleryItem
from importlib import reload
import construct_editor.core.custom as custom

# Allow reloading submodules (load_construct_selector)
import construct_atc_mi_adapters
reload(construct_atc_mi_adapters)
reload(construct_atc_mi)

# Set custom adapters in construct_editor
custom.add_custom_tunnel(construct_atc_mi.BtHomeCodec, "BtHomeCodec")
custom.add_custom_tunnel(construct_atc_mi.AtcMiCodec, "AtcMiCodec")
custom.add_custom_tunnel(construct_atc_mi.MiLikeCodec, "MiLikeCodec")
custom.add_custom_adapter(
    construct_atc_mi.ExprAdapter,
    "Value",
    custom.AdapterObjEditorType.String)
custom.add_custom_adapter(
    construct_atc_mi.ReversedMacAddress,
    "ReversedMacAddress",
    custom.AdapterObjEditorType.String)
custom.add_custom_adapter(
    construct_atc_mi.MacAddress,
    "MacAddress",
    custom.AdapterObjEditorType.String)

# Set construct_gallery
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
