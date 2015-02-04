

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

ROOT_PATH := ../../../../src
ELIX_PATH := ../../../../../elix/src
PLATFORM_DIRECTORY := platform/sdl2

SDL_PATH := ../SDL2
SDL_MIXER_PATH := SDL2_mixer

TARGET_PLATFORM = android-10

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_MIXER_PATH) $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(ROOT_PATH) $(LOCAL_PATH)/$(ROOT_PATH)/platform/sdl2/ $(LOCAL_PATH)/$(ROOT_PATH)/../include  $(LOCAL_PATH)/$(ELIX_PATH)

LOCAL_CFLAGS := -DANDROID_NDK -DHAVE_STDINT_H -Wno-psabi -DAMXTIME_NOIDLE -DAMX_NOPROPLIST -DAMXCONSOLE_NOIDLE -DAMX_NODYNALOAD  -DFIXEDPOINT -DFLOATPOINT -DNO_ZLIB -DPAWN_CELL_SIZE=32 -DPAWN_VERSION=4 -DPROGRAM_VERSION_STABLE="\"1.0\"" 
LOCAL_CPPFLAGS := -fno-access-control -fno-exceptions -fno-rtti  -DTIXML_USE_STL $(LOCAL_CFLAGS)

#Android code
LOCAL_SRC_FILES = $(SDL_PATH)/src/main/android/SDL_android_main.c

#Main code
LOCAL_SRC_FILES += $(ROOT_PATH)/config.cpp $(ROOT_PATH)/css.cpp $(ROOT_PATH)/dialog.cpp $(ROOT_PATH)/engine.cpp $(ROOT_PATH)/entity.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_manager.cpp $(ROOT_PATH)/entity_section.cpp $(ROOT_PATH)/gui_settings.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/gui_base.cpp $(ROOT_PATH)/gui_theme.cpp $(ROOT_PATH)/language.cpp $(ROOT_PATH)/map_object.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/misc_functions.cpp $(ROOT_PATH)/mokoi_game.cpp $(ROOT_PATH)/player.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/save_system.cpp $(ROOT_PATH)/widget.cpp $(ROOT_PATH)/game_system.cpp $(ROOT_PATH)/object_effect.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/sprite_sheet.cpp $(ROOT_PATH)/graphics_system.cpp $(ROOT_PATH)/lux_virtual_sprite.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/display.cpp $(ROOT_PATH)/display_functions.cpp  $(ROOT_PATH)/layers.cpp $(ROOT_PATH)/lux_canvas.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/lux_sprite.cpp $(ROOT_PATH)/map.cpp $(ROOT_PATH)/world_section.cpp $(ROOT_PATH)/map_screen.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/masks.cpp $(ROOT_PATH)/lux_polygon.cpp $(ROOT_PATH)/map_xml_reader.cpp $(ROOT_PATH)/game_config.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/options.cpp $(ROOT_PATH)/global_objects.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/downloader-none.cpp 

#Portal/Test code
LOCAL_SRC_FILES += $(ROOT_PATH)/portal/portal.cpp $(ROOT_PATH)/portal/game_info.cpp $(ROOT_PATH)/test/test_code.cpp 

#Platform code
LOCAL_SRC_FILES += $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/audio.cpp $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/core.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/graphics_native.cpp $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/worker.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/platform_controls.cpp $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/platform_functions.cpp $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/platform_media.cpp

#Tinyxml code
LOCAL_SRC_FILES += $(ROOT_PATH)/tinyxml/tinyxml2.cpp $(ROOT_PATH)/tinyxml/tinyxml2ext.cpp

#elix code
LOCAL_SRC_FILES += $(ELIX_PATH)/elix_directory.cpp $(ELIX_PATH)/elix_endian.cpp  $(ELIX_PATH)/elix_file_sdl.cpp $(ELIX_PATH)/elix_string.cpp 
LOCAL_SRC_FILES += $(ELIX_PATH)/elix_path.cpp $(ELIX_PATH)/elix_png.cpp $(ELIX_PATH)/elix_program.cpp

