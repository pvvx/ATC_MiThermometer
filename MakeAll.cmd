@Path=E:\Telink\SDK;E:\Telink\SDK\jre\bin;E:\Telink\SDK\opt\tc32\tools;E:\Telink\SDK\opt\tc32\bin;E:\Telink\SDK\usr\bin;E:\Telink\SDK\bin;%PATH%
@set SWVER=_v44
@del /Q "ATC%SWVER%.bin"
make -s -j PROJECT_NAME=ATC%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_LYWSD03MMC"
@if not exist "ATC%SWVER%.bin" goto :error
@del /Q "BTH%SWVER%.bin"
make -s -j PROJECT_NAME=BTH%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD05MMC"
@if not exist "BTH%SWVER%.bin" goto :error
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
@exit
:error
echo "Error!"

         