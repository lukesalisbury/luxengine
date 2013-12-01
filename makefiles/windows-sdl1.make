ifeq ($(SUPPORTPATH), )
	SUPPORTPATH = F:/supportlibs
endif

LDFLAGS+=-L"$(SUPPORTPATH)/lib"
CPPFLAGS+=-I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/sdl"

PLATFORM = __GNUWIN32__
PLATFORM_LIBS = -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -lwinmm -static-libgcc
#-static-libstdc++
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DUSE_SDL1
PLATFORM_OBJECTS = $(OBJDIR)/lux.res


ifeq ($(PLATFORM_DIRECTORY), )
	PLATFORM_DIRECTORY = platform/pc
endif

INCLUDE_PAWN = TRUE

ASM =
ASMTYPE = win32
ifeq ($(PLATFORMBITS), 32)
	ASM = nasmw
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


RES = $(OBJDIR)/lux.res
RES_SOURCE = res/lux.rc
RES_OUTPUT = pe-i386
ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
endif


CPP = g++
CC = gcc
ifeq ($(OPENGL), )
	OPENGL = TRUE
endif

#Network Settings
NETWORK = TRUE
ONLINEENABLED = TRUE
NET_OBJECT = $(OBJDIR)/enet/win32.o
PLATFORM_LIBS += -lws2_32

BINEXT = .exe
BIN  = luxengine-sdl.exe

$(RES): $(RES_SOURCE)
	windres -i $(RES_SOURCE) --input-format=rc -o $@ -O coff -F $(RES_OUTPUT)

