/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "config.h"
#include "core.h"
#include "display.h"
#include "engine.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "world.h"
#include "pawn.h"
#include <cmath>
#include <algorithm>
#include "elix_string.hpp"


extern const AMX_NATIVE_INFO Entity_Natives[];


/** pawnFunctionCall
* native FunctionCall(function);
*
*/
static cell AMX_NATIVE_CALL pawnFunctionCall(AMX *amx, const cell *params)
{
	cell ret = -1;
	if ( params[1] > 0 )
	{
		amx_Exec(amx, &ret, params[1]);
	}
	return ret;
}

/** Entity Position Functions  */
/** pawnEntitySetPosition
* native EntitySetPosition(Fixed:x = Fixed:cellmin, Fixed:y = Fixed:cellmin, Fixed:z = Fixed:cellmin, id = SELF);
* Updates Entity X/Y Positions
*/
static cell AMX_NATIVE_CALL pawnEntitySetPosition(AMX *amx, const cell *params)
{
	cell successful = 0;
#ifdef NETWORKENABLED
	lux::core->NetLock();
#endif
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[4] );
	if ( wanted != NULL )
	{
		if ( params[1] != CELLMIN )
			wanted->x = (fixed)params[1];
		if ( params[2] != CELLMIN )
			wanted->y = (fixed)params[2];
		if ( params[3] != CELLMIN )
			wanted->z = (fixed)params[3];

		successful =  1;
	}
#ifdef NETWORKENABLED
	lux::core->NetUnlock();
#endif
	return successful;
}

/** pawnEntityGetPosition
* native EntityGetPosition(&Fixed:x, &Fixed:y, &Fixed:z, id = SELF );
* Updates Entity X/Y Positions
*/
static cell AMX_NATIVE_CALL pawnEntityGetPosition(AMX *amx, const cell *params)
{
#ifdef NETWORKENABLED
	lux::core->NetLock();
#endif
	cell successful = 0;
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[4]);
	if ( wanted != NULL )
	{
		cell * xptr, * yptr, * zptr;
		xptr = amx_Address(amx, params[1]);
		yptr = amx_Address(amx, params[2]);
		zptr = amx_Address(amx, params[3]);
		if ( xptr )
			*xptr = wanted->x;
		if ( yptr )
			*yptr = wanted->y;
		if ( zptr )
			*zptr = wanted->z;
		successful = 1;
	}
#ifdef NETWORKENABLED
	lux::core->NetUnlock();
#endif
	return successful;
}

/** Entity Functions */

/** pawnEntityGetSetting
* native EntityGetSetting(key[], string[], id = SELF);
*
*/
static cell AMX_NATIVE_CALL pawnEntityGetSetting(AMX *amx, const cell *params)
{
	cell response = -1;
	cell * stringptr;
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[3]);
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
			amx_SetString(stringptr, str.c_str(), true, false, UNLIMITED);
			response = str.length();
			str.clear();

		}
	}
	return response;
}

/** pawnEntityGetSettingHash
* native EntityGetSettingHash(key[], id = SELF);
*
*/
static cell AMX_NATIVE_CALL pawnEntityGetSettingHash(AMX *amx, const cell *params)
{
	cell response = 0;
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[2]);
	if ( wanted != NULL )
	{
		std::string stf_key = Lux_PawnEntity_GetString(amx, params[1] );

		if ( !stf_key.empty() )
		{
			std::string str;
			str = wanted->GetSetting(stf_key);
			response = elix::string::Hash(str);
			str.clear();
		}

	}
	return response;
}


/** pawnEntityGetNumber
* native EntityGetNumber(key[], id = SELF );
*
*/
static cell AMX_NATIVE_CALL pawnEntityGetNumber(AMX *amx, const cell *params)
{
	cell response = -1;
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[2]);
	if ( wanted != NULL )
	{
		std::string stf_key = Lux_PawnEntity_GetString( amx, params[1] );
		if ( !stf_key.empty() )
		{
			response =  wanted->GetSettingAsInt(stf_key);
		}
	}
	return response;
}

