#Settings
INCLUDE_PAWN = TRUE
NETWORK = FALSE
DOWNLOADER_MODE = none
OPTIMIZE = NONE

BIN  = luxengine.html
CPP = em++
CC = emcc
ASM = 
ASMTYPE =

BUILDDIR = ./bin
OBJDIR = ./objects-js

LDFLAGS += -L"$(EMSCRIPTEN)/system/lib" -Wno-warn-absolute-paths
CPPFLAGS += -I"$(EMSCRIPTEN)/system/include" -I"$(SUPPORTPATH)/system/include/SDL2" -Wno-warn-absolute-paths -Wno-unknown-warning-option

#Static Settings
EMSCRIPTEN_OPTIONS = -s USE_SDL=2  -O2 -s EXPORTED_FUNCTIONS="['_loadGame', '_startGame', '_pauseGame', '_endGame']"
PLATFORM = EMSCRIPTEN
PLATFORM_DIRECTORY = platform/sdl2
PLATFORM_LIBS = $(EMSCRIPTEN_OPTIONS) --emrun --preload-file assests@/
PLATFORM_FLAGS = $(EMSCRIPTEN_OPTIONS) -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT -DNO_ZLIB -DUSE_SDL2 -DENDIAN_SDL -DAMX_ANSIONLY -DNOAUDIO -DPAWNSIMPLEEXEC

