/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_entities.h"
#include "entity_manager.h"
#include <stdint.h>

#include "core.h"
#include "game_system.h"

#define FIXED_MIN 0x7FFFFFFF

/** Entity Position Functions  */

/**
 * @brief Updates Entity X/Y Positions
 * @param wanted
 * @param fixed_x
 * @param fixed_y
 * @param fixed_z
 * @return
 */
uint8_t Lux_FFI_Entity_Object_Set_Position( Entity * wanted, int32_t fixed_x, int32_t fixed_y, int32_t fixed_z_layer )
{
	#ifdef NETWORKENABLED
	lux::core->NetworkLock();
	#endif

	if ( wanted != NULL )
	{
		if ( fixed_x != FIXED_MIN )
			wanted->x = (fixed)fixed_x;
		if ( fixed_y != FIXED_MIN )
			wanted->y = (fixed)fixed_y;
		if ( fixed_z_layer != FIXED_MIN )
			wanted->z_layer = (fixed)fixed_z_layer;
	}
	#ifdef NETWORKENABLED
	lux::core->NetworkUnlock();
	#endif
	return ( wanted != NULL );
}

/**
 * @brief Lux_FFI_Entity_Object_Get_Position
 * @param wanted
 * @param fixed_x
 * @param fixed_y
 * @param fixed_z
 * @return
 */
uint8_t Lux_FFI_Entity_Object_Get_Position( Entity * wanted, int32_t * fixed_x, int32_t * fixed_y, int32_t * fixed_z_layer)
{
	#ifdef NETWORKENABLED
	lux::core->NetworkLock();
	#endif

	if ( wanted != NULL )
	{
		if ( fixed_x )
			*fixed_x = wanted->x;
		if ( fixed_y )
			*fixed_y = wanted->y;
		if ( fixed_z_layer )
			*fixed_z_layer = wanted->z_layer;
	}
	#ifdef NETWORKENABLED
	lux::core->NetworkUnlock();
	#endif
	return ( wanted != NULL );
}

/**
 * @brief Lux_FFI_Entity_Object_Get_Setting
 * @param wanted
 * @param key
 * @return
 */
char * Lux_FFI_Entity_Object_Get_Setting( Entity * wanted, const char * key )
{
	char * string = NULL;

	if ( wanted != NULL )
	{
		std::string str = wanted->GetSetting( key );
		if ( str.length() )
		{
			string = new char[str.length() + 1];
			memset(string, 0, str.length() + 1 );
			std::copy(str.begin(), str.end(), string);
		}
	}
	return string;
}

/**
 * @brief Lux_FFI_Entity_Object_Get_Setting_Number
 * @param wanted
 * @param key
 * @return
 */
int32_t Lux_FFI_Entity_Object_Get_Setting_Number(Entity * wanted, const char * key)
{
	int32_t response = 0;

	if ( wanted != NULL )
	{
		response =  wanted->GetSettingAsInt(key);
	}

	return response;
}

/** Lux_FFI_Entity_Object_Delete
*
* Entity * wanted
*/
uint8_t Lux_FFI_Entity_Object_Delete( Entity * wanted )
{
	if ( wanted != NULL )
	{
		wanted->Delete();
		return 1;
	}
	return 0;
}



/** Entity Position Functions  */

/** Lux_FFI_Entity_Set_Position
*
* Updates Entity X/Y Positions
*/
uint8_t Lux_FFI_Entity_Set_Position(uint32_t hash_entity , int32_t fixed_x, int32_t fixed_y, uint8_t z_layer )
{
	Entity * wanted = lux::entities->GetEntity(hash_entity);

	return Lux_FFI_Entity_Object_Set_Position( wanted, fixed_x, fixed_y, z_layer );
}

/** Lux_FFI_Entity_Get_Position
*
* return Entity X/Y Positions
*/
uint8_t Lux_FFI_Entity_Get_Position(uint32_t hash_entity , int32_t * fixed_x, int32_t * fixed_y, int32_t * fixed_z_layer)
{
	Entity * wanted = lux::entities->GetEntity(hash_entity);

	return Lux_FFI_Entity_Object_Get_Position( wanted, fixed_x, fixed_y, fixed_z_layer );
}

/** Lux_FFI_Entity_Get_Setting
*
* uint32_t hash_entity, const char * key
*/
char * Lux_FFI_Entity_Get_Setting( uint32_t hash_entity, const char * key )
{
	Entity * wanted = lux::entities->GetEntity(hash_entity);

	return Lux_FFI_Entity_Object_Get_Setting( wanted, key );
}

/** Lux_FFI_Entity_Get_Number
*
* uint32_t hash_entity, const char * key
*/
int32_t Lux_FFI_Entity_Get_Setting_Number(uint32_t hash_entity, const char * key)
{
	Entity * wanted = lux::entities->GetEntity(hash_entity);

	return Lux_FFI_Entity_Object_Get_Setting_Number( wanted, key );
}

/** Lux_FFI_Entity_Create
*
* const char * script_file, const char * ident, int32_t x, int32_t y, int32_t z, uint32_t map_id
*/
uint32_t Lux_FFI_Entity_Create( const char * script_file, const char * ident, int32_t x, int32_t y, uint8_t z_layer, uint32_t map_id )
{
	uint32_t response = 0;
	std::string entity_parent, entity_id;
	Entity * wanted_entity = NULL;

	entity_parent = script_file;
	entity_id = ident;

	wanted_entity = lux::entities->NewEntity( ident, script_file, map_id );

	if ( wanted_entity != NULL )
	{
		wanted_entity->x = x;
		wanted_entity->y = y;
		wanted_entity->z_layer = z_layer;

		response = wanted_entity->hashid;
	}

	return response;
}

/** Lux_FFI_Entity_Delete
*
* uint32_t hash_entity
*/
uint8_t Lux_FFI_Entity_Delete( uint32_t hash_entity )
{
	Entity * wanted = lux::entities->GetEntity(hash_entity);

	return Lux_FFI_Entity_Object_Delete( wanted );
}


/** Entity Management Functions  */

/** Lux_FFI_Entities_List
*
* uint32_t map_id
*/
uint32_t Lux_FFI_Entities_List( uint32_t map_id )
{
	uint32_t count = 0;
	EntitySection * section = NULL;

	if ( map_id == 0 ) /* GLOBAL */
	{
		section = lux::game_system->GetEntities();
	}
	else if ( map_id )/* Map */
	{
		if ( lux::game_system->GetMap( map_id ) )
		{
			section = lux::game_system->GetMap( map_id )->GetEntities();
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

/** Lux_FFI_Entities_Next
*
* uint32_t map_id
*/
char * Lux_FFI_Entities_Next( uint32_t map_id, uint8_t * has_next )
{
	char * string = NULL;
	std::string string_value = "";
	EntitySection * section = NULL;

	*has_next = false;

	if ( map_id == 0 ) /* GLOBAL */
	{
		section = lux::game_system->GetEntities();
	}
	else if ( map_id )/* Map */
	{
		if ( lux::game_system->GetMap( map_id ) )
		{
			section = lux::game_system->GetMap( map_id )->GetEntities();
		}
	}


	if ( section != NULL )
	{
		if ( section->iter != section->children.end() )
		{
			string_value.assign( (*section->iter)->id );
			section->iter++;
			*has_next = true;
		}

	}

	if ( string_value.length() )
	{
		string = new char[string_value.length() + 1];
		memset(string, 0, string_value.length() + 1 );
		std::copy(string_value.begin(), string_value.end(), string);
	}



	return string;
}



