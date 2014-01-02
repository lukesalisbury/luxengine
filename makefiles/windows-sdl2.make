ifeq ($(SUPPORTPATH), )

$(error Please set SUPPORTPATH variable before running make. )

endif

SUPPORTPATH = c:/dev/sdl2libs
CURLPATH = c:/dev/libcurl

LDFLAGS += -L"$(SUPPORTPATH)/lib" -L"$(CURLPATH)/lib"
CPPFLAGS += -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/SDL2" -I"$(CURLPATH)/include"


OBJDIR = object-sdl2

PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lwinmm -static-libgcc
PLATFORM_LIBS += -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DNO_ZLIB -DUSE_SDL2 -DOPENGLONLY -Dmain=SDL_main
PLATFORM_OBJECTS = $(OBJDIR)/lux.res

ifeq ($(PLATFORM_DIRECTORY), )
	PLATFORM_DIRECTORY = platform/sdl2
endif

INCLUDE_PAWN = TRUE

ASM =
ASMTYPE = win32
ifeq ($(PLATFORMBITS), 32)
#	ASM = nasmw
	PLATFORM_FLAGS += -march=i586
endif

ifeq ($(PLATFORMBITS), 64)
	OBJDIR += 64
endif


ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_LIBS += -mconsole
else
	PLATFORM_LIBS += -mwindows
endif

RES_SOURCE = res/lux.rc
RES_OUTPUT = pe-i386
ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
endif


CPP = g++
CC = gcc
OPENGL = TRUE

#Network Settings
NETWORK = TRUE
DOWNLOADER_MODE = curl
NET_OBJECT = $(OBJDIR)/enet/win32.o
PLATFORM_LIBS += -lws2_32 -lcurldll -lssl.dll -lcrypto.dll

BINEXT = .exe
BIN  = luxengine.exe

$(OBJDIR)/lux.res: $(RES_SOURCE)
	windres -i $(RES_SOURCE) --input-format=rc -o $@ -O coff -F $(RES_OUTPUT)

