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
#include "game_system.h"
#include "elix_endian.hpp"


/** Display Object Functions */

/**
 * @brief Lux_FFI_Object_Create
 * @param global
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
uint32_t Lux_FFI_Object_Create( const uint8_t global, const uint8_t type, const int32_t x, const int32_t y,
								const fixed z, const uint16_t w, const uint16_t h, const uint32_t colour, const char * sprite )
{
	MapObject * map_object = NULL;
	uint32_t ident = 0;

	if ( lux::gamesystem->active_map )
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

		if ( (type == OBJECT_SPRITE || type == OBJECT_CANVAS) && sprite != NULL )
			map_object->sprite.assign( sprite );
		if ( (type == OBJECT_TEXT) && sprite != NULL )
			map_object->sprite.assign( sprite );

		if ( global )
			ident = lux::gamesystem->GetObjects()->AddObject(map_object, true);
		else
			ident = lux::gamesystem->active_map->AddObject(map_object, true);
	}
	return ident;

}


/**
 * @brief Lux_FFI_Object_Postion
 * @param object_id
 * @param x
 * @param y
 * @param z
 * @param w
 * @param h
 * @return
 */
int32_t Lux_FFI_Object_Position(uint32_t object_id, const int32_t x, const int32_t y, const int32_t z,
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


/**
 * @brief Lux_FFI_Object_Info
 * @param object_id
 * @param w
 * @param h
 * @return
 */
int32_t Lux_FFI_Object_Info(const uint32_t object_id, uint16_t * w, uint16_t * h, int32_t * x, int32_t *y )
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

		if ( x )
		{
			*x = map_object->position.x;
		}
		if ( h )
		{
			*y = map_object->position.y;
		}
		return 1;

	}
	return 0;
}


/**
 * @brief Lux_FFI_Object_Effect
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



/**
 * @brief Lux_FFI_Object_Replace
 * @param object_id
 * @param type
 * @param sprite
 * @return
 */
int32_t Lux_FFI_Object_Replace(uint32_t object_id, uint8_t type, const char *sprite )
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

/**
 * @brief Lux_FFI_Object_Flag
 * @param object_id
 * @param key
 * @param value
 * @return
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

/**
 * @brief Lux_FFI_Object_Delete
 * @param object_id
 * @return
 */
uint8_t Lux_FFI_Object_Delete( uint32_t object_id )
{
	if ( lux::gamesystem->active_map )
	{
		if ( (object_id & OBJECT_GLOBAL_VALUE) == OBJECT_GLOBAL_VALUE )
			return lux::gamesystem->GetObjects()->RemoveObject( object_id );
		else
			return lux::gamesystem->active_map->RemoveObject( object_id );
	}
	return 0;
}


/* Animation */

/**
 * @brief Lux_FFI_Animation_Length
 * @param sheet
 * @param sprite
 * @return
 */
uint32_t Lux_FFI_Animation_Length( const char * sheet, const char * sprite )
{
	return lux::display->isAnimation( sheet, sprite );
}

/**
 * @brief Lux_FFI_Animation_Create
 * @param animation_name
 * @return
 */
uint32_t Lux_FFI_Animation_Create( const char * animation_name )
{
	return 0;
}

/**
 * @brief Lux_FFI_Animation_Insert
 * @param animation_name
 * @param sprite
 * @param time_ms
 * @return
 */
uint32_t Lux_FFI_Animation_Insert( const char * animation_name, const char * sprite, const uint32_t time_ms  )
{
	return 0;
}

/* Text */

/**
 * @brief Lux_FFI_Text_Sprites
 * @param able
 * @param sheet
 */
void Lux_FFI_Text_Sprites( const uint8_t able, const char * sheet )
{
	lux::display->SetTextFont( (able ? true : false), sheet );
}

/* Polygon */

/**
 * @brief Lux_FFI_Polygon_Create
 * @param name
 * @return
 */
uint32_t Lux_FFI_Polygon_Create( const char * name )
{
	return 0;
}

/**
 * @brief Lux_FFI_Polygon_Add_Point
 * @param name
 * @param x
 * @param y
 * @return
 */
uint32_t Lux_FFI_Polygon_Add_Point( const char * name, int32_t x, int32_t y )
{

	return 0;
}

/**
 * @brief Lux_FFI_Canvas_Child_Info
 * @param object_id
 * @param child_name
 * @return
 */
uint32_t Lux_FFI_Canvas_Child_Info(uint32_t object_id, uint32_t child_id, int32_t * x, int32_t * y, uint16_t * w, uint16_t * h )
{
	MapObject * map_object = NULL;

	map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
	{
		if ( map_object->IsGlobal() )
		{
			child_id |= OBJECT_GLOBAL_VALUE;
		}
		else
		{
			child_id &= OBJECT_LOCAL_VALUE;
		}

		// Check if we are really changing object
		if ( map_object->type == OBJECT_CANVAS )
		{
			LuxCanvas * canvas = map_object->GetCanvas();
			if ( canvas )
			{
				MapObject * child_object = canvas->FindChild( child_id );
				if ( child_object )
				{
					if ( w )
					{
						*w = child_object->position.w;
					}
					if ( h )
					{
						*h = child_object->position.h;
					}

					if ( x )
					{
						*x = child_object->position.x;
					}
					if ( h )
					{
						*y = child_object->position.y;
					}
					return child_object->GetStaticMapID();

				}
			}

		}


	}
	return 0;
}

/**
 * @brief Lux_FFI_Object_Effect
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
int32_t Lux_FFI_Canvas_Child_Set_Effect( uint32_t object_id, uint32_t child_id, uint32_t primary_colour, uint32_t secondary_colour, uint16_t rotation, uint16_t scale_xaxis, uint16_t scale_yaxis, uint8_t flip_image, uint8_t style )
{
	MapObject * map_object = NULL;

	map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
	{
		if ( map_object->IsGlobal() )
		{
			child_id |= OBJECT_GLOBAL_VALUE;
		}
		else
		{
			child_id &= OBJECT_LOCAL_VALUE;
		}

		// Check if we are really changing object
		if ( map_object->type == OBJECT_CANVAS )
		{
			LuxCanvas * canvas = map_object->GetCanvas();
			if ( canvas )
			{
				MapObject * child_object = canvas->FindChild( child_id );
				if ( child_object )
				{
					cell_colour colour;
					colour.hex = elix::endian::host32(primary_colour);
					child_object->effects.primary_colour = colour.rgba;

					colour.hex = elix::endian::host32(secondary_colour);
					child_object->effects.secondary_colour = colour.rgba;

					child_object->effects.rotation = rotation;
					child_object->effects.scale_xaxis = scale_xaxis;
					child_object->effects.scale_yaxis = scale_yaxis;
					child_object->effects.flip_image = flip_image;
					child_object->effects.style = style;
				}
			}
		}
		return 1;
	}
	return 0;
}
