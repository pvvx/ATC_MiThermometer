@Path=E:\Telink\SDK;E:\Telink\SDK\jre\bin;E:\Telink\SDK\opt\tc32\tools;E:\Telink\SDK\opt\tc32\bin;E:\Telink\SDK\usr\bin;E:\Telink\SDK\bin;%PATH%
@set SWVER=_v52
@set FW_NAME=TB_INA226%SWVER%
@del /Q "%FW_NAME%.bin"
make -s -j PROJECT_NAME=%FW_NAME% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TB03F -DUSE_SENSOR_INA226=1"
@if not exist "%FW_NAME%.bin" goto :error
@set FW_NAME=TB_ENS160%SWVER%
@del /Q "%FW_NAME%.bin"
make -s -j PROJECT_NAME=%FW_NAME% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TB03F -DUSE_SENSOR_ENS160=1"
@if not exist "%FW_NAME%.bin" goto :error
@set FW_NAME=TB_SCD41%SWVER%
@del /Q "%FW_NAME%.bin"
make -s -j PROJECT_NAME=%FW_NAME% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TB03F -DUSE_SENSOR_SCD41=1"
@if not exist "%FW_NAME%.bin" goto :error
@set FW_NAME=TB_BME280%SWVER%
@del /Q "%FW_NAME%.bin"
make -s -j PROJECT_NAME=%FW_NAME% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TB03F -DUSE_SENSOR_BME280=1"
@if not exist %FW_NAME%.bin goto :error
@set FW_NAME=TB_INA3221%SWVER%
@del /Q "%FW_NAME%.bin"
make -s -j PROJECT_NAME=%FW_NAME% POJECT_DEF="-DDEVICE_TYPE=DEVICE_TB03F -DUSE_SENSOR_INA3221=1"
@if not exist "%FW_NAME%.bin" goto :error
@set FW_NAME=TB_PLM%SWVER%
@del /Q "%FW_NAME%.bin"
make -s -j PROJECT_NAME=%FW_NAME% POJECT_DEF="-DDEVICE_TYPE=DEVICE_PLM1 -DTEST_PLM1=1"
@if not exist "%FW_NAME%.bin" goto :error
@exit
:error
echo "Error!"

         