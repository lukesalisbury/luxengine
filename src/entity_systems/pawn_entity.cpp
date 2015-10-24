/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#define FFI_ALLOW_ENTITY_OBJECT 1

#include "audio.h"
#include "config.h"
#include "core.h"
#include "display/display.h"
#include "engine.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "game_system.h"
#include "pawn.h"
#include <cmath>
#include <algorithm>
#include "elix/elix_string.hpp"
#include "ffi_entities.h"
#include "ffi_collisions.h"
#include "ffi_path.h"

extern const AMX_NATIVE_INFO Entity_Natives[];


/**
 * @brief pawnFunctionIdent
 * @param amx
 * @param params
 * @return
 */
static cell pawnFunctionIdent(AMX *amx,const cell *params)
{
	char name[64];
	cell *cstr;
	int index, len;
	int32_t err;

	cstr = amx_Address(amx, params[1]);

	/* verify string length */
	amx_StrLen(cstr, &len);
	if ( len >= 64 )
	{
		amx_RaiseError(amx, AMX_ERR_NATIVE);
		return 0;
	}

	amx_GetString( name, cstr, 0, UNLIMITED );
	err = amx_FindPublic(amx,name,&index);
	if ( err != AMX_ERR_NONE )
		index=-1;   /* this is not considered a fatal error */
	return index;
}



/** pawnFunctionCall
* native FunctionCall(function);
*
*/
static cell pawnFunctionCall(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	cell ret = -1;
	if ( params[1] > 0 )
	{
		amx_Exec(amx, &ret, params[1]);
	}
	return ret;
}

/** Entity Position Functions  */
/** pawnEntitySetPosition
* native EntitySetPosition(Fixed:x = Fixed:cellmin, Fixed:y = Fixed:cellmin, Fixed:zlayer = Fixed:cellmin, id = SELF);
* Updates Entity X/Y Positions
*/
static cell pawnEntitySetPosition(AMX *amx, const cell *p)
{
	ASSERT_PAWN_PARAM( amx, p, 4 );

	cell successful = 0;
	Entity * entity = NULL;

	entity = Lux_PawnEntity_GetEntity( amx, p[4] );

	successful = Lux_FFI_Entity_Object_Set_Position( entity, p[1], p[2], p[3] );

	return successful;
}

/** pawnEntityGetPosition
* native EntityGetPosition(&Fixed:x, &Fixed:y, &z, id = SELF, whole = 0 );
* Updates Entity X/Y Positions
*/
static cell pawnEntityGetPosition(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 5 );

	cell successful = 0;
	int32_t fixed_x, fixed_y;
	int32_t fixed_z_layer;
	cell * xptr, * yptr, * zptr;

	Entity * entity = NULL;

	entity = Lux_PawnEntity_GetEntity( amx, params[4] );

	if ( Lux_FFI_Entity_Object_Get_Position( entity, &fixed_x, &fixed_y, &fixed_z_layer ) )
	{
		xptr = amx_Address(amx, params[1]);
		yptr = amx_Address(amx, params[2]);
		zptr = amx_Address(amx, params[3]);
		if ( xptr )
		{
			*xptr = params[5] ? MAKE_FIXED_INT(fixed_x) : fixed_x;
		}
		if ( yptr )
		{
			*yptr = params[5] ? MAKE_FIXED_INT(fixed_y) : fixed_y;
		}

		if ( zptr )
		{
			*zptr = params[5] ? MAKE_FIXED_INT(fixed_z_layer) : fixed_z_layer;
		}
		successful = 1;
	}

	return successful;
}

/** Entity Functions */

