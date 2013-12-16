@ECHO off
cls
@rem Make sure these paths are correct.
set MAKE=mingw32-make.exe 
set MINGWPATH=f:\MinGW64
set MSYSPATH=f:\mingw\msysbasic
set SUPPORTPATH=f:/sdl2libs
set PATH=%PATH%;%MINGWPATH%\bin\;%MSYSPATH%\bin;%SUPPORTPATH%\bin\
set BUILDOS=windows-standalone
set LDFLAGS=-L"%SUPPORTPATH%/lib" 
set CPPFLAGS=-I"%SUPPORTPATH%/include" -I"%SUPPORTPATH%/include/sdl" -I"%MINGWPATH%/include" -I"%MINGWPATH%/include/sdl"

set BUILDDEBUG=FALSE


cmd /k echo Lux Engine Build Environment. Just run 'make'