/** pawnEntityCreate
* native EntityCreate(parententity[],id[],x,y,z,map_id,args[],...);
*
*/
static cell AMX_NATIVE_CALL pawnEntityCreate(AMX *amx, const cell *params)
{
	cell response = 0;
	std::string entity_parent, entity_id;
	Entity * wanted_entity = NULL;

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
		wanted_entity->SetMapCollsion();

		cell * cptr = NULL, * arg_ptr = NULL;
		uint32_t arg_count = 0;
		native_pointer stack_mem = NULL;
		uint32_t num_param = (uint32_t)( params[0]/sizeof(cell) ) - 7;
		if ( num_param > 0 )
		{

			cptr = amx_Address(amx, params[7]);
			if ( *cptr )
			{
				while (*cptr)
				{
					arg_count++;
					cptr++;
				}
				cptr--; //move pointer back one.

				if ( num_param != arg_count )
				{
					arg_count = std::min(arg_count, num_param);
					num_param = arg_count;
				}


				do
				{
					arg_ptr = amx_Address(amx, params[num_param+7]);
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
						std::cerr << "Warning: You can't 'passed by value' when creating a entity." << std::endl;
					}
					cptr--;
					num_param--;
				} while ( num_param > 0 );
			}

			wanted_entity->callbacks->Call(wanted_entity->_data, (char*)"Init", stack_mem);
			wanted_entity->firstran = true;
		}
		else
		{
			wanted_entity->Init();
		}

		response = wanted_entity->hashid;
	}
	else
	{
		std::cerr << __FILE__ << ":" << __LINE__ << " | EntityCreate ('" << entity_parent;
		std::cerr << "' '" << entity_id << "') Failed." << std::endl;
		response = 0;
	}
	return response;
}

/** pawnEntityDelete
* native EntityDelete(id = SELF);
*
*/
static cell AMX_NATIVE_CALL pawnEntityDelete(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1]);
	if ( wanted_entity != NULL )
	{
		wanted_entity->Delete();
		amx->error=AMX_ERR_SLEEP;
		return 1;
	}
	return 0;
}

/** pawnEntityPublicVariable
* native EntityPublicVariable(id, variable[]);
*
*/
static cell AMX_NATIVE_CALL pawnEntityPublicVariable(AMX *amx, const cell *params)
{
	cell ret = 0;
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );
	if ( wanted_entity != NULL )
	{
		std::string varname = Lux_PawnEntity_GetString(amx, params[2]);
		ret = (cell)Lux_PawnEntity_PublicVariable( (AMX *)wanted_entity->_data, varname, NULL );
	}
	return ret;
}

/** pawnEntityPublicVariableSet
* native EntityPublicVariableSet(id, variable[], value);
*
*/
static cell AMX_NATIVE_CALL pawnEntityPublicVariableSet(AMX *amx, const cell *params)
{
	cell ret = 0;
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1]);
	if ( wanted_entity != NULL )
	{
		int32_t value = (int32_t)params[3];
		std::string varname = Lux_PawnEntity_GetString(amx, params[2]);
		ret = (cell)Lux_PawnEntity_PublicVariable( (AMX *)wanted_entity->_data, varname, NULL );

	}
	return ret;
}

/** pawnEntityPublicFunction
* native EntityPublicFunction(id, function[], args[]="", ...);
*
*/
static cell AMX_NATIVE_CALL pawnEntityPublicFunction(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );
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
			cptr--; //move pointer back one.

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
					//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << num_param << ": " << *cptr  << " = " << *arg_ptr << std::endl;
					int32_t value =  (int32_t)*arg_ptr;
					wanted_entity->callbacks->Push(wanted_entity->_data,(int32_t)*arg_ptr);
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
* native EntitiesNext(&id, map_id = 0, string{}="", maxlength = sizeof(string));
*
*/
static cell AMX_NATIVE_CALL pawnEntitiesNext(AMX *amx, const cell *params)
{
	cell respone = 0;
	cell *cptr, * str;
	EntitySection * section = NULL;
	std::string string_value = "";

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
	str = amx_Address( amx, params[3] );

	if ( section != NULL )
	{
		if ( section->iter != section->children.end() )
		{
			string_value.assign( (*section->iter)->id );
			if ( cptr )
				*cptr = (*section->iter)->hashid;

			section->iter++;
			respone = 1;
		}
	}

	amx_SetString(str, string_value.c_str(), 0, 0, params[4]);
	string_value.erase();

	return respone;
}