/** pawnEntityGetSetting
* native EntityGetSetting(key[], string[], id = SELF, length = sizeof string);
*
*/
static cell pawnEntityGetSetting(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	cell response = -1;
	cell * string_ptr;
	char * string = NULL;
	std::string stf_key = "";

	Entity * entity = NULL;

	entity = Lux_PawnEntity_GetEntity( amx, params[3] );

	stf_key = Lux_PawnEntity_GetString( amx, params[1] );
	string = Lux_FFI_Entity_Object_Get_Setting( entity, stf_key.c_str() );

	if ( string != NULL )
	{
		string_ptr = amx_Address(amx, params[2]);
		if ( string_ptr )
		{
			amx_SetString(string_ptr, string, true, false, params[4]);
			response = strlen( string );
		}

		delete[] string;
	}

	return response;
}

/** pawnEntityGetSettingHash
* native EntityGetSettingHash(key[], id = SELF);
*
*/
static cell pawnEntityGetSettingHash(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	cell response = 0;
	char * string = NULL;
	std::string stf_key = "";

	Entity * entity = NULL;

	entity = Lux_PawnEntity_GetEntity( amx, params[2] );
	stf_key = Lux_PawnEntity_GetString( amx, params[1] );
	string = Lux_FFI_Entity_Object_Get_Setting( entity, stf_key.c_str() );

	if ( string != NULL )
	{
		response = elix::string::Hash(string);

		delete[] string;
	}
	return response;
}


/** pawnEntityGetNumber
* native EntityGetNumber(key[], id = SELF );
*
*/
static cell pawnEntityGetNumber(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	cell response = -1;
	std::string stf_key = "";

	Entity * entity = NULL;

	entity = Lux_PawnEntity_GetEntity( amx, params[2] );
	stf_key = Lux_PawnEntity_GetString( amx, params[1] );
	response = Lux_FFI_Entity_Object_Get_Setting_Number( entity, stf_key.c_str() );

	return response;
}

/** pawnEntityCreate
* native entity:EntityCreate(parententity{}, id_string{}, Fixed:x, Fixed:y, Fixed:zlayer, map_id, args[]=[ARG_END], args_count = sizeof(args) {Fixed,_}:...);
*
*/
static cell pawnEntityCreate(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 8 );

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
		wanted_entity->z_layer = params[5];
		wanted_entity->SetMapCollsion();

		cell * cptr = NULL, * arg_ptr = NULL;
		uint32_t arg_count = 0;
		native_pointer stack_mem = NULL;
		uint32_t num_param = (uint32_t)( params[0]/sizeof(cell) ) - 8;

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
						lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Warning: You can't 'passed by value' when creating a entity." << std::endl;
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
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | EntityCreate ('" << entity_parent;
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "' '" << entity_id << "') Failed." << std::endl;
		response = 0;
	}
	return response;
}

/** pawnEntityDelete
* native EntityDelete(id = SELF);
*
*/
static cell pawnEntityDelete(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );

	if ( wanted_entity != NULL )
	{
		wanted_entity->Delete();
		return 1;
	}
	return 0;

}

/** pawnEntityPublicVariable
* native EntityPublicVariable(id, variable[]);
*
*/
static cell pawnEntityPublicVariable(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

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
static cell pawnEntityPublicVariableSet(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

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
* native EntityPublicFunction(entity:id, function{}, args[]=[ARG_END], args_count = sizeof(args), {Fixed,_}:...);
*
*/
static cell pawnEntityPublicFunction(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );
	if ( wanted_entity != NULL )
	{
		if ( !wanted_entity->_data )
		{
			return -1;
		}
		int f;
		cell results = 0;
		cell * cptr = NULL, * arg_ptr = NULL;
		std::string function_name;
		std::string arg_string;
		uint32_t arg_count = 0;
		uint32_t num_param = (uint32_t)( params[0]/sizeof(cell) ) - 4 ;
		mem_pointer stack_mem = NULL;

		function_name = Lux_PawnEntity_GetString(amx, params[2]);
		arg_string = Lux_PawnEntity_GetString(amx, params[3]);

		/* TODO Move check to Callback */

		if ( amx_FindPublic( (AMX *)wanted_entity->_data, function_name.c_str(), &f) )
		{
			return -2;
		}

		if ( num_param > 0 )
		{
			cptr = amx_Address(amx, params[3]);
			if ( *cptr )
			{
				// Check arg count
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
					arg_ptr = amx_Address(amx, params[num_param+4]);
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
						//lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << num_param << ": " << *cptr  << " = " << *arg_ptr << std::endl;
						int32_t value = (int32_t)*arg_ptr;
						wanted_entity->callbacks->Push(wanted_entity->_data,(int32_t)*arg_ptr);
					}
					cptr--;
					num_param--;
				} while ( num_param > 0 );
			}
		}
		results = wanted_entity->callbacks->Call(wanted_entity->_data, (char*)function_name.c_str(), stack_mem);
		return results;

	}
	return -1;
}