#FFI code
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/ffi_system.cpp $(ROOT_PATH)/entity_systems/ffi_object.cpp $(ROOT_PATH)/entity_systems/ffi_collisions.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/ffi_path.cpp $(ROOT_PATH)/entity_systems/ffi_entities.cpp $(ROOT_PATH)/entity_systems/ffi_player.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/ffi_functions.cpp $(ROOT_PATH)/entity_systems/ffi_shaders.cpp $(ROOT_PATH)/entity_systems/ffi_layer.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/ffi_spritesheet.cpp $(ROOT_PATH)/entity_systems/ffi_map.cpp $(ROOT_PATH)/entity_systems/ffi_world.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/ffi_game.cpp $(ROOT_PATH)/entity_systems/ffi_input.cpp $(ROOT_PATH)/entity_systems/ffi_mask.cpp

#Pawn code
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/pawn_core.cpp $(ROOT_PATH)/entity_systems/pawn_float.cpp $(ROOT_PATH)/entity_systems/pawn_string.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/pawn_functions.cpp $(ROOT_PATH)/entity_systems/pawn_graphics.cpp $(ROOT_PATH)/entity_systems/pawn_player.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/pawn_entity.cpp $(ROOT_PATH)/entity_systems/pawn_maps.cpp $(ROOT_PATH)/entity_systems/pawn_game.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/pawn_network.cpp $(ROOT_PATH)/entity_systems/pawn_online.cpp $(ROOT_PATH)/entity_systems/pawn_console.cpp 
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/pawn_time.cpp $(ROOT_PATH)/entity_systems/pawn_system.cpp $(ROOT_PATH)/entity_systems/pawn_input.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/entity_systems/pawn_fixed.cpp  $(ROOT_PATH)/entity_systems/pawn.cpp

LOCAL_SRC_FILES += $(ROOT_PATH)/scripting/amx4/amx.c $(ROOT_PATH)/scripting/amx4/amxcons.c $(ROOT_PATH)/scripting/amx4/amxcore.c

#Customisable code
LOCAL_SRC_FILES += $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/platform_config.cpp $(ROOT_PATH)/$(PLATFORM_DIRECTORY)/platform_main.cpp $(ROOT_PATH)/lux_screen.cpp $(ROOT_PATH)/project_media.cpp 

#Box2D
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Collision/b2BroadPhase.cpp $(ROOT_PATH)/Box2D/Collision/b2CollideCircle.cpp $(ROOT_PATH)/Box2D/Collision/b2CollideEdge.cpp $(ROOT_PATH)/Box2D/Collision/b2CollidePolygon.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Collision/b2Collision.cpp $(ROOT_PATH)/Box2D/Collision/b2Distance.cpp $(ROOT_PATH)/Box2D/Collision/b2DynamicTree.cpp $(ROOT_PATH)/Box2D/Collision/b2TimeOfImpact.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Collision/Shapes/b2ChainShape.cpp $(ROOT_PATH)/Box2D/Collision/Shapes/b2CircleShape.cpp $(ROOT_PATH)/Box2D/Collision/Shapes/b2EdgeShape.cpp $(ROOT_PATH)/Box2D/Collision/Shapes/b2PolygonShape.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Common/b2BlockAllocator.cpp $(ROOT_PATH)/Box2D/Common/b2Draw.cpp $(ROOT_PATH)/Box2D/Common/b2Math.cpp $(ROOT_PATH)/Box2D/Common/b2Settings.cpp $(ROOT_PATH)/Box2D/Common/b2StackAllocator.cpp $(ROOT_PATH)/Box2D/Common/b2Timer.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/b2Body.cpp $(ROOT_PATH)/Box2D/Dynamics/b2ContactManager.cpp $(ROOT_PATH)/Box2D/Dynamics/b2Fixture.cpp $(ROOT_PATH)/Box2D/Dynamics/b2Island.cpp $(ROOT_PATH)/Box2D/Dynamics/b2World.cpp $(ROOT_PATH)/Box2D/Dynamics/b2WorldCallbacks.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2CircleContact.cpp $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2Contact.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2ContactSolver.cpp $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/Joints/b2DistanceJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2FrictionJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2GearJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2Joint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2MotorJoint.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/Joints/b2MouseJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2PulleyJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Dynamics/Joints/b2RopeJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2WeldJoint.cpp $(ROOT_PATH)/Box2D/Dynamics/Joints/b2WheelJoint.cpp
LOCAL_SRC_FILES += $(ROOT_PATH)/Box2D/Rope/b2Rope.cpp


LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)





