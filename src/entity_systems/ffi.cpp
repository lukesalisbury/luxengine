/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi.hpp"
#include "entity_manager.h"



/** Collision Functions */
/** pawnCollisionSet
* native CollisionSet(id[], rect = 0, type = 0, x = 0, y = 0, w = 0, h = 0, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionSet(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity != NULL )
	{
		return wanted_entity->AddCollision(params[2], params[4], params[5], params[6], params[7], params[3]);
	}
	return -1;
}

/** pawnCollisionGet
* native CollisionGet(id[], rect, &x, &y, &w, &h, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionGet(AMX *amx, const cell *params)
{
	cell * valueptr = NULL;
	if ( params[2] >= 0 && params[2] < 7 )
	{
		Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
		if ( wanted_entity != NULL )
		{
			if ( wanted_entity->_collisions[params[2]].added )
			{
				valueptr = amx_Address(amx, params[3]);
				if (valueptr)
					*valueptr = wanted_entity->_collisions[params[2]].rect.x;

				valueptr = amx_Address(amx, params[4]);
				if (valueptr)
					*valueptr = wanted_entity->_collisions[params[2]].rect.y;

				valueptr = amx_Address(amx, params[5]);
				if (valueptr)
					*valueptr = wanted_entity->_collisions[params[2]].rect.w;

				valueptr = amx_Address(amx, params[6]);
				if (valueptr)
					*valueptr = wanted_entity->_collisions[params[2]].rect.h;
				return 1;
			}

		}
	}
	return -1;
}

/** pawnCollisionCheck
* native CollisionCheck(id1[], id2[], rect1 = -1, rect2 = -1, length1=sizeof id1, length2=sizeof id2);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionCheck(AMX *amx, const cell *params)
{
	Entity * first_entity, * second_entity;
	first_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( first_entity != NULL )
	{
		second_entity = Lux_PawnEntity_GetEntity(amx, params[2]);
		if ( second_entity != NULL )
		{
			if (params[3] == -1)
			{
				int icollision= 0;
				for(int n = 0; n < 7; n++)
				{
					icollision += first_entity->CheckCollision(-1, second_entity->_collisions[n].rect);
				}
				return icollision;
			}
			else if (0 <= params[3] && params[3] < 7)
			{
				return first_entity->CheckCollision(params[4], second_entity->_collisions[params[3]].rect);
			}
		}
	}
	return 0;
}

/** pawnCollisionCalculate
* native CollisionCalculate(id[] = SELF, rect = -1, type = -1, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionCalculate(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity )
	{
		return wanted_entity->GetHits();
	}
	return -1;
}

/** pawnCollisionGetCurrent
* native CollisionGetCurrent(id[] = SELF, current[], &angle, &dist, &rect, &type, maxlength=sizeof current, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionGetCurrent(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity )
	{
		uint32_t hit_count = wanted_entity->GetHitsCount();
		if( hit_count )
		{
			cell * cptr = amx_Address(amx, params[2] );
			CollisionResult * value = wanted_entity->GetCurrentHit();
			amx_SetString(cptr, value->entity_id.c_str(), 1, 0, params[7]);

			int16_t x_dist = value->rect_x - value->hit_x;
			int16_t y_dist = value->rect_y - value->hit_y;
			cell * xptr, * yptr;
			xptr = amx_Address(amx, params[3]);
			if (xptr)
			{
				*xptr = (cell)(atan2((float)y_dist, (float)x_dist) * 57.29578);
				*xptr += 90;
			}
			yptr = amx_Address(amx, params[4]);
			if (yptr)
			{
				x_dist *= x_dist;
				y_dist *= y_dist;
				*yptr = (cell)sqrt( (float)(x_dist + y_dist));
			}
			xptr = amx_Address(amx, params[5]);
			if (xptr)
				*xptr = value->hit_rect;
			xptr = amx_Address(amx, params[6]);
			if (xptr)
				*xptr = value->type;
		}
		return hit_count;
	}
	return -1;
}

/** pawnCollisionFromObject
* native CollisionFromObject( obj, type = 0, id[] = SELF, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionFromObject(AMX *amx, const cell *params)
{
	Entity * wanted = Lux_PawnEntity_GetEntity(amx, params[3]);
	if ( wanted != NULL )
	{
		if ( lux::world->map )
		{
			MapObject * object = NULL;
			if ( wanted->_mapid == 0 )
				object = lux::world->GetObject(params[1]);
			else
				object = lux::world->map->GetObject(params[1]);

			if ( object )
			{
				LuxRect rect[7] = {};
				object->CollisionRectangle( rect );
				for ( uint8_t c = 0; c < 7; c++ )
				{
					wanted->AddCollision(c, rect[c].x, rect[c].y, rect[c].w, rect[c].h, params[2]);
				}
				return 6;
			}
		}
	}
	return -1;
}

/* Paths */

/** pawnPathCount
* native PathCount( object:id );
*
*/
static cell AMX_NATIVE_CALL pawnPathCount(AMX *amx, const cell *params)
{
	Entity * wanted = Lux_PawnEntity_GetEntity(amx, params[3]);
	if ( wanted != NULL )
	{
		if ( lux::world->map )
		{
			MapObject * object = NULL;
			if ( wanted->_mapid == 0 )
				object = lux::world->GetObject(params[1]);
			else
				object = lux::world->map->GetObject(params[1]);

			if ( object )
			{
				return (cell)object->_path.size();
			}
		}
	}
	return -1;
}

/** pawnPathPoints
* native PathPoints( object:id, point, &x, &y, &t);
*
*/
static cell AMX_NATIVE_CALL pawnPathPoints(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;
	Entity * wanted = Lux_PawnEntity_GetEntity(amx, params[3]);
	if ( wanted != NULL )
	{
		if ( lux::world->map )
		{
			MapObject * object = NULL;
			if ( wanted->_mapid == 0 )
				object = lux::world->GetObject(params[1]);
			else
				object = lux::world->map->GetObject(params[1]);

			if ( object )
			{
				LuxPath next;
				next = object->_path.at(params[2]);
				cell * xptr = NULL;
				xptr = amx_Address(amx, params[3]);
if (xptr)
					*xptr = next.x;
				xptr = amx_Address(amx, params[4]);
if (xptr)
					*xptr = next.y;
				return 1;
			}
		}
	}

	return 0;
}

