ifeq ($(SUPPORTPATH), )

$(error Please set SUPPORTPATH variable before running make. )

endif

LDFLAGS+=-L"$(SUPPORTPATH)/lib"
CPPFLAGS+=-I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/sdl"

#Settings
INCLUDE_PAWN = TRUE
NETWORK = TRUE
DOWNLOADER_MODE = basic
OPENGL = TRUE

CPP = g++
CC = gcc

ASMTYPE = win32
ASM = 

BIN  = luxengine-sdl.exe
BINEXT = .exe
BUILDDIR = ./bin
OBJDIR = ./objects

RES = $(OBJDIR)/lux.res
RES_SOURCE = res/lux.rc

ifeq ($(PLATFORMBITS), 64)
	RES_OUTPUT = pe-x86-64
	OBJDIR += 64
else
	RES_OUTPUT = pe-i386
	ASM = nasmw
	PLATFORM_FLAGS += -march=i586
endif

#Static Settings
PLATFORM = __GNUWIN32__
PLATFORM_DIRECTORY = platform/pc

PLATFORM_LIBS = -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -lwinmm -static-libgcc -lopengl32 -lws2_32
PLATFORM_FLAGS = -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DUSE_SDL1
PLATFORM_OBJECTS = $(OBJDIR)/gles/gl_camera.o $(OBJDIR)/enet/win32.o $(OBJDIR)/lux.res

ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_LIBS += -mconsole
else
	PLATFORM_LIBS += -mwindows
endif


$(RES): $(RES_SOURCE)
	@windres -i $(RES_SOURCE) --input-format=rc -o $@ -O coff -F $(RES_OUTPUT)
