@echo off
set MAKE=make.exe

set DEVKITARM=C:\dev\devkitpro\devkitARM
set DEVKITPRO=C:\dev\devkitpro

set PATH=%PATH%;%DEVKITARM%\bin\

set BUILDOS=3ds
set BUILDDEBUG=FALSE
set IP=192.168.16.99
set EMU=G:\emus\citra\citra.exe 
@cd ..
@echo --------------------------
IF EXIST %DEVKITARM%\bin\arm-none-eabi-gcc.exe ( @echo Found %DEVKITARM%\bin\arm-none-eabi-gcc.exe ) ELSE (GOTO error)
IF EXIST %DEVKITARM% (GOTO menu ) ELSE (GOTO error)

:menu
echo --------------------------
echo R. Run
echo S. Send to 3DS
echo B. Build
echo C. Clean
echo X. Exit
set choice=
set /p choice=Select:
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='s' goto send
if '%choice%'=='r' goto run
if '%choice%'=='b' goto build
if '%choice%'=='c' goto clean
if '%choice%'=='q' goto buildrun
if '%choice%'=='x' goto end
cls
echo "%choice%" is not valid please try again
goto menu

:send
cls
echo %IP%
3dslink.exe -a %IP% bin\3ds\luxengine.3dsx
goto menu

:run
cls
%EMU% bin\3ds\luxengine.3dsx
goto menu

:buildrun
cls
%MAKE% all
%EMU% bin\3ds\luxengine.3dsx
goto menu

:build
%MAKE% all
goto menu

:clean
%MAKE% clean
goto menu


:error
@echo Path '%MINGWPATH%' not found. Edit this file and enter the corrent path to mingw
pause
GOTO end

:end
