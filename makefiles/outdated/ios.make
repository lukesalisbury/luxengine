PLATFORM = IOS
PLATFORM_LIBS = -lSDL -lSDL_image -lz -lSDL_mixer -Wl,-rpath -Wl,\$$ORIGIN/lib
PLATFORM_FLAGS =  -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT 
PLATFORM_DIRECTORY = platform/ios
INCLUDE_PAWN = TRUE


ASM = 
ASMTYPE = elf

CPP = arm-apple-darwin9-g++
CC = arm-apple-darwin9-gcc
RES = 
BIN  = luxengine.ios.bin
OPENGL = FALSE

NETWORK = FALSE
NET_OBJECT = $(OBJDIR)/enet/unix.o




