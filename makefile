# Lux Engine Makefile
# Build Options
# BUILDOS - windows, linux, apple
# OPTIMIZE - FULL, SOME, NONE
# BUILDDEBUG - TRUE, FALSE
# PLATFORMBITS 64, 32
# CC (Compiler)
# BIN (Binary output)
# CUSTOMSETTINGS - Custom build
# CUSTOMOBJECTS - Uses custom_config.cpp, custom_main.cpp and custom_screen.cpp instead of platform_config.cpp, platform_main.cpp and lux_screen.cpp

#Default Settings
include setting.mk

BIN = luxengine
DOWNLOADER_MODE = none

#Read platform and custom settings
include makefiles/$(BUILDOS).make
ifneq ($(CUSTOMSETTINGS), )
	include custom/$(CUSTOMSETTINGS).make
endif

COMPILER_LIBS = $(OPTIMIZER) $(DEBUG) -L'lib' $(PLATFORM_LIBS)
COMPILER_FLAGS = $(OPTIMIZER) $(DEBUG) -I'../elix/src/' -I'src' -I'src/$(PLATFORM_DIRECTORY)' -I'include'  -DTIXML_USE_STL -D$(PLATFORM) $(PLATFORM_FLAGS)
COMPILER_FLAGSPP = -Wno-write-strings -fno-access-control -fno-exceptions -fno-rtti $(COMPILER_FLAGS)

ifeq ($(PLATFORMBITS), 64)
	COMPILER_FLAGS +=  -m64
	COMPILER_LIBS +=  -m64
endif

ifeq ($(PLATFORMBITS), 32)
	COMPILER_FLAGS +=  -m32
	COMPILER_LIBS +=  -m32
endif


OBJ += $(OBJDIR)/config.o $(OBJDIR)/css.o $(OBJDIR)/dialog.o $(OBJDIR)/engine.o $(OBJDIR)/entity.o $(OBJDIR)/entity_manager.o $(OBJDIR)/entity_section.o $(OBJDIR)/gui_settings.o
OBJ += $(OBJDIR)/gui_base.o $(OBJDIR)/gui_theme.o $(OBJDIR)/language.o $(OBJDIR)/map_object.o $(OBJDIR)/misc_functions.o $(OBJDIR)/mokoi_game.o $(OBJDIR)/player.o
OBJ += $(OBJDIR)/save_system.o $(OBJDIR)/widget.o $(OBJDIR)/world.o $(OBJDIR)/object_effect.o $(OBJDIR)/sprite_sheet.o $(OBJDIR)/graphics_system.o $(OBJDIR)/lux_virtual_sprite.o
OBJ += $(OBJDIR)/portal.o $(OBJDIR)/game_info.o $(OBJDIR)/display.o $(OBJDIR)/display_functions.o $(OBJDIR)/layers.o $(OBJDIR)/lux_canvas.o $(OBJDIR)/lux_sprite.o $(OBJDIR)/map.o
OBJ += $(OBJDIR)/world_section.o  $(OBJDIR)/map_screen.o $(OBJDIR)/masks.o $(OBJDIR)/test_code.o $(OBJDIR)/lux_polygon.o $(OBJDIR)/map_xml_reader.o $(OBJDIR)/game_config.o

OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/audio.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/core.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/graphics_native.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/worker.o
OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_controls.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_functions.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_media.o

OBJ += $(OBJDIR)/entity_systems/ffi.o $(OBJDIR)/entity_systems/ffi_object.o $(OBJDIR)/entity_systems/ffi_collisions.o
OBJ += $(OBJDIR)/entity_systems/ffi_path.o $(OBJDIR)/entity_systems/ffi_entities.o $(OBJDIR)/entity_systems/ffi_player.o
OBJ += $(OBJDIR)/entity_systems/ffi_functions.o $(OBJDIR)/entity_systems/ffi_shaders.o $(OBJDIR)/entity_systems/ffi_layer.o
OBJ += $(OBJDIR)/entity_systems/ffi_spritesheet.o


