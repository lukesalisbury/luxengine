/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_object.h"
#include "ffi_functions.h"

#include "display.h"
#include "world.h"
#include "elix_endian.hpp"


/** Display Object Functions */

/** Lux_FFI_Object_Create
*
* uint8_t global,
* uint8_t type,
* int32_t x,
* int32_t y,
* int32_t z,
* uint16_t w,
* uint16_t h,
* uint32_t colour,
* char * sprite
*/
uint32_t Lux_FFI_Object_Create( const uint8_t global, const uint8_t type, const int32_t x, const int32_t y,
								const int32_t z, const uint16_t w, const uint16_t h, const uint32_t colour, const char * sprite )
{
	MapObject * map_object = NULL;

	if ( lux::world->active_map )
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

		if ( global )
			return lux::world->AddObject(map_object, true);
		else
			return lux::world->active_map->AddObject(map_object, true);
	}
	return 0;

}


/** Lux_FFI_Object_Postion
*
* uint32_t object
* int32_t x
* int32_t y
* int32_t z
* uint16_t w
* uint16_t h
*/
int32_t Lux_FFI_Object_Postion( uint32_t object_id, const int32_t x, const int32_t y, const int32_t z,
								const uint16_t w, const uint16_t h )
{
	MapObject * map_object = NULL;

	map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
	{
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

		return 1;
	}

	return 0;
}


/** Lux_FFI_Object_Info
*
* uint32_t object_id,
* uint16_t * w,
* uint16_t * h
*/
int32_t Lux_FFI_Object_Info( const uint32_t object_id, uint16_t * w, uint16_t * h )
{
	MapObject * map_object = NULL;

	map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
	{
		if ( w )
		{
			*w = map_object->position.w;
		}
		if ( h )
		{
			*h = map_object->position.h;
		}
		return 1;

	}
	return 0;
}


/** Lux_FFI_Object_Effect
*
* uint32_t object_id,
* uint32_t primary_colour,
* uint32_t secondary_colour,
* uint16_t rotation,
* uint16_t scale_xaxis,
* uint16_t scale_yaxis,
* uint8_t flip_image,
* uint8_t style
*/
int32_t Lux_FFI_Object_Effect( uint32_t object_id, uint32_t primary_colour, uint32_t secondary_colour, uint16_t rotation, uint16_t scale_xaxis, uint16_t scale_yaxis, uint8_t flip_image, uint8_t style )
{
	MapObject * map_object = NULL;

	map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
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
	return 0;
}



/** Lux_FFI_Object_Replace
*
* uint32_t object_id,
* uint8_t type,
* char * sprite
*/
int32_t Lux_FFI_Object_Replace( uint32_t object_id, uint8_t type, char * sprite )
{
	MapObject * map_object = NULL;

	map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
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
	return -2;
}

/** pawnObjectFlag
*
* uint32_t object_id, uint8_t key, int16_t value
*/
int16_t Lux_FFI_Object_Flag( uint32_t object_id, uint8_t key, int16_t value )
{
   MapObject * map_object = NULL;

   map_object = Lux_FFI_Object_Get( object_id );

   if ( map_object )
   {
		switch ( key )
		{
			case 0:
				map_object->speed = (int16_t)value;
				break;
			case 1:
				map_object->loop = (bool)value;
				break;
			case 2:
				map_object->reverse = (bool)value;
				break;
			case 4:
				map_object->timer = (int16_t)value;
				break;
			case 5:
				map_object->auto_delete = (bool)value;
				break;
			case 6:
				map_object->hidden = (bool)value;
				break;
		}

		return 1;
	}

	return 0;
}

/** Lux_FFI_Object_Delete
*
* uint32_t object_id
*/
uint8_t Lux_FFI_Object_Delete( uint32_t object_id )
{
	if ( lux::world->active_map )
	{
		if ( object_id >= OBJECT_GLOBAL_VALUE )
			return lux::world->RemoveObject( object_id );
		else
			return lux::world->active_map->RemoveObject( object_id );
	}
	return 0;
}


/** Animation */

/** Lux_FFI_Animation_Length
*
* const char * sheet, const char * sprite
*/
uint32_t Lux_FFI_Animation_Length( const char * sheet, const char * sprite )
{
	return lux::display->isAnimation( sheet, sprite );
}

/** Lux_FFI_Animation_Create
*
* const char * animation_name
*/
uint32_t Lux_FFI_Animation_Create( const char * animation_name )
{

	return 0;
}

/** Lux_FFI_Animation_Insert
*
* const char * animation_name, const char * sprite, const uint32_t time_ms
*/
uint32_t Lux_FFI_Animation_Insert( const char * animation_name, const char * sprite, const uint32_t time_ms  )
{
	return 0;
}

/** Text */

/** pawnTextSprites
* native TextSprites(able);

*/
void Lux_FFI_Text_Sprites( const uint8_t able, const char * sheet )
{

	lux::display->SetTextFont( (able ? true : false), sheet );

}

/** Polygon */

/** Lux_FFI_Polygon_Create
*
* const char * name
*/
uint32_t Lux_FFI_Polygon_Create( const char * name )
{
	return 0;
}

/** Lux_FFI_Polygon_Add_Point
*
* const char * name, int32_t x, int32_t y
*/
uint32_t Lux_FFI_Polygon_Add_Point( const char * name, int32_t x, int32_t y )
{

	return 0;
}
