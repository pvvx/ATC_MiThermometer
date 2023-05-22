# Library module used by atc_mi_advertising.py

# This module shall be reloadable, including relevant "construct" submodules

from importlib import reload
import construct_editor.core.custom as custom
from construct_gallery import GalleryItem
from . import atc_mi_construct

# Allow reloading submodules (load_construct_selector)
from . import atc_mi_construct_adapters
reload(atc_mi_construct_adapters)
reload(atc_mi_construct)

# Set custom adapters in construct_editor
custom.add_custom_tunnel(atc_mi_construct.BtHomeCodec, "BtHomeCodec")
custom.add_custom_tunnel(atc_mi_construct.AtcMiCodec, "AtcMiCodec")
custom.add_custom_tunnel(atc_mi_construct.MiLikeCodec, "MiLikeCodec")
custom.add_custom_adapter(
    atc_mi_construct.ExprAdapter,
    "Value",
    custom.AdapterObjEditorType.String)
custom.add_custom_adapter(
    atc_mi_construct.ReversedMacAddress,
    "ReversedMacAddress",
    custom.AdapterObjEditorType.String)
custom.add_custom_adapter(
    atc_mi_construct.MacAddress,
    "MacAddress",
    custom.AdapterObjEditorType.String)

# Set construct_gallery
gallery_descriptor = {
    "general_format": GalleryItem(
        construct=atc_mi_construct.general_format,
    ),
    "custom_format": GalleryItem(
        construct=atc_mi_construct.custom_format,
    ),
    "custom_enc_format": GalleryItem(
        construct=atc_mi_construct.custom_enc_format,
    ),
    "mi_like_format": GalleryItem(
        construct=atc_mi_construct.mi_like_format,
    ),
    "atc1441_format": GalleryItem(
        construct=atc_mi_construct.atc1441_format,
    ),
    "atc1441_enc_format": GalleryItem(
        construct=atc_mi_construct.atc1441_enc_format,
    ),
    "bt_home_format": GalleryItem(
        construct=atc_mi_construct.bt_home_format,
    ),
    "bt_home_enc_format": GalleryItem(
        construct=atc_mi_construct.bt_home_enc_format,
    ),
}
