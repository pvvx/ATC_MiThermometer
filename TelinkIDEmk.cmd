@set TLSDK=E:\Telink\SDK
@set PATH=%TLSDK%\bin;%TLSDK%\opt\tc32\bin;%TLSDK%\usr\bin;%PATH%
make -j TEL_PATH=E:/Telink/825x/Telink_825X_SDK %1 %2 %3