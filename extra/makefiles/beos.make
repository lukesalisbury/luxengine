#Settings
INCLUDE_PAWN = TRUE
NETWORK = TRUE
DOWNLOADER_MODE = none
OPENGL = FALSE

BIN  = luxengine
CPP = g++
CC = gcc


#Static Settings
PLATFORM = __HAIKU__
PLATFORM_DIRECTORY = platform/pc

PLATFORM_LIBS = -L"/Blank_BFS/common/lib" -lSDLmain -lSDL -lSDL_image -lSDL_gfx -lz -lSDL_mixer -lnetwork -lbe 
PLATFORM_FLAGS = -I"/Blank_BFS/common/include" -I"/Blank_BFS/common/include/SDL" -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT
PLATFORM_OBJECTS = $(OBJDIR)/enet/unix.o


$(FINALOUTPUT): $(BIN)
	xres  -o $(BIN) res/beos.rsrc
	mimeset -f $(BIN)
