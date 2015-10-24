@echo off
set MAKE=make.exe

set DEVKITARM=C:\dev\devkitpro\devkitARM
set DEVKITPRO=C:\dev\devkitpro

set PATH=%PATH%;%DEVKITARM%\bin\

set BUILDOS=3ds
set BUILDDEBUG=FALSE
set IP=192.168.16.107

@cd ..
@echo --------------------------
IF EXIST %DEVKITARM%\bin\arm-none-eabi-gcc.exe ( @echo Found %DEVKITARM%\bin\arm-none-eabi-gcc.exe ) ELSE (GOTO error)
IF EXIST %DEVKITARM% (GOTO menu ) ELSE (GOTO error)

:menu
echo --------------------------
echo R. Run
echo B. Build
echo C. Clean
echo X. Exit
set choice=
set /p choice=Select:
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='r' goto run
if '%choice%'=='b' goto build
if '%choice%'=='c' goto clean
if '%choice%'=='x' goto end
cls
echo "%choice%" is not valid please try again
goto menu

:run
3dslink.exe -a %IP% bin\3ds\luxengine.3dsx
pause
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
