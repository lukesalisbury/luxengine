PLATFORM = LINUX
PLATFORM_LIBS = -static -lSDL2  -lSDL_mixer -lcurl -Wl,-rpath -Wl,\$$ORIGIN/lib
PLATFORM_FLAGS = `sdl2-config --cflags` -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT -DNO_ZLIB -DUSE_SDL2 -DOPENGLONLY
PLATFORM_DIRECTORY = platform/sdl2
INCLUDE_PAWN = TRUE

ifeq (${shell grep -q BCM2708 /proc/cpuinfo && echo 1}, 1)
PLATFORMBITS =  Raspberry PI
PLATFORM_FLAGS += -DRASPBERRYPI=1 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads
PLATFORM_LIBS += -L/opt/vc/lib -lEGL -lGLESv1_CM
endif

ASM = 
ASMTYPE = elf

CPP = g++
CC = gcc
RES = 
BIN  = luxengine
OPENGL = TRUE

NETWORK = TRUE
DOWNLOADER_MODE = curl
NET_OBJECT = $(OBJDIR)/enet/unix.o


ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_FLAGS += -ggdb
endif


installdeps:
	apt-get install libsdl2.0-dev libsdl-mixer2.0-dev



