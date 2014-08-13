#Settings
INCLUDE_PAWN = TRUE
NETWORK = FALSE
DOWNLOADER_MODE = none
OPTIMIZE = NONE

BIN  = luxengine.html
CPP = emcc
CC = emcc
ASM = 
ASMTYPE =

BUILDDIR = ./bin/js
OBJDIR = ./objects-js

LDFLAGS += -L"$(SUPPORTPATH)/lib" -Wno-warn-absolute-paths
CPPFLAGS += -I"$(SUPPORTPATH)/include" -I"$(SUPPORTPATH)/include/SDL2" -Wno-warn-absolute-paths -Wno-unknown-warning-option

#Static Settings
PLATFORM = EMSCRIPTEN
PLATFORM_DIRECTORY = platform/sdl2
PLATFORM_LIBS = -LSDL2
PLATFORM_FLAGS = -DNO_ZLIB -DNOAUDIO -DAMX_ANSIONLY -DENDIAN_SDL -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT
