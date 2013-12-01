set JAVA_HOME=F:\Java\jdk1.7.0_03
REM f:\Android\android-ndk-r8b\ndk-build NDK_DEBUG=1 %1
F:\Java\ant\bin\ant debug

pause

REM $ adb shell stop
REM $ adb shell setprop log.redirect-stdio true
REM $ adb shell start


REM f:\Android\android-sdk\tools\android update project --path . --target 3

