/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_map.h"

#include "game_system.h"
#include "elix/elix_endian.hpp"
#include "display/display.h"

/**
 * @brief Lux_FFI_Map_Set
 * @param map_ident
 * @param offset_x
 * @param offset_y
 * @return
 */
uint8_t Lux_FFI_Map_Set( const uint32_t map_ident, const int32_t offset_x, const int32_t offset_y )
{
	if ( lux::game_system )
	{
		MokoiMap * map = lux::game_system->GetMap( map_ident );

		if ( map )
		{
			if ( lux::game_system->SetMap(map, (fixed)offset_x, (fixed)offset_y, (fixed)-1 ) > 0 )
			{
				return true;
			}
		}
	}

	return false;
}

/**
 * @brief Lux_FFI_Map_Find_Ident
 * @param map_file
 * @param create_new
 * @return
 */
uint32_t Lux_FFI_Map_Find_Ident( const char * map_file )
{
	uint32_t map_ident = 0;
	if ( lux::game_system )
	{
		MokoiMap * map = NULL;

		map = lux::game_system->FindMap( map_file );

		if ( map )
		{
			map_ident = map->Ident();
		}

	}
	return map_ident;

}
/**
 * @brief Lux_FFI_Map_Create
 * @param map_file
 * @return
 */
uint32_t Lux_FFI_Map_Create( const char * map_file )
{
	uint32_t map_ident = 0;
	if ( lux::game_system )
	{
		MokoiMap * map = NULL;
		map = lux::game_system->CreateMap( map_file, true, false, 0, 0 );

		if ( map )
		{
			map_ident = map->Ident();
		}

	}
	return map_ident;

}

/**
 * @brief Lux_FFI_Map_Reset
 * @return
 */
uint8_t Lux_FFI_Map_Reset()
{
	if ( lux::game_system )
	{
		MokoiMap * map = lux::game_system->GetActiveMap();
		if ( map )
		{
			if ( map->HasSnapshot() )
			{
				return map->LoadSnapshot();
			}
			else
			{
				return map->Reset();
			}
		}
	}
	return 0;
}

/**
 * @brief Lux_FFI_Map_Snapshot
 * @return
 */
uint8_t Lux_FFI_Map_Snapshot()
{
	if ( lux::game_system )
	{
		MokoiMap * map = lux::game_system->GetActiveMap();
		if ( map )
		{
			return map->SaveSnapshot();
		}
	}
	return 0;
}

/**
 * @brief Lux_FFI_Map_Delete
 * @param map_ident
 * @return
 */
uint8_t Lux_FFI_Map_Delete( const uint32_t map_ident )
{
	if ( lux::game_system )
	{
		return lux::game_system->DeleteMap(map_ident);
	}
	return false;
}

/**
 * @brief Lux_FFI_Map_Offset_Set
 * @param x
 * @param y
 */
void Lux_FFI_Map_Offset_Set( int32_t x, int32_t y )
{
	if ( lux::game_system )
	{
		lux::game_system->SetPosition( x, y, -1);
	}
}

/**
 * @brief Lux_FFI_Map_Offset_Get
 * @param axis
 * @return
 */
int32_t Lux_FFI_Map_Offset_Get( uint8_t axis )
{
	if ( lux::game_system )
	{
		return lux::game_system->GetPosition(axis);
	}
	return 0xFFFFFFFF;
}

/**
 * @brief Lux_FFI_Map_Edit_New
 * @param map_file
 * @param section_hash
 * @param width
 * @param height
 * @return
 */
uint32_t Lux_FFI_Map_Edit_New( const char * map_file, const uint32_t section_hash, uint32_t width, uint32_t height)
{
	uint32_t map_ident = 0;
	if ( lux::game_system )
	{
		MokoiMap * map = NULL;
		map = lux::game_system->CreateMap( std::string(map_file), false, true, width, height );

		if ( map )
		{
			map_ident = map->Ident();
		}
	}
	return map_ident;

}
/**
 * @brief Lux_FFI_Map_Edit_Save
 * @param map_ident
 * @return
 */
uint8_t Lux_FFI_Map_Edit_Save( const uint32_t map_ident )
{
	if ( lux::game_system )
	{
		MokoiMap * map = lux::game_system->GetMap( map_ident );

		if ( map )
		{
			return map->SaveFile();
		}
	}

	return false;
}

/**
 * @brief Lux_FFI_Map_Edit_Object_Create
 * @param map_ident
 * @param type
 * @param x
 * @param y
 * @param z
 * @param w
 * @param h
 * @param colour
 * @param sprite
 * @return
 */
uint32_t Lux_FFI_Map_Edit_Object_Create( const uint32_t map_ident, const uint8_t type, const int32_t x, const int32_t y,
								const fixed z, const uint16_t w, const uint16_t h, const uint32_t colour, const char * sprite )
{
	MapObject * map_object = NULL;
	MokoiMap * map = NULL;


	if ( lux::game_system )
	{
		map = lux::game_system->GetMap( map_ident );
		if ( map )
		{
			map_object = new MapObject( type );

			map_object->position.x = x;
			map_object->position.y = y;
			map_object->SetZPos( z );
			map_object->position.w = w;
			map_object->position.h = h;

			cell_colour get_colour;
			get_colour.hex = elix::endian::host32(colour);
			map_object->effects.primary_colour = get_colour.rgba;

			if ( type == 's' && sprite != NULL )
				map_object->sprite.assign( sprite );


			return map->AddObject( map_object, true );
		}
	}
	return 0;

}

