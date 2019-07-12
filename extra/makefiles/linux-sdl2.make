#Settings
INCLUDE_PAWN = TRUE
NETWORK = FALSE
DOWNLOADER_MODE = curl

BIN  = luxengine
CPP = g++
CC = gcc
ASM = 
ASMTYPE = elf

#Static Settings
PLATFORM = LINUX
PLATFORM_DIRECTORY = platform/sdl2

PLATFORM_LIBS = `sdl2-config --libs`  -lSDL2_mixer -lcurl -Wl,-rpath -Wl,\$$ORIGIN/lib
PLATFORM_FLAGS = `sdl2-config --cflags` -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT -DNO_ZLIB -DUSE_SDL2

ifeq ($(NETWORK), TRUE)
	PLATFORM_OBJECTS += $(OBJDIR)/enet/unix.o
endif


#Raspberry PI check
ifeq (${shell grep -q BCM270 /proc/cpuinfo && echo 1}, 1)
	OPENGL = FALSE
	PLATFORMBITS =  RaspberryPI
	PLATFORM_FLAGS += -DRASPBERRYPI=1 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -DDISPLAYMODE_NATIVE
	PLATFORM_LIBS += -L/opt/vc/lib -lEGL -lGLESv1_CM -lGLESv2
else
	OPENGL = TRUE
	PLATFORM_FLAGS += -DDISPLAYMODE_OPENGL
	PLATFORM_LIBS += -lGL
endif
ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_FLAGS += -ggdb
endif


installdeps:
	apt-get install libsdl2.0-dev libsdl-mixer2.0-dev
