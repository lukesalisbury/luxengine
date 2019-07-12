NAME=luxengine
TYPE=APP

SRCS=src/main.cpp src/functions.cpp src/gamepack.cpp src/engine.cpp src/config.cpp src/entity.cpp src/entity_manager.cpp src/player.cpp src/world.cpp src/2d/display.cpp src/2d/masks.cpp src/2d/layers.cpp src/2d/map.cpp src/entity_systems/pawn_functions.cpp src/entity_systems/pawn_graphics.cpp src/entity_systems/pawn_entity.cpp src/entity_systems/pawn.cpp src/widget.cpp src/gui_base.cpp src/pc/audio.cpp src/pc/core.cpp src/pc/gui.cpp src/pc/platform_functions.cpp src/pc/graphics_native.cpp src/pc/display_functions.cpp src/language.cpp

RSRCS= res/beos.rsrc

LIBS= be stdc++.r4 SDL SDL_image SDL_mixer SDL_gfx png z amx tinyxml
LIBPATHS= /boot/home/config/lib  lib

SYSTEM_INCLUDE_PATHS = /boot/home/config/include ./include/
LOCAL_INCLUDE_PATHS = 

OPTIMIZE= NONE
DEFINES= HAVE_INTTYPES_H

WARNINGS = NONE
SYMBOLS = FALSE
DEBUGGER = FALSE


COMPILER_FLAGS = `sdl-config --cflags`
LINKER_FLAGS = 

APP_VERSION = -app 0 1 4 d 014 -short 0104 -long "0104 "`echo -n -e '\302\251'`""


OBJ_DIR := objects
TARGET_DIR	:= build
INSTALL_DIR	:= ../MokoiGaming

## include the makefile-engine
include $(BUILDHOME)/etc/makefile-engine