int32_t Lux_FFI_Map_Edit_Object_Postion( const uint32_t map_ident, uint32_t object_id, const int32_t x, const int32_t y, const fixed z,
								const uint16_t w, const uint16_t h )
{
	MapObject * map_object = NULL;
	MokoiMap * map = NULL;


	if ( lux::game_system )
	{
		map = lux::game_system->GetMap( map_ident );
		if ( map )
		{
			map_object = map->GetObject( object_id );
			map_object->position.x = x;
			map_object->position.y = y;
			map_object->position.w = w;
			map_object->position.h = h;

			if ( z >= 0 )
			{
				uint8_t current_layer = map_object->layer;

				map_object->SetZPos( z );

				if ( current_layer != map_object->layer )
				{
					lux::display->RemoveObject(current_layer, map_object);
					lux::display->AddObjectToLayer(map_object->layer, map_object, true);
				}
				else
				{
					lux::display->ResortLayer( map_object->layer );
				}
				return 2;
			}
		}
	}

	return 0;
}


/**
 * @brief Lux_FFI_Map_Edit_Object_Effect
 * @param map_ident
 * @param object_id
 * @param primary_colour
 * @param secondary_colour
 * @param rotation
 * @param scale_xaxis
 * @param scale_yaxis
 * @param flip_image
 * @param style
 * @return
 */
int32_t Lux_FFI_Map_Edit_Object_Effect( const uint32_t map_ident, uint32_t object_id, uint32_t primary_colour, uint32_t secondary_colour, uint16_t rotation, uint16_t scale_xaxis, uint16_t scale_yaxis, uint8_t flip_image, uint8_t style )
{
	MapObject * map_object = NULL;
	MokoiMap * map = NULL;


	if ( lux::game_system )
	{
		map = lux::game_system->GetMap( map_ident );
		if ( map )
		{
			cell_colour colour;
			colour.hex = elix::endian::host32(primary_colour);
			map_object->effects.primary_colour = colour.rgba;

			colour.hex = elix::endian::host32(secondary_colour);
			map_object->effects.secondary_colour = colour.rgba;

			map_object->effects.rotation = rotation;
			map_object->effects.scale_xaxis = scale_xaxis;
			map_object->effects.scale_yaxis = scale_yaxis;
			map_object->effects.flip_image = flip_image;
			map_object->effects.style = style;

			return 1;
		}
	}
	return 0;
}

/**
 * @brief Lux_FFI_Map_Edit_Object_Replace
 * @param map_ident
 * @param object_id
 * @param type
 * @param sprite
 * @return
 */
int32_t Lux_FFI_Map_Edit_Object_Replace(const uint32_t map_ident, uint32_t object_id, uint8_t type, const char *sprite )
{
	MapObject * map_object = NULL;
	MokoiMap * map = NULL;


	if ( lux::game_system )
	{
		map = lux::game_system->GetMap( map_ident );
		if ( map )
		{
			// Check if we are really changing object
			if ( map_object->type == type )
			{
				if ( !map_object->sprite.compare(sprite) )
					return 0;
			}

			if ( map_object->type == OBJECT_SPRITE && type != OBJECT_SPRITE )
			{
				map_object->position.w = map_object->position.w ? map_object->position.w : 32;
				map_object->position.h = map_object->position.h ? map_object->position.h : 32;
			}

			map_object->FreeData();

			map_object->sprite.assign(sprite);
			map_object->type = type;
			map_object->timer = 0;

			return map_object->has_data ? 1 : -1;
		}
	}
	return -2;
}

/**
 * @brief Lux_FFI_Map_Edit_Object_Flag
 * @param map_ident
 * @param object_id
 * @param key
 * @param value
 * @return
 */
int16_t Lux_FFI_Map_Edit_Object_Flag( const uint32_t map_ident, const uint32_t object_id, const uint8_t key, const int16_t value )
{
	int16_t value_return = 0;
	MapObject * map_object = NULL;
	MokoiMap * map = NULL;


	if ( lux::game_system )
	{
		map = lux::game_system->GetMap( map_ident );
		if ( map )
		{
			switch ( key )
			{
				case 0:
					map_object->speed = (int16_t)value;
					value_return = map_object->speed;
					break;
				case 1:
					map_object->loop = (bool)value;
					value_return = map_object->loop;
					break;
				case 2:
					map_object->reverse = (bool)value;
					value_return = map_object->reverse;
					break;
				case 4:
					map_object->timer = (int16_t)value;
					value_return = map_object->timer;
					break;
				case 5:
					map_object->auto_delete = (bool)value;
					value_return = map_object->auto_delete;
					break;
				case 6:
					map_object->hidden = (bool)value;
					value_return = map_object->hidden;
					break;
			}

			return value_return;
		}
	}

	return 0;
}

/**
 * @brief Lux_FFI_Map_Edit_Object_Delete
 * @param map_ident
 * @param object_id
 * @return
 */
uint8_t Lux_FFI_Map_Edit_Object_Delete( const uint32_t map_ident, const uint32_t object_id )
{
	MokoiMap * map = NULL;

	if ( lux::game_system )
	{
		map = lux::game_system->GetMap( map_ident );
		if ( map )
		{
			map->RemoveObject( object_id );
		}
	}
	return 0;
}