/** Entity Management Functions  */

/** pawnEntitiesList
* native EntitiesList(map_id = CURRENT_MAP);
*
*/
static cell pawnEntitiesList(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint32_t count = 0;
	uint32_t map_id = params[1];

	count = Lux_FFI_Entities_List( map_id );

	return count;
}

/** pawnEntitiesNext
* native EntitiesNext(&entity:id, map_id = 0, entity_name{} = "", maxlength = sizeof(string) );
*
*/
static cell pawnEntitiesNext(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	uint8_t respone = 0;
	cell *cptr, * str;
	char * entity_name = NULL;
	uint32_t map_id = params[2];

	entity_name = Lux_FFI_Entities_Next( map_id, &respone );

	if ( entity_name != NULL )
	{
		respone = 1;
		// Store String
		str = amx_Address( amx, params[3] );
		Lux_PawnEntity_SetString(str, entity_name, (size_t)params[4] );

		// Store hash
		cptr = amx_Address( amx, params[1] );
		if ( cptr )
			*cptr = elix::string::Hash(entity_name);
	}

	return respone;
}



/** Collision Functions */
/** pawnCollisionSet
* native CollisionSet(id, rect = 0, type = 0, x = 0, y = 0, w = 0, h = 0);
*
*/
static cell pawnCollisionSet(AMX *amx, const cell *p)
{
	ASSERT_PAWN_PARAM( amx, p, 7 );

	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, p[1] );

	return Lux_FFI_Entity_Object_Collision_Set( wanted_entity, p[2], p[3], p[4], p[5], p[6], p[7]);
}

/** pawnCollisionGet
* native CollisionGet(id, rect, &x, &y, &w, &h);
*
*/
static cell pawnCollisionGet(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 5 );

	int32_t x, y, w, h;
	cell * valueptr;
	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );

	if ( Lux_FFI_Entity_Object_Collision_Get( wanted_entity, params[2], &x, &y, &w, &h ) )
	{
		valueptr = amx_Address(amx, params[3]);
		if (valueptr)
			*valueptr = x;

		valueptr = amx_Address(amx, params[4]);
		if (valueptr)
			*valueptr = y;

		valueptr = amx_Address(amx, params[5]);
		if (valueptr)
			*valueptr = w;

		valueptr = amx_Address(amx, params[6]);
		if (valueptr)
			*valueptr = h;

		return 1;
	}

	return 0;
}

/** pawnCollisionCheck
* native CollisionCheck(id1, id2, rect1 = -1, rect2 = -1);
*
*/
static cell pawnCollisionCheck(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	uint32_t first_entity, second_entity;

	first_entity = ( params[1] ? params[1] : Lux_PawnEntity_GetEntityHash(amx) );// Get Entity ID
	second_entity = ( params[2] ? params[2] : Lux_PawnEntity_GetEntityHash(amx) );// Get Entity ID

	return Lux_FFI_Collision_Check( first_entity, second_entity, params[3], params[4] );

}

/** pawnCollisionCalculate
* native CollisionCalculate(id = SELF, rect = -1, type = -1);
*
*/
static cell pawnCollisionCalculate(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );

	return Lux_FFI_Entity_Object_Collision_Calculate( wanted_entity );
}

