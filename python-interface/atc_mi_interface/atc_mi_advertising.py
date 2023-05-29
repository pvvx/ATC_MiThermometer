#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#############################################################################
# atc_mi_advertising.py
#############################################################################

import logging
import argparse

import wx
from wx.lib.embeddedimage import PyEmbeddedImage
from construct_gallery import BleakScannerConstruct

from .atc_mi_adv_format import atc_mi_advertising_format
from . import construct_module


class AtcMiConstructFrame(wx.Frame):
    icon_image = PyEmbeddedImage(
        "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAABHNCSVQICAgIfAhkiAAAAHFJ"
        "REFUWIXt1jsKgDAQRdF7xY25cpcWC60kioI6Fm/ahHBCMh+BRmGMnAgEWnvPpzK8dvrFCCCA"
        "coD8og4c5Lr6WB3Q3l1TBwLYPuF3YS1gn1HphgEEEABcKERrGy0E3B0HFJg7C1N/f/kTBBBA"
        "+Vi+AMkgFEvBPD17AAAAAElFTkSuQmCC")

    wx_log = None

    def __init__(
            self,
            *args,
            maximized=False,
            loadfile=None,
            ble_start=False,
            **kwargs):
        super().__init__(*args, **kwargs)

        self.SetTitle("Xiaomi Mijia Thermometer - BLE Advertisement Browser")
        self.SetSize(1000, 600)
        self.SetIcon(self.icon_image.GetIcon())
        self.Center()

        self.status_bar: wx.StatusBar = self.CreateStatusBar()
        self.main_panel = AtcMiBleakScannerConstruct(
            self,
            filter_hint_mac="A4:C1:38",
            filter_hint_name="LYWSD03MMC, ATC",
            key_label="Bindkey",
            description_label="Description",
            loadfile=loadfile,
            ble_start=ble_start,
            gallery_descriptor=construct_module,
            col_name_width=200,
            col_type_width=150)
        self.Bind(wx.EVT_CLOSE, self.on_close)
        if maximized:
            self.Maximize(True)

    def on_close(self, event):
        self.main_panel.on_application_close()
        event.Skip()


class AtcMiBleakScannerConstruct(BleakScannerConstruct):
    def __init__(self, *args, ble_start=False, **kwargs):
        super().__init__(*args, **kwargs)
        if ble_start:
            self.ble_start()

    def bleak_advertising(self, device, advertisement_data):
        format_label, adv_data = atc_mi_advertising_format(advertisement_data)
        if "Unknown" in format_label:
            logging.warning(
                "mac: %s. %s advertisement: %s. RSSI: %s",
                device.address, format_label, advertisement_data,
                advertisement_data.rssi)
            return
        if adv_data:
            self.add_data(
                data=adv_data,
                reference=device.address,
                append_label=format_label
            )
        logging.info(
            "mac: %s. %s advertisement: %s. RSSI: %s",
            device.address, format_label, advertisement_data,
            advertisement_data.rssi)


def main():
    parser = argparse.ArgumentParser(
        prog='atc_mi_advertising',
        epilog='Xiaomi Mijia Thermometer - BLE Advertisement Browser')
    parser.add_argument(
        '-s',
        "--start",
        dest='ble_start',
        action='store_true',
        help="start BLE")
    parser.add_argument(
        '-m',
        "--maximized",
        dest='maximized',
        action='store_true',
        help="display the frame maximized")
    parser.add_argument(
        '-l',
        "--load",
        dest='log_data_file',
        type=argparse.FileType('rb'),
        help="log data file(s) to be automatically loaded at startup.",
        default=0,
        nargs='+',
        metavar='LOG_DATA_FILE')
    parser.add_argument(
        '-i',
        "--inspectable",
        dest='inspectable',
        action='store_true',
        help="enable Inspection (Ctrl-Alt-I)")
    args = parser.parse_args()
    loadfile = None
    if args.log_data_file:
        loadfile = args.log_data_file
    if args.inspectable:
        import wx.lib.mixins.inspection as wit
        app = wit.InspectableApp()
    else:
        app = wx.App(False)
    frame = AtcMiConstructFrame(
        None,
        maximized=args.maximized,
        loadfile=loadfile,
        ble_start=args.ble_start
    )
    frame.Show(True)
    app.MainLoop()


if __name__ == "__main__":
    main()
