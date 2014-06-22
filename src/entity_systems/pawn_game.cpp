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


extern const AMX_NATIVE_INFO Game_Natives[];


/** Game Functions */

/** pawnGameReload
* native GameReload();
*
*/
static cell AMX_NATIVE_CALL pawnGameReload(AMX *amx, const cell *params)
{
	lux::engine->SetState(RESETGAME);
	return 1;
}

/** pawnGameState
* native GameState(newstate = -1);
* Get/Set Global State.
*/
static cell AMX_NATIVE_CALL pawnGameState(AMX *amx, const cell *params)
{
	return (cell)lux::engine->GameState((int32_t) params[1]);
}

/** pawnGameFrame
* native GameFrame();
* Return the length of last frame in milliseconds.
*/
static cell AMX_NATIVE_CALL pawnGameFrame(AMX *amx, const cell *params)
{
	uint32_t time = lux::core->GetFrameDelta();
	return (cell)time;
}

/** pawnGameSave
* native GameSave(slot, detail[64],  maxlength=sizeof detail );
* Save Current Game.
*/
static cell AMX_NATIVE_CALL pawnGameSave(AMX *amx, const cell *params)
{
	if ( params[1] > 254 || params[1] < 0)
		return 0;

	uint8_t requested_save_slot = (uint8_t)params[1];
	int32_t * cookie_data = new int32_t[64];

	cell * cptr = amx_Address(amx, params[2]);
	for ( uint8_t count = 0; count < 64; count++ )
	{
		cookie_data[count] = (int32_t)*cptr;
		cptr++;
	}

	return lux::engine->WriteSaveGame( requested_save_slot, cookie_data, 64 );

}


/** pawnGameSaveHibernate
* native GameSaveHibernate();
* Save Current Game.
*/
static cell AMX_NATIVE_CALL pawnGameSaveHibernate(AMX *amx, const cell *params)
{
	return lux::engine->WriteSaveGame( 0x00, NULL, 0 );
}

/** pawnGameHasSave
* native GameHasSave();
* Save Current Game.
*/
static cell AMX_NATIVE_CALL pawnGameHasSave(AMX *amx, const cell *params)
{
	LuxSaveState save_check;
	uint8_t requested_save_slot = (uint8_t)params[1];

	return save_check.Exists(requested_save_slot);
}


/** pawnGameRemoveSave
* native GameRemoveSave();
* Save Current Game.
*/
static cell AMX_NATIVE_CALL pawnGameRemoveSave(AMX *amx, const cell *params)
{
	return false;
}


/** pawnGameLoad
* native GameLoad(slot);
* Load Game from Slot.
*/
static cell AMX_NATIVE_CALL pawnGameLoad(AMX *amx, const cell *params)
{
	if ( params[1] > 254 || params[1] < 0)
		return 0;

	uint8_t requested_save_slot = (uint8_t)params[1];

	return lux::engine->RestoreSaveGame( requested_save_slot );
}

/** pawnGameGetDetails
* native GameGetDetails(slot, array[64], bool:pack_array = false, maxlength=sizeof array);
* Get Save Game Details.
*/
static cell AMX_NATIVE_CALL pawnGameGetDetails(AMX *amx, const cell *params)
{


	if ( params[1] > 254 || params[1] < 0)
		return 0;

	if ( params[4] != 64 )
		return 0;

	cell read_successful = false;
	int32_t * cookie_data = new int32_t[64];
	uint8_t requested_save_slot = (uint8_t)params[1];

	cell * cptr = amx_Address(amx, params[2]);

	if ( lux::engine )
	{
		if ( lux::engine->ReadSaveInfo( requested_save_slot, cookie_data, 64, lux::game->ident ) )
		{
			for (uint8_t count = 0; count < 64; count++)
			{
				*cptr = cookie_data[count];
				cptr ++;
			}
			read_successful = true;
		}
	}
	delete[] cookie_data;
	return read_successful;
}

