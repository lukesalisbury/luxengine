#Settings
INCLUDE_PAWN = TRUE
NETWORK = TRUE
DOWNLOADER_MODE = none
OPENGL = FALSE


BIN  = luxengine
CPP = gcc-4.2
CC = gcc-4.2

#Static Settings
PLATFORM = apple
PLATFORM_DIRECTORY = platform/sdl2

PLATFORMBITS = 32

PLATFORM_LIBS = -arch i386 -framework Cocoa -F../frameworks/ -framework SDL2_mixer -framework SDL2  -lstdc++ -lz
PLATFORM_LIBS += -framework OpenGL

PLATFORM_FLAGS = -arch i386 -DHAVE_STDINT_H -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 -Dmain=SDL_main -DHAS_SOCKLEN_T -DUSE_SDL2  -D__APPLE__
PLATFORM_FLAGS += -I"../frameworks/SDL2_mixer.framework/Headers/" -I"../frameworks/SDL2.framework/Headers/" -DDISPLAYMODE_NATIVE

PLATFORM_OBJECTS = res/sdlmain.o $(OBJDIR)/enet/unix.o





res/%.o : res/SDLMain.m
	@echo Compiling $@ $(MESSAGE)
	@$(CPP) -c -o $@ $< $(PLATFORM_FLAGS)
