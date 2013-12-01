

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

ROOT_SOURCE := ../../../../src
ELIX_SOURCE := ../../../../../elix/src
SDL_PATH := ../SDL
SDL_MIXER_PATH := ../SDL_mixer

TARGET_PLATFORM = android-10

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_MIXER_PATH) $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(ROOT_SOURCE) $(LOCAL_PATH)/$(ROOT_SOURCE)/platform/sdl2/ $(LOCAL_PATH)/$(ROOT_SOURCE)/../include  $(LOCAL_PATH)/$(ELIX_SOURCE)

LOCAL_CFLAGS := -DANDROID_NDK -DHAVE_STDINT_H -Wno-psabi -DAMXTIME_NOIDLE -DAMX_NOPROPLIST -DAMXCONSOLE_NOIDLE -DAMX_NODYNALOAD  -DFIXEDPOINT -DFLOATPOINT -DNO_ZLIB -DPAWN_CELL_SIZE=32 -DPAWN_VERSION=4 -DPROGRAM_VERSION_STABLE="\"1.0\"" 
LOCAL_CPPFLAGS := -fno-access-control -fno-exceptions -fno-rtti  -DTIXML_USE_STL $(LOCAL_CFLAGS)


LOCAL_SRC_FILES = $(SDL_PATH)/src/main/android/SDL_android_main.cpp

LOCAL_SRC_FILES += $(ROOT_SOURCE)/config.cpp $(ROOT_SOURCE)/css.cpp $(ROOT_SOURCE)/dialog.cpp $(ROOT_SOURCE)/engine.cpp $(ROOT_SOURCE)/entity.cpp $(ROOT_SOURCE)/entity_manager.cpp $(ROOT_SOURCE)/entity_section.cpp $(ROOT_SOURCE)/gui_settings.cpp
LOCAL_SRC_FILES += $(ROOT_SOURCE)/gui_base.cpp $(ROOT_SOURCE)/gui_theme.cpp $(ROOT_SOURCE)/language.cpp $(ROOT_SOURCE)/map_object.cpp $(ROOT_SOURCE)/misc_functions.cpp $(ROOT_SOURCE)/mokoi_game.cpp $(ROOT_SOURCE)/player.cpp
LOCAL_SRC_FILES += $(ROOT_SOURCE)/save_system.cpp $(ROOT_SOURCE)/widget.cpp $(ROOT_SOURCE)/world.cpp $(ROOT_SOURCE)/object_effect.cpp $(ROOT_SOURCE)/sprite_sheet.cpp $(ROOT_SOURCE)/graphics_system.cpp
LOCAL_SRC_FILES += $(ROOT_SOURCE)/portal.cpp $(ROOT_SOURCE)/game_info.cpp $(ROOT_SOURCE)/display.cpp $(ROOT_SOURCE)/display_functions.cpp $(ROOT_SOURCE)/layers.cpp $(ROOT_SOURCE)/lux_canvas.cpp $(ROOT_SOURCE)/lux_sprite.cpp $(ROOT_SOURCE)/map.cpp
LOCAL_SRC_FILES += $(ROOT_SOURCE)/world_section.cpp  $(ROOT_SOURCE)/map_screen.cpp $(ROOT_SOURCE)/masks.cpp $(ROOT_SOURCE)/test_code.cpp $(ROOT_SOURCE)/map_masks.cpp $(ROOT_SOURCE)/lux_polygon.cpp $(ROOT_SOURCE)/map_xml_reader.cpp

LOCAL_SRC_FILES += $(ROOT_SOURCE)/platform/sdl2/audio.cpp $(ROOT_SOURCE)/platform/sdl2/core.cpp $(ROOT_SOURCE)/platform/sdl2/graphics_native.cpp $(ROOT_SOURCE)/platform/sdl2/worker.cpp
LOCAL_SRC_FILES += $(ROOT_SOURCE)/platform/sdl2/platform_controls.cpp $(ROOT_SOURCE)/platform/sdl2/platform_functions.cpp $(ROOT_SOURCE)/platform/sdl2/platform_media.cpp

LOCAL_SRC_FILES += $(ROOT_SOURCE)/tinyxml/tinyxml2.cpp $(ROOT_SOURCE)/tinyxml/tinyxml2ext.cpp
LOCAL_SRC_FILES += $(ELIX_SOURCE)/elix_endian.cpp  $(ELIX_SOURCE)/elix_file.cpp $(ELIX_SOURCE)/elix_string.cpp $(ELIX_SOURCE)/elix_path.cpp $(ELIX_SOURCE)/elix_png.cpp

LOCAL_SRC_FILES += $(ROOT_SOURCE)/platform/sdl2/platform_config.cpp $(ROOT_SOURCE)/platform/sdl2/platform_main.cpp $(ROOT_SOURCE)/lux_screen.cpp $(ROOT_SOURCE)/project_media.cpp $(ROOT_SOURCE)/downloader-fake.cpp

LOCAL_SRC_FILES += $(ROOT_SOURCE)/entity_systems/pawn_core.cpp $(ROOT_SOURCE)/entity_systems/pawn_float.cpp $(ROOT_SOURCE)/entity_systems/pawn_string.cpp $(ROOT_SOURCE)/entity_systems/pawn_functions.cpp $(ROOT_SOURCE)/entity_systems/pawn_graphics.cpp $(ROOT_SOURCE)/entity_systems/pawn_player.cpp $(ROOT_SOURCE)/entity_systems/pawn_entity.cpp $(ROOT_SOURCE)/entity_systems/pawn_maps.cpp $(ROOT_SOURCE)/entity_systems/pawn_misc.cpp $(ROOT_SOURCE)/entity_systems/pawn_network.cpp  $(ROOT_SOURCE)/entity_systems/pawn_online.cpp $(ROOT_SOURCE)/entity_systems/pawn_console.cpp $(ROOT_SOURCE)/entity_systems/pawn_fixed.cpp $(ROOT_SOURCE)/entity_systems/pawn.cpp
LOCAL_SRC_FILES += $(ROOT_SOURCE)/amx4/amx.c $(ROOT_SOURCE)/amx4/amxcons.c $(ROOT_SOURCE)/amx4/amxcore.c


LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)





