/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "layers.h"
#include "display.h"
#include "world.h"
#include "core.h"
#include "misc_functions.h"
#include "game_config.h"

#include "gles/gles.hpp"

#define MAX_LAYER_OBJECTS 20480

bool ObjectSort(MapObject * a, MapObject * b )
{
	if ( a->position.z < b->position.z )
		return true;


	return false;
}



ObjectEffect Lux_Util_MergeEffects( LuxColour mod, ObjectEffect source )
{
	ObjectEffect newfx = source;
	float fr, fg, fb, fa = 1.0f;
	fr = (float)mod.r / 255;
	fg = (float)mod.g / 255;
	fb = (float)mod.b / 255;
	fa = (float)mod.a / 255;
	newfx.primary_colour.r = (uint8_t)((float)source.primary_colour.r * fr);
	newfx.primary_colour.g = (uint8_t)((float)source.primary_colour.g * fg);
	newfx.primary_colour.b = (uint8_t)((float)source.primary_colour.b * fb);
	newfx.primary_colour.a = (uint8_t)((float)source.primary_colour.a * fa);
	newfx.secondary_colour.r = (uint8_t)((float)source.secondary_colour.r * fr);
	newfx.secondary_colour.g = (uint8_t)((float)source.secondary_colour.g * fg);
	newfx.secondary_colour.b = (uint8_t)((float)source.secondary_colour.b * fb);
	newfx.secondary_colour.a = (uint8_t)((float)source.secondary_colour.a * fa);
	return newfx;
}

/* Layer

*/
Layer::Layer(DisplaySystem * parent, uint8_t layer_value, bool static_layer )
{
	this->_x = 0;
	this->_y = 0;
	this->_z = 0;
	this->_mapx = 0;
	this->_mapy = 0;
	this->_cachedynamic = false;
	this->_parent = parent;
	this->_roll = this->_pitch = this->_yaw = 0;
	this->colour = (LuxColour){ 255, 255, 255, 255 };
	this->display_dimension = parent->screen_dimension;
	this->display_dimension.x -= 16;
	this->display_dimension.y -= 16;
	this->refreshz = true;
	this->display_layer = layer_value;




	this->static_layer = static_layer;
	this->shader = SHADER_DEFAULT;
}

Layer::~Layer()
{
	this->ClearAll();
}

