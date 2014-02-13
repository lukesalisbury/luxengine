PLATFORM = LINUX
PLATFORM_LIBS = -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -Wl,-rpath -Wl,\$$ORIGIN/lib
PLATFORM_FLAGS = `sdl-config --cflags` -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT
PLATFORM_DIRECTORY = platform/pc
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
DOWNLOADER_MODE = basic
NET_OBJECT = $(OBJDIR)/enet/unix.o


ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_FLAGS += -ggdb
endif


installdeps:
	apt-get install libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev libsdl-gfx1.2-dev 



