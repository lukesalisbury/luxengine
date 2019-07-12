PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -lmingw32 -lz -lwinmm -static-libgcc -static-libstdc++
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H 
PLATFORM_DIRECTORY = platform/nacl


OBJDIR = objects
INCLUDE_PAWN = TRUE

#ASM = nasmw
ASM =
ASMTYPE = win32

RES = objects/lux.res
RES_SOURCE = res/lux.rc

CPP = g++
CC = gcc
OPENGL = FALSE

#Network Settings
NETWORK = FALSE

BINEXT = .exe
BIN  = luxengine.exe

$(RES): $(RES_SOURCE) 
	windres -i $(RES_SOURCE) --input-format=rc -o $@ -O coff 

