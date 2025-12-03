@Path=E:\Telink\SDK;E:\Telink\SDK\jre\bin;E:\Telink\SDK\opt\tc32\tools;E:\Telink\SDK\opt\tc32\bin;E:\Telink\SDK\usr\bin;E:\Telink\SDK\bin;%PATH%
@set SWVER=_v56
@del /Q "ATC%SWVER%.bin"
make -s -j PROJECT_NAME=ATC%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_LYWSD03MMC"
@if not exist "ATC%SWVER%.bin" goto :error
@python3 utils\zb_bin_ota.py -m0x1141 -i0x0203 -v0x99993001 -s"Zigbee devis to BLE" ATC%SWVER%.bin zigbee_ota\ATC%SWVER%
@python3 utils\zb_bin_ota.py -m0xDB15 -i0x0203 -v0x99993001 -s"Zigbee devis to BLE" ATC%SWVER%.bin zigbee_ota\ATC%SWVER%
@del /Q "BTH%SWVER%.bin"
make -s -j PROJECT_NAME=BTH%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD05MMC"
@if not exist "BTH%SWVER%.bin" goto :error
@del /Q "BTE%SWVER%.bin"
make -s -j PROJECT_NAME=BTE%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD05MMC_EN"
@if not exist "BTE%SWVER%.bin" goto :error
@del /Q "MJ6%SWVER%.bin"
make -s -j PROJECT_NAME=MJ6%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD06MMC"
@if not exist "MJ6%SWVER%.bin" goto :error
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
@del /Q "PLM1%SWVER%.bin"
make -s -j PROJECT_NAME=PLM1%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_PLM1"
@if not exist "PLM1%SWVER%.bin" goto :error
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
python3 utils\zb_bin_ota.py ZYZTH02%SWVER%.bin zigbee_ota\ZYZTH02BLE%SWVER% -m0x1002 -i0x0203 -v0x60993001 -s"Tuya to BLE"
@del /Q "ZYZTH02S1%SWVER%.bin"
make -s -j PROJECT_NAME=ZYZTH02S1%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZYZTH02 -DUSE_SENSOR_MY18B20=1"
@if not exist "ZYZTH02S1%SWVER%.bin" goto :error
@del /Q "ZYZTH02S2%SWVER%.bin"
make -s -j PROJECT_NAME=ZYZTH02S2%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZYZTH02 -DUSE_SENSOR_MY18B20=2"
@if not exist "ZYZTH02S2%SWVER%.bin" goto :error
@del /Q "ZYZTH02P%SWVER%.bin"
make -s -j PROJECT_NAME=ZYZTH02P%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZYZTH01"
@if not exist "ZYZTH02P%SWVER%.bin" goto :error
python3 utils\zb_bin_ota.py ZYZTH02P%SWVER%.bin zigbee_ota\ZYZTH02PBLE%SWVER% -m0x1002 -i0x0203 -v0x60993001 -s"Tuya to BLE"
@del /Q "ZG227Z%SWVER%.bin"
make -s -j PROJECT_NAME=ZG227Z%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZG_227Z"
@if not exist "ZG227Z%SWVER%.bin" goto :error
python3 utils\zb_bin_ota.py ZG227Z%SWVER%.bin zigbee_ota\ZG227ZBLE%SWVER% -m0x1286 -i0x0203 -v0x10983001 -s"Tuya to BLE"
@del /Q "ZG303Z%SWVER%.bin"
make -s -j PROJECT_NAME=ZG303Z%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZG303Z"
@if not exist "ZG303Z%SWVER%.bin" goto :error
@rem python3 utils\zb_bin_ota.py ZG303Z%SWVER%.bin zigbee_ota\ZG303ZBLE%SWVER% -m0x1286 -i0x0203 -v0x10983001 -s"Tuya to BLE"
@del /Q "ZBTH01%SWVER%.bin"
make -s -j PROJECT_NAME=ZBTH01%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZBEACON_TH01"
@if not exist "ZBTH01%SWVER%.bin" goto :error
python3 utils\zb_bin_ota.py ZBTH01%SWVER%.bin zigbee_ota\ZBTH01BLE%SWVER% -m0x1286 -i0x0202 -v0x10533607 -s"Tuya to BLE"
@del /Q "ZB_MC%SWVER%.bin"
make -s -j PROJECT_NAME=ZB_MC%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_ZB_MC"
@if not exist "ZB_MC%SWVER%.bin" goto :error
cd .\zigbee_ota\zigpy_ota
call update.cmd %SWVER%
cd ..\..
@exit
:error
echo "Error!"

         