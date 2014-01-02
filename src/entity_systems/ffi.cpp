/****************************
Copyright © 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi.hpp"
#include "entity_manager.h"

/** Entity Position Functions  */

/** FFIEntitySetPosition
*
* Updates Entity X/Y Positions
*/
void FFIEntitySetPosition( int32_t fixed_x, int32_t fixed_y, int32_t fixed_z, int32_t hash_entity )
{
	#ifdef NETWORKENABLED
	lux::core->NetLock();
	#endif
	Entity * wanted = lux::entities->GetEntity(hash_entity);
	if ( wanted != NULL )
	{
		if ( fixed_x != INT32_MIN )
			wanted->x = (fixed)fixed_x;
		if ( fixed_y != INT32_MIN )
			wanted->y = (fixed)fixed_y;
		if ( fixed_z != CELLMIN )
			wanted->z = (fixed)fixed_z;
	}
	#ifdef NETWORKENABLED
	lux::core->NetUnlock();
	#endif
	return ( wanted != NULL );
}

/** pawnEntityGetPosition
*
* return Entity X/Y Positions
*/
void FFIEntityGetPosition( int32_t * fixed_x, int32_t * fixed_y, int32_t * fixed_z, int32_t hash_entity )
{
#ifdef NETWORKENABLED
	lux::core->NetLock();
#endif
	Entity * wanted = lux::entities->GetEntity(hash_entity);
	if ( wanted != NULL )
	{
		if ( fixed_x )
			*fixed_x = wanted->x;
		if ( fixed_y )
			*fixed_y = wanted->y;
		if ( fixed_z )
			*fixed_z = wanted->z;
	}
#ifdef NETWORKENABLED
	lux::core->NetUnlock();
#endif
	return ( wanted != NULL );
}

/** Entity Functions */

/** pawnEntityGetSetting
* native EntityGetSetting(key[], string[], id[] = SELF, length=sizeof id );
*
*/
static cell AMX_NATIVE_CALL pawnEntityGetSetting(AMX *amx, const cell *params)
{
	cell response = -1;
	cell * stringptr;
	Entity * wanted = Lux_PawnEntity_GetEntity(amx, params[3]);
	if ( wanted != NULL )
	{
		std::string stf_key = Lux_PawnEntity_GetString(amx, params[1] );
		stringptr = amx_Address(amx, params[2]);
		if ( stringptr )
		{
			std::string str;
			if ( !stf_key.empty() )
				str = wanted->GetSetting(stf_key);
			else
				str = "";
			amx_SetString(stringptr, str.c_str(), 1, false, UNLIMITED);
			response = str.length();
			str.clear();

		}
	}
	return response;
}
/** pawnEntityGetNumber
* native EntityGetNumber(key[], id[] = SELF, length=sizeof id );
*
*/
static cell AMX_NATIVE_CALL pawnEntityGetNumber(AMX *amx, const cell *params)
{
	cell response = -1;
	char * key = NULL;
	Entity * wanted = Lux_PawnEntity_GetEntity(amx, params[2]);
	if ( wanted != NULL )
	{
		amx_StrParam_Type(amx, params[1], key, char*);
		if ( key )
		{
			response =  wanted->GetSettingAsInt(key);
		}
	}
	return response;
}