/** pawnGameDetails
* native GameDetails(id, slot, array[64], bool:pack_array = false, maxlength=sizeof array);
* Get another games Save Game Details.
*/
static cell AMX_NATIVE_CALL pawnGameDetails(AMX *amx, const cell *params)
{
	if ( params[2] > 254 || params[2] < 0)
		return 0;

	if ( params[5] != 64 )
		return 0;

	cell read_successful = false;
	uint32_t game_id = (uint32_t)params[1];
	int32_t * cookie_data = new int32_t[64];
	uint8_t requested_save_slot = (uint8_t)params[2];

	cell * cptr = amx_Address(amx, params[3]);

	if ( lux::engine->ReadSaveInfo( requested_save_slot, cookie_data, 64, game_id) )
	{
		for (uint8_t count = 0; count < 64; count++)
		{
			*cptr = cookie_data[count];
			cptr += sizeof(cell);
		}
		read_successful = true;
	}


	delete[] cookie_data;
	return read_successful;
}

/** Input Functions */

/** pawnKeyboardWatch
* native KeyboardWatch( able );
*
*/
static cell AMX_NATIVE_CALL pawnKeyboardWatch(AMX *amx, const cell *params)
{
	if ( params[1] )
		lux::entities->_keyboard = Lux_PawnEntity_GetParent(amx);
	else
		lux::entities->_keyboard = NULL;
	return 0;
}



/** pawnInputButton
* native InputButton(input, player = 0);
* Returns the button value for Player
*/
static cell AMX_NATIVE_CALL pawnInputButton(AMX *amx, const cell *params)
{
	if ( params[2] > 0 )
		return (cell)lux::engine->GetPlayerButton((uint32_t)params[2], (uint8_t)params[1]);
	else if ( params[2] == 0 )
		return lux::engine->GetPlayerButton(lux::engine->default_player, (uint8_t)params[1]);
	return -1;
}

/** pawnInputAxis
* native InputAxis(input, player = 0);
* Returns Axis value for Player
*/
static cell AMX_NATIVE_CALL pawnInputAxis(AMX *amx, const cell *params)
{
	if ( params[2] > 0 )
		return (cell)lux::engine->GetPlayerAxis((uint32_t)params[2], (uint8_t)params[1]);
	else if ( params[2] == 0 )
		return lux::engine->GetPlayerAxis(lux::engine->default_player, (uint8_t)params[1]);
	return -1;
}

/** pawnInputPointer
* native InputPointer(input, player = 0);
* Returns Pointer value for Player
*/
static cell AMX_NATIVE_CALL pawnInputPointer(AMX *amx, const cell *params)
{

	if ( params[2] > 0 )
		return lux::engine->GetPlayerPointer((uint32_t)params[2], (uint8_t)params[1]);
	else if ( params[2] == 0 )
		return lux::engine->GetPlayerPointer(lux::engine->default_player, (uint8_t)params[1]);
	return -1;
}

/** pawnInputButtonSet
* native InputButtonSet(input, value, player = 0);
* Set Button value for the Player
*/
static cell AMX_NATIVE_CALL pawnInputButtonSet(AMX *amx, const cell *params)
{

	if ( params[3] > 0 )
		lux::engine->SetPlayerButton((uint32_t)params[3], (int16_t)params[2], (uint8_t)params[1]);
	else if ( params[3] == 0 )
		lux::engine->SetPlayerButton(lux::engine->default_player, (int16_t)params[2], (uint8_t)params[1]);
	return -1;
}

/** pawnInputAxisSet
* native InputAxisSet(input, value, player = 0);
* Set Axis value for the Player
*/
static cell AMX_NATIVE_CALL pawnInputAxisSet(AMX *amx, const cell *params)
{

	if ( params[3] > 0 )
		lux::engine->SetPlayerAxis((uint32_t)params[3], (int16_t)params[2], (uint8_t)params[1]);
	else if ( params[3] == 0 )
		lux::engine->SetPlayerAxis(lux::engine->default_player, (int16_t)params[2], (uint8_t)params[1]);
	return -1;
}

/** pawnInputPointerSet
* native InputPointerSet(input, value, player = 0);
* Set Pointer value for the Player
*/
static cell AMX_NATIVE_CALL pawnInputPointerSet(AMX *amx, const cell *params)
{

	if ( params[3] > 0 )
		lux::engine->SetPlayerPointer((uint32_t)params[3], (int16_t)params[2], (uint8_t)params[1]);
	else if ( params[3] == 0 )
		lux::engine->SetPlayerPointer(lux::engine->default_player, (int16_t)params[2], (uint8_t)params[1]);
	return -1;
}