OBJ += $(OBJDIR)/tinyxml/tinyxml2.o $(OBJDIR)/tinyxml/tinyxml2ext.o
#OBJ += $(OBJDIR)/elix/elix_system.o
OBJ += $(OBJDIR)/elix/elix_directory.o $(OBJDIR)/elix/elix_endian.o  $(OBJDIR)/elix/elix_file.o $(OBJDIR)/elix/elix_string.o $(OBJDIR)/elix/elix_path.o $(OBJDIR)/elix/elix_png.o $(OBJDIR)/elix/elix_program.o
#$(OBJDIR)/elix/elix_httpdl.o

#Download code
OBJ += $(OBJDIR)/downloader-$(DOWNLOADER_MODE).o
ifeq ($(DOWNLOADER_MODE), curl)
#	COMPILER_LIBS += -lcurl
endif

ifeq ($(CUSTOMOBJECTS), TRUE)
	OBJ += $(OBJDIR)/custom/custom_config.o $(OBJDIR)/custom/custom_main.o $(OBJDIR)/custom/custom_screen.o $(OBJDIR)/custom/project_media.o
else
	OBJ += $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_config.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_main.o $(OBJDIR)/lux_screen.o $(OBJDIR)/project_media.o
endif

ifeq ($(INCLUDE_PAWN), TRUE)
	OBJ += $(OBJDIR)/entity_systems/pawn_core.o $(OBJDIR)/entity_systems/pawn_float.o $(OBJDIR)/entity_systems/pawn_string.o $(OBJDIR)/entity_systems/pawn_functions.o $(OBJDIR)/entity_systems/pawn_graphics.o $(OBJDIR)/entity_systems/pawn_player.o $(OBJDIR)/entity_systems/pawn_entity.o $(OBJDIR)/entity_systems/pawn_maps.o $(OBJDIR)/entity_systems/pawn_misc.o $(OBJDIR)/entity_systems/pawn_network.o  $(OBJDIR)/entity_systems/pawn_online.o $(OBJDIR)/entity_systems/pawn_console.o $(OBJDIR)/entity_systems/pawn_fixed.o $(OBJDIR)/entity_systems/pawn.o
	ifeq ($(PAWN), 4)
                OBJ += $(OBJDIR)/scripting/amx4/amx.o $(OBJDIR)/scripting/amx4/amxcons.o $(OBJDIR)/scripting/amx4/amxcore.o
		COMPILER_FLAGS += -DPAWN_VERSION=4
		ifdef ASM
                        OBJ += $(OBJDIR)/scripting/amx4/amxexecn.o
			COMPILER_FLAGS += -DAMX_ASM
		else
			ifdef PAWNSIMPLEEXEC

			else
                                OBJ += $(OBJDIR)/scripting/amx4/amxexec_gcc.o
				COMPILER_FLAGS += -DAMX_ALTCORE
			endif
		endif
	else
                OBJ += $(OBJDIR)/scripting/amx/amx.o $(OBJDIR)/scripting/amx/amxcons.o $(OBJDIR)/scripting/amx/amxcore.o
		COMPILER_FLAGS += -DPAWN_VERSION=3
		ifdef ASM
			OBJ += $(OBJDIR)/amx/amxexecn.o
			COMPILER_FLAGS += -DASM32
		endif
	endif
	COMPILER_FLAGS += -DAMXTIME_NOIDLE -DAMX_NOPROPLIST -DAMXCONSOLE_NOIDLE -DAMX_NODYNALOAD  -DFIXEDPOINT -DFLOATPOINT

	ifeq ($(PLATFORMBITS), 64)
		COMPILER_FLAGS += -DPAWN_CELL_SIZE=64 -DHAVE_I64
	endif


endif


