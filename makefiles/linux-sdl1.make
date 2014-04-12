#Settings
INCLUDE_PAWN = TRUE
NETWORK = TRUE
DOWNLOADER_MODE = basic
OPENGL = TRUE

BIN  = luxengine
CPP = g++
CC = gcc
ASM = 
ASMTYPE = elf


#Static Settings
PLATFORM = LINUX
PLATFORM_DIRECTORY = platform/pc

PLATFORM_LIBS = -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -Wl,-rpath -Wl,\$$ORIGIN/lib
PLATFORM_FLAGS = `sdl-config --cflags` -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT
PLATFORM_OBJECTS = $(OBJDIR)/enet/unix.o

#Raspberry PI check
ifeq (${shell grep -q BCM2708 /proc/cpuinfo && echo 1}, 1)
	PLATFORMBITS =  Raspberry PI
	PLATFORM_FLAGS += -DRASPBERRYPI=1 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads
	PLATFORM_LIBS += -L/opt/vc/lib -lEGL -lGLESv1_CM
else
	PLATFORM_LIBS += -lGL
endif


ifeq ($(BUILDDEBUG), TRUE)
	PLATFORM_FLAGS += -ggdb
endif


installdeps:
	apt-get install libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev libsdl-gfx1.2-dev 



