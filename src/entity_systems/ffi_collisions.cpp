/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_collisions.h"
#include "ffi_functions.h"
#include "entity_manager.h"
#include <stdint.h>
#include <cmath>


/** Lux_FFI_Entity_Object_Collision_Set
*
*
*/
int32_t Lux_FFI_Entity_Object_Collision_Set( Entity * wanted, int32_t rect, int32_t type, int32_t x, int32_t y, int32_t w, int32_t h )
{
	if ( wanted != NULL )
	{
		return wanted->AddCollision(rect, x, y, w, h, type);
	}
	return -1;
}

/** Lux_FFI_Entity_Object_Collision_Get
*
*
*/
uint8_t Lux_FFI_Entity_Object_Collision_Get( Entity * wanted, int32_t rect, int32_t * x, int32_t * y, int32_t * w, int32_t * h )
{
	if ( rect >= 0 && rect < 7 )
	{
		if ( wanted != NULL )
		{
			if ( wanted->_collisions[rect].added )
			{
				if (x)
					*x = wanted->_collisions[rect].rect.x;

				if (y)
					*y = wanted->_collisions[rect].rect.y;

				if (w)
					*w = wanted->_collisions[rect].rect.w;

				if (h)
					*h = wanted->_collisions[rect].rect.h;
				return 1;
			}
		}
	}
	return 0;
}

/** Lux_FFI_Entity_Object_Collision_Calculate
*
*
*/
int32_t Lux_FFI_Entity_Object_Collision_Calculate( Entity * wanted )
{

	if ( wanted )
	{
		return wanted->GetHits();
	}
	return 0;
}

/** Lux_FFI_Entity_Object_Collision_Calculate_Current
*
*
*/
int32_t Lux_FFI_Entity_Object_Collision_Calculate_Current(Entity * wanted, uint32_t * entity_hit, int32_t * angle, int32_t * dist, int32_t * rect, int32_t * type )
{
	CollisionResult * collision_value = NULL;
	int16_t y_dist;
	int16_t x_dist;
	int32_t hit_count = -1;
	if ( wanted )
	{
		hit_count = wanted->GetHitsCount();

		if ( hit_count )
		{
			collision_value = wanted->GetCurrentHit();

			if ( collision_value )
			{
				x_dist = collision_value->rect_x - collision_value->hit_x;
				y_dist = collision_value->rect_y - collision_value->hit_y;

				if ( entity_hit )
				{
					*entity_hit = collision_value->entity_id;
				}

				if ( angle )
				{
					*angle = (int32_t)(atan2((float)y_dist, (float)x_dist) * 57.29578);
					*angle += 90;
				}

				if ( dist )
				{
					x_dist *= x_dist;
					y_dist *= y_dist;
					*dist = (int32_t)sqrt( (float)(x_dist + y_dist));
				}

				if ( rect )
					*rect = collision_value->hit_rect;

				if ( type )
					*type = collision_value->type;
			}
		}

	}
	return hit_count;
}

/** Lux_FFI_Entity_Object_Collision_Set_From_Object
*
*
*/
int32_t Lux_FFI_Entity_Object_Collision_Set_From_Object( Entity * wanted, uint32_t object_id, int32_t type )
{
	MapObject * object = NULL;
	LuxRect rect[7] = {};

	if ( wanted != NULL )
	{
		object = Lux_FFI_Object_Get( object_id );

		if ( object )
		{

			object->CollisionRectangle( rect );
			for ( uint8_t c = 0; c < 7; c++ )
			{
				wanted->AddCollision(c, rect[c].x, rect[c].y, rect[c].w, rect[c].h, type );
			}
			return 6;
		}
	}
	return -1;
}


/** Collision Functions */
/** Lux_FFI_Collision_Set
*
*
*/
int32_t Lux_FFI_Collision_Set( uint32_t hash_entity, int32_t rect, int32_t type, int32_t x, int32_t y, int32_t w, int32_t h )
{

	Entity * wanted = lux::entities->GetEntity(hash_entity);
	if ( wanted != NULL )
	{
		return wanted->AddCollision(rect, x, y, w, h, type);
	}
	return -1;
}

