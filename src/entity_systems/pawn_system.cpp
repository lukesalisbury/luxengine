/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "game_config.h"
#include "core.h"
#include "display.h"
#include "engine.h"
#include "mokoi_game.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "world.h"
#include "elix_string.hpp"
#include "save_system.h"


extern const AMX_NATIVE_INFO System_Natives[];

/** System Functions */


/** pawnSystemAudioVolume
* native SystemAudioVolume(_AUDIO:device, level = -1);
*
*/
static cell AMX_NATIVE_CALL pawnSystemAudioVolume(AMX *amx, const cell *params)
{
	if (params[1] == 2)
		return (cell) lux::audio->SetMusicVolume(params[2]);
	else if (params[1] == 0)
		return (cell) lux::audio->SetEffectsVolume(params[2]);
	else
		return -1;
}

/** Config Functions */

/** pawnConfigGetString
* native ConfigGetString(config[], value[], maxlength=sizeof  value);
*
*/
static cell AMX_NATIVE_CALL pawnConfigGetString(AMX *amx, const cell *params)
{
	std::string value_string;
	char * config_string;
	cell * cptr;

	amx_StrParam_Type( amx, params[1], config_string, char*);
	value_string = lux::config->GetString(config_string, false);
	cptr = amx_Address(amx, params[2]);
	amx_SetString(cptr, value_string.c_str(), 1, 0, 5);
	return (cell) 1;
}

/** pawnConfigGetNumber
* native ConfigGetNumber(config[]);
*
*/
static cell AMX_NATIVE_CALL pawnConfigGetNumber(AMX *amx, const cell *params)
{
	char * config_string;
	amx_StrParam_Type( amx, params[1], config_string, char*);
	return (cell) lux::config->GetNumber(config_string);
}

/** pawnConfigSetString
* native ConfigSetString(config[], value[]);
*
*/
static cell AMX_NATIVE_CALL pawnConfigSetString(AMX *amx, const cell *params)
{
	char * value_string, * config_string;
	amx_StrParam_Type( amx, params[2], value_string, char*);
	amx_StrParam_Type( amx, params[1], config_string, char*);
	lux::config->SetString(config_string, value_string);
	return 0;
}

/** pawnConfigSave
* native ConfigSave();
*
*/
static cell AMX_NATIVE_CALL pawnConfigSave(AMX *amx, const cell *params)
{
	//return lux::config->SaveGame();
	return 0;
}


/* File System */

/** pawnFileGetList
* native FileGetList(strings[][], directory[] = "map", size=sizeof strings);
*
*/
static cell AMX_NATIVE_CALL pawnFileGetList(AMX *amx, const cell *params)
{
	cell * cptr;
	std::vector<std::string> results;
	std::string folder = Lux_PawnEntity_GetString(amx, params[2]);
	if ( !folder.length() )
	{
		return 0;
	}

	if ( lux::game->FolderList(folder, &results) )
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

	/** Config Functions */
	{ "ConfigSetString", pawnConfigSetString}, ///native ConfigSetString(config[], value[], maxlength=sizeof value);
	{ "ConfigGetNumber", pawnConfigGetNumber}, ///native ConfigGetNumber(config[]);
	{ "ConfigGetString", pawnConfigGetString}, ///native ConfigGetString(config[], value[]);
	{ "ConfigSave", pawnConfigSave}, ///native ConfigSave();

	/** File Listing Functions */
	{ "FileGetList", pawnFileGetList}, ///native FileGetList(strings[][], directory[] = "map", size=sizeof strings);

	{ 0, 0 }
};
