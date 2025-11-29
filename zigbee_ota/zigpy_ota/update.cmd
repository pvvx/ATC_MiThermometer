set BASE_URL="https://github.com/pvvx/ATC_MiThermometer/raw/master/zigbee_ota"
echo [] > index.json
for %%a in (../1141-????-99993001-*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\index%1.json

echo [] > index.json
for %%a in (../1286-0202-10993607-*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\th03_tuya2ble.json

echo [] > index.json
for %%a in (../1286-0203-10983001-*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\zg227z_tuya2ble.json

echo [] > index.json
for %%a in (../db15-0203-99993001-*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
for %%a in (../1141-0203-99993001-ATC_*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\devbis2ble.json

echo [] > index.json
for %%a in (../1141-d3a3-01983001-LKTMZL02BLE*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\lktml02_tuya2ble.json

echo [] > index.json
for %%a in (../1141-d3a3-01983001-TS0201BLE*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\ts0201_tuya2ble.json

echo [] > index.json
for %%a in (../1002-0203-60993001-ZYZTH02BLE_*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\zyzth02_tuya2ble.json

echo [] > index.json
for %%a in (../1002-0203-60993001-ZYZTH02PBLE_*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\zyzth02p_tuya2ble.json

echo [] > index.json
for %%a in (../1286-0202-10533607-ZBTH01BLE_*.zigbee) do (
start /wait node scripts/add.js ../%%a %BASE_URL%
)
copy /Y index.json ..\zbth01_tuya2ble.json



del *.zigbee
del index.json



