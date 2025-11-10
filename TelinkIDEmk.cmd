@set TLSDK=E:\Telink\SDK
@PATH=%TLSDK%\jre\bin;%TLSDK%\bin;%TLSDK%\opt\tc32\bin;%TLSDK%\usr\bin;%TLSDK%\opt\tc32\tools;%PATH%
make -s -j PROJECT_NAME=ATC_v55 POJECT_DEF="-DDEVICE_TYPE=DEVICE_LYWSD03MMC"
