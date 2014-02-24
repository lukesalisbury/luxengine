/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_entities.h"
#include <stdint.h>

/** Entity Position Functions  */

/** Lux_FFI_Entity_Set_Position
*
* Updates Entity X/Y Positions
*/
void Lux_FFI_Entity_Set_Position( int32_t fixed_x, int32_t fixed_y, int32_t fixed_z, int32_t hash_entity )
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

/** Lux_FFI_Entity_Get_Position
*
* return Entity X/Y Positions
*/
void Lux_FFI_Entity_Get_Position( int32_t * fixed_x, int32_t * fixed_y, int32_t * fixed_z, int32_t hash_entity )
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
char * Lux_FFI_Entity_Get_Setting( uint32_t hash_entity, const char * key )
{
	char * string = NULL;

	Entity * wanted = lux::entities->GetEntity(hash_entity);

	if ( wanted != NULL )
	{
		std::string str = wanted->GetSetting( key );
		if ( str.length() )
		{
			string = new char[str.length() + 1];
			std::copy(str.begin(), str.end(), string);
		}

	}
	return string;
}

/** pawnEntityGetNumber
* native EntityGetNumber(key[], id[] = SELF, length=sizeof id );
*
*/
int32_t Lux_FFI_Entity_Get_Number(uint32_t hash_entity, const char * key)
{
	int32_t response = 0;
	Entity * wanted = lux::entities->GetEntity(hash_entity);

	if ( wanted != NULL )
	{
		response =  wanted->GetSettingAsInt(key);
	}

	return response;
}

/** pawnEntityCreate
* native EntityCreate(parententity[], id[], x, y, z, map_id = CURRENT_MAP, idlength = sizeof(id), arg[], ...);
*
*/
uint32_t Lux_FFI_Entity_Create( const char * script_file, const char * ident, int32_t x, int32_t y, int32_t z, uint32_t map_id )
{
	cell response = 0;
	std::string entity_parent, entity_id;
	uint32_t id_length = 0;
	Entity * wanted_entity = NULL;
	cell * id_ptr;

	entity_parent = script_file;
	entity_id = ident;

	wanted_entity = lux::entities->NewEntity(ident, script_file, map_id );


	if ( wanted_entity != NULL )
	{
		wanted_entity->x = x;
		wanted_entity->y = y;
		wanted_entity->z = z;

		response = wanted_entity->id;
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