/** Collision Functions */
/** pawnCollisionSet
* native CollisionSet(id, rect = 0, type = 0, x = 0, y = 0, w = 0, h = 0);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionSet(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );
	if ( wanted_entity != NULL )
	{
		return wanted_entity->AddCollision(params[2], params[4], params[5], params[6], params[7], params[3]);
	}
	return -1;
}

/** pawnCollisionGet
* native CollisionGet(id, rect, &x, &y, &w, &h);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionGet(AMX *amx, const cell *params)
{
	cell * valueptr = NULL;
	if ( params[2] >= 0 && params[2] < 7 )
	{
		Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1]);
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
* native CollisionCheck(id1, id2, rect1 = -1, rect2 = -1, length1=sizeof id1, length2=sizeof id2);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionCheck(AMX *amx, const cell *params)
{
	Entity * first_entity, * second_entity;
	first_entity = Lux_PawnEntity_GetEntity( amx, params[1]);
	if ( first_entity != NULL )
	{
		second_entity = Lux_PawnEntity_GetEntity( amx, params[2]);
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
* native CollisionCalculate(id = SELF, rect = -1, type = -1, length=sizeof id);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionCalculate(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1]);
	if ( wanted_entity )
	{
		return wanted_entity->GetHits();
	}
	return -1;
}

/** pawnCollisionGetCurrent
* native CollisionGetCurrent(id = SELF, &current, &angle, &dist, &rect, &type);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionGetCurrent(AMX *amx, const cell *params)
{
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1]);
	if ( wanted_entity )
	{
		cell * current_ptr = NULL;
		cell * angle_ptr = NULL;
		cell * dist_ptr = NULL;
		cell * rect_ptr = NULL;
		cell * type_ptr = NULL;
		CollisionResult * collision_value = NULL;

		uint32_t hit_count = wanted_entity->GetHitsCount();

		if ( hit_count )
		{
			collision_value = wanted_entity->GetCurrentHit();

			if ( collision_value )
			{
				current_ptr = amx_Address( amx, params[2] );
				angle_ptr = amx_Address( amx, params[3] );
				dist_ptr = amx_Address( amx, params[4] );
				rect_ptr = amx_Address( amx, params[5] );
				type_ptr = amx_Address( amx, params[6] );

				if ( current_ptr )
				{
					*current_ptr = collision_value->entity_id;
				}

				int16_t x_dist = collision_value->rect_x - collision_value->hit_x;
				int16_t y_dist = collision_value->rect_y - collision_value->hit_y;

				if ( angle_ptr )
				{
					*angle_ptr = (cell)(atan2((float)y_dist, (float)x_dist) * 57.29578);
					*angle_ptr += 90;
				}

				if ( dist_ptr )
				{
					x_dist *= x_dist;
					y_dist *= y_dist;
					*dist_ptr = (cell)sqrt( (float)(x_dist + y_dist));
				}

				if ( rect_ptr )
					*rect_ptr = collision_value->hit_rect;

				if ( type_ptr )
					*type_ptr = collision_value->type;
			}
		}
		return hit_count;
	}
	return -1;
}

/** pawnCollisionFromObject
* native CollisionFromObject( obj, type = 0, id = SELF);
*
*/
static cell AMX_NATIVE_CALL pawnCollisionFromObject(AMX *amx, const cell *params)
{
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[3]);
	if ( wanted != NULL )
	{
		if ( lux::world->active_map )
		{
			MapObject * object = NULL;
			if ( wanted->_mapid == 0 )
				object = lux::world->GetObject(params[1]);
			else
				object = lux::world->active_map->GetObject(params[1]);

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
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[3]);
	if ( wanted != NULL )
	{
		if ( lux::world->active_map )
		{
			MapObject * object = NULL;
			if ( wanted->_mapid == 0 )
				object = lux::world->GetObject(params[1]);
			else
				object = lux::world->active_map->GetObject(params[1]);

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
	Entity * wanted = Lux_PawnEntity_GetEntity( amx, params[3]);
	if ( wanted != NULL )
	{
		if ( lux::world->active_map )
		{
			MapObject * object = NULL;
			if ( wanted->_mapid == 0 )
				object = lux::world->GetObject(params[1]);
			else
				object = lux::world->active_map->GetObject(params[1]);

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





const AMX_NATIVE_INFO Entity_Natives[] = {
	{ "FunctionCall", pawnFunctionCall},
	/** Collision Functions */
	{ "CollisionCheck", pawnCollisionCheck},
	{ "CollisionSet", pawnCollisionSet},
	{ "CollisionGet", pawnCollisionGet},
	{ "CollisionCalculate", pawnCollisionCalculate},
	{ "CollisionGetCurrent", pawnCollisionGetCurrent },
	{ "CollisionFromObject", pawnCollisionFromObject },
	/** Position Functions */
	{ "EntitySetPosition", pawnEntitySetPosition},
	{ "EntityGetPosition", pawnEntityGetPosition},
	{ "EntityGetSetting", pawnEntityGetSetting},
	{ "EntityGetSettingHash", pawnEntityGetSettingHash},
	{ "EntityGetNumber", pawnEntityGetNumber},
	/** Entity Management Functions  */
	{ "EntitiesList", pawnEntitiesList},
	{ "EntitiesNext", pawnEntitiesNext},
	{ "EntityPublicFunction", pawnEntityPublicFunction},
	{ "EntityPublicVariable", pawnEntityPublicVariable},
	{ "EntityPublicVariableSet", pawnEntityPublicVariableSet},
	{ "EntityCreate", pawnEntityCreate},
	{ "EntityDelete", pawnEntityDelete},
	/** Path Functions  */
	{ "PathCount", pawnPathCount},
	{ "PathPoints", pawnPathPoints},
	{ "PathCreate", pawnDeprecatedFunction},
	{ "PathAddPoint", pawnDeprecatedFunction},
	{ "PathRemovePoint", pawnDeprecatedFunction},
	{ 0, 0 }
};
