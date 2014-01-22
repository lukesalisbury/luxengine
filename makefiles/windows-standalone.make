ifeq ($(SUPPORTPATH), )
	$(error Please set SUPPORTPATH variable before running make.   )
endif

LDFLAGS += -L"$(SUPPORTPATH)/lib" 
CPPFLAGS += -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/SDL2" 


OBJDIR = object-standalone

PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -lmingw32 -static -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2 
PLATFORM_LIBS += -Wl,-Bdynamic -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DNO_ZLIB -DUSE_SDL2 -DOPENGLONLY -DSTANDALONE -Dmain=SDL_main
PLATFORM_OBJECTS =

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

RES_SOURCE = res/lux-standalone.rc
RES_OUTPUT = pe-i386
ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
endif


CPP = g++
CC = gcc
OPENGL = TRUE
OPTIMIZE=FULL

#Network Settings
NETWORK = TRUE
DOWNLOADER_MODE = basic
NET_OBJECT = $(OBJDIR)/enet/win32.o
PLATFORM_LIBS += -lws2_32

BINEXT = .exe
BIN  = lux.exe


