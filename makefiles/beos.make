PLATFORM = __HAIKU__
PLATFORM_LIBS = -L"/Blank_BFS/common/lib" -lSDLmain -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -lnetwork -lbe 
PLATFORM_FLAGS = -I"/Blank_BFS/common/include" -I"/Blank_BFS/common/include/SDL" -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT
PLATFORM_DIRECTORY = platform/pc
INCLUDE_PAWN = TRUE

ASM = 
ASMTYPE = elf

CPP = g++
CC = gcc
BIN  = luxengine
OPENGL = FALSE
BUILDDIR = bin/beos


NETWORK = TRUE
NET_OBJECT = $(OBJDIR)/enet/unix.o

ifeq ($(PLATFORMBITS), 64)
	PLATFORM_FLAGS += -DPAWN_CELL_SIZE=64 -DHAVE_I64 -m64
endif

$(FINALOUTPUT): $(BIN)
	xres  -o $(BIN) res/beos.rsrc
	mimeset -f $(BIN)
