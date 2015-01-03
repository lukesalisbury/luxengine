ifeq ($(SUPPORTPATH), )
$(error Please set SUPPORTPATH variable before running make. )
endif

LDFLAGS += -L"$(SUPPORTPATH)/lib"
CPPFLAGS += -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/SDL2"
PLATFORMBITS=32

#Settings
INCLUDE_PAWN = TRUE
NETWORK = FALSE
DOWNLOADER_MODE = curl
OPENGL = TRUE

CPP = g++
CC = gcc

ASMTYPE = win32
ASM = 

BIN  = luxengine.exe
BINEXT = .exe
OBJDIR = ./objects-sdl2

RES = $(OBJDIR)/lux.res
RES_SOURCE = res/lux.rc

ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
	OBJDIR += 64
else
	RES_OUTPUT = pe-i386
	PLATFORM_FLAGS += -march=i586
endif

#Static Settings

PLATFORM = __GNUWIN32__
PLATFORM_DIRECTORY = platform/sdl2

PLATFORM_LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lwinmm -static-libgcc -lws2_32 -lcurldll -lssl.dll -lcrypto.dll
PLATFORM_LIBS += -lopengl32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DNO_ZLIB -DUSE_SDL2 -DDISPLAYMODE_OPENGL -Dmain=SDL_main
PLATFORM_OBJECTS =  $(OBJDIR)/lux.res
ifeq ($(NETWORK), TRUE)
	PLATFORM_OBJECTS += $(OBJDIR)/enet/win32.o
endif


ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_LIBS += -mconsole
else
	PLATFORM_LIBS += -mwindows
endif


$(OBJDIR)/lux.res: $(RES_SOURCE)
	@windres -i $(RES_SOURCE) --input-format=rc -o $@ -O coff -F $(RES_OUTPUT)