# OpenGL
ifeq ($(OPENGL), TRUE)
	COMPILER_FLAGSPP += -DOPENGLENABLED

	OBJ += $(OBJDIR)/gles/gles_display.o $(OBJDIR)/gles/gl_camera.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/graphics_opengl.o $(OBJDIR)/gles/shaders.inc.o
	ifeq ($(PLATFORM), __GNUWIN32__)
		COMPILER_LIBS += -lopengl32
	else
		ifeq ($(PLATFORM), apple)
			COMPILER_LIBS += -framework OpenGL
		else
			COMPILER_LIBS += -lGL
		endif
	endif
endif

# Network
ifeq ($(NETWORK), TRUE)
	COMPILER_FLAGS += -DNETWORKENABLED
	OBJ += $(OBJDIR)/enet/callbacks.o $(OBJDIR)/enet/host.o  $(OBJDIR)/enet/packet.o $(OBJDIR)/enet/peer.o $(OBJDIR)/enet/protocol.o $(OBJDIR)/enet/list.o $(OBJDIR)/$(PLATFORM_DIRECTORY)/core_network.o $(OBJDIR)/map_network.o $(NET_OBJECT)
endif

OBJ += $(PLATFORM_OBJECTS)
COMPILER_FLAGS += $(CFLAGS)
COMPILER_FLAGSPP += $(CPPFLAGS) -DDOWNLOADER_MODE=$(DOWNLOADER_MODE)
COMPILER_LIBS += $(LDFLAGS)

RM = rm -f

.PHONY: all-before all clean
	@echo --------------------------------

all: all-before  $(BIN) $(FINALOUTPUT)
	@echo --------------------------------

all-before:
	@echo --------------------------------
	@echo Building $(BIN) $(PROGRAM_VERSION)
	@echo Build Platform: $(BUILDPLATFORM)
	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)
	@echo Debug Build? $(BUILDDEBUG)
	@echo Build Flags: $(COMPILER_FLAGS)
	@echo Build Libs: $(COMPILER_LIBS)
	@echo --------------------------------
clean:
	@echo Cleaning Lux Engine
	@${RM} $(OBJ)

clean-bin:
	@echo Cleaning Lux Engine
	@${RM} $(BUILDDIR)/$(BIN)

$(OBJDIR)/%.o : src/%.cpp
	@echo Compiling $@ $(MESSAGE)
	@-mkdir -p $(dir $@)
	@$(CPP) -c $(COMPILER_FLAGSPP) -o $@ $<

$(OBJDIR)/elix/%.o : ../elix/src/%.cpp
#	@echo Compiling $@ $(MESSAGE)
	@-mkdir -p $(dir $@)
	@$(CPP) -c $(COMPILER_FLAGSPP) -o $@ $<

$(OBJDIR)/%.o : src/%.c
#	@echo Compiling $@ $(MESSAGE)
	@-mkdir -p $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -o $@ $<

$(OBJDIR)/%.o : src/%.asm
#	@echo Compiling $@ $(MESSAGE) [ASM]
	@$(ASM) -O1 -f $(ASMTYPE) -o $@ $< -I$(dir $<)

$(BIN): $(OBJ) $(EXTRABINARIES)
	@echo --------------------------------
	@echo Building $(BIN) $(MESSAGE)
	@-mkdir -p $(dir $@)
	@$(CPP) $(OBJ) -o $(BUILDDIR)/$(BIN) $(COMPILER_LIBS)

install: $(BIN)
	@echo Installing $< to $(INSTALLDIR)
	@cp $(BUILDDIR)/$(BIN) $(INSTALLDIR)

input_header:
	$(CPP) -o $(OBJDIR)/update_input_header.o -c update_input_header.cpp
	$(CPP) -o update_input_header.exe $(OBJDIR)/update_input_header.o -s
	-rm $(OBJDIR)/$(PLATFORM_DIRECTORY)/platform_media.o
	./update_input_header.exe keyboard
	./update_input_header.exe joystick_generic
	./update_input_header.exe joystick_xbox360
	./update_input_header.exe joystick_dreamcast dc
	./update_input_header.exe mouse_generic


