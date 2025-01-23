@Path=E:\Telink\SDK;E:\Telink\SDK\jre\bin;E:\Telink\SDK\opt\tc32\tools;E:\Telink\SDK\opt\tc32\bin;E:\Telink\SDK\usr\bin;E:\Telink\SDK\bin;%PATH%
@set SWVER=_v50
@del /Q "ATC%SWVER%.bin"
make -s -j PROJECT_NAME=ATC%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_LYWSD03MMC"
@if not exist "ATC%SWVER%.bin" goto :error
@python3 utils\zb_bin_ota.py -m0x1141 -i0x0203 -v0x99993001 -s"Zigbee ver:devis to BLE" ATC%SWVER%.bin zigbee_ota\ATC%SWVER%
@python3 utils\zb_bin_ota.py -m56085 -i0x0203 -v0x99993001 -s"Zigbee ver:devis to BLE" ATC%SWVER%.bin zigbee_ota\ATC%SWVER%
@del /Q "BTH%SWVER%.bin"
make -s -j PROJECT_NAME=BTH%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD05MMC"
@if not exist "BTH%SWVER%.bin" goto :error
@del /Q "BTE%SWVER%.bin"
make -s -j PROJECT_NAME=BTE%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD05MMC_EN"
@if not exist "BTE%SWVER%.bin" goto :error
@del /Q "CGDK2%SWVER%.bin"
make -s -j PROJECT_NAME=CGDK2%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_CGDK2"
@if not exist "CGDK2%SWVER%.bin" goto :error
@del /Q "CGG1%SWVER%.bin"
make -s -j PROJECT_NAME=CGG1%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_CGG1 -DDEVICE_CGG1_ver=0"
@if not exist "CGG1%SWVER%.bin" goto :error
@del /Q "CGG1M%SWVER%.bin"
make -s -j PROJECT_NAME=CGG1M%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_CGG1 -DDEVICE_CGG1_ver=2022"
@if not exist "CGG1M%SWVER%.bin" goto :error
@del /Q "MHO_C401%SWVER%.bin"
make -s -j PROJECT_NAME=MHO_C401%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MHO_C401"
@if not exist "MHO_C401%SWVER%.bin" goto :error
@del /Q "MHO_C401N%SWVER%.bin"
make -s -j PROJECT_NAME=MHO_C401N%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MHO_C401N"
@if not exist "MHO_C401N%SWVER%.bin" goto :error
@del /Q "MHO_C122%SWVER%.bin"
make -s -j PROJECT_NAME=MHO_C122%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MHO_C122"
@if not exist "MHO_C122%SWVER%.bin" goto :error
@del /Q "TNK01%SWVER%.bin"
make -s -j PROJECT_NAME=TNK01%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TNK01"
@if not exist "TNK01%SWVER%.bin" goto :error
@del /Q "TS0201%SWVER%.bin"
make -s -j PROJECT_NAME=TS0201%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TS0201"
@if not exist "TS0201%SWVER%.bin" goto :error
python3 utils\zb_bin_ota.py TS0201%SWVER%.bin zigbee_ota\TS0201BLE%SWVER% -m0x1141 -i0xd3a3 -v0x01983001 -s"Tuya to BLE"
@del /Q "TS0201S1%SWVER%.bin"
make -s -j PROJECT_NAME=TS0201S1%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TS0201 -DUSE_SENSOR_MY18B20=1"
@if not exist "TS0201S1%SWVER%.bin" goto :error
@del /Q "TS0201S2%SWVER%.bin"
make -s -j PROJECT_NAME=TS0201S2%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TS0201 -DUSE_SENSOR_MY18B20=2"
@if not exist "TS0201S2%SWVER%.bin" goto :error
@del /Q "TH03Z%SWVER%.bin"
make -s -j PROJECT_NAME=TH03Z%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TH03Z"
@if not exist "TH03Z%SWVER%.bin" goto :error
@del /Q "ZTH01%SWVER%.bin"
make -s -j PROJECT_NAME=ZTH01%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZTH01"
@if not exist "ZTH01%SWVER%.bin" goto :error
@del /Q "ZTH01S1%SWVER%.bin"
make -s -j PROJECT_NAME=ZTH01S1%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZTH01 -DUSE_SENSOR_MY18B20=1"
@if not exist "ZTH01S1%SWVER%.bin" goto :error
@del /Q "ZTH01S2%SWVER%.bin"
make -s -j PROJECT_NAME=ZTH01S2%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZTH01 -DUSE_SENSOR_MY18B20=2"
@if not exist "ZTH01S2%SWVER%.bin" goto :error
@del /Q "ZTH02%SWVER%.bin"
make -s -j PROJECT_NAME=ZTH02%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZTH02"
@if not exist "ZTH02%SWVER%.bin" goto :error
@del /Q "TB03F%SWVER%.bin"
make -s -j PROJECT_NAME=TB03F%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TB03F"
@if not exist "TB03F%SWVER%.bin" goto :error
@del /Q "TH03%SWVER%.bin"
make -s -j PROJECT_NAME=TH03%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZTH03"
@if not exist "TH03%SWVER%.bin" goto :error
python3 utils\zb_bin_ota.py TH03%SWVER%.bin zigbee_ota\TH03BLE%SWVER% -m0x1286 -i0x0202 -v0x10993607 -s"Tuya to BLE"
@del /Q "LKTMZL02%SWVER%.bin"
make -s -j PROJECT_NAME=LKTMZL02%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_LKTMZL02"
@if not exist "LKTMZL02%SWVER%.bin" goto :error
python3 utils\zb_bin_ota.py LKTMZL02%SWVER%.bin zigbee_ota\LKTMZL02BLE%SWVER% -m0x1141 -i0xd3a3 -v0x01983001 -s"Tuya to BLE"
@del /Q "ZTH05%SWVER%.bin"
make -s -j PROJECT_NAME=ZTH05%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZTH05Z"
@if not exist "ZTH05%SWVER%.bin" goto :error
@del /Q "ZYZTH02%SWVER%.bin"
make -s -j PROJECT_NAME=ZYZTH02%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZYZTH02"
@if not exist "ZYZTH02%SWVER%.bin" goto :error
@exit
:error
echo "Error!"

         