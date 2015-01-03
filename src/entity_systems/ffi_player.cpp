/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#include "ffi_player.h"
#include "ffi_functions.h"
#include "engine.h"
#include "game_config.h"
#include "game_system.h"

#include "elix_endian.hpp"
#include "entity_manager.h"


/** Lux_FFI_Player_Set_Entity
*
* uint32_t player_number, uint32_t hash_entity
*/
uint8_t Lux_FFI_Entity_Object_Player_Set_Entity( uint32_t player_number, Entity * wanted )
{
	Player * player = NULL;

	player = lux::engine->GetPlayer( player_number );

	if ( wanted != NULL )
	{
		if ( player != NULL )
		{
			player->SetEntity(wanted);
			return 1;
		}
	}
	return 0;
}



/** Lux_FFI_Player_Set_Controller
*
*
*/
uint8_t Lux_FFI_Player_Set_Controller( uint32_t player_number, const char * controller_name )
{
	Player * player = lux::engine->GetPlayer( player_number );
	if ( player )
	{
		player->SetControls( controller_name );
		return 1;
	}
	return 0;

}


/** Lux_FFI_Player_Get_Controller
*
*
*/
char * Lux_FFI_Player_Get_Controller( uint32_t player_number )
{
	char * string = NULL;
	std::string str;
	Player * player = lux::engine->GetPlayer( player_number );
	if ( player )
	{
		str = player->GetControllerName( );

		if ( str.length() )
		{
			string = new char[str.length() + 1];
			memset(string, 0, str.length() + 1 );
			std::copy(str.begin(), str.end(), string);
		}

	}
	return string;

}


/** Lux_FFI_Player_Set_Colour
* native PlayerSetColour(player, colour);
*
*/
uint8_t Lux_FFI_Player_Set_Colour( uint32_t player_number, uint32_t new_colour )
{
	Player * player = lux::engine->GetPlayer( player_number );
	if ( player )
	{
		cell_colour colour;
		colour.hex = elix::endian::host32( new_colour );
		player->PlayerColour.primary_colour = colour.rgba;
		return 1;
	}
	return 0;

}

/** Lux_FFI_Player_Set_Entity
*
* uint32_t player_number, uint32_t hash_entity
*/
uint8_t Lux_FFI_Player_Set_Entity( uint32_t player_number, uint32_t hash_entity )
{
	Player * player = NULL;
	Entity * wanted = NULL;

	wanted = lux::entities->GetEntity( hash_entity );
	player = lux::engine->GetPlayer( player_number );

	if ( wanted != NULL )
	{
		if ( player != NULL )
		{
			player->SetEntity(wanted);
			return 1;
		}
	}
	return 0;
}

/** pawnPlayerSetName
*
*
*/
uint8_t Lux_FFI_Player_Set_Name( uint32_t player_number, const char * player_name )
{
	Player * player = NULL;

	player = lux::engine->GetPlayer( player_number );
	if ( player_name )
	{
		player->SetName( player_name);
		return 1;
	}
	return 0;
}

/** Lux_FFI_Player_Get_Name
*
*
*/
char * Lux_FFI_Player_Get_Name( uint32_t player_number )
{
	char * string = NULL;
	std::string str;
	Player * player = lux::engine->GetPlayer( player_number );
	if ( player )
	{
		str = player->GetName( );

		if ( str.length() )
		{
			string = new char[str.length() + 1];
			memset(string, 0, str.length() + 1 );
			std::copy(str.begin(), str.end(), string);
		}

	}

	return string;
}

/** Lux_FFI_Controller_Config
*
*
*/
char * Lux_FFI_Controller_Config( const char * controller_name )
{
	char * string = NULL;
	std::string str;
	std::string config_str = "controller.";


	config_str.append( controller_name );

	str = lux::config->GetString(config_str);

	if ( str.length() )
	{
		string = new char[str.length() + 1];
		memset(string, 0, str.length() + 1 );
		std::copy(str.begin(), str.end(), string);
	}

	return string;
}
