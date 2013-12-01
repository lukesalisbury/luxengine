PLATFORM = LINUX
PLATFORM_LIBS = -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -Wl,-rpath -Wl,\$$ORIGIN/lib
PLATFORM_FLAGS = -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT -DOPENGLES
PLATFORM_DIRECTORY = platform/pc
INCLUDE_PAWN = TRUE

ASM = 
ASMTYPE = elf

CPP = arm-none-linux-gnueabi-g++
CC = arm-none-linux-gnueabi-gcc
RES = 
BIN  = luxengine
OPENGL = FALSE
BUILDDIR = build/pandora


NETWORK = FALSE
NET_OBJECT = $(OBJDIR)/enet/unix.o

