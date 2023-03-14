@set TLSDK=E:\Telink\SDK
@PATH=%TLSDK%\jre\bin;%TLSDK%\bin;%TLSDK%\opt\tc32\bin;%TLSDK%\usr\bin;%TLSDK%\opt\tc32\tools;%PATH%
make -s -j %1 %2 %3