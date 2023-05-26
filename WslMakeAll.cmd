@set SWVER=_v43
@del /Q "ATC%SWVER%.bin"
wsl make -s PROJECT_NAME=ATC%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_LYWSD03MMC"
@if not exist "ATC%SWVER%.bin" goto :error
@del /Q "BTH%SWVER%.bin"
wsl make -s PROJECT_NAME=BTH%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MJWSD05MMC"
@if not exist "BTH%SWVER%.bin" goto :error
@del /Q "CGDK2%SWVER%.bin"
wsl make -s PROJECT_NAME=CGDK2%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_CGDK2"
@if not exist "CGDK2%SWVER%.bin" goto :error
@del /Q "CGG1%SWVER%.bin"
wsl make -s PROJECT_NAME=CGG1%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_CGG1 -DDEVICE_CGG1_ver=0"
@if not exist "CGG1%SWVER%.bin" goto :error
@del /Q "CGG1M%SWVER%.bin"
wsl make -s PROJECT_NAME=CGG1M%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_CGG1 -DDEVICE_CGG1_ver=2022"
@if not exist "CGG1M%SWVER%.bin" goto :error
@del /Q "MHO_C401%SWVER%.bin"
wsl make -s PROJECT_NAME=MHO_C401%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MHO_C401"
@if not exist "MHO_C401%SWVER%.bin" goto :error
@del /Q "MHO_C401N%SWVER%.bin"
wsl make -s PROJECT_NAME=MHO_C401N%SWVER% POJECT_DEF="-DDEVICE_TYPE=DEVICE_MHO_C401N"
@if not exist "MHO_C401N%SWVER%.bin" goto :error
@exit
:error
echo "Error!"

         