/** pawnEntityCreate
* native EntityCreate(parententity[], id[], x, y, z, map_id = CURRENT_MAP, idlength = sizeof(id), arg[], ...);
*
*/
static cell AMX_NATIVE_CALL pawnEntityCreate(AMX *amx, const cell *params)
{
	cell response = 0;
	std::string entity_parent, entity_id;
	uint32_t id_length = 0;
	Entity * wanted_entity = NULL;
	cell * id_ptr;

	entity_parent = Lux_PawnEntity_GetString(amx, params[1]);
	entity_id = Lux_PawnEntity_GetString(amx, params[2]);

	if ( params[6] == 0 ) /* GLOBAL */
	{
		wanted_entity = lux::entities->NewEntity(entity_id, entity_parent, 0);
	}
	else /* Map */
	{
		wanted_entity = lux::entities->NewEntity(entity_id, entity_parent, (uint32_t) params[6]);
	}

	if ( wanted_entity != NULL )
	{
		wanted_entity->x = params[3];
		wanted_entity->y = params[4];
		wanted_entity->z = params[5];

		uint32_t num_param = (uint32_t)( params[0]/sizeof(cell) ) - 8;
		if ( num_param > 0 )
		{
			cell * cptr = NULL, * arg_ptr = NULL;
			uint32_t arg_count = 0;
			native_pointer stack_mem = NULL;
			cptr = amx_Address(amx, params[8]);

			if ( *cptr )
			{
				while (*cptr)
				{
					arg_count++;
					cptr++;
				}
				cptr--;
				if ( num_param != arg_count )
				{
					arg_count = std::min(arg_count, num_param);
					num_param = arg_count;
				}
				do
				{
					arg_ptr = amx_Address(amx, params[num_param+8]);
					if ( *cptr == 'a' ) // Array, last value must be CELLMIN
					{
						uint32_t lemgth = 0;
						while ( *arg_ptr != CELLMIN )
						{
							lemgth++;
							arg_ptr++;
							if ( lemgth > 256 )
							{
								break;
							}
						}
						arg_ptr -= lemgth;
						lemgth++;
						wanted_entity->callbacks->PushArrayNative(wanted_entity->_data, arg_ptr, lemgth, (stack_mem ? NULL : &stack_mem) );
					}
					else if ( *cptr == 's' ) // String
					{
						uint32_t lemgth = 0;
						while ( *arg_ptr != 0 )
						{
							lemgth++;
							arg_ptr++;
							if ( lemgth > 256 )
							{
								break;
							}
						}
						arg_ptr -= lemgth;
						lemgth++;
						wanted_entity->callbacks->PushArrayNative(wanted_entity->_data, arg_ptr, lemgth, (stack_mem ? NULL : &stack_mem) );
					}
					else if ( *cptr == 'd' ) // Value, passed by reference
					{
						wanted_entity->callbacks->PushArrayNative(wanted_entity->_data, arg_ptr, 1, (stack_mem ? NULL : &stack_mem) );  //Pass an argument or array “by-reference”
					}
					else if ( *cptr == 'n' ) //Value, passed by value
					{
						wanted_entity->callbacks->Push(wanted_entity->_data, (int32_t)*arg_ptr);
					}
					cptr--;
					num_param--;
				} while ( num_param > 0 );
			}
			wanted_entity->SetMapCollsion();
			wanted_entity->callbacks->Call(wanted_entity->_data, "Init", stack_mem);
			wanted_entity->firstran = true;
		}
		else
		{
			wanted_entity->Init();
		}
		if ( entity_id.at(0) == '*' )
		{
			id_ptr = amx_Address( amx, params[2] );
			if ( id_ptr )
			{
				id_length = Lux_PawnEntity_HasString( amx, params[2] );
				if ( id_length > 2 )
				{
					amx_SetString(id_ptr, wanted_entity->id.c_str(), 1, false, id_length);
					response = 2;
				}
			}
		}
		response = 1;
	}
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | EntityCreate ('" << entity_parent;
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "' '" << entity_id << "') Failed." << std::endl;
		response = 0;
	}
	return response;
}

/** pawnEntityDelete
* native EntityDelete(id[] = SELF, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnEntityDelete(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity != NULL )
	{
		wanted_entity->Delete();
		return 1;
	}
	return 0;
}

/** pawnEntityPublicVariable
* native EntityPublicVariable(id[], variable[]);
*
*/
static cell AMX_NATIVE_CALL pawnEntityPublicVariable(AMX *amx, const cell *params)
{
	cell ret = 0;
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity != NULL )
	{
		std::string varname = Lux_PawnEntity_GetString(amx, params[2]);
		ret = (cell)Lux_PawnEntity_PublicVariable( (AMX *)wanted_entity->_data, varname, NULL );
	}
	return ret;
}

/** pawnEntityPublicVariableSet
* native EntityPublicVariableSet(id[], variable[], value);
*
*/
static cell AMX_NATIVE_CALL pawnEntityPublicVariableSet(AMX *amx, const cell *params)
{
	cell ret = 0;
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity != NULL )
	{
		int32_t value = (int32_t)params[3];
		std::string varname = Lux_PawnEntity_GetString(amx, params[2]);
		ret = (cell)Lux_PawnEntity_PublicVariable( (AMX *)wanted_entity->_data, varname, NULL );

	}
	return ret;
}

