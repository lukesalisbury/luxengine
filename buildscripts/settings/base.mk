#Main code
OBJ += config.o engine.o entity.o entity_manager.o entity_section.o
OBJ += language.o map_object.o misc_functions.o mokoi_game.o player.o
OBJ += save_system.o game_system.o object_effect.o sprite_sheet.o
OBJ += lux_virtual_sprite.o
OBJ += lux_canvas.o lux_sprite.o map.o
OBJ += world_section.o map_screen.o masks.o lux_polygon.o map_xml_reader.o game_config.o
OBJ += options.o global_objects.o

OBJ += gui/css.o gui/dialog.o gui/gui_base.o gui/gui_settings.o gui/gui_theme.o gui/widget.o
OBJ += portal/portal.o portal/game_info.o test/test_code.o
OBJ += display/display.o display/display_functions.o display/layers.o display/graphics_system.o

#Platform code
OBJ += platform/\$${platform_subsystem}/audio.o platform/\$${platform_subsystem}/core.o platform/\$${platform_subsystem}/graphics_native.o platform/\$${platform_subsystem}/worker.o
OBJ += platform/\$${platform_subsystem}/platform_controls.o platform/\$${platform_subsystem}/platform_functions.o platform/\$${platform_subsystem}/platform_media.o
OBJ += platform/\$${platform_subsystem}/platform_config.o platform/\$${platform_subsystem}/display_bitfont.o

#FFI code
OBJ += entity_systems/ffi_system.o entity_systems/ffi_object.o entity_systems/ffi_collisions.o
OBJ += entity_systems/ffi_path.o entity_systems/ffi_entities.o entity_systems/ffi_player.o
OBJ += entity_systems/ffi_functions.o entity_systems/ffi_shaders.o entity_systems/ffi_layer.o
OBJ += entity_systems/ffi_spritesheet.o entity_systems/ffi_map.o entity_systems/ffi_world.o
OBJ += entity_systems/ffi_game.o entity_systems/ffi_input.o entity_systems/ffi_mask.o

#Tinyxml code
OBJ += libs/tinyxml/tinyxml2.o libs/tinyxml/tinyxml2ext.o

#elix code
OBJ += libs/elix/elix_directory.o libs/elix/elix_endian.o  libs/elix/elix_file.o libs/elix/elix_string.o libs/elix/elix_path.o libs/elix/elix_png.o libs/elix/elix_program.o

#Download code
OBJ += downloader-none.o

OBJ += platform/\$${platform_subsystem}/platform_main.o lux_screen.o platform/\$${platform_subsystem}/project_media.o