/** Lux_FFI_Collision_Get
*
*
*/
uint8_t Lux_FFI_Collision_Get( uint32_t hash_entity, int32_t rect, int32_t * x, int32_t * y, int32_t * w, int32_t * h )
{
	if ( rect >= 0 && rect < 7 )
	{
		Entity * wanted = lux::entities->GetEntity(hash_entity);
		if ( wanted != NULL )
		{
			if ( wanted->_collisions[rect].added )
			{
				if (x)
					*x = wanted->_collisions[rect].rect.x;

				if (y)
					*y = wanted->_collisions[rect].rect.y;

				if (w)
					*w = wanted->_collisions[rect].rect.w;

				if (h)
					*h = wanted->_collisions[rect].rect.h;
				return 1;
			}

		}
	}
	return 0;
}

/** Lux_FFI_Collision_Check
*
*
*/
int32_t Lux_FFI_Collision_Check( uint32_t hash_entity_first, uint32_t hash_entity_second, int32_t rect_first, int32_t rect_second )
{
	Entity * first_entity, * second_entity;

	first_entity = lux::entities->GetEntity(hash_entity_first);
	if ( first_entity != NULL )
	{
		second_entity = lux::entities->GetEntity(hash_entity_second);
		if ( second_entity != NULL )
		{
			if ( rect_first == -1 )
			{
				int32_t icollision= 0;
				for(uint8_t n = 0; n < 7; n++)
				{
					icollision += first_entity->CheckCollision(-1, second_entity->_collisions[n].rect);
				}
				return icollision;
			}
			else if (0 <= rect_first && rect_first < 7)
			{
				return first_entity->CheckCollision(rect_second, second_entity->_collisions[rect_first].rect);
			}
		}
	}
	return 0;
}

/** Lux_FFI_Collision_Calculate
*
*
*/
int32_t Lux_FFI_Collision_Calculate( uint32_t hash_entity )
{
	Entity * wanted_entity = lux::entities->GetEntity(hash_entity);
	if ( wanted_entity )
	{
		return wanted_entity->GetHits();
	}
	return 0;
}

/** Lux_FFI_Collision_Calculate_Current
*
*
*/
int32_t Lux_FFI_Collision_Calculate_Current( uint32_t hash_entity, uint32_t * entity_hit, int32_t * angle, int32_t * dist, int32_t * rect, int32_t * type )
{
	Entity * wanted_entity = lux::entities->GetEntity(hash_entity);
	CollisionResult * collision_value = NULL;
	int16_t y_dist;
	int16_t x_dist;
	int32_t hit_count = -1;
	if ( wanted_entity )
	{
		hit_count = wanted_entity->GetHitsCount();

		if ( hit_count )
		{
			collision_value = wanted_entity->GetCurrentHit();

			if ( collision_value )
			{
				x_dist = collision_value->rect_x - collision_value->hit_x;
				y_dist = collision_value->rect_y - collision_value->hit_y;

				if ( entity_hit )
				{
					*entity_hit = collision_value->entity_id;
				}

				if ( angle )
				{
					*angle = (int32_t)(atan2((float)y_dist, (float)x_dist) * 57.29578);
					*angle += 90;
				}

				if ( dist )
				{
					x_dist *= x_dist;
					y_dist *= y_dist;
					*dist = (int32_t)sqrt( (float)(x_dist + y_dist));
				}

				if ( rect )
					*rect = collision_value->hit_rect;

				if ( type )
					*type = collision_value->type;
			}
		}

	}
	return hit_count;
}

/** Lux_FFI_Collision_Set_From_Object
*
*
*/
int32_t Lux_FFI_Collision_Set_From_Object( uint32_t hash_entity, uint32_t object_id, int32_t type )
{
	MapObject * object = NULL;
	Entity * wanted = lux::entities->GetEntity(hash_entity);
	LuxRect rect[7] = {};

	if ( wanted != NULL )
	{
		object = Lux_FFI_Object_Get( object_id );

		if ( object )
		{

			object->CollisionRectangle( rect );
			for ( uint8_t c = 0; c < 7; c++ )
			{
				wanted->AddCollision(c, rect[c].x, rect[c].y, rect[c].w, rect[c].h, type );
			}
			return 6;
		}
	}
	return -1;
}
