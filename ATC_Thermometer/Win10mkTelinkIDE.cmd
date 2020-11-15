@rem Install Telink IDE and Python3, clone https://github.com/Ai-Thinker-Open/Telink_825X_SDK
@set TLIDE=E:\Telink\SDK
@set TEL_PATH=E:/Telink/825x/Telink_825X_SDK
@set PATH=%TLIDE%\bin;%TLIDE%\opt\tc32\bin;%TLIDE%\usr\bin;%PATH%
make -j %1 %2 %3