/* Object Management Functions */
bool Layer::AddObject(MapObject * new_object, bool static_object)
{
	if ( !new_object->has_data )
	{
		new_object->SetData( new_object->type);
	}
	if ( static_object )
	{
		if ( this->objects_static.size() < MAX_LAYER_OBJECTS )
		{
			this->objects_static.push_back(new_object);
			new_object->layer_reference++;
			return true;
		}
	}
	else
	{
		if ( this->objects_dynamic.size() < MAX_LAYER_OBJECTS )
		{
			this->objects_dynamic.push_back(new_object);
			new_object->layer_reference = 1;
			return true;
		}
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | No more object can be added to this layer. " << std::endl;
	return false;
}

bool Layer::RemoveObject(MapObject * new_object)
{
	if ( new_object->layer_reference )
		new_object->layer_reference--;
	if ( !new_object->layer_reference )
	{
		this->objects_static.remove(new_object);
	}
	return true;
}

bool Layer::ClearObjects(bool static_object)
{
	if (static_object)
		this->objects_static.clear();
	else
		this->objects_dynamic.clear();
	return true;
}

bool Layer::ClearAll()
{
	this->objects_static.clear();
	this->objects_dynamic.clear();
	return true;
}

/* Camera Functions */
void Layer::SetOffset( fixed x, fixed y )
{
	this->_x = x;
	this->_y = y;
}

void Layer::SetRotation(int16_t roll, int16_t pitch, int16_t yaw)
{
	this->_roll = roll;
	this->_pitch = pitch;
	this->_yaw = yaw;
}

/* GetObjectLocation
** Returns the screen position of the object.
**
*/
LuxRect Layer::GetObjectLocation(LuxRect map_location, uint8_t type)
{
	if ( this->static_layer )
	{
		return map_location;
	}
	LuxRect screen_location = map_location;

	screen_location.x -= MAKE_FIXED_INT(this->_x + this->_mapx);
	screen_location.y -= MAKE_FIXED_INT(this->_y + this->_mapy);
	if (type == 'l')
	{
		screen_location.w -= MAKE_FIXED_INT(this->_x + this->_mapx);
		screen_location.h -= MAKE_FIXED_INT(this->_y + this->_mapy);
	}

	return screen_location;
}

/*  */
bool Layer::ObjectOnScreen(LuxRect o, uint8_t flipmode, LuxRect s)
{
	if(s.x + s.w < o.x)	return 0;
	if(s.y + s.h < o.y)	return 0;
	if ( flipmode == 1 || flipmode == 3 ||  flipmode == 17 ||  flipmode == 19 )
	{
		if(s.x - 48 > o.x + o.h)	return 0;
		if(s.y - 48 > o.y + o.w)	return 0;
	}
	else
	{
		if(s.x - 48 > o.x + o.w)	return 0;
		if(s.y - 48 > o.y + o.h)	return 0;
	}
	return 1;
}

bool Layer::ObjectOnVirtualScreen(LuxRect & o, uint8_t flipmode, LuxRect s, uint32_t offx, uint32_t offy )
{
	o.x += offx;
	o.y += offy;

	if(s.x + s.w < o.x)	return 0;
	if(s.y + s.h < o.y)	return 0;
	if ( flipmode == 1 || flipmode == 3 ||  flipmode == 17 || flipmode == 19)
	{
		if(s.x - 48 > o.x + o.h)	return 0;
		if(s.y - 48 > o.y + o.w)	return 0;
	}
	else
	{
		if(s.x - 48 > o.x + o.w)	return 0;
		if(s.y - 48 > o.y + o.h)	return 0;
	}
	return 1;
}

/* Caching Functions */
void Layer::GetSurface() {}

/* Drawing Functions */
void Layer::Display()
{
	MapObject * object;
	std::list<MapObject*>::iterator l_object;
	if ( lux::gameworld )
	{
		if ( lux::gameworld->active_map )
		{
			/* Map Offsets */
			this->_mapx = lux::gameworld->active_map->GetPosition(0);
			this->_mapy = lux::gameworld->active_map->GetPosition(1);

			this->_mapw = MAKE_FIXED_INT(lux::gameworld->active_map->map_width);
			this->_maph = MAKE_FIXED_INT(lux::gameworld->active_map->map_height);

			this->wrap_layer = lux::gameworld->active_map->wrap_mode;
			if ( this->wrap_layer )
			{
				/* Get valid top left poisition */
				this->wrap_mapw = (MAKE_FIXED_INT(this->_mapx) / this->_mapw) * this->_mapw;
				this->wrap_maph = (MAKE_FIXED_INT(this->_mapy) / this->_maph) * this->_maph;

				this->wrap_dimension.x = ( this->wrap_layer == MAP_WRAPYAXIS ? this->_mapx : this->_mapx - this->_mapw ); // MAP_WRAPXAXIS or MAP_WRAPBOTH enabled
				this->wrap_dimension.y = ( this->wrap_layer == MAP_WRAPXAXIS ? this->_mapy : this->_mapy - this->_maph ); // MAP_WRAPYAXIS or MAP_WRAPBOTH enabled
				this->wrap_dimension.w = ( this->wrap_layer == MAP_WRAPYAXIS ? this->_mapw : this->_mapw * 2 ); // MAP_WRAPXAXIS or MAP_WRAPBOTH enabled
				this->wrap_dimension.h = ( this->wrap_layer == MAP_WRAPXAXIS ? this->_maph : this->_maph * 2 ); // MAP_WRAPYAXIS or MAP_WRAPBOTH enabled
			}
		}
	}

	this->_parent->graphics.SetRotation( this->_roll, this->_pitch, this->_yaw);

	/* QUICK HACK TO SUPPORT BETTER Z INDEXING */
	if ( this->refreshz )
	{
		this->objects_static.sort( ObjectSort );
	}
	this->refreshz = false;
	/* QUICK HACK TO SUPPORT BETTER Z INDEXING */

	LuxRect obj_position;
	if ( this->objects_static.size() )
	{
		for ( l_object = this->objects_static.begin(); l_object != this->objects_static.end(); l_object++ )
		{
			object = (*l_object);
			if (!object)
				break;

			if ( !object->hidden )
			{
				obj_position = this->GetObjectLocation(object->position, object->type);
				if ( this->wrap_layer )
				{
					/* Set object position to the valid top left poisition */
					obj_position.x += this->wrap_mapw;
					obj_position.y += this->wrap_maph;
					int32_t initial_y = obj_position.y;

					if ( !object->has_data )
						object->SetData( NULL, object->type );

					for ( int32_t x = this->wrap_dimension.x; x < this->wrap_dimension.x + this->wrap_dimension.w; x += this->_mapw  )
					{
						obj_position.y = initial_y;
						for ( int32_t y = this->wrap_dimension.y; y < this->wrap_dimension.y + this->wrap_dimension.h; y += this->_maph  )
						{
							if ( this->ObjectOnScreen( obj_position, object->effects.flip_image, this->_parent->screen_dimension ) )
							{
								this->_parent->DrawMapObject( object, obj_position, Lux_Util_MergeEffects(this->colour, object->effects) );
							}
							obj_position.y += this->_maph;
						}
						obj_position.x += this->_mapw;
					}
				}
				else if ( this->ObjectOnScreen( obj_position, object->effects.flip_image, this->_parent->screen_dimension ) )
				{
					if ( !object->has_data )
					{
						object->SetData( NULL, object->type );
					}
					this->_parent->DrawMapObject( object, obj_position, Lux_Util_MergeEffects(this->colour, object->effects) );
				}
			}
		}
	}

	if ( this->objects_dynamic.size() )
	{
		for ( l_object = this->objects_dynamic.begin(); l_object != this->objects_dynamic.end(); l_object++ )
		{

			object = (*l_object);
			obj_position = this->GetObjectLocation(object->position, object->type);
			if ( ObjectOnScreen( obj_position, object->effects.flip_image, this->_parent->screen_dimension) )
			{
				this->_parent->DrawMapObject(object, obj_position, Lux_Util_MergeEffects(this->colour,object->effects) );
			}
			delete object;
		}
		if ( !this->_cachedynamic )
		{
			this->objects_dynamic.clear();
		}
	}
}




void Layer::SetShader( uint8_t new_shader )
{
	if ( new_shader < NUM_SHADERS )
	{
		this->shader = new_shader;
	}
}
