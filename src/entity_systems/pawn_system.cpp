/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "audio.h"
#include "game_config.h"
#include "core.h"
#include "display/display.h"
#include "engine.h"
#include "mokoi_game.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "game_system.h"
#include "elix/elix_string.hpp"
#include "save_system.h"
#include "ffi_system.h"

extern const AMX_NATIVE_INFO System_Natives[];

/** System Functions */

/**
 * @brief pawnSystemAudioVolume
 * @param amx
 * @param params
 * @return
 */
static cell pawnSystemAudioVolume(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	uint8_t device = (uint8_t)params[1];
	int8_t level = (int8_t)params[2];

	return Lux_FFI_System_Audio_Volume(device, level);
}

/**
 * @brief pawnSystemCommand
 * @param amx
 * @param params
 * @return
 */
static cell pawnSystemCommand(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	uint8_t command = (uint8_t)params[1];
	int32_t value = (int8_t)params[2];

	return Lux_FFI_System_Command( command, value );
}

/**
 * @brief pawnSystemListCount
 * @param amx
 * @param params
 * @return
 */
static cell pawnSystemListCount(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint8_t list = (uint8_t)params[1];

	return Lux_FFI_System_List_Count( list );
}

/**
 * @brief pawnSystemListItem
 * @param amx
 * @param params
 * @return
 */
static cell pawnSystemListItem(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	uint8_t list = (uint8_t)params[1];

	return Lux_FFI_System_List_Count( list );
}


/** Config Functions */

/**
 * @brief pawnConfigGetString
 * @param amx
 * @param params
 * @return
 */
static cell pawnConfigGetString(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	std::string value_string;
	std::string config_string;
	cell * cptr;

	config_string = Lux_PawnEntity_GetString( amx, params[1] );
	value_string = lux::config->GetString(config_string, false);

	cptr = amx_Address(amx, params[2]);

	return Lux_PawnEntity_SetString(cptr, value_string.c_str(), params[3] );

}

/**
 * @brief pawnConfigGetNumber
 * @param amx
 * @param params
 * @return
 */
static cell pawnConfigGetNumber(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	std::string config_string = Lux_PawnEntity_GetString( amx, params[1] );
	return (cell) lux::config->GetNumber( config_string );
}

/**
 * @brief pawnConfigSetString
 * @param amx
 * @param params
 * @return
 */
static cell pawnConfigSetString(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	std::string config_string = Lux_PawnEntity_GetString( amx, params[1] );
	std::string value_string = Lux_PawnEntity_GetString( amx, params[2] );

	lux::config->SetString(config_string, value_string);
	return 0;
}

/**
 * @brief pawnConfigSave
 * @param amx
 * @param params
 * @return
 */
static cell pawnConfigSave(AMX *amx, const cell *params)
{
	return Lux_FFI_Config_Save();
}


/* File System */

/** pawnFileGetList
* native FileGetList(strings[][], directory[] = "map", size=sizeof strings);
*
*/
static cell pawnFileGetList(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	cell * cptr;
	std::vector<std::string> results;
	std::string folder = Lux_PawnEntity_GetString(amx, params[2]);

	if ( !folder.length() )
	{
		return 0;
	}

	if ( lux::game_data->FolderList(folder, &results) )
	{
		if ( results.size() )
		{
			cptr = amx_Address(amx, params[1]);
			uint32_t length = (cptr[1] - cptr[0]);
			for ( uint32_t i = 0; i < results.size(); i++ )
			{
				if ( i < (uint32_t)params[3] )
				{
					uint32_t m = i + (cptr[i]/sizeof(cell));
					amx_SetString(&cptr[m], results[i].c_str(), 1, 0, length);
				}
			}
		}
	}
	results.clear();
	return 0;
}


const AMX_NATIVE_INFO System_Natives[] = {
	/** Misc Functions */
	{ "SystemAudioVolume", pawnSystemAudioVolume}, /// native SystemAudioVolume(_AUDIO:device, level = -1);
	{ "SystemCommand", pawnSystemCommand}, /// native SystemCommand( command, value = 0 );
	{ "SystemListCount", pawnSystemListCount}, /// native SystemListCount( list );
	{ "SystemListItem", pawnSystemListItem}, /// native SystemListItem( list, item, content{},  maxlength=sizeof content );

	/** Config Functions */
	{ "ConfigSetString", pawnConfigSetString}, ///native ConfigSetString(key{}, value{}, maxlength=sizeof value);
	{ "ConfigGetNumber", pawnConfigGetNumber}, ///native ConfigGetNumber(key{});
	{ "ConfigGetString", pawnConfigGetString}, ///native ConfigGetString(key{}, value{});
	{ "ConfigSave", pawnConfigSave}, ///native ConfigSave();

	/** File Listing Functions */
	{ "FileGetList", pawnFileGetList}, ///native FileGetList(strings[][], directory[] = "map", size=sizeof strings);

	{ 0, 0 }
};
