PLATFORM = apple
PLATFORMBITS = 32
PLATFORM_LIBS =  -arch i386 -framework Cocoa -F../resources/Frameworks/ -framework SDL_mixer -framework SDL  -framework SDL_image -framework SDL_gfx -lstdc++ -lz 
PLATFORM_FLAGS = -arch i386 -DHAVE_STDINT_H -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 -Dmain=SDL_main -DHAS_SOCKLEN_T -DENDIAN_SDL -D__APPLE__ 
PLATFORM_FLAGS += -I"../resources/Frameworks/Headers/"

PLATFORM_OBJECTS = res/sdlmain.o
PLATFORM_DIRECTORY = platform/pc


INCLUDE_PAWN = TRUE

BIN  = luxengine
OPENGL = TRUE
CPP = gcc-4.2

NETWORK = TRUE
DOWNLOADER_MODE = none
NET_OBJECT = $(OBJDIR)/enet/unix.o

res/%.o : res/SDLMain.m
	@echo Compiling $@ $(MESSAGE)
	@$(CPP) -c -o $@ $< $(PLATFORM_FLAGS)
