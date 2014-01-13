set JAVA_HOME=C:\dev\Java\jdk1.7.0_40
REM C:\dev\Android\android-ndk-r9\ndk-build NDK_DEBUG=1 %1
C:\dev\Java\ant\bin\ant debug

pause

REM $ adb shell stop
REM $ adb shell setprop log.redirect-stdio true
REM $ adb shell start


REM f:\Android\android-sdk\tools\android update project --path . --target 3