/** pawnInputSetDefault
* native InputSetDefault(player);
* Set the default player
*/
static cell AMX_NATIVE_CALL pawnInputSetDefault(AMX *amx, const cell *params)
{
	lux::engine->SetDefaultPlayer( (uint32_t)params[1] );
	return lux::engine->default_player;
}

/** Language Functions */

/** pawnLanguageString
* native LanguageString(line, returnstring[], maxlength=sizeof returnstring);
*
*/
static cell AMX_NATIVE_CALL pawnLanguageString(AMX *amx, const cell *params)
{
	std::string dialog_string;
	cell * cptr;
	cptr = amx_Address(amx, params[2]);

	dialog_string = lux::engine->GetString(params[1]);
	return amx_SetString(cptr, dialog_string.c_str(), 1, 0, params[3]);
}

/** pawnLanguageSet
* native LanguageSet(lang[]);
*
*/
static cell AMX_NATIVE_CALL pawnLanguageSet(AMX *amx, const cell *params)
{
	char * lang_string;
	amx_StrParam_Type( amx, params[1], lang_string, char*);
	return (cell)lux::engine->LoadLanguage(lang_string);
	return 0;
}

/** Dialog Functions*/

/** pawnDialogShow
* native DialogShow(line, callback[] = "dialogbox");
* Calls 'callback'(line, string[]);
*/
static cell AMX_NATIVE_CALL pawnDialogShow(AMX *amx, const cell *params)
{
	char * function_name;
	Entity * wanted_entity = lux::entities->_global;
	if ( wanted_entity != NULL )
	{
		amx_StrParam_Type(amx, params[2], function_name, char*);
		wanted_entity->Call(function_name, (char*)"d", params[1]);
		/* Todo
		- Pass (parsed) Dialog string instead
		*/
		return 1;
	}
	return 0;
}



/** pawnDialogGetString
* native DialogGetString(line, returnstring[], maxlength=sizeof returnstring);
*
*/
static cell AMX_NATIVE_CALL pawnDialogGetString(AMX *amx, const cell *params)
{
	std::string dialog_string;
	cell * cptr;
	cptr = amx_Address(amx, params[2]);

	dialog_string = lux::engine->GetDialogString(params[1]);
	return Lux_PawnEntity_SetString(cptr, dialog_string.c_str(), params[3]);
}


const AMX_NATIVE_INFO Game_Natives[] = {
	/** Engine Functions */
	{ "GameReload", pawnGameReload },
	{ "GameState", pawnGameState },
	{ "GameFrame", pawnGameFrame },

	/** Save Functions */
	{ "GameLoad", pawnGameLoad },
	{ "GameSave", pawnGameSave },
	{ "GameSaveHibernate", pawnGameSaveHibernate },
	{ "GameSetDetails", pawnDeprecatedFunction },
	{ "GameGetDetails", pawnGameGetDetails },
	{ "GameDetails", pawnGameDetails },
	{ "GameRemoveSave", pawnGameRemoveSave },
	{ "GameHasSave", pawnGameHasSave },

	/** Input Functions */
	{ "KeyboardWatch", pawnKeyboardWatch },
	{ "InputButton",  pawnInputButton },
	{ "InputAxis",  pawnInputAxis },
	{ "InputPointer",  pawnInputPointer },
	{ "InputButtonSet",  pawnInputButtonSet },
	{ "InputAxisSet",  pawnInputAxisSet },
	{ "InputPointerSet",  pawnInputPointerSet },
	{ "InputSetDefault",  pawnInputSetDefault },

	/** Language Functions */
	{ "LanguageString", pawnLanguageString },
	{ "LanguageSet", pawnLanguageSet },

	/** Dialog Functions*/
	{ "DialogShow", pawnDialogShow},///native DialogShow(line, function[] = "dialogbox");
	{ "DialogGetString", pawnDialogGetString},///native DialogGetString(line, returnstring[], maxlength=sizeof returnstring);

	{ 0, 0 }
};