/** pawnEntityPublicFunction
* native EntityPublicFunction(id[], function[], args[]="", ...);
*
*/
static cell AMX_NATIVE_CALL pawnEntityPublicFunction(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted_entity != NULL )
	{
		if ( !wanted_entity->_data )
		{
			return -1;
		}
		cell * cptr = NULL, * arg_ptr = NULL;
		std::string function_name;
		uint32_t arg_count = 0;
		uint32_t num_param = (uint32_t)( params[0]/sizeof(cell) ) - 3 ;
		mem_pointer stack_mem = NULL;


		function_name = Lux_PawnEntity_GetString(amx, params[2]);

		/* TODO Move check to Callback */
		int f;
		if ( amx_FindPublic( (AMX *)wanted_entity->_data, function_name.c_str(), &f) )
		{
			return -2;
		}

		cptr = amx_Address(amx, params[3]);
		if ( *cptr )
		{
			while (*cptr)
			{
				arg_count++;
				cptr++;
			}
			cptr--;
			if ( num_param != arg_count )
			{
				arg_count = std::min(arg_count, num_param);
				num_param = arg_count;
			}

			do
			{
				arg_ptr = amx_Address(amx, params[num_param+3]);
				if ( *cptr == 'a' ) // Array, last value must be CELLMIN
				{
					uint32_t lemgth = 0;
					while ( *arg_ptr != CELLMIN )
					{
						lemgth++;
						arg_ptr++;
						if ( lemgth > 256 )
						{
							break;
						}
					}
					arg_ptr -= lemgth;
					lemgth++;
					wanted_entity->callbacks->PushArrayNative(wanted_entity->_data, arg_ptr, lemgth, (stack_mem ? NULL : &stack_mem) );
				}
				else if ( *cptr == 's' ) // String
				{
					uint32_t lemgth = 0;
					while ( *arg_ptr != 0 )
					{
						lemgth++;
						arg_ptr++;
						if ( lemgth > 256 )
						{
							break;
						}
					}
					arg_ptr -= lemgth;
					lemgth++;
					wanted_entity->callbacks->PushArrayNative(wanted_entity->_data, arg_ptr, lemgth, (stack_mem ? NULL : &stack_mem) );
				}
				else if ( *cptr == 'd' ) // Value, passed by reference
				{
					wanted_entity->callbacks->PushArrayNative(wanted_entity->_data, arg_ptr, 1, (stack_mem ? NULL : &stack_mem) );  //Pass an argument or array “by-reference”
				}
				else if ( *cptr == 'n' ) //Value, passed by value
				{
					wanted_entity->callbacks->Push(wanted_entity->_data, (int32_t)*arg_ptr);
				}
				cptr--;
				num_param--;
			} while ( num_param > 0 );
		}
		int32_t results = wanted_entity->callbacks->Call(wanted_entity->_data, (char*)function_name.c_str(), stack_mem);
		return (cell)results;
	}
	return -1;
}

/** Entity Management Functions  */

/** pawnEntitiesList
* native EntitiesList(map_id = CURRENT_MAP);
*
*/
static cell AMX_NATIVE_CALL pawnEntitiesList(AMX *amx, const cell *params)
{
	uint32_t count = 0;
	EntitySection * section = NULL;

	if ( params[1] == 0 ) /* GLOBAL */
	{
		section = lux::world->GetEntities();
	}
	else if ( params[1] )/* Map */
	{
		if ( lux::world->GetMap((uint32_t)params[1]) )
		{
			section = lux::world->GetMap((uint32_t)params[1])->GetEntities();
		}
	}

	if ( section )
	{
		count = section->children.size();
		if ( count )
			section->iter = section->children.begin();
		else
			section->iter = section->children.end();
		return count;
	}
	return 0;
}

/** pawnEntitiesNext
* native EntitiesNext(string[], map_id = CURRENT_MAP, maxlength=sizeof string);
*
*/
static cell AMX_NATIVE_CALL pawnEntitiesNext(AMX *amx, const cell *params)
{
	cell *cptr;
	EntitySection * section = NULL;

	if ( params[2] == 0 ) /* GLOBAL */
	{
		section = lux::world->GetEntities();
	}
	else if ( params[2] )/* Map */
	{
		if ( lux::world->GetMap((uint32_t)params[2]) )
		{
			section = lux::world->GetMap((uint32_t)params[2])->GetEntities();
		}
	}
	cptr = amx_Address( amx, params[1] );
	std::string string_value = "";
	if ( section != NULL )
	{
		if ( section->iter != section->children.end() )
		{
			string_value.assign( (*section->iter)->id );
			amx_SetString(cptr, string_value.c_str(), 0, 0, params[3]);
			string_value.erase();
			section->iter++;
			return 1;
		}
	}
	amx_SetString(cptr, string_value.c_str(), 0, 0, params[3]);
	return 0;
}



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