/** pawnCollisionGetCurrent
* native CollisionGetCurrent(id = SELF, &current, &angle, &dist, &rect, &type);
*
*/
static cell pawnCollisionGetCurrent(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 6 );

	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[1] );

	int32_t hit_count = -1;
	uint32_t entity_hit  = 0;
	int32_t angle = 0;
	int32_t dist = 0;
	int32_t rect = 0;
	int32_t type = 0;
	cell * current_ptr = NULL;
	cell * angle_ptr = NULL;
	cell * dist_ptr = NULL;
	cell * rect_ptr = NULL;
	cell * type_ptr = NULL;

	hit_count = Lux_FFI_Entity_Object_Collision_Calculate_Current( wanted_entity, &entity_hit, &angle, &dist, &rect, &type );
	if ( hit_count > 0 )
	{
		current_ptr = amx_Address( amx, params[2] );
		angle_ptr = amx_Address( amx, params[3] );
		dist_ptr = amx_Address( amx, params[4] );
		rect_ptr = amx_Address( amx, params[5] );
		type_ptr = amx_Address( amx, params[6] );

		if ( current_ptr )
			*current_ptr = entity_hit;

		if ( angle_ptr )
			*angle_ptr = angle;

		if ( dist_ptr )
			*dist_ptr = dist;

		if ( rect_ptr )
			*rect_ptr = rect;

		if ( type_ptr )
			*type_ptr = type;

	}
	return hit_count;
}

/** pawnCollisionFromObject
* native CollisionFromObject( obj, type = 0, id = SELF);
*
*/
static cell pawnCollisionFromObject(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	Entity * wanted_entity = Lux_PawnEntity_GetEntity( amx, params[3] );

	return Lux_FFI_Entity_Object_Collision_Set_From_Object( wanted_entity, params[1], params[2] );
}

/* Paths */

/** pawnPathCount
* native PathCount( object:id );
*
*/
static cell pawnPathCount(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	return Lux_FFI_Path_Count( params[1] );
}

/** pawnPathGetPoint
* native PathGetPoint( object:id, point, &x, &y, &t);
*
*/
static cell pawnPathGetPoint(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 5 );

	uint8_t point = params[2];
	int16_t x = 0;
	int16_t y = 0;
	uint32_t ms_length = 0;


	if ( Lux_FFI_Path_Point( params[1], point, &x, &y, &ms_length ) )
	{
		cell * value_ptr = NULL;

		value_ptr = amx_Address(amx, params[3]);
		if (value_ptr)
			*value_ptr = x;

		value_ptr = amx_Address(amx, params[4]);
		if (value_ptr)
			*value_ptr = y;

		value_ptr = amx_Address(amx, params[5]);
		if (value_ptr)
			*value_ptr = ms_length;


		return 1;

	}

	return 0;
}

/** pawnPathMoveObject
* native PathMoveObject(object:id, Fixed:speed, &x, &y, loop);
*/
static cell pawnPathMoveObject(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 5 );

	uint32_t object_id = (uint32_t)params[1];
	int32_t fixed_speed = params[2];
	int16_t x = 0;
	int16_t y = 0;
	uint8_t loop = params[5];

	int32_t result = Lux_FFI_Path_Move_Object( object_id, fixed_speed, &x, &y, loop );

	if ( result >= 0 )
	{
		cell * xptr = NULL;

		xptr = amx_Address(amx, params[3]);
		if (xptr)
			*xptr = x;

		xptr = amx_Address(amx, params[4]);
		if (xptr)
			*xptr = y;
	}

	return result;

}



const AMX_NATIVE_INFO Entity_Natives[] = {
	{ "FunctionCall", pawnFunctionCall},
	{ "FunctionIdent", pawnFunctionIdent },
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
	{ "PathGetPoint", pawnPathGetPoint},
	{ "PathCreate", pawnDeprecatedFunction},
	{ "PathAddPoint", pawnDeprecatedFunction},
	{ "PathRemovePoint", pawnDeprecatedFunction},
	{ "PathMoveObject", pawnPathMoveObject }, ///native PathMoveObject(object:id, speed,&x, &y);

	{ 0, 0 }
};
